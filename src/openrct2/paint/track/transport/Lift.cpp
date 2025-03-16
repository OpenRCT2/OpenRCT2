/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

enum
{
    SPR_LIFT_CAGE_BACK = 14994,
    SPR_LIFT_CAGE_FRONT = 14995,
    SPR_LIFT_CAGE_NE_BACK = 14996,
    SPR_LIFT_CAGE_NE_FRONT = 14997,
    SPR_LIFT_CAGE_SE_BACK = 14998,
    SPR_LIFT_CAGE_SE_FRONT = 14999,
    SPR_LIFT_CAGE_SW_BACK = 15000,
    SPR_LIFT_CAGE_SW_FRONT = 15001,
    SPR_LIFT_CAGE_NW_BACK = 15002,
    SPR_LIFT_CAGE_NW_FRONT = 15003,
};

static constexpr uint32_t kLiftCageSprites[][2] = {
    { SPR_LIFT_CAGE_BACK, SPR_LIFT_CAGE_FRONT },       { SPR_LIFT_CAGE_NE_BACK, SPR_LIFT_CAGE_NE_FRONT },
    { SPR_LIFT_CAGE_SE_BACK, SPR_LIFT_CAGE_SE_FRONT }, { SPR_LIFT_CAGE_SW_BACK, SPR_LIFT_CAGE_SW_FRONT },
    { SPR_LIFT_CAGE_NW_BACK, SPR_LIFT_CAGE_NW_FRONT },
};

static void PaintLiftCage(PaintSession& session, int8_t index, ImageId colourFlags, int32_t height, uint8_t rotation)
{
    ImageId imageId;

    imageId = colourFlags.WithIndex(kLiftCageSprites[1 + index][0]);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 2, height }, { 2, 2, 30 } });

    imageId = colourFlags.WithIndex(kLiftCageSprites[1 + index][1]);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 28, height }, { 2, 2, 30 } });
}

/** rct2: 0x0076C6CC */
static void PaintLiftBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kTrackMap3x3[direction][trackSequence];

    if (trackSequence == 0)
    {
        PaintLiftCage(session, direction, session.TrackColours, height, session.CurrentRotation);

        PaintLiftCage(session, -1, session.TrackColours, height + 32, session.CurrentRotation);

        PaintLiftCage(session, -1, session.TrackColours, height + 64, session.CurrentRotation);

        PaintUtilSetVerticalTunnel(session, height + 96);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

        PaintUtilSetGeneralSupportHeight(session, height + 96);

        return;
    }

    int32_t edges = kEdges3x3[trackSequence];

    auto imageId = session.SupportColours.WithIndex(SPR_FLOOR_METAL_B);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours, height, kFenceSpritesMetalB,
        session.CurrentRotation);

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::leftCorner, PaintSegment::topLeftSide, PaintSegment::topCorner, PaintSegment::topRightSide,
                PaintSegment::rightCorner);
            break;
        case 2:
            blockedSegments = EnumsToFlags(PaintSegment::topCorner, PaintSegment::topRightSide, PaintSegment::rightCorner);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::topCorner, PaintSegment::topRightSide, PaintSegment::rightCorner, PaintSegment::bottomRightSide,
                PaintSegment::bottomCorner);
            break;
        case 4:
            blockedSegments = EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::leftCorner);
            break;
        case 5:
            blockedSegments = EnumsToFlags(
                PaintSegment::rightCorner, PaintSegment::bottomRightSide, PaintSegment::bottomCorner);
            break;
        case 6:
            blockedSegments = EnumsToFlags(
                PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::leftCorner, PaintSegment::bottomLeftSide,
                PaintSegment::bottomCorner);
            break;
        case 7:
            blockedSegments = EnumsToFlags(
                PaintSegment::leftCorner, PaintSegment::bottomLeftSide, PaintSegment::bottomCorner,
                PaintSegment::bottomRightSide, PaintSegment::rightCorner);
            break;
        case 8:
            blockedSegments = EnumsToFlags(PaintSegment::leftCorner, PaintSegment::bottomLeftSide, PaintSegment::bottomCorner);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, blockedSegments, 0xFFFF, 0);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~blockedSegments, height + 2, 0x20);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0076C6DC */
static void PaintLiftTowerSection(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence == 1)
    {
        return;
    }

    PaintLiftCage(session, -1, session.TrackColours, height, session.CurrentRotation);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

    PaintUtilSetVerticalTunnel(session, height + 32);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x0076C5BC
 */
TrackPaintFunction GetTrackPaintFunctionLift(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::TowerBase:
            return PaintLiftBase;

        case TrackElemType::TowerSection:
            return PaintLiftTowerSection;
        default:
            return TrackPaintFunctionDummy;
    }
}
