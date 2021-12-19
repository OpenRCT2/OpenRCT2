/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
    SPR_DODGEMS_FLOOR = 21925,
    SPR_DODGEMS_ROOF_FRAME = 21926, // 4 directions
    SPR_DODGEMS_ROOF_GLASS = 21930, // 4 directions
    SPR_DODGEMS_FENCE_TOP_RIGHT = 21934,
    SPR_DODGEMS_FENCE_BOTTOM_RIGHT = 21935,
    SPR_DODGEMS_FENCE_BOTTOM_LEFT = 21936,
    SPR_DODGEMS_FENCE_TOP_LEFT = 21937
};

static constexpr const uint32_t dodgems_fence_sprites[] = {
    SPR_DODGEMS_FENCE_TOP_RIGHT,
    SPR_DODGEMS_FENCE_BOTTOM_RIGHT,
    SPR_DODGEMS_FENCE_BOTTOM_LEFT,
    SPR_DODGEMS_FENCE_TOP_LEFT,
};

static void paint_dodgems_roof(paint_session& session, int32_t height, int32_t offset)
{
    uint32_t image_id = (SPR_DODGEMS_ROOF_FRAME + offset) | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParent(session, image_id, { 0, 0, height }, { 32, 32, 2 });

    image_id = (SPR_DODGEMS_ROOF_GLASS + offset) | (EnumValue(FilterPaletteID::PaletteDarken3) << 19) | IMAGE_TYPE_TRANSPARENT;
    PaintAttachToPreviousPS(session, image_id, 0, 0);
}

static void paint_dodgems(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint8_t relativeTrackSequence = track_map_4x4[direction][trackSequence];

    int32_t edges = edges_4x4[relativeTrackSequence];

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    if (stationObject != nullptr && !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
    {
        uint32_t imageId = SPR_DODGEMS_FLOOR | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 30, 30, 1 }, { 1, 1, height });

        track_paint_util_paint_fences(
            session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_SUPPORTS], height,
            dodgems_fence_sprites, session.CurrentRotation);

        switch (direction)
        {
            case 2:
                trackSequence = 15 - trackSequence;
                [[fallthrough]];
            case 0:
                if ((trackSequence / 4) & 1)
                {
                    paint_dodgems_roof(session, height + 30, 0);
                }
                else
                {
                    paint_dodgems_roof(session, height + 30, 2);
                }
                break;

            case 3:
                trackSequence = 15 - trackSequence;
                [[fallthrough]];
            case 1:
                if ((trackSequence / 4) & 1)
                {
                    paint_dodgems_roof(session, height + 30, 1);
                }
                else
                {
                    paint_dodgems_roof(session, height + 30, 3);
                }
                break;
        }
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, height + 36, 0x20);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/**
 * rct2:
 */
TRACK_PAINT_FUNCTION get_track_paint_function_dodgems(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack4x4)
    {
        return nullptr;
    }

    return paint_dodgems;
}
