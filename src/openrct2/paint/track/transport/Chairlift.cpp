/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../ride/Ride.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TileElement.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

#include <iterator>

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

enum
{
    SPR_CHAIRLIFT_CABLE_FLAT_SW_NE = 20500,
    SPR_CHAIRLIFT_CABLE_FLAT_SE_NW,
    SPR_20502,
    SPR_20503,
    SPR_20504,
    SPR_20505,
    SPR_CHAIRLIFT_STATION_COLUMN_NE_SW,
    SPR_CHAIRLIFT_STATION_COLUMN_SE_NW,
    SPR_20508,
    SPR_20509,
    SPR_20510,
    SPR_20511,
    SPR_20512,
    SPR_20513,
    SPR_20514,
    SPR_20515,
    SPR_CHAIRLIFT_CABLE_UP_SW_NE,
    SPR_CHAIRLIFT_CABLE_UP_NW_SE,
    SPR_CHAIRLIFT_CABLE_UP_NE_SW,
    SPR_CHAIRLIFT_CABLE_UP_SE_NW,
    SPR_20520,
    SPR_20521,
    SPR_20522,
    SPR_20523,
    SPR_20524,
    SPR_20525,
    SPR_20526,
    SPR_20527,
    SPR_CHAIRLIFT_CORNER_NW_SW,
    SPR_CHAIRLIFT_CORNER_NW_NE,
    SPR_CHAIRLIFT_CORNER_SE_NE,
    SPR_CHAIRLIFT_CORNER_SW_SE,
    SPR_20532,
    SPR_20533,
    SPR_20534,
    SPR_20535,
    SPR_20536,
    SPR_20537,
    SPR_20538,
    SPR_20539,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_1,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_2,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_3,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_4,
    SPR_CHAIRLIFT_STATION_END_CAP_NE,
    SPR_CHAIRLIFT_STATION_END_CAP_SE,
    SPR_CHAIRLIFT_STATION_END_CAP_SW,
    SPR_CHAIRLIFT_STATION_END_CAP_NW,
};

const uint32_t chairlift_bullwheel_frames[] = {
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_1,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_2,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_3,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_4,
};

static void ChairliftPaintUtilDrawSupports(PaintSession& session, int32_t segments, uint16_t height, SupportType supportType)
{
    bool success = false;

    for (uint8_t s = 0; s < std::size(kSegmentOffsets); s++)
    {
        if (!(segments & kSegmentOffsets[s]))
        {
            continue;
        }

        if (MetalASupportsPaintSetup(
                session, supportType.metal, static_cast<MetalSupportPlace>(s), 0, height, session.supportColours))
        {
            success = true;
        }
    }

    if (success)
    {
        return;
    }

    SupportHeight* supportSegments = session.supportSegments;
    for (uint8_t s = 0; s < std::size(kSegmentOffsets); s++)
    {
        if (!(segments & kSegmentOffsets[s]))
        {
            continue;
        }
        uint16_t temp = supportSegments[s].height;
        supportSegments[s].height = session.support.height;
        MetalASupportsPaintSetup(
            session, supportType.metal, static_cast<MetalSupportPlace>(s), 0, height, session.supportColours);
        supportSegments[s].height = temp;
    }
}

static const TrackElement* ChairliftPaintUtilMapGetTrackElementAtFromRideFuzzy(
    int32_t x, int32_t y, int32_t z, const Ride& ride)
{
    const TileElement* tileElement = MapGetFirstElementAt(CoordsXY{ x, y });
    if (tileElement == nullptr)
    {
        return nullptr;
    }

    do
    {
        if (tileElement->getType() != TileElementType::Track)
            continue;
        if (tileElement->GetRideIndex() != ride.id)
            continue;
        if (tileElement->baseHeight != z && tileElement->baseHeight != z - 1)
            continue;

        return tileElement->asTrack();
    } while (!(tileElement++)->isLastForTile());

    return nullptr;
}

static bool ChairliftPaintUtilIsFirstTrack(
    const Ride& ride, const TrackElement& trackElement, const CoordsXY& pos, TrackElemType trackType)
{
    if (trackElement.GetTrackType() != TrackElemType::beginStation)
    {
        return false;
    }

    CoordsXY delta = CoordsDirectionDelta[trackElement.getDirection()];
    CoordsXY newPos = {
        static_cast<int32_t>(pos.x - delta.x),
        static_cast<int32_t>(pos.y - delta.y),
    };

    const TrackElement* nextTrack = ChairliftPaintUtilMapGetTrackElementAtFromRideFuzzy(
        newPos.x, newPos.y, trackElement.baseHeight, ride);

    return nextTrack == nullptr;
}

static bool ChairliftPaintUtilIsLastTrack(
    const Ride& ride, const TrackElement& trackElement, const CoordsXY& pos, TrackElemType trackType)
{
    if (trackElement.GetTrackType() != TrackElemType::endStation)
    {
        return false;
    }

    CoordsXY delta = CoordsDirectionDelta[trackElement.getDirection()];
    CoordsXY newPos = {
        static_cast<int32_t>(pos.x + delta.x),
        static_cast<int32_t>(pos.y + delta.y),
    };

    const TrackElement* nextTrack = ChairliftPaintUtilMapGetTrackElementAtFromRideFuzzy(
        newPos.x, newPos.y, trackElement.baseHeight, ride);

    return nextTrack == nullptr;
}

static void ChairliftPaintStationNeSw(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const CoordsXY pos = session.mapPosition;
    auto trackType = trackElement.GetTrackType();
    ImageId imageId;

    bool isStart = ChairliftPaintUtilIsFirstTrack(ride, trackElement, pos, trackType);
    bool isEnd = ChairliftPaintUtilIsLastTrack(ride, trackElement, pos, trackType);

    const auto* stationObj = ride.getStationObject();
    auto stationColour = GetStationColourScheme(session, trackElement);
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height, stationColour);

    if (!isStart && !isEnd)
    {
        imageId = session.trackColours.WithIndex(((direction == 0) ? SPR_20502 : SPR_20504));
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 13, height + 22 }, { 32, 6, 1 } });
    }

    imageId = session.supportColours.WithIndex(SPR_FLOOR_TILE_CHEQUERBOARD);
    paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });

    bool hasFence = TrackPaintUtilHasFence(edgeNW, pos, trackElement, ride, session.currentRotation);
    if (hasFence)
    {
        imageId = session.trackColours.WithIndex(SPR_FENCE_PICKET_NW);
        paintAddImageAsChild(session, imageId, { 0, 0, height }, { { 0, 2, height + 2 }, { 32, 1, 7 } });
    }
    TrackPaintUtilDrawStationCovers(session, edgeNW, hasFence, stationObj, height, stationColour);

    if ((direction == 2 && isStart) || (direction == 0 && isEnd))
    {
        imageId = session.trackColours.WithIndex(SPR_FENCE_PICKET_NE);
        paintAddImageAsChild(session, imageId, { 0, 0, height }, { { 2, 2, height + 4 }, { 1, 28, 7 } });
    }

    hasFence = TrackPaintUtilHasFence(edgeSE, pos, trackElement, ride, session.currentRotation);
    if (hasFence)
    {
        imageId = session.trackColours.WithIndex(SPR_FENCE_PICKET_SE);
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 30, height + 2 }, { 32, 1, 20 } });
    }
    TrackPaintUtilDrawStationCovers(session, edgeSE, hasFence, stationObj, height, stationColour);

    bool drawFrontColumn = true;
    bool drawBackColumn = true;
    if ((direction == 0 && isStart) || (direction == 2 && isEnd))
    {
        imageId = session.trackColours.WithIndex(SPR_FENCE_PICKET_SW);
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 30, 2, height + 4 }, { 1, 28, 27 } });

        imageId = session.trackColours.WithIndex(chairlift_bullwheel_frames[ride.chairliftBullwheelRotation / 16384]);
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 14, height + 4 }, { 4, 4, 19 } });

        imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_STATION_END_CAP_NE);
        paintAddImageAsChild(session, imageId, { 0, 0, height }, { { 14, 14, height + 4 }, { 4, 4, 19 } });

        drawFrontColumn = false;
    }
    else if ((direction == 2 && isStart) || (direction == 0 && isEnd))
    {
        imageId = session.trackColours.WithIndex(chairlift_bullwheel_frames[ride.chairliftBullwheelRotation / 16384]);
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 14, height + 4 }, { 4, 4, 19 } });

        imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_STATION_END_CAP_SW);
        paintAddImageAsChild(session, imageId, { 0, 0, height }, { { 14, 14, height + 4 }, { 4, 4, 19 } });

        drawBackColumn = false;
    }

    if (drawBackColumn)
    {
        imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_STATION_COLUMN_NE_SW);
        paintAddImageAsParent(session, imageId, { 0, 16, height + 2 }, { { 1, 16, height + 2 }, { 1, 1, 7 } });
    }

    if (drawFrontColumn)
    {
        imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_STATION_COLUMN_NE_SW);
        paintAddImageAsParent(
            session, imageId, { 30, 16, height + 2 }, { { 1, 16, height + 2 }, { 1, 1, 7 } }); // bound offset x is wrong?
    }

    paintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    paintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ChairliftPaintStationSeNw(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const CoordsXY pos = session.mapPosition;
    auto trackType = trackElement.GetTrackType();
    ImageId imageId;

    bool isStart = ChairliftPaintUtilIsFirstTrack(ride, trackElement, pos, trackType);
    bool isEnd = ChairliftPaintUtilIsLastTrack(ride, trackElement, pos, trackType);

    const auto* stationObj = ride.getStationObject();
    auto stationColour = GetStationColourScheme(session, trackElement);

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height,
        GetStationColourScheme(session, trackElement));

    if (!isStart && !isEnd)
    {
        imageId = session.trackColours.WithIndex(((direction == 1) ? SPR_20503 : SPR_20505));
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 13, 0, height + 22 }, { 6, 32, 1 } });
    }

    imageId = session.supportColours.WithIndex(SPR_FLOOR_TILE_CHEQUERBOARD);
    paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });

    bool hasFence = TrackPaintUtilHasFence(edgeNE, pos, trackElement, ride, session.currentRotation);
    if (hasFence)
    {
        imageId = session.trackColours.WithIndex(SPR_FENCE_PICKET_NE);
        paintAddImageAsChild(session, imageId, { 0, 0, height }, { { 2, 0, height + 2 }, { 1, 32, 7 } });
    }
    TrackPaintUtilDrawStationCovers(session, edgeNE, hasFence, stationObj, height, stationColour);

    if ((direction == 1 && isStart) || (direction == 3 && isEnd))
    {
        imageId = session.trackColours.WithIndex(SPR_FENCE_PICKET_NW);
        paintAddImageAsChild(session, imageId, { 0, 0, height }, { { 2, 2, height + 4 }, { 28, 1, 7 } });
    }

    hasFence = TrackPaintUtilHasFence(edgeSW, pos, trackElement, ride, session.currentRotation);
    if (hasFence)
    {
        imageId = session.trackColours.WithIndex(SPR_FENCE_PICKET_SW);
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 30, 0, height + 2 }, { 1, 32, 20 } });
    }
    TrackPaintUtilDrawStationCovers(session, edgeSW, hasFence, stationObj, height, stationColour);

    bool drawRightColumn = true;
    bool drawLeftColumn = true;
    if ((direction == 1 && isStart) || (direction == 3 && isEnd))
    {
        imageId = session.trackColours.WithIndex(chairlift_bullwheel_frames[ride.chairliftBullwheelRotation / 16384]);
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 14, height + 4 }, { 4, 4, 19 } });

        imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_STATION_END_CAP_SE);
        paintAddImageAsChild(session, imageId, { 0, 0, height }, { { 14, 14, height + 4 }, { 4, 4, 19 } });

        drawLeftColumn = false;
    }
    else if ((direction == 3 && isStart) || (direction == 1 && isEnd))
    {
        imageId = session.trackColours.WithIndex(SPR_FENCE_PICKET_SE);
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 30, height + 4 }, { 28, 1, 27 } });

        imageId = session.trackColours.WithIndex(chairlift_bullwheel_frames[ride.chairliftBullwheelRotation / 16384]);

        auto bb = BoundBoxXYZ{ { 14, 14, height + 4 }, { 4, 4, 19 } };
        paintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_STATION_END_CAP_NW);
        paintAddImageAsChild(session, imageId, { 0, 0, height }, bb);

        drawRightColumn = false;
    }

    if (drawLeftColumn)
    {
        imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_STATION_COLUMN_SE_NW);
        paintAddImageAsParent(session, imageId, { 16, 0, height + 2 }, { { 16, 1, height + 2 }, { 1, 1, 7 } });
    }

    if (drawRightColumn)
    {
        imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_STATION_COLUMN_SE_NW);
        paintAddImageAsParent(
            session, imageId, { 16, 30, height + 2 }, { { 16, 1, height + 2 }, { 1, 1, 7 } }); // bound offset x is wrong?
    }

    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    paintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00744068 */
static void ChairliftPaintStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (direction % 2)
    {
        ChairliftPaintStationSeNw(session, ride, trackSequence, direction, height, trackElement, supportType);
    }
    else
    {
        ChairliftPaintStationNeSw(session, ride, trackSequence, direction, height, trackElement, supportType);
    }
}

static void ChairliftPaintFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    if (direction & 1)
    {
        imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_CABLE_FLAT_SE_NW);
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 13, 0, height + 28 }, { 6, 32, 2 } });
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_CABLE_FLAT_SW_NE);
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 13, height + 28 }, { 32, 6, 2 } });
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    paintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00743FD8 */
static void ChairliftPaint25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_CABLE_UP_SW_NE);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 13, height + 28 }, { 32, 6, 2 } });
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;

        case 1:
            imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_CABLE_UP_NW_SE);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 13, 0, height + 28 }, { 6, 32, 2 } });
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;

        case 2:
            imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_CABLE_UP_NE_SW);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 13, height + 28 }, { 32, 6, 2 } });
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;

        case 3:
            imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_CABLE_UP_SE_NW);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 13, 0, height + 28 }, { 6, 32, 2 } });
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    paintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x00743FD8 */
static void ChairliftPaintFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.trackColours.WithIndex(SPR_20508);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 13, height + 28 }, { 32, 6, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20520);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 14, height + 1 }, { 4, 4, 25 } });

            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;

        case 1:
            imageId = session.trackColours.WithIndex(SPR_20509);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 13, 0, height + 28 }, { 6, 32, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20521);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 14, height + 1 }, { 4, 4, 25 } });

            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;

        case 2:
            imageId = session.trackColours.WithIndex(SPR_20510);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 13, height + 28 }, { 32, 6, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20522);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 14, height + 1 }, { 4, 4, 25 } });

            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;

        case 3:
            imageId = session.trackColours.WithIndex(SPR_20511);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 13, 0, height + 28 }, { 6, 32, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20523);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 14, height + 1 }, { 4, 4, 25 } });

            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    ChairliftPaintUtilDrawSupports(session, EnumToFlag(PaintSegment::centre), height, supportType);
    paintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x00743FF8 */
static void ChairliftPaint25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.trackColours.WithIndex(SPR_20512);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 13, height + 28 }, { 32, 6, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20524);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 14, height + 1 }, { 4, 4, 25 } });

            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;

        case 1:
            imageId = session.trackColours.WithIndex(SPR_20513);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 13, 0, height + 28 }, { 6, 32, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20525);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 14, height + 1 }, { 4, 4, 25 } });

            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;

        case 2:
            imageId = session.trackColours.WithIndex(SPR_20514);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 13, height + 28 }, { 32, 6, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20526);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 14, height + 1 }, { 4, 4, 25 } });

            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;

        case 3:
            imageId = session.trackColours.WithIndex(SPR_20515);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 13, 0, height + 28 }, { 6, 32, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20527);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 14, height + 1 }, { 4, 4, 25 } });

            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    ChairliftPaintUtilDrawSupports(session, EnumToFlag(PaintSegment::centre), height, supportType);
    paintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x00744008 */
static void ChairliftPaint25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ChairliftPaint25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x00744018 */
static void ChairliftPaintFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ChairliftPaint25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x00744028 */
static void ChairliftPaint25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ChairliftPaintFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x00744038 */
static void ChairliftPaintLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_CORNER_NW_SW);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 16, 0, height + 28 }, { 16, 16, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20532);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 16, 4, height }, { 2, 2, 27 } });

            imageId = session.supportColours.WithIndex(SPR_20536);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 4, height }, { 2, 2, 27 } });

            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;

        case 1:
            imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_CORNER_NW_NE);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height + 28 }, { 16, 16, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20533);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 16, 4, height }, { 2, 2, 27 } });

            imageId = session.supportColours.WithIndex(SPR_20537);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 16, height }, { 2, 2, 27 } });
            break;

        case 2:
            imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_CORNER_SE_NE);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 16, height + 28 }, { 16, 16, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20534);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 16, height }, { 2, 2, 27 } });

            imageId = session.supportColours.WithIndex(SPR_20538);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 16, 28, height }, { 2, 2, 27 } });

            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;

        case 3:
            imageId = session.trackColours.WithIndex(SPR_CHAIRLIFT_CORNER_SW_SE);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 16, 16, height + 28 }, { 16, 16, 2 } });

            imageId = session.supportColours.WithIndex(SPR_20535);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 16, height }, { 2, 2, 27 } });

            imageId = session.supportColours.WithIndex(SPR_20539);
            paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 16, 28, height }, { 2, 2, 27 } });

            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    ChairliftPaintUtilDrawSupports(
        session, paintUtilRotateSegments(EnumsToFlags(PaintSegment::topLeft, PaintSegment::bottomLeft), direction), height,
        supportType);

    paintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00744048 */
static void ChairliftPaintRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ChairliftPaintLeftQuarterTurn1Tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/* 0x008AAA0C */
TrackPaintFunction GetTrackPaintFunctionChairlift(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::beginStation:
        case TrackElemType::middleStation:
        case TrackElemType::endStation:
            return ChairliftPaintStation;

        case TrackElemType::flat:
            return ChairliftPaintFlat;

        case TrackElemType::flatToUp25:
            return ChairliftPaintFlatTo25DegUp;
        case TrackElemType::up25:
            return ChairliftPaint25DegUp;
        case TrackElemType::up25ToFlat:
            return ChairliftPaint25DegUpToFlat;

        case TrackElemType::flatToDown25:
            return ChairliftPaintFlatTo25DegDown;
        case TrackElemType::down25:
            return ChairliftPaint25DegDown;
        case TrackElemType::down25ToFlat:
            return ChairliftPaint25DegDownToFlat;

        case TrackElemType::leftQuarterTurn1Tile:
            return ChairliftPaintLeftQuarterTurn1Tile;
        case TrackElemType::rightQuarterTurn1Tile:
            return ChairliftPaintRightQuarterTurn1Tile;
        default:
            return TrackPaintFunctionDummy;
    }
}
