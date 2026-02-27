/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/competition/CompetitionManager.h>

using namespace OpenRCT2::Competition;

class CompetitionManagerTest : public testing::Test
{
protected:
    std::unique_ptr<CompetitionManager> _competitionMgr;

    void SetUp() override
    {
        _competitionMgr = std::make_unique<CompetitionManager>();
    }

    void TearDown() override
    {
        _competitionMgr.reset();
    }
};

// ====================
// Lifecycle Tests
// ====================

TEST_F(CompetitionManagerTest, InitializeSetup)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);

    EXPECT_EQ(_competitionMgr->GetStatus(), CompetitionStatus::Lobby);
    EXPECT_EQ(_competitionMgr->GetType(), CompetitionType::ParkValue);
    EXPECT_FALSE(_competitionMgr->IsActive());
    EXPECT_TRUE(_competitionMgr->IsInLobby());
}

TEST_F(CompetitionManagerTest, StartCompetition)
{
    CompetitionConfig config;
    config.Type = CompetitionType::GuestCount;
    config.DurationMinutes = 15;

    _competitionMgr->Initialize(config);
    _competitionMgr->Start();

    EXPECT_TRUE(_competitionMgr->IsActive());
    EXPECT_FALSE(_competitionMgr->IsInLobby());
    EXPECT_EQ(_competitionMgr->GetStatus(), CompetitionStatus::Active);
}

TEST_F(CompetitionManagerTest, EndCompetition)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 1;

    _competitionMgr->Initialize(config);
    _competitionMgr->Start();
    _competitionMgr->End();

    EXPECT_FALSE(_competitionMgr->IsActive());
    EXPECT_TRUE(_competitionMgr->IsFinished());
    EXPECT_EQ(_competitionMgr->GetStatus(), CompetitionStatus::Finished);
}

TEST_F(CompetitionManagerTest, ResetCompetition)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;

    _competitionMgr->Initialize(config);
    _competitionMgr->AddPlayer(1, "Player1");
    _competitionMgr->Start();
    _competitionMgr->Reset();

    EXPECT_EQ(_competitionMgr->GetStatus(), CompetitionStatus::Lobby);
    EXPECT_EQ(_competitionMgr->GetAllPlayerData().size(), 0);
}

// ====================
// Player Management Tests
// ====================

TEST_F(CompetitionManagerTest, AddPlayer)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Alice");
    
    auto* playerData = _competitionMgr->GetPlayerData(1);
    ASSERT_NE(playerData, nullptr);
    EXPECT_EQ(playerData->PlayerId, 1);
    EXPECT_EQ(playerData->PlayerName, "Alice");
}

TEST_F(CompetitionManagerTest, AddMultiplePlayers)
{
    CompetitionConfig config;
    config.Type = CompetitionType::GuestCount;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Alice");
    _competitionMgr->AddPlayer(2, "Bob");
    _competitionMgr->AddPlayer(3, "Charlie");

    EXPECT_EQ(_competitionMgr->GetAllPlayerData().size(), 3);
}

TEST_F(CompetitionManagerTest, RemovePlayer)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Alice");
    _competitionMgr->AddPlayer(2, "Bob");
    
    _competitionMgr->RemovePlayer(1);

    EXPECT_EQ(_competitionMgr->GetAllPlayerData().size(), 1);
    EXPECT_EQ(_competitionMgr->GetPlayerData(1), nullptr);
    EXPECT_NE(_competitionMgr->GetPlayerData(2), nullptr);
}

TEST_F(CompetitionManagerTest, GetPlayerDataInvalid)
{
    CompetitionConfig config;
    _competitionMgr->Initialize(config);

    auto* playerData = _competitionMgr->GetPlayerData(99);
    EXPECT_EQ(playerData, nullptr);
}

// ====================
// Scoring Tests
// ====================

TEST_F(CompetitionManagerTest, ParkValueScoreCalculation)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Player1");
    auto* playerData = _competitionMgr->GetPlayerData(1);
    ASSERT_NE(playerData, nullptr);

    playerData->Stats.ParkValue = 10000.00_GBP;
    _competitionMgr->CalculateScores();

    EXPECT_EQ(playerData->Stats.Score, 100); // 10000 / 100
}

TEST_F(CompetitionManagerTest, GuestCountScoreCalculation)
{
    CompetitionConfig config;
    config.Type = CompetitionType::GuestCount;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Player1");
    auto* playerData = _competitionMgr->GetPlayerData(1);
    ASSERT_NE(playerData, nullptr);

    playerData->Stats.GuestCount = 500;
    _competitionMgr->CalculateScores();

    EXPECT_EQ(playerData->Stats.Score, 5000); // 500 * 10
}

TEST_F(CompetitionManagerTest, RideIncomeScoreCalculation)
{
    CompetitionConfig config;
    config.Type = CompetitionType::RideIncome;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Player1");
    auto* playerData = _competitionMgr->GetPlayerData(1);
    ASSERT_NE(playerData, nullptr);

    playerData->Stats.TotalIncome = 5000.00_GBP;
    _competitionMgr->CalculateScores();

    EXPECT_EQ(playerData->Stats.Score, 500); // 5000 / 10
}

// ====================
// Ranking Tests
// ====================

TEST_F(CompetitionManagerTest, RankingSinglePlayer)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Player1");
    auto* playerData = _competitionMgr->GetPlayerData(1);
    playerData->Stats.Score = 1000;

    _competitionMgr->UpdateRankings();

    EXPECT_EQ(playerData->Rank, 1);
}

TEST_F(CompetitionManagerTest, RankingMultiplePlayers)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Alice");
    _competitionMgr->AddPlayer(2, "Bob");
    _competitionMgr->AddPlayer(3, "Charlie");

    auto* alice = _competitionMgr->GetPlayerData(1);
    auto* bob = _competitionMgr->GetPlayerData(2);
    auto* charlie = _competitionMgr->GetPlayerData(3);

    alice->Stats.Score = 500;
    bob->Stats.Score = 1000;
    charlie->Stats.Score = 750;

    _competitionMgr->UpdateRankings();

    EXPECT_EQ(bob->Rank, 1);      // Highest score
    EXPECT_EQ(charlie->Rank, 2);  // Middle score
    EXPECT_EQ(alice->Rank, 3);    // Lowest score
}

TEST_F(CompetitionManagerTest, RankingWithEliminatedPlayers)
{
    CompetitionConfig config;
    config.Type = CompetitionType::Survival;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Alice");
    _competitionMgr->AddPlayer(2, "Bob");

    auto* alice = _competitionMgr->GetPlayerData(1);
    auto* bob = _competitionMgr->GetPlayerData(2);

    alice->Stats.Score = 1000;
    bob->Stats.Score = 500;
    bob->IsEliminated = true;

    _competitionMgr->UpdateRankings();

    EXPECT_EQ(alice->Rank, 1);
    EXPECT_EQ(bob->Rank, 999); // Eliminated players ranked last
}

// ====================
// Leaderboard Tests
// ====================

TEST_F(CompetitionManagerTest, GetLeaderboard)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Alice");
    _competitionMgr->AddPlayer(2, "Bob");

    auto leaderboard = _competitionMgr->GetLeaderboard();

    EXPECT_EQ(leaderboard.size(), 2);
}

TEST_F(CompetitionManagerTest, LeaderboardSortedByScore)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Alice");
    _competitionMgr->AddPlayer(2, "Bob");
    _competitionMgr->AddPlayer(3, "Charlie");

    auto* alice = _competitionMgr->GetPlayerData(1);
    auto* bob = _competitionMgr->GetPlayerData(2);
    auto* charlie = _competitionMgr->GetPlayerData(3);

    alice->Stats.Score = 300;
    bob->Stats.Score = 700;
    charlie->Stats.Score = 500;

    _competitionMgr->UpdateRankings();
    auto leaderboard = _competitionMgr->GetLeaderboard();

    EXPECT_EQ(leaderboard[0].PlayerId, 2); // Bob (highest)
    EXPECT_EQ(leaderboard[1].PlayerId, 3); // Charlie
    EXPECT_EQ(leaderboard[2].PlayerId, 1); // Alice (lowest)
}

// ====================
// Time Tests
// ====================

TEST_F(CompetitionManagerTest, TimeRemainingBeforeStart)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    _competitionMgr->Initialize(config);

    EXPECT_EQ(_competitionMgr->GetTimeRemaining(), 0);
}

TEST_F(CompetitionManagerTest, TimeElapsedAfterStart)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    _competitionMgr->Initialize(config);
    _competitionMgr->Start();

    // Time elapsed should be >= 0 after starting
    EXPECT_GE(_competitionMgr->GetTimeElapsed(), 0);
}

// ====================
// Winner Tests
// ====================

TEST_F(CompetitionManagerTest, DetermineWinnerSinglePlayer)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Alice");
    _competitionMgr->Start();
    _competitionMgr->End();

    EXPECT_EQ(_competitionMgr->GetWinnerId(), 1);
}

TEST_F(CompetitionManagerTest, DetermineWinnerMultiplePlayers)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayer(1, "Alice");
    _competitionMgr->AddPlayer(2, "Bob");

    auto* alice = _competitionMgr->GetPlayerData(1);
    auto* bob = _competitionMgr->GetPlayerData(2);

    alice->Stats.Score = 500;
    bob->Stats.Score = 1000;

    _competitionMgr->Start();
    _competitionMgr->End();

    EXPECT_EQ(_competitionMgr->GetWinnerId(), 2); // Bob has higher score
}

// ====================
// Configuration Tests
// ====================

TEST_F(CompetitionManagerTest, GetConfig)
{
    CompetitionConfig config;
    config.Type = CompetitionType::RideIncome;
    config.DurationMinutes = 45;
    config.MaxPlayers = 6;
    config.EnableChat = false;

    _competitionMgr->Initialize(config);

    auto& retrievedConfig = _competitionMgr->GetConfig();
    EXPECT_EQ(retrievedConfig.Type, CompetitionType::RideIncome);
    EXPECT_EQ(retrievedConfig.DurationMinutes, 45);
    EXPECT_EQ(retrievedConfig.MaxPlayers, 6);
    EXPECT_FALSE(retrievedConfig.EnableChat);
}

// ====================
// Edge Cases
// ====================

TEST_F(CompetitionManagerTest, UpdateBeforeStart)
{
    CompetitionConfig config;
    _competitionMgr->Initialize(config);

    // Should not crash when updating before start
    EXPECT_NO_THROW(_competitionMgr->Update());
}

TEST_F(CompetitionManagerTest, EndBeforeStart)
{
    CompetitionConfig config;
    _competitionMgr->Initialize(config);

    // Should not crash when ending before start
    EXPECT_NO_THROW(_competitionMgr->End());
}

TEST_F(CompetitionManagerTest, AddPlayerAfterStart)
{
    CompetitionConfig config;
    _competitionMgr->Initialize(config);
    _competitionMgr->Start();

    // Should still be able to add players (late join)
    EXPECT_NO_THROW(_competitionMgr->AddPlayer(1, "LateJoiner"));
}
