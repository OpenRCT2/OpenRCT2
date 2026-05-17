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
    // which matches the granularity of ATM fees (£0.00 .. £20.00 -> 0 .. 200).
    static constexpr int32_t feeInTenPence(money64 fee)
    {
        return static_cast<int32_t>(fee / 0.10_GBP);
    }

    bool guestAcceptsFee(money64 fee, uint8_t happiness, uint32_t randomValue)
    {
        // Free machines are always acceptable.
        if (fee <= 0)
            return true;

        // Probabilistic acceptance. Acceptance probability rises with happiness and
        // falls with fee, so there is always *some* randomness — a maximally happy
        // guest still occasionally refuses the £2 complaint threshold, and even up
        // at the £20 cap a maxed-out guest will sometimes agree to pay. Miserable
        // guests effectively never accept a non-trivial fee.
        //
        // pScaled is the accept probability in 0..1024 units. We compare it to a
        // uniformly distributed 10-bit random sample.
        //
        // Construction:
        //   happiness contribution: happiness * 4   (0..1020, max happiness → ~full)
        //   fee penalty:            feeUnits * 2    (0..400 over £0..£20)
        // Worked examples (probability of accepting):
        //   h=255, fee=£2  (20 units):  (1020 - 40)  / 1024 ≈ 0.96
        //   h=255, fee=£20 (200 units): (1020 - 400) / 1024 ≈ 0.61
        //   h=128, fee=£2:              (512  - 40)  / 1024 ≈ 0.46
        //   h=128, fee=£20:             (512  - 400) / 1024 ≈ 0.11
        //   h=0,   any fee:             0            / 1024  = 0.00
        const int32_t feeUnits = feeInTenPence(fee);
        const int32_t pScaled = std::max(0, static_cast<int32_t>(happiness) * 4 - feeUnits * 2);
        const uint32_t roll = randomValue % 1024u;

        return roll < static_cast<uint32_t>(pScaled);
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
