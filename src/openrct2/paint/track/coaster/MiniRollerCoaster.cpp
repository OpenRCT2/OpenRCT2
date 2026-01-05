/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

static constexpr uint32_t MINI_RC_BLOCK_BRAKE_SW_NE_OPEN = 18742;
static constexpr uint32_t MINI_RC_BLOCK_BRAKE_NW_SE_OPEN = 18743;
static constexpr uint32_t MINI_RC_BLOCK_BRAKE_SW_NE_CLOSED = 18744;
static constexpr uint32_t MINI_RC_BLOCK_BRAKE_NW_SE_CLOSED = 18745;

static constexpr uint32_t kMiniRCBlockBrakeImages[kNumOrthogonalDirections][2] = {
    { MINI_RC_BLOCK_BRAKE_SW_NE_OPEN, MINI_RC_BLOCK_BRAKE_SW_NE_CLOSED },
    { MINI_RC_BLOCK_BRAKE_NW_SE_OPEN, MINI_RC_BLOCK_BRAKE_NW_SE_CLOSED },
    { MINI_RC_BLOCK_BRAKE_SW_NE_OPEN, MINI_RC_BLOCK_BRAKE_SW_NE_CLOSED },
    { MINI_RC_BLOCK_BRAKE_NW_SE_OPEN, MINI_RC_BLOCK_BRAKE_NW_SE_CLOSED },
};

/** rct2: 0x008A4ABC */
static void MiniRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19044), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19045), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19046), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19047), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
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
                    session, direction, session.TrackColours.WithIndex(18738), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18739), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void MiniRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex imageIds[4] = {
        18746,
        18747,
        18746,
        18747,
    };

    if (trackElement.GetTrackType() == TrackElemType::endStation)
    {
        bool isClosed = trackElement.IsBrakeClosed();
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(kMiniRCBlockBrakeImages[direction][isClosed]), { 0, 0, height },
            { { 0, 6, height + 3 }, { 32, 20, 1 } });
    }
    else
    {
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(imageIds[direction]), { 0, 0, height },
            { { 0, 6, height + 3 }, { 32, 20, 1 } });
    }
    if (TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, StationBaseType::a, 0, 9, 11))
    {
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::tubes);
    }
    else if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
    }
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A4ACC */
static void MiniRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19056), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19057), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19058), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19059), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18796), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18797), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18798), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18799), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A4ADC */
static void MiniRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18812), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18813), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 98 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18814), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 98 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18815), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 38, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x008A4AEC */
static void MiniRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19048), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19049), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19050), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19051), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18788), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18789), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18790), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18791), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 9, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008A4AFC */
static void MiniRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18800), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18801), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18804), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 66 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18802), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18805), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 66 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18803), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 18, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008A4B0C */
static void MiniRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18806), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18807), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18810), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 66 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18808), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18811), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 66 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18809), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 26, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008A4B1C */
static void MiniRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19052), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19053), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19054), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(19055), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18792), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18793), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18794), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(18795), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 3 } });
                break;
        }
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 12, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008A4B2C */
static void MiniRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4B3C */
static void MiniRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4B4C */
static void MiniRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4B5C */
static void MiniRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4B6C */
static void MiniRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4B7C */
static void MiniRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4B8C */
static void MiniRCTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18851), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18856), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18861), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18846), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
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
                        session, direction, session.TrackColours.WithIndex(18850), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18855), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18860), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18845), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(18849), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18854), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18859), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18844), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
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
                        session, direction, session.TrackColours.WithIndex(18848), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18853), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18858), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18843), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(18847), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18852), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18857), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18842), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), 6, height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4B9C */
static void MiniRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MiniRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4BAC */
static void MiniRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18748), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18756), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18749), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18757), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18750), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18751), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A4BBC */
static void MiniRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18752), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18753), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18754), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18758), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18755), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18759), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A4BCC */
static void MiniRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18754), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18758), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18755), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18759), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18752), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18753), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A4BDC */
static void MiniRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18750), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18751), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18748), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18756), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18749), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18757), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A4BEC */
static void MiniRCTrackBankedLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18871), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18882), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18876), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18881), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18866), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
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
                        session, direction, session.TrackColours.WithIndex(18870), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18875), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18880), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18865), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(18869), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18874), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18879), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18864), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
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
                        session, direction, session.TrackColours.WithIndex(18868), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18873), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18878), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18863), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(18867), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18872), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18877), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18883), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18862), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), 6, height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4BFC */
static void MiniRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MiniRCTrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4C0C */
static void MiniRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18760), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18764), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18761), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18765), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18762), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18763), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 9, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008A4C1C */
static void MiniRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18766), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18767), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18768), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18770), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18769), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18771), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 9, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008A4C2C */
static void MiniRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18772), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18776), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18773), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18777), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18774), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18775), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 12, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008A4C3C */
static void MiniRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18778), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18779), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18780), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18782), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18781), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18783), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 12, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008A4C4C */
static void MiniRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4C5C */
static void MiniRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4C6C */
static void MiniRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4C7C */
static void MiniRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4C8C */
static void MiniRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18784), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18785), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18786), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18787), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A4C9C */
static void MiniRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4CAC */
static void MiniRCTrackLeftQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18936), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18941), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18946), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18951), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18937), { 0, 0, height }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18942), { 0, 0, height }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18947), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18952), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
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
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18938), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18943), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18948), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18953), { 0, 0, height }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18939), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18944), { 0, 0, height }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18949), { 0, 0, height }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18954), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
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
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18940), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18945), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18950), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18955), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), 14, height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A4CBC */
static void MiniRCTrackRightQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18916), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18921), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18926), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18931), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18917), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18922), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18927), { 0, 0, height }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18932), { 0, 0, height }, { 32, 16, 3 });
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
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18918), { 0, 0, height }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18923), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18928), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18933), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18919), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18924), { 0, 0, height }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18929), { 0, 0, height }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18934), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
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
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18920), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18925), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18930), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18935), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionNext(direction), 14, height,
                session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A4CCC */
static void MiniRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MiniRCTrackRightQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4CDC */
static void MiniRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MiniRCTrackLeftQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4CEC */
static void MiniRCTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18900), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18904), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18903), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18907), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            DrawSBendLeftSupports(session, supportType.metal, trackSequence, direction, height, 6, 6);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18901), { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18905), { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18902), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18906), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
            }
            DrawSBendLeftSupports(session, supportType.metal, trackSequence, direction, height, 3, 6);
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
                        session, direction, session.TrackColours.WithIndex(18902), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18906), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18901), { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18905), { 0, 0, height }, { 32, 26, 3 });
                    break;
            }
            DrawSBendLeftSupports(session, supportType.metal, trackSequence, direction, height, 3, 6);
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
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18903), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18907), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18900), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18904), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
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
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4CFC */
static void MiniRCTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18908), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18912), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18911), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18915), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height, 6, 6);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18909), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18913), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18910), { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18914), { 0, 0, height }, { 32, 26, 3 });
                    break;
            }
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height, 6, 6);
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
                        session, direction, session.TrackColours.WithIndex(18910), { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18914), { 0, 0, height }, { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18909), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18913), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 3 } });
                    break;
            }
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height, 6, 6);
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
                        session, direction, session.TrackColours.WithIndex(18911), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18915), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18908), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18912), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height, 6, 6);
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
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4D3C */
static void MiniRCTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18821), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18824), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18827), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18818), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
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
                        session, direction, session.TrackColours.WithIndex(18820), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18823), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18826), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18817), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(18819), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18822), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18825), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18816), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), 6, height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4D4C */
static void MiniRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MiniRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4D5C */
static void MiniRCTrackLeftQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18833), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18840), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18836), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18839), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18830), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
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
                        session, direction, session.TrackColours.WithIndex(18832), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18835), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18838), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18829), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(18831), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18834), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18837), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18841), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18828), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), 6, height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4D6C */
static void MiniRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MiniRCTrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4D7C */
static void MiniRCTrackLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18967), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18969), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18971), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18965), { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18966), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18968), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18970), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18964), { 6, 0, height }, { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), 14, height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A4D8C */
static void MiniRCTrackRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18956), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18958), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18960), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18962), { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18957), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18959), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18961), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18963), { 6, 0, height }, { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionNext(direction), 14, height,
                session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A4D9C */
static void MiniRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MiniRCTrackRightQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4DAC */
static void MiniRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MiniRCTrackLeftQuarterTurn325DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4DBC */
static void MiniRCTrackLeftHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18991), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18998), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18994), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18997), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18988), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 8, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
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
                        session, direction, session.TrackColours.WithIndex(18990), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18993), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18996), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18987), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(18989), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18992), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18995), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18999), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18986), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), (direction == 2) ? 15 : 12,
                height, session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18988), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18991), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18998), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18994), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18997), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), 8, height,
                session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18987), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18990), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18993), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18996), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
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
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18986), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18989), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18992), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18995), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18999), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionReverse(direction), (direction == 3) ? 15 : 12,
                height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4DCC */
static void MiniRCTrackRightHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18972), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18975), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18978), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18981), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18985), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 8, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
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
                        session, direction, session.TrackColours.WithIndex(18973), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18976), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18979), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18982), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
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
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18974), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18977), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18984), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18980), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18983), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            constexpr uint8_t _specialValues[] = { 12, 15, 14, 12 };
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionNext(direction), _specialValues[direction],
                height, session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        }
        case 4:
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18975), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18978), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18981), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18985), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18972), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            constexpr uint8_t _specialValues[] = { 8, 9, 11, 8 };
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionNext(direction), _specialValues[direction],
                height, session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        }
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18976), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18979), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18982), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18973), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
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
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18977), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18984), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18980), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18983), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(18974), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 3 } });
                    break;
            }
            constexpr uint8_t _specialValues[] = { 15, 14, 12, 12 };
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionReverse(direction), _specialValues[direction],
                height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        }
    }
}

/** rct2: 0x008A4DDC */
static void MiniRCTrackLeftHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MiniRCTrackRightHalfBankedHelixUpSmall(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4DEC */
static void MiniRCTrackRightHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MiniRCTrackLeftHalfBankedHelixUpSmall(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4DFC */
static void MiniRCTrackLeftHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19031), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19042), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19036), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19041), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19026), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 7, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
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
                        session, direction, session.TrackColours.WithIndex(19030), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19035), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19040), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19025), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19029), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19034), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19039), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19024), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
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
                        session, direction, session.TrackColours.WithIndex(19028), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19033), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19038), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19023), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19027), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19032), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19037), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19043), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19022), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), 13, height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19026), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19031), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19042), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19036), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19041), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), 7, height,
                session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 8:
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19025), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19030), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19035), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19040), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
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
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19024), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19029), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19034), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19039), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
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
        case 11:
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19023), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19028), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19033), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19038), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
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
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19022), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19027), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19032), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19037), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19043), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionReverse(direction), 13, height,
                session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4E0C */
static void MiniRCTrackRightHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19000), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19005), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19010), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19015), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19021), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 7, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
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
                        session, direction, session.TrackColours.WithIndex(19001), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19006), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19011), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19016), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
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
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19002), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19007), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19012), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19017), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
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
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19003), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19008), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19013), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19018), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19004), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19009), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19020), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19014), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19019), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionNext(direction), 13, height,
                session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19005), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19010), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19015), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19021), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19000), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionNext(direction), 7, height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 8:
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19006), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19011), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19016), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19001), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
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
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19007), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19012), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19017), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19002), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 11:
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19008), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19013), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19018), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19003), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
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
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19009), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19020), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19014), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19019), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19004), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionReverse(direction), 13, height,
                session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4E1C */
static void MiniRCTrackLeftHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MiniRCTrackRightHalfBankedHelixUpLarge(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4E2C */
static void MiniRCTrackRightHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MiniRCTrackLeftHalfBankedHelixUpLarge(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4E5C */
static void MiniRCTrackLeftQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18893), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18897), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18894), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18898), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18895), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18899), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18892), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18896), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
    }
    TrackPaintUtilLeftQuarterTurn1TileTunnel(
        session, kTunnelGroup, direction, height, -8, TunnelSubType::SlopeStart, +56, TunnelSubType::SlopeEnd);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x008A4E3C */
static void MiniRCTrackRightQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18884), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18888), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18885), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18889), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18886), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18890), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18887), { 0, 0, height },
                { { 2, 2, height }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18891), { 0, 0, height },
                { { 2, 2, height + 99 }, { 28, 28, 1 } });
            break;
    }
    TrackPaintUtilRightQuarterTurn1TileTunnel(
        session, kTunnelGroup, direction, height, -8, TunnelSubType::SlopeStart, +56, TunnelSubType::SlopeEnd);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x008A4E4C */
static void MiniRCTrackLeftQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackRightQuarterTurn160DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4E6C */
static void MiniRCTrackRightQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackLeftQuarterTurn160DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4E7C */
static void MiniRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18740), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18741), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A50CC */
static void MiniRCTrack25DegUpLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19222), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19223), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19224), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19225), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A50DC */
static void MiniRCTrack25DegUpRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19226), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19227), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19228), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19229), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A4E8C */
static void MiniRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilOnridePhotoPlatformPaintBase(session, height);
    DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal, 6);

    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18738), { 0, 0, height },
                { { 0, 6, height + 3 }, { 32, 20, 0 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18739), { 0, 0, height },
                { { 0, 6, height + 3 }, { 32, 20, 0 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18738), { 0, 0, height },
                { { 0, 6, height + 3 }, { 32, 20, 0 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(18739), { 0, 0, height },
                { { 0, 6, height + 3 }, { 32, 20, 0 } });
            break;
    }
    TrackPaintUtilOnridePhotoPaint2(session, direction, trackElement, height);
}

/** rct2: 0x008A50EC */
static void MiniRCTrack25DegDownLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrack25DegUpRightBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A50FC */
static void MiniRCTrack25DegDownRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrack25DegUpLeftBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4EAC */
static void MiniRCTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19076), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19080), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19084), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19088), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
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
                        session, direction, session.TrackColours.WithIndex(19077), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19081), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19085), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19089), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19078), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19082), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19086), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19090), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19079), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19083), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19087), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19091), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4EBC */
static void MiniRCTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19060), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19064), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19068), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19072), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
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
                        session, direction, session.TrackColours.WithIndex(19061), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19065), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19069), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19073), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19062), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19066), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19070), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19074), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19063), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19067), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19071), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19075), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4ECC */
static void MiniRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MiniRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4EDC */
static void MiniRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MiniRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4EEC */
static void MiniRCTrackLeftEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19108), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19112), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19116), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19120), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
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
                        session, direction, session.TrackColours.WithIndex(19109), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19113), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 34, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19117), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19121), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19110), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19114), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19118), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19122), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19111), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19115), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 18, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19119), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19123), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4EFC */
static void MiniRCTrackRightEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19092), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19096), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19100), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19104), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
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
                        session, direction, session.TrackColours.WithIndex(19093), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19097), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19101), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 34, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19105), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19094), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19098), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19102), { 0, 0, height },
                        { { 4, 4, height + 27 }, { 28, 28, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19106), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19095), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19099), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19103), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 18, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19107), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4F0C */
static void MiniRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MiniRCTrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4F1C */
static void MiniRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MiniRCTrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A4E9C */
static void MiniRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    constexpr ImageIndex images[2][kNumOrthogonalDirections] = {
        { 19124, 19125, 19126, 19127 },
        { 19186, 19187, 19188, 19189 },
    };

    TrackPaintUtilDiagTilesPaint(
        session, 3, height, direction, trackSequence, images[trackElement.HasChain()], defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    if (trackSequence == 3)
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(BlockedSegments::kDiagStraightFlat[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A4F4C */
static void MiniRCTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(19201), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19139), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
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
                            session, direction, session.TrackColours.WithIndex(19198), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19136), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(19200), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19138), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(19199), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(19137), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 14, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x008A4FAC */
static void MiniRCTrackDiag60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19151), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19148), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19150), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19149), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, (direction & 1) ? 42 : 38, height,
                session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
    }
}

/** rct2: 0x008A4F2C */
static void MiniRCTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(19193), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19131), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
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
                            session, direction, session.TrackColours.WithIndex(19190), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19128), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(19192), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19130), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(19191), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(19129), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008A4F8C */
static void MiniRCTrackDiag25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19143), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19140), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19142), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19141), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 22, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A4F9C */
static void MiniRCTrackDiag60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19147), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19144), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19146), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19145), { -16, -16, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 27, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A4F3C */
static void MiniRCTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(19197), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19135), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
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
                            session, direction, session.TrackColours.WithIndex(19194), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19132), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(19196), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19134), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(19195), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(19133), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 10, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x008A4F7C */
static void MiniRCTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(19199), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19137), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
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
                            session, direction, session.TrackColours.WithIndex(19200), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19138), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(19198), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19136), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(19201), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(19139), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 14, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x008A4FDC */
static void MiniRCTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19149), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19150), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19148), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19151), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, (direction & 1) ? 34 : 30, height,
                session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
    }
}

/** rct2: 0x008A4F5C */
static void MiniRCTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(19195), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19133), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
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
                            session, direction, session.TrackColours.WithIndex(19196), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19134), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
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
                            session, direction, session.TrackColours.WithIndex(19194), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19132), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(19197), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(19135), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 10, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A4FBC */
static void MiniRCTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19145), { -16, -16, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19146), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19144), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19147), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 23, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A4FCC */
static void MiniRCTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19141), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19142), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19140), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19143), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 14, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A4F6C */
static void MiniRCTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(19191), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19129), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
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
                            session, direction, session.TrackColours.WithIndex(19192), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19130), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(19190), { -16, -16, height },
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
                            session, direction, session.TrackColours.WithIndex(19128), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(19193), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(19131), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 3 } });
                        break;
                }
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008A500C */
static void MiniRCTrackDiagFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19159), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19156), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19160), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19158), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19157), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A501C */
static void MiniRCTrackDiagFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19164), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19161), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19163), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19165), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19162), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A502C */
static void MiniRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19162), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19163), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19165), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19161), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19164), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A503C */
static void MiniRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19157), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19158), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19156), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19160), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19159), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A506C */
static void MiniRCTrackDiagLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19179), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19176), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19180), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19178), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19177), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008A507C */
static void MiniRCTrackDiagRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19184), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19181), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19183), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19185), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19182), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008A504C */
static void MiniRCTrackDiag25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19169), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19166), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19170), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19168), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19167), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 10, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x008A505C */
static void MiniRCTrackDiag25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19174), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19171), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19173), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19175), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19172), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 10, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x008A508C */
static void MiniRCTrackDiagLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19172), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19173), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19175), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19171), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19174), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 10, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A509C */
static void MiniRCTrackDiagRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19167), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19168), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19166), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19170), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19169), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 10, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A50AC */
static void MiniRCTrackDiag25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19182), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19183), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19185), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19181), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19184), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008A50BC */
static void MiniRCTrackDiag25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19177), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19178), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19176), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19180), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19179), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008A4FEC */
static void MiniRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19155), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19152), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19154), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19153), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A4FFC */
static void MiniRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19153), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19154), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(19152), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
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
                        session, direction, session.TrackColours.WithIndex(19155), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 6, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008A530C */
static void MiniRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isClosed = trackElement.IsBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(kMiniRCBlockBrakeImages[direction][isClosed]), { 0, 0, height },
        { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008A510C */
static void MiniRCTrackLeftBankedQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19317), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19319), { 0, 6, height },
                        { { 0, 27, height }, { 32, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19321), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19315), { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19316), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19318), { 6, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19320), { 6, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19314), { 6, 0, height }, { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), 14, height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A511C */
static void MiniRCTrackRightBankedQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19306), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19308), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19310), { 0, 6, height },
                        { { 0, 27, height }, { 32, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19312), { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19307), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19309), { 6, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19311), { 6, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19313), { 6, 0, height }, { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionNext(direction), 14, height,
                session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A512C */
static void MiniRCTrackLeftBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MiniRCTrackRightBankedQuarterTurn325DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A513C */
static void MiniRCTrackRightBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MiniRCTrackLeftBankedQuarterTurn325DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A514C */
static void MiniRCTrackLeftBankedQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19286), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19291), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19296), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19301), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19287), { 0, 0, height }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19292), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19297), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19302), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
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
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19288), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19293), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19298), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19303), { 0, 0, height }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19289), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19294), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19299), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19304), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
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
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19290), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19295), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19300), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19305), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionNext(direction), 14, height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A515C */
static void MiniRCTrackRightBankedQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19266), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19271), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19276), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19281), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19267), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19272), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19277), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19282), { 0, 0, height }, { 32, 16, 3 });
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
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19268), { 0, 0, height }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19273), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19278), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19283), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19269), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19274), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19279), { 0, 0, height },
                        { { 30, 30, height }, { 1, 1, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19284), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 3 } });
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
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19270), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19275), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19280), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 34 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19285), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionNext(direction), 14, height,
                session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008A516C */
static void MiniRCTrackLeftBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MiniRCTrackRightBankedQuarterTurn525DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A517C */
static void MiniRCTrackRightBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MiniRCTrackLeftBankedQuarterTurn525DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A518C */
static void MiniRCTrack25DegUpToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19230), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19231), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19238), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19232), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19233), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A519C */
static void MiniRCTrack25DegUpToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19234), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19235), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19236), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19239), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19237), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A51AC */
static void MiniRCTrackLeftBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19240), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19241), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19248), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19242), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19243), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A51BC */
static void MiniRCTrackRightBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19244), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19245), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19246), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19249), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19247), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008A51CC */
static void MiniRCTrack25DegDownToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackRightBanked25DegUpTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A51DC */
static void MiniRCTrack25DegDownToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackLeftBanked25DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A51EC */
static void MiniRCTrackLeftBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrack25DegUpToRightBanked25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A51FC */
static void MiniRCTrackRightBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrack25DegUpToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A520C */
static void MiniRCTrackLeftBankedFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19250), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19251), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19252), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19253), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 9, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008A521C */
static void MiniRCTrackRightBankedFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19254), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19255), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19256), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19257), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 9, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008A524C */
static void MiniRCTrackLeftBanked25DegUpToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19258), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19259), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19260), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19261), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 12, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008A525C */
static void MiniRCTrackRightBanked25DegUpToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19262), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19263), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19264), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19265), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 12, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008A526C */
static void MiniRCTrackLeftBankedFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackRightBanked25DegUpToRightBankedFlat(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A527C */
static void MiniRCTrackRightBankedFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackLeftBanked25DegUpToLeftBankedFlat(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A522C */
static void MiniRCTrackLeftBanked25DegDownToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackRightBankedFlatToRightBanked25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A523C */
static void MiniRCTrackRightBanked25DegDownToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackLeftBankedFlatToLeftBanked25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A528C */
static void MiniRCTrackFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19202), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19203), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19210), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19204), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19205), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 9, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008A529C */
static void MiniRCTrackFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19206), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19207), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19208), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19211), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19209), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 9, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008A52AC */
static void MiniRCTrackLeftBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19212), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19213), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19220), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19214), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19215), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 12, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008A52BC */
static void MiniRCTrackRightBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19216), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19217), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19218), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19221), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19219), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 12, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008A52CC */
static void MiniRCTrackFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackRightBanked25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A52DC */
static void MiniRCTrackFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackLeftBanked25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A52EC */
static void MiniRCTrackLeftBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackFlatToRightBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008A52FC */
static void MiniRCTrackRightBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniRCTrackFlatToLeftBanked25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void MiniRCTrackLeftCurvedLiftHill(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19333), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19335), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19337), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19331), { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19332), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19334), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19336), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19330), { 6, 0, height }, { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionPrev(direction), 17, height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 16, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 16, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void MiniRCTrackRightCurvedLiftHill(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19322), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19324), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19326), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19328), { 0, 6, height }, { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 14, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19323), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19325), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19327), { 6, 0, height }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19329), { 6, 0, height }, { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, DirectionNext(direction), 17, height,
                session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 16, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 16, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void MiniRCTrackBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_MINI_RC_BOOSTER_NE_SW), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_TRACKS_MINI_RC_BOOSTER_NW_SE), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

TrackPaintFunction GetTrackPaintFunctionMiniRC(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::flat:
            return MiniRCTrackFlat;
        case TrackElemType::endStation:
        case TrackElemType::beginStation:
        case TrackElemType::middleStation:
            return MiniRCTrackStation;
        case TrackElemType::up25:
            return MiniRCTrack25DegUp;
        case TrackElemType::up60:
            return MiniRCTrack60DegUp;
        case TrackElemType::flatToUp25:
            return MiniRCTrackFlatTo25DegUp;
        case TrackElemType::up25ToUp60:
            return MiniRCTrack25DegUpTo60DegUp;
        case TrackElemType::up60ToUp25:
            return MiniRCTrack60DegUpTo25DegUp;
        case TrackElemType::up25ToFlat:
            return MiniRCTrack25DegUpToFlat;
        case TrackElemType::down25:
            return MiniRCTrack25DegDown;
        case TrackElemType::down60:
            return MiniRCTrack60DegDown;
        case TrackElemType::flatToDown25:
            return MiniRCTrackFlatTo25DegDown;
        case TrackElemType::down25ToDown60:
            return MiniRCTrack25DegDownTo60DegDown;
        case TrackElemType::down60ToDown25:
            return MiniRCTrack60DegDownTo25DegDown;
        case TrackElemType::down25ToFlat:
            return MiniRCTrack25DegDownToFlat;
        case TrackElemType::leftQuarterTurn5Tiles:
            return MiniRCTrackLeftQuarterTurn5;
        case TrackElemType::rightQuarterTurn5Tiles:
            return MiniRCTrackRightQuarterTurn5;
        case TrackElemType::flatToLeftBank:
            return MiniRCTrackFlatToLeftBank;
        case TrackElemType::flatToRightBank:
            return MiniRCTrackFlatToRightBank;
        case TrackElemType::leftBankToFlat:
            return MiniRCTrackLeftBankToFlat;
        case TrackElemType::rightBankToFlat:
            return MiniRCTrackRightBankToFlat;
        case TrackElemType::bankedLeftQuarterTurn5Tiles:
            return MiniRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::bankedRightQuarterTurn5Tiles:
            return MiniRCTrackBankedRightQuarterTurn5;
        case TrackElemType::leftBankToUp25:
            return MiniRCTrackLeftBankTo25DegUp;
        case TrackElemType::rightBankToUp25:
            return MiniRCTrackRightBankTo25DegUp;
        case TrackElemType::up25ToLeftBank:
            return MiniRCTrack25DegUpToLeftBank;
        case TrackElemType::up25ToRightBank:
            return MiniRCTrack25DegUpToRightBank;
        case TrackElemType::leftBankToDown25:
            return MiniRCTrackLeftBankTo25DegDown;
        case TrackElemType::rightBankToDown25:
            return MiniRCTrackRightBankTo25DegDown;
        case TrackElemType::down25ToLeftBank:
            return MiniRCTrack25DegDownToLeftBank;
        case TrackElemType::down25ToRightBank:
            return MiniRCTrack25DegDownToRightBank;
        case TrackElemType::leftBank:
            return MiniRCTrackLeftBank;
        case TrackElemType::rightBank:
            return MiniRCTrackRightBank;
        case TrackElemType::leftQuarterTurn5TilesUp25:
            return MiniRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::rightQuarterTurn5TilesUp25:
            return MiniRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::leftQuarterTurn5TilesDown25:
            return MiniRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::rightQuarterTurn5TilesDown25:
            return MiniRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::sBendLeft:
            return MiniRCTrackSBendLeft;
        case TrackElemType::sBendRight:
            return MiniRCTrackSBendRight;
        case TrackElemType::leftQuarterTurn3Tiles:
            return MiniRCTrackLeftQuarterTurn3;
        case TrackElemType::rightQuarterTurn3Tiles:
            return MiniRCTrackRightQuarterTurn3;
        case TrackElemType::leftBankedQuarterTurn3Tiles:
            return MiniRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::rightBankedQuarterTurn3Tiles:
            return MiniRCTrackRightQuarterTurn3Bank;
        case TrackElemType::leftQuarterTurn3TilesUp25:
            return MiniRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::rightQuarterTurn3TilesUp25:
            return MiniRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::leftQuarterTurn3TilesDown25:
            return MiniRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::rightQuarterTurn3TilesDown25:
            return MiniRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::leftHalfBankedHelixUpSmall:
            return MiniRCTrackLeftHalfBankedHelixUpSmall;
        case TrackElemType::rightHalfBankedHelixUpSmall:
            return MiniRCTrackRightHalfBankedHelixUpSmall;
        case TrackElemType::leftHalfBankedHelixDownSmall:
            return MiniRCTrackLeftHalfBankedHelixDownSmall;
        case TrackElemType::rightHalfBankedHelixDownSmall:
            return MiniRCTrackRightHalfBankedHelixDownSmall;
        case TrackElemType::leftHalfBankedHelixUpLarge:
            return MiniRCTrackLeftHalfBankedHelixUpLarge;
        case TrackElemType::rightHalfBankedHelixUpLarge:
            return MiniRCTrackRightHalfBankedHelixUpLarge;
        case TrackElemType::leftHalfBankedHelixDownLarge:
            return MiniRCTrackLeftHalfBankedHelixDownLarge;
        case TrackElemType::rightHalfBankedHelixDownLarge:
            return MiniRCTrackRightHalfBankedHelixDownLarge;
        case TrackElemType::leftQuarterTurn1TileUp60:
            return MiniRCTrackLeftQuarterTurn160DegUp;
        case TrackElemType::rightQuarterTurn1TileUp60:
            return MiniRCTrackRightQuarterTurn160DegUp;
        case TrackElemType::leftQuarterTurn1TileDown60:
            return MiniRCTrackLeftQuarterTurn160DegDown;
        case TrackElemType::rightQuarterTurn1TileDown60:
            return MiniRCTrackRightQuarterTurn160DegDown;
        case TrackElemType::brakes:
            return MiniRCTrackBrakes;
        case TrackElemType::up25LeftBanked:
            return MiniRCTrack25DegUpLeftBanked;
        case TrackElemType::up25RightBanked:
            return MiniRCTrack25DegUpRightBanked;
        case TrackElemType::onRidePhoto:
            return MiniRCTrackOnRidePhoto;
        case TrackElemType::down25LeftBanked:
            return MiniRCTrack25DegDownLeftBanked;
        case TrackElemType::down25RightBanked:
            return MiniRCTrack25DegDownRightBanked;
        case TrackElemType::leftEighthToDiag:
            return MiniRCTrackLeftEighthToDiag;
        case TrackElemType::rightEighthToDiag:
            return MiniRCTrackRightEighthToDiag;
        case TrackElemType::leftEighthToOrthogonal:
            return MiniRCTrackLeftEighthToOrthogonal;
        case TrackElemType::rightEighthToOrthogonal:
            return MiniRCTrackRightEighthToOrthogonal;
        case TrackElemType::leftEighthBankToDiag:
            return MiniRCTrackLeftEighthBankToDiag;
        case TrackElemType::rightEighthBankToDiag:
            return MiniRCTrackRightEighthBankToDiag;
        case TrackElemType::leftEighthBankToOrthogonal:
            return MiniRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::rightEighthBankToOrthogonal:
            return MiniRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::diagFlat:
            return MiniRCTrackDiagFlat;
        case TrackElemType::diagUp25:
            return MiniRCTrackDiag25DegUp;
        case TrackElemType::diagUp60:
            return MiniRCTrackDiag60DegUp;
        case TrackElemType::diagFlatToUp25:
            return MiniRCTrackDiagFlatTo25DegUp;
        case TrackElemType::diagUp25ToUp60:
            return MiniRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::diagUp60ToUp25:
            return MiniRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::diagUp25ToFlat:
            return MiniRCTrackDiag25DegUpToFlat;
        case TrackElemType::diagDown25:
            return MiniRCTrackDiag25DegDown;
        case TrackElemType::diagDown60:
            return MiniRCTrackDiag60DegDown;
        case TrackElemType::diagFlatToDown25:
            return MiniRCTrackDiagFlatTo25DegDown;
        case TrackElemType::diagDown25ToDown60:
            return MiniRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::diagDown60ToDown25:
            return MiniRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::diagDown25ToFlat:
            return MiniRCTrackDiag25DegDownToFlat;
        case TrackElemType::diagFlatToLeftBank:
            return MiniRCTrackDiagFlatToLeftBank;
        case TrackElemType::diagFlatToRightBank:
            return MiniRCTrackDiagFlatToRightBank;
        case TrackElemType::diagLeftBankToFlat:
            return MiniRCTrackDiagLeftBankToFlat;
        case TrackElemType::diagRightBankToFlat:
            return MiniRCTrackDiagRightBankToFlat;
        case TrackElemType::diagLeftBankToUp25:
            return MiniRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::diagRightBankToUp25:
            return MiniRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::diagUp25ToLeftBank:
            return MiniRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::diagUp25ToRightBank:
            return MiniRCTrackDiag25DegUpToRightBank;
        case TrackElemType::diagLeftBankToDown25:
            return MiniRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::diagRightBankToDown25:
            return MiniRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::diagDown25ToLeftBank:
            return MiniRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::diagDown25ToRightBank:
            return MiniRCTrackDiag25DegDownToRightBank;
        case TrackElemType::diagLeftBank:
            return MiniRCTrackDiagLeftBank;
        case TrackElemType::diagRightBank:
            return MiniRCTrackDiagRightBank;
        case TrackElemType::blockBrakes:
            return MiniRCTrackBlockBrakes;
        case TrackElemType::leftBankedQuarterTurn3TileUp25:
            return MiniRCTrackLeftBankedQuarterTurn325DegUp;
        case TrackElemType::rightBankedQuarterTurn3TileUp25:
            return MiniRCTrackRightBankedQuarterTurn325DegUp;
        case TrackElemType::leftBankedQuarterTurn3TileDown25:
            return MiniRCTrackLeftBankedQuarterTurn325DegDown;
        case TrackElemType::rightBankedQuarterTurn3TileDown25:
            return MiniRCTrackRightBankedQuarterTurn325DegDown;
        case TrackElemType::leftBankedQuarterTurn5TileUp25:
            return MiniRCTrackLeftBankedQuarterTurn525DegUp;
        case TrackElemType::rightBankedQuarterTurn5TileUp25:
            return MiniRCTrackRightBankedQuarterTurn525DegUp;
        case TrackElemType::leftBankedQuarterTurn5TileDown25:
            return MiniRCTrackLeftBankedQuarterTurn525DegDown;
        case TrackElemType::rightBankedQuarterTurn5TileDown25:
            return MiniRCTrackRightBankedQuarterTurn525DegDown;
        case TrackElemType::up25ToLeftBankedUp25:
            return MiniRCTrack25DegUpToLeftBanked25DegUp;
        case TrackElemType::up25ToRightBankedUp25:
            return MiniRCTrack25DegUpToRightBanked25DegUp;
        case TrackElemType::leftBankedUp25ToUp25:
            return MiniRCTrackLeftBanked25DegUpTo25DegUp;
        case TrackElemType::rightBankedUp25ToUp25:
            return MiniRCTrackRightBanked25DegUpTo25DegUp;
        case TrackElemType::down25ToLeftBankedDown25:
            return MiniRCTrack25DegDownToLeftBanked25DegDown;
        case TrackElemType::down25ToRightBankedDown25:
            return MiniRCTrack25DegDownToRightBanked25DegDown;
        case TrackElemType::leftBankedDown25ToDown25:
            return MiniRCTrackLeftBanked25DegDownTo25DegDown;
        case TrackElemType::rightBankedDown25ToDown25:
            return MiniRCTrackRightBanked25DegDownTo25DegDown;
        case TrackElemType::leftBankedFlatToLeftBankedUp25:
            return MiniRCTrackLeftBankedFlatToLeftBanked25DegUp;
        case TrackElemType::rightBankedFlatToRightBankedUp25:
            return MiniRCTrackRightBankedFlatToRightBanked25DegUp;
        case TrackElemType::leftBankedUp25ToLeftBankedFlat:
            return MiniRCTrackLeftBanked25DegUpToLeftBankedFlat;
        case TrackElemType::rightBankedUp25ToRightBankedFlat:
            return MiniRCTrackRightBanked25DegUpToRightBankedFlat;
        case TrackElemType::leftBankedFlatToLeftBankedDown25:
            return MiniRCTrackLeftBankedFlatToLeftBanked25DegDown;
        case TrackElemType::rightBankedFlatToRightBankedDown25:
            return MiniRCTrackRightBankedFlatToRightBanked25DegDown;
        case TrackElemType::leftBankedDown25ToLeftBankedFlat:
            return MiniRCTrackLeftBanked25DegDownToLeftBankedFlat;
        case TrackElemType::rightBankedDown25ToRightBankedFlat:
            return MiniRCTrackRightBanked25DegDownToRightBankedFlat;
        case TrackElemType::flatToLeftBankedUp25:
            return MiniRCTrackFlatToLeftBanked25DegUp;
        case TrackElemType::flatToRightBankedUp25:
            return MiniRCTrackFlatToRightBanked25DegUp;
        case TrackElemType::leftBankedUp25ToFlat:
            return MiniRCTrackLeftBanked25DegUpToFlat;
        case TrackElemType::rightBankedUp25ToFlat:
            return MiniRCTrackRightBanked25DegUpToFlat;
        case TrackElemType::flatToLeftBankedDown25:
            return MiniRCTrackFlatToLeftBanked25DegDown;
        case TrackElemType::flatToRightBankedDown25:
            return MiniRCTrackFlatToRightBanked25DegDown;
        case TrackElemType::leftBankedDown25ToFlat:
            return MiniRCTrackLeftBanked25DegDownToFlat;
        case TrackElemType::rightBankedDown25ToFlat:
            return MiniRCTrackRightBanked25DegDownToFlat;
        case TrackElemType::leftCurvedLiftHill:
            return MiniRCTrackLeftCurvedLiftHill;
        case TrackElemType::rightCurvedLiftHill:
            return MiniRCTrackRightCurvedLiftHill;
        case TrackElemType::booster:
            return MiniRCTrackBooster;
        default:
            return TrackPaintFunctionDummy;
    }
}
