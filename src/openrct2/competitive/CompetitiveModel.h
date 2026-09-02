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
        cash,
        parkValue,
    };

    enum class Ability : uint8_t
    {
        vandal,
        misinformation,
        poison,
        toiletBomber,
        agitator,
        saboteur,
        hitman,
        researchSabotage,
        karens,
        stoners,
        unionDisruption,
    };

    // Highest valid ability value, used for bounds checks on the wire.
    constexpr Ability kLastAbility = Ability::unionDisruption;

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

    // Real seconds to advance one in-game competitive year at 1x speed.
    // Derived from the date clock: 8 months/year, a 16-bit monthTicks counter
    // stepped by kMonthTicksIncrement (4) once per logic tick at kGameUpdateFPS (40).
    //   8 * (65536 / 4) / 40 = 3276.8
    constexpr double kRealSecondsPerInGameYear = 8.0 * (65536.0 / 4.0) / 40.0;

    // Synthetic competitive-clock days in one in-game year (8 months * 32 days).
    // See GetCurrentLocalDay in CompetitiveSession.cpp.
    constexpr uint32_t kCompetitiveDaysPerYear = 256;
    constexpr uint32_t kCompetitiveDaysPerMonth = 32;

    struct AbilityRule
    {
        bool enabled = true;
        money64 cost{};
        // Minimum attacker-local-day wait between uses of this action.
        uint16_t cooldownDays = 256;
        // Hard cap on uses per attacker-local year. 0 means unlimited.
        uint16_t usesPerYear = 4;
        uint16_t durationDays{};
        uint16_t potency{};
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
        //          enabled  cost            gap  uses/yr  duration  potency
        AbilityRule vandal{ true, 200.00_GBP, 32, 8, 64, 20 };
        // Misinformation cost/duration are fixed: 4-week campaign at 1.5x the
        // equivalent half-price-entry campaign (GBP 50/week). Host cannot change them.
        AbilityRule misinformation{ true, 300.00_GBP, 128, 2, 28, 200 };
        AbilityRule poison{ true, 600.00_GBP, 64, 4, 14, 40 };
        // Toilet bomber / saboteur / hitman act once and are then done (the toilet bomber dies in the
        // blast; the others leave), so durationDays is not a host knob for them - it is only a hidden
        // "give up and go home" backstop for an operative that never reaches its target.
        AbilityRule toiletBomber{ true, 2000.00_GBP, 256, 1, 128, 1 };
        AbilityRule agitator{ true, 800.00_GBP, 128, 2, 32, 14 };
        AbilityRule saboteur{ true, 400.00_GBP, 64, 4, 128, 1 };
        AbilityRule hitman{ true, 750.00_GBP, 256, 1, 128, 1 };
        // Research sabotage: forces the victim's research funding to "None" for the duration,
        // then restores whatever level the victim last chose. potency is unused.
        AbilityRule researchSabotage{ true, 200.00_GBP, 256, 1, 32, 0 };
        // Karens / Stoners: potency is the group size (number of guests spawned). durationDays is a
        // hidden ~1-year backstop only - the group actually ends when its guests have all left.
        AbilityRule karens{ true, 450.00_GBP, 128, 2, 256, 20 };
        AbilityRule stoners{ true, 250.00_GBP, 128, 2, 256, 20 };
        // Union disruption: doubles the victim's staff wages for the duration. potency is unused.
        // Duration is deliberately longer than an in-game month so a full month of wages is doubled.
        AbilityRule unionDisruption{ true, 250.00_GBP, 256, 1, 40, 0 };
    };

    struct ParkMetrics
    {
        uint16_t localYear = 1;
        uint32_t monthsElapsed{};
        uint32_t localDay = 1;
        uint16_t rating{};
        uint32_t guests{};
        money64 parkValue{};
        money64 cash{};
        uint8_t meanHappiness{};
        struct TargetRide
        {
            int32_t rideId = -1;
            std::string name;

            bool operator==(const TargetRide&) const = default;
        };
        std::vector<TargetRide> openFoodDrinkStalls;
        std::vector<TargetRide> openToilets;
        std::vector<TargetRide> openRides;
    };

    struct Score
    {
        ParticipantId participantId = kInvalidParticipantId;
        int64_t points{};
        uint16_t rating{};
        uint32_t guests{};
        money64 parkValue{};
        money64 cash{};
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

    [[nodiscard]] bool IsSameScenario(const ScenarioIdentity& lhs, const ScenarioIdentity& rhs);
    [[nodiscard]] bool DeadlineReached(uint32_t monthsElapsed, uint16_t deadlineYear);
    [[nodiscard]] bool CanTarget(const Participant& participant);
    [[nodiscard]] bool CompetitionComplete(const std::vector<Participant>& participants);
    [[nodiscard]] int64_t CalculateDailyPoints(const ParkMetrics& metrics);
    [[nodiscard]] int64_t GetMetricValue(const Score& score, Metric metric);
    [[nodiscard]] ParticipantStatus GetParticipantStatus(const Participant& participant, Phase phase);

    void UpdateLiveScore(Score& score, const ParkMetrics& metrics);
    void FreezeScore(Score& score, const ParkMetrics& metrics, uint16_t year);
    [[nodiscard]] std::optional<ParticipantId> ChooseWinner(
        const std::vector<Score>& scores, const std::vector<Participant>& participants, Metric metric);
    [[nodiscard]] bool TargetReached(const Score& score, const MatchRules& rules);
} // namespace OpenRCT2::Competitive
