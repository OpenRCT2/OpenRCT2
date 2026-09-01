/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/competitive/CompetitiveModel.h>

using namespace OpenRCT2::Competitive;

TEST(CompetitiveTests, ScenarioHashTakesPriorityOverDisplayMetadata)
{
    ScenarioIdentity lhs{ "forest.sc6", "Forest", "abc", 128, 128, false };
    ScenarioIdentity rhs{ "renamed.sc6", "Renamed", "abc", 256, 256, true };
    EXPECT_TRUE(IsSameScenario(lhs, rhs));

    rhs.contentHash = "different";
    EXPECT_FALSE(IsSameScenario(lhs, rhs));
}

TEST(CompetitiveTests, DeadlineUsesEightScenarioMonthsPerYear)
{
    EXPECT_FALSE(DeadlineReached(159, 20));
    EXPECT_TRUE(DeadlineReached(160, 20));
}

TEST(CompetitiveTests, OfflineAndFinishedParksCannotBeTargeted)
{
    Participant participant{ .id = 1, .online = true };
    EXPECT_TRUE(CanTarget(participant));
    participant.online = false;
    EXPECT_FALSE(CanTarget(participant));
    participant.online = true;
    participant.finished = true;
    EXPECT_FALSE(CanTarget(participant));
}

TEST(CompetitiveTests, DailyPointsPreservePluginFormula)
{
    ParkMetrics metrics{ .guests = 500, .meanHappiness = 204 };
    EXPECT_EQ(CalculateDailyPoints(metrics), 400);
}

TEST(CompetitiveTests, FrozenScoresIgnoreLaterReports)
{
    Score score{ .participantId = 1, .competitiveCash = 20000.00_GBP };
    ParkMetrics frozen{ .localYear = 20, .rating = 900, .guests = 2000, .parkValue = 500000.00_GBP };
    FreezeScore(score, frozen, 20);

    ParkMetrics later{ .localYear = 21, .rating = 999, .guests = 9000, .parkValue = 900000.00_GBP };
    UpdateLiveScore(score, later);
    FreezeScore(score, later, 21);

    EXPECT_EQ(score.rating, 900);
    EXPECT_EQ(score.guests, 2000u);
    EXPECT_EQ(score.frozenAtYear, 20);
}

TEST(CompetitiveTests, EconomyReportsAreIdempotent)
{
    MatchRules rules;
    Score score{ .participantId = 1, .competitiveCash = rules.economy.startingCash };
    EconomyTotals accepted;
    ParkMetrics report{
        .arrivalsGenerated = 10,
        .constructionSpend = 100.00_GBP,
        .rideCustomers = 4,
        .stallCustomers = 2,
    };

    ApplyEconomyDelta(score, accepted, report, rules.economy);
    const auto afterFirstReport = score.competitiveCash;
    ApplyEconomyDelta(score, accepted, report, rules.economy);

    EXPECT_EQ(score.competitiveCash, afterFirstReport);
    EXPECT_EQ(score.lifetimeSpend, 100.00_GBP);
    EXPECT_EQ(score.lifetimeIncome, 40.40_GBP);
}

TEST(CompetitiveTests, WinnerUsesMetricThenPointsThenStableId)
{
    std::vector<Participant> participants = {
        { .id = 10, .online = true },
        { .id = 20, .online = true },
        { .id = 30, .online = true, .forfeited = true },
    };
    std::vector<Score> scores = {
        { .participantId = 10, .points = 100, .guests = 500 },
        { .participantId = 20, .points = 200, .guests = 500 },
        { .participantId = 30, .points = 9999, .guests = 9999 },
    };

    EXPECT_EQ(ChooseWinner(scores, participants, Metric::guests), 20);
    scores[0].points = 200;
    EXPECT_EQ(ChooseWinner(scores, participants, Metric::guests), 10);
}

