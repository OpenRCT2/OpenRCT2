/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../world/Sprite.h"
#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "Paint.Sprite.h"
#include "../Paint.h"
#include "../../localisation/StringIds.h"

/** rct2: 0x0097EDA4 */
static constexpr const int8_t money_wave[] = {
    0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1,
    0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1
};

/** rct2: 0x0097ED90 */
const uint32_t vehicle_particle_base_sprites[] = {
    22577, 22589, 22601, 22613, 22625
};

/**
 * rct2: 0x00672AC9
 */
void misc_paint(paint_session * session, const rct_sprite *misc, int32_t imageDirection)
{
    rct_drawpixelinfo * dpi = session->DPI;

    switch (misc->steam_particle.misc_identifier) {
        case SPRITE_MISC_STEAM_PARTICLE: // 0
        {
            uint32_t imageId = 22637 + (misc->steam_particle.frame / 256);
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, misc->unknown.z);
            break;
        }

        case SPRITE_MISC_MONEY_EFFECT: // 1
        {
            if (dpi->zoom_level != 0) {
                return;
            }

            const rct_money_effect * moneyEffect = &misc->money_effect;
            money32 value;
            rct_string_id stringId = money_effect_get_string_id(moneyEffect, &value);
            paint_floating_money_effect(
                session, value, stringId, moneyEffect->y, moneyEffect->z, (int8_t *)&money_wave[moneyEffect->wiggle % 22],
                moneyEffect->offset_x, session->CurrentRotation);
            break;
        }

        case SPRITE_MISC_CRASHED_VEHICLE_PARTICLE: // 2
        {
            if (dpi->zoom_level != 0) {
                return;
            }

            rct_crashed_vehicle_particle particle = misc->crashed_vehicle_particle;
            uint32_t imageId = vehicle_particle_base_sprites[particle.crashed_sprite_base] + particle.frame / 256;
            imageId = imageId | (particle.colour[0] << 19) | (particle.colour[1] << 24) | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS;
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, misc->unknown.z);
            break;
        }

        case SPRITE_MISC_EXPLOSION_CLOUD: // 3
        {
            uint32_t imageId = 22878 + (misc->unknown.frame / 256);
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, misc->unknown.z);
            break;
        }

        case SPRITE_MISC_CRASH_SPLASH: // 4
        {
            rct_crash_splash crashSplash = misc->crash_splash;
            uint32_t imageId = 22927 + (crashSplash.frame / 256);
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, crashSplash.z);
            break;
        }

        case SPRITE_MISC_EXPLOSION_FLARE: // 5
        {
            // Like a flare
            uint32_t imageId = 22896 + (misc->unknown.frame / 256);
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, misc->unknown.z);
            break;
        }

        case SPRITE_MISC_JUMPING_FOUNTAIN_WATER: // 6
        case SPRITE_MISC_JUMPING_FOUNTAIN_SNOW: // 9
        {
            if (dpi->zoom_level != 0) {
                return;
            }

            rct_jumping_fountain jumpingFountain = misc->jumping_fountain;

            uint16_t height = jumpingFountain.z + 6;
            int32_t ebx = imageDirection / 8;

            uint8_t al = (jumpingFountain.fountain_flags / 128) & 1;
            uint8_t ah = (jumpingFountain.sprite_direction / 16) & 1;

            if (al == ah) {
                al = ebx / 2;
            } else {
                al = ebx / 2;
                al = al ^ 1;
            }

            uint32_t baseImageId = (jumpingFountain.misc_identifier == SPRITE_MISC_JUMPING_FOUNTAIN_SNOW) ? 23037 : 22973;
            uint32_t imageId = baseImageId + ebx * 16 + jumpingFountain.frame;
            if (al & 1) {
                switch (ebx) {
                    case 0:
                        sub_98197C(session, imageId, 0, 0, 32, 1, 3, height, -32, -3, height);
                        break;

                    case 1:
                        sub_98197C(session, imageId, 0, 0, 1, 32, 3, height, -3, 0, height);
                        break;

                    case 2:
                        sub_98197C(session, imageId, 0, 0, 32, 1, 3, height, 0, -3, height);
                        break;

                    case 3:
                        sub_98197C(session, imageId, 0, 0, 1, 32, 3, height, -3, -32, height);
                        break;
                }
            } else {
                switch (ebx) {
                    case 0:
                        sub_98197C(session, imageId, 0, 0, 32, 1, 3, height, -32, 3, height);
                        break;

                    case 1:
                        sub_98197C(session, imageId, 0, 0, 1, 32, 3, height, 3, 0, height);
                        break;

                    case 2:
                        sub_98197C(session, imageId, 0, 0, 32, 1, 3, height, 0, 3, height);
                        break;

                    case 3:
                        sub_98197C(session, imageId, 0, 0, 1, 32, 3, height, 3, -32, height);
                        break;
                }
            }
            break;
        }

        case SPRITE_MISC_BALLOON: // 7
        {
            rct_balloon balloon = misc->balloon;

            uint32_t imageId = 22651 + (balloon.frame & 7);
            if (balloon.popped != 0) {
                imageId += 8;
            }

            imageId = imageId | (balloon.colour << 19) | IMAGE_TYPE_REMAP;
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, balloon.z);
            break;
        }

        case SPRITE_MISC_DUCK:
            if (dpi->zoom_level == 0) {
                const rct_duck * duck = &misc->duck;
                uint32_t imageId = duck_get_frame_image(&misc->duck, imageDirection);
                if (imageId != 0) {
                    sub_98196C(session, imageId, 0, 0, 1, 1, 0, duck->z);
                }
            }
            break;
    }
}
