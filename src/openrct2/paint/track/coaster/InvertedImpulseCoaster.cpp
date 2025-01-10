/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Inverted;

/** rct2: 0x008B0460 */
static void InvertedImpulseRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19662), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19663), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height + 44, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008B0470, 0x008B0480, 0x008B0490 */
static void InvertedImpulseRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][3] = {
        { SPR_STATION_BASE_C_SW_NE, 19662, SPR_STATION_INVERTED_BAR_B_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 19663, SPR_STATION_INVERTED_BAR_B_NW_SE },
        { SPR_STATION_BASE_C_SW_NE, 19662, SPR_STATION_INVERTED_BAR_B_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 19663, SPR_STATION_INVERTED_BAR_B_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][0]), { 0, 0, height },
        { { 0, 2, height }, { 32, 28, 1 } });
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(imageIds[direction][1]), { 0, 0, height + 29 },
        { { 0, 6, height + 29 }, { 32, 20, 3 } });
    PaintAddImageAsChildRotated(
        session, direction, session.SupportColours.WithIndex(imageIds[direction][2]), { 0, 6, height + 29 },
        { { 0, 6, height + 29 }, { 32, 20, 3 } });
    DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);
    TrackPaintUtilDrawStationInverted(session, ride, direction, height, trackElement, STATION_VARIANT_TALL);
    TrackPaintUtilDrawStationTunnelTall(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008B04A0 */
static void InvertedImpulseRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19672), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19673), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19674), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19675), { 0, 0, height + 29 },
                { { 0, 6, height + 45 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopRightSide, 0, height + 62, session.SupportColours);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomRightSide, 0, height + 62, session.SupportColours);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomLeftSide, 0, height + 62, session.SupportColours);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopLeftSide, 0, height + 62, session.SupportColours);
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
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008B04B0 */
static void InvertedImpulseRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19688), { 0, 0, height + 29 },
                { { 0, 6, height + 93 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19689), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 81 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19690), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 81 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19691), { 0, 0, height + 29 },
                { { 0, 6, height + 93 }, { 32, 20, 3 } });
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
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 120);
}

/** rct2: 0x008B04C0 */
static void InvertedImpulseRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19664), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19665), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19666), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19667), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    switch (direction)
    {
        case 0:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::TopRightSide, 0, height + 54, session.SupportColours);
            break;
        case 1:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::BottomRightSide, 0, height + 54, session.SupportColours);
            break;
        case 2:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::BottomLeftSide, 0, height + 54, session.SupportColours);
            break;
        case 3:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::TopLeftSide, 0, height + 54, session.SupportColours);
            break;
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

/** rct2: 0x008B04D0 */
static void InvertedImpulseRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19676), { 0, 0, height + 29 },
                { { 0, 6, height + 61 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19680), { 0, 0, height + 29 },
                { { 0, 10, height + 11 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19677), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 49 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19681), { 0, 0, height + 29 },
                { { 0, 10, height + 11 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19678), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 49 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19679), { 0, 0, height + 29 },
                { { 0, 6, height + 61 }, { 32, 20, 3 } });
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
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 88);
}

/** rct2: 0x008B04E0 */
static void InvertedImpulseRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19682), { 0, 0, height + 29 },
                { { 0, 6, height + 61 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19686), { 0, 0, height + 29 },
                { { 0, 10, height + 11 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19683), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 49 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19687), { 0, 0, height + 29 },
                { { 0, 10, height + 11 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19684), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 49 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19685), { 0, 0, height + 29 },
                { { 0, 6, height + 61 }, { 32, 20, 3 } });
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
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 88);
}

/** rct2: 0x008B04F0 */
static void InvertedImpulseRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19668), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19669), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19670), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19671), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
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

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008B0500 */
static void InvertedImpulseRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0510 */
static void InvertedImpulseRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0520 */
static void InvertedImpulseRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0530 */
static void InvertedImpulseRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack60DegUpTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0540 */
static void InvertedImpulseRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack25DegUpTo60DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0550 */
static void InvertedImpulseRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B05A0 */
static void InvertedImpulseRCTrack90DegUp(
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
                        session, direction, session.TrackColours.WithIndex(19700), { 0, 0, height + 29 },
                        { { 0, 6, height + 61 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19701), { 0, 0, height + 29 },
                        { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19702), { 0, 0, height + 29 },
                        { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19703), { 0, 0, height + 29 },
                        { { 0, 6, height + 61 }, { 32, 20, 3 } });
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

/** rct2: 0x008B05B0 */
static void InvertedImpulseRCTrack90DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0560 */
static void InvertedImpulseRCTrack60DegUpTo90DegUp(
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
                        session, direction, session.TrackColours.WithIndex(19692), { 0, 0, height + 29 },
                        { { 0, 6, height + 85 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19693), { 0, 0, height + 29 },
                        { { 0, 4, height + 11 }, { 32, 2, 55 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19694), { 0, 0, height + 29 },
                        { { 0, 4, height + 11 }, { 32, 2, 55 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19695), { 0, 0, height + 29 },
                        { { 0, 6, height + 85 }, { 32, 20, 3 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
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

/** rct2: 0x008B0570 */
static void InvertedImpulseRCTrack90DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack60DegUpTo90DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0580 */
static void InvertedImpulseRCTrack90DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19696), { 0, 0, height + 29 },
                { { 0, 6, height + 85 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19697), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 55 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19698), { 0, 0, height + 29 },
                { { 0, 4, height + 11 }, { 32, 2, 55 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(19699), { 0, 0, height + 29 },
                { { 0, 6, height + 85 }, { 32, 20, 3 } });
            break;
    }
    switch (direction)
    {
        case 1:
            PaintUtilPushTunnelRight(session, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 96);
}

/** rct2: 0x008B0590 */
static void InvertedImpulseRCTrack60DegDownTo90DegDown(
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
                        session, direction, session.TrackColours.WithIndex(19698), { 0, 0, height + 29 },
                        { { 0, 4, height + 11 }, { 32, 2, 55 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19699), { 0, 0, height + 29 },
                        { { 0, 6, height + 85 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19696), { 0, 0, height + 29 },
                        { { 0, 6, height + 85 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19697), { 0, 0, height + 29 },
                        { { 0, 4, height + 11 }, { 32, 2, 55 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B05C0 */
static void InvertedImpulseRCTrackLeftQuarterTurn190DegUp(
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
                        session, direction, session.TrackColours.WithIndex(19708), { 0, 0, height + 29 },
                        { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19709), { 0, 0, height + 29 },
                        { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19717), { 0, 0, height + 29 },
                        { { 4, 0, height + 11 }, { 2, 32, 31 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19710), { 0, 0, height + 29 },
                        { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19718), { 0, 0, height + 29 },
                        { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19711), { 0, 0, height + 29 },
                        { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19719), { 0, 0, height + 29 },
                        { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    break;
            }
            PaintUtilSetVerticalTunnel(session, height + 96);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B05D0 */
static void InvertedImpulseRCTrackRightQuarterTurn190DegUp(
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
                        session, direction, session.TrackColours.WithIndex(19704), { 0, 0, height + 29 },
                        { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19712), { 0, 0, height + 29 },
                        { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19705), { 0, 0, height + 29 },
                        { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19713), { 0, 0, height + 29 },
                        { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19706), { 0, 0, height + 29 },
                        { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19714), { 0, 0, height + 29 },
                        { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(19707), { 0, 0, height + 29 },
                        { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    break;
            }
            PaintUtilSetVerticalTunnel(session, height + 96);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B05E0 */
static void InvertedImpulseRCTrackLeftQuarterTurn190DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrackRightQuarterTurn190DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B05F0 */
static void InvertedImpulseRCTrackRightQuarterTurn190DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrackLeftQuarterTurn190DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

TrackPaintFunction GetTrackPaintFunctionInvertedImpulseRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return InvertedImpulseRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return InvertedImpulseRCTrackStation;
        case TrackElemType::Up25:
            return InvertedImpulseRCTrack25DegUp;
        case TrackElemType::Up60:
            return InvertedImpulseRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return InvertedImpulseRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return InvertedImpulseRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return InvertedImpulseRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return InvertedImpulseRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return InvertedImpulseRCTrack25DegDown;
        case TrackElemType::Down60:
            return InvertedImpulseRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return InvertedImpulseRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return InvertedImpulseRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return InvertedImpulseRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return InvertedImpulseRCTrack25DegDownToFlat;
        case TrackElemType::Up90:
            return InvertedImpulseRCTrack90DegUp;
        case TrackElemType::Down90:
            return InvertedImpulseRCTrack90DegDown;
        case TrackElemType::Up60ToUp90:
            return InvertedImpulseRCTrack60DegUpTo90DegUp;
        case TrackElemType::Down90ToDown60:
            return InvertedImpulseRCTrack90DegDownTo60DegDown;
        case TrackElemType::Up90ToUp60:
            return InvertedImpulseRCTrack90DegUpTo60DegUp;
        case TrackElemType::Down60ToDown90:
            return InvertedImpulseRCTrack60DegDownTo90DegDown;
        case TrackElemType::LeftQuarterTurn1TileUp90:
            return InvertedImpulseRCTrackLeftQuarterTurn190DegUp;
        case TrackElemType::RightQuarterTurn1TileUp90:
            return InvertedImpulseRCTrackRightQuarterTurn190DegUp;
        case TrackElemType::LeftQuarterTurn1TileDown90:
            return InvertedImpulseRCTrackLeftQuarterTurn190DegDown;
        case TrackElemType::RightQuarterTurn1TileDown90:
            return InvertedImpulseRCTrackRightQuarterTurn190DegDown;
        default:
            return TrackPaintFunctionDummy;
    }
}
