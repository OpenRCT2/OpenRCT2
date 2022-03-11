/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Viewport.h"
#include "../ride/TrackPaint.h"
#include "Paint.h"

paint_struct* PaintAddImageAsParentRotated(
    paint_session& session, uint8_t direction, uint32_t image_id, int32_t x_offset, int32_t y_offset,
    int32_t bound_box_length_x, int32_t bound_box_length_y, int32_t bound_box_length_z, int32_t z_offset,
    int32_t bound_box_offset_x, int32_t bound_box_offset_y, int32_t bound_box_offset_z)
{
    if (direction & 1)
    {
        return PaintAddImageAsParent(
            session, image_id, { y_offset, x_offset, z_offset }, { bound_box_length_y, bound_box_length_x, bound_box_length_z },
            { bound_box_offset_y, bound_box_offset_x, bound_box_offset_z });
    }

    return PaintAddImageAsParent(
        session, image_id, { x_offset, y_offset, z_offset }, { bound_box_length_x, bound_box_length_y, bound_box_length_z },
        { bound_box_offset_x, bound_box_offset_y, bound_box_offset_z });
}

paint_struct* PaintAddImageAsParentRotated(
    paint_session& session, const uint8_t direction, const uint32_t image_id, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize, const CoordsXYZ& boundBoxOffset)
{
    if (direction & 1)
    {
        return PaintAddImageAsParent(
            session, image_id, { offset.y, offset.x, offset.z }, { boundBoxSize.y, boundBoxSize.x, boundBoxSize.z },
            { boundBoxOffset.y, boundBoxOffset.x, boundBoxOffset.z });
    }

    return PaintAddImageAsParent(session, image_id, offset, boundBoxSize, boundBoxOffset);
}

paint_struct* PaintAddImageAsParentRotated(
    paint_session& session, const uint8_t direction, const uint32_t image_id, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize)
{
    if (direction & 1)
    {
        return PaintAddImageAsParent(
            session, image_id, { offset.y, offset.x, offset.z }, { boundBoxSize.y, boundBoxSize.x, boundBoxSize.z });
    }

    return PaintAddImageAsParent(session, image_id, offset, boundBoxSize);
}

paint_struct* PaintAddImageAsChildRotated(
    paint_session& session, uint8_t direction, uint32_t image_id, int32_t x_offset, int32_t y_offset,
    int32_t bound_box_length_x, int32_t bound_box_length_y, int32_t bound_box_length_z, int32_t z_offset,
    int32_t bound_box_offset_x, int32_t bound_box_offset_y, int32_t bound_box_offset_z)
{
    if (direction & 1)
    {
        return PaintAddImageAsChild(
            session, image_id, y_offset, x_offset, bound_box_length_y, bound_box_length_x, bound_box_length_z, z_offset,
            bound_box_offset_y, bound_box_offset_x, bound_box_offset_z);
    }

    return PaintAddImageAsChild(
        session, image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset,
        bound_box_offset_x, bound_box_offset_y, bound_box_offset_z);
}

paint_struct* PaintAddImageAsChildRotated(
    paint_session& session, const uint8_t direction, const uint32_t image_id, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize, const CoordsXYZ& boundBoxOffset)
{
    if (direction & 1)
    {
        return PaintAddImageAsChild(
            session, image_id, { offset.y, offset.x, offset.z }, { boundBoxSize.y, boundBoxSize.x, boundBoxSize.z },
            { boundBoxOffset.y, boundBoxOffset.x, boundBoxOffset.z });
    }

    return PaintAddImageAsChild(session, image_id, offset, boundBoxSize, boundBoxOffset);
}

void paint_util_push_tunnel_rotated(paint_session& session, uint8_t direction, uint16_t height, uint8_t type)
{
    if (direction & 1)
    {
        paint_util_push_tunnel_right(session, height, type);
    }
    else
    {
        paint_util_push_tunnel_left(session, height, type);
    }
}
