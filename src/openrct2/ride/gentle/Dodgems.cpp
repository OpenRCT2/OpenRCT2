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
#include "../../util/Util.h"
#include "../Ride.h"
#include "../Track.h"
#include "../TrackPaint.h"

enum
{
    SprDodgemsFloor = 21925,
    SprDodgemsRoofFrame = 21926, // 4 directions
    SprDodgemsRoofGlass = 21930, // 4 directions
    SprDodgemsFenceTopRight = 21934,
    SprDodgemsFenceBottomRight = 21935,
    SprDodgemsFenceBottomLeft = 21936,
    SprDodgemsFenceTopLeft = 21937
};

static constexpr const uint32_t DodgemsFenceSprites[] = {
    SprDodgemsFenceTopRight,
    SprDodgemsFenceBottomRight,
    SprDodgemsFenceBottomLeft,
    SprDodgemsFenceTopLeft,
};

static void PaintDodgemsRoof(PaintSession& session, int32_t height, int32_t offset)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex((SprDodgemsRoofFrame + offset));
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 2 });

    imageId = ImageId(SprDodgemsRoofGlass + offset).WithTransparancy(FilterPaletteID::PaletteDarken3);
    PaintAttachToPreviousPS(session, imageId, 0, 0);
}

static void PaintDodgems(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint8_t relativeTrackSequence = track_map_4x4[direction][trackSequence];

    int32_t edges = edges_4x4[relativeTrackSequence];

    WoodenASupportsPaintSetup(session, direction & 1, 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
    {
        auto imageId = session.TrackColours[SCHEME_SUPPORTS].WithIndex(SprDodgemsFloor);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 30, 30, 1 }, { 1, 1, height });

        track_paint_util_paint_fences(
            session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_SUPPORTS], height,
            DodgemsFenceSprites, session.CurrentRotation);

        switch (direction)
        {
            case 2:
                trackSequence = 15 - trackSequence;
                [[fallthrough]];
            case 0:
                if ((trackSequence / 4) & 1)
                {
                    PaintDodgemsRoof(session, height + 30, 0);
                }
                else
                {
                    PaintDodgemsRoof(session, height + 30, 2);
                }
                break;

            case 3:
                trackSequence = 15 - trackSequence;
                [[fallthrough]];
            case 1:
                if ((trackSequence / 4) & 1)
                {
                    PaintDodgemsRoof(session, height + 30, 1);
                }
                else
                {
                    PaintDodgemsRoof(session, height + 30, 3);
                }
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, height + 36, 0x20);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/**
 * rct2:
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionDodgems(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack4x4)
    {
        return nullptr;
    }

    return PaintDodgems;
}
