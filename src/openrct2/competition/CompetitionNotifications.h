/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/localisation/StringIds.h>
#include <cstdint>
#include <string>

namespace OpenRCT2::Competition
{
    /**
     * Notification system for competition events
     */
    class CompetitionNotifications
    {
    public:
        // Competition lifecycle notifications
        static void ShowCompetitionStarting(uint32_t secondsRemaining);
        static void ShowCompetitionStarted();
        static void ShowCompetitionEnded();
        static void ShowVictory();
        static void ShowDefeat();

        // Player event notifications
        static void ShowPlayerEliminated(const std::string& playerName);
        static void ShowPlayerLeading(const std::string& playerName);
        static void ShowPlayerJoined(const std::string& playerName);
        static void ShowPlayerLeft(const std::string& playerName);

        // Territory notifications
        static void ShowTerritoryAssigned();
        static void ShowBuildingRestriction();

        // Score notifications
        static void ShowRankChanged(uint32_t oldRank, uint32_t newRank);
        static void ShowScoreMilestone(uint32_t score);

        // Time notifications
        static void ShowTimeWarning(uint32_t minutesRemaining);
    };

} // namespace OpenRCT2::Competition
