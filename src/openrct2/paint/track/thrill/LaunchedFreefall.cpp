/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../ride/Ride.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../../world/tile_element/TileElement.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

#include <cassert>

using namespace OpenRCT2;

enum
{
    SPR_LAUNCHED_FREEFALL_TOWER_BASE = 14564,
    SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT = 14565,
    SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT_TOP = 14566,
};

/** rct2: 0x006FD1F8 */
static void PaintLaunchedFreefallBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kTrackMap3x3[direction][trackSequence];

    int32_t edges = kEdges3x3[trackSequence];

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height,
        GetStationColourScheme(session, trackElement));

    const StationObject* stationObject = ride.getStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.SupportColours, height, kFloorSpritesTileChequerboard, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours, height, kFenceSpritesPicket,
        session.CurrentRotation);

    if (trackSequence == 0)
    {
        auto imageId = session.TrackColours.WithIndex(SPR_LAUNCHED_FREEFALL_TOWER_BASE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 8, 8, height + 3 }, { 2, 2, 27 } });

        height += 32;
        imageId = session.TrackColours.WithIndex(SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 2, 2, 30 } });

        height += 32;
        imageId = session.TrackColours.WithIndex(SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 2, 2, 30 } });

        PaintUtilSetVerticalTunnel(session, height + 32);

        height -= 64;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = kSegmentsAll;
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::topRight, PaintSegment::right);
            break;
        case 2:
            blockedSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topRight, PaintSegment::right);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::top, PaintSegment::topRight, PaintSegment::right, PaintSegment::bottomRight,
                PaintSegment::bottom);
            break;
        case 4:
            blockedSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::left);
            break;
        case 5:
            blockedSegments = EnumsToFlags(PaintSegment::right, PaintSegment::bottomRight, PaintSegment::bottom);
            break;
        case 6:
            blockedSegments = EnumsToFlags(
                PaintSegment::top, PaintSegment::topLeft, PaintSegment::left, PaintSegment::bottomLeft, PaintSegment::bottom);
            break;
        case 7:
            blockedSegments = EnumsToFlags(
                PaintSegment::left, PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::bottomRight,
                PaintSegment::right);
            break;
        case 8:
            blockedSegments = EnumsToFlags(PaintSegment::left, PaintSegment::bottomLeft, PaintSegment::bottom);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, blockedSegments, 0xFFFF, 0);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~blockedSegments, height + 2, 0x20);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x006FD208 */
static void PaintLaunchedFreefallTowerSection(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence == 1)
    {
        return;
    }

    auto imageId = session.TrackColours.WithIndex(SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 2, 2, 30 } });

    const TileElement* nextTileElement = reinterpret_cast<const TileElement*>(&trackElement) + 1;
    if (trackElement.IsLastForTile() || trackElement.GetClearanceZ() != nextTileElement->GetBaseZ())
    {
        imageId = session.TrackColours.WithIndex(SPR_LAUNCHED_FREEFALL_TOWER_SEGMENT_TOP);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 2, 2, 30 } });
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);

    PaintUtilSetVerticalTunnel(session, height + 32);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x006FD0E8
 */
TrackPaintFunction GetTrackPaintFunctionLaunchedFreefall(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::towerBase:
            return PaintLaunchedFreefallBase;

        case TrackElemType::towerSection:
            return PaintLaunchedFreefallTowerSection;
        default:
            return TrackPaintFunctionDummy;
    }
}
