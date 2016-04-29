#include "paint.h"
#include "../drawing/drawing.h"
#include "../addresses.h"

/**
 *
 *  rct2: 0x0068615B
 */
void painter_setup() {
	RCT2_GLOBAL(0xEE7888, uint32) = 0x00EE788C;
	RCT2_GLOBAL(0xF1AD28, uint32) = 0;
	RCT2_GLOBAL(0xF1AD2C, uint32) = 0;
	uint8* edi = RCT2_ADDRESS(0xF1A50C, uint8);
	memset(edi, 0, 2048);
	RCT2_GLOBAL(0xF1AD0C, sint32) = -1;
	RCT2_GLOBAL(0xF1AD10, uint32) = 0;
	RCT2_GLOBAL(0xF1AD20, uint32) = 0;
	RCT2_GLOBAL(0xF1AD24, uint32) = 0;
}

/**
 *
 *  rct2: 0x006874B0, 0x00687618, 0x0068778C, 0x00687902, 0x0098199C
 *
 * @param image_id (ebx)
 * @param x_offset (al)
 * @param y_offset (cl)
 * @param bound_box_length_x (di)
 * @param bound_box_length_y (si)
 * @param bound_box_length_z (ah)
 * @param z_offset (dx)
 * @param bound_box_offset_x (0x009DEA52)
 * @param bound_box_offset_y (0x009DEA54)
 * @param bound_box_offset_z (0x009DEA56)
 * @param rotation (ebp)
 * @return (!CF) success
 */
bool sub_98199C(
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	uint16 z_offset,
	sint16 bound_box_offset_x, uint16 bound_box_offset_y, sint16 bound_box_offset_z,
	uint32 rotation
) {
	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_X, uint16) = bound_box_offset_x;
	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Y, uint16) = bound_box_offset_y;
	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Z, uint16) = bound_box_offset_z;

	int flags = RCT2_CALLPROC_X(RCT2_ADDRESS(0x98199C, uint32_t)[rotation],
		x_offset | (bound_box_length_z << 8),
		image_id,
		y_offset,
		z_offset,
		bound_box_length_y,
		bound_box_length_x,
		rotation);

	return !(flags & (1 << 8));
}

/**
 *  rct2: 0x006861AC, 0x00686337, 0x006864D0, 0x0068666B, 0x0098196C
 *
 * @param image_id (ebx)
 * @param x_offset (al)
 * @param y_offset (cl)
 * @param bound_box_length_x (di)
 * @param bound_box_length_y (si)
 * @param bound_box_length_z (ah)
 * @param z_offset (dx)
 * @param rotation (ebp)
 * @return (!CF) success
 */
bool sub_98196C(
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	uint16 z_offset,
	uint32 rotation
) {
	int flags = RCT2_CALLPROC_X(RCT2_ADDRESS(0x0098196C, uint32)[rotation],
		x_offset | (bound_box_length_z << 8),
		image_id,
		y_offset,
		z_offset,
		bound_box_length_y,
		bound_box_length_x,
		rotation
	);

	return !(flags & (1 << 8));
}

/**
 *  rct2: 0x00686806, 0x006869B2, 0x00686B6F, 0x00686D31, 0x0098197C
 *
 * @param image_id (ebx)
 * @param x_offset (al)
 * @param y_offset (cl)
 * @param bound_box_length_x (di)
 * @param bound_box_length_y (si)
 * @param bound_box_length_z (ah)
 * @param z_offset (dx)
 * @param bound_box_offset_x (0x009DEA52)
 * @param bound_box_offset_y (0x009DEA54)
 * @param bound_box_offset_z (0x009DEA56)
 * @param rotation (ebp)
 * @return (!CF) success
 */
bool sub_98197C(
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	uint16 z_offset,
	sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
	uint32 rotation
) {

	RCT2_GLOBAL(0xF1AD28, paint_struct*) = 0;
	RCT2_GLOBAL(0xF1AD2C, uint32) = 0;

	//Not a paint struct but something similar
	paint_struct* ps = RCT2_GLOBAL(0xEE7888, paint_struct*);

	if ((uint32)ps >= RCT2_GLOBAL(0xEE7880, uint32))return false;

	ps->image_id = image_id;

	uint32 image_element = image_id & 0x7FFFF;
	rct_g1_element *g1Element = gfx_get_g1_element(image_element);

	rct_xyz16 coord_3d = {
		.x = x_offset,
		.y = y_offset,
		.z = z_offset
	};

	switch (rotation) {
	case 0:
		rotate_map_coordinates(&coord_3d.x, &coord_3d.y, 0);
		break;
	case 1:
		rotate_map_coordinates(&coord_3d.x, &coord_3d.y, 3);
		break;
	case 2:
		rotate_map_coordinates(&coord_3d.x, &coord_3d.y, 2);
		break;
	case 3:
		rotate_map_coordinates(&coord_3d.x, &coord_3d.y, 1);
		break;
	}
	coord_3d.x += RCT2_GLOBAL(0x9DE568, sint16);
	coord_3d.y += RCT2_GLOBAL(0x9DE56C, sint16);

	rct_xy16 map = coordinate_3d_to_2d(&coord_3d, rotation);

	ps->x = map.x;
	ps->y = map.y;

	int left = map.x + g1Element->x_offset;
	int bottom = map.y + g1Element->y_offset;

	int right = left + g1Element->width;
	int top = bottom + g1Element->height;

	RCT2_GLOBAL(0xF1AD1C, uint16) = left;
	RCT2_GLOBAL(0xF1AD1E, uint16) = bottom;

	rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);

	if (right <= dpi->x)return false;
	if (top <= dpi->y)return false;
	if (left > dpi->x + dpi->width)return false;
	if (bottom > dpi->y + dpi->height)return false;

	rct_xy16 boundBox = {
		.x = bound_box_length_x,
		.y = bound_box_length_y
	};

	rct_xy16 boundBoxOffset = {
		.x = bound_box_offset_x,
		.y = bound_box_offset_y
	};

	// Unsure why rots 1 and 3 need to swap
	switch (rotation) {
	case 0:
		boundBox.x--;
		boundBox.y--;
		rotate_map_coordinates(&boundBoxOffset.x, &boundBoxOffset.y, 0);
		rotate_map_coordinates(&boundBox.x, &boundBox.y, 0);
		break;
	case 1:
		boundBox.x--;
		rotate_map_coordinates(&boundBoxOffset.x, &boundBoxOffset.y, 3);
		rotate_map_coordinates(&boundBox.x, &boundBox.y, 3);
		break;
	case 2:
		rotate_map_coordinates(&boundBox.x, &boundBox.y, 2);
		rotate_map_coordinates(&boundBoxOffset.x, &boundBoxOffset.y, 2);
		break;
	case 3:
		boundBox.y--;
		rotate_map_coordinates(&boundBox.x, &boundBox.y, 1);
		rotate_map_coordinates(&boundBoxOffset.x, &boundBoxOffset.y, 1);
		break;
	}

	ps->bound_box_x_end = boundBox.x + boundBoxOffset.x + RCT2_GLOBAL(0x9DE568, sint16);
	ps->bound_box_z = bound_box_offset_z;
	int boundBoxZEnd = bound_box_length_z + bound_box_offset_z;
	ps->bound_box_z_end = boundBoxZEnd;
	ps->bound_box_y_end = boundBox.y + boundBoxOffset.y + RCT2_GLOBAL(0x009DE56C, sint16);
	ps->flags = 0;
	ps->bound_box_x = boundBoxOffset.x + RCT2_GLOBAL(0x9DE568, sint16);
	ps->bound_box_y = boundBoxOffset.y + RCT2_GLOBAL(0x009DE56C, sint16);
	ps->attached_ps = NULL;
	ps->var_20 = NULL;
	ps->sprite_type = RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8);
	ps->var_29 = RCT2_GLOBAL(0x9DE571, uint8);
	ps->map_x = RCT2_GLOBAL(0x9DE574, uint16);
	ps->map_y = RCT2_GLOBAL(0x9DE576, uint16);
	ps->mapElement = RCT2_GLOBAL(0x9DE578, rct_map_element*);

	RCT2_GLOBAL(0xF1AD28, paint_struct*) = ps;

	rct_xy16 attach = {
		.x = ps->bound_box_x,
		.y = ps->bound_box_y
	};

	rotate_map_coordinates(&attach.x, &attach.y, rotation);
	switch (rotation) {
	case 0:
		break;
	case 1:
	case 3:
		attach.x += 0x2000;
		break;
	case 2:
		attach.x += 0x4000;
		break;
	}

	sint16 di = attach.x + attach.y;

	if (di < 0)
		di = 0;

	di /= 32;
	if (di > 511)
		di = 511;

	ps->var_18 = di;
	paint_struct* old_ps = RCT2_ADDRESS(0x00F1A50C, paint_struct*)[di];
	RCT2_ADDRESS(0x00F1A50C, paint_struct*)[di] = ps;
	ps->next_quadrant_ps = old_ps;

	if ((uint16)di < RCT2_GLOBAL(0x00F1AD0C, uint32)) {
		RCT2_GLOBAL(0x00F1AD0C, uint32) = di;
	}

	if ((uint16)di > RCT2_GLOBAL(0x00F1AD10, uint32)) {
		RCT2_GLOBAL(0x00F1AD10, uint32) = di;
	}

	RCT2_GLOBAL(0xEE7888, paint_struct*)++;
	return true;
}

/**
 *
 *  rct2: 0x00686EF0, 0x00687056, 0x006871C8, 0x0068733C, 0x0098198C
 *
 * @param image_id (ebx)
 * @param x_offset (al)
 * @param y_offset (cl)
 * @param bound_box_length_x (di)
 * @param bound_box_length_y (si)
 * @param bound_box_length_z (ah)
 * @param z_offset (dx)
 * @param bound_box_offset_x (0x009DEA52)
 * @param bound_box_offset_y (0x009DEA54)
 * @param bound_box_offset_z (0x009DEA56)
 * @param rotation
 * @return (!CF) success
 */
bool sub_98198C(
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	uint16 z_offset,
	sint16 bound_box_offset_x, uint16 bound_box_offset_y, sint16 bound_box_offset_z,
	uint32 rotation
) {
	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_X, uint16) = bound_box_offset_x;
	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Y, uint16) = bound_box_offset_y;
	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Z, uint16) = bound_box_offset_z;

	int flags = RCT2_CALLPROC_X(RCT2_ADDRESS(0x0098198C, uint32)[rotation],
		x_offset | (bound_box_length_z << 8),
		image_id,
		y_offset,
		z_offset,
		bound_box_length_y,
		bound_box_length_x,
		rotation
	);

	return !(flags & (1 << 8));
}

/**
 * rct2: 0x006881D0
 *
 * @param image_id (ebx)
 * @param x (ax)
 * @param y (cx)
 * @return (!CF) success
 */
bool paint_attach_to_previous_attach(uint32 image_id, uint16 x, uint16 y)
{
    if (RCT2_GLOBAL(0xF1AD2C, uint32) == 0) {
        return paint_attach_to_previous_ps(image_id, x, y);
    }

    attached_paint_struct * ps = RCT2_GLOBAL(0xEE7888, attached_paint_struct *);

    if ((uint32) ps >= RCT2_GLOBAL(0xEE7880, uint32)) {
        return false;
    }

    ps->image_id = image_id;
    ps->x = x;
    ps->y = y;
    ps->flags = 0;

    attached_paint_struct * ebx = RCT2_GLOBAL(0xF1AD2C, attached_paint_struct *);

    ps->next = NULL;
    ebx->next = ps;

    RCT2_GLOBAL(0xF1AD2C, attached_paint_struct *) = ps;

    RCT2_GLOBAL(0xEE7888, attached_paint_struct *)++;

    return true;
}

/**
 * rct2: 0x0068818E
 *
 * @param image_id (ebx)
 * @param x (ax)
 * @param y (cx)
 * @return (!CF) success
 */
bool paint_attach_to_previous_ps(uint32 image_id, uint16 x, uint16 y)
{
    attached_paint_struct * ps = RCT2_GLOBAL(0xEE7888, attached_paint_struct *);

    if ((uint32) ps >= RCT2_GLOBAL(0xEE7880, uint32)) {
        return false;
    }

    ps->image_id = image_id;
    ps->x = x;
    ps->y = y;
    ps->flags = 0;

    paint_struct * masterPs = RCT2_GLOBAL(0xF1AD28, paint_struct *);
    if (masterPs == NULL) {
        return false;
    }

    RCT2_GLOBAL(0x00EE7888, attached_paint_struct *)++;

    attached_paint_struct * oldFirstAttached = masterPs->attached_ps;
	masterPs->attached_ps = ps;

    ps->next = oldFirstAttached;

    RCT2_GLOBAL(0xF1AD2C, attached_paint_struct *) = ps;

    return true;
}

/**
 * rct2: 0x00685EBC, 0x00686046, 0x00685FC8, 0x00685F4A, 0x00685ECC
 * @param amount (eax)
 * @param string_id (bx)
 * @param y (cx)
 * @param z (dx)
 * @param offset_x (si)
 * @param y_offsets (di)
 * @param rotation (ebp)
 */
void sub_685EBC(money32 amount, uint16 string_id, sint16 y, sint16 z, sint8 y_offsets[], sint16 offset_x, uint32 rotation)
{
	paint_string_struct * ps = RCT2_GLOBAL(0xEE7888, paint_string_struct*);

	if ((uint32) ps >= RCT2_GLOBAL(0xEE7880, uint32)) {
		return;
	}

	ps->string_id = string_id;
	ps->next = 0;
	ps->args[0] = amount;
	ps->args[4] = y;
	ps->args[8] = y_offsets;
	ps->args[12] = 0;
	ps->y_offsets = y_offsets;

	rct_xyz16 position = {.x = RCT2_GLOBAL(0x9DE568, sint16), .y = RCT2_GLOBAL(0x9DE56C, sint16), .z = z};
	rct_xy16 coord = coordinate_3d_to_2d(&position, rotation);

	ps->x = coord.x + offset_x;
	ps->y = coord.y;

	RCT2_GLOBAL(0xEE7888, uint32) += 0x1E;

	paint_string_struct * oldPs = RCT2_GLOBAL(0xF1AD24, paint_string_struct*);

	RCT2_GLOBAL(0xF1AD24, paint_string_struct*) = ps;

	if (oldPs == 0) { // 0 or NULL?
		RCT2_GLOBAL(0xF1AD20, paint_string_struct *) = ps;
	} else {
		oldPs->next = ps;
	}
}
