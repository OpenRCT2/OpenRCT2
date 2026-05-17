/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CashMachine.h"

#include <algorithm>

namespace OpenRCT2::CashMachine
{
    // Convert a money64 amount expressed in £ to a small integer in 10p units,
    // which matches the granularity of ATM fees (£0.00 .. £2.00 -> 0 .. 20).
    static constexpr int32_t feeInTenPence(money64 fee)
    {
        return static_cast<int32_t>(fee / 0.10_GBP);
    }

    bool guestAcceptsFee(money64 fee, uint8_t happiness, uint32_t randomValue)
    {
        // Free machines are always acceptable.
        if (fee <= 0)
            return true;

        // Tolerance scales with happiness: a maximally happy guest (255) tolerates
        // roughly the full £2 cap, while a miserable guest (~50) will balk at any
        // non-trivial fee.
        const int32_t feeUnits = feeInTenPence(fee);
        const int32_t happinessTolerance = happiness / 12; // 0..21
        const int32_t randomBonus = static_cast<int32_t>(randomValue & 0x07); // 0..7
        const int32_t tolerance = happinessTolerance + randomBonus;

        return feeUnits <= tolerance;
    }

    int32_t computeFeeHappinessPenalty(money64 fee, uint32_t randomValue)
    {
        if (fee <= 0)
            return 0;

        // Base penalty of 4 happiness for the inconvenience, plus 2 per 10p of fee,
        // plus a small random jitter so identical fees don't always produce identical
        // reactions. £2.00 fee -> ~44 base + 0..7 jitter.
        const int32_t feeUnits = feeInTenPence(fee);
        const int32_t base = 4 + feeUnits * 2;
        const int32_t jitter = static_cast<int32_t>(randomValue & 0x07);
        // Clamp so we never produce a negative penalty (defensive) or absurdly high one.
        return std::clamp(base + jitter, 1, 64);
    }
} // namespace OpenRCT2::CashMachine
