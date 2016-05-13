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
 *  rct2: 0x006D5FAB
 */
void vehicle_visual_launched_freefall(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	int image_id;
	int baseImage_id = vehicleEntry->base_image_id + ((vehicle->restraints_position / 64) * 2);

	const uint8 rotation = get_current_rotation();
	// Draw back:
	image_id = (baseImage_id + 2) | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0xA0000000;
	sub_98197C(image_id, 0, 0, 2, 2, 41, z, -11, -11, z + 1, rotation);

	// Draw front:
	image_id = (baseImage_id + 1) | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0xA0000000;
	sub_98197C(image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1, rotation);

	// Draw peeps:
	if (RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*)->zoom_level < 2) {
		if (vehicle->num_peeps > 0) {
			baseImage_id = vehicleEntry->base_image_id + 9;
			if ((vehicle->restraints_position / 64) == 3) {
				baseImage_id += 2; // Draw peeps sitting without transparent area between them for restraints
			}
			image_id = (baseImage_id + ((((imageDirection / 8) + 0) & 3) * 3)) | (vehicle->peep_tshirt_colours[0] << 19) | (vehicle->peep_tshirt_colours[1] << 24) | 0xA0000000;
			sub_98199C(image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1, rotation);
			if (vehicle->num_peeps > 2) {
				image_id = (baseImage_id + ((((imageDirection / 8) + 1) & 3) * 3)) | (vehicle->peep_tshirt_colours[2] << 19) | (vehicle->peep_tshirt_colours[3] << 24) | 0xA0000000;
				sub_98199C(image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1, rotation);
			}
			if (vehicle->num_peeps > 4) {
				image_id = (baseImage_id + ((((imageDirection / 8) + 2) & 3) * 3)) | (vehicle->peep_tshirt_colours[4] << 19) | (vehicle->peep_tshirt_colours[5] << 24) | 0xA0000000;
				sub_98199C(image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1, rotation);
			}
			if (vehicle->num_peeps > 6) {
				image_id = (baseImage_id + ((((imageDirection / 8) + 3) & 3) * 3)) | (vehicle->peep_tshirt_colours[6] << 19) | (vehicle->peep_tshirt_colours[7] << 24) | 0xA0000000;
				sub_98199C(image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1, rotation);
			}
		}
	}

	assert(vehicleEntry->effect_visual == 1);
}
