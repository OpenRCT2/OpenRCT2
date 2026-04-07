/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "CompetitionScoring.h"

    #include "../GameState.h"
    #include "../network/Network.h"
    #include "../ride/Ride.h"

namespace OpenRCT2::Scripting
{
    static int64_t ComputeBuiltinScore(CompetitionMetric metric, const GameState_t& gameState)
    {
        switch (metric)
        {
            case CompetitionMetric::ParkValue:
                return gameState.park.value;
            case CompetitionMetric::GuestCount:
                return static_cast<int64_t>(gameState.park.numGuestsInPark);
            case CompetitionMetric::ParkRating:
                return static_cast<int64_t>(gameState.park.rating);
            case CompetitionMetric::RideCount:
                return static_cast<int64_t>(RideGetCount());
            case CompetitionMetric::MoneyEarned:
            case CompetitionMetric::Custom:
                // MoneyEarned is per-player, handled below.
                // Custom is set by the plugin directly.
                return 0;
        }
        return 0;
    }

    void UpdateCompetitionScores(GameState_t& gameState)
    {
        auto& comp = gameState.Competition;
        if (!comp.IsActive())
            return;

        if (comp.Metric == CompetitionMetric::Custom)
        {
            // Scores are set directly by the plugin via setPlayerScore().
            return;
        }

    #ifndef DISABLE_NETWORK
        const auto numPlayers = Network::GetNumPlayers();
        for (int32_t i = 0; i < numPlayers; i++)
        {
            auto playerId = static_cast<uint8_t>(Network::GetPlayerID(i));
            auto& entry = comp.Scores[playerId];
            entry.PlayerId = playerId;
            entry.PlayerName = Network::GetPlayerName(i);

            if (comp.Metric == CompetitionMetric::MoneyEarned)
            {
                // MoneySpent is positive for spending; negate for "earned" metric.
                entry.Score = -static_cast<int64_t>(Network::GetPlayerMoneySpent(i));
            }
            else
            {
                entry.Score = ComputeBuiltinScore(comp.Metric, gameState);
            }
        }
    #else
        // Single-player fallback: one player with id 0.
        auto& entry = comp.Scores[0];
        entry.PlayerId = 0;
        entry.PlayerName = "Player";
        entry.Score = ComputeBuiltinScore(comp.Metric, gameState);
    #endif

        // Compute ranks by sorting scores descending.
        std::vector<uint8_t> playerIds;
        playerIds.reserve(comp.Scores.size());
        for (auto& [id, _] : comp.Scores)
        {
            playerIds.push_back(id);
        }
        std::sort(playerIds.begin(), playerIds.end(), [&comp](uint8_t a, uint8_t b) {
            return comp.Scores[a].Score > comp.Scores[b].Score;
        });
        for (int32_t rank = 0; rank < static_cast<int32_t>(playerIds.size()); rank++)
        {
            comp.Scores[playerIds[rank]].Rank = rank + 1;
        }
    }
} // namespace OpenRCT2::Scripting

#endif
