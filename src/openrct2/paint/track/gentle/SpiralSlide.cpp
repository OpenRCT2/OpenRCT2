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
#include "../../../ride/RideEntry.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"

using namespace OpenRCT2;

enum
{
    SpiralSlideLeftR0 = 0,
    SpiralSlideCentreR0 = 1,
    SpiralSlideRightR0 = 2,

    SpiralSlideLeftR1 = 3,
    SpiralSlideCentreR1 = 4,
    SpiralSlideRightR1 = 5,

    SpiralSlideLeftR2 = 6,
    SpiralSlideCentreR2 = 7,
    SpiralSlideRightR2 = 8,

    SpiralSlideLeftR3 = 9,
    SpiralSlideCentreR3 = 10,
    SpiralSlideRightR3 = 11,

    SpiralSlideInsideR1 = 12,
    SpiralSlideInsideR2 = 13,

    SpiralSlideBaseA = 14,
    SpiralSlideBaseB = 15,

    SpiralSlideFenceTopRight = 16,
    SpiralSlideFenceBottomRight = 17,
    SpiralSlideFenceBottomLeft = 18,
    SpiralSlideFenceTopLeft = 19,

    SpiralSlidePeep = 20, // 46 sprites per direction
};

static void SpiralSlidePaintTileRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    ImageId imageId;

    if (direction == 0)
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideRightR0));
    else if (direction == 1)
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideRightR1));
    else if (direction == 2)
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideRightR2));
    else
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideRightR3));

    PaintAddImageAsParent(session, imageId, { 16, 16, height }, { { 16, 0, height + 3 }, { 16, 16, 108 } });
}

static void SpiralSlidePaintTileLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    ImageId imageId;

    if (direction == 0)
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideLeftR0));
    else if (direction == 1)
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideLeftR1));
    else if (direction == 2)
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideLeftR2));
    else
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideLeftR3));

    PaintAddImageAsParent(session, imageId, { 16, 16, height }, { { 0, 16, height + 3 }, { 16, 16, 108 } });
}

static void SpiralSlidePaintTileFront(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    ImageId imageId;

    if (direction == 1)
    {
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideInsideR1));
        PaintAddImageAsParent(session, imageId, { 16, 16, height }, { { -12, 0, height + 3 }, { 2, 16, 108 } });
    }
    else if (direction == 2)
    {
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideInsideR2));
        PaintAddImageAsParent(session, imageId, { 16, 16, height }, { { 0, -12, height + 3 }, { 16, 2, 108 } });
    }

    if (direction == 0)
    {
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideCentreR0));
        PaintAddImageAsParent(session, imageId, { 16, 16, height }, { { 0, 8, height + 3 }, { 16, 8, 108 } });
    }
    else if (direction == 1)
    {
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideCentreR1));
        PaintAddImageAsParent(session, imageId, { 16, 16, height }, { { 14, 0, height + 3 }, { 2, 16, 108 } });
    }
    else if (direction == 2)
    {
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideCentreR2));
        PaintAddImageAsParent(session, imageId, { 16, 16, height }, { { 0, 14, height + 3 }, { 16, 2, 108 } });
    }
    else if (direction == 3)
    {
        imageId = session.TrackColours.WithIndex((rideEntry->Cars[0].base_image_id + SpiralSlideCentreR3));
        PaintAddImageAsParent(session, imageId, { 16, 16, height }, { { 8, 0, height + 3 }, { 8, 16, 108 } });
    }

    if (session.DPI.zoom_level <= ZoomLevel{ 0 } && ride.slide_in_use != 0)
    {
        uint8_t slide_progress = ride.spiral_slide_progress;
        if (slide_progress != 0)
        {
            slide_progress--;
        }

        if (slide_progress == 46)
        {
            slide_progress--;
        }

        if (slide_progress < 46)
        {
            int32_t offset = rideEntry->Cars[0].base_image_id + SpiralSlidePeep + 46 * direction;
            CoordsXYZ boundingBox = { 0, 0, 108 };
            CoordsXYZ boundingBoxOffset = { 0, 0, static_cast<int16_t>(height + 3) };

            if (direction == 0)
            {
                boundingBoxOffset.x = 0;
                boundingBoxOffset.y = 8;
                boundingBox.y = 8;
                boundingBox.x = 16;
            }
            else if (direction == 1)
            {
                boundingBoxOffset.x = 14;
                boundingBoxOffset.y = 0;
                boundingBox.y = 2;
                boundingBox.x = 16;
            }
            else if (direction == 2)
            {
                boundingBoxOffset.x = 0;
                boundingBoxOffset.y = 14;
                boundingBox.y = 16;
                boundingBox.x = 2;
            }
            else if (direction == 3)
            {
                boundingBoxOffset.x = 8;
                boundingBoxOffset.y = 0;
                boundingBox.y = 16;
                boundingBox.x = 8;
            }

            imageId = ImageId(offset + slide_progress, ride.slide_peep_t_shirt_colour, COLOUR_GREY);

            PaintAddImageAsChild(session, imageId, { 16, 16, height }, { boundingBoxOffset, boundingBox });
        }
    }
}

/**
 * rct: 0x007485C8
 */
static void PaintSpiralSlide(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    trackSequence = kTrackMap2x2[direction][trackSequence];

    int32_t edges = kEdges2x2[trackSequence];

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height,
        GetStationColourScheme(session, trackElement));

    // Base
    const StationObject* stationObject = ride.GetStationObject();

    if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
    {
        auto imageId = session.SupportColours.WithIndex(
            rideEntry->Cars[0].base_image_id + ((direction & 1) ? SpiralSlideBaseB : SpiralSlideBaseA));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
    }

    const uint32_t spiral_slide_fence_sprites[] = {
        rideEntry->Cars[0].base_image_id + SpiralSlideFenceTopRight,
        rideEntry->Cars[0].base_image_id + SpiralSlideFenceBottomRight,
        rideEntry->Cars[0].base_image_id + SpiralSlideFenceBottomLeft,
        rideEntry->Cars[0].base_image_id + SpiralSlideFenceTopLeft,
    };

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours, height, spiral_slide_fence_sprites,
        session.CurrentRotation);

    switch (trackSequence)
    {
        case 1:
            SpiralSlidePaintTileRight(session, ride, trackSequence, direction, height, trackElement, supportType);
            break;
        case 2:
            SpiralSlidePaintTileLeft(session, ride, trackSequence, direction, height, trackElement, supportType);
            break;
        case 3:
            SpiralSlidePaintTileFront(session, ride, trackSequence, direction, height, trackElement, supportType);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 128);
}

/**
 * rct2: 0x0074840C
 */
TrackPaintFunction GetTrackPaintFunctionSpiralSlide(OpenRCT2::TrackElemType trackType)
{
    if (trackType != TrackElemType::FlatTrack2x2)
    {
        return TrackPaintFunctionDummy;
    }

    return PaintSpiralSlide;
}
