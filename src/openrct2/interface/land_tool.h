#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"
#include "../sprites.h"

#define MINIMUM_TOOL_SIZE         1
#define MAXIMUM_TOOL_SIZE         64
// The highest tool size to have a sprite. Bigger tool sizes simply display a number.
#define MAX_TOOL_SIZE_WITH_SPRITE 7

extern uint16 gLandToolSize;
extern money32 gLandToolRaiseCost;
extern money32 gLandToolLowerCost;
extern uint8 gLandToolTerrainSurface;
extern uint8 gLandToolTerrainEdge;
extern money32 gWaterToolRaiseCost;
extern money32 gWaterToolLowerCost;

uint32 land_tool_size_to_sprite_index(uint16 size);
void land_tool_show_surface_style_dropdown(rct_window * w, rct_widget * widget, uint8 currentSurfaceType);
void land_tool_show_edge_style_dropdown(rct_window * w, rct_widget * widget, uint8 currentEdgeType);
