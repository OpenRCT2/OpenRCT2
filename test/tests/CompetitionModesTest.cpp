/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/competition/CompetitionModes.h>

using namespace OpenRCT2::Competition;

// ====================
// Mode Factory Tests
// ====================

TEST(CompetitionModeFactoryTest, CreateParkValueMode)
{
    auto mode = CompetitionModeFactory::CreateMode(CompetitionType::ParkValue);
    ASSERT_NE(mode, nullptr);
}

TEST(CompetitionModeFactoryTest, CreateGuestCountMode)
{
    auto mode = CompetitionModeFactory::CreateMode(CompetitionType::GuestCount);
    ASSERT_NE(mode, nullptr);
}

TEST(CompetitionModeFactoryTest, CreateSpeedChallengeMode)
{
    auto mode = CompetitionModeFactory::CreateMode(CompetitionType::FastestGoal);
    ASSERT_NE(mode, nullptr);
}

TEST(CompetitionModeFactoryTest, CreateSurvivalMode)
{
    auto mode = CompetitionModeFactory::CreateMode(CompetitionType::Survival);
    ASSERT_NE(mode, nullptr);
}

TEST(CompetitionModeFactoryTest, CreateRideMasterMode)
{
    auto mode = CompetitionModeFactory::CreateMode(CompetitionType::RideMaster);
    ASSERT_NE(mode, nullptr);
}

TEST(CompetitionModeFactoryTest, CreateDefaultMode)
{
    auto mode = CompetitionModeFactory::CreateMode(CompetitionType::None);
    ASSERT_NE(mode, nullptr);
}

// ====================
// Park Value Mode Tests
// ====================

class ParkValueModeTest : public testing::Test
{
protected:
    std::unique_ptr<ParkValueMode> _mode;

    void SetUp() override
    {
        _mode = std::make_unique<ParkValueMode>();
    }
};

TEST_F(ParkValueModeTest, OnStartNoException)
{
    EXPECT_NO_THROW(_mode->OnStart());
}

TEST_F(ParkValueModeTest, OnUpdateNoException)
{
    EXPECT_NO_THROW(_mode->OnUpdate());
}

TEST_F(ParkValueModeTest, OnEndNoException)
{
    EXPECT_NO_THROW(_mode->OnEnd());
}

TEST_F(ParkValueModeTest, NoEarlyWin)
{
    PlayerCompetitionData player;
    player.PlayerId = 1;
    player.Stats.ParkValue = 100000.00_GBP;
    player.Stats.Score = 1000;

    EXPECT_FALSE(_mode->CheckWinCondition(player));
}

// ====================
// Guest Count Mode Tests
// ====================

class GuestCountModeTest : public testing::Test
{
protected:
    std::unique_ptr<GuestCountMode> _mode;

    void SetUp() override
    {
        _mode = std::make_unique<GuestCountMode>();
    }
};

TEST_F(GuestCountModeTest, OnStartNoException)
{
    EXPECT_NO_THROW(_mode->OnStart());
}

TEST_F(GuestCountModeTest, NoEarlyWin)
{
    PlayerCompetitionData player;
    player.PlayerId = 1;
    player.Stats.GuestCount = 5000;
    player.Stats.Score = 5000;

    EXPECT_FALSE(_mode->CheckWinCondition(player));
}

// ====================
// Speed Challenge Mode Tests
// ====================

class SpeedChallengeModeTest : public testing::Test
{
protected:
    std::unique_ptr<SpeedChallengeMode> _mode;

    void SetUp() override
    {
        _mode = std::make_unique<SpeedChallengeMode>();
    }
};

TEST_F(SpeedChallengeModeTest, SetObjectiveGuestCount)
{
    EXPECT_NO_THROW(_mode->SetObjective(SpeedChallengeMode::ObjectiveType::ReachGuestCount, 1000));
}

TEST_F(SpeedChallengeModeTest, SetObjectiveParkValue)
{
    EXPECT_NO_THROW(_mode->SetObjective(SpeedChallengeMode::ObjectiveType::ReachParkValue, 50000));
}

TEST_F(SpeedChallengeModeTest, SetObjectiveBuildRides)
{
    EXPECT_NO_THROW(_mode->SetObjective(SpeedChallengeMode::ObjectiveType::BuildRides, 10));
}

TEST_F(SpeedChallengeModeTest, OnStartSetsDefaults)
{
    _mode->OnStart();
    
    PlayerCompetitionData player;
    player.Stats.GuestCount = 0;
    
    // Should not win with 0 guests (default target is 1000)
    EXPECT_FALSE(_mode->CheckWinCondition(player));
}

TEST_F(SpeedChallengeModeTest, CheckWinConditionGuestCount)
{
    _mode->SetObjective(SpeedChallengeMode::ObjectiveType::ReachGuestCount, 500);
    _mode->OnStart();

    PlayerCompetitionData player;
    player.Stats.GuestCount = 499;
    EXPECT_FALSE(_mode->CheckWinCondition(player));

    player.Stats.GuestCount = 500;
    EXPECT_TRUE(_mode->CheckWinCondition(player));

    player.Stats.GuestCount = 501;
    EXPECT_TRUE(_mode->CheckWinCondition(player));
}

TEST_F(SpeedChallengeModeTest, CheckWinConditionParkValue)
{
    _mode->SetObjective(SpeedChallengeMode::ObjectiveType::ReachParkValue, 10000);
    _mode->OnStart();

    PlayerCompetitionData player;
    player.Stats.ParkValue = 9999.00_GBP;
    EXPECT_FALSE(_mode->CheckWinCondition(player));

    player.Stats.ParkValue = 1000000.00_GBP; // $10,000 = 1,000,000 cents
    EXPECT_TRUE(_mode->CheckWinCondition(player));
}

TEST_F(SpeedChallengeModeTest, CheckWinConditionBuildRides)
{
    _mode->SetObjective(SpeedChallengeMode::ObjectiveType::BuildRides, 5);
    _mode->OnStart();

    PlayerCompetitionData player;
    player.Stats.RidesBuilt = 4;
    EXPECT_FALSE(_mode->CheckWinCondition(player));

    player.Stats.RidesBuilt = 5;
    EXPECT_TRUE(_mode->CheckWinCondition(player));

    player.Stats.RidesBuilt = 6;
    EXPECT_TRUE(_mode->CheckWinCondition(player));
}

// ====================
// Survival Mode Tests
// ====================

class SurvivalModeTest : public testing::Test
{
protected:
    std::unique_ptr<SurvivalMode> _mode;

    void SetUp() override
    {
        _mode = std::make_unique<SurvivalMode>();
    }
};

TEST_F(SurvivalModeTest, IsPlayerBankrupt)
{
    PlayerCompetitionData player;
    
    player.Stats.TotalProfit = -5000.00_GBP;
    EXPECT_FALSE(_mode->IsPlayerBankrupt(player));

    player.Stats.TotalProfit = -10000.00_GBP;
    EXPECT_TRUE(_mode->IsPlayerBankrupt(player));

    player.Stats.TotalProfit = -15000.00_GBP;
    EXPECT_TRUE(_mode->IsPlayerBankrupt(player));
}

TEST_F(SurvivalModeTest, IsPlayerNotBankrupt)
{
    PlayerCompetitionData player;
    player.Stats.TotalProfit = 5000.00_GBP;
    
    EXPECT_FALSE(_mode->IsPlayerBankrupt(player));
}

TEST_F(SurvivalModeTest, NoEarlyWin)
{
    PlayerCompetitionData player;
    player.Stats.TotalProfit = 50000.00_GBP;
    
    EXPECT_FALSE(_mode->CheckWinCondition(player));
}

// ====================
// Ride Master Mode Tests
// ====================

class RideMasterModeTest : public testing::Test
{
protected:
    std::unique_ptr<RideMasterMode> _mode;

    void SetUp() override
    {
        _mode = std::make_unique<RideMasterMode>();
    }
};

TEST_F(RideMasterModeTest, OnStartNoException)
{
    EXPECT_NO_THROW(_mode->OnStart());
}

TEST_F(RideMasterModeTest, NoEarlyWin)
{
    PlayerCompetitionData player;
    player.Stats.BestRideExcitement = RideRating::make(8, 50);
    
    EXPECT_FALSE(_mode->CheckWinCondition(player));
}

// ====================
// Integration Tests
// ====================

TEST(CompetitionModesIntegrationTest, LifecycleAllModes)
{
    std::vector<CompetitionType> types = {
        CompetitionType::ParkValue,
        CompetitionType::GuestCount,
        CompetitionType::RideIncome,
        CompetitionType::FastestGoal,
        CompetitionType::Survival,
        CompetitionType::RideMaster
    };

    for (auto type : types)
    {
        auto mode = CompetitionModeFactory::CreateMode(type);
        ASSERT_NE(mode, nullptr);
        
        EXPECT_NO_THROW(mode->OnStart());
        EXPECT_NO_THROW(mode->OnUpdate());
        EXPECT_NO_THROW(mode->OnEnd());
    }
}

TEST(CompetitionModesIntegrationTest, SpeedChallengeScenarios)
{
    auto mode = std::make_unique<SpeedChallengeMode>();

    // Scenario 1: Guest count objective
    mode->SetObjective(SpeedChallengeMode::ObjectiveType::ReachGuestCount, 1000);
    mode->OnStart();

    PlayerCompetitionData player1;
    player1.Stats.GuestCount = 1000;
    EXPECT_TRUE(mode->CheckWinCondition(player1));

    // Scenario 2: Park value objective
    mode->SetObjective(SpeedChallengeMode::ObjectiveType::ReachParkValue, 50000);
    mode->OnStart();

    PlayerCompetitionData player2;
    player2.Stats.ParkValue = 5000000.00_GBP; // $50,000
    EXPECT_TRUE(mode->CheckWinCondition(player2));

    // Scenario 3: Build rides objective
    mode->SetObjective(SpeedChallengeMode::ObjectiveType::BuildRides, 10);
    mode->OnStart();

    PlayerCompetitionData player3;
    player3.Stats.RidesBuilt = 10;
    EXPECT_TRUE(mode->CheckWinCondition(player3));
}

TEST(CompetitionModesIntegrationTest, SurvivalMultiplePlayers)
{
    auto mode = std::make_unique<SurvivalMode>();
    mode->OnStart();

    PlayerCompetitionData player1, player2, player3;
    
    player1.Stats.TotalProfit = 5000.00_GBP;
    player2.Stats.TotalProfit = -5000.00_GBP;
    player3.Stats.TotalProfit = -15000.00_GBP;

    EXPECT_FALSE(mode->IsPlayerBankrupt(player1)); // Healthy
    EXPECT_FALSE(mode->IsPlayerBankrupt(player2)); // Struggling but not bankrupt
    EXPECT_TRUE(mode->IsPlayerBankrupt(player3));  // Bankrupt
}

// ====================
// Edge Cases
// ====================

TEST(CompetitionModesEdgeCaseTest, SpeedChallengeZeroTarget)
{
    auto mode = std::make_unique<SpeedChallengeMode>();
    mode->SetObjective(SpeedChallengeMode::ObjectiveType::ReachGuestCount, 0);
    mode->OnStart();

    PlayerCompetitionData player;
    player.Stats.GuestCount = 0;
    
    // Should be able to win with 0 guests if target is 0
    EXPECT_TRUE(mode->CheckWinCondition(player));
}

TEST(CompetitionModesEdgeCaseTest, SurvivalExactThreshold)
{
    auto mode = std::make_unique<SurvivalMode>();

    PlayerCompetitionData player;
    player.Stats.TotalProfit = -10000.00_GBP; // Exactly at threshold
    
    EXPECT_TRUE(mode->IsPlayerBankrupt(player));
}

TEST(CompetitionModesEdgeCaseTest, MultipleOnStartCalls)
{
    auto mode = std::make_unique<SpeedChallengeMode>();
    
    // Should handle multiple OnStart calls gracefully
    EXPECT_NO_THROW(mode->OnStart());
    EXPECT_NO_THROW(mode->OnStart());
    EXPECT_NO_THROW(mode->OnStart());
}
