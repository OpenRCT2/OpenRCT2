/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "CompetitionManager.h"

namespace OpenRCT2::Competition
{
    /**
     * Base class for competition game modes
     */
    class CompetitionMode
    {
    public:
        virtual ~CompetitionMode() = default;

        virtual void OnStart() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnEnd() = 0;
        virtual bool CheckWinCondition(const PlayerCompetitionData& player) = 0;
    };

    /**
     * Park Value Race: Highest park value wins when time expires
     */
    class ParkValueMode : public CompetitionMode
    {
    public:
        void OnStart() override;
        void OnUpdate() override;
        void OnEnd() override;
        bool CheckWinCondition(const PlayerCompetitionData& player) override;
    };

    /**
     * Guest Count Challenge: Most guests when time expires
     */
    class GuestCountMode : public CompetitionMode
    {
    public:
        void OnStart() override;
        void OnUpdate() override;
        void OnEnd() override;
        bool CheckWinCondition(const PlayerCompetitionData& player) override;
    };

    /**
     * Speed Challenge: First to reach objective wins immediately
     */
    class SpeedChallengeMode : public CompetitionMode
    {
    public:
        enum class ObjectiveType
        {
            ReachGuestCount,
            ReachParkValue,
            BuildRides,
        };

        void SetObjective(ObjectiveType type, uint32_t target);
        void OnStart() override;
        void OnUpdate() override;
        void OnEnd() override;
        bool CheckWinCondition(const PlayerCompetitionData& player) override;

    private:
        ObjectiveType _objectiveType = ObjectiveType::ReachGuestCount;
        uint32_t _targetValue = 1000;
    };

    /**
     * Survival Mode: Last player not bankrupt wins
     */
    class SurvivalMode : public CompetitionMode
    {
    public:
        void OnStart() override;
        void OnUpdate() override;
        void OnEnd() override;
        bool CheckWinCondition(const PlayerCompetitionData& player) override;
        bool IsPlayerBankrupt(const PlayerCompetitionData& player);

    private:
        money64 _bankruptcyThreshold = -10000.00_GBP;
    };

    /**
     * Ride Master: Best roller coasters win
     */
    class RideMasterMode : public CompetitionMode
    {
    public:
        void OnStart() override;
        void OnUpdate() override;
        void OnEnd() override;
        bool CheckWinCondition(const PlayerCompetitionData& player) override;

    private:
        RideRating_t _minimumExcitement = RideRating::make(6, 0); // 6.00
    };

    /**
     * Factory for creating competition modes
     */
    class CompetitionModeFactory
    {
    public:
        static std::unique_ptr<CompetitionMode> CreateMode(CompetitionType type);
    };

} // namespace OpenRCT2::Competition
