/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../drawing/ImageId.hpp"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Location.hpp"
#include "../../Boundbox.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"

#include <cstdint>

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

struct WoodenTrackSection
{
    ImageIndex track;
    ImageIndex handrail = kImageIndexUndefined;
    ImageIndex frontTrack = kImageIndexUndefined;
    ImageIndex frontHandrail = kImageIndexUndefined;
};

struct SpriteBoundBox2
{
    ImageIndex ImageIdA;
    ImageIndex ImageIdB;
    CoordsXYZ offset;
    ::BoundBoxXYZ BoundBox;
};

// Magic number 4 refers to the number of track blocks in a diagonal track element
static constexpr const WoodenSupportSubType kWoodenRCDiagonalSupports[4][kNumOrthogonalDirections] = {
    { WoodenSupportSubType::Null, WoodenSupportSubType::Null, WoodenSupportSubType::Null,
      WoodenSupportSubType::Null }, // sequence 0
    { WoodenSupportSubType::Corner0, WoodenSupportSubType::Corner1, WoodenSupportSubType::Corner2,
      WoodenSupportSubType::Corner3 }, // sequence 1
    { WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner0,
      WoodenSupportSubType::Corner1 }, // sequence 2
    { WoodenSupportSubType::Null, WoodenSupportSubType::Null, WoodenSupportSubType::Null,
      WoodenSupportSubType::Null } // sequence 3
};

template<bool isClassic>
ImageId WoodenRCGetTrackColour(const PaintSession& session)
{
    if (isClassic)
        return session.TrackColours;
    else
        return session.SupportColours;
}

ImageId WoodenRCGetRailsColour(PaintSession& session);

template<bool isClassic>
PaintStruct* WoodenRCTrackPaint(
    PaintSession& session, uint8_t direction, ImageIndex imageIdTrack, ImageIndex imageIdRails, const CoordsXYZ& offset,
    const BoundBoxXYZ& boundBox)
{
    if (isClassic)
    {
        const ImageId imageId = session.TrackColours.WithIndex(imageIdTrack);

        return PaintAddImageAsParentRotated(session, direction, imageId, offset, boundBox);
    }
    else
    {
        const ImageId imageId = session.SupportColours.WithIndex(imageIdTrack);
        const ImageId railsImageId = WoodenRCGetRailsColour(session).WithIndex(imageIdRails);

        PaintAddImageAsParentRotated(session, direction, imageId, offset, boundBox);
        return PaintAddImageAsChildRotated(session, direction, railsImageId, offset, boundBox);
    }
}

template<bool isClassic>
void WoodenRCTrackPaintBb(PaintSession& session, const SpriteBoundBox2* bb, int16_t height)
{
    if (bb->ImageIdA == 0)
        return;

    ImageId imageId = WoodenRCGetTrackColour<isClassic>(session).WithIndex(bb->ImageIdA);
    PaintAddImageAsParent(
        session, imageId, { bb->offset.x, bb->offset.y, height + bb->offset.z },
        { { bb->BoundBox.offset.x, bb->BoundBox.offset.y, height + bb->BoundBox.offset.z }, bb->BoundBox.length });
    if (bb->ImageIdB != 0)
    {
        ImageId railsImageId = WoodenRCGetRailsColour(session).WithIndex(bb->ImageIdB);
        PaintAddImageAsChild(
            session, railsImageId, { bb->offset.x, bb->offset.y, height + bb->offset.z },
            { { bb->BoundBox.offset, height + bb->BoundBox.offset.z }, bb->BoundBox.length });
    }
}

template<bool isClassic, std::array<WoodenTrackSection, kNumOrthogonalDirections> imageIds>
static void WoodenRCTrackStraightBankTrack(PaintSession& session, uint8_t direction, int32_t height)
{
    WoodenRCTrackPaint<isClassic>(
        session, direction, imageIds[direction].track, imageIds[direction].handrail, { 0, 0, height },
        { { 0, 3, height }, { 32, 25, 2 } });
    if (imageIds[direction].frontTrack != kImageIndexUndefined)
    {
        WoodenRCTrackPaint<isClassic>(
            session, direction, imageIds[direction].frontTrack, imageIds[direction].frontHandrail, { 0, 0, height },
            { { 0, 26, height + 5 }, { 32, 1, 9 } });
    }
}

/** rct2: 0x008AC658, 0x008AC668, 0x008AC738 */
template<bool isClassic, std::array<WoodenTrackSection, kNumOrthogonalDirections> imageIds>
void WoodenRCTrackFlatToBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackStraightBankTrack<isClassic, imageIds>(session, direction, height);
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AC6D8, 0x008AC6E8 */
template<bool isClassic, std::array<WoodenTrackSection, kNumOrthogonalDirections> imageIds>
static void WoodenRCTrack25DegUpToBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackStraightBankTrack<isClassic, imageIds>(session, direction, height);
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
template<bool isClassic, std::array<WoodenTrackSection, kNumOrthogonalDirections> imageIds>
static void WoodenRCTrackBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackStraightBankTrack<isClassic, imageIds>(session, direction, height);
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
template<bool isClassic, std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 3> imageIds>
static void WoodenRCTrackLeftQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
                    break;
            }
    }

    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    static constexpr int blockedSegments[4] = {
        kSegmentsAll,
        kSegmentsAll,
        EnumsToFlags(PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide),
        kSegmentsAll,
    };

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftBankedQuarterTurn3Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

template<bool isClassic, std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 5> imageIds>
static void WoodenRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 2, height }, { 32, 32, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 32, 0 } });
                    }
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 2, height }, { 32, 32, 2 } });
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    }
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    }
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 32, 16, 0 } });
                    }
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    break;
            }
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 2 } });
                    if (imageIds[3][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][direction].frontTrack, imageIds[3][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 0, height + 27 }, { 16, 32, 0 } });
                    }
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 2 } });
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 2 } });
                    if (imageIds[3][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][direction].frontTrack, imageIds[3][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 16, 32, 0 } });
                    }
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 2 } });
                    break;
            }
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 2, 0, height }, { 32, 32, 2 } });
                    if (imageIds[4][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[4][direction].frontTrack, imageIds[4][direction].frontHandrail,
                            { 0, 0, height }, { { 2, 0, height + 27 }, { 32, 32, 0 } });
                    }
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 2 } });
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 2 } });
                    if (imageIds[4][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[4][direction].frontTrack, imageIds[4][direction].frontHandrail,
                            { 0, 0, height }, { { 2, 0, height + 27 }, { 27, 32, 0 } });
                    }
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 2, 0, height }, { 32, 32, 2 } });
                    break;
            }
            break;
    }

    TrackPaintUtilRightQuarterTurn5TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    static constexpr int blockedSegments[7] = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide),
        EnumsToFlags(
            PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
            PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
        EnumsToFlags(
            PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::centre,
            PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
        EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide),
        EnumsToFlags(
            PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
            PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
        kSegmentsAll,
    };

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::BankedRightQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008ACAB8 */
template<bool isClassic, std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 3> imageIds>
static void WoodenRCTrackLeftHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
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
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
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
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    }
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
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][3].track, imageIds[0][3].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[0][3].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][3].frontTrack, imageIds[0][3].frontHandrail, { 0, 0, height },
                            { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][0].track, imageIds[0][0].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][1].track, imageIds[0][1].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[0][1].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][1].frontTrack, imageIds[0][1].frontHandrail, { 0, 0, height },
                            { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][2].track, imageIds[0][2].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
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
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][3].track, imageIds[1][3].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    if (imageIds[1][3].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][3].frontTrack, imageIds[1][3].frontHandrail, { 0, 0, height },
                            { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][0].track, imageIds[1][0].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][1].track, imageIds[1][1].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[1][1].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][1].frontTrack, imageIds[1][1].frontHandrail, { 0, 0, height },
                            { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][2].track, imageIds[1][2].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    break;
            }
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
        case 7:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][3].track, imageIds[2][3].handrail, { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    if (imageIds[2][3].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][3].frontTrack, imageIds[2][3].frontHandrail, { 0, 0, height },
                            { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][0].track, imageIds[2][0].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][1].track, imageIds[2][1].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[2][1].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][1].frontTrack, imageIds[2][1].frontHandrail, { 0, 0, height },
                            { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][2].track, imageIds[2][2].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::bottomCorner,
                        PaintSegment::topLeftSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008ACAC8 */
template<bool isClassic, std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 3> imageIds>
static void WoodenRCTrackRightHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::bottomCorner,
                        PaintSegment::topLeftSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
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
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    break;
            }
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
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][1].track, imageIds[0][1].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][2].track, imageIds[0][2].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[0][2].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][2].frontTrack, imageIds[0][2].frontHandrail, { 0, 0, height },
                            { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][3].track, imageIds[0][3].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][0].track, imageIds[0][0].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[0][0].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][0].frontTrack, imageIds[0][0].frontHandrail, { 0, 0, height },
                            { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
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
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][1].track, imageIds[1][1].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][2].track, imageIds[1][2].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[1][2].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][2].frontTrack, imageIds[1][2].frontHandrail, { 0, 0, height },
                            { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][3].track, imageIds[1][3].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][0].track, imageIds[1][0].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    if (imageIds[1][0].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][0].frontTrack, imageIds[1][0].frontHandrail, { 0, 0, height },
                            { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    }
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
        case 7:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][1].track, imageIds[2][1].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][2].track, imageIds[2][2].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[2][2].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][2].frontTrack, imageIds[2][2].frontHandrail, { 0, 0, height },
                            { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][3].track, imageIds[2][3].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][0].track, imageIds[2][0].handrail, { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    if (imageIds[2][0].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][0].frontTrack, imageIds[2][0].frontHandrail, { 0, 0, height },
                            { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
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
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008ACAF8 */
template<bool isClassic, std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 5> imageIds>
static void WoodenRCTrackLeftHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
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
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 32, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
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
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 16, height + 29 }, { 16, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::bottomCorner, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                48, 0x20);
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
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 2 } });
                    if (imageIds[3][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][direction].frontTrack, imageIds[3][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 33 }, { 16, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 2 } });
                    if (imageIds[3][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][direction].frontTrack, imageIds[3][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 0, height + 27 }, { 16, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
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
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 2 } });
                    if (imageIds[4][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[4][direction].frontTrack, imageIds[4][direction].frontHandrail,
                            { 0, 0, height }, { { 2, 0, height + 33 }, { 27, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 2 } });
                    if (imageIds[4][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[4][direction].frontTrack, imageIds[4][direction].frontHandrail,
                            { 0, 0, height }, { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][3].track, imageIds[0][3].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[0][3].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][3].frontTrack, imageIds[0][3].frontHandrail, { 0, 0, height },
                            { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][0].track, imageIds[0][0].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][1].track, imageIds[0][1].handrail, { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 2 } });
                    if (imageIds[0][1].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][1].frontTrack, imageIds[0][1].frontHandrail, { 0, 0, height },
                            { { 2, 0, height + 27 }, { 27, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][2].track, imageIds[0][2].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 8:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][3].track, imageIds[1][3].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 2 } });
                    if (imageIds[1][3].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][3].frontTrack, imageIds[1][3].frontHandrail, { 0, 0, height },
                            { { 16, 0, height + 27 }, { 16, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][0].track, imageIds[1][0].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][1].track, imageIds[1][1].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 2 } });
                    if (imageIds[1][1].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][1].frontTrack, imageIds[1][1].frontHandrail, { 0, 0, height },
                            { { 0, 0, height + 27 }, { 16, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][2].track, imageIds[1][2].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
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
        case 10:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][3].track, imageIds[2][3].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[2][3].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][3].frontTrack, imageIds[2][3].frontHandrail, { 0, 0, height },
                            { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][0].track, imageIds[2][0].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][1].track, imageIds[2][1].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    if (imageIds[2][1].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][1].frontTrack, imageIds[2][1].frontHandrail, { 0, 0, height },
                            { { 16, 16, height + 29 }, { 16, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][2].track, imageIds[2][2].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 11:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][3].track, imageIds[3][3].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    if (imageIds[3][3].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][3].frontTrack, imageIds[3][3].frontHandrail, { 0, 0, height },
                            { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][0].track, imageIds[3][0].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][1].track, imageIds[3][1].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    if (imageIds[3][1].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][1].frontTrack, imageIds[3][1].frontHandrail, { 0, 0, height },
                            { { 0, 0, height + 33 }, { 32, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][2].track, imageIds[3][2].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
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
        case 13:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][3].track, imageIds[4][3].handrail, { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    if (imageIds[4][3].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[4][3].frontTrack, imageIds[4][3].frontHandrail, { 0, 0, height },
                            { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][0].track, imageIds[4][0].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][1].track, imageIds[4][1].handrail, { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    if (imageIds[4][1].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[4][1].frontTrack, imageIds[4][1].frontHandrail, { 0, 0, height },
                            { { 0, 2, height + 33 }, { 32, 27, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][2].track, imageIds[4][2].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::bottomCorner,
                        PaintSegment::topLeftSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008ACB08 */
template<bool isClassic, std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 5> imageIds>
static void WoodenRCTrackRightHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::bottomCorner,
                        PaintSegment::topLeftSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 32, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
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
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 16, height + 29 }, { 16, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 2 } });
                    if (imageIds[3][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][direction].frontTrack, imageIds[3][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 0, height + 27 }, { 16, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 2 } });
                    if (imageIds[3][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][direction].frontTrack, imageIds[3][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 33 }, { 16, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
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
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 2 } });
                    if (imageIds[4][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[4][direction].frontTrack, imageIds[4][direction].frontHandrail,
                            { 0, 0, height }, { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 2 } });
                    if (imageIds[4][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[4][direction].frontTrack, imageIds[4][direction].frontHandrail,
                            { 0, 0, height }, { { 2, 0, height + 33 }, { 27, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][direction].track, imageIds[4][direction].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][1].track, imageIds[0][1].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][2].track, imageIds[0][2].handrail, { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 2 } });
                    if (imageIds[0][2].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][2].frontTrack, imageIds[0][2].frontHandrail, { 0, 0, height },
                            { { 2, 0, height + 27 }, { 27, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][3].track, imageIds[0][3].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][0].track, imageIds[0][0].handrail, { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[0][0].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][0].frontTrack, imageIds[0][0].frontHandrail, { 0, 0, height },
                            { { 6, 0, height + 27 }, { 20, 32, 0 } });
                    }
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
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner,
                        PaintSegment::topRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 8:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][1].track, imageIds[1][1].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][2].track, imageIds[1][2].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 2 } });
                    if (imageIds[1][2].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][2].frontTrack, imageIds[1][2].frontHandrail, { 0, 0, height },
                            { { 0, 0, height + 27 }, { 16, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][3].track, imageIds[1][3].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][0].track, imageIds[1][0].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 2 } });
                    if (imageIds[1][0].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][0].frontTrack, imageIds[1][0].frontHandrail, { 0, 0, height },
                            { { 16, 0, height + 27 }, { 16, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
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
        case 10:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][1].track, imageIds[2][1].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][2].track, imageIds[2][2].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    if (imageIds[2][2].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][2].frontTrack, imageIds[2][2].frontHandrail, { 0, 0, height },
                            { { 16, 16, height + 29 }, { 16, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][3].track, imageIds[2][3].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][0].track, imageIds[2][0].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[2][0].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][0].frontTrack, imageIds[2][0].frontHandrail, { 0, 0, height },
                            { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::bottomCorner, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                48, 0x20);
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
        case 11:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][1].track, imageIds[3][1].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][2].track, imageIds[3][2].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    if (imageIds[3][2].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][2].frontTrack, imageIds[3][2].frontHandrail, { 0, 0, height },
                            { { 0, 0, height + 33 }, { 32, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][3].track, imageIds[3][3].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][0].track, imageIds[3][0].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    if (imageIds[3][0].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][0].frontTrack, imageIds[3][0].frontHandrail, { 0, 0, height },
                            { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
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
        case 13:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][1].track, imageIds[4][1].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][2].track, imageIds[4][2].handrail, { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    if (imageIds[4][2].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[4][2].frontTrack, imageIds[4][2].frontHandrail, { 0, 0, height },
                            { { 0, 2, height + 33 }, { 32, 27, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][3].track, imageIds[4][3].handrail, { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[4][0].track, imageIds[4][0].handrail, { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 2 } });
                    if (imageIds[4][0].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[4][0].frontTrack, imageIds[4][0].frontHandrail, { 0, 0, height },
                            { { 0, 6, height + 27 }, { 32, 20, 0 } });
                    }
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
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner,
                        PaintSegment::bottomRightSide),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008ACB38 */
template<bool isClassic, std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 2> imageIds>
static void WoodenRCTrackLeftBankToLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 6, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 6, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 6, height }, { { 0, 6, height + 67 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 6, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 6, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 6, height }, { { 0, 6, height + 67 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
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
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 6, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 6, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 6, 0, height }, { { 6, 0, height + 67 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 6, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 6, 0, height }, { { 6, 0, height + 67 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 6, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 6, 0, height }, { { 6, 0, height + 67 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
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

/** rct2: 0x008ACB48 */
template<bool isClassic, std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 2> imageIds>
static void WoodenRCTrackRightBankToRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 6, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 6, height }, { { 0, 6, height + 67 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 6, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 6, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 6, height }, { { 0, 6, height + 67 }, { 32, 20, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 6, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
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
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 6, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 6, 0, height }, { { 6, 0, height + 67 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 6, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 6, 0, height }, { { 6, 0, height + 67 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 6, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 6, 0, height }, { { 6, 0, height + 67 }, { 20, 32, 0 } });
                    }
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 6, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
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
template<bool isClassic, std::array<WoodenTrackSection, kNumOrthogonalDirections> imageIds>
static void WoodenRCTrackDiagFlatToBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    if (imageIds[direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[direction].frontTrack, imageIds[direction].frontHandrail,
                            { -16, -16, height }, { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    }
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    if (imageIds[direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[direction].frontTrack, imageIds[direction].frontHandrail,
                            { -16, -16, height }, { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    }
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008ACA58, 0x008ACA68 */
template<bool isClassic, std::array<WoodenTrackSection, kNumOrthogonalDirections> imageIds>
static void WoodenRCTrackDiagBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    if (imageIds[direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[direction].frontTrack, imageIds[direction].frontHandrail,
                            { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    }
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    if (imageIds[direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[direction].frontTrack, imageIds[direction].frontHandrail,
                            { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    }
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagFlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008ACA38, 0x008ACA48 */
template<bool isClassic, std::array<WoodenTrackSection, kNumOrthogonalDirections> imageIds>
static void WoodenRCTrackDiagUp25ToBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    if (imageIds[direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[direction].frontTrack, imageIds[direction].frontHandrail,
                            { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    }
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    if (imageIds[direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[direction].frontTrack, imageIds[direction].frontHandrail,
                            { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    }
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AC9D8 */
template<bool isClassic, std::array<WoodenTrackSection, kNumOrthogonalDirections> imageIds>
static void WoodenRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    if (imageIds[direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[direction].frontTrack, imageIds[direction].frontHandrail,
                            { -16, -16, height }, { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    }
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    if (imageIds[direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[direction].frontTrack, imageIds[direction].frontHandrail,
                            { -16, -16, height }, { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    }
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[direction].track, imageIds[direction].handrail, { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 2 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AC998 */
template<bool isClassic, std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 4> imageIds>
static void WoodenRCTrackLeftEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    }
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    }
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 32, 16, 0 } });
                    }
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    }
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
            }
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 2 } });
                    if (imageIds[3][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][direction].frontTrack, imageIds[3][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 16, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    if (imageIds[3][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][direction].frontTrack, imageIds[3][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
            }
            break;
    }

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftEighthBankToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AC9A8 */
template<bool isClassic, std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 4> imageIds>
static void WoodenRCTrackRightEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    }
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    if (imageIds[0][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[0][direction].frontTrack, imageIds[0][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 32, 32, 0 } });
                    }
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[0][direction].track, imageIds[0][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 2 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 16, height + 27 }, { 32, 16, 0 } });
                    }
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 2 } });
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 2 } });
                    if (imageIds[1][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[1][direction].frontTrack, imageIds[1][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 32, 16, 0 } });
                    }
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[1][direction].track, imageIds[1][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 2 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 4, 4, height }, { 28, 28, 2 } });
                    if (imageIds[2][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[2][direction].frontTrack, imageIds[2][direction].frontHandrail,
                            { 0, 0, height }, { { 4, 4, height + 27 }, { 28, 28, 0 } });
                    }
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[2][direction].track, imageIds[2][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    break;
            }
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    if (imageIds[3][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][direction].frontTrack, imageIds[3][direction].frontHandrail,
                            { 0, 0, height }, { { 16, 0, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 1:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    break;
                case 2:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 2 } });
                    if (imageIds[3][direction].frontTrack != kImageIndexUndefined)
                    {
                        WoodenRCTrackPaint<isClassic>(
                            session, direction, imageIds[3][direction].frontTrack, imageIds[3][direction].frontHandrail,
                            { 0, 0, height }, { { 0, 16, height + 27 }, { 16, 16, 0 } });
                    }
                    break;
                case 3:
                    WoodenRCTrackPaint<isClassic>(
                        session, direction, imageIds[3][direction].track, imageIds[3][direction].handrail, { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightEighthBankToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

TrackPaintFunction GetTrackPaintFunctionClassicWoodenRCFallback(OpenRCT2::TrackElemType trackType);
