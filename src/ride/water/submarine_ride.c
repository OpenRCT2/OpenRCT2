#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../../addresses.h"
#include "../../config.h"
#include "../../interface/viewport.h"
#include "../../world/sprite.h"
#include "../../paint/paint.h"
#include "../vehicle_paint.h"

/**
 *
 *  rct2: 0x006D44D5
 */
void vehicle_visual_submarine(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	int baseImage_id = imageDirection;
	int image_id;
	if (vehicle->restraints_position >= 64) {
		if ((vehicleEntry->sprite_flags & 0x2000) && !(imageDirection & 3)) {
			baseImage_id /= 8;
			baseImage_id += ((vehicle->restraints_position - 64) / 64) * 4;
			baseImage_id *= vehicleEntry->var_16;
			baseImage_id += vehicleEntry->var_1C;
		}
	} else {
		if (vehicleEntry->flags_a & 0x800) {
			baseImage_id /= 2;
		}
		if (vehicleEntry->sprite_flags & 0x8000) {
			baseImage_id /= 8;
		}
		baseImage_id *= vehicleEntry->var_16;
		baseImage_id += vehicleEntry->base_image_id;
		baseImage_id += vehicle->var_4A;
	}

	vehicle_boundbox bb = VehicleBoundboxes[vehicleEntry->draw_order][imageDirection / 2];

	image_id = baseImage_id | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0x80000000;
	if (sub_98197C(image_id, 0, 0, bb.length_x, bb.length_y, bb.length_z, z, bb.offset_x, bb.offset_y, bb.offset_z + z, get_current_rotation())) {
		paint_struct* ps = RCT2_GLOBAL(0xEE7888, paint_struct*) - 1; // sub_98197C increments this but need original
		ps->tertiary_colour = vehicle->colours_extended;
	}

	image_id = (baseImage_id + 1) | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0x80000000;
	if (sub_98197C(image_id, 0, 0, bb.length_x, bb.length_y, 2, z, bb.offset_x, bb.offset_y, bb.offset_z + z - 10, get_current_rotation())) {
		paint_struct* ps = RCT2_GLOBAL(0xEE7888, paint_struct*) - 1; // sub_98197C increments this but need original
		ps->tertiary_colour = vehicle->colours_extended;
	}

	assert(vehicleEntry->effect_visual == 1);
}
