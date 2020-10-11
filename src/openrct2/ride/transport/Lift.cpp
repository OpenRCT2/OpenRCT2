/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../common.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Track.h"
#include "../TrackPaint.h"

enum
{
    SPR_LIFT_CAGE_BACK = 14994,
    SPR_LIFT_CAGE_FRONT = 14995,
    SPR_LIFT_CAGE_NE_BACK = 14996,
    SPR_LIFT_CAGE_NE_FRONT = 14997,
    SPR_LIFT_CAGE_SE_BACK = 14998,
    SPR_LIFT_CAGE_SE_FRONT = 14999,
    SPR_LIFT_CAGE_SW_BACK = 15000,
    SPR_LIFT_CAGE_SW_FRONT = 15001,
    SPR_LIFT_CAGE_NW_BACK = 15002,
    SPR_LIFT_CAGE_NW_FRONT = 15003,
};

static constexpr const uint32_t lift_cage_sprites[][2] = {
    { SPR_LIFT_CAGE_BACK, SPR_LIFT_CAGE_FRONT },       { SPR_LIFT_CAGE_NE_BACK, SPR_LIFT_CAGE_NE_FRONT },
    { SPR_LIFT_CAGE_SE_BACK, SPR_LIFT_CAGE_SE_FRONT }, { SPR_LIFT_CAGE_SW_BACK, SPR_LIFT_CAGE_SW_FRONT },
    { SPR_LIFT_CAGE_NW_BACK, SPR_LIFT_CAGE_NW_FRONT },
};

static void paint_lift_cage(paint_session* session, int8_t index, uint32_t colourFlags, int32_t height, uint8_t rotation)
{
    uint32_t imageId;

    imageId = lift_cage_sprites[1 + index][0] | colourFlags;
    sub_98197C(session, imageId, 0, 0, 2, 2, 30, height, 2, 2, height);

    imageId = lift_cage_sprites[1 + index][1] | colourFlags;
    sub_98197C(session, imageId, 0, 0, 2, 2, 30, height, 28, 28, height);
}

/** rct2: 0x0076C6CC */
static void paint_lift_base(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    if (trackSequence == 0)
    {
        paint_lift_cage(session, direction, session->TrackColours[SCHEME_TRACK], height, session->CurrentRotation);

        paint_lift_cage(session, -1, session->TrackColours[SCHEME_TRACK], height + 32, session->CurrentRotation);

        paint_lift_cage(session, -1, session->TrackColours[SCHEME_TRACK], height + 64, session->CurrentRotation);

        paint_util_set_vertical_tunnel(session, height + 96);
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);

#ifdef __TESTPAINT__
        paint_util_set_general_support_height(session, height + 32, 0x20);
#else
        paint_util_set_general_support_height(session, height + 96, 0x20);
#endif

        return;
    }

    int32_t edges = edges_3x3[trackSequence];

    uint32_t imageId = SPR_FLOOR_METAL_B | session->TrackColours[SCHEME_SUPPORTS];
    sub_98197C(session, imageId, 0, 0, 32, 32, 1, height, 0, 0, height);

    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        track_paint_util_paint_fences(
            session, edges, session->MapPosition, tileElement, ride, session->TrackColours[SCHEME_TRACK], height,
            fenceSpritesMetalB, session->CurrentRotation);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 1:
            blockedSegments = SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 2:
            blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 3:
            blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 4:
            blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8;
            break;
        case 5:
            blockedSegments = SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 6:
            blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
        case 7:
            blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC;
            break;
        case 8:
            blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
    }
    paint_util_set_segment_support_height(session, blockedSegments, 0xFFFF, 0);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~blockedSegments, height + 2, 0x20);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0076C6DC */
static void paint_lift_tower_section(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (trackSequence == 1)
    {
        return;
    }

    paint_lift_cage(session, -1, session->TrackColours[SCHEME_TRACK], height, session->CurrentRotation);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);

    paint_util_set_vertical_tunnel(session, height + 32);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/**
 * rct2: 0x0076C5BC
 */
TRACK_PAINT_FUNCTION get_track_paint_function_lift(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::TowerBase:
            return paint_lift_base;

        case TrackElemType::TowerSection:
            return paint_lift_tower_section;
    }

    return nullptr;
}
