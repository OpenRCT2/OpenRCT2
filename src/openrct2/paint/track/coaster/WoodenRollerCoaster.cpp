/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../config/Config.h"
#include "../../../drawing/Drawing.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Boundbox.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Paint.Surface.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

/** rct2: 0x008AC658, 0x008AC668, 0x008AC738 */
template<bool isClassic>
void WoodenRCTrackFlatToBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AC6D8, 0x008AC6E8 */
template<bool isClassic>
static void WoodenRCTrack25DegUpToBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
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

/** rct2: 0x008AC6B8, 0x008AC6C8 */
template<bool isClassic>
static void WoodenRCTrackBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
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

/** rct2: 0x008AC808 */
template<bool isClassic>
static void WoodenRCTrackLeftQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);

    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    static constexpr int blockedSegments[4] = {
        kSegmentsAll,
        kSegmentsAll,
        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
        kSegmentsAll,
    };

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftBankedQuarterTurn3Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

template<bool isClassic>
static void WoodenRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);

    TrackPaintUtilRightQuarterTurn5TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    static constexpr int blockedSegments[7] = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight),
        EnumsToFlags(
            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft,
            PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight),
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        kSegmentsAll,
    };

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::BankedRightQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008ACAB8 */
template<bool isClassic>
static void WoodenRCTrackLeftHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
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
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
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
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::topRight),
                    direction),
                48, 0x20);
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
                    EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                48, 0x20);
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
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                48, 0x20);
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

/** rct2: 0x008ACAC8 */
template<bool isClassic>
static void WoodenRCTrackRightHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
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
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                48, 0x20);
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
                    EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                48, 0x20);
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
        case 4:
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
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::topRight),
                    direction),
                48, 0x20);
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
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
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

/** rct2: 0x008ACAF8 */
template<bool isClassic>
static void WoodenRCTrackLeftHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                48, 0x20);
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
                    EnumsToFlags(PaintSegment::top, PaintSegment::bottom, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                48, 0x20);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::topRight),
                    direction),
                48, 0x20);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                48, 0x20);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                48, 0x20);
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
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 11:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                48, 0x20);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                48, 0x20);
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

/** rct2: 0x008ACB08 */
template<bool isClassic>
static void WoodenRCTrackRightHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                48, 0x20);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                48, 0x20);
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
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                48, 0x20);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                48, 0x20);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::topRight),
                    direction),
                48, 0x20);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                48, 0x20);
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
                    EnumsToFlags(PaintSegment::top, PaintSegment::bottom, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 11:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                48, 0x20);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
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

/** rct2: 0x008ACB38 */
template<bool isClassic>
static void WoodenRCTrackLeftBankToLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
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

/** rct2: 0x008ACB48 */
template<bool isClassic>
static void WoodenRCTrackRightBankToRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightBankToRightQuarterTurn3TilesUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
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

/** rct2: 0x008ACA18, 0x008AC9F8 */
template<bool isClassic>
static void WoodenRCTrackDiagFlatToBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008ACA58, 0x008ACA68 */
template<bool isClassic>
static void WoodenRCTrackDiagBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagFlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008ACA38, 0x008ACA48 */
template<bool isClassic>
static void WoodenRCTrackDiagUp25ToBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AC9D8 */
template<bool isClassic>
static void WoodenRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AC998 */
template<bool isClassic>
static void WoodenRCTrackLeftEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftEighthBankToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AC9A8 */
template<bool isClassic>
static void WoodenRCTrackRightEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightEighthBankToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AC568 */
template<bool isClassic>
static void WoodenRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteSupportColoursWithChildChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

template<bool isClassic>
static void WoodenRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        trackPaintSpriteSupportColoursBrakePlatformlessStation(
            session, ride, trackSequence, direction, height, trackElement, supportType);
    }
    else
    {
        trackPaintSpriteSupportColoursPlatformlessStation(
            session, ride, trackSequence, direction, height, trackElement, supportType);
    }
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AC578 */
template<bool isClassic>
static void WoodenRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChildChain(session, ride, trackSequence, direction, height, trackElement, supportType);
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

/** rct2: 0x008AC588 */
template<bool isClassic>
static void WoodenRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteSupportColoursWithChildChain(session, ride, trackSequence, direction, height, trackElement, supportType);

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

/** rct2: 0x008AC598 */
template<bool isClassic>
static void WoodenRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChildChain(session, ride, trackSequence, direction, height, trackElement, supportType);
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

/** rct2: 0x008AC5A8 */
template<bool isClassic>
static void WoodenRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChildChain(session, ride, trackSequence, direction, height, trackElement, supportType);
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

/** rct2: 0x008AC5B8 */
template<bool isClassic>
static void WoodenRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChildChain(session, ride, trackSequence, direction, height, trackElement, supportType);

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

/** rct2: 0x008AC5C8 */
template<bool isClassic>
static void WoodenRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChildChain(session, ride, trackSequence, direction, height, trackElement, supportType);
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

/** rct2: 0x008AC5D8 */
template<bool isClassic>
static void WoodenRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUp<isClassic>(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC5E8 */
template<bool isClassic>
static void WoodenRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack60DegUp<isClassic>(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC5F8 */
template<bool isClassic>
static void WoodenRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpToFlat<isClassic>(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC608 */
template<bool isClassic>
static void WoodenRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack60DegUpTo25DegUp<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC618 */
template<bool isClassic>
static void WoodenRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpTo60DegUp<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC628 */
template<bool isClassic>
static void WoodenRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatTo25DegUp<isClassic>(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);

    static WoodenSupportSubType supportSubType[kNumOrthogonalDirections][7] = {
        { WoodenSupportSubType::NeSw, WoodenSupportSubType::Null, WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner0,
          WoodenSupportSubType::Null, WoodenSupportSubType::Corner2, WoodenSupportSubType::NwSe },
        { WoodenSupportSubType::NwSe, WoodenSupportSubType::Null, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner1,
          WoodenSupportSubType::Null, WoodenSupportSubType::Corner3, WoodenSupportSubType::NeSw },
        { WoodenSupportSubType::NeSw, WoodenSupportSubType::Null, WoodenSupportSubType::Corner0, WoodenSupportSubType::Corner2,
          WoodenSupportSubType::Null, WoodenSupportSubType::Corner0, WoodenSupportSubType::NwSe },
        { WoodenSupportSubType::NwSe, WoodenSupportSubType::Null, WoodenSupportSubType::Corner1, WoodenSupportSubType::Corner3,
          WoodenSupportSubType::Null, WoodenSupportSubType::Corner1, WoodenSupportSubType::NeSw },
    };
    TrackPaintUtilRightQuarterTurn5TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    if (supportSubType[direction][trackSequence] != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetup(
            session, supportType.wooden, supportSubType[direction][trackSequence], height, session.SupportColours);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = kSegmentsAll;
            break;
        case 1:
            blockedSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                PaintSegment::bottomLeft, PaintSegment::bottomRight);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft,
                PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
            break;
        case 4:
            blockedSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
            break;
        case 5:
            blockedSegments = EnumsToFlags(
                PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                PaintSegment::bottomRight);
            break;
        case 6:
            blockedSegments = kSegmentsAll;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

template<bool isClassic>
static void WoodenRCTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    WoodenRCTrackRightQuarterTurn5<isClassic>(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC678 */
template<bool isClassic>
static void WoodenRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatToBank<isClassic>(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC688 */
template<bool isClassic>
static void WoodenRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatToBank<isClassic>(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrackBankedLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    WoodenRCTrackBankedRightQuarterTurn5<isClassic>(
        session, ride, trackSequence, DirectionNext(direction), height, trackElement, supportType);
}

/** rct2:  */

/** rct2: 0x008AC6F8 */
template<bool isClassic>
static void WoodenRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpToBank<isClassic>(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC708 */
template<bool isClassic>
static void WoodenRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpToBank<isClassic>(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC718 */
template<bool isClassic>
static void WoodenRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackBankTo25DegUp<isClassic>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AC728 */
template<bool isClassic>
static void WoodenRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackBankTo25DegUp<isClassic>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AC748 */
template<bool isClassic>
static void WoodenRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatToBank<isClassic>(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC758 */
template<bool isClassic>
static void WoodenRCTrackLeftQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
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

/** rct2: 0x008AC768 */
template<bool isClassic>
static void WoodenRCTrackRightQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
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
                        PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
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

/** rct2: 0x008AC778 */
template<bool isClassic>
static void WoodenRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    WoodenRCTrackRightQuarterTurn525DegUp<isClassic>(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC788 */
template<bool isClassic>
static void WoodenRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    WoodenRCTrackLeftQuarterTurn525DegUp<isClassic>(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC798 */
template<bool isClassic>
static void WoodenRCTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::SBendLeft>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
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
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
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

/** rct2: 0x008AC7A8 */
template<bool isClassic>
static void WoodenRCTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::SBendRight>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
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
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
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
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
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

/** rct2: 0x008ACE18 */
static void WoodenRCTrackLeftVerticalLoop(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(
                session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 8, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 20, height, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 9, height, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 16, height, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 16, height, session.SupportColours);
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
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 3:
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
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
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
        case 7:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 16, height, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 16, height, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 20, height, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 9, height, session.SupportColours);
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
        case 9:
            MetalASupportsPaintSetup(
                session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 8, height, session.SupportColours);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
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
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x008ACE28 */
static void WoodenRCTrackRightVerticalLoop(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(
                session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 8, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 16, height, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 16, height, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 9, height, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 20, height, session.SupportColours);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 3:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 7:
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 9, height, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 20, height, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 16, height, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 16, height, session.SupportColours);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 9:
            MetalASupportsPaintSetup(
                session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 8, height, session.SupportColours);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }

    TrackPaintUtilRightVerticalLoopSegments(session, direction, trackSequence);
}

/** rct2: 0x008AC7E8 */
template<bool isClassic>
static void WoodenRCTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftQuarterTurn3Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
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

/** rct2: 0x008AC7F8 */
template<bool isClassic>
static void WoodenRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WoodenRCTrackLeftQuarterTurn3<isClassic>(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC818 */
template<bool isClassic>
static void WoodenRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WoodenRCTrackLeftQuarterTurn3Bank<isClassic>(
        session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

/** rct2: 0x008AC828 */
template<bool isClassic>
static void WoodenRCTrackLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftQuarterTurn3TilesUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
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

/** rct2: 0x008AC838 */
template<bool isClassic>
static void WoodenRCTrackRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightQuarterTurn3TilesUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
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

/** rct2: 0x008AC848 */
template<bool isClassic>
static void WoodenRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WoodenRCTrackRightQuarterTurn325DegUp<isClassic>(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC858 */
template<bool isClassic>
static void WoodenRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WoodenRCTrackLeftQuarterTurn325DegUp<isClassic>(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACAD8 */
template<bool isClassic>
static void WoodenRCTrackLeftHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = DirectionPrev(direction);
    }
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WoodenRCTrackRightHalfBankedHelixUpSmall<isClassic>(
        session, ride, trackSequence, DirectionNext(direction), height, trackElement, supportType);
}

/** rct2: 0x008ACAE8 */
template<bool isClassic>
static void WoodenRCTrackRightHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = DirectionNext(direction);
    }
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WoodenRCTrackLeftHalfBankedHelixUpSmall<isClassic>(
        session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

/** rct2: 0x008ACB18 */
template<bool isClassic>
static void WoodenRCTrackLeftHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = DirectionPrev(direction);
    }
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    WoodenRCTrackRightHalfBankedHelixUpLarge<isClassic>(
        session, ride, trackSequence, DirectionNext(direction), height, trackElement, supportType);
}

/** rct2: 0x008ACB28 */
template<bool isClassic>
static void WoodenRCTrackRightHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = DirectionNext(direction);
    }
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    WoodenRCTrackLeftHalfBankedHelixUpLarge<isClassic>(
        session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

/** rct2: 0x008ACB98 */
template<bool isClassic>
static void WoodenRCTrackLeftQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilLeftQuarterTurn1TileTunnel(
        session, kTunnelGroup, direction, height, -8, TunnelSubType::SlopeStart, +56, TunnelSubType::SlopeEnd);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x008ACB78 */
template<bool isClassic>
static void WoodenRCTrackRightQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilRightQuarterTurn1TileTunnel(
        session, kTunnelGroup, direction, height, -8, TunnelSubType::SlopeStart, +56, TunnelSubType::SlopeEnd);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x008ACB88 */
template<bool isClassic>
static void WoodenRCTrackLeftQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackRightQuarterTurn160DegUp<isClassic>(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACBA8 */
template<bool isClassic>
static void WoodenRCTrackRightQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackLeftQuarterTurn160DegUp<isClassic>(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC868 */
template<bool isClassic>
static void WoodenRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteSupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008ACC78 */
template<bool isClassic>
static void WoodenRCTrack25DegUpLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::Up25LeftBanked>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACC88 */
template<bool isClassic>
static void WoodenRCTrack25DegUpRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::Up25RightBanked>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008AC878 */
template<bool isClassic>
static void WoodenRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteSupportColoursWithChildOnRidePhoto(
        session, ride, trackSequence, direction, height, trackElement, supportType);
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, TunnelType::InvertedSquare);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

template<bool isClassic>
static void WoodenRCTrackFlatTo60DegUpLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::FlatToUp60LongBase>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
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

template<bool isClassic>
static void WoodenRCTrack60DegUpToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::Up60ToFlatLongBase>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
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

template<bool isClassic>
static void WoodenRCTrackFlatTo60DegDownLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack60DegUpToFlatLongBase<isClassic>(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrack60DegDownToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatTo60DegUpLongBase<isClassic>(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACC98 */
template<bool isClassic>
static void WoodenRCTrack25DegDownLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpRightBanked<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACCA8 */
template<bool isClassic>
static void WoodenRCTrack25DegDownRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpLeftBanked<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACE08 */
template<bool isClassic>
static void WoodenRCTrackWaterSplash(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintWatersplashSupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::Watersplash>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AC958 */
template<bool isClassic>
static void WoodenRCTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftEighthToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008AC968 */
template<bool isClassic>
static void WoodenRCTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightEighthToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008AC978 */
template<bool isClassic>
static void WoodenRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    WoodenRCTrackRightEighthToDiag<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC988 */
template<bool isClassic>
static void WoodenRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    WoodenRCTrackLeftEighthToDiag<isClassic>(
        session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AC9B8 */
template<bool isClassic>
static void WoodenRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    WoodenRCTrackRightEighthBankToDiag<isClassic>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AC9C8 */
template<bool isClassic>
static void WoodenRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    WoodenRCTrackLeftEighthBankToDiag<isClassic>(
        session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrackDiagBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagBrakes>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(kSegmentsAll, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

template<bool isClassic>
static void WoodenRCTrackDiagBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagBlockBrakes>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(kSegmentsAll, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AC888 */
template<bool isClassic>
static void WoodenRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChildChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AC8B8 */
template<bool isClassic>
static void WoodenRCTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChildChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AC8E8 */
template<bool isClassic>
static void WoodenRCTrackDiag60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp60>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x008AC898 */
template<bool isClassic>
static void WoodenRCTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChildChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagFlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AC8C8 */
template<bool isClassic>
static void WoodenRCTrackDiag25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp25ToUp60>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008AC8D8 */
template<bool isClassic>
static void WoodenRCTrackDiag60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp60ToUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008AC8A8 */
template<bool isClassic>
static void WoodenRCTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChildChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AC918 */
template<bool isClassic>
static void WoodenRCTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackDiag25DegUp<isClassic>(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AC948 */
template<bool isClassic>
static void WoodenRCTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackDiag60DegUp<isClassic>(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AC8F8 */
template<bool isClassic>
static void WoodenRCTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackDiag25DegUpToFlat<isClassic>(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AC928 */
template<bool isClassic>
static void WoodenRCTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackDiag60DegUpTo25DegUp<isClassic>(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AC938 */
template<bool isClassic>
static void WoodenRCTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackDiag25DegUp<isClassic>(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AC908 */
template<bool isClassic>
static void WoodenRCTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackDiagFlatTo25DegUp<isClassic>(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagFlatToBank<isClassic>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagFlatToBank<isClassic>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void WoodenRCTrackDiagLeftBankToDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagUp25ToBank<false>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void WoodenRCTrackDiagRightBankToDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagUp25ToBank<false>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrackDiagDown25ToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagBankTo25DegUp<isClassic>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrackDiagDown25ToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagBankTo25DegUp<isClassic>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AC9E8 */
template<bool isClassic>
static void WoodenRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackDiagLeftBank<isClassic>(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008ACB58 */
template<bool isClassic>
static void WoodenRCTrackLeftQuarterTurn325DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WoodenRCTrackRightBankToRightQuarterTurn325DegUp<isClassic>(
        session, ride, trackSequence, DirectionNext(direction), height, trackElement, supportType);
}

/** rct2: 0x008ACB68 */
template<bool isClassic>
static void WoodenRCTrackRightQuarterTurn325DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WoodenRCTrackLeftBankToLeftQuarterTurn325DegUp<isClassic>(
        session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

/** rct2: 0x008ACDF8 */
template<bool isClassic>
static void WoodenRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteSupportColoursWithChildBrake(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::BlockBrakes>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008ACCB8 */
template<bool isClassic>
static void WoodenRCTrackLeftBankedQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftBankedQuarterTurn3TileUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
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

/** rct2: 0x008ACCC8 */
template<bool isClassic>
static void WoodenRCTrackRightBankedQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightBankedQuarterTurn3TileUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
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

/** rct2: 0x008ACCD8 */
template<bool isClassic>
static void WoodenRCTrackLeftBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WoodenRCTrackRightBankedQuarterTurn325DegUp<isClassic>(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACCE8 */
template<bool isClassic>
static void WoodenRCTrackRightBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WoodenRCTrackLeftBankedQuarterTurn325DegUp<isClassic>(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACC38 */
template<bool isClassic>
static void WoodenRCTrackLeftBankedQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftBankedQuarterTurn5TileUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
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

/** rct2: 0x008ACC48 */
template<bool isClassic>
static void WoodenRCTrackRightBankedQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightBankedQuarterTurn5TileUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (trackSequence)
    {
        case 0:
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
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

/** rct2: 0x008ACC58 */
template<bool isClassic>
static void WoodenRCTrackLeftBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    WoodenRCTrackRightBankedQuarterTurn525DegUp<isClassic>(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACC68 */
template<bool isClassic>
static void WoodenRCTrackRightBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    WoodenRCTrackLeftBankedQuarterTurn525DegUp<isClassic>(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACCF8 */
template<bool isClassic>
static void WoodenRCTrack25DegUpToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::Up25ToLeftBankedUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACD08 */
template<bool isClassic>
static void WoodenRCTrack25DegUpToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::Up25ToRightBankedUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACD18 */
template<bool isClassic>
static void WoodenRCTrackLeftBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteSupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftBankedUp25ToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACD28 */
template<bool isClassic>
static void WoodenRCTrackRightBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteSupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightBankedUp25ToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACD38 */
template<bool isClassic>
static void WoodenRCTrack25DegDownToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackRightBanked25DegUpTo25DegUp<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACD48 */
template<bool isClassic>
static void WoodenRCTrack25DegDownToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackLeftBanked25DegUpTo25DegUp<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACD58 */
template<bool isClassic>
static void WoodenRCTrackLeftBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpToRightBanked25DegUp<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACD68 */
template<bool isClassic>
static void WoodenRCTrackRightBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpToLeftBanked25DegUp<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACD78 */
template<bool isClassic>
static void WoodenRCTrackLeftBankedFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftBankedFlatToLeftBankedUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACD88 */
template<bool isClassic>
static void WoodenRCTrackRightBankedFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightBankedFlatToRightBankedUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACD98 */
template<bool isClassic>
static void WoodenRCTrackLeftBanked25DegUpToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftBankedUp25ToLeftBankedFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACDA8 */
template<bool isClassic>
static void WoodenRCTrackRightBanked25DegUpToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightBankedUp25ToRightBankedFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACDB8 */
template<bool isClassic>
static void WoodenRCTrackLeftBankedFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackRightBanked25DegUpToRightBankedFlat<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACDC8 */
template<bool isClassic>
static void WoodenRCTrackRightBankedFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackLeftBanked25DegUpToLeftBankedFlat<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACDD8 */
template<bool isClassic>
static void WoodenRCTrackLeftBanked25DegDownToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackRightBankedFlatToRightBanked25DegUp<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACDE8 */
template<bool isClassic>
static void WoodenRCTrackRightBanked25DegDownToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackLeftBankedFlatToLeftBanked25DegUp<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACBB8 */
template<bool isClassic>
static void WoodenRCTrackFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::FlatToLeftBankedUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACBC8 */
template<bool isClassic>
static void WoodenRCTrackFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::FlatToRightBankedUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACBD8 */
template<bool isClassic>
static void WoodenRCTrackLeftBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftBankedUp25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACBE8 */
template<bool isClassic>
static void WoodenRCTrackRightBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2SupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightBankedUp25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

/** rct2: 0x008ACBF8 */
template<bool isClassic>
static void WoodenRCTrackFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackRightBanked25DegUpToFlat<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACC08 */
template<bool isClassic>
static void WoodenRCTrackFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackLeftBanked25DegUpToFlat<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACC18 */
template<bool isClassic>
static void WoodenRCTrackLeftBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatToRightBanked25DegUp<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008ACC28 */
template<bool isClassic>
static void WoodenRCTrackRightBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatToLeftBanked25DegUp<isClassic>(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrackBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteSupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<OpenRCT2::TrackElemType::Booster>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

template<bool isClassic>
static void WoodenRCTrackLeftMediumHalfLoopUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(
                session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 7, height, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::top),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::TopLeftSide, 14, height, session.SupportColours);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::TopRightSide, 16, height, session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::BottomRightSide, 18, height,
                        session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::BottomLeftSide, 14, height,
                        session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 144);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 144);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottom,
                        PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

template<bool isClassic>
static void WoodenRCTrackRightMediumHalfLoopUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(
                session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 7, height, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottom,
                        PaintSegment::bottomRight, PaintSegment::right),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::BottomRightSide, 14, height,
                        session.SupportColours);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::BottomLeftSide, 18, height,
                        session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::TopLeftSide, 16, height, session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::TopRightSide, 14, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 144);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 144);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::top),
                    direction),
                0xFFFF, 0);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

template<bool isClassic>
static void WoodenRCTrackLeftMediumHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackRightMediumHalfLoopUp<isClassic>(
        session, ride, 4 - trackSequence, direction, height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrackRightMediumHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackLeftMediumHalfLoopUp<isClassic>(
        session, ride, 4 - trackSequence, direction, height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrackLeftLargeHalfLoopUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(
                session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 5, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 12, height, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 6, height, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
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
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::TopLeftSide, 22, height, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::TopRightSide, 14, height + 8,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::BottomRightSide, 0, height + 21,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::BottomLeftSide, 22, height,
                        session.SupportColours);
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
            PaintUtilSetGeneralSupportHeight(session, height + 224);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 128);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 224);
            break;
        case 6:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
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

template<bool isClassic>
static void WoodenRCTrackRightLargeHalfLoopUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(
                session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 5, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 0, height, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 6, height, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::Centre, 12, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
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
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::BottomRightSide, 22, height,
                        session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::BottomLeftSide, 0, height + 21,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::TopLeftSide, 14, height + 8,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportType::Boxed, MetalSupportPlace::TopRightSide, 22, height, session.SupportColours);
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
            PaintUtilSetGeneralSupportHeight(session, height + 224);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 128);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 224);
            break;
        case 6:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

template<bool isClassic>
static void WoodenRCTrackLeftLargeHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackRightLargeHalfLoopUp<isClassic>(
        session, ride, 6 - trackSequence, direction, height, trackElement, supportType);
}

template<bool isClassic>
static void WoodenRCTrackRightLargeHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackLeftLargeHalfLoopUp<isClassic>(session, ride, 6 - trackSequence, direction, height, trackElement, supportType);
}

template<bool isClassic>
TrackPaintFunction GetTrackPaintFunctionWoodenAndClassicWoodenRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return WoodenRCTrackFlat<isClassic>;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return WoodenRCTrackStation<isClassic>;
        case TrackElemType::Up25:
            return WoodenRCTrack25DegUp<isClassic>;
        case TrackElemType::Up60:
            return WoodenRCTrack60DegUp<isClassic>;
        case TrackElemType::FlatToUp25:
            return WoodenRCTrackFlatTo25DegUp<isClassic>;
        case TrackElemType::Up25ToUp60:
            return WoodenRCTrack25DegUpTo60DegUp<isClassic>;
        case TrackElemType::Up60ToUp25:
            return WoodenRCTrack60DegUpTo25DegUp<isClassic>;
        case TrackElemType::Up25ToFlat:
            return WoodenRCTrack25DegUpToFlat<isClassic>;
        case TrackElemType::Down25:
            return WoodenRCTrack25DegDown<isClassic>;
        case TrackElemType::Down60:
            return WoodenRCTrack60DegDown<isClassic>;
        case TrackElemType::FlatToDown25:
            return WoodenRCTrackFlatTo25DegDown<isClassic>;
        case TrackElemType::Down25ToDown60:
            return WoodenRCTrack25DegDownTo60DegDown<isClassic>;
        case TrackElemType::Down60ToDown25:
            return WoodenRCTrack60DegDownTo25DegDown<isClassic>;
        case TrackElemType::Down25ToFlat:
            return WoodenRCTrack25DegDownToFlat<isClassic>;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return WoodenRCTrackLeftQuarterTurn5<isClassic>;
        case TrackElemType::RightQuarterTurn5Tiles:
            return WoodenRCTrackRightQuarterTurn5<isClassic>;
        case TrackElemType::FlatToLeftBank:
            return WoodenRCTrackFlatToBank<isClassic>;
        case TrackElemType::FlatToRightBank:
            return WoodenRCTrackFlatToBank<isClassic>;
        case TrackElemType::LeftBankToFlat:
            return WoodenRCTrackLeftBankToFlat<isClassic>;
        case TrackElemType::RightBankToFlat:
            return WoodenRCTrackRightBankToFlat<isClassic>;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return WoodenRCTrackBankedLeftQuarterTurn5<isClassic>;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return WoodenRCTrackBankedRightQuarterTurn5<isClassic>;
        case TrackElemType::LeftBankToUp25:
            return WoodenRCTrackBankTo25DegUp<isClassic>;
        case TrackElemType::RightBankToUp25:
            return WoodenRCTrackBankTo25DegUp<isClassic>;
        case TrackElemType::Up25ToLeftBank:
            return WoodenRCTrack25DegUpToBank<isClassic>;
        case TrackElemType::Up25ToRightBank:
            return WoodenRCTrack25DegUpToBank<isClassic>;
        case TrackElemType::LeftBankToDown25:
            return WoodenRCTrackLeftBankTo25DegDown<isClassic>;
        case TrackElemType::RightBankToDown25:
            return WoodenRCTrackRightBankTo25DegDown<isClassic>;
        case TrackElemType::Down25ToLeftBank:
            return WoodenRCTrack25DegDownToLeftBank<isClassic>;
        case TrackElemType::Down25ToRightBank:
            return WoodenRCTrack25DegDownToRightBank<isClassic>;
        case TrackElemType::LeftBank:
            return WoodenRCTrackFlatToBank<isClassic>;
        case TrackElemType::RightBank:
            return WoodenRCTrackRightBank<isClassic>;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return WoodenRCTrackLeftQuarterTurn525DegUp<isClassic>;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return WoodenRCTrackRightQuarterTurn525DegUp<isClassic>;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return WoodenRCTrackLeftQuarterTurn525DegDown<isClassic>;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return WoodenRCTrackRightQuarterTurn525DegDown<isClassic>;
        case TrackElemType::SBendLeft:
            return WoodenRCTrackSBendLeft<isClassic>;
        case TrackElemType::SBendRight:
            return WoodenRCTrackSBendRight<isClassic>;
        case TrackElemType::LeftVerticalLoop:
            return WoodenRCTrackLeftVerticalLoop;
        case TrackElemType::RightVerticalLoop:
            return WoodenRCTrackRightVerticalLoop;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return WoodenRCTrackLeftQuarterTurn3<isClassic>;
        case TrackElemType::RightQuarterTurn3Tiles:
            return WoodenRCTrackRightQuarterTurn3<isClassic>;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return WoodenRCTrackLeftQuarterTurn3Bank<isClassic>;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return WoodenRCTrackRightQuarterTurn3Bank<isClassic>;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return WoodenRCTrackLeftQuarterTurn325DegUp<isClassic>;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return WoodenRCTrackRightQuarterTurn325DegUp<isClassic>;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return WoodenRCTrackLeftQuarterTurn325DegDown<isClassic>;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return WoodenRCTrackRightQuarterTurn325DegDown<isClassic>;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return WoodenRCTrackLeftHalfBankedHelixUpSmall<isClassic>;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return WoodenRCTrackRightHalfBankedHelixUpSmall<isClassic>;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return WoodenRCTrackLeftHalfBankedHelixDownSmall<isClassic>;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return WoodenRCTrackRightHalfBankedHelixDownSmall<isClassic>;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return WoodenRCTrackLeftHalfBankedHelixUpLarge<isClassic>;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return WoodenRCTrackRightHalfBankedHelixUpLarge<isClassic>;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return WoodenRCTrackLeftHalfBankedHelixDownLarge<isClassic>;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return WoodenRCTrackRightHalfBankedHelixDownLarge<isClassic>;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return WoodenRCTrackLeftQuarterTurn160DegUp<isClassic>;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return WoodenRCTrackRightQuarterTurn160DegUp<isClassic>;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return WoodenRCTrackLeftQuarterTurn160DegDown<isClassic>;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return WoodenRCTrackRightQuarterTurn160DegDown<isClassic>;
        case TrackElemType::Brakes:
            return WoodenRCTrackBrakes<isClassic>;
        case TrackElemType::Up25LeftBanked:
            return WoodenRCTrack25DegUpLeftBanked<isClassic>;
        case TrackElemType::Up25RightBanked:
            return WoodenRCTrack25DegUpRightBanked<isClassic>;
        case TrackElemType::OnRidePhoto:
            return WoodenRCTrackOnRidePhoto<isClassic>;
        case TrackElemType::FlatToUp60LongBase:
            return WoodenRCTrackFlatTo60DegUpLongBase<isClassic>;
        case TrackElemType::Up60ToFlatLongBase:
            return WoodenRCTrack60DegUpToFlatLongBase<isClassic>;
        case TrackElemType::FlatToDown60LongBase:
            return WoodenRCTrackFlatTo60DegDownLongBase<isClassic>;
        case TrackElemType::Down60ToFlatLongBase:
            return WoodenRCTrack60DegDownToFlatLongBase<isClassic>;
        case TrackElemType::Down25LeftBanked:
            return WoodenRCTrack25DegDownLeftBanked<isClassic>;
        case TrackElemType::Down25RightBanked:
            return WoodenRCTrack25DegDownRightBanked<isClassic>;
        case TrackElemType::Watersplash:
            return WoodenRCTrackWaterSplash<isClassic>;
        case TrackElemType::LeftEighthToDiag:
            return WoodenRCTrackLeftEighthToDiag<isClassic>;
        case TrackElemType::RightEighthToDiag:
            return WoodenRCTrackRightEighthToDiag<isClassic>;
        case TrackElemType::LeftEighthToOrthogonal:
            return WoodenRCTrackLeftEighthToOrthogonal<isClassic>;
        case TrackElemType::RightEighthToOrthogonal:
            return WoodenRCTrackRightEighthToOrthogonal<isClassic>;
        case TrackElemType::LeftEighthBankToDiag:
            return WoodenRCTrackLeftEighthBankToDiag<isClassic>;
        case TrackElemType::RightEighthBankToDiag:
            return WoodenRCTrackRightEighthBankToDiag<isClassic>;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return WoodenRCTrackLeftEighthBankToOrthogonal<isClassic>;
        case TrackElemType::RightEighthBankToOrthogonal:
            return WoodenRCTrackRightEighthBankToOrthogonal<isClassic>;
        case TrackElemType::DiagFlat:
            return WoodenRCTrackDiagFlat<isClassic>;
        case TrackElemType::DiagUp25:
            return WoodenRCTrackDiag25DegUp<isClassic>;
        case TrackElemType::DiagUp60:
            return WoodenRCTrackDiag60DegUp<isClassic>;
        case TrackElemType::DiagFlatToUp25:
            return WoodenRCTrackDiagFlatTo25DegUp<isClassic>;
        case TrackElemType::DiagUp25ToUp60:
            return WoodenRCTrackDiag25DegUpTo60DegUp<isClassic>;
        case TrackElemType::DiagUp60ToUp25:
            return WoodenRCTrackDiag60DegUpTo25DegUp<isClassic>;
        case TrackElemType::DiagUp25ToFlat:
            return WoodenRCTrackDiag25DegUpToFlat<isClassic>;
        case TrackElemType::DiagDown25:
            return WoodenRCTrackDiag25DegDown<isClassic>;
        case TrackElemType::DiagDown60:
            return WoodenRCTrackDiag60DegDown<isClassic>;
        case TrackElemType::DiagFlatToDown25:
            return WoodenRCTrackDiagFlatTo25DegDown<isClassic>;
        case TrackElemType::DiagDown25ToDown60:
            return WoodenRCTrackDiag25DegDownTo60DegDown<isClassic>;
        case TrackElemType::DiagDown60ToDown25:
            return WoodenRCTrackDiag60DegDownTo25DegDown<isClassic>;
        case TrackElemType::DiagDown25ToFlat:
            return WoodenRCTrackDiag25DegDownToFlat<isClassic>;
        case TrackElemType::DiagFlatToLeftBank:
            return WoodenRCTrackDiagFlatToBank<isClassic>;
        case TrackElemType::DiagFlatToRightBank:
            return WoodenRCTrackDiagFlatToBank<isClassic>;
        case TrackElemType::DiagLeftBankToFlat:
            return WoodenRCTrackDiagLeftBankToFlat<isClassic>;
        case TrackElemType::DiagRightBankToFlat:
            return WoodenRCTrackDiagRightBankToFlat<isClassic>;
        case TrackElemType::DiagLeftBankToUp25:
            return WoodenRCTrackDiagBankTo25DegUp<isClassic>;
        case TrackElemType::DiagRightBankToUp25:
            return WoodenRCTrackDiagBankTo25DegUp<isClassic>;
        case TrackElemType::DiagUp25ToLeftBank:
            return WoodenRCTrackDiagUp25ToBank<isClassic>;
        case TrackElemType::DiagUp25ToRightBank:
            return WoodenRCTrackDiagUp25ToBank<isClassic>;
        case TrackElemType::DiagLeftBankToDown25:
            return WoodenRCTrackDiagLeftBankToDown25;
        case TrackElemType::DiagRightBankToDown25:
            return WoodenRCTrackDiagRightBankToDown25;
        case TrackElemType::DiagDown25ToLeftBank:
            return WoodenRCTrackDiagDown25ToLeftBank<isClassic>;
        case TrackElemType::DiagDown25ToRightBank:
            return WoodenRCTrackDiagDown25ToRightBank<isClassic>;
        case TrackElemType::DiagLeftBank:
            return WoodenRCTrackDiagLeftBank<isClassic>;
        case TrackElemType::DiagRightBank:
            return WoodenRCTrackDiagRightBank<isClassic>;
        case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
            return WoodenRCTrackLeftBankToLeftQuarterTurn325DegUp<isClassic>;
        case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
            return WoodenRCTrackRightBankToRightQuarterTurn325DegUp<isClassic>;
        case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
            return WoodenRCTrackLeftQuarterTurn325DegDownToLeftBank<isClassic>;
        case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
            return WoodenRCTrackRightQuarterTurn325DegDownToRightBank<isClassic>;
        case TrackElemType::BlockBrakes:
            return WoodenRCTrackBlockBrakes<isClassic>;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
            return WoodenRCTrackLeftBankedQuarterTurn325DegUp<isClassic>;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
            return WoodenRCTrackRightBankedQuarterTurn325DegUp<isClassic>;
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            return WoodenRCTrackLeftBankedQuarterTurn325DegDown<isClassic>;
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            return WoodenRCTrackRightBankedQuarterTurn325DegDown<isClassic>;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
            return WoodenRCTrackLeftBankedQuarterTurn525DegUp<isClassic>;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
            return WoodenRCTrackRightBankedQuarterTurn525DegUp<isClassic>;
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            return WoodenRCTrackLeftBankedQuarterTurn525DegDown<isClassic>;
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            return WoodenRCTrackRightBankedQuarterTurn525DegDown<isClassic>;
        case TrackElemType::Up25ToLeftBankedUp25:
            return WoodenRCTrack25DegUpToLeftBanked25DegUp<isClassic>;
        case TrackElemType::Up25ToRightBankedUp25:
            return WoodenRCTrack25DegUpToRightBanked25DegUp<isClassic>;
        case TrackElemType::LeftBankedUp25ToUp25:
            return WoodenRCTrackLeftBanked25DegUpTo25DegUp<isClassic>;
        case TrackElemType::RightBankedUp25ToUp25:
            return WoodenRCTrackRightBanked25DegUpTo25DegUp<isClassic>;
        case TrackElemType::Down25ToLeftBankedDown25:
            return WoodenRCTrack25DegDownToLeftBanked25DegDown<isClassic>;
        case TrackElemType::Down25ToRightBankedDown25:
            return WoodenRCTrack25DegDownToRightBanked25DegDown<isClassic>;
        case TrackElemType::LeftBankedDown25ToDown25:
            return WoodenRCTrackLeftBanked25DegDownTo25DegDown<isClassic>;
        case TrackElemType::RightBankedDown25ToDown25:
            return WoodenRCTrackRightBanked25DegDownTo25DegDown<isClassic>;
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
            return WoodenRCTrackLeftBankedFlatToLeftBanked25DegUp<isClassic>;
        case TrackElemType::RightBankedFlatToRightBankedUp25:
            return WoodenRCTrackRightBankedFlatToRightBanked25DegUp<isClassic>;
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
            return WoodenRCTrackLeftBanked25DegUpToLeftBankedFlat<isClassic>;
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
            return WoodenRCTrackRightBanked25DegUpToRightBankedFlat<isClassic>;
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
            return WoodenRCTrackLeftBankedFlatToLeftBanked25DegDown<isClassic>;
        case TrackElemType::RightBankedFlatToRightBankedDown25:
            return WoodenRCTrackRightBankedFlatToRightBanked25DegDown<isClassic>;
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
            return WoodenRCTrackLeftBanked25DegDownToLeftBankedFlat<isClassic>;
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
            return WoodenRCTrackRightBanked25DegDownToRightBankedFlat<isClassic>;
        case TrackElemType::FlatToLeftBankedUp25:
            return WoodenRCTrackFlatToLeftBanked25DegUp<isClassic>;
        case TrackElemType::FlatToRightBankedUp25:
            return WoodenRCTrackFlatToRightBanked25DegUp<isClassic>;
        case TrackElemType::LeftBankedUp25ToFlat:
            return WoodenRCTrackLeftBanked25DegUpToFlat<isClassic>;
        case TrackElemType::RightBankedUp25ToFlat:
            return WoodenRCTrackRightBanked25DegUpToFlat<isClassic>;
        case TrackElemType::FlatToLeftBankedDown25:
            return WoodenRCTrackFlatToLeftBanked25DegDown<isClassic>;
        case TrackElemType::FlatToRightBankedDown25:
            return WoodenRCTrackFlatToRightBanked25DegDown<isClassic>;
        case TrackElemType::LeftBankedDown25ToFlat:
            return WoodenRCTrackLeftBanked25DegDownToFlat<isClassic>;
        case TrackElemType::RightBankedDown25ToFlat:
            return WoodenRCTrackRightBanked25DegDownToFlat<isClassic>;
        case TrackElemType::Booster:
            return WoodenRCTrackBooster<isClassic>;
        case TrackElemType::DiagBrakes:
            return WoodenRCTrackDiagBrakes<isClassic>;
        case TrackElemType::DiagBlockBrakes:
            return WoodenRCTrackDiagBlockBrakes<isClassic>;
        case TrackElemType::LeftMediumHalfLoopUp:
            return WoodenRCTrackLeftMediumHalfLoopUp<isClassic>;
        case TrackElemType::RightMediumHalfLoopUp:
            return WoodenRCTrackRightMediumHalfLoopUp<isClassic>;
        case TrackElemType::LeftMediumHalfLoopDown:
            return WoodenRCTrackLeftMediumHalfLoopDown<isClassic>;
        case TrackElemType::RightMediumHalfLoopDown:
            return WoodenRCTrackRightMediumHalfLoopDown<isClassic>;
        case TrackElemType::LeftLargeHalfLoopUp:
            return WoodenRCTrackLeftLargeHalfLoopUp<isClassic>;
        case TrackElemType::RightLargeHalfLoopUp:
            return WoodenRCTrackRightLargeHalfLoopUp<isClassic>;
        case TrackElemType::LeftLargeHalfLoopDown:
            return WoodenRCTrackLeftLargeHalfLoopDown<isClassic>;
        case TrackElemType::RightLargeHalfLoopDown:
            return WoodenRCTrackRightLargeHalfLoopDown<isClassic>;
        default:
            return TrackPaintFunctionDummy;
    }
}

TrackPaintFunction GetTrackPaintFunctionWoodenRC(OpenRCT2::TrackElemType trackType)
{
    return GetTrackPaintFunctionWoodenAndClassicWoodenRC<false>(trackType);
}
