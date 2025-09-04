/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapSelection.h"

#include "../interface/Viewport.h"
#include "Map.h"

uint16_t gMapSelectFlags;
uint16_t gMapSelectType;
CoordsXY gMapSelectPositionA;
CoordsXY gMapSelectPositionB;
CoordsXYZ gMapSelectArrowPosition;
uint8_t gMapSelectArrowDirection;

std::vector<CoordsXY> gMapSelectionTiles;

/**
 *
 *  rct2: 0x0068AB1B
 */
void MapInvalidateMapSelectionTiles()
{
    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT))
        return;

    for (const auto& position : gMapSelectionTiles)
        MapInvalidateTileFull(position);
}

/**
 *
 *  rct2: 0x0068AAE1
 */
void MapInvalidateSelectionRect()
{
    int32_t x0, y0, x1, y1, left, right, top, bottom;

    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
        return;

    x0 = gMapSelectPositionA.x + 16;
    y0 = gMapSelectPositionA.y + 16;
    x1 = gMapSelectPositionB.x + 16;
    y1 = gMapSelectPositionB.y + 16;
    MapGetBoundingBox({ x0, y0, x1, y1 }, &left, &top, &right, &bottom);
    left -= 32;
    right += 32;
    bottom += 32;
    top -= 32 + 2080;

    OpenRCT2::ViewportsInvalidate({ { left, top }, { right, bottom } });
}
