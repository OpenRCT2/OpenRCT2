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
