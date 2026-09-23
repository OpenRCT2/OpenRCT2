/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/ted/TrackElemType.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../track/Segment.h"

#include <cstdint>

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::square;
static constexpr uint8_t kSupportHeight = 39;
static constexpr uint8_t kInvertedSpriteOffset = 24;

static constexpr const uint32_t kInvertedFlyingRCDiagBrakeImages[kNumOrthogonalDirections] = {
    SPR_TRACKS_BM_INVERT_DIAG_BRAKES,
    SPR_TRACKS_BM_INVERT_DIAG_BRAKES + 1,
    SPR_TRACKS_BM_INVERT_DIAG_BRAKES,
    SPR_TRACKS_BM_INVERT_DIAG_BRAKES + 1,
};

/** rct2: 0x007C6FF4 */
static void InvertedFlyingRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.hasChain())
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27131), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 1 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27132), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 1 } });
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
                    session, direction, session.TrackColours.WithIndex(27129), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 1 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27130), { 0, 0, height + 24 },
                    { { 0, 6, height + 22 }, { 32, 20, 1 } });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007C7244, 0x007C7254, 0x007C7264 */
static void InvertedFlyingRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex imageIds[4][2] = {
        { 27131, SPR_STATION_INVERTED_BAR_C_SW_NE },
        { 27132, SPR_STATION_INVERTED_BAR_C_NW_SE },
        { 27131, SPR_STATION_INVERTED_BAR_C_SW_NE },
        { 27132, SPR_STATION_INVERTED_BAR_C_NW_SE },
    };
    const bool stationDrawn = TrackPaintUtilDrawStationInverted(
        session, ride, direction, height, trackElement, STATION_VARIANT_1);
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(imageIds[direction][0]), { 0, 0, height + 24 },
        { { 0, 6, height + 24 }, { 32, 20, 3 } });
    if (stationDrawn)
    {
        PaintAddImageAsChildRotated(
            session, direction, session.SupportColours.WithIndex(imageIds[direction][1]), { 0, 6, height + 24 },
            { { 0, 6, height + 24 }, { 32, 20, 3 } });
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);
    }
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007C7004 */
static void InvertedFlyingRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.hasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27249), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27250), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27251), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27252), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27221), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27222), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27223), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27224), { 0, 0, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 3 } });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 57, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x007C7014 */
static void InvertedFlyingRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27237), { 0, 0, height + 24 },
                { { 0, 6, height + 88 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27238), { 0, 0, height + 24 },
                { { 0, 4, height + 6 }, { 32, 2, 81 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27239), { 0, 0, height + 24 },
                { { 0, 4, height + 6 }, { 32, 2, 81 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27240), { 0, 0, height + 24 },
                { { 0, 6, height + 88 }, { 32, 20, 3 } });
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x007C7024 */
static void InvertedFlyingRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.hasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27241), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27242), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27243), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27244), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27213), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27214), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27215), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27216), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 49, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x007C7034 */
static void InvertedFlyingRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27225), { 0, 0, height + 24 },
                { { 0, 6, height + 56 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27229), { 0, 0, height + 24 },
                { { 0, 10, height + 6 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27226), { 0, 0, height + 24 },
                { { 0, 4, height + 6 }, { 32, 2, 49 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27230), { 0, 0, height + 24 },
                { { 0, 10, height + 6 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27227), { 0, 0, height + 24 },
                { { 0, 4, height + 6 }, { 32, 2, 49 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27228), { 0, 0, height + 24 },
                { { 0, 6, height + 56 }, { 32, 20, 3 } });
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x007C7044 */
static void InvertedFlyingRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27231), { 0, 0, height + 24 },
                { { 0, 6, height + 56 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27235), { 0, 0, height + 24 },
                { { 0, 10, height + 6 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27232), { 0, 0, height + 24 },
                { { 0, 4, height + 6 }, { 32, 2, 49 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27236), { 0, 0, height + 24 },
                { { 0, 10, height + 6 }, { 32, 10, 49 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27233), { 0, 0, height + 24 },
                { { 0, 4, height + 6 }, { 32, 2, 49 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27234), { 0, 0, height + 24 },
                { { 0, 6, height + 56 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 71, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x007C7054 */
static void InvertedFlyingRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.hasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27245), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27246), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27247), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27248), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27217), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27218), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27219), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(27220), { 0, 0, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 3 } });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 47, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::flatTo25Deg);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x007C7064 */
static void InvertedFlyingRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7074 */
static void InvertedFlyingRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7084 */
static void InvertedFlyingRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7094 */
static void InvertedFlyingRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C70A4 */
static void InvertedFlyingRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C70B4 */
static void InvertedFlyingRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C70C4 */
static void InvertedFlyingRCTrackLeftQuarterTurn5(
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
                        session, direction, session.TrackColours.WithIndex(27142), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27147), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27152), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27137), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
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
                        session, direction, session.TrackColours.WithIndex(27141), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27146), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27151), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27136), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27140), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27145), { 0, 0, height + 24 },
                        { { 16, 16, height + 22 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27150), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27135), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
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
                        session, direction, session.TrackColours.WithIndex(27139), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27144), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27149), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27134), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27138), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27143), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27148), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27133), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C70D4 */
static void InvertedFlyingRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedFlyingRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C70E4 */
static void InvertedFlyingRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27269), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27270), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27271), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27272), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
              PaintSegment::bottomRight },
            direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007C70F4 */
static void InvertedFlyingRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27273), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27274), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27275), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27276), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
              PaintSegment::bottomLeft },
            direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007C7104 */
static void InvertedFlyingRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27275), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27276), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27273), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27274), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
              PaintSegment::bottomRight },
            direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007C7114 */
static void InvertedFlyingRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27271), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27272), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27269), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27270), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
              PaintSegment::bottomLeft },
            direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007C7124 */
static void InvertedFlyingRCTrackBankedLeftQuarterTurn5(
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
                        session, direction, session.TrackColours.WithIndex(27162), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27167), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27172), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27157), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
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
                        session, direction, session.TrackColours.WithIndex(27161), { 0, 0, height + 24 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27166), { 0, 0, height + 24 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27171), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27156), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27160), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27165), { 0, 0, height + 24 },
                        { { 16, 16, height + 24 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27170), { 0, 0, height + 24 },
                        { { 16, 0, height + 24 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27155), { 0, 0, height + 24 }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
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
                        session, direction, session.TrackColours.WithIndex(27159), { 0, 0, height + 24 },
                        { { 16, 0, height + 24 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27164), { 0, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27169), { 0, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27154), { 0, 0, height + 24 },
                        { { 16, 0, height + 24 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27158), { 0, 0, height + 24 },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27163), { 0, 0, height + 24 },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27168), { 0, 0, height + 24 },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27153), { 0, 0, height + 24 },
                        { { 6, 0, height + 24 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7134 */
static void InvertedFlyingRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedFlyingRCTrackBankedLeftQuarterTurn5(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7144 */
static void InvertedFlyingRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27277), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27278), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27279), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27280), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
              PaintSegment::bottomRight },
            direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 47, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x007C7154 */
static void InvertedFlyingRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27281), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27282), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27283), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27284), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
              PaintSegment::bottomLeft },
            direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 47, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x007C7164 */
static void InvertedFlyingRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27285), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27286), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27287), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27288), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
              PaintSegment::bottomRight },
            direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 47, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::flatTo25Deg);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x007C7174 */
static void InvertedFlyingRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27289), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27290), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27291), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27292), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
              PaintSegment::bottomLeft },
            direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 47, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::flatTo25Deg);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x007C7184 */
static void InvertedFlyingRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack25DegUpToRightBank(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7194 */
static void InvertedFlyingRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack25DegUpToLeftBank(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C71A4 */
static void InvertedFlyingRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackRightBankTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C71B4 */
static void InvertedFlyingRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackLeftBankTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C71C4 */
static void InvertedFlyingRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27293), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27294), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27295), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27296), { 0, 0, height + 24 },
                { { 0, 6, height + 22 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
              PaintSegment::bottomRight },
            direction),
        0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007C71D4 */
static void InvertedFlyingRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C71E4 */
static void InvertedFlyingRCTrackLeftQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27317), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27322), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27327), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27332), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 49, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }
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
                        session, direction, session.TrackColours.WithIndex(27318), { 0, 0, height + 24 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27323), { 0, 0, height + 24 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27328), { 0, 16, height + 24 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27333), { 0, 16, height + 24 }, { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27319), { 0, 16, height + 24 }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27324), { 16, 16, height + 24 }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27329), { 16, 0, height + 24 }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27334), { 0, 0, height + 24 }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
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
                        session, direction, session.TrackColours.WithIndex(27320), { 16, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27325), { 0, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27330), { 0, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27335), { 16, 0, height + 24 }, { 16, 32, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27321), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27326), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27331), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27336), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 49, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x007C71F4 */
static void InvertedFlyingRCTrackRightQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27297), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27302), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27307), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27312), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 49, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }
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
                        session, direction, session.TrackColours.WithIndex(27298), { 0, 16, height + 24 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27303), { 0, 16, height + 24 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27308), { 0, 0, height + 24 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27313), { 0, 0, height + 24 }, { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27299), { 0, 0, height + 24 }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27304), { 16, 0, height + 24 }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27309), { 16, 16, height + 24 }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27314), { 0, 16, height + 24 }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
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
                        session, direction, session.TrackColours.WithIndex(27300), { 16, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27305), { 0, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27310), { 0, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27315), { 16, 0, height + 24 }, { 16, 32, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27301), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27306), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27311), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27316), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 49, session.SupportColours);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x007C7204 */
static void InvertedFlyingRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedFlyingRCTrackRightQuarterTurn525DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7214 */
static void InvertedFlyingRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedFlyingRCTrackLeftQuarterTurn525DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7224 */
static void InvertedFlyingRCTrackSBendLeft(
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
                        session, direction, session.TrackColours.WithIndex(27253), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27257), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27256), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27260), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27254), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 26, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27258), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 26, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27255), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 26, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27259), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 26, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + kSupportHeight,
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
                        session, direction, session.TrackColours.WithIndex(27255), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 26, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27259), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 26, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27254), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 26, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27258), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 26, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + kSupportHeight,
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
                        session, direction, session.TrackColours.WithIndex(27256), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27260), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27253), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27257), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7234 */
static void InvertedFlyingRCTrackSBendRight(
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
                        session, direction, session.TrackColours.WithIndex(27261), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27265), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27264), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27268), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27262), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 26, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27266), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 26, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27263), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 26, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27267), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 26, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + kSupportHeight,
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
                        session, direction, session.TrackColours.WithIndex(27263), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 26, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27267), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 26, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27262), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 26, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27266), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 26, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + kSupportHeight,
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
                        session, direction, session.TrackColours.WithIndex(27264), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27268), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27261), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27265), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7274 */
static void InvertedFlyingRCTrackLeftQuarterTurn3(
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
                        session, direction, session.TrackColours.WithIndex(27392), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27395), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27398), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27389), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
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
                        session, direction, session.TrackColours.WithIndex(27391), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27394), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27397), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27388), { 0, 0, height + 24 },
                        { { 16, 16, height + 22 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27390), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27393), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27396), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27387), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7284 */
static void InvertedFlyingRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedFlyingRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7294 */
static void InvertedFlyingRCTrackLeftQuarterTurn3Bank(
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
                        session, direction, session.TrackColours.WithIndex(27404), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27407), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27410), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27401), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
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
                        session, direction, session.TrackColours.WithIndex(27403), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27406), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27409), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27400), { 0, 0, height + 24 },
                        { { 16, 16, height + 22 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27402), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27405), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27408), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27399), { 0, 0, height + 24 },
                        { { 6, 0, height + 22 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C72A4 */
static void InvertedFlyingRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedFlyingRCTrackLeftQuarterTurn3Bank(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C72B4 */
static void InvertedFlyingRCTrackLeftQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27422), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27424), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27426), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27420), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 47, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }
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
                        session, direction, session.TrackColours.WithIndex(27421), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27423), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27425), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27419), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 47, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x007C72C4 */
static void InvertedFlyingRCTrackRightQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27411), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27413), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27415), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27417), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 47, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }
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
                        session, direction, session.TrackColours.WithIndex(27412), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27414), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27416), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27418), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 47, session.SupportColours);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x007C72D4 */
static void InvertedFlyingRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedFlyingRCTrackRightQuarterTurn325DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C72E4 */
static void InvertedFlyingRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedFlyingRCTrackLeftQuarterTurn325DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C73B4 */
static void InvertedFlyingRCTrackLeftQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27352), { 0, 0, height - 5 },
                { { 2, 2, height - 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27348), { 0, 0, height - 5 },
                { { 2, 2, height + 94 }, { 28, 28, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27353), { 0, 0, height - 5 },
                { { 2, 2, height - 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27349), { 0, 0, height - 5 },
                { { 2, 2, height + 94 }, { 28, 28, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27354), { 0, 0, height - 5 },
                { { 2, 2, height - 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27350), { 0, 0, height - 5 },
                { { 2, 2, height + 94 }, { 28, 28, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27351), { 0, 0, height - 5 },
                { { 2, 2, height - 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27347), { 0, 0, height - 5 },
                { { 2, 2, height + 94 }, { 28, 28, 1 } });
            break;
    }

    TrackPaintUtilLeftQuarterTurn1TileTunnel(
        session, kTunnelGroup, direction, height, -8, TunnelSubType::slopeStart, +56, TunnelSubType::slopeEnd);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x007C7394 */
static void InvertedFlyingRCTrackRightQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27343), { 0, 0, height - 5 },
                { { 2, 2, height - 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27339), { 0, 0, height - 5 },
                { { 2, 2, height + 94 }, { 28, 28, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27344), { 0, 0, height - 5 },
                { { 2, 2, height - 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27340), { 0, 0, height - 5 },
                { { 2, 2, height + 94 }, { 28, 28, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27345), { 0, 0, height - 5 },
                { { 2, 2, height - 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27341), { 0, 0, height - 5 },
                { { 2, 2, height + 94 }, { 28, 28, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27346), { 0, 0, height - 5 },
                { { 2, 2, height - 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27342), { 0, 0, height - 5 },
                { { 2, 2, height + 94 }, { 28, 28, 1 } });
            break;
    }
    TrackPaintUtilRightQuarterTurn1TileTunnel(
        session, kTunnelGroup, direction, height, -8, TunnelSubType::slopeStart, +56, TunnelSubType::slopeEnd);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x007C73A4 */
static void InvertedFlyingRCTrackLeftQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackRightQuarterTurn160DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C73C4 */
static void InvertedFlyingRCTrackRightQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackLeftQuarterTurn160DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C73D4 */
static void InvertedFlyingRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27337), { 0, 0, height + 24 },
                { { 0, 6, height + 24 }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27338), { 0, 0, height + 24 },
                { { 0, 6, height + 24 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void InvertedFlyingRCTrackBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const auto imageId = (direction & 1) ? SPR_TRACKS_BM_INVERT_BOOSTER_2 : SPR_TRACKS_BM_INVERT_BOOSTER_1;
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height + 24 },
        { { 0, 6, height + 22 }, { 32, 20, 1 } });

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007C7674 */
static void InvertedFlyingRCTrackLeftQuarterBankedHelixLargeUp(
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
                        session, direction, session.TrackColours.WithIndex(27202), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27207), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27212), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27197), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 41, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::tall);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27201), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27206), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27211), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27196), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27200), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27205), { 0, 0, height + 24 },
                        { { 16, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27210), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27195), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27199), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27204), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27209), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27194), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27198), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27203), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27208), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27193), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 53, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 16, kTunnelGroup, TunnelSubType::tall);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 16, kTunnelGroup, TunnelSubType::tall);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x007C7684 */
static void InvertedFlyingRCTrackRightQuarterBankedHelixLargeUp(
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
                        session, direction, session.TrackColours.WithIndex(27173), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27178), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27183), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27188), { 0, 0, height + 24 },
                        { { 0, 6, height + 30 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 41, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::tall);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27174), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27179), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27184), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27189), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27175), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27180), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27185), { 0, 0, height + 24 },
                        { { 16, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27190), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27176), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27181), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27186), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27191), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27177), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27182), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27187), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27192), { 0, 0, height + 24 },
                        { { 6, 0, height + 38 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 53, session.SupportColours);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 16, kTunnelGroup, TunnelSubType::tall);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 16, kTunnelGroup, TunnelSubType::tall);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x007C7694 */
static void InvertedFlyingRCTrackLeftQuarterBankedHelixLargeDown(
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
                        session, direction, session.TrackColours.WithIndex(27182), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27187), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27192), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27177), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 53, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::tall);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27181), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27186), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27191), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27176), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27180), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27185), { 0, 0, height + 24 },
                        { { 16, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27190), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27175), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27179), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27184), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27189), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27174), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27178), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27183), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27188), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27173), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 41, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::tall);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::tall);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x007C76A4 */
static void InvertedFlyingRCTrackRightQuarterBankedHelixLargeDown(
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
                        session, direction, session.TrackColours.WithIndex(27193), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27198), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27203), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27208), { 0, 0, height + 24 },
                        { { 0, 6, height + 38 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 53, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::tall);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27194), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27199), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27204), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27209), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27195), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27200), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27205), { 0, 0, height + 24 },
                        { { 16, 16, height + 38 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27210), { 0, 0, height + 24 },
                        { { 0, 16, height + 38 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27196), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27201), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27206), { 0, 0, height + 24 },
                        { { 0, 0, height + 38 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27211), { 0, 0, height + 24 },
                        { { 16, 0, height + 38 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27197), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27202), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27207), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27212), { 0, 0, height + 24 },
                        { { 6, 0, height + 30 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 41, session.SupportColours);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::tall);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::tall);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x007C78B4 */
static void InvertedFlyingRCTrack25DegUpLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27711), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27712), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27713), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27714), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 57, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x007C78C4 */
static void InvertedFlyingRCTrack25DegUpRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27715), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27716), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27717), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27718), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 57, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x007C73E4 */
static void InvertedFlyingRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilOnridePhotoPlatformPaint(session, direction, height, supportType.metal);

    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27129), { 0, 0, height + 24 },
                { { 0, 6, height + 24 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27130), { 0, 0, height + 24 },
                { { 0, 6, height + 24 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27129), { 0, 0, height + 24 },
                { { 0, 6, height + 24 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27130), { 0, 0, height + 24 },
                { { 0, 6, height + 24 }, { 32, 20, 3 } });
            break;
    }
    TrackPaintUtilOnridePhotoPaint2(session, direction, trackElement, height, kGeneralSupportHeightOnRidePhotoInverted);
}

/** rct2: 0x007C78D4 */
static void InvertedFlyingRCTrack25DegDownLeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack25DegUpRightBanked(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C78E4 */
static void InvertedFlyingRCTrack25DegDownRightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack25DegUpLeftBanked(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7404 */
static void InvertedFlyingRCTrackLeftEighthToDiag(
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
                        session, direction, session.TrackColours.WithIndex(27507), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27511), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27515), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27519), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27508), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27512), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27516), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27520), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27509), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27513), { 0, 0, height + 24 },
                        { { 16, 16, height + 22 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27517), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27521), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27510), { 0, 0, height + 24 },
                        { { 16, 16, height + 22 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27514), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 16, 18, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27518), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27522), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 0, height + kSupportHeight,
                session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7414 */
static void InvertedFlyingRCTrackRightEighthToDiag(
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
                        session, direction, session.TrackColours.WithIndex(27491), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27495), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27499), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27503), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27492), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27496), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27500), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 34, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27504), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27493), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27497), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27501), { 0, 0, height + 24 },
                        { { 16, 16, height + 22 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27505), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27494), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27498), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27502), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 16, 18, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27506), { 0, 0, height + 24 },
                        { { 16, 16, height + 22 }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + kSupportHeight,
                session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7424 */
static void InvertedFlyingRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedFlyingRCTrackRightEighthToDiag(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7434 */
static void InvertedFlyingRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedFlyingRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7444 */
static void InvertedFlyingRCTrackLeftEighthBankToDiag(
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
                        session, direction, session.TrackColours.WithIndex(27539), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27543), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27547), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27551), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27540), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27544), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27548), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27552), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27541), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27545), { 0, 0, height + 24 },
                        { { 16, 16, height + 22 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27549), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27553), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27542), { 0, 0, height + 24 },
                        { { 16, 16, height + 22 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27546), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 16, 18, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27550), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27554), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 0, height + kSupportHeight,
                session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7454 */
static void InvertedFlyingRCTrackRightEighthBankToDiag(
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
                        session, direction, session.TrackColours.WithIndex(27523), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27527), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27531), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27535), { 0, 0, height + 24 },
                        { { 0, 6, height + 22 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27524), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27528), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27532), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 34, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27536), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27525), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27529), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27533), { 0, 0, height + 24 },
                        { { 4, 4, height + 22 }, { 28, 28, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27537), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27526), { 0, 0, height + 24 },
                        { { 16, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27530), { 0, 0, height + 24 },
                        { { 0, 0, height + 22 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27534), { 0, 0, height + 24 },
                        { { 0, 16, height + 22 }, { 16, 18, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27538), { 0, 0, height + 24 },
                        { { 16, 16, height + 22 }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + kSupportHeight,
                session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7464 */
static void InvertedFlyingRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedFlyingRCTrackRightEighthBankToDiag(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7474 */
static void InvertedFlyingRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedFlyingRCTrackLeftEighthBankToDiag(
        session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C73F4 */
static void InvertedFlyingRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27614), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27558), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27611), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27555), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27613), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27557), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27612), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27556), { -16, -16, height + 24 },
                            { { -16, -16, height + 22 }, { 32, 32, 3 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + kSupportHeight,
                session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C74A4 */
static void InvertedFlyingRCTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27626), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27570), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27623), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27567), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27625), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27569), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27624), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27568), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 3 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 51, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x007C7504 */
static void InvertedFlyingRCTrackDiag60DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27582), { -16, -16, height + 24 },
                        { { -16, -16, height + 88 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27579), { -16, -16, height + 24 },
                        { { -16, -16, height + 88 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27581), { -16, -16, height + 24 },
                        { { -16, -16, height + 88 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27580), { -16, -16, height + 24 },
                        { { -16, -16, height + 24 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 32, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 36, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 32, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 36, height + kSupportHeight,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
    }
}

/** rct2: 0x007C7484 */
static void InvertedFlyingRCTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27618), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27562), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27615), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27559), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27617), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27561), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27616), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27560), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 43, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x007C74E4 */
static void InvertedFlyingRCTrackDiag25DegUpTo60DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27574), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27571), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27573), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27572), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 16, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 16, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 16, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 16, height + kSupportHeight,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x007C74F4 */
static void InvertedFlyingRCTrackDiag60DegUpTo25DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27578), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27575), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27577), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27576), { -16, -16, height + 24 },
                        { { 0, 0, height + 56 }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 21, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 21, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 21, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 21, height + kSupportHeight,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x007C7494 */
static void InvertedFlyingRCTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27622), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27566), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27619), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27563), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27621), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27565), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(27620), { -16, -16, height + 24 },
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
                            session, direction, session.TrackColours.WithIndex(27564), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 45, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x007C74D4 */
static void InvertedFlyingRCTrackDiag25DegDown(
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
                        session, direction, session.TrackColours.WithIndex(27568), { -16, -16, height + 24 },
                        { { -16, -16, height + 40 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27569), { -16, -16, height + 24 },
                        { { -16, -16, height + 40 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27567), { -16, -16, height + 24 },
                        { { -16, -16, height + 40 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27570), { -16, -16, height + 24 },
                        { { -16, -16, height + 40 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 51, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x007C7534 */
static void InvertedFlyingRCTrackDiag60DegDown(
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
                        session, direction, session.TrackColours.WithIndex(27580), { -16, -16, height + 24 },
                        { { -16, -16, height + 24 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27581), { -16, -16, height + 24 },
                        { { -16, -16, height + 88 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27579), { -16, -16, height + 24 },
                        { { -16, -16, height + 88 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27582), { -16, -16, height + 24 },
                        { { -16, -16, height + 88 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 24, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 28, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 24, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 28, height + kSupportHeight,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
    }
}

/** rct2: 0x007C74B4 */
static void InvertedFlyingRCTrackDiagFlatTo25DegDown(
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
                        session, direction, session.TrackColours.WithIndex(27564), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27565), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27563), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27566), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 45, session.SupportColours);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x007C7514 */
static void InvertedFlyingRCTrackDiag25DegDownTo60DegDown(
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
                        session, direction, session.TrackColours.WithIndex(27576), { -16, -16, height + 24 },
                        { { 0, 0, height + 56 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27577), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27575), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27578), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 17, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 17, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 17, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 17, height + kSupportHeight,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x007C7524 */
static void InvertedFlyingRCTrackDiag60DegDownTo25DegDown(
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
                        session, direction, session.TrackColours.WithIndex(27572), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27573), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27571), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27574), { -16, -16, height + 24 },
                        { { -16, -16, height + 56 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 8, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 8, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 8, height + kSupportHeight,
                        session.SupportColours);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 8, height + kSupportHeight,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x007C74C4 */
static void InvertedFlyingRCTrackDiag25DegDownToFlat(
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
                        session, direction, session.TrackColours.WithIndex(27560), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27561), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27559), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27562), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 43, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x007C7564 */
static void InvertedFlyingRCTrackDiagFlatToLeftBank(
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
                        session, direction, session.TrackColours.WithIndex(27590), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27587), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27589), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27588), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + kSupportHeight,
                session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7574 */
static void InvertedFlyingRCTrackDiagFlatToRightBank(
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
                        session, direction, session.TrackColours.WithIndex(27594), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27591), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27593), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27592), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + kSupportHeight,
                session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7584 */
static void InvertedFlyingRCTrackDiagLeftBankToFlat(
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
                        session, direction, session.TrackColours.WithIndex(27592), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27593), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27591), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27594), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + kSupportHeight,
                session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7594 */
static void InvertedFlyingRCTrackDiagRightBankToFlat(
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
                        session, direction, session.TrackColours.WithIndex(27588), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27589), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27587), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27590), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + kSupportHeight,
                session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C75C4 */
static void InvertedFlyingRCTrackDiagLeftBankTo25DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27606), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27603), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27605), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27604), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 43, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x007C75D4 */
static void InvertedFlyingRCTrackDiagRightBankTo25DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27610), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27607), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27609), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27608), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 43, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x007C75A4 */
static void InvertedFlyingRCTrackDiag25DegUpToLeftBank(
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
                        session, direction, session.TrackColours.WithIndex(27598), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27595), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27597), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27596), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 45, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x007C75B4 */
static void InvertedFlyingRCTrackDiag25DegUpToRightBank(
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
                        session, direction, session.TrackColours.WithIndex(27602), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27599), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27601), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27600), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 45, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

/** rct2: 0x007C75E4 */
static void InvertedFlyingRCTrackDiagLeftBankTo25DegDown(
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
                        session, direction, session.TrackColours.WithIndex(27600), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27601), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27599), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27602), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 45, session.SupportColours);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x007C75F4 */
static void InvertedFlyingRCTrackDiagRightBankTo25DegDown(
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
                        session, direction, session.TrackColours.WithIndex(27596), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27597), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27595), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27598), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 45, session.SupportColours);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x007C7604 */
static void InvertedFlyingRCTrackDiag25DegDownToLeftBank(
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
                        session, direction, session.TrackColours.WithIndex(27608), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27609), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27607), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27610), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 43, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x007C7614 */
static void InvertedFlyingRCTrackDiag25DegDownToRightBank(
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
                        session, direction, session.TrackColours.WithIndex(27604), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27605), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27603), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27606), { -16, -16, height + 24 },
                        { { -16, -16, height + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + 43, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x007C7544 */
static void InvertedFlyingRCTrackDiagLeftBank(
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
                        session, direction, session.TrackColours.WithIndex(27586), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27583), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27585), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27584), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + kSupportHeight,
                session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7554 */
static void InvertedFlyingRCTrackDiagRightBank(
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
                        session, direction, session.TrackColours.WithIndex(27584), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27585), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27583), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27586), { -16, -16, height + 24 },
                        { { -16, -16, height + 22 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + kSupportHeight,
                session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7664 */
static void InvertedFlyingRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27337), { 0, 0, height + 24 },
                { { 0, 6, height + 24 }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27338), { 0, 0, height + 24 },
                { { 0, 6, height + 24 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007C76B4 */
static void InvertedFlyingRCTrackLeftBankedQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27762), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27764), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27766), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27760), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 47, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27761), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27763), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27765), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27759), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 47, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x007C76C4 */
static void InvertedFlyingRCTrackRightBankedQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27751), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27753), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27755), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27757), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 47, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27752), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27754), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27756), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27758), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 47, session.SupportColours);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x007C76D4 */
static void InvertedFlyingRCTrackLeftBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedFlyingRCTrackRightBankedQuarterTurn325DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C76E4 */
static void InvertedFlyingRCTrackRightBankedQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    InvertedFlyingRCTrackLeftBankedQuarterTurn325DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C76F4 */
static void InvertedFlyingRCTrackLeftBankedQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27787), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27792), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27797), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27802), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 49, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27788), { 0, 0, height + 24 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27793), { 0, 0, height + 24 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27798), { 0, 16, height + 24 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27803), { 0, 16, height + 24 }, { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27789), { 0, 16, height + 24 }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27794), { 16, 16, height + 24 }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27799), { 16, 0, height + 24 }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27804), { 0, 0, height + 24 }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27790), { 16, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27795), { 0, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27800), { 0, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27805), { 16, 0, height + 24 }, { 16, 32, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27791), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27796), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27801), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27806), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 49, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x007C7704 */
static void InvertedFlyingRCTrackRightBankedQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours.WithIndex(27767), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27772), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27777), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27782), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 49, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27768), { 0, 16, height + 24 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27773), { 0, 16, height + 24 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27778), { 0, 0, height + 24 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27783), { 0, 0, height + 24 }, { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27769), { 0, 0, height + 24 }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27774), { 16, 0, height + 24 }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27779), { 16, 16, height + 24 }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27784), { 0, 16, height + 24 }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27770), { 16, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27775), { 0, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27780), { 0, 0, height + 24 }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27785), { 16, 0, height + 24 }, { 16, 32, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27771), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27776), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27781), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27786), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 49, session.SupportColours);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x007C7714 */
static void InvertedFlyingRCTrackLeftBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedFlyingRCTrackRightBankedQuarterTurn525DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7724 */
static void InvertedFlyingRCTrackRightBankedQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedFlyingRCTrackLeftBankedQuarterTurn525DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7734 */
static void InvertedFlyingRCTrack25DegUpToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27719), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27720), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27721), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27722), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 57, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x007C7744 */
static void InvertedFlyingRCTrack25DegUpToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27723), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27724), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27725), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27726), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 57, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x007C7754 */
static void InvertedFlyingRCTrackLeftBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27727), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27728), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27729), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27730), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 57, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x007C7764 */
static void InvertedFlyingRCTrackRightBanked25DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27731), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27732), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27733), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27734), { 0, 0, height + 24 },
                { { 0, 6, height + 40 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 57, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x007C7774 */
static void InvertedFlyingRCTrack25DegDownToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackRightBanked25DegUpTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7784 */
static void InvertedFlyingRCTrack25DegDownToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackLeftBanked25DegUpTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7794 */
static void InvertedFlyingRCTrackLeftBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack25DegUpToRightBanked25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C77A4 */
static void InvertedFlyingRCTrackRightBanked25DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack25DegUpToLeftBanked25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C77B4 */
static void InvertedFlyingRCTrackLeftBankedFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27735), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27736), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27737), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27738), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 49, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

/** rct2: 0x007C77C4 */
static void InvertedFlyingRCTrackRightBankedFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27739), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27740), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27741), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27742), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 49, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

/** rct2: 0x007C77F4 */
static void InvertedFlyingRCTrackLeftBanked25DegUpToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27743), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27744), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27745), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27746), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 47, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::flatTo25Deg);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x007C7804 */
static void InvertedFlyingRCTrackRightBanked25DegUpToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27747), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27748), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27749), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27750), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 47, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::flatTo25Deg);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x007C7814 */
static void InvertedFlyingRCTrackLeftBankedFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackRightBanked25DegUpToRightBankedFlat(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7824 */
static void InvertedFlyingRCTrackRightBankedFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackLeftBanked25DegUpToLeftBankedFlat(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C77D4 */
static void InvertedFlyingRCTrackLeftBanked25DegDownToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackRightBankedFlatToRightBanked25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C77E4 */
static void InvertedFlyingRCTrackRightBanked25DegDownToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackLeftBankedFlatToLeftBanked25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7834 */
static void InvertedFlyingRCTrackFlatToLeftBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27695), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27696), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27697), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27698), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 49, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

/** rct2: 0x007C7844 */
static void InvertedFlyingRCTrackFlatToRightBanked25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27699), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27700), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27701), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27702), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 49, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::slopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

/** rct2: 0x007C7854 */
static void InvertedFlyingRCTrackLeftBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27703), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27704), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27705), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27706), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 47, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::flatTo25Deg);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x007C7864 */
static void InvertedFlyingRCTrackRightBanked25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27707), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27708), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27709), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(27710), { 0, 0, height + 24 },
                { { 0, 6, height + 32 }, { 32, 20, 3 } });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0, height + 47, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::flatTo25Deg);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x007C7874 */
static void InvertedFlyingRCTrackFlatToLeftBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackRightBanked25DegUpToFlat(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7884 */
static void InvertedFlyingRCTrackFlatToRightBanked25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackLeftBanked25DegUpToFlat(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C7894 */
static void InvertedFlyingRCTrackLeftBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackFlatToRightBanked25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x007C78A4 */
static void InvertedFlyingRCTrackRightBanked25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackFlatToLeftBanked25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrack90DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:

        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 8),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 61 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 9),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 10),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 11),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 61 }, { 32, 20, 3 } });
                    break;
            }
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

static void InvertedFlyingRCTrack90DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilDiagTilesPaint(
        session, 1, height + 24, direction, trackSequence, kInvertedFlyingRCDiagBrakeImages, defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    PaintSegments blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    if (trackSequence == 3)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + kSupportHeight,
            session.SupportColours);
    }
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void InvertedFlyingRCTrackDiagBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagBrakes(session, ride, trackSequence, direction, height, trackElement, supportType);
}

/** rct2: 0x007C7644 */
static void InvertedFlyingRCTrackLeftFlyerTwistDown(
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
                        session, direction, session.TrackColours.WithIndex(27427), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27433), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27432), { 0, 6, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27438), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27428), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27434), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27431), { 0, 6, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27437), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27429), { 0, 6, height - 5 },
                        { { 0, 6, height + 6 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27435), { 0, 6, height - 5 },
                        { { 0, 6, height + 6 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27430), { 0, 6, height - 5 },
                        { { 0, 6, height + 6 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27436), { 0, 6, height - 5 },
                        { { 0, 6, height + 6 }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height - 5, session.SupportColours);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7654 */
static void InvertedFlyingRCTrackRightFlyerTwistDown(
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
                        session, direction, session.TrackColours.WithIndex(27439), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27445), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27444), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27450), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27440), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27446), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27443), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27449), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27441), { 0, 6, height - 5 },
                        { { 0, 6, height + 6 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27447), { 0, 6, height - 5 },
                        { { 0, 6, height + 6 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27442), { 0, 6, height - 5 },
                        { { 0, 6, height + 6 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27448), { 0, 6, height - 5 },
                        { { 0, 6, height + 6 }, { 32, 20, 3 } });
                    break;
            }
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height - 5, session.SupportColours);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x007C7304 */
static void FlyingRCTrackFlyerHalfLoopDown(
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
                        session, direction, session.TrackColours.WithIndex(17629), { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17637), { 0, 0, height + 32 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17630), { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17638), { 0, 16, height + 32 }, { 32, 12, 3 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
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
                        session, direction, session.TrackColours.WithIndex(17628), { 16, 0, height }, { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17636), { 12, 0, height },
                        { { 12, 0, height }, { 3, 16, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17631), { 10, 16, height }, { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17639), { 16, 16, height },
                        { { 15, 6, height }, { 2, 16, 119 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17627), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 20, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17635), { 0, 14, height },
                        { { 28, 6, height }, { 3, 20, 63 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 15, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17632), { 0, 6, height },
                        { { 28, 6, height }, { 3, 20, 63 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 16, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17640), { 0, 6, height }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 16, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17626), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17634), { 0, 6, height }, { 32, 20, 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17633), { 0, 6, height }, { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(17641), { 0, 6, height }, { 32, 20, 7 });
                    break;
            }
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::centre, 8, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

// transitions
static void InvertedFlyingRCTrackFlatTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            { PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight, PaintSegment::bottomRight }, direction),
        0xFFFF, 0);
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 0)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + kInvertedSpriteOffset + 16 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 1)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + kInvertedSpriteOffset }, { 32, 2, 43 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 2)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 29, 4, height + kInvertedSpriteOffset }, { 1, 24, 43 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 3)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + kInvertedSpriteOffset }, { 32, 2, 43 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 4)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 29, 4, height + kInvertedSpriteOffset }, { 1, 24, 43 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 5)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + kInvertedSpriteOffset + 16 }, { 32, 20, 3 } });
            break;
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::slopeEnd);
    }

    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void InvertedFlyingRCTrack60DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 6)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + kInvertedSpriteOffset + 24 }, { 32, 20, 3 } });
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 20, session.SupportColours);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 7)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height }, { 32, 1, kInvertedSpriteOffset + 24 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 8)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + kInvertedSpriteOffset + 24 }, { 32, 20, 3 } });
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 20, session.SupportColours);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 9)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height }, { 32, 1, kInvertedSpriteOffset + 24 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 10)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + kInvertedSpriteOffset + 24 }, { 32, 20, 3 } });
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 20, session.SupportColours);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 11)),
                { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + kInvertedSpriteOffset + 24 }, { 32, 20, 3 } });
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 20, session.SupportColours);
            break;
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::flat);
    }

    PaintUtilSetGeneralSupportHeight(session, height + 80);
}

static void InvertedFlyingRCTrackFlatTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack60DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrack60DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackFlatTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}
static void InvertedFlyingRCTrackFlatTo60DegUpLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 0),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 4),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 8),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 12),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + kSupportHeight + 4,
                            session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + kSupportHeight + 4,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + kSupportHeight + 4,
                            session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + kSupportHeight + 4,
                            session.SupportColours);
                        break;
                }
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }

            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);

            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 1),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 37 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 5),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 37 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 9),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 37 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 13),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 37 }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    switch (direction)
                    {
                        case 0:
                            MetalASupportsPaintSetup(
                                session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 65,
                                session.SupportColours);
                            break;
                        case 1:
                            MetalASupportsPaintSetup(
                                session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 65,
                                session.SupportColours);
                            break;
                        case 2:
                            MetalASupportsPaintSetup(
                                session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 65,
                                session.SupportColours);
                            break;
                        case 3:
                            MetalASupportsPaintSetup(
                                session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 65,
                                session.SupportColours);
                            break;
                    }
                }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 2),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 49 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 6),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 49 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 10),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 49 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 14),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 49 }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 72,
                            session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 76,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 72,
                            session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 72, session.SupportColours);
                        break;
                }
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 3),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 83 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 7),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 75 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 11),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 75 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 15),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 83 }, { 32, 20, 3 } });
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 24, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 24, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

static void InvertedFlyingRCTrack60DegUpToFlatLongBase(
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
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 16),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 83 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 20),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 75 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 24),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 75 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 28),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 83 }, { 32, 20, 3 } });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::slopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 17),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 57 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 21),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 57 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 25),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 57 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 29),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 57 }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 72,
                            session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 74,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 76,
                            session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 74, session.SupportColours);
                        break;
                }
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 18),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 22),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 26),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 30),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + 65,
                            session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + 65,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + 65,
                            session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + 65, session.SupportColours);
                        break;
                }
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 19),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 37 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 23),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 37 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 27),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 37 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP + 31),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 37 }, { 32, 20, 3 } });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + kSupportHeight + 4,
                            session.SupportColours);
                        break;
                    case 1:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + kSupportHeight + 8,
                            session.SupportColours);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + kSupportHeight + 10,
                            session.SupportColours);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + kSupportHeight + 8,
                            session.SupportColours);
                        break;
                }
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::flatTo25Deg);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::flatTo25Deg);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void InvertedFlyingRCTrackFlatTo60DegDownLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack60DegUpToFlatLongBase(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrack60DegDownToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackFlatTo60DegUpLongBase(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagFlatTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 15)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 40 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 12)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 40 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 14)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 40 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 13)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 40 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void InvertedFlyingRCTrackDiag60DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 19)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 53 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 16)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 53 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 18)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 53 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 24,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 17)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 53 }, { 32, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 24,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 24,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 24,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackDiagFlatTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 17)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 53 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 18)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 53 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 16)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 53 }, { 32, 32, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 20,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 19)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 53 }, { 32, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 20,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 20,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 20,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackDiag60DegDownToFlat(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 13)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 40 }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 14)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 40 }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 12)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 40 }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 6,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_SMALL_FLAT_TO_STEEP + 15)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 40 }, { 32, 32, 1 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 6,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 6,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 6,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void InvertedFlyingRCTrackDiagFlatTo60DegUpLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 9)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 8 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 0)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 8 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 6)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 8 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                      PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 14,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 3)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 8 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 14,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 9,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 10)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 24 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 22,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 1)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 24 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 7)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 24 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                      PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 24,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 4)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 24 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 40,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 24,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 11)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 64 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 44,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 7:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 2)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 64 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 128);
            break;
        case 8:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 8)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 64 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 128);
            break;
        case 9:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 5)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 64 }, { 32, 32, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 144);
            break;
    }
}

static void InvertedFlyingRCTrackDiag60DegUpToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 21)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 64 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 12)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 64 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 112);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 18)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 64 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 104);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                      PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 15)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 64 }, { 32, 32, 3 } });

                    break;

                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 22)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 72 }, { 32, 32, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 13)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 19)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 32 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                      PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 24,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 16)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 32 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 24,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 22,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 23)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 32 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 24,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 7:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 14)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 8 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 8:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 20)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 8 }, { 32, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 9:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 12,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_FLAT_TO_STEEP_DIAGONAL + 17)),
                        { -16, -16, height + kInvertedSpriteOffset },
                        { { -16, -16, height + kInvertedSpriteOffset + 8 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 16,
                        session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 12,
                        session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 16,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackDiagFlatTo60DegDownLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiag60DegUpToFlatLongBase(
        session, ride, 9 - trackSequence, direction ^ 2, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiag60DegDownToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagFlatTo60DegUpLongBase(
        session, ride, 9 - trackSequence, direction ^ 2, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackLeftBankToLeftQuarterTurn3Tile25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 0)),
                        { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 6,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 2)),
                        { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 8,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 4)),
                        { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 6,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 6)),
                        { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 6,
                        session.SupportColours);
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::topLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 1)),
                        { 6, 0, height + kInvertedSpriteOffset }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + kSupportHeight + 8,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 3)),
                        { 6, 0, height + kInvertedSpriteOffset }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + kSupportHeight + 9,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 5)),
                        { 6, 0, height + kInvertedSpriteOffset }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + kSupportHeight + 9,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 7)),
                        { 6, 0, height + kInvertedSpriteOffset }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + kSupportHeight + 8,
                        session.SupportColours);
                    break;
            }

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void InvertedFlyingRCTrackRightBankToRightQuarterTurn3Tile25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                      PaintSegment::topRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 8)),
                        { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 10)),
                        { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 12)),
                        { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 14)),
                        { 0, 6, height + kInvertedSpriteOffset }, { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 6, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::topLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 9)),
                        { 6, 0, height + kInvertedSpriteOffset }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomRightSide, 0, height + kSupportHeight + 9,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 11)),
                        { 6, 0, height + kInvertedSpriteOffset }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomLeftSide, 0, height + kSupportHeight + 9,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 13)),
                        { 6, 0, height + kInvertedSpriteOffset }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topLeftSide, 0, height + kSupportHeight + 8,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_TURN_BANK_TRANSITION + 15)),
                        { 6, 0, height + kInvertedSpriteOffset }, { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topRightSide, 0, height + kSupportHeight + 8,
                        session.SupportColours);
                    break;
            }

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::slopeEnd);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void InvertedFlyingRCTrackLeftQuarterTurn3Tile25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackRightBankToRightQuarterTurn3Tile25DegUp(
        session, ride, 3 - trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}
static void InvertedFlyingRCTrackRightQuarterTurn3Tile25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackLeftBankToLeftQuarterTurn3Tile25DegUp(
        session, ride, 3 - trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}
static void InvertedFlyingRCTrack60DegUpTo90DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 0),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 1),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 2),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 3),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 0),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 1),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 2),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 3),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                }
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
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

static void InvertedFlyingRCTrack90DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack60DegUpTo90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrack90DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.hasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 4),
                    { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 5),
                    { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 6),
                    { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 7),
                    { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 4),
                    { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 5),
                    { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 6),
                    { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 7),
                    { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                break;
        }
    }
    switch (direction)
    {
        case 1:
            PaintUtilPushTunnelRight(session, height + 48, kTunnelGroup, TunnelSubType::slopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 48, kTunnelGroup, TunnelSubType::slopeEnd);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 96);
}

static void InvertedFlyingRCTrack60DegDownTo90DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.hasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 6),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 7),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 4),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 5),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 6),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 7),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 4),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 5),
                            { 0, 0, height + kInvertedSpriteOffset }, { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                }
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 48, kTunnelGroup, TunnelSubType::slopeEnd);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            break;
    }
}

static void InvertedFlyingRCTrackLeftQuarterTurn1TileUp90(
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
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 12),
                        { 0, 0, height + 29 }, { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 13),
                        { 0, 0, height + 29 }, { { 4, 0, height + 11 }, { 2, 32, 31 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 14),
                        { 0, 0, height + 29 }, { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 15),
                        { 0, 0, height + 29 }, { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 16),
                        { 0, 0, height + 29 }, { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 17),
                        { 0, 0, height + 29 }, { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 18),
                        { 0, 0, height + 29 }, { { 0, 6, height + 125 }, { 32, 20, 3 } });
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

static void InvertedFlyingRCTrackRightQuarterTurn1TileUp90(
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
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 19),
                        { 0, 0, height + 29 }, { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 20),
                        { 0, 0, height + 29 }, { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 21),
                        { 0, 0, height + 29 }, { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 22),
                        { 0, 0, height + 29 }, { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 23),
                        { 0, 0, height + 29 }, { { 0, 4, height + 11 }, { 32, 2, 31 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 24),
                        { 0, 0, height + 29 }, { { 0, 6, height + 125 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_VERTICAL + 25),
                        { 0, 0, height + 29 }, { { 0, 6, height + 125 }, { 32, 20, 3 } });
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

static void InvertedFlyingRCTrackLeftQuarterTurn1TileDown90(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    InvertedFlyingRCTrackRightQuarterTurn1TileUp90(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackRightQuarterTurn1TileDown90(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    InvertedFlyingRCTrackLeftQuarterTurn1TileUp90(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrack90DegToInvertedFlatQuarterLoopUp(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 0)),
                        { 0, 0, height - 8 }, { { 28, 6, height }, { 2, 20, 31 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 4)),
                        { 0, 0, height - 8 }, { { 0, 6, height - 8 }, { 20, 1, 48 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 7)),
                        { 0, 0, height - 8 }, { { 0, 6, height - 8 }, { 20, 1, 48 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 10)),
                        { 0, 0, height - 8 }, { { 28, 6, height }, { 2, 20, 31 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 1)),
                        { 0, 0, height - 8 }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 2)),
                        { 0, 0, height - 8 }, { { 0, 25, height + 1 }, { 32, 1, 32 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 5)),
                        { 0, 0, height - 8 }, { { 0, 6, height - 8 }, { 20, 1, 48 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 8)),
                        { 0, 0, height - 8 }, { { 0, 6, height - 8 }, { 20, 1, 48 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 11)),
                        { 0, 0, height - 8 }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 12)),
                        { 0, 0, height - 8 }, { { 0, 25, height + 1 }, { 32, 1, 32 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 3)),
                        { 0, 0, height - 8 }, { { 0, 6, height + 4 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 6)),
                        { 0, 0, height - 8 }, { { 0, 6, height + 4 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 9)),
                        { 0, 0, height - 8 }, { { 0, 6, height + 4 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_QUARTER_LOOP + 13)),
                        { 0, 0, height - 8 }, { { 0, 6, height + 4 }, { 32, 20, 1 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 32, kTunnelGroup, TunnelSubType::flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 68);
            break;
    }
}

static void InvertedFlyingRCTrackInvertedFlatTo90DegQuarterLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrack90DegToInvertedFlatQuarterLoopUp(
        session, ride, 2 - trackSequence, direction, height, trackElement, supportType);
}

// diag banks and wide turns

// wide turn - unbanked
static void InvertedFlyingRCTrackLeftEighthToDiagUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 0)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 4)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 8)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 12)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 1)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 5)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 9)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 13)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 2)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 6)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 10)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 14)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 3)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 7)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 11)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 15)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackLeftEighthToOrthogonalUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::right, PaintSegment::bottomRight,
                      PaintSegment::bottomLeft, PaintSegment::topRight, PaintSegment::topLeft },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 32)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 36)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 40)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 44)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottomRight, PaintSegment::bottom, PaintSegment::centre, PaintSegment::right,
                      PaintSegment::bottomLeft, PaintSegment::topRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 33)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 37)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 18,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 41)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 18,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 45)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 18,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments({ PaintSegment::topLeft, PaintSegment::top, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 34)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 38)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 42)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 8 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 46)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 8 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::left, PaintSegment::right },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 35)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 39)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 43)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 47)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
            }
            if (direction == 1 || direction == 2)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::slopeEnd);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackRightEighthToDiagUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 16)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 20)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 24)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 28)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 17)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 21)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 25)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 29)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::centre,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre,
                      PaintSegment::topRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 18)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 22)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 26)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 8,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 30)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 19)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 23)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 27)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 31)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void InvertedFlyingRCTrackRightEighthToOrthogonalUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::left, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight, PaintSegment::topLeft, PaintSegment::topRight },
                    direction),
                0xFFFF, 0);

            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 48)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 52)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 56)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 60)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::centre, PaintSegment::topRight, PaintSegment::right, PaintSegment::topLeft,
                      PaintSegment::top },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 49)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 17,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 53)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 17,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 57)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 61)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 15,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments({ PaintSegment::topRight, PaintSegment::top, PaintSegment::topLeft }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomRight,
                      PaintSegment::left },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 50)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 8 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 54)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 58)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 62)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 8 }, { 16, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments({ PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 51)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 6, 0, height + kInvertedSpriteOffset + 8 }, { 20, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 55)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 6, 0, height + kInvertedSpriteOffset + 8 }, { 20, 32, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 59)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 6, 0, height + kInvertedSpriteOffset + 8 }, { 20, 32, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE + 63)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 6, 0, height + kInvertedSpriteOffset + 8 }, { 20, 32, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 1)
            {
                PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackLeftEighthToDiagDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t map[5] = { 4, 3, 1, 2, 0 };
    trackSequence = map[trackSequence];
    InvertedFlyingRCTrackRightEighthToOrthogonalUp25(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackLeftEighthToOrthogonalDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedFlyingRCTrackRightEighthToDiagUp25(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackRightEighthToDiagDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t map[5] = { 4, 3, 1, 2, 0 };
    trackSequence = map[trackSequence];
    InvertedFlyingRCTrackLeftEighthToOrthogonalUp25(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackRightEighthToOrthogonalDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedFlyingRCTrackLeftEighthToDiagUp25(
        session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

// wide turn - banked
static void InvertedFlyingRCTrackLeftEighthBankToDiagUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::topRight, PaintSegment::right, PaintSegment::bottomRight, PaintSegment::bottom,
                      PaintSegment::left, PaintSegment::centre, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 0)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 4)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 8)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 12)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 1)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 5)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 9)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 13)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 2)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 6)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 10)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 14)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments({ PaintSegment::topLeft, PaintSegment::left, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomRight, PaintSegment::topLeft, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 3)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 7)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 11)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 15)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackLeftEighthBankToOrthogonalUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::right, PaintSegment::bottomRight,
                      PaintSegment::bottomLeft, PaintSegment::topRight, PaintSegment::topLeft },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 32)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 36)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 40)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 44)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottomRight, PaintSegment::bottom, PaintSegment::centre, PaintSegment::right,
                      PaintSegment::bottomLeft, PaintSegment::topRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 33)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 17,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 37)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 17,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 41)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 19,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 45)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 15,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments({ PaintSegment::topLeft, PaintSegment::top, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 34)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 38)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 42)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 8 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 46)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 8 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::left, PaintSegment::right },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 35)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 39)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 43)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 47)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
            }
            if (direction == 1 || direction == 2)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackRightEighthBankToDiagUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::topRight, PaintSegment::top, PaintSegment::topLeft, PaintSegment::left,
                      PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 16)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 20)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 24)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 28)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 6, height + kInvertedSpriteOffset + 8 }, { 32, 20, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::slopeStart);
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::centre,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 17)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 21)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 25)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 29)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 32, 16, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre,
                      PaintSegment::topRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 18)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 22)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 26)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 8,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 30)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 13,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::bottomRight, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 19)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 23)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 27)),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 31)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackRightEighthBankToOrthogonalUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::left, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight, PaintSegment::topLeft, PaintSegment::topRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 48)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 52)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 56)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 60)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 16, 16, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::centre, PaintSegment::topRight, PaintSegment::right, PaintSegment::topLeft,
                      PaintSegment::top },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 49)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + kSupportHeight + 19,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 53)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + kSupportHeight + 19,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 57)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 16, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + kSupportHeight + 19,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 61)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 16, height + kInvertedSpriteOffset + 16 }, { 16, 16, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + kSupportHeight + 17,
                        session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments({ PaintSegment::topRight, PaintSegment::top, PaintSegment::topLeft }, direction),
                0xFFFF, 0);

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomRight,
                      PaintSegment::left },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 50)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 8 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 54)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 58)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 0, 0, height + kInvertedSpriteOffset + 8 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 62)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 16, 0, height + kInvertedSpriteOffset + 8 }, { 16, 32, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::top,
                      PaintSegment::topRight, PaintSegment::right },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 51)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 6, 0, height + kInvertedSpriteOffset + 8 }, { 20, 32, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 55)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 6, 0, height + kInvertedSpriteOffset + 8 }, { 20, 32, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 59)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 6, 0, height + kInvertedSpriteOffset + 8 }, { 20, 32, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_GENTLE_LARGE_CURVE_BANKED + 63)),
                        { 0, 0, height + kInvertedSpriteOffset },
                        { { 6, 0, height + kInvertedSpriteOffset + 8 }, { 20, 32, 3 } });

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::centre, 0, height + kSupportHeight + 10,
                        session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 1)
            {
                PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::slopeEnd);
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackLeftEighthBankToDiagDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t map[5] = { 4, 3, 1, 2, 0 };
    trackSequence = map[trackSequence];
    InvertedFlyingRCTrackRightEighthBankToOrthogonalUp25(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackRightEighthBankToDiagDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t map[5] = { 4, 3, 1, 2, 0 };
    trackSequence = map[trackSequence];
    InvertedFlyingRCTrackLeftEighthBankToOrthogonalUp25(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackLeftEighthBankToOrthogonalDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedFlyingRCTrackRightEighthBankToDiagUp25(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackRightEighthBankToOrthogonalDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    InvertedFlyingRCTrackLeftEighthBankToDiagUp25(
        session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

// diag banked track and transitions

static void InvertedFlyingRCTrackDiagUp25ToLeftBankedUp25(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 3)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 0)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 52, session.SupportColours);
                    break;
                case 1:

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 52, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 2)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 52, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 52, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 1)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackDiagUp25ToRightBankedUp25(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 7)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 4)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 6)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 60, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 5)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 60, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 60, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 60, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackDiagLeftBankedUp25ToUp25(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 11)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 8)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 52, session.SupportColours);
                    break;
                case 1:

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 52, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 10)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 52, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 52, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 9)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackDiagRightBankedUp25ToUp25(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 15)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 12)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 14)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 62, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 13)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 60, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 60, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 60, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackDiagDown25ToLeftBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagRightBankedUp25ToUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagDown25ToRightBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagLeftBankedUp25ToUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagLeftBankedDown25ToDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagUp25ToRightBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagRightBankedDown25ToDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagUp25ToLeftBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagLeftBankedFlatToLeftBankedUp25(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 19)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 16)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 48, session.SupportColours);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 52, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 18)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 52, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 52, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 17)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void InvertedFlyingRCTrackDiagRightBankedFlatToRightBankedUp25(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 23)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 20)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 22)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 52, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 21)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 52, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 52, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 52, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void InvertedFlyingRCTrackDiagLeftBankedUp25ToLeftBankedFlat(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 27)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 24)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 52, session.SupportColours);
                    break;
                case 1:

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 54, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 26)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 52, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 52, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 25)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void InvertedFlyingRCTrackDiagRightBankedUp25ToRightBankedFlat(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 31)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 28)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:

            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 30)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 52, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 29)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 52, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 52, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 52, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void InvertedFlyingRCTrackDiagLeftBankedFlatToLeftBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagRightBankedUp25ToRightBankedFlat(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagRightBankedFlatToRightBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagLeftBankedUp25ToLeftBankedFlat(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagLeftBankedDown25ToLeftBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagRightBankedFlatToRightBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagRightBankedDown25ToRightBankedFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagLeftBankedFlatToLeftBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagUp25LeftBanked(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 35)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 32)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 52, session.SupportColours);
                    break;
                case 1:

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 52, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 34)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 52, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 52, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 33)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackDiagUp25RightBanked(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 39)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 36)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 38)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 60, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 37)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 45 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 60, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 60, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 60, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void InvertedFlyingRCTrackDiagDown25LeftBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagUp25RightBanked(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagDown25RightBanked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagUp25LeftBanked(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagFlatToLeftBankedUp25(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 43)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 40)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 48, session.SupportColours);
                    break;
                case 1:

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 48, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 42)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 52, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 50, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 41)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void InvertedFlyingRCTrackDiagFlatToRightBankedUp25(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 47)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 44)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 46)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 48, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 45)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 48, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 48, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 48, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
    }
}

static void InvertedFlyingRCTrackDiagLeftBankedUp25ToFlat(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 51)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 48)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 52, session.SupportColours);
                    break;
                case 1:

                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 52, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 50)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 52, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 52, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 49)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void InvertedFlyingRCTrackDiagRightBankedUp25ToFlat(
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
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 55)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 52)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 54)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft }, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::leftCorner, 0, height + 52, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_BM_INVERT_DIAG_SLOPED_BANKING + 53)),
                        { -16, -16, height + kInvertedSpriteOffset }, { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::topCorner, 0, height + 52, session.SupportColours);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::rightCorner, 0, height + 52, session.SupportColours);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, supportType.metal, MetalSupportPlace::bottomCorner, 0, height + 50, session.SupportColours);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

static void InvertedFlyingRCTrackDiagFlatToLeftBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagRightBankedUp25ToFlat(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagFlatToRightBankedDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagLeftBankedUp25ToFlat(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagLeftBankedDown25ToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagFlatToRightBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void InvertedFlyingRCTrackDiagRightBankedDown25ToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedFlyingRCTrackDiagFlatToLeftBankedUp25(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

// unbanked 1/4 helix

static void InvertedFlyingRCTrackLeftQuarterHelixLargeUp(
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
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 5),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 10),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 15),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 46, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 1),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 6),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 11),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 16),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::topRight, PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 2),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 7),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 12),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 17),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + 43 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 3),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 8),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 13),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 18),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 4),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 9),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 14),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_LEFT + 19),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 58, session.SupportColours);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 16, kTunnelGroup, TunnelSubType::flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 16, kTunnelGroup, TunnelSubType::flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void InvertedFlyingRCTrackRightQuarterHelixLargeUp(
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
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 5),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 10),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 15),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomLeft },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 46, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 1),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 6),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 11),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 16),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 2),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 7),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 12),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 17),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 16, height + 43 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight }, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 3),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 8),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 13),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 18),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                      PaintSegment::bottomLeft, PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 4),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 9),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 14),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_TRACKS_BM_INVERT_QUARTER_HELIX_RIGHT + 19),
                        { 0, 0, height + kInvertedSpriteOffset }, { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    { PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                      PaintSegment::bottomRight },
                    direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::centre, 0, height + 58, session.SupportColours);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 16, kTunnelGroup, TunnelSubType::flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 16, kTunnelGroup, TunnelSubType::flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

static void InvertedFlyingRCTrackRightQuarterHelixLargeDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedFlyingRCTrackLeftQuarterHelixLargeUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}
static void InvertedFlyingRCTrackLeftQuarterHelixLargeDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    InvertedFlyingRCTrackRightQuarterHelixLargeUp(
        session, ride, trackSequence, DirectionNext(direction), height, trackElement, supportType);
}

TrackPaintFunction GetTrackPaintFunctionFlyingRCInverted(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::flat:
            return InvertedFlyingRCTrackFlat;
        case TrackElemType::endStation:
        case TrackElemType::beginStation:
        case TrackElemType::middleStation:
            return InvertedFlyingRCTrackStation;
        case TrackElemType::up25:
            return InvertedFlyingRCTrack25DegUp;
        case TrackElemType::up60:
            return InvertedFlyingRCTrack60DegUp;
        case TrackElemType::flatToUp25:
            return InvertedFlyingRCTrackFlatTo25DegUp;
        case TrackElemType::up25ToUp60:
            return InvertedFlyingRCTrack25DegUpTo60DegUp;
        case TrackElemType::up60ToUp25:
            return InvertedFlyingRCTrack60DegUpTo25DegUp;
        case TrackElemType::up25ToFlat:
            return InvertedFlyingRCTrack25DegUpToFlat;
        case TrackElemType::down25:
            return InvertedFlyingRCTrack25DegDown;
        case TrackElemType::down60:
            return InvertedFlyingRCTrack60DegDown;
        case TrackElemType::flatToDown25:
            return InvertedFlyingRCTrackFlatTo25DegDown;
        case TrackElemType::down25ToDown60:
            return InvertedFlyingRCTrack25DegDownTo60DegDown;
        case TrackElemType::down60ToDown25:
            return InvertedFlyingRCTrack60DegDownTo25DegDown;
        case TrackElemType::down25ToFlat:
            return InvertedFlyingRCTrack25DegDownToFlat;
        case TrackElemType::leftQuarterTurn5Tiles:
            return InvertedFlyingRCTrackLeftQuarterTurn5;
        case TrackElemType::rightQuarterTurn5Tiles:
            return InvertedFlyingRCTrackRightQuarterTurn5;
        case TrackElemType::flatToLeftBank:
            return InvertedFlyingRCTrackFlatToLeftBank;
        case TrackElemType::flatToRightBank:
            return InvertedFlyingRCTrackFlatToRightBank;
        case TrackElemType::leftBankToFlat:
            return InvertedFlyingRCTrackLeftBankToFlat;
        case TrackElemType::rightBankToFlat:
            return InvertedFlyingRCTrackRightBankToFlat;
        case TrackElemType::bankedLeftQuarterTurn5Tiles:
            return InvertedFlyingRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::bankedRightQuarterTurn5Tiles:
            return InvertedFlyingRCTrackBankedRightQuarterTurn5;
        case TrackElemType::leftBankToUp25:
            return InvertedFlyingRCTrackLeftBankTo25DegUp;
        case TrackElemType::rightBankToUp25:
            return InvertedFlyingRCTrackRightBankTo25DegUp;
        case TrackElemType::up25ToLeftBank:
            return InvertedFlyingRCTrack25DegUpToLeftBank;
        case TrackElemType::up25ToRightBank:
            return InvertedFlyingRCTrack25DegUpToRightBank;
        case TrackElemType::leftBankToDown25:
            return InvertedFlyingRCTrackLeftBankTo25DegDown;
        case TrackElemType::rightBankToDown25:
            return InvertedFlyingRCTrackRightBankTo25DegDown;
        case TrackElemType::down25ToLeftBank:
            return InvertedFlyingRCTrack25DegDownToLeftBank;
        case TrackElemType::down25ToRightBank:
            return InvertedFlyingRCTrack25DegDownToRightBank;
        case TrackElemType::leftBank:
            return InvertedFlyingRCTrackLeftBank;
        case TrackElemType::rightBank:
            return InvertedFlyingRCTrackRightBank;
        case TrackElemType::leftQuarterTurn5TilesUp25:
            return InvertedFlyingRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::rightQuarterTurn5TilesUp25:
            return InvertedFlyingRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::leftQuarterTurn5TilesDown25:
            return InvertedFlyingRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::rightQuarterTurn5TilesDown25:
            return InvertedFlyingRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::sBendLeft:
            return InvertedFlyingRCTrackSBendLeft;
        case TrackElemType::sBendRight:
            return InvertedFlyingRCTrackSBendRight;
        case TrackElemType::leftQuarterTurn3Tiles:
            return InvertedFlyingRCTrackLeftQuarterTurn3;
        case TrackElemType::rightQuarterTurn3Tiles:
            return InvertedFlyingRCTrackRightQuarterTurn3;
        case TrackElemType::leftBankedQuarterTurn3Tiles:
            return InvertedFlyingRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::rightBankedQuarterTurn3Tiles:
            return InvertedFlyingRCTrackRightQuarterTurn3Bank;
        case TrackElemType::leftQuarterTurn3TilesUp25:
            return InvertedFlyingRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::rightQuarterTurn3TilesUp25:
            return InvertedFlyingRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::leftQuarterTurn3TilesDown25:
            return InvertedFlyingRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::rightQuarterTurn3TilesDown25:
            return InvertedFlyingRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::leftQuarterTurn1TileUp60:
            return InvertedFlyingRCTrackLeftQuarterTurn160DegUp;
        case TrackElemType::rightQuarterTurn1TileUp60:
            return InvertedFlyingRCTrackRightQuarterTurn160DegUp;
        case TrackElemType::leftQuarterTurn1TileDown60:
            return InvertedFlyingRCTrackLeftQuarterTurn160DegDown;
        case TrackElemType::rightQuarterTurn1TileDown60:
            return InvertedFlyingRCTrackRightQuarterTurn160DegDown;
        case TrackElemType::brakes:
            return InvertedFlyingRCTrackBrakes;
        case TrackElemType::leftQuarterBankedHelixLargeUp:
            return InvertedFlyingRCTrackLeftQuarterBankedHelixLargeUp;
        case TrackElemType::rightQuarterBankedHelixLargeUp:
            return InvertedFlyingRCTrackRightQuarterBankedHelixLargeUp;
        case TrackElemType::leftQuarterBankedHelixLargeDown:
            return InvertedFlyingRCTrackLeftQuarterBankedHelixLargeDown;
        case TrackElemType::rightQuarterBankedHelixLargeDown:
            return InvertedFlyingRCTrackRightQuarterBankedHelixLargeDown;
        case TrackElemType::up25LeftBanked:
            return InvertedFlyingRCTrack25DegUpLeftBanked;
        case TrackElemType::up25RightBanked:
            return InvertedFlyingRCTrack25DegUpRightBanked;
        case TrackElemType::onRidePhoto:
            return InvertedFlyingRCTrackOnRidePhoto;
        case TrackElemType::down25LeftBanked:
            return InvertedFlyingRCTrack25DegDownLeftBanked;
        case TrackElemType::down25RightBanked:
            return InvertedFlyingRCTrack25DegDownRightBanked;
        case TrackElemType::leftEighthToDiag:
            return InvertedFlyingRCTrackLeftEighthToDiag;
        case TrackElemType::rightEighthToDiag:
            return InvertedFlyingRCTrackRightEighthToDiag;
        case TrackElemType::leftEighthToOrthogonal:
            return InvertedFlyingRCTrackLeftEighthToOrthogonal;
        case TrackElemType::rightEighthToOrthogonal:
            return InvertedFlyingRCTrackRightEighthToOrthogonal;
        case TrackElemType::leftEighthBankToDiag:
            return InvertedFlyingRCTrackLeftEighthBankToDiag;
        case TrackElemType::rightEighthBankToDiag:
            return InvertedFlyingRCTrackRightEighthBankToDiag;
        case TrackElemType::leftEighthBankToOrthogonal:
            return InvertedFlyingRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::rightEighthBankToOrthogonal:
            return InvertedFlyingRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::diagFlat:
            return InvertedFlyingRCTrackDiagFlat;
        case TrackElemType::diagUp25:
            return InvertedFlyingRCTrackDiag25DegUp;
        case TrackElemType::diagUp60:
            return InvertedFlyingRCTrackDiag60DegUp;
        case TrackElemType::diagFlatToUp25:
            return InvertedFlyingRCTrackDiagFlatTo25DegUp;
        case TrackElemType::diagUp25ToUp60:
            return InvertedFlyingRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::diagUp60ToUp25:
            return InvertedFlyingRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::diagUp25ToFlat:
            return InvertedFlyingRCTrackDiag25DegUpToFlat;
        case TrackElemType::diagDown25:
            return InvertedFlyingRCTrackDiag25DegDown;
        case TrackElemType::diagDown60:
            return InvertedFlyingRCTrackDiag60DegDown;
        case TrackElemType::diagFlatToDown25:
            return InvertedFlyingRCTrackDiagFlatTo25DegDown;
        case TrackElemType::diagDown25ToDown60:
            return InvertedFlyingRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::diagDown60ToDown25:
            return InvertedFlyingRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::diagDown25ToFlat:
            return InvertedFlyingRCTrackDiag25DegDownToFlat;
        case TrackElemType::diagFlatToLeftBank:
            return InvertedFlyingRCTrackDiagFlatToLeftBank;
        case TrackElemType::diagFlatToRightBank:
            return InvertedFlyingRCTrackDiagFlatToRightBank;
        case TrackElemType::diagLeftBankToFlat:
            return InvertedFlyingRCTrackDiagLeftBankToFlat;
        case TrackElemType::diagRightBankToFlat:
            return InvertedFlyingRCTrackDiagRightBankToFlat;
        case TrackElemType::diagLeftBankToUp25:
            return InvertedFlyingRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::diagRightBankToUp25:
            return InvertedFlyingRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::diagUp25ToLeftBank:
            return InvertedFlyingRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::diagUp25ToRightBank:
            return InvertedFlyingRCTrackDiag25DegUpToRightBank;
        case TrackElemType::diagLeftBankToDown25:
            return InvertedFlyingRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::diagRightBankToDown25:
            return InvertedFlyingRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::diagDown25ToLeftBank:
            return InvertedFlyingRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::diagDown25ToRightBank:
            return InvertedFlyingRCTrackDiag25DegDownToRightBank;
        case TrackElemType::diagLeftBank:
            return InvertedFlyingRCTrackDiagLeftBank;
        case TrackElemType::diagRightBank:
            return InvertedFlyingRCTrackDiagRightBank;
        case TrackElemType::blockBrakes:
            return InvertedFlyingRCTrackBlockBrakes;
        case TrackElemType::leftBankedQuarterTurn3TileUp25:
            return InvertedFlyingRCTrackLeftBankedQuarterTurn325DegUp;
        case TrackElemType::rightBankedQuarterTurn3TileUp25:
            return InvertedFlyingRCTrackRightBankedQuarterTurn325DegUp;
        case TrackElemType::leftBankedQuarterTurn3TileDown25:
            return InvertedFlyingRCTrackLeftBankedQuarterTurn325DegDown;
        case TrackElemType::rightBankedQuarterTurn3TileDown25:
            return InvertedFlyingRCTrackRightBankedQuarterTurn325DegDown;
        case TrackElemType::leftBankedQuarterTurn5TileUp25:
            return InvertedFlyingRCTrackLeftBankedQuarterTurn525DegUp;
        case TrackElemType::rightBankedQuarterTurn5TileUp25:
            return InvertedFlyingRCTrackRightBankedQuarterTurn525DegUp;
        case TrackElemType::leftBankedQuarterTurn5TileDown25:
            return InvertedFlyingRCTrackLeftBankedQuarterTurn525DegDown;
        case TrackElemType::rightBankedQuarterTurn5TileDown25:
            return InvertedFlyingRCTrackRightBankedQuarterTurn525DegDown;
        case TrackElemType::up25ToLeftBankedUp25:
            return InvertedFlyingRCTrack25DegUpToLeftBanked25DegUp;
        case TrackElemType::up25ToRightBankedUp25:
            return InvertedFlyingRCTrack25DegUpToRightBanked25DegUp;
        case TrackElemType::leftBankedUp25ToUp25:
            return InvertedFlyingRCTrackLeftBanked25DegUpTo25DegUp;
        case TrackElemType::rightBankedUp25ToUp25:
            return InvertedFlyingRCTrackRightBanked25DegUpTo25DegUp;
        case TrackElemType::down25ToLeftBankedDown25:
            return InvertedFlyingRCTrack25DegDownToLeftBanked25DegDown;
        case TrackElemType::down25ToRightBankedDown25:
            return InvertedFlyingRCTrack25DegDownToRightBanked25DegDown;
        case TrackElemType::leftBankedDown25ToDown25:
            return InvertedFlyingRCTrackLeftBanked25DegDownTo25DegDown;
        case TrackElemType::rightBankedDown25ToDown25:
            return InvertedFlyingRCTrackRightBanked25DegDownTo25DegDown;
        case TrackElemType::leftBankedFlatToLeftBankedUp25:
            return InvertedFlyingRCTrackLeftBankedFlatToLeftBanked25DegUp;
        case TrackElemType::rightBankedFlatToRightBankedUp25:
            return InvertedFlyingRCTrackRightBankedFlatToRightBanked25DegUp;
        case TrackElemType::leftBankedUp25ToLeftBankedFlat:
            return InvertedFlyingRCTrackLeftBanked25DegUpToLeftBankedFlat;
        case TrackElemType::rightBankedUp25ToRightBankedFlat:
            return InvertedFlyingRCTrackRightBanked25DegUpToRightBankedFlat;
        case TrackElemType::leftBankedFlatToLeftBankedDown25:
            return InvertedFlyingRCTrackLeftBankedFlatToLeftBanked25DegDown;
        case TrackElemType::rightBankedFlatToRightBankedDown25:
            return InvertedFlyingRCTrackRightBankedFlatToRightBanked25DegDown;
        case TrackElemType::leftBankedDown25ToLeftBankedFlat:
            return InvertedFlyingRCTrackLeftBanked25DegDownToLeftBankedFlat;
        case TrackElemType::rightBankedDown25ToRightBankedFlat:
            return InvertedFlyingRCTrackRightBanked25DegDownToRightBankedFlat;
        case TrackElemType::flatToLeftBankedUp25:
            return InvertedFlyingRCTrackFlatToLeftBanked25DegUp;
        case TrackElemType::flatToRightBankedUp25:
            return InvertedFlyingRCTrackFlatToRightBanked25DegUp;
        case TrackElemType::leftBankedUp25ToFlat:
            return InvertedFlyingRCTrackLeftBanked25DegUpToFlat;
        case TrackElemType::rightBankedUp25ToFlat:
            return InvertedFlyingRCTrackRightBanked25DegUpToFlat;
        case TrackElemType::flatToLeftBankedDown25:
            return InvertedFlyingRCTrackFlatToLeftBanked25DegDown;
        case TrackElemType::flatToRightBankedDown25:
            return InvertedFlyingRCTrackFlatToRightBanked25DegDown;
        case TrackElemType::leftBankedDown25ToFlat:
            return InvertedFlyingRCTrackLeftBanked25DegDownToFlat;
        case TrackElemType::rightBankedDown25ToFlat:
            return InvertedFlyingRCTrackRightBanked25DegDownToFlat;
        case TrackElemType::leftFlyerTwistDown:
            return InvertedFlyingRCTrackLeftFlyerTwistDown;
        case TrackElemType::rightFlyerTwistDown:
            return InvertedFlyingRCTrackRightFlyerTwistDown;
        case TrackElemType::flyerHalfLoopInvertedDown:
            return FlyingRCTrackFlyerHalfLoopDown;
        // OpenRCT2-specific paint code
        case TrackElemType::booster:
            return InvertedFlyingRCTrackBooster;
        case TrackElemType::up90:
            return InvertedFlyingRCTrack90DegUp;
        case TrackElemType::down90:
            return InvertedFlyingRCTrack90DegDown;
        // OpenRCT2-specific track elements
        case TrackElemType::diagBrakes:
            return InvertedFlyingRCTrackDiagBrakes;
        case TrackElemType::diagBlockBrakes:
            return InvertedFlyingRCTrackDiagBlockBrakes;

        // flat to steep transitions
        // large
        case TrackElemType::flatToUp60LongBase:
            return InvertedFlyingRCTrackFlatTo60DegUpLongBase;
        case TrackElemType::up60ToFlatLongBase:
            return InvertedFlyingRCTrack60DegUpToFlatLongBase;
        case TrackElemType::down60ToFlatLongBase:
            return InvertedFlyingRCTrack60DegDownToFlatLongBase;
        case TrackElemType::flatToDown60LongBase:
            return InvertedFlyingRCTrackFlatTo60DegDownLongBase;

        case TrackElemType::diagFlatToUp60LongBase:
            return InvertedFlyingRCTrackDiagFlatTo60DegUpLongBase;
        case TrackElemType::diagUp60ToFlatLongBase:
            return InvertedFlyingRCTrackDiag60DegUpToFlatLongBase;
        case TrackElemType::diagFlatToDown60LongBase:
            return InvertedFlyingRCTrackDiagFlatTo60DegDownLongBase;
        case TrackElemType::diagDown60ToFlatLongBase:
            return InvertedFlyingRCTrackDiag60DegDownToFlatLongBase;

            // small
        case TrackElemType::flatToUp60:
            return InvertedFlyingRCTrackFlatTo60DegUp;
        case TrackElemType::up60ToFlat:
            return InvertedFlyingRCTrack60DegUpToFlat;
        case TrackElemType::flatToDown60:
            return InvertedFlyingRCTrackFlatTo60DegDown;
        case TrackElemType::down60ToFlat:
            return InvertedFlyingRCTrack60DegDownToFlat;
        case TrackElemType::diagFlatToUp60:
            return InvertedFlyingRCTrackDiagFlatTo60DegUp;
        case TrackElemType::diagUp60ToFlat:
            return InvertedFlyingRCTrackDiag60DegUpToFlat;
        case TrackElemType::diagFlatToDown60:
            return InvertedFlyingRCTrackDiagFlatTo60DegDown;
        case TrackElemType::diagDown60ToFlat:
            return InvertedFlyingRCTrackDiag60DegDownToFlat;

        // banked to sloped small turns
        case TrackElemType::leftBankToLeftQuarterTurn3TilesUp25:
            return InvertedFlyingRCTrackLeftBankToLeftQuarterTurn3Tile25DegUp;
        case TrackElemType::rightBankToRightQuarterTurn3TilesUp25:
            return InvertedFlyingRCTrackRightBankToRightQuarterTurn3Tile25DegUp;
        case TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank:
            return InvertedFlyingRCTrackLeftQuarterTurn3Tile25DegDownToLeftBank;
        case TrackElemType::rightQuarterTurn3TilesDown25ToRightBank:
            return InvertedFlyingRCTrackRightQuarterTurn3Tile25DegDownToRightBank;

        // vertical slopes and transitions
        case TrackElemType::up60ToUp90:
            return InvertedFlyingRCTrack60DegUpTo90DegUp;
        case TrackElemType::down90ToDown60:
            return InvertedFlyingRCTrack90DegDownTo60DegDown;
        case TrackElemType::up90ToUp60:
            return InvertedFlyingRCTrack90DegUpTo60DegUp;
        case TrackElemType::down60ToDown90:
            return InvertedFlyingRCTrack60DegDownTo90DegDown;

        // Vertical turns
        case TrackElemType::leftQuarterTurn1TileUp90:
            return InvertedFlyingRCTrackLeftQuarterTurn1TileUp90;
        case TrackElemType::rightQuarterTurn1TileUp90:
            return InvertedFlyingRCTrackRightQuarterTurn1TileUp90;
        case TrackElemType::leftQuarterTurn1TileDown90:
            return InvertedFlyingRCTrackLeftQuarterTurn1TileDown90;
        case TrackElemType::rightQuarterTurn1TileDown90:
            return InvertedFlyingRCTrackRightQuarterTurn1TileDown90;

        // quarter loops
        case TrackElemType::multiDimInvertedFlatToDown90QuarterLoop:
            return InvertedFlyingRCTrackInvertedFlatTo90DegQuarterLoopDown;
        case TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop:
            return InvertedFlyingRCTrack90DegToInvertedFlatQuarterLoopUp;

        // wide sloped turns
        case TrackElemType::leftEighthToDiagUp25:
            return InvertedFlyingRCTrackLeftEighthToDiagUp25;
        case TrackElemType::leftEighthToOrthogonalUp25:
            return InvertedFlyingRCTrackLeftEighthToOrthogonalUp25;
        case TrackElemType::rightEighthToDiagUp25:
            return InvertedFlyingRCTrackRightEighthToDiagUp25;
        case TrackElemType::rightEighthToOrthogonalUp25:
            return InvertedFlyingRCTrackRightEighthToOrthogonalUp25;
        case TrackElemType::leftEighthToDiagDown25:
            return InvertedFlyingRCTrackLeftEighthToDiagDown25;
        case TrackElemType::leftEighthToOrthogonalDown25:
            return InvertedFlyingRCTrackLeftEighthToOrthogonalDown25;
        case TrackElemType::rightEighthToDiagDown25:
            return InvertedFlyingRCTrackRightEighthToDiagDown25;
        case TrackElemType::rightEighthToOrthogonalDown25:
            return InvertedFlyingRCTrackRightEighthToOrthogonalDown25;

        // wide sloped banked turns
        case TrackElemType::leftEighthBankToDiagUp25:
            return InvertedFlyingRCTrackLeftEighthBankToDiagUp25;
        case TrackElemType::rightEighthBankToDiagUp25:
            return InvertedFlyingRCTrackRightEighthBankToDiagUp25;
        case TrackElemType::leftEighthBankToDiagDown25:
            return InvertedFlyingRCTrackLeftEighthBankToDiagDown25;
        case TrackElemType::rightEighthBankToDiagDown25:
            return InvertedFlyingRCTrackRightEighthBankToDiagDown25;
        case TrackElemType::leftEighthBankToOrthogonalUp25:
            return InvertedFlyingRCTrackLeftEighthBankToOrthogonalUp25;
        case TrackElemType::rightEighthBankToOrthogonalUp25:
            return InvertedFlyingRCTrackRightEighthBankToOrthogonalUp25;
        case TrackElemType::leftEighthBankToOrthogonalDown25:
            return InvertedFlyingRCTrackLeftEighthBankToOrthogonalDown25;
        case TrackElemType::rightEighthBankToOrthogonalDown25:
            return InvertedFlyingRCTrackRightEighthBankToOrthogonalDown25;

        // diag bank and slopes
        case TrackElemType::diagUp25ToLeftBankedUp25:
            return InvertedFlyingRCTrackDiagUp25ToLeftBankedUp25;
        case TrackElemType::diagUp25ToRightBankedUp25:
            return InvertedFlyingRCTrackDiagUp25ToRightBankedUp25;
        case TrackElemType::diagLeftBankedUp25ToUp25:
            return InvertedFlyingRCTrackDiagLeftBankedUp25ToUp25;
        case TrackElemType::diagRightBankedUp25ToUp25:
            return InvertedFlyingRCTrackDiagRightBankedUp25ToUp25;
        case TrackElemType::diagDown25ToLeftBankedDown25:
            return InvertedFlyingRCTrackDiagDown25ToLeftBankedDown25;
        case TrackElemType::diagDown25ToRightBankedDown25:
            return InvertedFlyingRCTrackDiagDown25ToRightBankedDown25;
        case TrackElemType::diagLeftBankedDown25ToDown25:
            return InvertedFlyingRCTrackDiagLeftBankedDown25ToDown25;
        case TrackElemType::diagRightBankedDown25ToDown25:
            return InvertedFlyingRCTrackDiagRightBankedDown25ToDown25;
        case TrackElemType::diagLeftBankedFlatToLeftBankedUp25:
            return InvertedFlyingRCTrackDiagLeftBankedFlatToLeftBankedUp25;
        case TrackElemType::diagRightBankedFlatToRightBankedUp25:
            return InvertedFlyingRCTrackDiagRightBankedFlatToRightBankedUp25;
        case TrackElemType::diagLeftBankedUp25ToLeftBankedFlat:
            return InvertedFlyingRCTrackDiagLeftBankedUp25ToLeftBankedFlat;
        case TrackElemType::diagRightBankedUp25ToRightBankedFlat:
            return InvertedFlyingRCTrackDiagRightBankedUp25ToRightBankedFlat;
        case TrackElemType::diagLeftBankedFlatToLeftBankedDown25:
            return InvertedFlyingRCTrackDiagLeftBankedFlatToLeftBankedDown25;
        case TrackElemType::diagRightBankedFlatToRightBankedDown25:
            return InvertedFlyingRCTrackDiagRightBankedFlatToRightBankedDown25;
        case TrackElemType::diagLeftBankedDown25ToLeftBankedFlat:
            return InvertedFlyingRCTrackDiagLeftBankedDown25ToLeftBankedFlat;
        case TrackElemType::diagRightBankedDown25ToRightBankedFlat:
            return InvertedFlyingRCTrackDiagRightBankedDown25ToRightBankedFlat;
        case TrackElemType::diagUp25LeftBanked:
            return InvertedFlyingRCTrackDiagUp25LeftBanked;
        case TrackElemType::diagUp25RightBanked:
            return InvertedFlyingRCTrackDiagUp25RightBanked;
        case TrackElemType::diagDown25LeftBanked:
            return InvertedFlyingRCTrackDiagDown25LeftBanked;
        case TrackElemType::diagDown25RightBanked:
            return InvertedFlyingRCTrackDiagDown25RightBanked;
        case TrackElemType::diagFlatToLeftBankedUp25:
            return InvertedFlyingRCTrackDiagFlatToLeftBankedUp25;
        case TrackElemType::diagFlatToRightBankedUp25:
            return InvertedFlyingRCTrackDiagFlatToRightBankedUp25;
        case TrackElemType::diagLeftBankedUp25ToFlat:
            return InvertedFlyingRCTrackDiagLeftBankedUp25ToFlat;
        case TrackElemType::diagRightBankedUp25ToFlat:
            return InvertedFlyingRCTrackDiagRightBankedUp25ToFlat;
        case TrackElemType::diagFlatToLeftBankedDown25:
            return InvertedFlyingRCTrackDiagFlatToLeftBankedDown25;
        case TrackElemType::diagFlatToRightBankedDown25:
            return InvertedFlyingRCTrackDiagFlatToRightBankedDown25;
        case TrackElemType::diagLeftBankedDown25ToFlat:
            return InvertedFlyingRCTrackDiagLeftBankedDown25ToFlat;
        case TrackElemType::diagRightBankedDown25ToFlat:
            return InvertedFlyingRCTrackDiagRightBankedDown25ToFlat;

        // Unbanked Helix
        case TrackElemType::leftQuarterHelixLargeUp:
            return InvertedFlyingRCTrackLeftQuarterHelixLargeUp;
        case TrackElemType::rightQuarterHelixLargeUp:
            return InvertedFlyingRCTrackRightQuarterHelixLargeUp;
        case TrackElemType::leftQuarterHelixLargeDown:
            return InvertedFlyingRCTrackLeftQuarterHelixLargeDown;
        case TrackElemType::rightQuarterHelixLargeDown:
            return InvertedFlyingRCTrackRightQuarterHelixLargeDown;

        default:
            return GetTrackPaintFunctionFlyingRC(trackType);
    }
}
