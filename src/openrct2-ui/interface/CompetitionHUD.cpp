/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CompetitionHUD.h"

#include <openrct2/Context.h>
#include <openrct2/competition/CompetitionManager.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/Network.h>

namespace OpenRCT2::Ui
{
    static constexpr int32_t kHudX = 10;
    static constexpr int32_t kHudY = 50;
    static constexpr int32_t kHudWidth = 200;
    static constexpr int32_t kHudLineHeight = 14;

    void DrawCompetitionHUD(DrawPixelInfo& dpi)
    {
        auto* ctx = GetContext();
        auto* competitionMgr = ctx->GetCompetitionManager();

        if (!competitionMgr || !competitionMgr->IsActive())
            return;

        auto screenCoords = ScreenCoordsXY{ kHudX, kHudY };

        // Draw semi-transparent background
        GfxFillRectInset(
            dpi, { screenCoords, screenCoords + ScreenCoordsXY{ kHudWidth, kHudLineHeight * 4 } }, 
            COLOUR_BLACK, INSET_RECT_FLAG_FILL_NONE);

        screenCoords.y += 2;

        // Competition Type
        auto ft = Formatter();
        const char* typeName = "Competition";
        switch (competitionMgr->GetType())
        {
            case Competition::CompetitionType::ParkValue:
                typeName = "Park Value Race";
                break;
            case Competition::CompetitionType::GuestCount:
                typeName = "Guest Count Challenge";
                break;
            case Competition::CompetitionType::RideIncome:
                typeName = "Ride Income Master";
                break;
            case Competition::CompetitionType::FastestGoal:
                typeName = "Speed Challenge";
                break;
            case Competition::CompetitionType::Survival:
                typeName = "Survival Mode";
                break;
            case Competition::CompetitionType::RideMaster:
                typeName = "Ride Master";
                break;
            default:
                break;
        }
        ft.Add<const char*>(typeName);
        DrawTextBasic(dpi, screenCoords, STR_STRING, ft, { COLOUR_WHITE });

        screenCoords.y += kHudLineHeight;

        // Time Remaining
        uint32_t timeRemaining = competitionMgr->GetTimeRemaining();
        uint32_t minutes = timeRemaining / 60;
        uint32_t seconds = timeRemaining % 60;
        
        ft = Formatter();
        ft.Add<uint32_t>(minutes);
        ft.Add<uint32_t>(seconds);
        DrawTextBasic(dpi, screenCoords, STR_TIME_REMAINING_FORMAT, ft, { COLOUR_WHITE });

        screenCoords.y += kHudLineHeight;

        // Player's Rank and Score
#ifndef DISABLE_NETWORK
        auto playerId = Network::GetCurrentPlayerId();
        auto* playerData = competitionMgr->GetPlayerData(playerId);

        if (playerData)
        {
            // Rank
            ft = Formatter();
            ft.Add<uint32_t>(playerData->Rank);
            DrawTextBasic(dpi, screenCoords, STR_YOUR_RANK_FORMAT, ft, { COLOUR_YELLOW });

            screenCoords.y += kHudLineHeight;

            // Score
            ft = Formatter();
            ft.Add<uint32_t>(playerData->Stats.Score);
            DrawTextBasic(dpi, screenCoords, STR_YOUR_SCORE_FORMAT, ft, { COLOUR_YELLOW });
        }
#endif
    }

} // namespace OpenRCT2::Ui
