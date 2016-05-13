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
#include "../../game.h"

// 0x0099279E:
static const vehicle_boundbox _riverRapidsBoundbox[] = {
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 },
	{ -13, -13,  1, 26, 26, 13 }
};

/**
 *
 *  rct2: 0x006D5889
 */
void vehicle_visual_river_rapids(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	int image_id;
	int baseImage_id = imageDirection;
	uint32 rotation = get_current_rotation();
	int ecx = ((vehicle->var_BA / 8) + (rotation * 8)) & 31;
	int j = 0;
	if (vehicle->vehicle_sprite_type == 0) {
		baseImage_id = ecx & 7;
	} else {
		if (vehicle->vehicle_sprite_type == 1 || vehicle->vehicle_sprite_type == 5) {
			if (vehicle->vehicle_sprite_type == 5){
				baseImage_id = imageDirection ^ 16;
			}
			baseImage_id &= 24;
			j = (baseImage_id / 8) + 1;
			baseImage_id += (ecx & 7);
			baseImage_id += 8;
		} else
		if (vehicle->vehicle_sprite_type == 2 || vehicle->vehicle_sprite_type == 6) {
			if (vehicle->vehicle_sprite_type == 6){
				baseImage_id = imageDirection ^ 16;
			}
			baseImage_id &= 24;
			j = (baseImage_id / 8) + 5;
			baseImage_id += (ecx & 7);
			baseImage_id += 40;
		} else {
			baseImage_id = ecx & 7;
		}
	}
	baseImage_id += vehicleEntry->base_image_id;

	const vehicle_boundbox *bb = &_riverRapidsBoundbox[j];
	image_id = baseImage_id | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0xA0000000;
	sub_98197C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);

	if (RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*)->zoom_level < 2 && vehicle->num_peeps > 0) {
		// Draw peeps: (this particular vehicle doesn't sort them back to front like others so the back ones sometimes clip, but thats how the original does it...)
		int peeps = ((ecx / 8) + 0) & 3;
		image_id = (baseImage_id + ((peeps + 1) * 72)) | (vehicle->peep_tshirt_colours[0] << 19) | (vehicle->peep_tshirt_colours[1] << 24) | 0xA0000000;
		sub_98199C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
		if (vehicle->num_peeps > 2) {
			peeps = ((ecx / 8) + 2) & 3;
			image_id = (baseImage_id + ((peeps + 1) * 72)) | (vehicle->peep_tshirt_colours[2] << 19) | (vehicle->peep_tshirt_colours[3] << 24) | 0xA0000000;
			sub_98199C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
		}
		if (vehicle->num_peeps > 4) {
			peeps = ((ecx / 8) + 1) & 3;
			image_id = (baseImage_id + ((peeps + 1) * 72)) | (vehicle->peep_tshirt_colours[4] << 19) | (vehicle->peep_tshirt_colours[5] << 24) | 0xA0000000;
			sub_98199C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
		}
		if (vehicle->num_peeps > 6) {
			peeps = ((ecx / 8) + 3) & 3;
			image_id = (baseImage_id + ((peeps + 1) * 72)) | (vehicle->peep_tshirt_colours[6] << 19) | (vehicle->peep_tshirt_colours[7] << 24) | 0xA0000000;
			sub_98199C(image_id, 0, 0, bb->length_x, bb->length_y, bb->length_z, z, bb->offset_x, bb->offset_y, bb->offset_z + z, rotation);
		}
	}

	vehicle_visual_splash_effect(z, vehicle, vehicleEntry);
}
