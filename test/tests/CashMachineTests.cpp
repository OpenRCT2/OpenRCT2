/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/core/Money.hpp>
#include <openrct2/ride/CashMachine.h>

using namespace OpenRCT2;
using namespace OpenRCT2::CashMachine;

// The cap matches kRideMaxPrice (£20) so the price spinner in the Ride window
// has the same upper bound as other priced rides. Behaviourally, guests refuse
// anything above kComplaintFeeThreshold, but the raw cap is intentionally higher.
TEST(CashMachineTests, MaxFeeIsTwentyPounds)
{
    EXPECT_EQ(kMaxFee, 20.00_GBP);
}

TEST(CashMachineTests, ComplaintThresholdIsTwoPounds)
{
    EXPECT_EQ(kComplaintFeeThreshold, 2.00_GBP);
}

// A free ATM should always be acceptable to every guest, even a miserable one.
TEST(CashMachineTests, GuestAcceptsFreeFee)
{
    // Every combination of happiness and random value should accept fee == 0.
    for (uint32_t happiness = 0; happiness <= 255; ++happiness)
    {
        for (uint32_t r = 0; r < 8; ++r)
        {
            EXPECT_TRUE(guestAcceptsFee(0, static_cast<uint8_t>(happiness), r))
                << "happiness=" << happiness << " random=" << r;
        }
    }
}

// A maximally happy guest will pay at least the smallest non-zero fee even with
// the worst-case random roll.
TEST(CashMachineTests, HappyGuestAcceptsSmallFee)
{
    EXPECT_TRUE(guestAcceptsFee(0.10_GBP, 255, 0));
    EXPECT_TRUE(guestAcceptsFee(0.50_GBP, 255, 0));
}

// A miserable guest (low happiness) should refuse the £2 complaint threshold
// even with a favourable random roll.
TEST(CashMachineTests, MiserableGuestRefusesComplaintThreshold)
{
    EXPECT_FALSE(guestAcceptsFee(kComplaintFeeThreshold, 0, 0));
    EXPECT_FALSE(guestAcceptsFee(kComplaintFeeThreshold, 0, 7));
}

// Any fee above the complaint threshold must be refused regardless of
// happiness or random roll — kMaxFee is much larger than what guests will pay.
TEST(CashMachineTests, FeeAboveComplaintThresholdAlwaysRefused)
{
    for (money64 fee : { kComplaintFeeThreshold + 0.10_GBP, 5.00_GBP, 10.00_GBP, kMaxFee })
    {
        for (uint32_t happiness = 0; happiness <= 255; happiness += 51)
        {
            for (uint32_t r = 0; r < 8; ++r)
            {
                EXPECT_FALSE(guestAcceptsFee(fee, static_cast<uint8_t>(happiness), r))
                    << "fee=" << fee << " happiness=" << happiness << " random=" << r;
            }
        }
    }
}

// Happiness should make a difference: the same fee that a sad guest refuses,
// a happy guest accepts (with the same random roll).
TEST(CashMachineTests, HappinessIncreasesTolerance)
{
    const money64 fee = 1.00_GBP;
    const uint32_t random = 0;
    EXPECT_FALSE(guestAcceptsFee(fee, 30, random));
    EXPECT_TRUE(guestAcceptsFee(fee, 255, random));
}

// A free ATM imposes no happiness penalty.
TEST(CashMachineTests, FreeFeeHasNoHappinessPenalty)
{
    for (uint32_t r = 0; r < 8; ++r)
    {
        EXPECT_EQ(computeFeeHappinessPenalty(0, r), 0);
    }
}

// The penalty grows monotonically with the fee for a fixed random seed.
TEST(CashMachineTests, PenaltyGrowsWithFee)
{
    constexpr uint32_t random = 0;
    int32_t prev = computeFeeHappinessPenalty(0.10_GBP, random);
    for (int32_t tenths = 2; tenths <= 20; ++tenths)
    {
        const money64 fee = tenths * 0.10_GBP;
        const int32_t penalty = computeFeeHappinessPenalty(fee, random);
        EXPECT_GE(penalty, prev) << "fee=" << fee;
        prev = penalty;
    }
}

// Any non-zero fee produces a positive, bounded penalty regardless of randomness.
TEST(CashMachineTests, NonZeroFeePenaltyIsBounded)
{
    for (int32_t tenths = 1; tenths <= 20; ++tenths)
    {
        const money64 fee = tenths * 0.10_GBP;
        for (uint32_t r = 0; r < 8; ++r)
        {
            const int32_t penalty = computeFeeHappinessPenalty(fee, r);
            EXPECT_GE(penalty, 1) << "fee=" << fee << " r=" << r;
            EXPECT_LE(penalty, 64) << "fee=" << fee << " r=" << r;
        }
    }
}

// Different random values should be able to produce different penalties for the
// same fee (jitter), so the system isn't perfectly deterministic.
TEST(CashMachineTests, PenaltyHasRandomJitter)
{
    const money64 fee = 1.00_GBP;
    const int32_t a = computeFeeHappinessPenalty(fee, 0);
    const int32_t b = computeFeeHappinessPenalty(fee, 7);
    EXPECT_NE(a, b);
}
