#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../interface/viewport.h"
#include "../ride/track_paint.h"
#include "paint.h"

paint_struct * sub_98196C_rotated(
	uint8 direction,
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	sint16 z_offset)
{
	if (direction & 1) {
		return sub_98196C(image_id, y_offset, x_offset, bound_box_length_y, bound_box_length_x, bound_box_length_z, z_offset, get_current_rotation());
	} else {
		return sub_98196C(image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset, get_current_rotation());
	}
}

paint_struct * sub_98197C_rotated(
	uint8 direction,
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	sint16 z_offset,
	sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z)
{
	if (direction & 1) {
		return sub_98197C(image_id, y_offset, x_offset, bound_box_length_y, bound_box_length_x, bound_box_length_z, z_offset, bound_box_offset_y, bound_box_offset_x, bound_box_offset_z, get_current_rotation());
	} else {
		return sub_98197C(image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset, bound_box_offset_x, bound_box_offset_y, bound_box_offset_z, get_current_rotation());
	}
}

paint_struct * sub_98199C_rotated(
	uint8 direction,
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	sint16 z_offset,
	sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z)
{
	if (direction & 1) {
		return sub_98199C(image_id, y_offset, x_offset, bound_box_length_y, bound_box_length_x, bound_box_length_z, z_offset, bound_box_offset_y, bound_box_offset_x, bound_box_offset_z, get_current_rotation());
	} else {
		return sub_98199C(image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset, bound_box_offset_x, bound_box_offset_y, bound_box_offset_z, get_current_rotation());
	}
}

void paint_util_push_tunnel_rotated(uint8 direction, uint16 height, uint8 type)
{
	if (direction & 1) {
		paint_util_push_tunnel_right(height, type);
	} else {
		paint_util_push_tunnel_left(height, type);
	}
}
