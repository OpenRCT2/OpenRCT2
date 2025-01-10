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
#include "../../support/MetalSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

enum
{
    SPR_DINGHY_SLIDE_FLAT_COVERED_SW_NE = 19736,
    SPR_DINGHY_SLIDE_FLAT_COVERED_NW_SE = 19737,
    SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_SW_NE = 19738,
    SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_NW_SE = 19739,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_SW_NE = 19812,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_NW_SE = 19813,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_NE_SW = 19814,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_SE_NW = 19815,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_SW_NE = 19816,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_NW_SE = 19817,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_NE_SW = 19818,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_SE_NW = 19819,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_SW_NE = 19820,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_NW_SE = 19821,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_NE_SW = 19822,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_SE_NW = 19823,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_SW_NE = 19824,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_NW_SE = 19825,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_NE_SW = 19826,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_SE_NW = 19827,
    SPR_DINGHY_SLIDE_25_DEG_COVERED_SW_NE = 19828,
    SPR_DINGHY_SLIDE_25_DEG_COVERED_NW_SE = 19829,
    SPR_DINGHY_SLIDE_25_DEG_COVERED_NE_SW = 19830,
    SPR_DINGHY_SLIDE_25_DEG_COVERED_SE_NW = 19831,
    SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_SW_NE = 19832,
    SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_NW_SE = 19833,
    SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_NE_SW = 19834,
    SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_SE_NW = 19835,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_SW_NE = 19836,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_NW_SE = 19837,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_NE_SW = 19838,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_SE_NW = 19839,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_SW_NE = 19840,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_NW_SE = 19841,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_NE_SW = 19842,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_SE_NW = 19843,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_SW_NE = 19844,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_NW_SE = 19845,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_NE_SW = 19846,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_SE_NW = 19847,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_SW_NE = 19848,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_NW_SE = 19849,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_NE_SW = 19850,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_SE_NW = 19851,
    SPR_DINGHY_SLIDE_60_DEG_COVERED_SW_NE = 19852,
    SPR_DINGHY_SLIDE_60_DEG_COVERED_NW_SE = 19853,
    SPR_DINGHY_SLIDE_60_DEG_COVERED_NE_SW = 19854,
    SPR_DINGHY_SLIDE_60_DEG_COVERED_SE_NW = 19855,
    SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_SW_NE = 19856,
    SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_NW_SE = 19857,
    SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_NE_SW = 19858,
    SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_SE_NW = 19859,

    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_0 = 19884,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_1 = 19885,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_2 = 19886,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_0 = 19887,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_1 = 19888,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_2 = 19889,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_0 = 19890,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_1 = 19891,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_2 = 19892,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_0 = 19893,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_1 = 19894,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_2 = 19895,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_0 = 19896,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_1 = 19897,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_2 = 19898,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_0 = 19899,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_1 = 19900,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_2 = 19901,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_0 = 19902,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_1 = 19903,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_2 = 19904,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_0 = 19905,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_1 = 19906,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_2 = 19907,

    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_0 = 19948,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_1 = 19949,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_2 = 19950,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_3 = 19951,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_4 = 19952,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_0 = 19953,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_1 = 19954,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_2 = 19955,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_3 = 19956,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_4 = 19957,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_0 = 19958,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_1 = 19959,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_2 = 19960,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_3 = 19961,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_4 = 19962,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_0 = 19963,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_1 = 19964,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_2 = 19965,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_3 = 19966,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_4 = 19967,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_0 = 19968,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_1 = 19969,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_2 = 19970,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_3 = 19971,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_4 = 19972,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_0 = 19973,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_1 = 19974,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_2 = 19975,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_3 = 19976,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_4 = 19977,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_0 = 19978,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_1 = 19979,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_2 = 19980,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_3 = 19981,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_4 = 19982,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_0 = 19983,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_1 = 19984,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_2 = 19985,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_3 = 19986,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_4 = 19987,

    SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_3 = 20020,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_2 = 20021,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_1 = 20022,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_0 = 20023,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_0 = 20024,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_1 = 20025,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_2 = 20026,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_3 = 20027,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_3 = 20028,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_2 = 20029,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_1 = 20030,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_0 = 20031,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_0 = 20032,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_1 = 20033,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_2 = 20034,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_3 = 20035,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_0 = 20036,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_1 = 20037,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_2 = 20038,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_3 = 20039,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_0 = 20040,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_1 = 20041,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_2 = 20042,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_3 = 20043,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_0 = 20044,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_1 = 20045,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_2 = 20046,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_3 = 20047,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_0 = 20048,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_1 = 20049,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_2 = 20050,
    SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_3 = 20051,
};

static void DinghySlideTrackCoveredFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_DINGHY_SLIDE_FLAT_COVERED_SW_NE, SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_SW_NE },
        { SPR_DINGHY_SLIDE_FLAT_COVERED_NW_SE, SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_NW_SE },
        { SPR_DINGHY_SLIDE_FLAT_COVERED_SW_NE, SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_SW_NE },
        { SPR_DINGHY_SLIDE_FLAT_COVERED_NW_SE, SPR_DINGHY_SLIDE_FLAT_COVERED_FRONT_NW_SE },
    };

    auto imageId = session.TrackColours.WithIndex(imageIds[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    imageId = session.TrackColours.WithIndex(imageIds[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 26 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void DinghySlideTrackCovered25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_DINGHY_SLIDE_25_DEG_COVERED_SW_NE, SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_SW_NE },
        { SPR_DINGHY_SLIDE_25_DEG_COVERED_NW_SE, SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_NW_SE },
        { SPR_DINGHY_SLIDE_25_DEG_COVERED_NE_SW, SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_NE_SW },
        { SPR_DINGHY_SLIDE_25_DEG_COVERED_SE_NW, SPR_DINGHY_SLIDE_25_DEG_COVERED_FRONT_SE_NW },
    };

    auto imageId = session.TrackColours.WithIndex(imageIds[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    imageId = session.TrackColours.WithIndex(imageIds[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 50 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 8, height, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void DinghySlideTrackCovered60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_DINGHY_SLIDE_60_DEG_COVERED_SW_NE, SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_SW_NE },
        { SPR_DINGHY_SLIDE_60_DEG_COVERED_NW_SE, SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_NW_SE },
        { SPR_DINGHY_SLIDE_60_DEG_COVERED_NE_SW, SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_NE_SW },
        { SPR_DINGHY_SLIDE_60_DEG_COVERED_SE_NW, SPR_DINGHY_SLIDE_60_DEG_COVERED_FRONT_SE_NW },
    };

    auto imageId = session.TrackColours.WithIndex(imageIds[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    imageId = session.TrackColours.WithIndex(imageIds[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 98 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 32, height, session.SupportColours);
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
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

static void DinghySlideTrackCoveredFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_SW_NE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_SW_NE },
        { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_NW_SE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_NW_SE },
        { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_NE_SW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_NE_SW },
        { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_SE_NW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_COVERED_FRONT_SE_NW },
    };

    auto imageId = session.TrackColours.WithIndex(imageIds[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    imageId = session.TrackColours.WithIndex(imageIds[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 42 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 3, height, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void DinghySlideTrackCovered25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_SW_NE, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_SW_NE },
        { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_NW_SE, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_NW_SE },
        { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_NE_SW, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_NE_SW },
        { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_SE_NW, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_COVERED_FRONT_SE_NW },
    };

    auto imageId = session.TrackColours.WithIndex(imageIds[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    imageId = session.TrackColours.WithIndex(imageIds[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 66 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 12, height, session.SupportColours);
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
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void DinghySlideTrackCovered60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_SW_NE, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_SW_NE },
        { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_NW_SE, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_NW_SE },
        { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_NE_SW, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_NE_SW },
        { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_SE_NW, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_COVERED_FRONT_SE_NW },
    };

    auto imageId = session.TrackColours.WithIndex(imageIds[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    imageId = session.TrackColours.WithIndex(imageIds[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 66 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 20, height, session.SupportColours);
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
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void DinghySlideTrackCovered25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_SW_NE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_SW_NE },
        { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_NW_SE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_NW_SE },
        { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_NE_SW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_NE_SW },
        { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_SE_NW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_COVERED_FRONT_SE_NW },
    };

    auto imageId = session.TrackColours.WithIndex(imageIds[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    imageId = session.TrackColours.WithIndex(imageIds[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 34 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 6, height, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

static void DinghySlideTrackCovered25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrackCovered25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void DinghySlideTrackCovered60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrackCovered60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void DinghySlideTrackCoveredFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrackCovered25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void DinghySlideTrackCovered25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrackCovered60DegUpTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void DinghySlideTrackCovered25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrackCoveredFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void DinghySlideTrackCoveredRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBb imageIds[4][5] = {
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_1, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_2, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_3, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SW_SE_PART_4, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 34, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_2, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_3, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NW_SW_PART_4, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_2, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_3, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_NE_NW_PART_4, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_2, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_3, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_SE_NE_PART_4, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
        },
    };
    static constexpr SpriteBb frontImageIds[4][5] = {
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 30, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_1, { 0, 0, 0 }, { 0, 16, 27 }, { 32, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_2, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_3, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SW_SE_PART_4, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 34, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_2, { 0, 0, 0 }, { 0, 16, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_3, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NW_SW_PART_4, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_2, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_3, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_NE_NW_PART_4, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_2, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_3, { 0, 0, 0 }, { 0, 16, 27 }, { 32, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_COVERED_FRONT_SE_NE_PART_4, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
        },
    };

    TrackPaintUtilRightQuarterTurn5TilesPaint2(session, height, direction, trackSequence, session.TrackColours, imageIds);
    TrackPaintUtilRightQuarterTurn5TilesPaint2(session, height, direction, trackSequence, session.TrackColours, frontImageIds);

    switch (trackSequence)
    {
        case 0:
        case 6:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
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
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::rightCorner),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::bottomRightSide,
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRightSide, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::topLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRightSide, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::leftCorner),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void DinghySlideTrackCoveredLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    DinghySlideTrackCoveredRightQuarterTurn5(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

static void DinghySlideTrackCoveredSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][4][2] = {
        {
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_0 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_3 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_0 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_3 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_3 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_SE_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_NW_NE_SEQ_0 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_3 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_NW_SW_NW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NW_SW_NW_SEQ_0 },
        },
    };

    auto imageId = session.TrackColours.WithIndex(imageIds[direction][trackSequence][0]);
    auto frontImageId = session.TrackColours.WithIndex(imageIds[direction][trackSequence][1]);
    int16_t bboy;

    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            DrawSBendLeftSupports(session, supportType.metal, trackSequence, direction, height, 0, 0);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 1:
            bboy = (direction == 0 || direction == 1) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });
            DrawSBendLeftSupports(session, supportType.metal, trackSequence, direction, height, 0, 1);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            bboy = (direction == 2 || direction == 3) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });
            DrawSBendLeftSupports(session, supportType.metal, trackSequence, direction, height, 0, 1);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            DrawSBendLeftSupports(session, supportType.metal, trackSequence, direction, height, 0, 0);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
    }

    if (trackSequence == 0)
    {
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else if (trackSequence == 3)
    {
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void DinghySlideTrackCoveredSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][4][2] = {
        {
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_0 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_3 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_0 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_3 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_3 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SW_NW_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_NE_SE_NE_SEQ_0 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_3 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_COVERED_SE_SW_SE_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_COVERED_FRONT_SE_SW_SE_SEQ_0 },
        },
    };

    auto imageId = session.TrackColours.WithIndex(imageIds[direction][trackSequence][0]);
    auto frontImageId = session.TrackColours.WithIndex(imageIds[direction][trackSequence][1]);
    int16_t bboy;

    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height, 0, 0);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 1:
            bboy = (direction == 2 || direction == 3) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height, 0, 0);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            bboy = (direction == 0 || direction == 1) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height, 0, 0);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            DrawSBendRightSupports(session, supportType.metal, trackSequence, direction, height, 0, 0);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
    }

    if (trackSequence == 0)
    {
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else if (trackSequence == 3)
    {
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void DinghySlideTrackCoveredRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBb imageIds[4][3] = {
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_1, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SW_SE_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NW_SW_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_NE_NW_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_1, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_SE_NE_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
        },
    };
    static constexpr SpriteBb frontImageIds[4][3] = {
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_1, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SW_SE_PART_2, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NW_SW_PART_2, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_NE_NW_PART_2, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_1, { 0, 0, 0 }, { 0, 16, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_COVERED_FRONT_SE_NE_PART_2, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
        },
    };

    TrackPaintUtilRightQuarterTurn3TilesPaint3(session, height, direction, trackSequence, session.TrackColours, imageIds);
    TrackPaintUtilRightQuarterTurn3TilesPaint3(session, height, direction, trackSequence, session.TrackColours, frontImageIds);
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
        case 3:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::rightCorner);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::bottomRightSide, PaintSegment::bottomCorner);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRightSide, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::leftCorner);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void DinghySlideTrackCoveredLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    DinghySlideTrackCoveredRightQuarterTurn3(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

static void DinghySlideTrackCovered60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrackCovered25DegUpTo60DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

TrackPaintFunction GetTrackPaintFunctionDinghySlideCovered(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return DinghySlideTrackCoveredFlat;
        case TrackElemType::Up25:
            return DinghySlideTrackCovered25DegUp;
        case TrackElemType::Up60:
            return DinghySlideTrackCovered60DegUp;
        case TrackElemType::FlatToUp25:
            return DinghySlideTrackCoveredFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return DinghySlideTrackCovered25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return DinghySlideTrackCovered60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return DinghySlideTrackCovered25DegUpToFlat;
        case TrackElemType::Down25:
            return DinghySlideTrackCovered25DegDown;
        case TrackElemType::Down60:
            return DinghySlideTrackCovered60DegDown;
        case TrackElemType::FlatToDown25:
            return DinghySlideTrackCoveredFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return DinghySlideTrackCovered25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return DinghySlideTrackCovered60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return DinghySlideTrackCovered25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return DinghySlideTrackCoveredLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return DinghySlideTrackCoveredRightQuarterTurn5;
        case TrackElemType::SBendLeft:
            return DinghySlideTrackCoveredSBendLeft;
        case TrackElemType::SBendRight:
            return DinghySlideTrackCoveredSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return DinghySlideTrackCoveredLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return DinghySlideTrackCoveredRightQuarterTurn3;
        default:
            return TrackPaintFunctionDummy;
    }
}
