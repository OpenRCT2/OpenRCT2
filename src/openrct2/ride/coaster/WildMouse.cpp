/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../paint/tile_element/Paint.TileElement.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"

enum
{
    SPR_WILD_MOUSE_FLAT_SW_NE = 16900,
    SPR_WILD_MOUSE_FLAT_NW_SE = 16901,
    SPR_WILD_MOUSE_BRAKES_SW_NE = 16902,
    SPR_WILD_MOUSE_BRAKES_NW_SE = 16903,
    SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_OPEN = 16904,
    SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_OPEN = 16905,
    SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_CLOSED = 16906,
    SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_CLOSED = 16907,

    SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_SW_NE = 16908,
    SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_NW_SE = 16909,
    SPR_WILD_MOUSE_FLAT_CHAIN_SW_NE = 16910,
    SPR_WILD_MOUSE_FLAT_CHAIN_NW_SE = 16911,
    SPR_WILD_MOUSE_FLAT_CHAIN_NE_SW = 16912,
    SPR_WILD_MOUSE_FLAT_CHAIN_SE_NW = 16913,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_SW_NE = 16914,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_NW_SE = 16915,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_NE_SW = 16916,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_SE_NW = 16917,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_SW_NE = 16918,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_NW_SE = 16919,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_NE_SW = 16920,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_SE_NW = 16921,
    SPR_WILD_MOUSE_25_DEG_SW_NE = 16922,
    SPR_WILD_MOUSE_25_DEG_NW_SE = 16923,
    SPR_WILD_MOUSE_25_DEG_NE_SW = 16924,
    SPR_WILD_MOUSE_25_DEG_SE_NW = 16925,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_SW_NE = 16926,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_NW_SE = 16927,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_NE_SW = 16928,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_SE_NW = 16929,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_FRONT_NW_SE = 16930,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_FRONT_NE_SW = 16931,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_SW_NE = 16932,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_NW_SE = 16933,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_NE_SW = 16934,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_SE_NW = 16935,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_FRONT_NW_SE = 16936,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_FRONT_NE_SW = 16937,
    SPR_WILD_MOUSE_60_DEG_SW_NE = 16938,
    SPR_WILD_MOUSE_60_DEG_NW_SE = 16939,
    SPR_WILD_MOUSE_60_DEG_NE_SW = 16940,
    SPR_WILD_MOUSE_60_DEG_SE_NW = 16941,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_SW_NE = 16942,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_NW_SE = 16943,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_NE_SW = 16944,
    SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_SE_NW = 16945,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_SW_NE = 16946,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_NW_SE = 16947,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_NE_SW = 16948,
    SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_SE_NW = 16949,
    SPR_WILD_MOUSE_25_DEG_CHAIN_SW_NE = 16950,
    SPR_WILD_MOUSE_25_DEG_CHAIN_NW_SE = 16951,
    SPR_WILD_MOUSE_25_DEG_CHAIN_NE_SW = 16952,
    SPR_WILD_MOUSE_25_DEG_CHAIN_SE_NW = 16953,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_SW_NE = 16954,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_NW_SE = 16955,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_NE_SW = 16956,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_SE_NW = 16957,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_FRONT_NW_SE = 16958,
    SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_FRONT_NE_SW = 16959,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_SW_NE = 16960,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_NW_SE = 16961,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_NE_SW = 16962,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_SE_NW = 16963,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_FRONT_NW_SE = 16964,
    SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_FRONT_NE_SW = 16965,
    SPR_WILD_MOUSE_60_DEG_CHAIN_SW_NE = 16966,
    SPR_WILD_MOUSE_60_DEG_CHAIN_NW_SE = 16967,
    SPR_WILD_MOUSE_60_DEG_CHAIN_NE_SW = 16968,
    SPR_WILD_MOUSE_60_DEG_CHAIN_SE_NW = 16969,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_SW_NE = 16970,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_NW_SE = 16971,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_NE_SW = 16972,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_SE_NW = 16973,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_FRONT_NW_SE = 16974,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_FRONT_NE_SW = 16975,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_SW_NE = 16976,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_NW_SE = 16977,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_NE_SW = 16978,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_SE_NW = 16979,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_FRONT_NW_SE = 16980,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_FRONT_NE_SW = 16981,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_SW_NE = 16982,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_NW_SE = 16983,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_NE_SW = 16984,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_SE_NW = 16985,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_FRONT_NW_SE = 16986,
    SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_FRONT_NE_SW = 16987,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_SW_NE = 16988,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_NW_SE = 16989,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_NE_SW = 16990,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_SE_NW = 16991,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_FRONT_NW_SE = 16992,
    SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_FRONT_NE_SW = 16993,
    SPR_WILD_MOUSE_QUARTER_TURN_1_SW_NE = 16994,
    SPR_WILD_MOUSE_QUARTER_TURN_1_NW_SE = 16995,
    SPR_WILD_MOUSE_QUARTER_TURN_1_NE_SW = 16996,
    SPR_WILD_MOUSE_QUARTER_TURN_1_SE_NW = 16997,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_0 = 16998,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_1 = 16999,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_2 = 17000,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_0 = 17001,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_1 = 17002,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_2 = 17003,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_0 = 17004,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_1 = 17005,
    SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_2 = 17006,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_0 = 17007,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_1 = 17008,
    SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_2 = 17009,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SW_SE_PART_0 = 17010,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SW_SE_PART_1 = 17011,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NW_SW_PART_0 = 17012,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NW_SW_PART_1 = 17013,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NE_NW_PART_0 = 17014,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NE_NW_PART_1 = 17015,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SE_NE_PART_0 = 17016,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SE_NE_PART_1 = 17017,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SW_SE_PART_0 = 17018,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SW_SE_PART_1 = 17019,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NW_SW_PART_0 = 17020,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NW_SW_PART_1 = 17021,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NE_NW_PART_0 = 17022,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NE_NW_PART_1 = 17023,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SE_NE_PART_0 = 17024,
    SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SE_NE_PART_1 = 17025,
};

static constexpr const uint32_t _wild_mouse_brakes_image_ids[4] = {
    SPR_WILD_MOUSE_BRAKES_SW_NE,
    SPR_WILD_MOUSE_BRAKES_NW_SE,
    SPR_WILD_MOUSE_BRAKES_SW_NE,
    SPR_WILD_MOUSE_BRAKES_NW_SE,
};

static constexpr const uint32_t _wild_mouse_block_brakes_image_ids[NumOrthogonalDirections][2] = {
    { SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_CLOSED },
    { SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_CLOSED },
    { SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_CLOSED },
    { SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_CLOSED },
};

/** rct2: 0x0078B1E4 */
static void WildMouseTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_FLAT_SW_NE, SPR_WILD_MOUSE_FLAT_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_FLAT_NW_SE, SPR_WILD_MOUSE_FLAT_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_FLAT_SW_NE, SPR_WILD_MOUSE_FLAT_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_FLAT_NW_SE, SPR_WILD_MOUSE_FLAT_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][isChained]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -1, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void WildMouseTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t baseImageIds[4] = {
        SPR_STATION_BASE_B_SW_NE,
        SPR_STATION_BASE_B_NW_SE,
        SPR_STATION_BASE_B_SW_NE,
        SPR_STATION_BASE_B_NW_SE,
    };

    int32_t trackType = trackElement.GetTrackType();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_MISC].WithIndex(baseImageIds[direction]), { 0, 0, height - 2 },
        { { 0, 2, height }, { 32, 28, 2 } });
    if (trackType == TrackElemType::EndStation)
    {
        bool isClosed = trackElement.IsBrakeClosed();
        PaintAddImageAsChildRotated(
            session, direction,
            session.TrackColours[SCHEME_TRACK].WithIndex(_wild_mouse_block_brakes_image_ids[direction][isClosed]),
            { 0, 0, height }, { 32, 20, 2 }, { 0, 0, height });
    }
    else
    {
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_wild_mouse_brakes_image_ids[direction]),
            { 0, 0, height }, { 32, 20, 2 }, { 0, 0, height });
    }
    TrackPaintUtilDrawStationMetalSupports(session, direction, height, session.TrackColours[SCHEME_SUPPORTS]);
    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0078B1F4 */
static void WildMouseTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_25_DEG_SW_NE, SPR_WILD_MOUSE_25_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_25_DEG_NW_SE, SPR_WILD_MOUSE_25_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_NE_SW, SPR_WILD_MOUSE_25_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_25_DEG_SE_NW, SPR_WILD_MOUSE_25_DEG_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][isChained]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -9, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x0078B204 */
static void WildMouseTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_60_DEG_SW_NE, SPR_WILD_MOUSE_60_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_60_DEG_NW_SE, SPR_WILD_MOUSE_60_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_NE_SW, SPR_WILD_MOUSE_60_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_60_DEG_SE_NW, SPR_WILD_MOUSE_60_DEG_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][isChained]);
    if (direction == 0 || direction == 3)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    }
    else
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 98 } });
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        if (direction == 0 || direction == 3)
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -33, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        else
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x0078B214 */
static void WildMouseTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_SW_NE, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_NW_SE, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_NE_SW, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_SE_NW, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][isChained]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -4, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x0078B224 */
static void WildMouseTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_SW_NE, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_NW_SE, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_NE_SW, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_SE_NW, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_SE_NW },
    };
    static constexpr const uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_FRONT_NW_SE, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_FRONT_NE_SW, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][isChained]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(frontImageIds[direction][isChained]);
    if (direction == 0 || direction == 3)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    }
    else
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
        PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 66 } });
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -13, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x0078B234 */
static void WildMouseTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_SW_NE, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_SE_NW, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_SE_NW },
    };
    static constexpr const uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_FRONT_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_FRONT_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][isChained]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(frontImageIds[direction][isChained]);
    if (direction == 0 || direction == 3)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    }
    else
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
        PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 66 } });
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -21, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x0078B244 */
static void WildMouseTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_SW_NE, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_NW_SE, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_NE_SW, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_SE_NW, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][isChained]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -7, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x0078B254 */
static void WildMouseTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WildMouseTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B264 */
static void WildMouseTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WildMouseTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B274 */
static void WildMouseTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WildMouseTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B284 */
static void WildMouseTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WildMouseTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B294 */
static void WildMouseTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WildMouseTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B2A4 */
static void WildMouseTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WildMouseTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void WildMouseTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const SpriteBb imageIds[4][3] = {
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_1, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SW_SE_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_1, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NW_SW_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_1, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_NE_NW_PART_2, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_1, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_SE_NE_PART_2, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 3 } },
        },
    };

    TrackPaintUtilRightQuarterTurn3TilesPaint3(
        session, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], imageIds);
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, height, direction, trackSequence, TUNNEL_0);

    switch (trackSequence)
    {
        case 0:
        case 3:
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -1, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0;
            break;
        case 2:
            blockedSegments = SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4;
            break;
        case 3:
            blockedSegments = SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void WildMouseTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const SpriteBb imageIds[4][2] = {
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SW_SE_PART_0, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SW_SE_PART_1, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NW_SW_PART_0, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NW_SW_PART_1, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NE_NW_PART_0, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_NE_NW_PART_1, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SE_NE_PART_0, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_DOWN_SE_NE_PART_1, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
        },
    };
    static constexpr const int16_t generalSupportHeights[] = { 72, 56, 56, 72 };

    if (trackSequence == 0 || trackSequence == 3)
    {
        int32_t part = trackSequence == 0 ? 0 : 1;
        const SpriteBb* sbb = &imageIds[direction][part];
        const auto& offset = sbb->offset;
        PaintAddImageAsParent(
            session, session.TrackColours[SCHEME_TRACK].WithIndex(sbb->sprite_id), { offset.x, offset.y, height + offset.z },
            sbb->bb_size);
    }

    TrackPaintUtilRightQuarterTurn3Tiles25DegDownTunnel(session, height, direction, trackSequence, TUNNEL_2, TUNNEL_1);

    switch (trackSequence)
    {
        case 0:
        case 3:
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -9, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0;
            break;
        case 3:
            blockedSegments = SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence], 0x20);
}

static void WildMouseTrackLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WildMouseTrackRightQuarterTurn325DegDown(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x0078B314 */
static void WildMouseTrackRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const SpriteBb imageIds[4][2] = {
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SW_SE_PART_0, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SW_SE_PART_1, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NW_SW_PART_0, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NW_SW_PART_1, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NE_NW_PART_0, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_NE_NW_PART_1, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
        },
        {
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SE_NE_PART_0, { 6, 0, 0 }, { 0, 0, 0 }, { 20, 32, 3 } },
            { SPR_WILD_MOUSE_QUARTER_TURN_3_25_DEG_UP_SE_NE_PART_1, { 0, 6, 0 }, { 0, 0, 0 }, { 32, 20, 3 } },
        },
    };
    static constexpr const int16_t generalSupportHeights[] = { 72, 56, 56, 72 };

    if (trackSequence == 0 || trackSequence == 3)
    {
        int32_t part = trackSequence == 0 ? 0 : 1;
        const SpriteBb* sbb = &imageIds[direction][part];
        PaintAddImageAsParent(
            session, session.TrackColours[SCHEME_TRACK].WithIndex(sbb->sprite_id),
            { sbb->offset.x, sbb->offset.y, height + sbb->offset.z }, sbb->bb_size);
    }

    TrackPaintUtilRightQuarterTurn3Tiles25DegUpTunnel(session, height, direction, trackSequence, TUNNEL_1, TUNNEL_2);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -9, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            if (direction == 2)
            {
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -11, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            else
            {
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0;
            break;
        case 3:
            blockedSegments = SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence], 0x20);
}

/** rct2: 0x0078B324 */
static void WildMouseTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WildMouseTrackRightQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

static void WildMouseTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WildMouseTrackRightQuarterTurn3(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x0078B394 */
static void WildMouseTrackLeftQuarterTurn1(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4] = {
        SPR_WILD_MOUSE_QUARTER_TURN_1_SW_NE,
        SPR_WILD_MOUSE_QUARTER_TURN_1_NW_SE,
        SPR_WILD_MOUSE_QUARTER_TURN_1_NE_SW,
        SPR_WILD_MOUSE_QUARTER_TURN_1_SE_NW,
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction]);
    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 24, 2 }, { 6, 2, height });
            break;
        case 1:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 26, 2 });
            break;
        case 2:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 26, 2 }, { 2, 6, height });
            break;
        case 3:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 24, 24, 2 }, { 6, 6, height });
            break;
    }
    MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -1, height, session.TrackColours[SCHEME_SUPPORTS]);
    TrackPaintUtilLeftQuarterTurn1TileTunnel(session, direction, height, 0, TUNNEL_0, 0, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0078B3A4 */
static void WildMouseTrackRightQuarterTurn1(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WildMouseTrackLeftQuarterTurn1(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x0078B354 */
static void WildMouseTrackFlatTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_SW_NE, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_NW_SE, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_NE_SW, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_SE_NW, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_SE_NW },
    };
    static constexpr const uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_FRONT_NW_SE, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_FRONT_NE_SW, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][isChained]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(frontImageIds[direction][isChained]);
    if (direction == 0 || direction == 3)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
    }
    else
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 29, 4, height + 2 }, { 1, 24, 43 } });
        PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 4, height }, { 32, 2, 43 } });
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -5, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x0078B364 */
static void WildMouseTrack60DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_SW_NE, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_SE_NW, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_SE_NW },
    };
    static constexpr const uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_FRONT_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_FRONT_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][isChained]);
    auto frontImageId = session.TrackColours[SCHEME_TRACK].WithIndex(frontImageIds[direction][isChained]);
    if (direction == 0 || direction == 3)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 27, 2 } });
    }
    else
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 29, 4, height + 2 }, { 1, 24, 43 } });
        PaintAddImageAsParentRotated(session, direction, frontImageId, { 0, 0, height }, { { 0, 4, height }, { 32, 2, 43 } });
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        if (direction == 0 || direction == 3)
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, -17, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        else
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_0);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x0078B374 */
static void WildMouseTrackFlatTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WildMouseTrack60DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B384 */
static void WildMouseTrack60DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    WildMouseTrackFlatTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0078B344 */
static void WildMouseTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(_wild_mouse_brakes_image_ids[direction]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0078B3C4 */
static void WildMouseTrackRotationControlToggle(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4] = {
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_SW_NE,
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_NW_SE,
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_SW_NE,
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_NW_SE,
    };

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0078B3B4 */
static void WildMouseTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isClosed = trackElement.IsBrakeClosed();
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(_wild_mouse_block_brakes_image_ids[direction][isClosed]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionWildMouse(int32_t trackType)
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
    }
    return nullptr;
}
