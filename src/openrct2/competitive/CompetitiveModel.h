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

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace OpenRCT2::Competitive
{
    using ParticipantId = uint64_t;

    constexpr ParticipantId kInvalidParticipantId = 0;

    enum class Role : uint8_t
    {
        none,
        host,
        player,
        spectator,
    };

    enum class Phase : uint8_t
    {
        none,
        lobby,
        running,
        finished,
    };

    enum class VictoryMode : uint8_t
    {
        deadline,
        target,
    };

    enum class Metric : uint8_t
    {
        points,
        rating,
        guests,
        competitiveCash,
        parkValue,
    };

    enum class Ability : uint8_t
    {
        vandal,
        misinformation,
        poison,
    };

    enum class ParticipantStatus : uint8_t
    {
        lobby,
        ready,
        playing,
        offline,
        finished,
        forfeited,
    };

    struct ScenarioIdentity
    {
        std::string fileName;
        std::string name;
        std::string contentHash;
        uint16_t mapWidth{};
        uint16_t mapHeight{};
        bool noMoney{};

        bool operator==(const ScenarioIdentity&) const = default;
    };

    struct AbilityRule
    {
        bool enabled = true;
        money64 cost{};
        uint16_t cooldownDays = 256;
        uint16_t durationDays{};
        uint16_t potency{};
    };

    struct EconomyRules
    {
        money64 startingCash = 20000.00_GBP;
        money64 incomePerArrival = 3.00_GBP;
        money64 incomePerRideCustomer = 2.00_GBP;
        money64 incomePerStallCustomer = 1.20_GBP;
    };

    struct MatchRules
    {
        VictoryMode victoryMode = VictoryMode::deadline;
        Metric metric = Metric::points;
        int64_t target = 100000;
        uint16_t deadlineYear = 20;
        uint8_t maxPlayers = 8;
        bool allowLateJoin = false;
        uint8_t maxGameSpeed = 1;
        EconomyRules economy{};
        AbilityRule vandal{ true, 2500.00_GBP, 256, 64, 4 };
        AbilityRule misinformation{ true, 1800.00_GBP, 256, 14, 200 };
        AbilityRule poison{ true, 2200.00_GBP, 256, 7, 25 };
    };

    struct ParkMetrics
    {
        uint16_t localYear = 1;
        uint32_t monthsElapsed{};
        uint32_t localDay = 1;
        uint16_t rating{};
        uint32_t guests{};
        money64 parkValue{};
        uint8_t meanHappiness{};
        uint64_t arrivalsGenerated{};
        money64 constructionSpend{};
        uint64_t rideCustomers{};
        uint64_t stallCustomers{};
        struct Stall
        {
            int32_t rideId = -1;
            std::string name;

            bool operator==(const Stall&) const = default;
        };
        std::vector<Stall> openFoodDrinkStalls;
    };

    struct Score
    {
        ParticipantId participantId = kInvalidParticipantId;
        int64_t points{};
        uint16_t rating{};
        uint32_t guests{};
        money64 parkValue{};
        money64 competitiveCash{};
        money64 lifetimeIncome{};
        money64 lifetimeSpend{};
        std::optional<uint16_t> frozenAtYear{};
    };

    struct Participant
    {
        ParticipantId id = kInvalidParticipantId;
        std::string identityKey;
        std::string name;
        Role role = Role::player;
        bool online = false;
        bool ready = false;
        bool finished = false;
        bool forfeited = false;
        uint32_t joinedOrder{};
        uint16_t currentYear = 1;
        ScenarioIdentity scenario{};
        std::string watchHost;
        uint16_t watchPort{};
    };

    struct EconomyTotals
    {
        uint64_t arrivalsGenerated{};
        money64 constructionSpend{};
        uint64_t rideCustomers{};
        uint64_t stallCustomers{};
    };

    [[nodiscard]] bool IsSameScenario(const ScenarioIdentity& lhs, const ScenarioIdentity& rhs);
    [[nodiscard]] bool DeadlineReached(uint32_t monthsElapsed, uint16_t deadlineYear);
    [[nodiscard]] bool CanTarget(const Participant& participant);
    [[nodiscard]] bool CompetitionComplete(const std::vector<Participant>& participants);
    [[nodiscard]] int64_t CalculateDailyPoints(const ParkMetrics& metrics);
    [[nodiscard]] int64_t GetMetricValue(const Score& score, Metric metric);
    [[nodiscard]] ParticipantStatus GetParticipantStatus(const Participant& participant, Phase phase);

    void UpdateLiveScore(Score& score, const ParkMetrics& metrics);
    void FreezeScore(Score& score, const ParkMetrics& metrics, uint16_t year);
    void ApplyEconomyDelta(
        Score& score, EconomyTotals& acceptedTotals, const ParkMetrics& report, const EconomyRules& rules);

    [[nodiscard]] std::optional<ParticipantId> ChooseWinner(
        const std::vector<Score>& scores, const std::vector<Participant>& participants, Metric metric);
    [[nodiscard]] bool TargetReached(const Score& score, const MatchRules& rules);
} // namespace OpenRCT2::Competitive
