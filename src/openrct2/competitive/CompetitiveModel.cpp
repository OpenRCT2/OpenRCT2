/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CompetitiveModel.h"

#include <algorithm>
#include <cmath>

namespace OpenRCT2::Competitive
{
    bool IsSameScenario(const ScenarioIdentity& lhs, const ScenarioIdentity& rhs)
    {
        if (!lhs.contentHash.empty() || !rhs.contentHash.empty())
        {
            return !lhs.contentHash.empty() && lhs.contentHash == rhs.contentHash;
        }

        return lhs.fileName == rhs.fileName && lhs.mapWidth == rhs.mapWidth && lhs.mapHeight == rhs.mapHeight;
    }

    bool DeadlineReached(uint32_t monthsElapsed, uint16_t deadlineYear)
    {
        return monthsElapsed >= static_cast<uint32_t>(deadlineYear) * 8;
    }

    bool CanTarget(const Participant& participant)
    {
        return participant.role != Role::spectator && participant.online && !participant.finished && !participant.forfeited;
    }

    bool CompetitionComplete(const std::vector<Participant>& participants)
    {
        bool hasEligibleParticipant = false;
        for (const auto& participant : participants)
        {
            if (participant.role == Role::spectator || participant.forfeited)
            {
                continue;
            }

            hasEligibleParticipant = true;
            if (!participant.finished)
            {
                return false;
            }
        }
        return hasEligibleParticipant;
    }

    int64_t GetMetricValue(const Score& score, Metric metric)
    {
        switch (metric)
        {
            case Metric::parkRating:
                return score.rating;
            case Metric::guestHappiness:
                return score.happiness;
            case Metric::guestCount:
                return score.guests;
            case Metric::parkValue:
                return score.parkValue;
            case Metric::cash:
                return score.cash;
        }
        return 0;
    }

    std::optional<Metric> SingleMetric(const MatchRules& rules)
    {
        std::optional<Metric> only;
        for (size_t m = 0; m < kMetricCount; m++)
        {
            if (rules.metricWeights[m] == 0)
                continue;
            if (only.has_value())
                return std::nullopt; // more than one weighted metric
            only = static_cast<Metric>(m);
        }
        return only;
    }

    namespace
    {
        bool ScoreIsEligible(const Score& score, const std::vector<Participant>& participants)
        {
            const auto p = std::find_if(participants.begin(), participants.end(), [&](const Participant& v) {
                return v.id == score.participantId;
            });
            return p != participants.end() && p->role != Role::spectator && !p->forfeited;
        }
    } // namespace

    int64_t ComputeCompositeScore(
        ParticipantId participantId, const std::vector<Score>& scores, const std::vector<Participant>& participants,
        const MatchRules& rules)
    {
        std::array<double, kMetricCount> maxValue{};
        const Score* mine = nullptr;
        for (const auto& score : scores)
        {
            if (!ScoreIsEligible(score, participants))
                continue;
            if (score.participantId == participantId)
                mine = &score;
            for (size_t m = 0; m < kMetricCount; m++)
            {
                const auto value = static_cast<double>(GetMetricValue(score, static_cast<Metric>(m)));
                maxValue[m] = std::max(maxValue[m], value);
            }
        }
        if (mine == nullptr)
            return 0;

        uint32_t totalWeight = 0;
        double blended = 0.0;
        for (size_t m = 0; m < kMetricCount; m++)
        {
            const auto weight = rules.metricWeights[m];
            if (weight == 0)
                continue;
            totalWeight += weight;
            const double value = static_cast<double>(GetMetricValue(*mine, static_cast<Metric>(m)));
            const double normalised = maxValue[m] <= 0.0 ? 0.0 : std::clamp(value / maxValue[m], 0.0, 1.0);
            blended += (weight / 100.0) * normalised;
        }
        if (totalWeight == 0)
            return 0;
        blended *= 100.0 / totalWeight; // tolerate weights that don't sum to exactly 100
        return static_cast<int64_t>(std::llround(std::clamp(blended, 0.0, 1.0) * 1000.0));
    }

    ParticipantStatus GetParticipantStatus(const Participant& participant, Phase phase)
    {
        if (participant.forfeited)
        {
            return ParticipantStatus::forfeited;
        }
        if (participant.finished)
        {
            return ParticipantStatus::finished;
        }
        if (!participant.online)
        {
            return ParticipantStatus::offline;
        }
        if (phase == Phase::lobby)
        {
            return participant.ready ? ParticipantStatus::ready : ParticipantStatus::lobby;
        }
        return ParticipantStatus::playing;
    }

    void UpdateLiveScore(Score& score, const ParkMetrics& metrics)
    {
        if (score.frozenAtYear.has_value())
        {
            return;
        }

        score.rating = metrics.rating;
        score.guests = metrics.guests;
        score.happiness = metrics.meanHappiness;
        score.parkValue = metrics.parkValue;
        score.cash = metrics.cash;
    }

    void FreezeScore(Score& score, const ParkMetrics& metrics, uint16_t year)
    {
        if (score.frozenAtYear.has_value())
        {
            return;
        }

        score.rating = metrics.rating;
        score.guests = metrics.guests;
        score.happiness = metrics.meanHappiness;
        score.parkValue = metrics.parkValue;
        score.cash = metrics.cash;
        score.frozenAtYear = year;
    }

    std::optional<ParticipantId> ChooseWinner(
        const std::vector<Score>& scores, const std::vector<Participant>& participants, const MatchRules& rules)
    {
        std::optional<ParticipantId> winner;
        int64_t bestComposite = -1;
        for (const auto& score : scores)
        {
            if (!ScoreIsEligible(score, participants))
                continue;
            const auto composite = ComputeCompositeScore(score.participantId, scores, participants, rules);
            if (!winner.has_value() || composite > bestComposite
                || (composite == bestComposite && score.participantId < *winner))
            {
                bestComposite = composite;
                winner = score.participantId;
            }
        }
        return winner;
    }

    bool TargetReached(
        ParticipantId participantId, const std::vector<Score>& scores, const std::vector<Participant>& participants,
        const MatchRules& rules)
    {
        return rules.victoryMode == VictoryMode::target
            && ComputeCompositeScore(participantId, scores, participants, rules) >= rules.target;
    }
} // namespace OpenRCT2::Competitive
