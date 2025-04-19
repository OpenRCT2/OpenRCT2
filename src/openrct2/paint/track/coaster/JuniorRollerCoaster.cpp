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
#include "../../../object/StationObject.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

#include <algorithm>

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

static void JuniorRCPaintTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void JuniorRCPaintStation(
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
    TrackPaintUtilDrawStationTunnel(session, direction, height);

    if (TrackPaintUtilDrawStation(session, ride, direction, height, trackElement, StationBaseType::b, -2))
    {
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Boxed);
    }
    else if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void JuniorRCPaintTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    int8_t tunnelHeights[4] = { -8, 8, 8, -8 };
    TunnelSubType tunnelType[4] = { TunnelSubType::SlopeStart, TunnelSubType::SlopeEnd, TunnelSubType::SlopeEnd,
                                    TunnelSubType::SlopeStart };
    PaintUtilPushTunnelRotated(session, direction, height + tunnelHeights[direction], kTunnelGroup, tunnelType[direction]);

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 8, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void JuniorRCPaintTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        uint16_t ax = (direction == 0) ? 5 : 3;
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, ax, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void JuniorRCPaintTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    TunnelSubType tunnelType;
    int16_t tunnelHeight;
    if (direction == 1 || direction == 2)
    {
        tunnelType = TunnelSubType::FlatTo25Deg;
        tunnelHeight = height + 8;
    }
    else
    {
        tunnelType = TunnelSubType::Flat;
        tunnelHeight = height - 8;
    }

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, tunnelHeight, kTunnelGroup, tunnelType);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, tunnelHeight, kTunnelGroup, tunnelType);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 6, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

static constexpr int8_t junior_rc_track_right_quarter_turn_5_tiles_support_height_offset[][7] = {
    { 0, 0, 0, 0, 0, 0, -1 },
    { -1, 0, 0, 0, 0, 0, 0 },
    { 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

static void JuniorRCRightQuarterTurn5TilesPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    int32_t supportHeight = height + junior_rc_track_right_quarter_turn_5_tiles_support_height_offset[direction][trackSequence];
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, supportHeight, session.SupportColours);
            break;
        case 6:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), 0, supportHeight,
                session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::right,
                        PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left,
                        PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::left),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static constexpr uint8_t junior_rc_left_quarter_turn_5_tiles_to_right_turn_map[] = {
    6, 4, 5, 3, 1, 2, 0,
};

/* rct2: 0x0051917A */
static void JuniorRCLeftQuarterTurn5TilesPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = junior_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence];
    JuniorRCRightQuarterTurn5TilesPaintSetup(
        session, ride, trackSequence, DirectionNext(direction), height, trackElement, supportType);
}

/**
 * rct2: 0x008AAFC0, 0x00521593, 0x005216A1, 0x005217AF, 0x00521884
 */
static void JuniorRCFlatToLeftBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x008AAFD0, 0x00521959, 0x00521A2E, 0x00521B03, 0x00521C11
 */
static void JuniorRCFlatToRightBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void JuniorRCLeftBankToFlatPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCFlatToRightBankPaintSetup(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

static void JuniorRCRightBankToFlatPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCFlatToLeftBankPaintSetup(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/* rct2: 0x008AB010, 0x0052304C */
static void JuniorRCBankedRightQuarterTurn5TilesPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    int32_t supportHeight = height;
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, supportHeight, session.SupportColours);
            break;
        case 6:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), 0, supportHeight,
                session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::right,
                        PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left,
                        PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::left),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/* rct2: 0x008AB000 */
static void JuniorRCBankedLeftQuarterTurn5TilesPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = junior_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence];
    JuniorRCBankedRightQuarterTurn5TilesPaintSetup(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/**
 * rct2: 0x008AB020, 0x00522073, 0x00522182, 0x00522291, 0x00522367
 */
static void JuniorRCLeftBankTo25DegUpPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 3, height, session.SupportColours);
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/**
 * rct2: 0x008AB030, 0x0052243D, 0x00522513, 0x005225E9, 0x005226F8
 */
static void JuniorRCRightBankTo25DegUpPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 3, height, session.SupportColours);
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/**
 * rct2: 0x008AB040, 0x00522807, 0x0052291A, 0x00522A2D, 0x00522B07
 */
static void JuniorRC25DegUpToLeftBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    TunnelSubType tunnelType;
    int16_t tunnelHeight;
    if (direction == 1 || direction == 2)
    {
        tunnelType = TunnelSubType::FlatTo25Deg;
        tunnelHeight = height + 8;
    }
    else
    {
        tunnelType = TunnelSubType::Flat;
        tunnelHeight = height - 8;
    }

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, tunnelHeight, kTunnelGroup, tunnelType);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, tunnelHeight, kTunnelGroup, tunnelType);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 6, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/**
 * rct2: 0x008AB050, 0x00522BE1, 0x00522CBB, 0x00522D95, 0x00522EA8
 */
static void JuniorRC25DegUpToRightBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    TunnelSubType tunnelType;
    int16_t tunnelHeight;
    if (direction == 1 || direction == 2)
    {
        tunnelType = TunnelSubType::FlatTo25Deg;
        tunnelHeight = height + 8;
    }
    else
    {
        tunnelType = TunnelSubType::Flat;
        tunnelHeight = height - 8;
    }

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, tunnelHeight, kTunnelGroup, tunnelType);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, tunnelHeight, kTunnelGroup, tunnelType);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 6, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/**
 * rct2: 0x008AB060
 */
static void JuniorRCLeftBankTo25DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRC25DegUpToRightBankPaintSetup(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/**
 * rct2: 0x008AB070
 */
static void JuniorRCRightBankTo25DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRC25DegUpToLeftBankPaintSetup(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/**
 * rct2: 0x008AB080
 */
static void JuniorRC25DegDownToLeftBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCRightBankTo25DegUpPaintSetup(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/**
 * rct2: 0x008AB090
 */
static void JuniorRC25DegDownToRightBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCLeftBankTo25DegUpPaintSetup(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/**
 * rct2: 0x008AB0A0, 0x00521D1F, 0x00521DF4, 0x00521EC9, 0x00521F9E
 */
static void JuniorRCLeftBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x008AB0B0
 */
static void JuniorRCRightBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCLeftBankPaintSetup(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

static void JuniorRCPaintTrackLeftQuarterTurn5Tiles25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr uint8_t supportSpecial[4] = { 8, 8, 8, 3 };
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, (direction == 0 || direction == 3) ? 10 : 8,
                height, session.SupportColours);
            break;
        case 6:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, DirectionPrev(direction), supportSpecial[direction],
                height, session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    if (direction == 2 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    if (direction == 3 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::top),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::top, PaintSegment::left,
                        PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomRight, PaintSegment::right, PaintSegment::topRight, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left,
                        PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottom),
                    direction),
                0xFFFF, 0);
            break;
    }
    if (trackSequence == 3)
    {
        PaintUtilSetGeneralSupportHeight(session, height + 64);
    }
    else
    {
        PaintUtilSetGeneralSupportHeight(session, height + 72);
    }
}

static void JuniorRCPaintTrackRightQuarterTurn5Tiles25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr uint8_t supportSpecial[4] = { 11, 8, 8, 7 };
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, supportSpecial[direction], height,
                session.SupportColours);
            break;
        case 6:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), 8, height,
                session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::right,
                        PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left,
                        PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::left),
                    direction),
                0xFFFF, 0);
            break;
    }
    if (trackSequence == 3)
    {
        PaintUtilSetGeneralSupportHeight(session, height + 64);
    }
    else
    {
        PaintUtilSetGeneralSupportHeight(session, height + 72);
    }
}

/* rct2: 0x008AAE30, 0x0051AA42, 0x0051AA68, 0x0051AA8C, 0x0051AAB0 */
static void JuniorRCLeftQuarterTurn5Tiles25DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrackRightQuarterTurn5Tiles25DegUp(
        session, ride, junior_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence], (direction + 1) & 3, height,
        trackElement, supportType);
}

/* rct2: 0x008AAE40, 0x0051AAD4, 0x0051AE04, 0x0051B10E, 0x0051B3F4 */
static void JuniorRCRightQuarterTurn5Tiles25DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrackLeftQuarterTurn5Tiles25DegUp(
        session, ride, junior_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence], (direction - 1) & 3, height,
        trackElement, supportType);
}

/* rct2: 0x008AAE50, 0x0051B6FE, 0x0051B946, 0x0051BB8E, 0x0051BBA8 */
static void JuniorRCSBendLeftPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            break;
        case 1:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::TopLeftSide, direction & 1, 0, height, session.SupportColours);
            break;
        case 3:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::top);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::top);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                PaintSegment::bottomRight, PaintSegment::right);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, (direction & 1)), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/* rct2: 0x008AAE60, 0x0051BBC0, 0x0051BE06, 0x0051C04E, 0x0051C068 */
static void JuniorRCSBendRightPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            break;
        case 1:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::BottomRightSide, direction & 1, 0, height,
                session.SupportColours);
            break;
        case 3:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                PaintSegment::bottomRight, PaintSegment::right);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::top);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, (direction & 1)), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AAEB0, 0x0051C0E0, 0x0051C2C1, 0x0051C47F, 0x0051C61D */
static void JuniorRCRightQuarterTurn3TilesPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            break;
        case 3:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, DirectionPrev(direction), 0, height,
                session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::bottom);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::left);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AAEA0 */
static void JuniorRCLeftQuarterTurn3TilesPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    JuniorRCRightQuarterTurn3TilesPaintSetup(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AA0D0, 0x00523EA0, 0x005240CC, 0x0052430F, 0x00524500*/
static void JuniorRCRightQuarterTurn3TilesBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            break;
        case 3:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), 0, height,
                session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::bottom);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::left);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void JuniorRCPaintTrackRightQuarterTurn3Tiles25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }

    if (direction == 0 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    if (direction == 1 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 8, height, session.SupportColours);
            break;
        case 3:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), 8, height,
                session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::left);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    if (trackSequence == 0 || trackSequence == 3)
        PaintUtilSetGeneralSupportHeight(session, height + 72);
    else
        PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void JuniorRCPaintTrackRightQuarterTurn3Tiles25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    if (direction == 0 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }

    if (direction == 1 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 8, height, session.SupportColours);
            break;
        case 3:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), 8, height,
                session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::left);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    if (trackSequence == 0 || trackSequence == 3)
        PaintUtilSetGeneralSupportHeight(session, height + 72);
    else
        PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AA0C0 */
static void JuniorRCLeftQuarterTurn3TilesBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    JuniorRCRightQuarterTurn3TilesBankPaintSetup(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

static constexpr uint8_t junior_rc_left_quarter_turn_3_tiles_to_right_turn_map[] = {
    3,
    1,
    2,
    0,
};

/** rct2: 0x008AAEC0 */
static void JuniorRCLeftQuarterTurn3Tiles25DegUpPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = junior_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    JuniorRCPaintTrackRightQuarterTurn3Tiles25DegDown(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AAEE0 */
static void JuniorRCLeftQuarterTurn3Tiles25DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = junior_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    JuniorRCPaintTrackRightQuarterTurn3Tiles25DegUp(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AB0F0, 0x0052B3A4, 0x0052B5F8, 0x0052B863, 0x0052BA78 */
static void JuniorRCRightHalfBankedHelixUpSmallPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 0)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 2, height, session.SupportColours);
    }
    else if (trackSequence == 3)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), 6, height, session.SupportColours);
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 1 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right,
                PaintSegment::bottomRight);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft,
                PaintSegment::left);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AB110, 0x0052BD80, 0x0052BFD4, 0x0052C23B, 0x0052C450 */
static void JuniorRCRightHalfBankedHelixDownSmallPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 0)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 6, height, session.SupportColours);
    }
    else if (trackSequence == 3)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), 2, height, session.SupportColours);
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 1 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right,
                PaintSegment::bottomRight);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft,
                PaintSegment::left);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AB0E0 */
static void JuniorRCLeftHalfBankedHelixUpSmallPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence > 3)
    {
        trackSequence -= 4;
        direction--;
        direction &= 3;
    }
    trackSequence = junior_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    JuniorRCRightHalfBankedHelixDownSmallPaintSetup(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AB100 */
static void JuniorRCLeftHalfBankedHelixDownSmallPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence > 3)
    {
        trackSequence -= 4;
        direction--;
        direction &= 3;
    }
    trackSequence = junior_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    JuniorRCRightHalfBankedHelixUpSmallPaintSetup(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AB130, 0x0052C7BC, 0x0052CB6B, 0x0052CF32, 0x0052D2B3 */
static void JuniorRCRightHalfBankedHelixUpLargePaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 0)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 1, height, session.SupportColours);
    }
    else if (trackSequence == 6)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), 7, height, session.SupportColours);
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::right),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::right,
                        PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left,
                        PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::left),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AB150, 0x0052D778, 0x0052DB27, 0x0052DEEA, 0x0052E26B */
static void JuniorRCRightHalfBankedHelixDownLargePaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 0)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 7, height, session.SupportColours);
    }
    else if (trackSequence == 6)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), 1, height, session.SupportColours);
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::right),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::right,
                        PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left,
                        PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::left),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AB120 */
static void JuniorRCLeftHalfBankedHelixUpLargePaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence > 6)
    {
        trackSequence -= 7;
        direction--;
        direction &= 3;
    }
    trackSequence = junior_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence];
    JuniorRCRightHalfBankedHelixDownLargePaintSetup(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AB140 */
static void JuniorRCLeftHalfBankedHelixDownLargePaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence > 6)
    {
        trackSequence -= 7;
        direction--;
        direction &= 3;
    }
    trackSequence = junior_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence];
    JuniorRCRightHalfBankedHelixUpLargePaintSetup(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/**
 * rct2: 0x008AAF00, 0x0051851C, 0x005185D8
 */
static void JuniorRCBrakePaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x008AB280, 0x00518694, 0x00518759
 */
static void JuniorRCBlockBrakePaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteBrake(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AAF80 */
static void JuniorRCLeftEighthToDiagPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            break;
        case 4:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::LeftCorner, DirectionPrev(direction), 0, height,
                session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::top);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                PaintSegment::right);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::left, PaintSegment::topLeft);
            break;
        case 4:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::topLeft,
                PaintSegment::bottom, PaintSegment::bottomRight);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AAF90 */
static void JuniorRCRightEighthToDiagPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            break;
        case 4:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::BottomCorner, DirectionNext(direction), 0, height,
                session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                PaintSegment::bottomRight, PaintSegment::right);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::centre, PaintSegment::topRight, PaintSegment::left, PaintSegment::topLeft, PaintSegment::top);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight);
            break;
        case 4:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::bottomRight);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AAFA0 */
static void JuniorRCLeftEighthToOrthogonalPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    JuniorRCRightEighthToDiagPaintSetup(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AAFB0 */
static void JuniorRCRightEighthToOrthogonalPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    JuniorRCLeftEighthToDiagPaintSetup(session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AB160 */
static void JuniorRCLeftEighthToDiagBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            break;
        case 4:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::BottomCorner, direction, 0, height, session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::top);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                PaintSegment::right);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::left, PaintSegment::topLeft);
            break;
        case 4:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::topLeft,
                PaintSegment::bottom, PaintSegment::bottomRight);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AB170 */
static void JuniorRCRightEighthToDiagBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            break;
        case 4:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height, session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                PaintSegment::bottomRight, PaintSegment::right);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::centre, PaintSegment::topRight, PaintSegment::left, PaintSegment::topLeft, PaintSegment::top);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight);
            break;
        case 4:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::bottomRight);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AB180 */
static void JuniorRCLeftEighthToOrthogonalBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    JuniorRCRightEighthToDiagBankPaintSetup(
        session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AB190 */
static void JuniorRCRightEighthToOrthogonalBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    JuniorRCLeftEighthToDiagBankPaintSetup(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

static void JuniorRCPaintTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (SupportedSequences::kDiagStraightFlat[trackSequence] != MetalSupportPlace::None)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, SupportedSequences::kDiagStraightFlat[trackSequence], direction, 0, height,
            session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(BlockedSegments::kDiagStraightFlat[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void JuniorRCTrackDiagBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void JuniorRCTrackDiagBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteBrake(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void JuniorRCPaintTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 8, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void JuniorRCPaintTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void JuniorRCPaintTrackDiagFlatTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height + 6, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

static void JuniorRCPaintTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 4, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void JuniorRCPaintTrackDiag60DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 4, height + 13, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

static void JuniorRCPaintTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrackDiag25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void JuniorRCPaintTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrackDiag25DegUpToFlat(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void JuniorRCPaintTrackDiagFlatTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrackDiag60DegUpToFlat(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void JuniorRCPaintTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrackDiagFlatTo25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void JuniorRCPaintTrackDiag60DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrackDiagFlatTo60DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AB1C0 */
static void JuniorRCDiagFlatToLeftBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (trackSequence == 3)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AB1D0 */
static void JuniorRCDiagFlatToRightBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (trackSequence == 3)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AB1E0 */
static void JuniorRCDiagLeftBankToFlatPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCDiagFlatToRightBankPaintSetup(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AB1F0 */
static void JuniorRCDiagRightBankToFlatPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCDiagFlatToLeftBankPaintSetup(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AB220 */
static void JuniorRCDiagLeftBankTo25DegUpPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AB230 */
static void JuniorRCDiagRightBankTo25DegUpPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AB200 */
static void JuniorRCDiag25DegUpToLeftBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 4, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AB210 */
static void JuniorRCDiag25DegUpToRightBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 4, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AB200 */
static void JuniorRCDiagLeftBankTo25DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCDiag25DegUpToRightBankPaintSetup(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AB210 */
static void JuniorRCDiagRightBankTo25DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCDiag25DegUpToLeftBankPaintSetup(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AB220 */
static void JuniorRCDiag25DegDownToLeftBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCDiagRightBankTo25DegUpPaintSetup(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AB230 */
static void JuniorRCDiag25DegDownToRightBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCDiagLeftBankTo25DegUpPaintSetup(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AB1A0 */
static void JuniorRCDiagLeftBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AB1B0 */
static void JuniorRCDiagRightBankPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCDiagLeftBankPaintSetup(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void JuniorRCPaintTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    static constexpr int8_t support[4] = { 35, 29, 25, 32 };
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, support[direction & 3], height,
            session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

static void JuniorRC60DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrack60DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

static void JuniorRCPaintTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2Chain(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    static constexpr int8_t support[4] = { 12, 12, 12, 14 };
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, support[direction & 3], height,
            session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void JuniorRC60DegDownTo25DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

static void JuniorRCPaintTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2Chain(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 20, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void JuniorRC25DegDownTo60DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

static void JuniorRCPaintTrackDiag60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 36, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

static void JuniorRCPaintTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrackDiag60DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void JuniorRCPaintTrackDiag25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 16, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void JuniorRCPaintTrackDiag60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 21, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void JuniorRCPaintTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrackDiag60DegUpTo25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void JuniorRCPaintTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrackDiag25DegUpTo60DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void JuniorRCFlatTo60DegUpPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2Chain(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    static constexpr int8_t support[4] = { 12, 12, 12, 14 };
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, support[direction], height - 7,
            session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void JuniorRC60DegDownToFlatPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCFlatTo60DegUpPaintSetup(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void JuniorRC60DegUpToFlatPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2Chain(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 20, height - 5, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void JuniorRCFlatTo60DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRC60DegUpToFlatPaintSetup(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/* rct2: 0x005189B0 */
static void JuniorRC25DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/* rct2: 0x00518FE8 */
static void JuniorRCFlatTo25DegDownPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/* rct2: 0x00518CCC */
static void JuniorRC25DegDownToFlatPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    JuniorRCPaintTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void JuniorRCBoosterPaintSetup(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void JuniorRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintAddImageAsParentRotated(
        session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
    DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal, 6);

    TrackPaintUtilOnridePhotoPaint2(session, direction, trackElement, height);
}

/* 0x008AAA0C */
static TrackPaintFunction GetTrackPaintFunctionJuniorRCTemplate(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return JuniorRCPaintTrackFlat;
        case TrackElemType::EndStation:
            return JuniorRCPaintStation;
        case TrackElemType::BeginStation:
            return JuniorRCPaintStation;
        case TrackElemType::MiddleStation:
            return JuniorRCPaintStation;
        case TrackElemType::Up25:
            return JuniorRCPaintTrack25DegUp;
        case TrackElemType::Up60:
            return JuniorRCPaintTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return JuniorRCPaintTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return JuniorRCPaintTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return JuniorRCPaintTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return JuniorRCPaintTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return JuniorRC25DegDownPaintSetup;
        case TrackElemType::Down60:
            return JuniorRC60DegDownPaintSetup;
        case TrackElemType::FlatToDown25:
            return JuniorRCFlatTo25DegDownPaintSetup;
        case TrackElemType::Down25ToDown60:
            return JuniorRC25DegDownTo60DegDownPaintSetup;
        case TrackElemType::Down60ToDown25:
            return JuniorRC60DegDownTo25DegDownPaintSetup;
        case TrackElemType::Down25ToFlat:
            return JuniorRC25DegDownToFlatPaintSetup;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return JuniorRCLeftQuarterTurn5TilesPaintSetup;
        case TrackElemType::RightQuarterTurn5Tiles:
            return JuniorRCRightQuarterTurn5TilesPaintSetup;
        case TrackElemType::FlatToLeftBank:
            return JuniorRCFlatToLeftBankPaintSetup;
        case TrackElemType::FlatToRightBank:
            return JuniorRCFlatToRightBankPaintSetup;
        case TrackElemType::LeftBankToFlat:
            return JuniorRCLeftBankToFlatPaintSetup;
        case TrackElemType::RightBankToFlat:
            return JuniorRCRightBankToFlatPaintSetup;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return JuniorRCBankedLeftQuarterTurn5TilesPaintSetup;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return JuniorRCBankedRightQuarterTurn5TilesPaintSetup;
        case TrackElemType::LeftBankToUp25:
            return JuniorRCLeftBankTo25DegUpPaintSetup;
        case TrackElemType::RightBankToUp25:
            return JuniorRCRightBankTo25DegUpPaintSetup;
        case TrackElemType::Up25ToLeftBank:
            return JuniorRC25DegUpToLeftBankPaintSetup;
        case TrackElemType::Up25ToRightBank:
            return JuniorRC25DegUpToRightBankPaintSetup;
        case TrackElemType::LeftBankToDown25:
            return JuniorRCLeftBankTo25DegDownPaintSetup;
        case TrackElemType::RightBankToDown25:
            return JuniorRCRightBankTo25DegDownPaintSetup;
        case TrackElemType::Down25ToLeftBank:
            return JuniorRC25DegDownToLeftBankPaintSetup;
        case TrackElemType::Down25ToRightBank:
            return JuniorRC25DegDownToRightBankPaintSetup;
        case TrackElemType::LeftBank:
            return JuniorRCLeftBankPaintSetup;
        case TrackElemType::RightBank:
            return JuniorRCRightBankPaintSetup;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return JuniorRCPaintTrackLeftQuarterTurn5Tiles25DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return JuniorRCPaintTrackRightQuarterTurn5Tiles25DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return JuniorRCLeftQuarterTurn5Tiles25DegDownPaintSetup;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return JuniorRCRightQuarterTurn5Tiles25DegDownPaintSetup;
        case TrackElemType::SBendLeft:
            return JuniorRCSBendLeftPaintSetup;
        case TrackElemType::SBendRight:
            return JuniorRCSBendRightPaintSetup;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return JuniorRCLeftQuarterTurn3TilesPaintSetup;
        case TrackElemType::RightQuarterTurn3Tiles:
            return JuniorRCRightQuarterTurn3TilesPaintSetup;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return JuniorRCLeftQuarterTurn3TilesBankPaintSetup;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return JuniorRCRightQuarterTurn3TilesBankPaintSetup;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return JuniorRCLeftQuarterTurn3Tiles25DegUpPaintSetup;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return JuniorRCPaintTrackRightQuarterTurn3Tiles25DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return JuniorRCLeftQuarterTurn3Tiles25DegDownPaintSetup;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return JuniorRCPaintTrackRightQuarterTurn3Tiles25DegDown;

        case TrackElemType::FlatToUp60:
            return JuniorRCFlatTo60DegUpPaintSetup;
        case TrackElemType::Up60ToFlat:
            return JuniorRC60DegUpToFlatPaintSetup;
        case TrackElemType::FlatToDown60:
            return JuniorRCFlatTo60DegDownPaintSetup;
        case TrackElemType::Down60ToFlat:
            return JuniorRC60DegDownToFlatPaintSetup;

        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return JuniorRCLeftHalfBankedHelixUpSmallPaintSetup;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return JuniorRCRightHalfBankedHelixUpSmallPaintSetup;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return JuniorRCLeftHalfBankedHelixDownSmallPaintSetup;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return JuniorRCRightHalfBankedHelixDownSmallPaintSetup;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return JuniorRCLeftHalfBankedHelixUpLargePaintSetup;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return JuniorRCRightHalfBankedHelixUpLargePaintSetup;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return JuniorRCLeftHalfBankedHelixDownLargePaintSetup;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return JuniorRCRightHalfBankedHelixDownLargePaintSetup;

        case TrackElemType::Brakes:
            return JuniorRCBrakePaintSetup;

        case TrackElemType::LeftEighthToDiag:
            return JuniorRCLeftEighthToDiagPaintSetup;
        case TrackElemType::RightEighthToDiag:
            return JuniorRCRightEighthToDiagPaintSetup;
        case TrackElemType::LeftEighthToOrthogonal:
            return JuniorRCLeftEighthToOrthogonalPaintSetup;
        case TrackElemType::RightEighthToOrthogonal:
            return JuniorRCRightEighthToOrthogonalPaintSetup;
        case TrackElemType::LeftEighthBankToDiag:
            return JuniorRCLeftEighthToDiagBankPaintSetup;
        case TrackElemType::RightEighthBankToDiag:
            return JuniorRCRightEighthToDiagBankPaintSetup;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return JuniorRCLeftEighthToOrthogonalBankPaintSetup;
        case TrackElemType::RightEighthBankToOrthogonal:
            return JuniorRCRightEighthToOrthogonalBankPaintSetup;
        case TrackElemType::DiagFlat:
            return JuniorRCPaintTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return JuniorRCPaintTrackDiag25DegUp;
        case TrackElemType::DiagUp60:
            return JuniorRCPaintTrackDiag60DegUp;
        case TrackElemType::DiagFlatToUp25:
            return JuniorRCPaintTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return JuniorRCPaintTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return JuniorRCPaintTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return JuniorRCPaintTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return JuniorRCPaintTrackDiag25DegDown;
        case TrackElemType::DiagDown60:
            return JuniorRCPaintTrackDiag60DegDown;
        case TrackElemType::DiagFlatToDown25:
            return JuniorRCPaintTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return JuniorRCPaintTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return JuniorRCPaintTrackDiag60DegDownTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return JuniorRCPaintTrackDiag25DegDownToFlat;

        case TrackElemType::DiagFlatToLeftBank:
            return JuniorRCDiagFlatToLeftBankPaintSetup;
        case TrackElemType::DiagFlatToRightBank:
            return JuniorRCDiagFlatToRightBankPaintSetup;
        case TrackElemType::DiagLeftBankToFlat:
            return JuniorRCDiagLeftBankToFlatPaintSetup;
        case TrackElemType::DiagRightBankToFlat:
            return JuniorRCDiagRightBankToFlatPaintSetup;
        case TrackElemType::DiagLeftBankToUp25:
            return JuniorRCDiagLeftBankTo25DegUpPaintSetup;
        case TrackElemType::DiagRightBankToUp25:
            return JuniorRCDiagRightBankTo25DegUpPaintSetup;
        case TrackElemType::DiagUp25ToLeftBank:
            return JuniorRCDiag25DegUpToLeftBankPaintSetup;
        case TrackElemType::DiagUp25ToRightBank:
            return JuniorRCDiag25DegUpToRightBankPaintSetup;
        case TrackElemType::DiagLeftBankToDown25:
            return JuniorRCDiagLeftBankTo25DegDownPaintSetup;
        case TrackElemType::DiagRightBankToDown25:
            return JuniorRCDiagRightBankTo25DegDownPaintSetup;
        case TrackElemType::DiagDown25ToLeftBank:
            return JuniorRCDiag25DegDownToLeftBankPaintSetup;
        case TrackElemType::DiagDown25ToRightBank:
            return JuniorRCDiag25DegDownToRightBankPaintSetup;
        case TrackElemType::DiagLeftBank:
            return JuniorRCDiagLeftBankPaintSetup;
        case TrackElemType::DiagRightBank:
            return JuniorRCDiagRightBankPaintSetup;

        case TrackElemType::BlockBrakes:
            return JuniorRCBlockBrakePaintSetup;
        case TrackElemType::DiagBrakes:
            return JuniorRCTrackDiagBrakes;
        case TrackElemType::DiagBlockBrakes:
            return JuniorRCTrackDiagBlockBrakes;

        case TrackElemType::Booster:
            return JuniorRCBoosterPaintSetup;

        case TrackElemType::DiagDown60ToFlat:
            return JuniorRCPaintTrackDiag60DegDownToFlat;
        case TrackElemType::DiagUp60ToFlat:
            return JuniorRCPaintTrackDiag60DegUpToFlat;
        case TrackElemType::DiagFlatToUp60:
            return JuniorRCPaintTrackDiagFlatTo60DegUp;
        case TrackElemType::DiagFlatToDown60:
            return JuniorRCPaintTrackDiagFlatTo60DegDown;

        case TrackElemType::OnRidePhoto:
            return JuniorRCTrackOnRidePhoto;
        default:
            return TrackPaintFunctionDummy;
    }
}

TrackPaintFunction GetTrackPaintFunctionJuniorRC(OpenRCT2::TrackElemType trackType)
{
    return GetTrackPaintFunctionJuniorRCTemplate(trackType);
}

TrackPaintFunction GetTrackPaintFunctionWaterRC(OpenRCT2::TrackElemType trackType)
{
    return GetTrackPaintFunctionJuniorRCTemplate(trackType);
}
