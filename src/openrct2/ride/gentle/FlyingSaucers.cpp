/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Track.h"
#include "../TrackPaint.h"

enum
{
    SPR_FLYING_SAUCERS_FLOOR = 21920,
    SPR_FLYING_SAUCERS_FENCE_NE = 21921,
    SPR_FLYING_SAUCERS_FENCE_SE = 21922,
    SPR_FLYING_SAUCERS_FENCE_SW = 21923,
    SPR_FLYING_SAUCERS_FENCE_NW = 21924,
};

static constexpr const uint32_t flying_saucers_fence_sprites[] = {
    SPR_FLYING_SAUCERS_FENCE_NE,
    SPR_FLYING_SAUCERS_FENCE_SE,
    SPR_FLYING_SAUCERS_FENCE_SW,
    SPR_FLYING_SAUCERS_FENCE_NW,
};

/**
 * rct2: 0x008873D8
 */
static void paint_flying_saucers(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint8_t relativeTrackSequence = track_map_4x4[direction][trackSequence];

    int32_t edges = edges_4x4[relativeTrackSequence];

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    uint32_t imageId = SPR_FLYING_SAUCERS_FLOOR | session->TrackColours[SCHEME_TRACK];
    PaintAddImageAsParent(session, imageId, 0, 0, 30, 30, 1, height, 1, 1, height);

    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        track_paint_util_paint_fences(
            session, edges, session->MapPosition, tileElement, ride, session->TrackColours[SCHEME_TRACK], height,
            flying_saucers_fence_sprites, session->CurrentRotation);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/**
 * rct2: 0x00887208
 */
TRACK_PAINT_FUNCTION get_track_paint_function_flying_saucers(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack4x4)
    {
        return nullptr;
    }

    return paint_flying_saucers;
}
