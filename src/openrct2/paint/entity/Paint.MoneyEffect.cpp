/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.MoneyEffect.h"

#include "../../OpenRCT2.h"
#include "../../config/Config.h"
#include "../../entity/MoneyEffect.h"
#include "../../profiling/Profiling.h"
#include "../Paint.h"

using namespace OpenRCT2;

void PaintMoneyEffect(PaintSession& session, const MoneyEffect& moneyEffect)
{
    PROFILED_FUNCTION();

    if (gLegacyScene == LegacyScene::titleSequence)
    {
        // Don't render any money in the title screen.
        return;
    }

    if (moneyEffect.guestPurchase && !Config::Get().general.showGuestPurchases)
    {
        // Don't show the money effect for guest purchases when the option is disabled.
        return;
    }

    auto& rt = session.rt;
    if (rt.zoom_level > ZoomLevel{ 0 })
    {
        return;
    }

    /** rct2: 0x0097EDA4 */
    static constexpr int8_t kWaveOffset[] = {
        0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1,
        0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1,
    };

    auto [stringId, stringValue] = moneyEffect.GetStringId();
    PaintFloatingMoneyEffect(
        session, stringValue, stringId, moneyEffect.y, moneyEffect.z,
        const_cast<int8_t*>(&kWaveOffset[moneyEffect.wiggle % 22]), moneyEffect.offsetX, session.CurrentRotation);
}
