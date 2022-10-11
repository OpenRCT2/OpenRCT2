/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../object/StationObject.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Ride.h"
#include "../Track.h"
#include "../TrackPaint.h"

enum
{
    SprFlyingSaucersFloor = 21920,
    SprFlyingSaucersFenceNe = 21921,
    SprFlyingSaucersFenceSe = 21922,
    SprFlyingSaucersFenceSw = 21923,
    SprFlyingSaucersFenceNw = 21924,
};

static constexpr const uint32_t FlyingSaucersFenceSprites[] = {
    SprFlyingSaucersFenceNe,
    SprFlyingSaucersFenceSe,
    SprFlyingSaucersFenceSw,
    SprFlyingSaucersFenceNw,
};

/**
 * rct2: 0x008873D8
 */
static void paint_flying_saucers(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint8_t relativeTrackSequence = track_map_4x4[direction][trackSequence];

    int32_t edges = edges_4x4[relativeTrackSequence];

    WoodenASupportsPaintSetup(session, direction & 1, 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
    {
        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SprFlyingSaucersFloor);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 30, 30, 1 }, { 1, 1, height });
    }

    track_paint_util_paint_fences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_TRACK], height,
        FlyingSaucersFenceSprites, session.CurrentRotation);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/**
 * rct2: 0x00887208
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionFlyingSaucers(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack4x4)
    {
        return nullptr;
    }

    return paint_flying_saucers;
}
