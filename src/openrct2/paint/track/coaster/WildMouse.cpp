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
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

/** rct2: 0x0078B1E4 */
static void WildMouseTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -1, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void WildMouseTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        trackPaintSpriteBrake(session, ride, trackSequence, direction, height, trackElement, supportType);
    }
    else
    {
        trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    }
    if (TrackPaintUtilDrawStation(session, ride, direction, height, trackElement, StationBaseType::b, -2))
    {
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Boxed);
    }
    else if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, -1, height, session.SupportColours);
    }
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0078B1F4 */
static void WildMouseTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -9, height, session.SupportColours);
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

/** rct2: 0x0078B204 */
static void WildMouseTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        if (direction == 0 || direction == 3)
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::Centre, -33, height, session.SupportColours);
        }
        else
        {
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 32, height, session.SupportColours);
        }
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

/** rct2: 0x0078B214 */
static void WildMouseTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -4, height, session.SupportColours);
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

/** rct2: 0x0078B224 */
static void WildMouseTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2Chain(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -13, height, session.SupportColours);
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

/** rct2: 0x0078B234 */
static void WildMouseTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2Chain(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -21, height, session.SupportColours);
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

/** rct2: 0x0078B244 */
static void WildMouseTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -7, height, session.SupportColours);
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

/** rct2: 0x0078B254 */
static void WildMouseTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B264 */
static void WildMouseTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B274 */
static void WildMouseTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B284 */
static void WildMouseTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B294 */
static void WildMouseTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B2A4 */
static void WildMouseTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void WildMouseTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
        case 3:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -1, height, session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void WildMouseTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr int16_t generalSupportHeights[] = { 72, 56, 56, 72 };

    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    TrackPaintUtilRightQuarterTurn3Tiles25DegDownTunnel(
        session, kTunnelGroup, height, direction, trackSequence, TunnelSubType::SlopeEnd, TunnelSubType::SlopeStart);

    switch (trackSequence)
    {
        case 0:
        case 3:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -9, height, session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void WildMouseTrackLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WildMouseTrackRightQuarterTurn325DegDown(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x0078B314 */
static void WildMouseTrackRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr int16_t generalSupportHeights[] = { 72, 56, 56, 72 };

    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    TrackPaintUtilRightQuarterTurn3Tiles25DegUpTunnel(
        session, kTunnelGroup, height, direction, trackSequence, TunnelSubType::SlopeStart, TunnelSubType::SlopeEnd);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -9, height, session.SupportColours);
            break;
        case 3:
            if (direction == 2)
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::Centre, -11, height, session.SupportColours);
            }
            else
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::Centre, -9, height, session.SupportColours);
            }
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

/** rct2: 0x0078B324 */
static void WildMouseTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WildMouseTrackRightQuarterTurn325DegUp(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

static void WildMouseTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WildMouseTrackRightQuarterTurn3(session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x0078B394 */
static void WildMouseTrackLeftQuarterTurn1(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -1, height, session.SupportColours);
    TrackPaintUtilLeftQuarterTurn1TileTunnel(
        session, kTunnelGroup, direction, height, 0, TunnelSubType::Flat, 0, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0078B3A4 */
static void WildMouseTrackRightQuarterTurn1(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrackLeftQuarterTurn1(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B354 */
static void WildMouseTrackFlatTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2Chain(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -5, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

/** rct2: 0x0078B364 */
static void WildMouseTrack60DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2Chain(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        if (direction == 0 || direction == 3)
        {
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::Centre, -17, height, session.SupportColours);
        }
        else
        {
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 16, height, session.SupportColours);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x0078B374 */
static void WildMouseTrackFlatTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack60DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B384 */
static void WildMouseTrack60DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrackFlatTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B344 */
static void WildMouseTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0078B3C4 */
static void WildMouseTrackRotationControlToggle(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0078B3B4 */
static void WildMouseTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteBrake(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

TrackPaintFunction GetTrackPaintFunctionWildMouse(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return WildMouseTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return WildMouseTrackStation;
        case TrackElemType::Up25:
            return WildMouseTrack25DegUp;
        case TrackElemType::Up60:
            return WildMouseTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return WildMouseTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return WildMouseTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return WildMouseTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return WildMouseTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return WildMouseTrack25DegDown;
        case TrackElemType::Down60:
            return WildMouseTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return WildMouseTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return WildMouseTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return WildMouseTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return WildMouseTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return WildMouseTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return WildMouseTrackRightQuarterTurn3;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return WildMouseTrackLeftQuarterTurn325DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return WildMouseTrackRightQuarterTurn325DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return WildMouseTrackLeftQuarterTurn325DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return WildMouseTrackRightQuarterTurn325DegDown;
        case TrackElemType::LeftQuarterTurn1Tile:
            return WildMouseTrackLeftQuarterTurn1;
        case TrackElemType::RightQuarterTurn1Tile:
            return WildMouseTrackRightQuarterTurn1;
        case TrackElemType::FlatToUp60:
            return WildMouseTrackFlatTo60DegUp;
        case TrackElemType::Up60ToFlat:
            return WildMouseTrack60DegUpToFlat;
        case TrackElemType::FlatToDown60:
            return WildMouseTrackFlatTo60DegDown;
        case TrackElemType::Down60ToFlat:
            return WildMouseTrack60DegDownToFlat;
        case TrackElemType::Brakes:
            return WildMouseTrackBrakes;
        case TrackElemType::RotationControlToggle:
            return WildMouseTrackRotationControlToggle;
        case TrackElemType::BlockBrakes:
            return WildMouseTrackBlockBrakes;
        default:
            return TrackPaintFunctionDummy;
    }
}
