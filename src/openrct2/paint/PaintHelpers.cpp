/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Viewport.h"
#include "../ride/TrackPaint.h"
#include "Paint.h"

paint_struct * sub_98196C_rotated(
    paint_session * session,
    uint8 direction,
    uint32 image_id,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset)
{
    if (direction & 1) {
        return sub_98196C(
            session, image_id, y_offset, x_offset, bound_box_length_y, bound_box_length_x, bound_box_length_z, z_offset);
    } else {
        return sub_98196C(
            session, image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset);
    }
}

paint_struct * sub_98197C_rotated(
    paint_session * session,
    uint8 direction,
    uint32 image_id,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset,
    sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z)
{
    if (direction & 1) {
        return sub_98197C(
            session, image_id, y_offset, x_offset, bound_box_length_y, bound_box_length_x, bound_box_length_z, z_offset,
            bound_box_offset_y, bound_box_offset_x, bound_box_offset_z);
    } else {
        return sub_98197C(
            session, image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset,
            bound_box_offset_x, bound_box_offset_y, bound_box_offset_z);
    }
}

paint_struct * sub_98199C_rotated(
    paint_session * session,
    uint8 direction,
    uint32 image_id,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset,
    sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z)
{
    if (direction & 1) {
        return sub_98199C(
            session, image_id, y_offset, x_offset, bound_box_length_y, bound_box_length_x, bound_box_length_z, z_offset,
            bound_box_offset_y, bound_box_offset_x, bound_box_offset_z);
    } else {
        return sub_98199C(
            session, image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset,
            bound_box_offset_x, bound_box_offset_y, bound_box_offset_z);
    }
}

void paint_util_push_tunnel_rotated(paint_session * session, uint8 direction, uint16 height, uint8 type)
{
    if (direction & 1) {
        paint_util_push_tunnel_right(session, height, type);
    } else {
        paint_util_push_tunnel_left(session, height, type);
    }
}
