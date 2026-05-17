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

// Any fee above the complaint threshold is no longer an outright refusal — a
// max-happy guest still has a decent chance of paying even £20 — so test the
// probabilistic behaviour instead. See MaxHappyGuestSometimesAcceptsMaxFee and
// MaxHappyGuestSometimesRefusesComplaintThreshold below.

// Happiness should make a difference: with the same random roll, a near-zero
// happiness guest refuses a £1 fee while a max-happiness guest accepts it.
// Roll value 200 is chosen so it sits inside the accept window for h=255
// (pScaled ≈ 1000) but outside it for h=30 (pScaled ≈ 100).
TEST(CashMachineTests, HappinessIncreasesAcceptanceProbability)
{
    const money64 fee = 1.00_GBP;
    const uint32_t random = 200;
    EXPECT_FALSE(guestAcceptsFee(fee, 30, random));
    EXPECT_TRUE(guestAcceptsFee(fee, 255, random));
}

// At the £2 complaint threshold a max-happy guest mostly pays, but the outcome
// is still randomised — there exist random rolls that produce a refusal.
TEST(CashMachineTests, MaxHappyGuestSometimesRefusesComplaintThreshold)
{
    // With h=255, fee=£2 the accept probability ≈ 980/1024, so rolls in
    // [980, 1023] refuse. 1000 sits inside that window.
    EXPECT_FALSE(guestAcceptsFee(kComplaintFeeThreshold, 255, 1000));
    // And a low roll inside the accept window still accepts.
    EXPECT_TRUE(guestAcceptsFee(kComplaintFeeThreshold, 255, 0));
}

// Up at the £20 cap, even a max-happy guest has a markedly lower chance of
// paying — but it is not zero, so some random rolls still produce acceptance.
TEST(CashMachineTests, MaxHappyGuestSometimesAcceptsMaxFee)
{
    // With h=255, fee=£20 the accept probability ≈ 620/1024.
    // Roll 100 lies in the accept window; roll 900 lies outside it.
    EXPECT_TRUE(guestAcceptsFee(kMaxFee, 255, 100));
    EXPECT_FALSE(guestAcceptsFee(kMaxFee, 255, 900));
}

// Acceptance probability should fall as the fee climbs: a max-happy guest is
// more likely to pay £2 than £20. Sample the full 0..1023 roll space to get
// exact accept counts.
TEST(CashMachineTests, AcceptanceProbabilityDecreasesWithFee)
{
    auto acceptRateOver1024 = [](money64 fee, uint8_t happiness) {
        int32_t accepts = 0;
        for (uint32_t r = 0; r < 1024; ++r)
        {
            if (guestAcceptsFee(fee, happiness, r))
                ++accepts;
        }
        return accepts;
    };

    const int32_t atTwo = acceptRateOver1024(kComplaintFeeThreshold, 255);
    const int32_t atTwenty = acceptRateOver1024(kMaxFee, 255);
    EXPECT_GT(atTwo, atTwenty);
    // Sanity: at £2 the guest pays most of the time, at £20 they pay markedly less.
    EXPECT_GT(atTwo, 900);
    EXPECT_LT(atTwenty, 700);
    EXPECT_GT(atTwenty, 400); // but still meaningfully > 0
}

// Acceptance probability should rise with happiness for any given fee.
TEST(CashMachineTests, AcceptanceProbabilityIncreasesWithHappiness)
{
    auto acceptRateOver1024 = [](money64 fee, uint8_t happiness) {
        int32_t accepts = 0;
        for (uint32_t r = 0; r < 1024; ++r)
        {
            if (guestAcceptsFee(fee, happiness, r))
                ++accepts;
        }
        return accepts;
    };

    const money64 fee = 10.00_GBP;
    EXPECT_LT(acceptRateOver1024(fee, 64), acceptRateOver1024(fee, 128));
    EXPECT_LT(acceptRateOver1024(fee, 128), acceptRateOver1024(fee, 192));
    EXPECT_LT(acceptRateOver1024(fee, 192), acceptRateOver1024(fee, 255));
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
