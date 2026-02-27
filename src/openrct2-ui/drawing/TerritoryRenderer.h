/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/drawing/Drawing.h>
#include <openrct2/world/Location.hpp>

namespace OpenRCT2::Ui
{
    /**
     * Draws territory boundaries for all players in competition mode
     */
    void DrawTerritoryBoundaries(DrawPixelInfo& dpi, const ScreenCoordsXY& screenPos);

    /**
     * Gets the color for a player's territory
     */
    uint8_t GetPlayerTerritoryColour(uint8_t playerId);

} // namespace OpenRCT2::Ui
