/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TerritoryRenderer.h"

#include <openrct2/Context.h>
#include <openrct2/competition/CompetitionManager.h>
#include <openrct2/competition/PlayerTerritory.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/world/Map.h>

namespace OpenRCT2::Ui
{
    // Player colors for territories
    static constexpr uint8_t kPlayerColours[] = {
        COLOUR_BRIGHT_RED,    // Player 1
        COLOUR_BRIGHT_BLUE,   // Player 2
        COLOUR_BRIGHT_GREEN,  // Player 3
        COLOUR_YELLOW,        // Player 4
        COLOUR_BRIGHT_PURPLE, // Player 5
        COLOUR_ORANGE,        // Player 6
        COLOUR_LIGHT_BLUE,    // Player 7
        COLOUR_BRIGHT_PINK,   // Player 8
    };

    uint8_t GetPlayerTerritoryColour(uint8_t playerId)
    {
        if (playerId == 0 || playerId > 8)
            return COLOUR_GREY;

        return kPlayerColours[playerId - 1];
    }

    void DrawTerritoryBoundaries(DrawPixelInfo& dpi, const ScreenCoordsXY& screenPos)
    {
        auto* ctx = GetContext();
        auto* competitionMgr = ctx->GetCompetitionManager();

        if (!competitionMgr || !competitionMgr->IsActive())
            return;

        auto& territoryMgr = competitionMgr->GetTerritoryManager();
        auto playerCount = territoryMgr.GetTerritoryCount();

        if (playerCount == 0)
            return;

        // Draw boundaries for each territory
        for (uint8_t playerId = 1; playerId <= playerCount; playerId++)
        {
            auto* territory = territoryMgr.GetTerritory(playerId);
            if (!territory || !territory->HasBounds())
                continue;

            auto bounds = territory->GetBounds();
            uint8_t colour = GetPlayerTerritoryColour(playerId);

            // Draw border lines (top, right, bottom, left)
            // Top line
            for (int32_t x = bounds.TopLeft.x; x <= bounds.BottomRight.x; x++)
            {
                auto tileCoord = TileCoordsXY{ x, bounds.TopLeft.y };
                auto worldCoord = tileCoord.ToCoordsXY();
                
                // Convert world to screen coordinates
                // Note: This is simplified - actual implementation needs proper coordinate transformation
                // GfxDrawLine(dpi, ..., colour);
            }

            // Bottom line
            for (int32_t x = bounds.TopLeft.x; x <= bounds.BottomRight.x; x++)
            {
                auto tileCoord = TileCoordsXY{ x, bounds.BottomRight.y };
                // Draw line
            }

            // Left line
            for (int32_t y = bounds.TopLeft.y; y <= bounds.BottomRight.y; y++)
            {
                auto tileCoord = TileCoordsXY{ bounds.TopLeft.x, y };
                // Draw line
            }

            // Right line
            for (int32_t y = bounds.TopLeft.y; y <= bounds.BottomRight.y; y++)
            {
                auto tileCoord = TileCoordsXY{ bounds.BottomRight.x, y };
                // Draw line
            }
        }
    }

} // namespace OpenRCT2::Ui
