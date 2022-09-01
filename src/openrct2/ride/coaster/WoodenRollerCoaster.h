/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../paint/Paint.h"
#include "../../world/Location.hpp"
#include "../TrackPaint.h"

#include <cstdint>

struct sprite_bb_2
{
    uint32_t sprite_id_a;
    uint32_t sprite_id_b;
    CoordsXYZ offset;
    CoordsXYZ bb_offset;
    CoordsXYZ bb_size;
};

template<bool isClassic> uint32_t wooden_rc_get_track_colour(const paint_session& session)
{
    if (isClassic)
        return session.TrackColours[SCHEME_TRACK];
    else
        return (session.TrackColours[SCHEME_TRACK] & ~0xF80000) | session.TrackColours[SCHEME_SUPPORTS];
}

uint32_t wooden_rc_get_rails_colour(paint_session& session);

template<bool isClassic>
paint_struct* wooden_rc_track_paint(
    paint_session& session, uint32_t imageIdTrack, uint32_t imageIdRails, uint8_t direction, int8_t x_offset, int8_t y_offset,
    int16_t bound_box_length_x, int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset,
    int16_t bound_box_offset_x, int16_t bound_box_offset_y, int16_t bound_box_offset_z)
{
    uint32_t imageId = imageIdTrack | wooden_rc_get_track_colour<isClassic>(session);
    uint32_t railsImageId = imageIdRails | wooden_rc_get_rails_colour(session);

    PaintAddImageAsParentRotated(
        session, direction, imageId, { x_offset, y_offset, z_offset },
        { bound_box_length_x, bound_box_length_y, bound_box_length_z },
        { bound_box_offset_x, bound_box_offset_y, bound_box_offset_z });
    return PaintAddImageAsChildRotated(
        session, direction, railsImageId, { x_offset, y_offset, z_offset },
        { bound_box_length_x, bound_box_length_y, bound_box_length_z },
        { bound_box_offset_x, bound_box_offset_y, bound_box_offset_z });
}

template<bool isClassic> void wooden_rc_track_paint_bb(paint_session& session, const sprite_bb_2* bb, int16_t height)
{
    if (bb->sprite_id_a == 0)
        return;

    uint32_t imageId = bb->sprite_id_a | wooden_rc_get_track_colour<isClassic>(session);
    PaintAddImageAsParent(
        session, imageId, { bb->offset.x, bb->offset.y, height + bb->offset.z }, bb->bb_size,
        { bb->bb_offset.x, bb->bb_offset.y, height + bb->bb_offset.z });
    if (bb->sprite_id_b != 0)
    {
        uint32_t railsImageId = bb->sprite_id_b | wooden_rc_get_rails_colour(session);
        PaintAddImageAsChild(
            session, railsImageId, { bb->offset.x, bb->offset.y, height + bb->offset.z }, bb->bb_size,
            { bb->bb_offset.x, bb->bb_offset.y, height + bb->bb_offset.z });
    }
}

TRACK_PAINT_FUNCTION get_track_paint_function_classic_wooden_rc_fallback(int32_t trackType);
