/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../entity/Balloon.h"
#include "../../entity/Duck.h"
#include "../../entity/Fountain.h"
#include "../../entity/MoneyEffect.h"
#include "../../entity/Particle.h"
#include "../../interface/Viewport.h"
#include "../../localisation/StringIds.h"
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
    PaintAddImageAsParent(session, imageId, { 0, 0, particle->z }, { 1, 1, 0 });
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
    PaintAddImageAsParent(session, imageId, { 0, 0, particle->z }, { 1, 1, 0 });
}

template<> void PaintEntity(paint_session* session, const ExplosionCloud* particle, int32_t imageDirection)
{
    if (particle == nullptr)
        return;
    uint32_t imageId = 22878 + (particle->frame / 256);
    PaintAddImageAsParent(session, imageId, { 0, 0, particle->z }, { 1, 1, 0 });
}
