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
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

enum
{
    SPR_DINGHY_SLIDE_FLAT_SW_NE = 19720,
    SPR_DINGHY_SLIDE_FLAT_NW_SE = 19721,
    SPR_DINGHY_SLIDE_FLAT_FRONT_SW_NE = 19722,
    SPR_DINGHY_SLIDE_FLAT_FRONT_NW_SE = 19723,
    SPR_DINGHY_SLIDE_FLAT_CHAIN_SW_NE = 19724,
    SPR_DINGHY_SLIDE_FLAT_CHAIN_NW_SE = 19725,
    SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_SW_NE = 19726,
    SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_NW_SE = 19727,
    SPR_DINGHY_SLIDE_FLAT_CHAIN_NE_SW = 19728,
    SPR_DINGHY_SLIDE_FLAT_CHAIN_SE_NW = 19729,
    SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_NE_SW = 19730,
    SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_SE_NW = 19731,
    SPR_DINGHY_SLIDE_STATION_SW_NE = 19732,
    SPR_DINGHY_SLIDE_STATION_NW_SE = 19733,

    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_SW_NE = 19740,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_NW_SE = 19741,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_NE_SW = 19742,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_SE_NW = 19743,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_SW_NE = 19744,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_NW_SE = 19745,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_NE_SW = 19746,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_SE_NW = 19747,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_SW_NE = 19748,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_NW_SE = 19749,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_NE_SW = 19750,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_SE_NW = 19751,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_SW_NE = 19752,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_NW_SE = 19753,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_NE_SW = 19754,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_SE_NW = 19755,
    SPR_DINGHY_SLIDE_25_DEG_SW_NE = 19756,
    SPR_DINGHY_SLIDE_25_DEG_NW_SE = 19757,
    SPR_DINGHY_SLIDE_25_DEG_NE_SW = 19758,
    SPR_DINGHY_SLIDE_25_DEG_SE_NW = 19759,
    SPR_DINGHY_SLIDE_25_DEG_FRONT_SW_NE = 19760,
    SPR_DINGHY_SLIDE_25_DEG_FRONT_NW_SE = 19761,
    SPR_DINGHY_SLIDE_25_DEG_FRONT_NE_SW = 19762,
    SPR_DINGHY_SLIDE_25_DEG_FRONT_SE_NW = 19763,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_SW_NE = 19764,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_NW_SE = 19765,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_NE_SW = 19766,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_SE_NW = 19767,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_SW_NE = 19768,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_NW_SE = 19769,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_NE_SW = 19770,
    SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_SE_NW = 19771,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_SW_NE = 19772,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_NW_SE = 19773,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_NE_SW = 19774,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_SE_NW = 19775,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_SW_NE = 19776,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_NW_SE = 19777,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_NE_SW = 19778,
    SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_SE_NW = 19779,
    SPR_DINGHY_SLIDE_60_DEG_SW_NE = 19780,
    SPR_DINGHY_SLIDE_60_DEG_NW_SE = 19781,
    SPR_DINGHY_SLIDE_60_DEG_NE_SW = 19782,
    SPR_DINGHY_SLIDE_60_DEG_SE_NW = 19783,
    SPR_DINGHY_SLIDE_60_DEG_FRONT_SW_NE = 19784,
    SPR_DINGHY_SLIDE_60_DEG_FRONT_NW_SE = 19785,
    SPR_DINGHY_SLIDE_60_DEG_FRONT_NE_SW = 19786,
    SPR_DINGHY_SLIDE_60_DEG_FRONT_SE_NW = 19787,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_SW_NE = 19788,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_NW_SE = 19789,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_NE_SW = 19790,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_SE_NW = 19791,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_SW_NE = 19792,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_NW_SE = 19793,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_NE_SW = 19794,
    SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_SE_NW = 19795,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_SW_NE = 19796,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_NW_SE = 19797,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_NE_SW = 19798,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_SE_NW = 19799,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_SW_NE = 19800,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_NW_SE = 19801,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_NE_SW = 19802,
    SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_SE_NW = 19803,
    SPR_DINGHY_SLIDE_25_DEG_CHAIN_SW_NE = 19804,
    SPR_DINGHY_SLIDE_25_DEG_CHAIN_NW_SE = 19805,
    SPR_DINGHY_SLIDE_25_DEG_CHAIN_NE_SW = 19806,
    SPR_DINGHY_SLIDE_25_DEG_CHAIN_SE_NW = 19807,
    SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_SW_NE = 19808,
    SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_NW_SE = 19809,
    SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_NE_SW = 19810,
    SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_SE_NW = 19811,

    SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_0 = 19860,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_1 = 19861,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_2 = 19862,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_0 = 19863,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_1 = 19864,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_2 = 19865,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_0 = 19866,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_1 = 19867,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_2 = 19868,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_0 = 19869,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_1 = 19870,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_2 = 19871,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_0 = 19872,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_1 = 19873,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_2 = 19874,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_0 = 19875,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_1 = 19876,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_2 = 19877,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_0 = 19878,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_1 = 19879,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_2 = 19880,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_0 = 19881,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_1 = 19882,
    SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_2 = 19883,

    SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_0 = 19908,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_1 = 19909,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_2 = 19910,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_3 = 19911,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_4 = 19912,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_0 = 19913,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_1 = 19914,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_2 = 19915,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_3 = 19916,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_4 = 19917,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_0 = 19918,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_1 = 19919,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_2 = 19920,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_3 = 19921,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_4 = 19922,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_0 = 19923,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_1 = 19924,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_2 = 19925,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_3 = 19926,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_4 = 19927,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_0 = 19928,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_1 = 19929,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_2 = 19930,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_3 = 19931,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_4 = 19932,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_0 = 19933,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_1 = 19934,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_2 = 19935,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_3 = 19936,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_4 = 19937,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_0 = 19938,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_1 = 19939,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_2 = 19940,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_3 = 19941,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_4 = 19942,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_0 = 19943,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_1 = 19944,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_2 = 19945,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_3 = 19946,
    SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_4 = 19947,

    SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_3 = 19988,
    SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_2 = 19989,
    SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_1 = 19990,
    SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_0 = 19991,
    SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_0 = 19992,
    SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_1 = 19993,
    SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_2 = 19994,
    SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_3 = 19995,
    SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_3 = 19996,
    SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_2 = 19997,
    SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_1 = 19998,
    SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_0 = 19999,
    SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_0 = 20000,
    SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_1 = 20001,
    SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_2 = 20002,
    SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_3 = 20003,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_0 = 20004,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_1 = 20005,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_2 = 20006,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_3 = 20007,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_0 = 20008,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_1 = 20009,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_2 = 20010,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_3 = 20011,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_0 = 20012,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_1 = 20013,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_2 = 20014,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_3 = 20015,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_0 = 20016,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_1 = 20017,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_2 = 20018,
    SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_3 = 20019,
};

static void DinghySlideTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[2][4][2] = {
        {
            { SPR_DINGHY_SLIDE_FLAT_SW_NE, SPR_DINGHY_SLIDE_FLAT_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_FLAT_NW_SE, SPR_DINGHY_SLIDE_FLAT_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_FLAT_SW_NE, SPR_DINGHY_SLIDE_FLAT_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_FLAT_NW_SE, SPR_DINGHY_SLIDE_FLAT_FRONT_NW_SE },
        },
        {
            { SPR_DINGHY_SLIDE_FLAT_CHAIN_SW_NE, SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_FLAT_CHAIN_NW_SE, SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_FLAT_CHAIN_NE_SW, SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_FLAT_CHAIN_SE_NW, SPR_DINGHY_SLIDE_FLAT_CHAIN_FRONT_SE_NW },
        },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[isChained][direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    imageId = session.TrackColours.WithIndex(imageIds[isChained][direction][1]);
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

static void DinghySlideTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_DINGHY_SLIDE_STATION_SW_NE, SPR_STATION_BASE_B_SW_NE },
        { SPR_DINGHY_SLIDE_STATION_NW_SE, SPR_STATION_BASE_B_NW_SE },
        { SPR_DINGHY_SLIDE_STATION_SW_NE, SPR_STATION_BASE_B_SW_NE },
        { SPR_DINGHY_SLIDE_STATION_NW_SE, SPR_STATION_BASE_B_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(imageIds[direction][0]), { 0, 0, height },
        { { 0, 6, height + 3 }, { 32, 20, 1 } });
    PaintAddImageAsParentRotated(
        session, direction, GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][1]), { 0, 0, height },
        { 32, 32, 1 });

    DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);

    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);

    TrackPaintUtilDrawStationTunnel(session, direction, height);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void DinghySlideTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[2][4][2] = {
        {
            { SPR_DINGHY_SLIDE_25_DEG_SW_NE, SPR_DINGHY_SLIDE_25_DEG_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_25_DEG_NW_SE, SPR_DINGHY_SLIDE_25_DEG_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_25_DEG_NE_SW, SPR_DINGHY_SLIDE_25_DEG_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_25_DEG_SE_NW, SPR_DINGHY_SLIDE_25_DEG_FRONT_SE_NW },
        },
        {
            { SPR_DINGHY_SLIDE_25_DEG_CHAIN_SW_NE, SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_25_DEG_CHAIN_NW_SE, SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_25_DEG_CHAIN_NE_SW, SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_25_DEG_CHAIN_SE_NW, SPR_DINGHY_SLIDE_25_DEG_CHAIN_FRONT_SE_NW },
        },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[isChained][direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    imageId = session.TrackColours.WithIndex(imageIds[isChained][direction][1]);
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

static void DinghySlideTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_DINGHY_SLIDE_60_DEG_SW_NE, SPR_DINGHY_SLIDE_60_DEG_FRONT_SW_NE },
        { SPR_DINGHY_SLIDE_60_DEG_NW_SE, SPR_DINGHY_SLIDE_60_DEG_FRONT_NW_SE },
        { SPR_DINGHY_SLIDE_60_DEG_NE_SW, SPR_DINGHY_SLIDE_60_DEG_FRONT_NE_SW },
        { SPR_DINGHY_SLIDE_60_DEG_SE_NW, SPR_DINGHY_SLIDE_60_DEG_FRONT_SE_NW },
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

static void DinghySlideTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[2][4][2] = {
        {
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_SW_NE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_NW_SE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_NE_SW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_SE_NW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_FRONT_SE_NW },
        },
        {
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_SW_NE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_NW_SE, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_NE_SW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_SE_NW, SPR_DINGHY_SLIDE_FLAT_TO_25_DEG_CHAIN_FRONT_SE_NW },
        },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[isChained][direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    imageId = session.TrackColours.WithIndex(imageIds[isChained][direction][1]);
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

static void DinghySlideTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_SW_NE, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_SW_NE },
        { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_NW_SE, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_NW_SE },
        { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_NE_SW, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_NE_SW },
        { SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_SE_NW, SPR_DINGHY_SLIDE_25_DEG_TO_60_DEG_FRONT_SE_NW },
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

static void DinghySlideTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_SW_NE, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_SW_NE },
        { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_NW_SE, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_NW_SE },
        { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_NE_SW, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_NE_SW },
        { SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_SE_NW, SPR_DINGHY_SLIDE_60_DEG_TO_25_DEG_FRONT_SE_NW },
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

static void DinghySlideTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[2][4][2] = {
        {
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_SW_NE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_NW_SE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_NE_SW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_SE_NW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_FRONT_SE_NW },
        },
        {
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_SW_NE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_SW_NE },
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_NW_SE, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_NW_SE },
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_NE_SW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_NE_SW },
            { SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_SE_NW, SPR_DINGHY_SLIDE_25_DEG_TO_FLAT_CHAIN_FRONT_SE_NW },
        },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[isChained][direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    imageId = session.TrackColours.WithIndex(imageIds[isChained][direction][1]);
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

static void DinghySlideTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void DinghySlideTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void DinghySlideTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void DinghySlideTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void DinghySlideTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void DinghySlideTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBb imageIds[4][5] = {
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_0, { 0, 2, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_1, { 0, 16, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_2, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_3, { 16, 0, 0 }, { 16, 0, 0 }, { 16, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SW_SE_PART_4, { 2, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_0, { 2, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_1, { 16, 0, 0 }, { 16, 0, 0 }, { 16, 34, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_2, { 0, 16, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_3, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NW_SW_PART_4, { 0, 2, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_0, { 0, 2, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_2, { 16, 16, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_3, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_NE_NW_PART_4, { 2, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_0, { 2, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_2, { 16, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_3, { 0, 16, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_SE_NE_PART_4, { 0, 2, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
        },
    };
    static constexpr SpriteBb frontImageIds[4][5] = {
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_0, { 0, 2, 0 }, { 0, 6, 27 }, { 32, 30, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_1, { 0, 16, 0 }, { 0, 16, 27 }, { 32, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_2, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_3, { 16, 0, 0 }, { 16, 0, 27 }, { 16, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SW_SE_PART_4, { 2, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_0, { 2, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_1, { 16, 0, 0 }, { 16, 0, 27 }, { 16, 34, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_2, { 0, 16, 0 }, { 0, 16, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_3, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NW_SW_PART_4, { 0, 2, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_0, { 0, 2, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_2, { 16, 16, 0 }, { 16, 16, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_3, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_NE_NW_PART_4, { 2, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_0, { 2, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_2, { 16, 0, 0 }, { 16, 0, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_3, { 0, 16, 0 }, { 0, 16, 27 }, { 32, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_5_FRONT_SE_NE_PART_4, { 0, 2, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
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

static void DinghySlideTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    DinghySlideTrackRightQuarterTurn5(session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

static void DinghySlideTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    DinghySlideTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void DinghySlideTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][4][2] = {
        {
            { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_0 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_3 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_0 },
            { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_3 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_3 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_SE_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_NW_NE_SEQ_0 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_3 },
            { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_NW_SW_NW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NW_SW_NW_SEQ_0 },
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

static void DinghySlideTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][4][2] = {
        {
            { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_0 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_3 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_0 },
            { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_3 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_3 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_SW_NW_SW_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_NE_SE_NE_SEQ_0 },
        },
        {
            { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_3, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_3 },
            { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_2, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_2 },
            { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_1, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_1 },
            { SPR_DINGHY_SLIDE_S_BEND_SE_SW_SE_SEQ_0, SPR_DINGHY_SLIDE_S_BEND_FRONT_SE_SW_SE_SEQ_0 },
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

static void DinghySlideTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBb imageIds[4][3] = {
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_1, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SW_SE_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NW_SW_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_NE_NW_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_1, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_SE_NE_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
        },
    };
    static constexpr SpriteBb frontImageIds[4][3] = {
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_1, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SW_SE_PART_2, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NW_SW_PART_2, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_NE_NW_PART_2, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
        },
        {
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_1, { 0, 0, 0 }, { 0, 16, 27 }, { 16, 16, 0 } },
            { SPR_DINGHY_SLIDE_QUARTER_TURN_3_FRONT_SE_NE_PART_2, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
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

static void DinghySlideTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    DinghySlideTrackRightQuarterTurn3(session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

TrackPaintFunction GetTrackPaintFunctionDinghySlide(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return DinghySlideTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return DinghySlideTrackStation;
        case TrackElemType::Up25:
            return DinghySlideTrack25DegUp;
        case TrackElemType::Up60:
            return DinghySlideTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return DinghySlideTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return DinghySlideTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return DinghySlideTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return DinghySlideTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return DinghySlideTrack25DegDown;
        case TrackElemType::Down60:
            return DinghySlideTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return DinghySlideTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return DinghySlideTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return DinghySlideTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return DinghySlideTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return DinghySlideTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return DinghySlideTrackRightQuarterTurn5;
        case TrackElemType::SBendLeft:
            return DinghySlideTrackSBendLeft;
        case TrackElemType::SBendRight:
            return DinghySlideTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return DinghySlideTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return DinghySlideTrackRightQuarterTurn3;
        default:
            return TrackPaintFunctionDummy;
    }
}
