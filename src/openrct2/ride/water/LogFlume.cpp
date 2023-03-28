/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../sprites.h"
#include "../Track.h"
#include "../TrackPaint.h"

enum
{
    SPR_LOG_FLUME_FLAT_SW_NE = 20996,
    SPR_LOG_FLUME_FLAT_NW_SE = 20997,
    SPR_LOG_FLUME_FLAT_NE_SW = 20998,
    SPR_LOG_FLUME_FLAT_SE_NW = 20999,
    SPR_LOG_FLUME_FLAT_FRONT_SW_NE = 21000,
    SPR_LOG_FLUME_FLAT_FRONT_NW_SE = 21001,
    SPR_LOG_FLUME_FLAT_FRONT_NE_SW = 21002,
    SPR_LOG_FLUME_FLAT_FRONT_SE_NW = 21003,
    SPR_LOG_FLUME_REVERSER_SW_NE = 21004,
    SPR_LOG_FLUME_REVERSER_NW_SE = 21005,
    SPR_LOG_FLUME_REVERSER_NE_SW = 21006,
    SPR_LOG_FLUME_REVERSER_SE_NW = 21007,
    SPR_LOG_FLUME_REVERSER_FRONT_SW_NE = 21008,
    SPR_LOG_FLUME_REVERSER_FRONT_NW_SE = 21009,
    SPR_LOG_FLUME_REVERSER_FRONT_NE_SW = 21010,
    SPR_LOG_FLUME_REVERSER_FRONT_SE_NW = 21011,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_SW_NE = 21012,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_NW_SE = 21013,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_NE_SW = 21014,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_SE_NW = 21015,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 21016,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 21017,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 21018,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 21019,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_SW_NE = 21020,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_NW_SE = 21021,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_NE_SW = 21022,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_SE_NW = 21023,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SW_NE = 21024,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NW_SE = 21025,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NE_SW = 21026,
    SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SE_NW = 21027,
    SPR_LOG_FLUME_25_DEG_UP_SW_NE = 21028,
    SPR_LOG_FLUME_25_DEG_UP_NW_SE = 21029,
    SPR_LOG_FLUME_25_DEG_UP_NE_SW = 21030,
    SPR_LOG_FLUME_25_DEG_UP_SE_NW = 21031,
    SPR_LOG_FLUME_25_DEG_UP_FRONT_SW_NE = 21032,
    SPR_LOG_FLUME_25_DEG_UP_FRONT_NW_SE = 21033,
    SPR_LOG_FLUME_25_DEG_UP_FRONT_NE_SW = 21034,
    SPR_LOG_FLUME_25_DEG_UP_FRONT_SE_NW = 21035,
    SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_NE_SW = 21036,
    SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_SE_NW = 21037,
    SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_SW_NE = 21038,
    SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_NW_SE = 21039,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_NE_SW = 21040,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_SE_NW = 21041,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_SW_NE = 21042,
    SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_NW_SE = 21043,
    SPR_LOG_FLUME_25_DEG_DOWN_NE_SW = 21044,
    SPR_LOG_FLUME_25_DEG_DOWN_SE_NW = 21045,
    SPR_LOG_FLUME_25_DEG_DOWN_SW_NE = 21046,
    SPR_LOG_FLUME_25_DEG_DOWN_NW_SE = 21047,
    SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_0 = 21048,
    SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_2 = 21049,
    SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_3 = 21050,
    SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_0 = 21051,
    SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_2 = 21052,
    SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_3 = 21053,
    SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_0 = 21054,
    SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_2 = 21055,
    SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_3 = 21056,
    SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_0 = 21057,
    SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_2 = 21058,
    SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_3 = 21059,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_0 = 21060,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_2 = 21061,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_3 = 21062,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_0 = 21063,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_2 = 21064,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_3 = 21065,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_0 = 21066,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_2 = 21067,
    SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_3 = 21068,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_0 = 21069,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_2 = 21070,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_3 = 21071,
    SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_0 = 21072,
    SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_2 = 21073,
    SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_3 = 21074,
    SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_0 = 21075,
    SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_2 = 21076,
    SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_3 = 21077,
    SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_0 = 21078,
    SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_2 = 21079,
    SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_3 = 21080,
    SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_0 = 21081,
    SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_2 = 21082,
    SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_3 = 21083,
    SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_3 = 21084,
    SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_2 = 21085,
    SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_1 = 21086,
    SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_0 = 21087,
    SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_0 = 21088,
    SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_1 = 21089,
    SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_2 = 21090,
    SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_3 = 21091,
    SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_3 = 21092,
    SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_2 = 21093,
    SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_1 = 21094,
    SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_0 = 21095,
    SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_0 = 21096,
    SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_1 = 21097,
    SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_2 = 21098,
    SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_3 = 21099,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_0 = 21100,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_1 = 21101,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_2 = 21102,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_3 = 21103,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_0 = 21104,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_1 = 21105,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_2 = 21106,
    SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_3 = 21107,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_0 = 21108,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_1 = 21109,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_2 = 21110,
    SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_3 = 21111,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_0 = 21112,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_1 = 21113,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_2 = 21114,
    SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_3 = 21115,
    SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_0 = 21116,
    SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_1 = 21117,
    SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_2 = 21118,
    SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_3 = 21119,
    SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_3 = 21120,
    SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_2 = 21121,
    SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_1 = 21122,
    SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_0 = 21123,
    SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_0 = 21124,
    SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_1 = 21125,
    SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_2 = 21126,
    SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_3 = 21127,
    SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_3 = 21128,
    SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_2 = 21129,
    SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_1 = 21130,
    SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_0 = 21131,
};

static constexpr const uint32_t LogFlumeTrackFlatImageIds[4][2] = {
    { SPR_LOG_FLUME_FLAT_SW_NE, SPR_LOG_FLUME_FLAT_FRONT_SW_NE },
    { SPR_LOG_FLUME_FLAT_NW_SE, SPR_LOG_FLUME_FLAT_FRONT_NW_SE },
    { SPR_LOG_FLUME_FLAT_NE_SW, SPR_LOG_FLUME_FLAT_FRONT_NE_SW },
    { SPR_LOG_FLUME_FLAT_SE_NW, SPR_LOG_FLUME_FLAT_FRONT_SE_NW },
};

static void PaintLogFlumeTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(LogFlumeTrackFlatImageIds[direction][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(LogFlumeTrackFlatImageIds[direction][1]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 26 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void PaintLogFlumeTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(LogFlumeTrackFlatImageIds[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height + 3 }, { 32, 20, 1 } });

    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_NW_SE);
    }
    else
    {
        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_SW_NE);
    }
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 1 });

    if (direction & 1)
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    else
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    TrackPaintUtilDrawStation3(session, ride, direction, height + 2, height, trackElement);
    // Covers shouldn't be offset by +2

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void PaintLogFlumeTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_LOG_FLUME_25_DEG_UP_SW_NE, SPR_LOG_FLUME_25_DEG_UP_FRONT_SW_NE },
        { SPR_LOG_FLUME_25_DEG_UP_NW_SE, SPR_LOG_FLUME_25_DEG_UP_FRONT_NW_SE },
        { SPR_LOG_FLUME_25_DEG_UP_NE_SW, SPR_LOG_FLUME_25_DEG_UP_FRONT_NE_SW },
        { SPR_LOG_FLUME_25_DEG_UP_SE_NW, SPR_LOG_FLUME_25_DEG_UP_FRONT_SE_NW },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 50 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

static void PaintLogFlumeTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_SW_NE, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SW_NE },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_NW_SE, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NW_SE },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_NE_SW, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NE_SW },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_SE_NW, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SE_NW },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 42 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

static void PaintLogFlumeTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_SW_NE, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SW_NE },
        { SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_NW_SE, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NW_SE },
        { SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_NE_SW, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NE_SW },
        { SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_SE_NW, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SE_NW },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 34 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_12);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

static void PaintLogFlumeTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_LOG_FLUME_25_DEG_DOWN_SW_NE, SPR_LOG_FLUME_25_DEG_UP_FRONT_NE_SW },
        { SPR_LOG_FLUME_25_DEG_DOWN_NW_SE, SPR_LOG_FLUME_25_DEG_UP_FRONT_SE_NW },
        { SPR_LOG_FLUME_25_DEG_DOWN_NE_SW, SPR_LOG_FLUME_25_DEG_UP_FRONT_SW_NE },
        { SPR_LOG_FLUME_25_DEG_DOWN_SE_NW, SPR_LOG_FLUME_25_DEG_UP_FRONT_NW_SE },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 50 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_2);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

static void PaintLogFlumeTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_SW_NE, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NE_SW },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_NW_SE, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SE_NW },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_NE_SW, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_SW_NE },
        { SPR_LOG_FLUME_FLAT_TO_25_DEG_DOWN_SE_NW, SPR_LOG_FLUME_25_DEG_UP_TO_FLAT_FRONT_NW_SE },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 34 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_12);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

static void PaintLogFlumeTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_SW_NE, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NE_SW },
        { SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_NW_SE, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SE_NW },
        { SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_NE_SW, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_SW_NE },
        { SPR_LOG_FLUME_25_DEG_DOWN_TO_FLAT_SE_NW, SPR_LOG_FLUME_FLAT_TO_25_DEG_UP_FRONT_NW_SE },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 42 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

static void PaintLogFlumeTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][4][2] = {
        {
            { SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_0 },
            { SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_NE_NW_NE_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_3 },
        },
        {
            { SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_0 },
            { SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_NW_SW_NW_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_3 },
        },
        {
            { SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_3 },
            { SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_SW_SE_SW_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NE_NW_NE_SEQ_0 },
        },
        {
            { SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_3 },
            { SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_SE_NE_SE_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NW_SW_NW_SEQ_0 },
        },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][1]);
    int16_t bboy;

    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
        case 1:
            bboy = (direction == 0 || direction == 1) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });
            if (direction == 0 || direction == 1)
            {
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5 + (direction & 1), (direction & 1), height,
                    session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            break;
        case 2:
            bboy = (direction == 2 || direction == 3) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });
            if (direction == 2 || direction == 3)
            {
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 5 + (direction & 1), (direction & 1), height,
                    session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    if (trackSequence == 0)
    {
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        }
    }
    else if (trackSequence == 3)
    {
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void PaintLogFlumeTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][4][2] = {
        {
            { SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_0 },
            { SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_NE_SE_NE_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_3 },
        },
        {
            { SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_0 },
            { SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_SE_SW_SE_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_3 },
        },
        {
            { SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_3 },
            { SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_SW_NW_SW_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_NE_SE_NE_SEQ_0 },
        },
        {
            { SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_0, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_3 },
            { SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_1, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_2 },
            { SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_2, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_1 },
            { SPR_LOG_FLUME_3_TURN_NW_NE_NW_SEQ_3, SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SE_SEQ_0 },
        },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][trackSequence][1]);
    int16_t bboy;

    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
        case 1:
            bboy = (direction == 2 || direction == 3) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });
            if (direction == 0 || direction == 1)
            {
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8 - (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            break;
        case 2:
            bboy = (direction == 0 || direction == 1) ? 0 : 6;
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, bboy, height }, { 32, 26, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, bboy, height + 27 }, { 32, 26, 0 } });
            if (direction == 2 || direction == 3)
            {
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_BOXED, 8 - (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
            PaintAddImageAsParentRotated(
                session, direction, frontImageId, { 0, 0, height }, { { 0, 2, height + 27 }, { 32, 27, 0 } });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    if (trackSequence == 0)
    {
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        }
    }
    else if (trackSequence == 3)
    {
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void PaintLogFlumeTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_0, SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_2, SPR_LOG_FLUME_3_TURN_NW_SW_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_0, SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_2, SPR_LOG_FLUME_3_TURN_NE_NW_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_0, SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_2, SPR_LOG_FLUME_3_TURN_SE_NE_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_0, SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_2, SPR_LOG_FLUME_3_TURN_SW_SE_SEQ_3 },
    };

    static constexpr const uint32_t imageIdsFront[4][3] = {
        {
            SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_0,
            SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_2,
            SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_3,
        },
        {
            SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_0,
            SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_2,
            SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_3,
        },
        {
            SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_0,
            SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_2,
            SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_3,
        },
        {
            SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_0,
            SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_2,
            SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_3,
        },
    };

    TrackPaintUtilLeftQuarterTurn3TilesPaint(
        session, 2, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIds);
    TrackPaintUtilLeftQuarterTurn3TilesPaintWithHeightOffset(
        session, 0, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIdsFront, 27);

    if (trackSequence != 1 && trackSequence != 2)
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            break;
        case 3:
            if (direction == 2 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, TUNNEL_0);
            }
            break;
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void PaintLogFlumeTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_0, SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_2, SPR_LOG_FLUME_3_TURN_NW_NE_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_0, SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_2, SPR_LOG_FLUME_3_TURN_NE_SE_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_0, SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_2, SPR_LOG_FLUME_3_TURN_SE_SW_SEQ_3 },
        { SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_0, SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_2, SPR_LOG_FLUME_3_TURN_SW_NW_SEQ_3 }
    };

    static constexpr const uint32_t imageIdsFront[4][3] = {
        {
            SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_0,
            SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_2,
            SPR_LOG_FLUME_3_TURN_FRONT_NW_NE_SEQ_3,
        },
        {
            SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_0,
            SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_2,
            SPR_LOG_FLUME_3_TURN_FRONT_SW_SE_SEQ_3,
        },
        {
            SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_0,
            SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_2,
            SPR_LOG_FLUME_3_TURN_FRONT_SE_SW_SEQ_3,
        },
        {
            SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_0,
            SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_2,
            SPR_LOG_FLUME_3_TURN_FRONT_SW_NW_SEQ_3,
        },
    };

    TrackPaintUtilRightQuarterTurn3TilesPaint2(
        session, 2, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIds);
    TrackPaintUtilRightQuarterTurn3TilesPaint2WithHeightOffset(
        session, 0, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIdsFront, 27);

    if (trackSequence != 1 && trackSequence != 2)
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            break;
        case 3:
            if (direction == 0 || direction == 1)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, TUNNEL_0);
            }
            break;
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void PaintLogFlumeTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintAddImageAsParent(session, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });

    if (direction & 1)
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_FORK_ALT, 6, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_FORK_ALT, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    else
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_FORK, 5, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_FORK, 8, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(LogFlumeTrackFlatImageIds[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height + 3 }, { 32, 20, 0 } });

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(LogFlumeTrackFlatImageIds[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height + 5 }, { 32, 1, 21 } });

    TrackPaintUtilOnridePhotoPaint(session, direction, height + 3, trackElement);

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

static void PaintLogFlumeTrackReverser(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_LOG_FLUME_REVERSER_SW_NE, SPR_LOG_FLUME_REVERSER_FRONT_SW_NE },
        { SPR_LOG_FLUME_REVERSER_NW_SE, SPR_LOG_FLUME_REVERSER_FRONT_NW_SE },
        { SPR_LOG_FLUME_REVERSER_NE_SW, SPR_LOG_FLUME_REVERSER_FRONT_NE_SW },
        { SPR_LOG_FLUME_REVERSER_SE_NW, SPR_LOG_FLUME_REVERSER_FRONT_SE_NW },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 26 } });

    MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

// Steep Additions added by OpenRCT2

static void LogFlumeTrack25Down60(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_G2_FLUME_25_60_NW_SE_BACK, SPR_G2_FLUME_25_60_NW_SE_BACK_WATER },
        { SPR_G2_EMPTY, SPR_G2_FLUME_25_60_NW_SE },
        { SPR_G2_EMPTY, SPR_G2_FLUME_25_60_NE_SW },
        { SPR_G2_FLUME_25_60_NE_SW_BACK, SPR_G2_FLUME_25_60_NE_SW_BACK_WATER },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height + 4 }, { 32, 1, 42 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 0 } });

    if (direction == 1 || direction == 2)
    {
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_14);
    }
    else
    {
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 8, height + 12, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

static void LogFlumeTrack60Down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_G2_FLUME_60_NW_SE_BACK, SPR_G2_EMPTY },
        { SPR_G2_EMPTY, SPR_G2_FLUME_60_NW_SE },
        { SPR_G2_EMPTY, SPR_G2_FLUME_60_NE_SW },
        { SPR_G2_FLUME_60_NE_SW_BACK, SPR_G2_EMPTY },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height + 4 }, { 32, 1, 98 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 2 } });

    if (direction == 1 || direction == 2)
    {
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 8, height + 12, session.TrackColours[SCHEME_SUPPORTS]);
        }

        PaintUtilPushTunnelRotated(session, direction, height + 56, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

static void LogFlumeTrack60Down25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_G2_FLUME_60_25_NW_SE_BACK, SPR_G2_FLUME_60_25_NW_SE_BACK_WATER },
        { SPR_G2_EMPTY, SPR_G2_FLUME_60_25_NW_SE },
        { SPR_G2_EMPTY, SPR_G2_FLUME_60_25_NE_SW },
        { SPR_G2_FLUME_60_25_NE_SW_BACK, SPR_G2_FLUME_60_25_NE_SW_BACK_WATER },
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height + 4 }, { 32, 1, 42 } });
    PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 0 } });

    if (direction == 1 || direction == 2)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_1);

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_SQUARE_FLAT);

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 8, height + 8, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionLogFlume(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintLogFlumeTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintLogFlumeTrackStation;
        case TrackElemType::Up25:
            return PaintLogFlumeTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintLogFlumeTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintLogFlumeTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return PaintLogFlumeTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintLogFlumeTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintLogFlumeTrack25DegDownToFlat;
        case TrackElemType::SBendLeft:
            return PaintLogFlumeTrackSBendLeft;
        case TrackElemType::SBendRight:
            return PaintLogFlumeTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintLogFlumeTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintLogFlumeTrackRightQuarterTurn3Tiles;
        case TrackElemType::OnRidePhoto:
            return PaintLogFlumeTrackOnRidePhoto;
        case TrackElemType::LogFlumeReverser:
            return PaintLogFlumeTrackReverser;

            // Added by OpenRCT2
        case TrackElemType::Down25ToDown60:
            return LogFlumeTrack25Down60;
        case TrackElemType::Down60:
            return LogFlumeTrack60Down;
        case TrackElemType::Down60ToDown25:
            return LogFlumeTrack60Down25;
    }

    return nullptr;
}
