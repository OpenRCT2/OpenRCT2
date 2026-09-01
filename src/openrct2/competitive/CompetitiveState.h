/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "CompetitiveModel.h"

#include <optional>
#include <string>
#include <vector>

namespace OpenRCT2::Competitive
{
    constexpr uint16_t kProtocolVersion = 2;
    constexpr uint16_t kDefaultPort = 11755;

    struct ActiveEffect
    {
        uint32_t id{};
        Ability ability = Ability::vandal;
        ParticipantId sourceId = kInvalidParticipantId;
        ParticipantId targetId = kInvalidParticipantId;
        int32_t targetRideId = -1;
        bool delivered = false;
        money64 reservedCost{};
        uint32_t startsAtDay{};
        uint32_t endsAtDay{};
        uint16_t potency{};
    };

    struct ParticipantReport
    {
        ParticipantId participantId = kInvalidParticipantId;
        ParkMetrics metrics{};
        EconomyTotals acceptedEconomy{};
        uint32_t lastScoredDay{};
    };

    struct AbilityCooldown
    {
        ParticipantId participantId = kInvalidParticipantId;
        Ability ability = Ability::vandal;
        uint16_t availableYear{};
    };

    struct MatchState
    {
        uint16_t protocol = kProtocolVersion;
        std::string matchId;
        std::string name;
        uint32_t epoch = 1;
        uint64_t revision{};
        Phase phase = Phase::none;
        ParticipantId hostId = kInvalidParticipantId;
        uint32_t startLocalDay = 1;
        MatchRules rules{};
        ScenarioIdentity scenario{};
        std::vector<Participant> participants;
        std::vector<Score> scores;
        std::vector<ParticipantReport> reports;
        std::vector<AbilityCooldown> cooldowns;
        std::vector<ActiveEffect> effects;
        uint32_t nextEffectId = 1;
        std::optional<ParticipantId> winnerId{};
        bool closedEarly = false;
    };

    [[nodiscard]] Participant* FindParticipant(MatchState& state, ParticipantId id);
    [[nodiscard]] const Participant* FindParticipant(const MatchState& state, ParticipantId id);
    [[nodiscard]] Score* FindScore(MatchState& state, ParticipantId id);
    [[nodiscard]] const Score* FindScore(const MatchState& state, ParticipantId id);
    [[nodiscard]] ParticipantReport* FindReport(MatchState& state, ParticipantId id);
    [[nodiscard]] const ParticipantReport* FindReport(const MatchState& state, ParticipantId id);
} // namespace OpenRCT2::Competitive
