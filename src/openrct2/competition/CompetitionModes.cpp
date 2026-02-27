/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CompetitionModes.h"

#include "../core/Money.hpp"

namespace OpenRCT2::Competition
{
    // ParkValueMode implementation
    void ParkValueMode::OnStart()
    {
        // No special setup needed
    }

    void ParkValueMode::OnUpdate()
    {
        // Scores updated by competition manager
    }

    void ParkValueMode::OnEnd()
    {
        // Winner determined by highest score
    }

    bool ParkValueMode::CheckWinCondition(const PlayerCompetitionData& player)
    {
        // No early win condition - time-based only
        return false;
    }

    // GuestCountMode implementation
    void GuestCountMode::OnStart()
    {
        // No special setup needed
    }

    void GuestCountMode::OnUpdate()
    {
        // Scores updated by competition manager
    }

    void GuestCountMode::OnEnd()
    {
        // Winner determined by highest guest count
    }

    bool GuestCountMode::CheckWinCondition(const PlayerCompetitionData& player)
    {
        // No early win condition - time-based only
        return false;
    }

    // SpeedChallengeMode implementation
    void SpeedChallengeMode::SetObjective(ObjectiveType type, uint32_t target)
    {
        _objectiveType = type;
        _targetValue = target;
    }

    void SpeedChallengeMode::OnStart()
    {
        // Set default objectives if not configured
        if (_targetValue == 0)
        {
            switch (_objectiveType)
            {
                case ObjectiveType::ReachGuestCount:
                    _targetValue = 1000;
                    break;
                case ObjectiveType::ReachParkValue:
                    _targetValue = 50000; // $50,000
                    break;
                case ObjectiveType::BuildRides:
                    _targetValue = 10;
                    break;
            }
        }
    }

    void SpeedChallengeMode::OnUpdate()
    {
        // Competition manager will check win conditions
    }

    void SpeedChallengeMode::OnEnd()
    {
        // Winner already determined by early completion
    }

    bool SpeedChallengeMode::CheckWinCondition(const PlayerCompetitionData& player)
    {
        switch (_objectiveType)
        {
            case ObjectiveType::ReachGuestCount:
                return player.Stats.GuestCount >= _targetValue;

            case ObjectiveType::ReachParkValue:
                return player.Stats.ParkValue >= static_cast<money64>(_targetValue) * 100.0_GBP;

            case ObjectiveType::BuildRides:
                return player.Stats.RidesBuilt >= _targetValue;

            default:
                return false;
        }
    }

    // SurvivalMode implementation
    void SurvivalMode::OnStart()
    {
        // All players start with equal conditions
    }

    void SurvivalMode::OnUpdate()
    {
        // Competition manager will eliminate bankrupt players
    }

    void SurvivalMode::OnEnd()
    {
        // Last player standing wins
    }

    bool SurvivalMode::CheckWinCondition(const PlayerCompetitionData& player)
    {
        // No specific win condition - survival is automatic win
        return false;
    }

    bool SurvivalMode::IsPlayerBankrupt(const PlayerCompetitionData& player)
    {
        // Check if player has negative cash flow and low park value
        return player.Stats.TotalProfit < _bankruptcyThreshold;
    }

    // RideMasterMode implementation
    void RideMasterMode::OnStart()
    {
        // No special setup
    }

    void RideMasterMode::OnUpdate()
    {
        // Scores updated based on best ride excitement
    }

    void RideMasterMode::OnEnd()
    {
        // Winner has highest total excitement ratings
    }

    bool RideMasterMode::CheckWinCondition(const PlayerCompetitionData& player)
    {
        // No early win - time-based only
        return false;
    }

    // CompetitionModeFactory implementation
    std::unique_ptr<CompetitionMode> CompetitionModeFactory::CreateMode(CompetitionType type)
    {
        switch (type)
        {
            case CompetitionType::ParkValue:
                return std::make_unique<ParkValueMode>();

            case CompetitionType::GuestCount:
                return std::make_unique<GuestCountMode>();

            case CompetitionType::FastestGoal:
                return std::make_unique<SpeedChallengeMode>();

            case CompetitionType::Survival:
                return std::make_unique<SurvivalMode>();

            case CompetitionType::RideMaster:
                return std::make_unique<RideMasterMode>();

            default:
                return std::make_unique<ParkValueMode>(); // Default
        }
    }

} // namespace OpenRCT2::Competition
