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

    int64_t CalculateDailyPoints(const ParkMetrics& metrics)
    {
        if (metrics.guests == 0)
        {
            return 0;
        }

        return static_cast<int64_t>(std::llround(
            static_cast<double>(metrics.guests) * static_cast<double>(metrics.meanHappiness) / 255.0));
    }

    int64_t GetMetricValue(const Score& score, Metric metric)
    {
        switch (metric)
        {
            case Metric::rating:
                return score.rating;
            case Metric::guests:
                return score.guests;
            case Metric::competitiveCash:
                return score.competitiveCash;
            case Metric::parkValue:
                return score.parkValue;
            case Metric::points:
                return score.points;
        }
        return score.points;
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
        score.parkValue = metrics.parkValue;
        score.points += CalculateDailyPoints(metrics);
    }

    void FreezeScore(Score& score, const ParkMetrics& metrics, uint16_t year)
    {
        if (score.frozenAtYear.has_value())
        {
            return;
        }

        score.rating = metrics.rating;
        score.guests = metrics.guests;
        score.parkValue = metrics.parkValue;
        score.frozenAtYear = year;
    }

    void ApplyEconomyDelta(
        Score& score, EconomyTotals& acceptedTotals, const ParkMetrics& report, const EconomyRules& rules)
    {
        if (score.frozenAtYear.has_value())
        {
            return;
        }

        const auto arrivals = report.arrivalsGenerated > acceptedTotals.arrivalsGenerated
            ? report.arrivalsGenerated - acceptedTotals.arrivalsGenerated
            : 0;
        const auto spend = report.constructionSpend > acceptedTotals.constructionSpend
            ? report.constructionSpend - acceptedTotals.constructionSpend
            : 0;
        const auto rideCustomers = report.rideCustomers > acceptedTotals.rideCustomers
            ? report.rideCustomers - acceptedTotals.rideCustomers
            : 0;
        const auto stallCustomers = report.stallCustomers > acceptedTotals.stallCustomers
            ? report.stallCustomers - acceptedTotals.stallCustomers
            : 0;

        const money64 income = static_cast<money64>(arrivals) * rules.incomePerArrival
            + static_cast<money64>(rideCustomers) * rules.incomePerRideCustomer
            + static_cast<money64>(stallCustomers) * rules.incomePerStallCustomer;

        score.competitiveCash += income - spend;
        score.lifetimeIncome += income;
        score.lifetimeSpend += spend;

        acceptedTotals.arrivalsGenerated = std::max(acceptedTotals.arrivalsGenerated, report.arrivalsGenerated);
        acceptedTotals.constructionSpend = std::max(acceptedTotals.constructionSpend, report.constructionSpend);
        acceptedTotals.rideCustomers = std::max(acceptedTotals.rideCustomers, report.rideCustomers);
        acceptedTotals.stallCustomers = std::max(acceptedTotals.stallCustomers, report.stallCustomers);
    }

    std::optional<ParticipantId> ChooseWinner(
        const std::vector<Score>& scores, const std::vector<Participant>& participants, Metric metric)
    {
        const Score* winner = nullptr;
        for (const auto& score : scores)
        {
            const auto participant = std::find_if(
                participants.begin(), participants.end(),
                [&score](const Participant& value) { return value.id == score.participantId; });
            if (participant == participants.end() || participant->role == Role::spectator || participant->forfeited)
            {
                continue;
            }

            if (winner == nullptr || GetMetricValue(score, metric) > GetMetricValue(*winner, metric)
                || (GetMetricValue(score, metric) == GetMetricValue(*winner, metric) && score.points > winner->points)
                || (GetMetricValue(score, metric) == GetMetricValue(*winner, metric) && score.points == winner->points
                    && score.participantId < winner->participantId))
            {
                winner = &score;
            }
        }

        if (winner == nullptr)
        {
            return std::nullopt;
        }
        return winner->participantId;
    }

    bool TargetReached(const Score& score, const MatchRules& rules)
    {
        return rules.victoryMode == VictoryMode::target && GetMetricValue(score, rules.metric) >= rules.target;
    }
} // namespace OpenRCT2::Competitive

