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

#include "../../world/sprite.h"
#include "../../drawing/drawing.h"
#include "../../interface/viewport.h"
#include "../../paint/sprite/sprite.h"
#include "../paint.h"
#include "../../localisation/string_ids.h"

/** rct2: 0x0097EDA4 */
const sint8 money_wave[] = {
	0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1,
	0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1
};

/** rct2: 0x0097ED90 */
const uint32 vehicle_particle_base_sprites[] = {
	22577, 22589, 22601, 22613, 22625
};

extern const uint8 * DuckAnimations[];

/**
 * rct2: 0x00672AC9
 */
void misc_paint(rct_sprite *misc, sint32 imageDirection)
{
	rct_drawpixelinfo * dpi = unk_140E9A8;

	switch (misc->steam_particle.misc_identifier) {
		case SPRITE_MISC_STEAM_PARTICLE: // 0
		{
			uint32 imageId = 22637 + (misc->steam_particle.frame / 256);
			sub_98196C(imageId, 0, 0, 1, 1, 0, misc->unknown.z, get_current_rotation());
			break;
		}

		case SPRITE_MISC_MONEY_EFFECT: // 1
		{
			if (dpi->zoom_level != 0) {
				return;
			}

			rct_money_effect * moneyEffect = &misc->money_effect;
			rct_string_id stringId = money_effect_get_string_id(moneyEffect);
			paint_floating_money_effect(moneyEffect->value, stringId, moneyEffect->y, moneyEffect->z, (sint8 *) &money_wave[moneyEffect->wiggle % 22], moneyEffect->offset_x, get_current_rotation());
			break;
		}

		case SPRITE_MISC_CRASHED_VEHICLE_PARTICLE: // 2
		{
			if (dpi->zoom_level != 0) {
				return;
			}

			rct_crashed_vehicle_particle particle = misc->crashed_vehicle_particle;
			uint32 imageId = vehicle_particle_base_sprites[particle.var_2E] + particle.frame / 256;
			imageId = imageId | (particle.colour[0] << 19) | (particle.colour[1] << 24) | 0xA0000000;
			sub_98196C(imageId, 0, 0, 1, 1, 0, misc->unknown.z, get_current_rotation());
			break;
		}

		case SPRITE_MISC_EXPLOSION_CLOUD: // 3
		{
			uint32 imageId = 22878 + (misc->unknown.frame / 256);
			sub_98196C(imageId, 0, 0, 1, 1, 0, misc->unknown.z, get_current_rotation());
			break;
		}

		case SPRITE_MISC_CRASH_SPLASH: // 4
		{
			rct_crash_splash crashSplash = misc->crash_splash;
			uint32 imageId = 22927 + (crashSplash.frame / 256);
			sub_98196C(imageId, 0, 0, 1, 1, 0, crashSplash.z, get_current_rotation());
			break;
		}

		case SPRITE_MISC_EXPLOSION_FLARE: // 5
		{
			// Like a flare
			uint32 imageId = 22896 + (misc->unknown.frame / 256);
			sub_98196C(imageId, 0, 0, 1, 1, 0, misc->unknown.z, get_current_rotation());
			break;
		}

		case SPRITE_MISC_JUMPING_FOUNTAIN_WATER: // 6
		case SPRITE_MISC_JUMPING_FOUNTAIN_SNOW: // 9
		{
			if (dpi->zoom_level != 0) {
				return;
			}

			rct_jumping_fountain jumpingFountain = misc->jumping_fountain;

			uint16 height = jumpingFountain.z + 6;
			sint32 ebx = imageDirection / 8;

			uint8 al = (jumpingFountain.fountain_flags / 128) & 1;
			uint8 ah = (jumpingFountain.sprite_direction / 16) & 1;

			if (al == ah) {
				al = ebx / 2;
			} else {
				al = ebx / 2;
				al = al ^ 1;
			}

			uint32 baseImageId = (jumpingFountain.misc_identifier == SPRITE_MISC_JUMPING_FOUNTAIN_SNOW) ? 23037 : 22973;
			uint32 imageId = baseImageId + ebx * 16 + jumpingFountain.var_26b;
			if (al & 1) {
				switch (ebx) {
					case 0:
						sub_98197C(imageId, 0, 0, 32, 1, 3, height, -32, -3, height, get_current_rotation());
						break;

					case 1:
						sub_98197C(imageId, 0, 0, 1, 32, 3, height, -3, 0, height, get_current_rotation());
						break;

					case 2:
						sub_98197C(imageId, 0, 0, 32, 1, 3, height, 0, -3, height, get_current_rotation());
						break;

					case 3:
						sub_98197C(imageId, 0, 0, 1, 32, 3, height, -3, -32, height, get_current_rotation());
						break;
				}
			} else {
				switch (ebx) {
					case 0:
						sub_98197C(imageId, 0, 0, 32, 1, 3, height, -32, 3, height, get_current_rotation());
						break;

					case 1:
						sub_98197C(imageId, 0, 0, 1, 32, 3, height, 3, 0, height, get_current_rotation());
						break;

					case 2:
						sub_98197C(imageId, 0, 0, 32, 1, 3, height, 0, 3, height, get_current_rotation());
						break;

					case 3:
						sub_98197C(imageId, 0, 0, 1, 32, 3, height, 3, -32, height, get_current_rotation());
						break;
				}
			}
			break;
		}

		case SPRITE_MISC_BALLOON: // 7
		{
			rct_balloon balloon = misc->balloon;

			uint32 imageId = 22651 + ((balloon.frame / 256) & 7);
			if (balloon.popped != 0) {
				imageId += 8;
			}

			imageId = imageId | (balloon.colour << 19) | 0x20000000;
			sub_98196C(imageId, 0, 0, 1, 1, 0, balloon.z, get_current_rotation());
			break;
		}

		case SPRITE_MISC_DUCK:
			if (dpi->zoom_level == 0) {
				rct_duck * duck = &misc->duck;
				uint32 imageId = duck_get_frame_image(&misc->duck, imageDirection);
				if (imageId != 0) {
					sub_98196C(imageId, 0, 0, 1, 1, 0, duck->z, get_current_rotation());
				}
			}
			break;
	}
}
