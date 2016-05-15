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
 *  rct2: 0x006D5DA9
 */
void vehicle_visual_roto_drop(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	int image_id;
	int baseImage_id = (vehicleEntry->base_image_id + 4) + ((vehicle->var_C5 / 4) & 0x3);
	if (vehicle->restraints_position >= 64) {
		baseImage_id += 7;
		baseImage_id += (vehicle->restraints_position / 64);
	}

	const uint8 rotation = get_current_rotation();
	// Draw back:
	image_id = baseImage_id | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0xA0000000;
	sub_98197C(image_id, 0, 0, 2, 2, 41, z, -11, -11, z + 1, rotation);

	// Draw front:
	image_id = (baseImage_id + 4) | (vehicle->colours.body_colour << 19) | (vehicle->colours.trim_colour << 24) | 0xA0000000;
	sub_98197C(image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1, rotation);

	uint8 riding_peep_sprites[64];
	memset(riding_peep_sprites, 0xFF, sizeof(riding_peep_sprites));
	for (int i = 0; i < vehicle->num_peeps; i++) {
		uint8 cl = (i & 3) * 16;
		cl += (i & 0xFC);
		cl += vehicle->var_C5 / 4;
		cl += (imageDirection / 8) * 16;
		cl &= 0x3F;
		riding_peep_sprites[cl] = vehicle->peep_tshirt_colours[i];
	}

	// Draw riding peep sprites in back to front order:
	for (int j = 0; j <= 48; j++) {
		int i = (j % 2) ? (48 - (j / 2)) : (j / 2);
		if (riding_peep_sprites[i] != 0xFF) {
			baseImage_id = vehicleEntry->base_image_id + 20 + i;
			if (vehicle->restraints_position >= 64) {
				baseImage_id += 64;
				baseImage_id += vehicle->restraints_position / 64;
			}
			image_id = baseImage_id | (riding_peep_sprites[i] << 19) | 0x20000000;
			sub_98199C(image_id, 0, 0, 16, 16, 41, z, -5, -5, z + 1, rotation);
		}
	};

	assert(vehicleEntry->effect_visual == 1);
	// Although called in original code, effect_visual (splash effects) are not used for many rides and does not make sense so it was taken out
}
