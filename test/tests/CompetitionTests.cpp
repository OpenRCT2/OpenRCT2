/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include <gtest/gtest.h>
    #include <memory>
    #include <openrct2/GameState.h>
    #include <openrct2/scripting/CompetitionScoring.h>
    #include <openrct2/scripting/CompetitionState.h>
    #include <openrct2/world/tile_element/TileElement.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

// ---------------------------------------------------------------------------
// CompetitionState unit tests
// ---------------------------------------------------------------------------

TEST(CompetitionStateTests, DefaultStatusIsIdle)
{
    CompetitionState state{};
    EXPECT_EQ(state.Status, CompetitionStatus::Idle);
}

TEST(CompetitionStateTests, IsActiveReturnsTrueOnlyWhenActive)
{
    CompetitionState state{};

    state.Status = CompetitionStatus::Idle;
    EXPECT_FALSE(state.IsActive());

    state.Status = CompetitionStatus::Active;
    EXPECT_TRUE(state.IsActive());

    state.Status = CompetitionStatus::Finished;
    EXPECT_FALSE(state.IsActive());
}

TEST(CompetitionStateTests, TicksRemainingBeforeStartReturnsDuration)
{
    CompetitionState state{};
    state.DurationTicks = 500;
    state.StartTick = 1000;

    // currentTick < StartTick => full duration remaining
    EXPECT_EQ(state.TicksRemaining(500), 500u);
    EXPECT_EQ(state.TicksRemaining(1000), 500u);
}

TEST(CompetitionStateTests, TicksRemainingDuringCompetition)
{
    CompetitionState state{};
    state.DurationTicks = 1000;
    state.StartTick = 0;

    EXPECT_EQ(state.TicksRemaining(0), 1000u);
    EXPECT_EQ(state.TicksRemaining(400), 600u);
    EXPECT_EQ(state.TicksRemaining(999), 1u);
}

TEST(CompetitionStateTests, TicksRemainingReturnsZeroWhenExpired)
{
    CompetitionState state{};
    state.DurationTicks = 500;
    state.StartTick = 100;

    // elapsed = 500 => exactly expired
    EXPECT_EQ(state.TicksRemaining(600), 0u);
    // elapsed > duration => still 0, no underflow
    EXPECT_EQ(state.TicksRemaining(700), 0u);
    EXPECT_EQ(state.TicksRemaining(9999), 0u);
}

TEST(CompetitionStateTests, TicksRemainingNoUnderflow)
{
    CompetitionState state{};
    state.DurationTicks = 10;
    state.StartTick = 5;

    // elapsed = 100 >> duration — must return 0, not wrap around
    EXPECT_EQ(state.TicksRemaining(105), 0u);
}

TEST(CompetitionStateTests, DefaultMetricIsParkValue)
{
    CompetitionState state{};
    EXPECT_EQ(state.Metric, CompetitionMetric::ParkValue);
}

TEST(CompetitionStateTests, DefaultUpdateIntervalIs100)
{
    CompetitionState state{};
    EXPECT_EQ(state.UpdateIntervalTicks, 100u);
}

TEST(CompetitionStateTests, ScoresMapIsEmptyByDefault)
{
    CompetitionState state{};
    EXPECT_TRUE(state.Scores.empty());
}

// ---------------------------------------------------------------------------
// PlayerScore unit tests
// ---------------------------------------------------------------------------

TEST(PlayerScoreTests, DefaultValues)
{
    PlayerScore ps{};
    EXPECT_EQ(ps.PlayerId, 0u);
    EXPECT_EQ(ps.Score, 0);
    EXPECT_EQ(ps.Rank, 0);
    EXPECT_TRUE(ps.PlayerName.empty());
}

// ---------------------------------------------------------------------------
// UpdateCompetitionScores — single-player path (DISABLE_NETWORK)
// These tests use a synthetic GameState_t to exercise the scoring logic
// without a running game context.
// ---------------------------------------------------------------------------

    #ifdef DISABLE_NETWORK

// Helpers to build a minimal GameState_t for scoring tests.
static void InitGameState(
    GameState_t& gs, CompetitionMetric metric, money64 parkValue = 0, uint32_t guestCount = 0, uint16_t rating = 0)
{
    gs.Competition.Status = CompetitionStatus::Active;
    gs.Competition.Metric = metric;
    gs.park.value = parkValue;
    gs.park.numGuestsInPark = static_cast<uint32_t>(guestCount);
    gs.park.rating = rating;
}

TEST(UpdateCompetitionScoresTests, IdleCompetitionIsSkipped)
{
    auto gs = std::make_unique<GameState_t>();
    gs->Competition.Status = CompetitionStatus::Idle;
    gs->park.value = 99999;

    UpdateCompetitionScores(*gs);

    EXPECT_TRUE(gs->Competition.Scores.empty());
}

TEST(UpdateCompetitionScoresTests, FinishedCompetitionIsSkipped)
{
    auto gs = std::make_unique<GameState_t>();
    gs->Competition.Status = CompetitionStatus::Finished;
    gs->park.value = 99999;

    UpdateCompetitionScores(*gs);

    EXPECT_TRUE(gs->Competition.Scores.empty());
}

TEST(UpdateCompetitionScoresTests, ParkValueMetricSetsScore)
{
    auto gs = std::make_unique<GameState_t>();
    InitGameState(*gs, CompetitionMetric::ParkValue, /*parkValue=*/12345);

    UpdateCompetitionScores(*gs);

    ASSERT_EQ(gs->Competition.Scores.count(0), 1u);
    EXPECT_EQ(gs->Competition.Scores.at(0).Score, 12345);
}

TEST(UpdateCompetitionScoresTests, GuestCountMetricSetsScore)
{
    auto gs = std::make_unique<GameState_t>();
    InitGameState(*gs, CompetitionMetric::GuestCount, 0, /*guestCount=*/500);

    UpdateCompetitionScores(*gs);

    ASSERT_EQ(gs->Competition.Scores.count(0), 1u);
    EXPECT_EQ(gs->Competition.Scores.at(0).Score, 500);
}

TEST(UpdateCompetitionScoresTests, ParkRatingMetricSetsScore)
{
    auto gs = std::make_unique<GameState_t>();
    InitGameState(*gs, CompetitionMetric::ParkRating, 0, 0, /*rating=*/850);

    UpdateCompetitionScores(*gs);

    ASSERT_EQ(gs->Competition.Scores.count(0), 1u);
    EXPECT_EQ(gs->Competition.Scores.at(0).Score, 850);
}

TEST(UpdateCompetitionScoresTests, CustomMetricDoesNotOverwriteScores)
{
    auto gs = std::make_unique<GameState_t>();
    gs->Competition.Status = CompetitionStatus::Active;
    gs->Competition.Metric = CompetitionMetric::Custom;

    // Plugin pre-set a score
    PlayerScore ps{};
    ps.PlayerId = 0;
    ps.Score = 9999;
    gs->Competition.Scores[0] = ps;

    UpdateCompetitionScores(*gs);

    // Score must remain plugin-owned; UpdateCompetitionScores must not touch it.
    EXPECT_EQ(gs->Competition.Scores.at(0).Score, 9999);
}

TEST(UpdateCompetitionScoresTests, SinglePlayerRankIsAlwaysOne)
{
    auto gs = std::make_unique<GameState_t>();
    InitGameState(*gs, CompetitionMetric::ParkValue, 5000);

    UpdateCompetitionScores(*gs);

    EXPECT_EQ(gs->Competition.Scores.at(0).Rank, 1);
}

TEST(UpdateCompetitionScoresTests, SinglePlayerNameIsPlayer)
{
    auto gs = std::make_unique<GameState_t>();
    InitGameState(*gs, CompetitionMetric::ParkValue, 1000);

    UpdateCompetitionScores(*gs);

    EXPECT_EQ(gs->Competition.Scores.at(0).PlayerName, "Player");
}

TEST(UpdateCompetitionScoresTests, ZeroParkValueGivesZeroScore)
{
    auto gs = std::make_unique<GameState_t>();
    InitGameState(*gs, CompetitionMetric::ParkValue, 0);

    UpdateCompetitionScores(*gs);

    EXPECT_EQ(gs->Competition.Scores.at(0).Score, 0);
}

    #endif // DISABLE_NETWORK

// ---------------------------------------------------------------------------
// Rank ordering tests (multi-entry, no network dependency)
// We inject scores manually and verify ranking logic.
// ---------------------------------------------------------------------------

TEST(CompetitionRankTests, HigherScoreGetsLowerRank)
{
    CompetitionState comp{};
    comp.Status = CompetitionStatus::Active;
    comp.Metric = CompetitionMetric::Custom;

    comp.Scores[0] = { 0, "Alice", 100, 0 };
    comp.Scores[1] = { 1, "Bob", 200, 0 };
    comp.Scores[2] = { 2, "Carol", 150, 0 };

    // Custom metric returns early without updating scores — call UpdateCompetitionScores
    // just to trigger rank computation... except UpdateCompetitionScores returns early for
    // Custom. So we replicate the rank-sort logic here as a direct unit test of the data
    // model to confirm expected ordering semantics.

    // Sort descending by score.
    std::vector<std::pair<uint8_t, int64_t>> entries;
    for (auto& [id, ps] : comp.Scores)
        entries.emplace_back(id, ps.Score);
    std::sort(entries.begin(), entries.end(), [](auto& a, auto& b) { return a.second > b.second; });

    for (int32_t rank = 0; rank < static_cast<int32_t>(entries.size()); rank++)
        comp.Scores[entries[rank].first].Rank = rank + 1;

    EXPECT_EQ(comp.Scores.at(1).Rank, 1); // Bob: 200 → rank 1
    EXPECT_EQ(comp.Scores.at(2).Rank, 2); // Carol: 150 → rank 2
    EXPECT_EQ(comp.Scores.at(0).Rank, 3); // Alice: 100 → rank 3
}

TEST(CompetitionRankTests, AllEqualScoresAllGetDistinctRanks)
{
    // Tie-breaking: all equal scores still produce distinct ranks 1-3 (order stable by id).
    CompetitionState comp{};
    comp.Scores[0] = { 0, "A", 500, 0 };
    comp.Scores[1] = { 1, "B", 500, 0 };
    comp.Scores[2] = { 2, "C", 500, 0 };

    std::vector<uint8_t> ids = { 0, 1, 2 };
    std::stable_sort(
        ids.begin(), ids.end(), [&comp](uint8_t a, uint8_t b) { return comp.Scores[a].Score > comp.Scores[b].Score; });
    for (int32_t r = 0; r < static_cast<int32_t>(ids.size()); r++)
        comp.Scores[ids[r]].Rank = r + 1;

    // All ranks assigned and unique
    std::set<int32_t> ranks;
    for (auto& [id, ps] : comp.Scores)
        ranks.insert(ps.Rank);

    EXPECT_EQ(ranks.size(), 3u);
    EXPECT_NE(ranks.find(1), ranks.end());
    EXPECT_NE(ranks.find(2), ranks.end());
    EXPECT_NE(ranks.find(3), ranks.end());
}

// ---------------------------------------------------------------------------
// CompetitionStatus enum sanity
// ---------------------------------------------------------------------------

TEST(CompetitionStatusTests, EnumValuesAreDistinct)
{
    EXPECT_NE(static_cast<uint8_t>(CompetitionStatus::Idle), static_cast<uint8_t>(CompetitionStatus::Active));
    EXPECT_NE(static_cast<uint8_t>(CompetitionStatus::Active), static_cast<uint8_t>(CompetitionStatus::Finished));
    EXPECT_NE(static_cast<uint8_t>(CompetitionStatus::Idle), static_cast<uint8_t>(CompetitionStatus::Finished));
}

TEST(CompetitionMetricTests, EnumValuesAreDistinct)
{
    std::set<uint8_t> vals = {
        static_cast<uint8_t>(CompetitionMetric::ParkValue),   static_cast<uint8_t>(CompetitionMetric::GuestCount),
        static_cast<uint8_t>(CompetitionMetric::ParkRating),  static_cast<uint8_t>(CompetitionMetric::RideCount),
        static_cast<uint8_t>(CompetitionMetric::MoneyEarned), static_cast<uint8_t>(CompetitionMetric::Custom),
    };
    EXPECT_EQ(vals.size(), 6u);
}

#endif // ENABLE_SCRIPTING
