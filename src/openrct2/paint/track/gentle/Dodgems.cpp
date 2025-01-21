/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../object/StationObject.h"
#include "../../../ride/Ride.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"

using namespace OpenRCT2;

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

static constexpr uint32_t kDodgemsFenceSprites[] = {
    SprDodgemsFenceTopRight,
    SprDodgemsFenceBottomRight,
    SprDodgemsFenceBottomLeft,
    SprDodgemsFenceTopLeft,
};

static void PaintDodgemsRoof(PaintSession& session, int32_t height, int32_t offset)
{
    auto imageId = session.TrackColours.WithIndex((SprDodgemsRoofFrame + offset));
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 2 });

    imageId = ImageId(SprDodgemsRoofGlass + offset).WithTransparency(FilterPaletteID::PaletteDarken3);
    PaintAttachToPreviousPS(session, imageId, 0, 0);
}

static void PaintDodgems(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t relativeTrackSequence = kTrackMap4x4[direction][trackSequence];

    int32_t edges = kEdges4x4[relativeTrackSequence];

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height,
        GetStationColourScheme(session, trackElement));

    const StationObject* stationObject = ride.GetStationObject();

    if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
    {
        auto imageId = session.SupportColours.WithIndex(SprDodgemsFloor);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 1, height }, { 30, 30, 1 } });

        TrackPaintUtilPaintFences(
            session, edges, session.MapPosition, trackElement, ride, session.SupportColours, height, kDodgemsFenceSprites,
            session.CurrentRotation);

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

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, height + 36, 0x20);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/**
 * rct2:
 */
TrackPaintFunction GetTrackPaintFunctionDodgems(OpenRCT2::TrackElemType trackType)
{
    if (trackType != TrackElemType::FlatTrack4x4)
    {
        return TrackPaintFunctionDummy;
    }

    return PaintDodgems;
}
