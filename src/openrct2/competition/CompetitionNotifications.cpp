/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CompetitionNotifications.h"

#include "../Context.h"
#include "../interface/Window.h"
#include "../localisation/Formatter.h"
#include "../localisation/StringIdType.h"
#include "../localisation/StringIds.h"
#include "../ui/UiContext.h"

namespace OpenRCT2::Competition
{
    void CompetitionNotifications::ShowCompetitionStarting(uint32_t secondsRemaining)
    {
        auto ft = Formatter();
        ft.Add<uint32_t>(secondsRemaining);
        
        ContextShowError(STR_COMPETITION_STARTING, kStringIdNone, ft, false);
    }

    void CompetitionNotifications::ShowCompetitionStarted()
    {
        auto ft = Formatter();
        ContextShowError(STR_COMPETITION_STARTED, kStringIdNone, ft, true);
    }

    void CompetitionNotifications::ShowCompetitionEnded()
    {
        auto ft = Formatter();
        ContextShowError(STR_COMPETITION_ENDED, kStringIdNone, ft, false);
    }

    void CompetitionNotifications::ShowVictory()
    {
        auto ft = Formatter();
        ContextShowError(STR_YOU_WON_COMPETITION, kStringIdNone, ft, false);
        
        // Play victory sound
        // Audio::Play3D(SoundId::Applause, ...);
    }

    void CompetitionNotifications::ShowDefeat()
    {
        auto ft = Formatter();
        ContextShowError(STR_YOU_LOST_COMPETITION, kStringIdNone, ft, false);
    }

    void CompetitionNotifications::ShowPlayerEliminated(const std::string& playerName)
    {
        auto ft = Formatter();
        ft.Add<const char*>(playerName.c_str());
        
        ContextShowError(STR_PLAYER_ELIMINATED, kStringIdNone, ft, true);
    }

    void CompetitionNotifications::ShowPlayerLeading(const std::string& playerName)
    {
        auto ft = Formatter();
        ft.Add<const char*>(playerName.c_str());
        
        ContextShowError(STR_PLAYER_LEADING, kStringIdNone, ft, true);
    }

    void CompetitionNotifications::ShowPlayerJoined(const std::string& playerName)
    {
        auto ft = Formatter();
        ft.Add<const char*>(playerName.c_str());
        
        // Show in chat or notification area
    }

    void CompetitionNotifications::ShowPlayerLeft(const std::string& playerName)
    {
        auto ft = Formatter();
        ft.Add<const char*>(playerName.c_str());
        
        // Show in chat or notification area
    }

    void CompetitionNotifications::ShowTerritoryAssigned()
    {
        auto ft = Formatter();
        ContextShowError(STR_TERRITORY_ASSIGNED, kStringIdNone, ft, true);
    }

    void CompetitionNotifications::ShowBuildingRestriction()
    {
        auto ft = Formatter();
        ContextShowError(STR_CANT_BUILD_IN_OTHER_PLAYERS_TERRITORY, kStringIdNone, ft, false);
    }

    void CompetitionNotifications::ShowRankChanged(uint32_t oldRank, uint32_t newRank)
    {
        if (newRank < oldRank)
        {
            // Moved up in rankings
            auto ft = Formatter();
            ft.Add<uint32_t>(newRank);
            // Show positive notification
        }
    }

    void CompetitionNotifications::ShowScoreMilestone(uint32_t score)
    {
        // Show milestone achievements (e.g., 1000, 5000, 10000 points)
        auto ft = Formatter();
        ft.Add<uint32_t>(score);
        // Optional: Show achievement notification
    }

    void CompetitionNotifications::ShowTimeWarning(uint32_t minutesRemaining)
    {
        auto ft = Formatter();
        ft.Add<uint32_t>(minutesRemaining);
        
        ContextShowError(STR_TIME_REMAINING_FORMAT, kStringIdNone, ft, true);
    }

} // namespace OpenRCT2::Competition
