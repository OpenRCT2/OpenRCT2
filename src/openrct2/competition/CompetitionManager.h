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
#include "../ride/RideRatings.h"
#include "PlayerTerritory.h"

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace OpenRCT2::Competition
{
    enum class CompetitionType : uint8_t
    {
        None,
        ParkValue,      // Highest park value wins
        GuestCount,     // Most guests wins
        RideIncome,     // Highest ride income wins
        FastestGoal,    // First to achieve objective
        Survival,       // Last player standing
        RideMaster,     // Best roller coasters
    };

    enum class CompetitionStatus : uint8_t
    {
        None,       // No competition configured
        Lobby,      // Waiting for players
        Countdown,  // Starting soon
        Active,     // Competition in progress
        Finished,   // Competition ended
    };

    struct CompetitionConfig
    {
        CompetitionType Type = CompetitionType::None;
        uint32_t DurationMinutes = 30;
        uint32_t MaxPlayers = 8;
        bool EnableChat = true;
        bool ShowLiveScores = true;
        std::string MapName;
    };

    constexpr uint32_t kMaxCompetitionPlayers = 8;
    constexpr uint32_t kMaxCompetitionDurationMinutes = 600; // 10 hours
    constexpr uint32_t kMinCompetitionDurationMinutes = 5;

    struct CompetitiveStats
    {
        money64 ParkValue = 0.00_GBP;
        money64 TotalProfit = 0.00_GBP;
        money64 TotalIncome = 0.00_GBP;
        money64 TotalExpenses = 0.00_GBP;
        uint32_t GuestCount = 0;
        uint32_t RidesBuilt = 0;
        uint32_t StaffCount = 0;
        RideRating_t ParkRating = 0;
        RideRating_t BestRideExcitement = 0;
        uint32_t Score = 0;
    };

    struct PlayerCompetitionData
    {
        uint8_t PlayerId;
        std::string PlayerName;
        CompetitiveStats Stats;
        uint32_t Rank = 0;
        bool IsEliminated = false;
        bool IsReady = false;
    };

    class CompetitionManager
    {
    public:
        CompetitionManager() = default;
        ~CompetitionManager() = default;

        // Competition lifecycle
        void Initialize(const CompetitionConfig& config);
        void Start();
        void Update();
        void End();
        void Reset();

        // Lobby management
        bool CanJoinLobby() const;
        bool AddPlayerToLobby(uint8_t playerId, const std::string& name);
        void RemovePlayerFromLobby(uint8_t playerId);
        void SetPlayerReady(uint8_t playerId, bool ready);
        bool IsPlayerReady(uint8_t playerId) const;
        bool AreAllPlayersReady() const;
        uint8_t GetLobbyPlayerCount() const;
        bool CanStartCompetition() const;

        // Player management
        void AddPlayer(uint8_t playerId, const std::string& name);
        void RemovePlayer(uint8_t playerId);
        PlayerCompetitionData* GetPlayerData(uint8_t playerId);
        const std::vector<PlayerCompetitionData>& GetAllPlayerData() const { return _playerData; }

        // Score management
        void UpdatePlayerStats(uint8_t playerId);
        void CalculateScores();
        void UpdateRankings();
        std::vector<PlayerCompetitionData> GetLeaderboard() const;

        // State queries
        bool IsActive() const { return _status == CompetitionStatus::Active; }
        bool IsInLobby() const { return _status == CompetitionStatus::Lobby; }
        bool IsFinished() const { return _status == CompetitionStatus::Finished; }
        CompetitionStatus GetStatus() const { return _status; }
        CompetitionType GetType() const { return _config.Type; }
        uint32_t GetTimeRemaining() const;
        uint32_t GetTimeElapsed() const;
        uint8_t GetWinnerId() const { return _winnerId; }
        const CompetitionConfig& GetConfig() const { return _config; }

        // Territory management
        TerritoryManager& GetTerritoryManager() { return _territoryManager; }
        const TerritoryManager& GetTerritoryManager() const { return _territoryManager; }
        void InitializeTerritories(uint32_t mapWidth, uint32_t mapHeight);
        bool CanPlayerBuildAt(uint8_t playerId, const TileCoordsXY& coord) const;
        bool CanPlayerBuildAt(uint8_t playerId, const CoordsXY& coord) const;

    private:
        CompetitionConfig _config;
        CompetitionStatus _status = CompetitionStatus::None;
        std::vector<PlayerCompetitionData> _playerData;
        std::chrono::steady_clock::time_point _startTime;
        std::chrono::steady_clock::time_point _endTime;
        uint8_t _winnerId = 0;
        TerritoryManager _territoryManager;

        void CheckWinConditions();
        void CalculateParkValueScore(PlayerCompetitionData& data);
        void CalculateGuestCountScore(PlayerCompetitionData& data);
        void CalculateRideIncomeScore(PlayerCompetitionData& data);
        void CalculateRideMasterScore(PlayerCompetitionData& data);
    };

} // namespace OpenRCT2::Competition
