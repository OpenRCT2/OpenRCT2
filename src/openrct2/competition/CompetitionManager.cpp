/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CompetitionManager.h"
#include "PlayerTerritory.h"

#include <algorithm>

namespace OpenRCT2::Competition
{
    void CompetitionManager::Initialize(const CompetitionConfig& config)
    {
        _config = config;
        _status = CompetitionStatus::Lobby;
        _playerData.clear();
        _winnerId = 0;
    }

    void CompetitionManager::Start()
    {
        _status = CompetitionStatus::Active;
        _startTime = std::chrono::steady_clock::now();
        _endTime = _startTime + std::chrono::minutes(_config.DurationMinutes);

        // Initialize all player stats
        for (auto& data : _playerData)
        {
            data.Stats = CompetitiveStats{};
            data.Rank = 0;
            data.IsEliminated = false;
        }
    }

    void CompetitionManager::Update()
    {
        if (_status != CompetitionStatus::Active)
            return;

        // Check if time expired (except for FastestGoal and Survival modes)
        if (_config.Type != CompetitionType::FastestGoal && _config.Type != CompetitionType::Survival)
        {
            auto now = std::chrono::steady_clock::now();
            if (now >= _endTime)
            {
                End();
                return;
            }
        }

        // Update stats for all active players
        for (auto& data : _playerData)
        {
            if (!data.IsEliminated)
            {
                UpdatePlayerStats(data.PlayerId);
            }
        }

        CalculateScores();
        UpdateRankings();
        CheckWinConditions();
    }

    void CompetitionManager::End()
    {
        _status = CompetitionStatus::Finished;

        // Determine winner (highest score among non-eliminated players)
        PlayerCompetitionData* winner = nullptr;
        uint32_t highestScore = 0;

        for (auto& data : _playerData)
        {
            if (!data.IsEliminated && data.Stats.Score > highestScore)
            {
                highestScore = data.Stats.Score;
                winner = &data;
            }
        }

        if (winner != nullptr)
        {
            _winnerId = winner->PlayerId;
        }
    }

    void CompetitionManager::Reset()
    {
        _status = CompetitionStatus::None;
        _playerData.clear();
        _winnerId = 0;
        _config = CompetitionConfig{};
    }

    void CompetitionManager::AddPlayer(uint8_t playerId, const std::string& name)
    {
        // Check if player already exists
        for (const auto& data : _playerData)
        {
            if (data.PlayerId == playerId)
                return;
        }

        PlayerCompetitionData data;
        data.PlayerId = playerId;
        data.PlayerName = name;
        data.Stats = CompetitiveStats{};
        _playerData.push_back(data);
    }

    void CompetitionManager::RemovePlayer(uint8_t playerId)
    {
        _playerData.erase(
            std::remove_if(
                _playerData.begin(), _playerData.end(),
                [playerId](const auto& data) { return data.PlayerId == playerId; }),
            _playerData.end());
    }

    PlayerCompetitionData* CompetitionManager::GetPlayerData(uint8_t playerId)
    {
        for (auto& data : _playerData)
        {
            if (data.PlayerId == playerId)
                return &data;
        }
        return nullptr;
    }

    void CompetitionManager::UpdatePlayerStats(uint8_t playerId)
    {
        auto* data = GetPlayerData(playerId);
        if (!data)
            return;

        // Get player's territory
        auto* territory = _territoryManager.GetTerritory(playerId);
        if (!territory || !territory->HasBounds())
        {
            // No territory assigned, use default values
            data->Stats.ParkValue = 0.00_GBP;
            data->Stats.GuestCount = 0;
            data->Stats.RidesBuilt = 0;
            data->Stats.StaffCount = 0;
            data->Stats.ParkRating = 0;
            data->Stats.BestRideExcitement = 0;
            data->Stats.TotalIncome = 0.00_GBP;
            data->Stats.TotalExpenses = 0.00_GBP;
            data->Stats.TotalProfit = 0.00_GBP;
            return;
        }

        // Update territory statistics
        territory->UpdateStatistics();

        // Copy statistics from territory to player data
        data->Stats.ParkValue = territory->GetTerritoryValue();
        data->Stats.GuestCount = territory->GetGuestCount();
        data->Stats.RidesBuilt = territory->GetRideCount();
        data->Stats.StaffCount = territory->GetStaffCount();
        data->Stats.ParkRating = territory->GetAverageParkRating();
        data->Stats.BestRideExcitement = territory->GetBestRideExcitement();
        data->Stats.TotalIncome = territory->GetTotalIncome();
        data->Stats.TotalExpenses = territory->GetTotalExpenses();
        data->Stats.TotalProfit = data->Stats.TotalIncome - data->Stats.TotalExpenses;
    }

    void CompetitionManager::CalculateScores()
    {
        for (auto& data : _playerData)
        {
            if (data.IsEliminated)
                continue;

            switch (_config.Type)
            {
                case CompetitionType::ParkValue:
                    CalculateParkValueScore(data);
                    break;
                case CompetitionType::GuestCount:
                    CalculateGuestCountScore(data);
                    break;
                case CompetitionType::RideIncome:
                    CalculateRideIncomeScore(data);
                    break;
                case CompetitionType::RideMaster:
                    CalculateRideMasterScore(data);
                    break;
                default:
                    data.Stats.Score = 0;
                    break;
            }
        }
    }

    void CompetitionManager::CalculateParkValueScore(PlayerCompetitionData& data)
    {
        // Score = Park Value / 100
        data.Stats.Score = static_cast<uint32_t>(data.Stats.ParkValue / 100);
    }

    void CompetitionManager::CalculateGuestCountScore(PlayerCompetitionData& data)
    {
        // Score = Guest Count * 10
        data.Stats.Score = data.Stats.GuestCount * 10;
    }

    void CompetitionManager::CalculateRideIncomeScore(PlayerCompetitionData& data)
    {
        // Score = Total Income / 10
        data.Stats.Score = static_cast<uint32_t>(data.Stats.TotalIncome / 10);
    }

    void CompetitionManager::CalculateRideMasterScore(PlayerCompetitionData& data)
    {
        // Score = Best Ride Excitement * 100
        data.Stats.Score = static_cast<uint32_t>(data.Stats.BestRideExcitement * 100);
    }

    void CompetitionManager::UpdateRankings()
    {
        // Sort by score descending
        std::sort(_playerData.begin(), _playerData.end(), [](const auto& a, const auto& b) {
            // Eliminated players go to the bottom
            if (a.IsEliminated != b.IsEliminated)
                return !a.IsEliminated;
            return a.Stats.Score > b.Stats.Score;
        });

        // Assign ranks
        uint32_t rank = 1;
        for (auto& data : _playerData)
        {
            if (!data.IsEliminated)
            {
                data.Rank = rank++;
            }
            else
            {
                data.Rank = 999; // Eliminated players
            }
        }
    }

    std::vector<PlayerCompetitionData> CompetitionManager::GetLeaderboard() const
    {
        return _playerData;
    }

    uint32_t CompetitionManager::GetTimeRemaining() const
    {
        if (_status != CompetitionStatus::Active)
            return 0;

        auto now = std::chrono::steady_clock::now();
        if (now >= _endTime)
            return 0;

        auto remaining = std::chrono::duration_cast<std::chrono::seconds>(_endTime - now);
        return static_cast<uint32_t>(remaining.count());
    }

    uint32_t CompetitionManager::GetTimeElapsed() const
    {
        if (_status == CompetitionStatus::Lobby)
            return 0;

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - _startTime);
        return static_cast<uint32_t>(elapsed.count());
    }

    void CompetitionManager::CheckWinConditions()
    {
        // For FastestGoal mode, check if any player achieved objective
        if (_config.Type == CompetitionType::FastestGoal)
        {
            for ([[maybe_unused]] auto& data : _playerData)
            {
                // TODO: Check if player completed objective
                // If yes, set them as winner and end competition
                // This will be implemented when territory system is ready
            }
        }

        // For Survival mode, check for bankruptcy
        if (_config.Type == CompetitionType::Survival)
        {
            // TODO: Check if players are bankrupt
            // Eliminate bankrupt players
            // This will be implemented when territory system is ready

            // If only one player remains, they win
            auto activeCount = std::count_if(
                _playerData.begin(), _playerData.end(), [](const auto& d) { return !d.IsEliminated; });

            if (activeCount <= 1)
            {
                End();
            }
        }
    }

    void CompetitionManager::InitializeTerritories(uint32_t mapWidth, uint32_t mapHeight)
    {
        // Clear existing territories
        _territoryManager.Clear();

        // Divide map based on number of players
        uint8_t numPlayers = static_cast<uint8_t>(_playerData.size());
        if (numPlayers == 0)
            return;

        _territoryManager.DivideMapEqually(numPlayers, mapWidth, mapHeight);

        // Assign territories to players
        for (size_t i = 0; i < _playerData.size(); i++)
        {
            _territoryManager.AssignTerritoryToPlayer(_playerData[i].PlayerId, static_cast<uint32_t>(i));
        }
    }

    bool CompetitionManager::CanPlayerBuildAt(uint8_t playerId, const TileCoordsXY& coord) const
    {
        return _territoryManager.CanPlayerBuildAt(playerId, coord);
    }

    bool CompetitionManager::CanPlayerBuildAt(uint8_t playerId, const CoordsXY& coord) const
    {
        return _territoryManager.CanPlayerBuildAt(playerId, coord);
    }

} // namespace OpenRCT2::Competition
