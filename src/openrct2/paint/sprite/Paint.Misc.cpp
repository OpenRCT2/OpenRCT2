/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../localisation/StringIds.h"
#include "../../world/Fountain.h"
#include "../../world/Sprite.h"
#include "../Paint.h"
#include "Paint.Sprite.h"

/** rct2: 0x0097EDA4 */
static constexpr const int8_t money_wave[] = {
    0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1,
    0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1,
};

/** rct2: 0x0097ED90 */
const uint32_t vehicle_particle_base_sprites[] = {
    22577, 22589, 22601, 22613, 22625,
};

/**
 * rct2: 0x00672AC9
 */
void misc_paint(paint_session* session, const SpriteBase* misc, int32_t imageDirection)
{
    rct_drawpixelinfo* dpi = &session->DPI;

    switch (misc->type)
    {
        case SPRITE_MISC_STEAM_PARTICLE: // 0
        {
            auto particle = misc->As<SteamParticle>();
            if (particle == nullptr)
                return;
            uint32_t imageId = 22637 + (particle->frame / 256);
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, particle->z);
            break;
        }

        case SPRITE_MISC_MONEY_EFFECT: // 1
        {
            if (dpi->zoom_level > 0)
            {
                return;
            }
            auto moneyEffect = misc->As<MoneyEffect>();
            if (moneyEffect == nullptr)
                return;
            auto [stringId, value] = moneyEffect->GetStringId();
            paint_floating_money_effect(
                session, value, stringId, moneyEffect->y, moneyEffect->z,
                const_cast<int8_t*>(&money_wave[moneyEffect->Wiggle % 22]), moneyEffect->OffsetX, session->CurrentRotation);
            break;
        }

        case SPRITE_MISC_CRASHED_VEHICLE_PARTICLE: // 2
        {
            if (dpi->zoom_level > 0)
            {
                return;
            }
            auto particle = misc->As<VehicleCrashParticle>();
            if (particle == nullptr)
                return;
            uint32_t imageId = vehicle_particle_base_sprites[particle->crashed_sprite_base] + particle->frame / 256;
            imageId = imageId | (particle->colour[0] << 19) | (particle->colour[1] << 24) | IMAGE_TYPE_REMAP
                | IMAGE_TYPE_REMAP_2_PLUS;
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, particle->z);
            break;
        }

        case SPRITE_MISC_EXPLOSION_CLOUD: // 3
        {
            auto particle = misc->As<ExplosionCloud>();
            if (particle == nullptr)
                return;
            uint32_t imageId = 22878 + (particle->frame / 256);
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, particle->z);
            break;
        }

        case SPRITE_MISC_CRASH_SPLASH: // 4
        {
            auto crashSplash = misc->As<CrashSplashParticle>();
            if (crashSplash == nullptr)
                return;
            uint32_t imageId = 22927 + (crashSplash->frame / 256);
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, crashSplash->z);
            break;
        }

        case SPRITE_MISC_EXPLOSION_FLARE: // 5
        {
            // Like a flare
            auto flare = misc->As<ExplosionFlare>();
            if (flare == nullptr)
                return;
            uint32_t imageId = 22896 + (flare->frame / 256);
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, flare->z);
            break;
        }

        case SPRITE_MISC_JUMPING_FOUNTAIN_WATER: // 6
        case SPRITE_MISC_JUMPING_FOUNTAIN_SNOW:  // 9
        {
            if (dpi->zoom_level > 0)
            {
                return;
            }

            auto jumpingFountain = misc->As<JumpingFountain>();
            if (jumpingFountain == nullptr)
                return;
            uint16_t height = jumpingFountain->z + 6;
            int32_t ebx = imageDirection / 8;

            // Fountain is firing anti clockwise
            bool reversed = (jumpingFountain->FountainFlags & FOUNTAIN_FLAG::DIRECTION);
            // Fountain rotation
            bool rotated = (jumpingFountain->sprite_direction / 16) & 1;
            bool isAntiClockwise = (ebx / 2) & 1; // Clockwise or Anti-clockwise

            // These cancel each other out
            if (reversed != rotated)
            {
                isAntiClockwise = !isAntiClockwise;
            }

            uint32_t baseImageId = (jumpingFountain->type == SPRITE_MISC_JUMPING_FOUNTAIN_SNOW) ? 23037 : 22973;
            uint32_t imageId = baseImageId + ebx * 16 + jumpingFountain->frame;
            constexpr std::array<CoordsXY, 2> antiClockWiseBoundingBoxes = { CoordsXY{ -COORDS_XY_STEP, -3 },
                                                                             CoordsXY{ 0, -3 } };
            constexpr std::array<CoordsXY, 2> clockWiseBoundingBoxes = { CoordsXY{ -COORDS_XY_STEP, 3 }, CoordsXY{ 0, 3 } };

            auto bb = isAntiClockwise ? antiClockWiseBoundingBoxes : clockWiseBoundingBoxes;

            sub_98197C_rotated(session, ebx, imageId, 0, 0, 32, 1, 3, height, bb[ebx & 1].x, bb[ebx & 1].y, height);
            break;
        }

        case SPRITE_MISC_BALLOON: // 7
        {
            auto balloon = misc->As<Balloon>();
            if (balloon == nullptr)
                return;
            uint32_t imageId = 22651 + (balloon->frame & 7);
            if (balloon->popped != 0)
            {
                imageId += 8;
            }

            imageId = imageId | (balloon->colour << 19) | IMAGE_TYPE_REMAP;
            sub_98196C(session, imageId, 0, 0, 1, 1, 0, balloon->z);
            break;
        }

        case SPRITE_MISC_DUCK:
            if (dpi->zoom_level <= 1)
            {
                const Duck* duck = misc->As<Duck>();
                if (duck == nullptr)
                    return;
                uint32_t imageId = duck->GetFrameImage(imageDirection);
                if (imageId != 0)
                {
                    sub_98196C(session, imageId, 0, 0, 1, 1, 0, duck->z);
                }
            }
            break;
    }
}
