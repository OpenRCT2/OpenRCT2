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

template<> void PaintEntity(paint_session* session, const SteamParticle* particle, int32_t imageDirection)
{
    uint32_t imageId = 22637 + (particle->frame / 256);
    PaintAddImageAsParent(session, imageId, 0, 0, 1, 1, 0, particle->z);
}

template<> void PaintEntity(paint_session* session, const MoneyEffect* moneyEffect, int32_t imageDirection)
{
    rct_drawpixelinfo* dpi = &session->DPI;

    if (dpi->zoom_level > 0)
    {
        return;
    }
    if (moneyEffect == nullptr)
        return;
    auto [stringId, value] = moneyEffect->GetStringId();
    PaintFloatingMoneyEffect(
        session, value, stringId, moneyEffect->y, moneyEffect->z, const_cast<int8_t*>(&money_wave[moneyEffect->Wiggle % 22]),
        moneyEffect->OffsetX, session->CurrentRotation);
}

template<> void PaintEntity(paint_session* session, const VehicleCrashParticle* particle, int32_t imageDirection)
{
    rct_drawpixelinfo* dpi = &session->DPI;
    if (dpi->zoom_level > 0)
    {
        return;
    }

    if (particle == nullptr)
        return;
    uint32_t imageId = vehicle_particle_base_sprites[particle->crashed_sprite_base] + particle->frame / 256;
    imageId = imageId | (particle->colour[0] << 19) | (particle->colour[1] << 24) | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS;
    PaintAddImageAsParent(session, imageId, 0, 0, 1, 1, 0, particle->z);
}

template<> void PaintEntity(paint_session* session, const ExplosionCloud* particle, int32_t imageDirection)
{
    if (particle == nullptr)
        return;
    uint32_t imageId = 22878 + (particle->frame / 256);
    PaintAddImageAsParent(session, imageId, 0, 0, 1, 1, 0, particle->z);
}

template<> void PaintEntity(paint_session* session, const CrashSplashParticle* crashSplash, int32_t imageDirection)
{
    if (crashSplash == nullptr)
        return;
    uint32_t imageId = 22927 + (crashSplash->frame / 256);
    PaintAddImageAsParent(session, imageId, 0, 0, 1, 1, 0, crashSplash->z);
}

template<> void PaintEntity(paint_session* session, const ExplosionFlare* flare, int32_t imageDirection)
{
    // Like a flare
    if (flare == nullptr)
        return;
    uint32_t imageId = 22896 + (flare->frame / 256);
    PaintAddImageAsParent(session, imageId, 0, 0, 1, 1, 0, flare->z);
}

constexpr uint32_t JumpingFountainSnowBaseImage = 23037;
constexpr uint32_t JumpingFountainWaterBaseImage = 22973;

template<> void PaintEntity(paint_session* session, const JumpingFountain* jumpingFountain, int32_t imageDirection)
{
    rct_drawpixelinfo* dpi = &session->DPI;
    if (dpi->zoom_level > 0)
    {
        return;
    }

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

    uint32_t baseImageId = (jumpingFountain->FountainType == JumpingFountainType::Snow) ? JumpingFountainSnowBaseImage
                                                                                        : JumpingFountainWaterBaseImage;
    uint32_t imageId = baseImageId + ebx * 16 + jumpingFountain->frame;
    constexpr std::array<CoordsXY, 2> antiClockWiseBoundingBoxes = { CoordsXY{ -COORDS_XY_STEP, -3 }, CoordsXY{ 0, -3 } };
    constexpr std::array<CoordsXY, 2> clockWiseBoundingBoxes = { CoordsXY{ -COORDS_XY_STEP, 3 }, CoordsXY{ 0, 3 } };

    auto bb = isAntiClockwise ? antiClockWiseBoundingBoxes : clockWiseBoundingBoxes;

    PaintAddImageAsParentRotated(session, ebx, imageId, 0, 0, 32, 1, 3, height, bb[ebx & 1].x, bb[ebx & 1].y, height);
}

template<> void PaintEntity(paint_session* session, const Balloon* balloon, int32_t imageDirection)
{
    if (balloon == nullptr)
        return;
    uint32_t imageId = 22651 + (balloon->frame & 7);
    if (balloon->popped != 0)
    {
        imageId += 8;
    }

    imageId = imageId | (balloon->colour << 19) | IMAGE_TYPE_REMAP;
    PaintAddImageAsParent(session, imageId, 0, 0, 1, 1, 0, balloon->z);
}

template<> void PaintEntity(paint_session* session, const Duck* duck, int32_t imageDirection)
{
    rct_drawpixelinfo* dpi = &session->DPI;
    if (dpi->zoom_level <= 1)
    {
        if (duck == nullptr)
            return;
        uint32_t imageId = duck->GetFrameImage(imageDirection);
        if (imageId != 0)
        {
            PaintAddImageAsParent(session, imageId, 0, 0, 1, 1, 0, duck->z);
        }
    }
}
