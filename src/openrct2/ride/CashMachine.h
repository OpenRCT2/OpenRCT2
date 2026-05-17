/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"

#include <cstdint>

namespace OpenRCT2::CashMachine
{
    /**
     * Maximum fee a park owner can charge a guest for using a cash machine.
     * Matches kRideMaxPrice so the spinner has the same upper bound as other
     * priced rides; in practice guests refuse anything above
     * kComplaintFeeThreshold, so very high fees mostly serve as a sandbox knob.
     */
    constexpr money64 kMaxFee = 20.00_GBP;

    /**
     * Fee at which guests begin to refuse the cash machine and emit a complaint
     * thought. Any fee strictly greater than this is universally refused by
     * guestAcceptsFee(); at or below it, refusal/tolerance scales with happiness.
     */
    constexpr money64 kComplaintFeeThreshold = 2.00_GBP;

    /**
     * Returns true if a guest with the given happiness is willing to pay the
     * given fee at a cash machine. Happier guests tolerate higher fees.
     *
     * The randomValue parameter introduces a small amount of variation; callers
     * should supply ScenarioRand() output. Tests can pass deterministic values.
     */
    bool guestAcceptsFee(money64 fee, uint8_t happiness, uint32_t randomValue);

    /**
     * Returns the happiness reduction a guest suffers from paying a cash
     * machine fee. The penalty grows with the fee, has a small random
     * component, and is zero when the fee is zero.
     */
    int32_t computeFeeHappinessPenalty(money64 fee, uint32_t randomValue);
} // namespace OpenRCT2::CashMachine
