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

class CompetitionIntegrationTest : public testing::Test
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

    void SetupFullLobby(uint8_t playerCount)
    {
        CompetitionConfig config;
        config.Type = CompetitionType::ParkValue;
        config.DurationMinutes = 30;
        config.MaxPlayers = playerCount;

        _competitionMgr->Initialize(config);

        for (uint8_t i = 1; i <= playerCount; i++)
        {
            _competitionMgr->AddPlayerToLobby(i, "Player" + std::to_string(i));
        }
    }

    void SetAllPlayersReady()
    {
        const auto& players = _competitionMgr->GetAllPlayerData();
        for (const auto& player : players)
        {
            _competitionMgr->SetPlayerReady(player.PlayerId, true);
        }
    }
};

// ====================
// Complete Flow Tests
// ====================

TEST_F(CompetitionIntegrationTest, CompleteCompetitionFlow)
{
    // 1. Initialize lobby
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);
    EXPECT_TRUE(_competitionMgr->IsInLobby());

    // 2. Players join
    _competitionMgr->AddPlayerToLobby(1, "Alice");
    _competitionMgr->AddPlayerToLobby(2, "Bob");
    _competitionMgr->AddPlayerToLobby(3, "Charlie");
    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 3);

    // 3. Players ready up
    _competitionMgr->SetPlayerReady(1, true);
    _competitionMgr->SetPlayerReady(2, true);
    EXPECT_FALSE(_competitionMgr->AreAllPlayersReady());
    
    _competitionMgr->SetPlayerReady(3, true);
    EXPECT_TRUE(_competitionMgr->AreAllPlayersReady());

    // 4. Start competition
    EXPECT_TRUE(_competitionMgr->CanStartCompetition());
    _competitionMgr->Start();
    EXPECT_TRUE(_competitionMgr->IsActive());
    EXPECT_FALSE(_competitionMgr->IsInLobby());

    // 5. End competition
    _competitionMgr->End();
    EXPECT_TRUE(_competitionMgr->IsFinished());
    EXPECT_FALSE(_competitionMgr->IsActive());
}

TEST_F(CompetitionIntegrationTest, PlayerJoinLeaveCycle)
{
    CompetitionConfig config;
    config.Type = CompetitionType::GuestCount;
    config.DurationMinutes = 60;
    config.MaxPlayers = 8;

    _competitionMgr->Initialize(config);

    // Player joins
    EXPECT_TRUE(_competitionMgr->AddPlayerToLobby(1, "Player1"));
    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 1);

    // Player leaves
    _competitionMgr->RemovePlayerFromLobby(1);
    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 0);

    // Player rejoins
    EXPECT_TRUE(_competitionMgr->AddPlayerToLobby(1, "Player1"));
    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 1);
}

TEST_F(CompetitionIntegrationTest, ReadyUnreadyCycle)
{
    SetupFullLobby(3);

    _competitionMgr->SetPlayerReady(1, true);
    EXPECT_TRUE(_competitionMgr->IsPlayerReady(1));
    EXPECT_FALSE(_competitionMgr->AreAllPlayersReady());

    _competitionMgr->SetPlayerReady(1, false);
    EXPECT_FALSE(_competitionMgr->IsPlayerReady(1));

    _competitionMgr->SetPlayerReady(1, true);
    _competitionMgr->SetPlayerReady(2, true);
    _competitionMgr->SetPlayerReady(3, true);
    EXPECT_TRUE(_competitionMgr->AreAllPlayersReady());
}

TEST_F(CompetitionIntegrationTest, HostLeavesBeforeStart)
{
    SetupFullLobby(4);
    
    _competitionMgr->SetPlayerReady(1, true);
    _competitionMgr->SetPlayerReady(2, true);
    _competitionMgr->SetPlayerReady(3, true);
    _competitionMgr->SetPlayerReady(4, true);

    // Host (Player 1) leaves
    _competitionMgr->RemovePlayerFromLobby(1);

    // Should still be able to start with remaining 3 players
    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 3);
    EXPECT_TRUE(_competitionMgr->AreAllPlayersReady());
    EXPECT_TRUE(_competitionMgr->CanStartCompetition());
}

TEST_F(CompetitionIntegrationTest, LastPlayerUnreadiesBeforeStart)
{
    SetupFullLobby(4);
    
    _competitionMgr->SetPlayerReady(1, true);
    _competitionMgr->SetPlayerReady(2, true);
    _competitionMgr->SetPlayerReady(3, true);
    _competitionMgr->SetPlayerReady(4, true);

    EXPECT_TRUE(_competitionMgr->CanStartCompetition());

    // Last player unreadies
    _competitionMgr->SetPlayerReady(4, false);

    EXPECT_FALSE(_competitionMgr->CanStartCompetition());
    EXPECT_FALSE(_competitionMgr->AreAllPlayersReady());
}

// ====================
// Stress Tests
// ====================

TEST_F(CompetitionIntegrationTest, MaximumPlayerCapacity)
{
    SetupFullLobby(8);
    
    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 8);
    EXPECT_FALSE(_competitionMgr->CanJoinLobby());

    // All ready up
    SetAllPlayersReady();
    EXPECT_TRUE(_competitionMgr->AreAllPlayersReady());
    EXPECT_TRUE(_competitionMgr->CanStartCompetition());

    // Start with max players
    _competitionMgr->Start();
    EXPECT_TRUE(_competitionMgr->IsActive());
}

TEST_F(CompetitionIntegrationTest, MinimumPlayerRequirement)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 8;

    _competitionMgr->Initialize(config);

    // Add exactly 2 players (minimum)
    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");

    _competitionMgr->SetPlayerReady(1, true);
    _competitionMgr->SetPlayerReady(2, true);

    EXPECT_TRUE(_competitionMgr->CanStartCompetition());
    _competitionMgr->Start();
    EXPECT_TRUE(_competitionMgr->IsActive());
}

TEST_F(CompetitionIntegrationTest, RapidJoinLeave)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 8;

    _competitionMgr->Initialize(config);

    // Rapid join/leave cycles
    for (int cycle = 0; cycle < 5; cycle++)
    {
        for (uint8_t i = 1; i <= 4; i++)
        {
            _competitionMgr->AddPlayerToLobby(i, "Player" + std::to_string(i));
        }
        EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 4);

        for (uint8_t i = 1; i <= 4; i++)
        {
            _competitionMgr->RemovePlayerFromLobby(i);
        }
        EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 0);
    }
}

TEST_F(CompetitionIntegrationTest, AllCompetitionTypes)
{
    std::vector<CompetitionType> types = {
        CompetitionType::ParkValue,
        CompetitionType::GuestCount,
        CompetitionType::RideIncome,
        CompetitionType::FastestGoal,
        CompetitionType::Survival,
        CompetitionType::RideMaster
    };

    for (const auto& type : types)
    {
        CompetitionConfig config;
        config.Type = type;
        config.DurationMinutes = 30;
        config.MaxPlayers = 4;

        _competitionMgr->Initialize(config);
        _competitionMgr->AddPlayerToLobby(1, "Player1");
        _competitionMgr->AddPlayerToLobby(2, "Player2");
        _competitionMgr->SetPlayerReady(1, true);
        _competitionMgr->SetPlayerReady(2, true);

        EXPECT_TRUE(_competitionMgr->CanStartCompetition());
        _competitionMgr->Start();
        EXPECT_TRUE(_competitionMgr->IsActive());

        _competitionMgr->Reset();
    }
}

// ====================
// Error Condition Tests
// ====================

TEST_F(CompetitionIntegrationTest, InvalidPlayerOperations)
{
    SetupFullLobby(3);

    // Try to ready non-existent player
    _competitionMgr->SetPlayerReady(99, true);
    EXPECT_FALSE(_competitionMgr->IsPlayerReady(99));

    // Try to remove non-existent player
    uint8_t countBefore = _competitionMgr->GetLobbyPlayerCount();
    _competitionMgr->RemovePlayerFromLobby(99);
    EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), countBefore);

    // Get data for non-existent player
    auto* data = _competitionMgr->GetPlayerData(99);
    EXPECT_EQ(data, nullptr);
}

TEST_F(CompetitionIntegrationTest, StateTransitionValidation)
{
    CompetitionConfig config;
    config.Type = CompetitionType::ParkValue;
    config.DurationMinutes = 30;
    config.MaxPlayers = 4;

    _competitionMgr->Initialize(config);

    // Add players
    _competitionMgr->AddPlayerToLobby(1, "Player1");
    _competitionMgr->AddPlayerToLobby(2, "Player2");
    _competitionMgr->SetPlayerReady(1, true);
    _competitionMgr->SetPlayerReady(2, true);

    // Start competition
    _competitionMgr->Start();
    EXPECT_TRUE(_competitionMgr->IsActive());

    // Cannot join after start
    EXPECT_FALSE(_competitionMgr->CanJoinLobby());
    EXPECT_FALSE(_competitionMgr->AddPlayerToLobby(3, "Player3"));

    // Cannot ready/unready during active competition
    auto* player1 = _competitionMgr->GetPlayerData(1);
    bool readyStateBefore = player1->IsReady;
    _competitionMgr->SetPlayerReady(1, false);
    EXPECT_EQ(player1->IsReady, readyStateBefore); // Should not change
}

TEST_F(CompetitionIntegrationTest, MultipleResetsCycle)
{
    for (int i = 0; i < 3; i++)
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

        EXPECT_TRUE(_competitionMgr->IsActive());

        _competitionMgr->Reset();
        EXPECT_EQ(_competitionMgr->GetLobbyPlayerCount(), 0);
        EXPECT_EQ(_competitionMgr->GetStatus(), CompetitionStatus::None);
    }
}

// ====================
// Configuration Tests
// ====================

TEST_F(CompetitionIntegrationTest, DifferentDurations)
{
    std::vector<uint32_t> durations = { 5, 30, 60, 120, 300, 600 };

    for (uint32_t duration : durations)
    {
        CompetitionConfig config;
        config.Type = CompetitionType::ParkValue;
        config.DurationMinutes = duration;
        config.MaxPlayers = 4;

        _competitionMgr->Initialize(config);
        EXPECT_EQ(_competitionMgr->GetConfig().DurationMinutes, duration);

        _competitionMgr->Reset();
    }
}

TEST_F(CompetitionIntegrationTest, DifferentMaxPlayers)
{
    std::vector<uint32_t> maxPlayers = { 2, 4, 6, 8 };

    for (uint32_t max : maxPlayers)
    {
        CompetitionConfig config;
        config.Type = CompetitionType::ParkValue;
        config.DurationMinutes = 30;
        config.MaxPlayers = max;

        _competitionMgr->Initialize(config);
        EXPECT_EQ(_competitionMgr->GetConfig().MaxPlayers, max);

        // Fill lobby
        for (uint8_t i = 1; i <= max; i++)
        {
            EXPECT_TRUE(_competitionMgr->AddPlayerToLobby(i, "Player" + std::to_string(i)));
        }

        // Cannot add more
        EXPECT_FALSE(_competitionMgr->AddPlayerToLobby(max + 1, "ExtraPlayer"));

        _competitionMgr->Reset();
    }
}
