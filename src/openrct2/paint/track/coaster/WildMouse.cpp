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
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

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

static constexpr uint32_t _wild_mouse_brakes_image_ids[4] = {
    SPR_WILD_MOUSE_BRAKES_SW_NE,
    SPR_WILD_MOUSE_BRAKES_NW_SE,
    SPR_WILD_MOUSE_BRAKES_SW_NE,
    SPR_WILD_MOUSE_BRAKES_NW_SE,
};

static constexpr uint32_t _wild_mouse_block_brakes_image_ids[kNumOrthogonalDirections][2] = {
    { SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_CLOSED },
    { SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_CLOSED },
    { SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_SW_NE_CLOSED },
    { SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_OPEN, SPR_WILD_MOUSE_BLOCK_BRAKES_NW_SE_CLOSED },
};

/** rct2: 0x0078B1E4 */
static void WildMouseTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_FLAT_SW_NE, SPR_WILD_MOUSE_FLAT_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_FLAT_NW_SE, SPR_WILD_MOUSE_FLAT_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_FLAT_SW_NE, SPR_WILD_MOUSE_FLAT_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_FLAT_NW_SE, SPR_WILD_MOUSE_FLAT_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[direction][isChained]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -1, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void WildMouseTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t baseImageIds[4] = {
        SPR_STATION_BASE_B_SW_NE,
        SPR_STATION_BASE_B_NW_SE,
        SPR_STATION_BASE_B_SW_NE,
        SPR_STATION_BASE_B_NW_SE,
    };

    auto trackType = trackElement.GetTrackType();
    PaintAddImageAsParentRotated(
        session, direction, GetStationColourScheme(session, trackElement).WithIndex(baseImageIds[direction]),
        { 0, 0, height - 2 }, { { 0, 2, height }, { 32, 28, 2 } });
    if (trackType == TrackElemType::EndStation)
    {
        bool isClosed = trackElement.IsBrakeClosed();
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours.WithIndex(_wild_mouse_block_brakes_image_ids[direction][isClosed]),
            { 0, 0, height }, { { 0, 0, height }, { 32, 20, 2 } });
    }
    else
    {
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours.WithIndex(_wild_mouse_brakes_image_ids[direction]), { 0, 0, height },
            { { 0, 0, height }, { 32, 20, 2 } });
    }
    DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Boxed);
    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0078B1F4 */
static void WildMouseTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_25_DEG_SW_NE, SPR_WILD_MOUSE_25_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_25_DEG_NW_SE, SPR_WILD_MOUSE_25_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_NE_SW, SPR_WILD_MOUSE_25_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_25_DEG_SE_NW, SPR_WILD_MOUSE_25_DEG_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[direction][isChained]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -9, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0078B204 */
static void WildMouseTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_60_DEG_SW_NE, SPR_WILD_MOUSE_60_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_60_DEG_NW_SE, SPR_WILD_MOUSE_60_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_NE_SW, SPR_WILD_MOUSE_60_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_60_DEG_SE_NW, SPR_WILD_MOUSE_60_DEG_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[direction][isChained]);
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
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::Centre, -33, height, session.SupportColours);
        }
        else
        {
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 32, height, session.SupportColours);
        }
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
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x0078B214 */
static void WildMouseTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_SW_NE, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_NW_SE, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_NE_SW, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_FLAT_TO_25_DEG_SE_NW, SPR_WILD_MOUSE_FLAT_TO_25_DEG_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[direction][isChained]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -4, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x0078B224 */
static void WildMouseTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_SW_NE, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_NW_SE, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_NE_SW, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_SE_NW, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_SE_NW },
    };
    static constexpr uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_FRONT_NW_SE, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_TO_60_DEG_FRONT_NE_SW, SPR_WILD_MOUSE_25_DEG_TO_60_DEG_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[direction][isChained]);
    auto frontImageId = session.TrackColours.WithIndex(frontImageIds[direction][isChained]);
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
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -13, height, session.SupportColours);
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
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x0078B234 */
static void WildMouseTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_SW_NE, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_SE_NW, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_SE_NW },
    };
    static constexpr uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_FRONT_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_25_DEG_FRONT_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_25_DEG_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[direction][isChained]);
    auto frontImageId = session.TrackColours.WithIndex(frontImageIds[direction][isChained]);
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
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -21, height, session.SupportColours);
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
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x0078B244 */
static void WildMouseTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_SW_NE, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_NW_SE, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_NE_SW, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_25_DEG_TO_FLAT_SE_NW, SPR_WILD_MOUSE_25_DEG_TO_FLAT_CHAIN_SE_NW },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[direction][isChained]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -7, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x0078B254 */
static void WildMouseTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B264 */
static void WildMouseTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B274 */
static void WildMouseTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B284 */
static void WildMouseTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B294 */
static void WildMouseTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B2A4 */
static void WildMouseTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void WildMouseTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBb imageIds[4][3] = {
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

    TrackPaintUtilRightQuarterTurn3TilesPaint3(session, height, direction, trackSequence, session.TrackColours, imageIds);
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
        case 3:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -1, height, session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomRightSide);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void WildMouseTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBb imageIds[4][2] = {
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
    static constexpr int16_t generalSupportHeights[] = { 72, 56, 56, 72 };

    if (trackSequence == 0 || trackSequence == 3)
    {
        int32_t part = trackSequence == 0 ? 0 : 1;
        const SpriteBb* sbb = &imageIds[direction][part];
        const auto& offset = sbb->offset;
        PaintAddImageAsParent(
            session, session.TrackColours.WithIndex(sbb->sprite_id), { offset.x, offset.y, height + offset.z }, sbb->bb_size);
    }

    TrackPaintUtilRightQuarterTurn3Tiles25DegDownTunnel(
        session, kTunnelGroup, height, direction, trackSequence, TunnelSubType::SlopeEnd, TunnelSubType::SlopeStart);

    switch (trackSequence)
    {
        case 0:
        case 3:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -9, height, session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomRightSide);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void WildMouseTrackLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WildMouseTrackRightQuarterTurn325DegDown(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x0078B314 */
static void WildMouseTrackRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBb imageIds[4][2] = {
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
    static constexpr int16_t generalSupportHeights[] = { 72, 56, 56, 72 };

    if (trackSequence == 0 || trackSequence == 3)
    {
        int32_t part = trackSequence == 0 ? 0 : 1;
        const SpriteBb* sbb = &imageIds[direction][part];
        PaintAddImageAsParent(
            session, session.TrackColours.WithIndex(sbb->sprite_id), { sbb->offset.x, sbb->offset.y, height + sbb->offset.z },
            sbb->bb_size);
    }

    TrackPaintUtilRightQuarterTurn3Tiles25DegUpTunnel(
        session, kTunnelGroup, height, direction, trackSequence, TunnelSubType::SlopeStart, TunnelSubType::SlopeEnd);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -9, height, session.SupportColours);
            break;
        case 3:
            if (direction == 2)
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::Centre, -11, height, session.SupportColours);
            }
            else
            {
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::Centre, -9, height, session.SupportColours);
            }
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomRightSide);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

/** rct2: 0x0078B324 */
static void WildMouseTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WildMouseTrackRightQuarterTurn325DegUp(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

static void WildMouseTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    WildMouseTrackRightQuarterTurn3(session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x0078B394 */
static void WildMouseTrackLeftQuarterTurn1(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4] = {
        SPR_WILD_MOUSE_QUARTER_TURN_1_SW_NE,
        SPR_WILD_MOUSE_QUARTER_TURN_1_NW_SE,
        SPR_WILD_MOUSE_QUARTER_TURN_1_NE_SW,
        SPR_WILD_MOUSE_QUARTER_TURN_1_SE_NW,
    };

    auto imageId = session.TrackColours.WithIndex(imageIds[direction]);
    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 2, height }, { 26, 24, 2 } });
            break;
        case 1:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 26, 2 });
            break;
        case 2:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 6, height }, { 24, 26, 2 } });
            break;
        case 3:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 6, height }, { 24, 24, 2 } });
            break;
    }
    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -1, height, session.SupportColours);
    TrackPaintUtilLeftQuarterTurn1TileTunnel(
        session, kTunnelGroup, direction, height, 0, TunnelSubType::Flat, 0, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(
                PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide),
            direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0078B3A4 */
static void WildMouseTrackRightQuarterTurn1(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrackLeftQuarterTurn1(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B354 */
static void WildMouseTrackFlatTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_SW_NE, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_NW_SE, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_NE_SW, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_SE_NW, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_SE_NW },
    };
    static constexpr uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_FRONT_NW_SE, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_FLAT_TO_60_DEG_FRONT_NE_SW, SPR_WILD_MOUSE_FLAT_TO_60_DEG_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[direction][isChained]);
    auto frontImageId = session.TrackColours.WithIndex(frontImageIds[direction][isChained]);
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
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -5, height, session.SupportColours);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

/** rct2: 0x0078B364 */
static void WildMouseTrack60DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4][2] = {
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_SW_NE, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_SW_NE },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_NE_SW },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_SE_NW, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_SE_NW },
    };
    static constexpr uint32_t frontImageIds[4][2] = {
        { 0, 0 },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_FRONT_NW_SE, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_FRONT_NW_SE },
        { SPR_WILD_MOUSE_60_DEG_TO_FLAT_FRONT_NE_SW, SPR_WILD_MOUSE_60_DEG_TO_FLAT_CHAIN_FRONT_NE_SW },
        { 0, 0 },
    };

    uint8_t isChained = trackElement.HasChain() ? 1 : 0;
    auto imageId = session.TrackColours.WithIndex(imageIds[direction][isChained]);
    auto frontImageId = session.TrackColours.WithIndex(frontImageIds[direction][isChained]);
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
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::Centre, -17, height, session.SupportColours);
        }
        else
        {
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 16, height, session.SupportColours);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x0078B374 */
static void WildMouseTrackFlatTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrack60DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B384 */
static void WildMouseTrack60DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WildMouseTrackFlatTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0078B344 */
static void WildMouseTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(_wild_mouse_brakes_image_ids[direction]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0078B3C4 */
static void WildMouseTrackRotationControlToggle(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4] = {
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_SW_NE,
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_NW_SE,
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_SW_NE,
        SPR_WILD_MOUSE_ROTATION_CONTROL_TOGGLE_NW_SE,
    };

    auto imageId = session.TrackColours.WithIndex(imageIds[direction]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0078B3B4 */
static void WildMouseTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isClosed = trackElement.IsBrakeClosed();
    auto imageId = session.TrackColours.WithIndex(_wild_mouse_block_brakes_image_ids[direction][isClosed]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

TrackPaintFunction GetTrackPaintFunctionWildMouse(OpenRCT2::TrackElemType trackType)
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
        default:
            return TrackPaintFunctionDummy;
    }
}
