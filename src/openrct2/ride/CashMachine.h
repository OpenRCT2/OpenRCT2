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
     * Fee at or above which a paying guest emits a complaint thought
     * (PeepThoughtType::NotPaying). Acceptance itself is probabilistic and
     * driven by happiness — see guestAcceptsFee() — so fees above this
     * threshold are not automatically refused, just much less likely to be
     * paid (and complained about when they are).
     */
    constexpr money64 kComplaintFeeThreshold = 2.00_GBP;

    /**
     * Returns true if a guest with the given happiness is willing to pay the
     * given fee at a cash machine. The decision is probabilistic: acceptance
     * probability rises with happiness and falls with fee, so even at the £2
     * complaint threshold there is some chance of refusal, and even at the £20
     * cap a maximally happy guest will sometimes still pay.
     *
     * The randomValue parameter drives the probability roll; callers should
     * supply ScenarioRand() output. Tests can pass deterministic values.
     */
    bool guestAcceptsFee(money64 fee, uint8_t happiness, uint32_t randomValue);

    /**
     * Returns the happiness reduction a guest suffers from paying a cash
     * machine fee. The penalty grows with the fee, has a small random
     * component, and is zero when the fee is zero.
     */
    int32_t computeFeeHappinessPenalty(money64 fee, uint32_t randomValue);
} // namespace OpenRCT2::CashMachine
