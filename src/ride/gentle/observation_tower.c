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

/**
 *
 *  rct2: 0x006D6258
 */
void vehicle_visual_observation_tower(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	int image_id;
	int baseImage_id = (vehicle->restraints_position / 64);
	if (vehicle->restraints_position >= 64) {
		if ((imageDirection / 8) && (imageDirection / 8) != 3) {
			baseImage_id *= 2;
			baseImage_id += vehicleEntry->base_image_id + 28;
			if ((imageDirection / 8) != 1) {
				baseImage_id -= 6;
			}
		} else {
			baseImage_id = vehicleEntry->base_image_id + 8;
		}
	} else {
		baseImage_id = (vehicle->var_C5 * 2) + vehicleEntry->base_image_id + 8;
	}

	image_id = baseImage_id | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0x80000000;
	paint_struct* ps = sub_98197C(image_id, 0, 0, 2, 2, 41, z, -11, -11, z + 1, get_current_rotation());
	if (ps != NULL) {
		ps->tertiary_colour = vehicle->colours_extended;
	}

	image_id++;

	ps = sub_98197C(image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1, get_current_rotation());
	if (ps != NULL) {
		ps->tertiary_colour = vehicle->colours_extended;
	}

	assert(vehicleEntry->effect_visual == 1);
}
