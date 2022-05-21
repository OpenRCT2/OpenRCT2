/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2-ui/interface/Window.h>
#include <openrct2/common.h>
#include <openrct2/sprites.h>

#define MINIMUM_TOOL_SIZE 1
#define MAXIMUM_TOOL_SIZE 64
// The highest tool size to have a sprite. Bigger tool sizes simply display a number.
#define MAX_TOOL_SIZE_WITH_SPRITE 7

extern uint16_t gLandToolSize;
extern money64 gLandToolRaiseCost;
extern money64 gLandToolLowerCost;
extern ObjectEntryIndex gLandToolTerrainSurface;
extern ObjectEntryIndex gLandToolTerrainEdge;
extern money64 gWaterToolRaiseCost;
extern money64 gWaterToolLowerCost;

namespace LandTool
{
    uint32_t SizeToSpriteIndex(uint16_t size);
    void ShowSurfaceStyleDropdown(rct_window* w, rct_widget* widget, ObjectEntryIndex currentSurfaceType);
    void ShowEdgeStyleDropdown(rct_window* w, rct_widget* widget, ObjectEntryIndex currentEdgeType);
} // namespace LandTool
