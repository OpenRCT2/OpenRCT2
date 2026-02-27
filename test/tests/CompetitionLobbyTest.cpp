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

class CompetitionLobbyTest : public testing::Test
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
// Lobby Initialization Tests
// ====================

TEST_F(CompetitionLobbyTest, InitializeCreatesLobby)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);

    EXPECT_TRUE(_competitionMgr->IsInLobby());
    EXPECT_FALSE(_competitionMgr->IsActive());
    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 0);
}

TEST_F(CompetitionLobbyTest, EnforcesMaxPlayerLimit)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 12; // Above limit

    _competitionMgr->Initialize(config);

    // Should be capped at kMaxCompetitionPlayers (8)
    EXPECT_EQ(_competitionMgr->GetConfig().MaxPlayers, kMaxCompetitionPlayers);
}

TEST_F(CompetitionLobbyTest, EnforcesDurationLimits)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 800; // Above max
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);

    EXPECT_EQ(_competitionMgr->GetConfig().DurationMinutes, kMaxCompetitionDurationMinutes);

    // Test minimum
    config.DurationMinutes = 2; // Below min
    _competitionMgr->Initialize(config);

    EXPECT_EQ(_competitionMgr->GetConfig().DurationMinutes, kMinCompetitionDurationMinutes);
}

// ====================
// Player Join/Leave Tests
// ====================

TEST_F(CompetitionLobbyTest, PlayerCanJoinLobby)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);

    bool joined = _competitionMgr->AddPlayerToLobby(1, "Alice");

    EXPECT_TRUE(joined);
    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 1);

    auto* playerData = _competitionMgr->GetPlayerData(1);
    ASSERT_NE(playerData, nullptr);
    EXPECT_EQ(playerData->PlayerName, "Alice");
    EXPECT_FALSE(playerData->IsReady);
}

TEST_F(CompetitionLobbyTest, CannotJoinFullLobby)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 3;

    _competitionMgr->Initialize(config);

    // Fill lobby
    EXPECT_TRUE(_competitionMgr->AddPlayerToLobby(1, "Player1"));
    EXPECT_TRUE(_competitionMgr->AddPlayerToLobby(2, "Player2"));
    EXPECT_TRUE(_competitionMgr->AddPlayerToLobby(3, "Player3"));

    // Try to add one more
    EXPECT_FALSE(_competitionMgr->AddPlayerToLobby(4, "Player4"));
    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 3);
}

TEST_F(CompetitionLobbyTest, CannotJoinTwice)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);

    EXPECT_TRUE(_competitionMgr->AddPlayerToLobby(1, "Alice"));
    EXPECT_FALSE(_competitionMgr->AddPlayerToLobby(1, "Alice")); // Same player again

    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 1);
}

TEST_F(CompetitionLobbyTest, CannotJoinAfterStart)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);
    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");
    _competitionMgr->SetPlayerReady(1, true);
    _competitionMgr->SetPlayerReady(2, true);
    _competitionMgr->Start();

    // Try to join after start
    bool joined = _competitionMgr->AddPlayerToLobby(3, "Player3");
    EXPECT_FALSE(joined);
}

TEST_F(CompetitionLobbyTest, RemovePlayerFromLobby)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");

    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 2);

    _competitionMgr->RemovePlayerFromLobby(1);

    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 1);
    EXPECT_EQ(_competitionMgr->GetPlayerData(1), nullptr);
    EXPECT_NE(_competitionMgr->GetPlayerData(2), nullptr);
}

// ====================
// Ready State Tests
// ====================

TEST_F(CompetitionLobbyTest, PlayerReadyState)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);
    _competitionMgr->AddPlayerToLobby(1, "Player1");

    EXPECT_FALSE(_competitionMgr->IsPlayerReady(1));

    _competitionMgr->SetPlayerReady(1, true);
    EXPECT_TRUE(_competitionMgr->IsPlayerReady(1));

    _competitionMgr->SetPlayerReady(1, false);
    EXPECT_FALSE(_competitionMgr->IsPlayerReady(1));
}

TEST_F(CompetitionLobbyTest, AllPlayersReady)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");
    _competitionMgr->AddPlayerToLobby(3, "Player3");

    EXPECT_FALSE(_competitionMgr->AreAllPlayersReady());

    _competitionMgr->SetPlayerReady(1, true);
    EXPECT_FALSE(_competitionMgr->AreAllPlayersReady());

    _competitionMgr->SetPlayerReady(2, true);
    EXPECT_FALSE(_competitionMgr->AreAllPlayersReady());

    _competitionMgr->SetPlayerReady(3, true);
    EXPECT_TRUE(_competitionMgr->AreAllPlayersReady());
}

TEST_F(CompetitionLobbyTest, AllPlayersReadyEmptyLobby)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);

    // Empty lobby should not be ready
    EXPECT_FALSE(_competitionMgr->AreAllPlayersReady());
}

TEST_F(CompetitionLobbyTest, ReadyStateResetOnRemove)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);

    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");
    
    _competitionMgr->SetPlayerReady(1, true);
    _competitionMgr->SetPlayerReady(2, true);

    EXPECT_TRUE(_competitionMgr->AreAllPlayersReady());

    _competitionMgr->RemovePlayerFromLobby(1);

    // Player 2 still ready but now only one player
    EXPECT_TRUE(_competitionMgr->IsPlayerReady(2));
    EXPECT_TRUE(_competitionMgr->AreAllPlayersReady());
}

// ====================
// Start Condition Tests
// ====================

TEST_F(CompetitionLobbyTest, CannotStartWithOnePlayer)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);
    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->SetPlayerReady(1, true);

    EXPECT_FALSE(_competitionMgr->CanStartCompetition());
}

TEST_F(CompetitionLobbyTest, CannotStartWhenNotAllReady)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);
    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");
    _competitionMgr->SetPlayerReady(1, true);
    // Player 2 not ready

    EXPECT_FALSE(_competitionMgr->CanStartCompetition());
}

TEST_F(CompetitionLobbyTest, CanStartWhenAllReady)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);
    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");
    _competitionMgr->SetPlayerReady(1, true);
    _competitionMgr->SetPlayerReady(2, true);

    EXPECT_TRUE(_competitionMgr->CanStartCompetition());
}

TEST_F(CompetitionLobbyTest, CannotStartFromNonLobbyState)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);
    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");
    _competitionMgr->SetPlayerReady(1, true);
    _competitionMgr->SetPlayerReady(2, true);
    
    _competitionMgr->Start();

    EXPECT_FALSE(_competitionMgr->CanStartCompetition());
}

TEST_F(CompetitionLobbyTest, StartTransitionsToActive)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);
    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");
    _competitionMgr->SetPlayerReady(1, true);
    _competitionMgr->SetPlayerReady(2, true);

    EXPECT_TRUE(_competitionMgr->IsInLobby());

    _competitionMgr->Start();

    EXPECT_FALSE(_competitionMgr->IsInLobby());
    EXPECT_TRUE(_competitionMgr->IsActive());
}

TEST_F(CompetitionLobbyTest, CannotStartDirectlyWithoutReady)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);
    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");

    // Try to start without ready
    _competitionMgr->Start();

    // Should remain in lobby
    EXPECT_TRUE(_competitionMgr->IsInLobby());
    EXPECT_FALSE(_competitionMgr->IsActive());
}

// ====================
// Edge Case Tests
// ====================

TEST_F(CompetitionLobbyTest, MaxPlayerCapacityEightPlayers)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 8;

    _competitionMgr->Initialize(config);

    // Add 8 players
    for (uint8_t i = 1; i <= 8; i++)
    {
        EXPECT_TRUE(_competitionMgr->AddPlayerToLobby(i, "Player" + std::to_string(i)));
    }

    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 8);

    // Try to add 9th player
    EXPECT_FALSE(_competitionMgr->AddPlayerToLobby(9, "Player9"));
}

TEST_F(CompetitionLobbyTest, DurationRangeValidation)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.MaxPlayers = 4;

    // Test exactly at boundaries
    config.DurationMinutes = kMinCompetitionDurationMinutes;
    _competitionMgr->Initialize(config);
    EXPECT_EQ(_competitionMgr->GetConfig().DurationMinutes, kMinCompetitionDurationMinutes);

    config.DurationMinutes = kMaxCompetitionDurationMinutes;
    _competitionMgr->Initialize(config);
    EXPECT_EQ(_competitionMgr->GetConfig().DurationMinutes, kMaxCompetitionDurationMinutes);
}

TEST_F(CompetitionLobbyTest, ResetClearsLobby)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);
    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");
    _competitionMgr->SetPlayerReady(1, true);

    _competitionMgr->Reset();

    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 0);
    EXPECT_EQ(_competitionMgr->GetStatus(), CompetitionStatus::None);
}
