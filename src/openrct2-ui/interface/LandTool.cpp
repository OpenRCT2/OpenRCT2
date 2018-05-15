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

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Surface.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2/Input.h>
#include <openrct2/drawing/Drawing.h>

// clang-format off
static uint16 toolSizeSpriteIndices[] =
{
    SPR_LAND_TOOL_SIZE_0,
    SPR_LAND_TOOL_SIZE_1,
    SPR_LAND_TOOL_SIZE_2,
    SPR_LAND_TOOL_SIZE_3,
    SPR_LAND_TOOL_SIZE_4,
    SPR_LAND_TOOL_SIZE_5,
    SPR_LAND_TOOL_SIZE_6,
    SPR_LAND_TOOL_SIZE_7,
};

static uint32 FloorTextureOrder[] =
{
    TERRAIN_SAND_DARK, TERRAIN_SAND_LIGHT,  TERRAIN_DIRT,      TERRAIN_GRASS_CLUMPS, TERRAIN_GRASS,
    TERRAIN_ROCK,      TERRAIN_SAND,        TERRAIN_MARTIAN,   TERRAIN_CHECKERBOARD, TERRAIN_ICE,
    TERRAIN_GRID_RED,  TERRAIN_GRID_YELLOW, TERRAIN_GRID_BLUE, TERRAIN_GRID_GREEN
};

uint32 WallTextureOrder[] =
{
    TERRAIN_EDGE_ROCK,         TERRAIN_EDGE_WOOD_RED,    TERRAIN_EDGE_WOOD_BLACK, TERRAIN_EDGE_ICE,          TERRAIN_EDGE_BRICK,
    TERRAIN_EDGE_GREY,         TERRAIN_EDGE_YELLOW,      TERRAIN_EDGE_RED,        TERRAIN_EDGE_PURPLE,       TERRAIN_EDGE_GREEN,
    TERRAIN_EDGE_IRON,         TERRAIN_EDGE_STONE_BROWN, TERRAIN_EDGE_STONE_GREY, TERRAIN_EDGE_SKYSCRAPER_A, TERRAIN_EDGE_SKYSCRAPER_B,
    0, 0
};

uint32 WallTexturePreviews[] =
{
    SPR_WALL_TEXTURE_ROCK,
    SPR_WALL_TEXTURE_WOOD_RED,
    SPR_WALL_TEXTURE_WOOD_BLACK,
    SPR_WALL_TEXTURE_ICE,
    SPR_G2_WALL_TEXTURE_BRICK,
    SPR_G2_WALL_TEXTURE_IRON,
    SPR_G2_WALL_TEXTURE_GREY,
    SPR_G2_WALL_TEXTURE_YELLOW,
    SPR_G2_WALL_TEXTURE_RED,
    SPR_G2_WALL_TEXTURE_PURPLE,
    SPR_G2_WALL_TEXTURE_GREEN,
    SPR_G2_WALL_TEXTURE_STONE_BROWN,
    SPR_G2_WALL_TEXTURE_STONE_GREY,
    SPR_G2_WALL_TEXTURE_SKYSCRAPER_A,
    SPR_G2_WALL_TEXTURE_SKYSCRAPER_B,
};
// clang-format on

uint16 gLandToolSize;
money32 gLandToolRaiseCost;
money32 gLandToolLowerCost;
uint8 gLandToolTerrainSurface;
uint8 gLandToolTerrainEdge;
money32 gWaterToolRaiseCost;
money32 gWaterToolLowerCost;

uint32 land_tool_size_to_sprite_index(uint16 size)
{
    if (size <= MAX_TOOL_SIZE_WITH_SPRITE)
    {
        return toolSizeSpriteIndices[size];
    }
    else
    {
        return 0xFFFFFFFF;
    }
}

void land_tool_show_surface_style_dropdown(rct_window * w, rct_widget * widget, uint8 currentSurfaceType)
{
    uint8 defaultIndex = 0;

    for (uint8 i = 0; i < TERRAIN_COUNT_REGULAR; i++)
    {
        gDropdownItemsFormat[i] = DROPDOWN_FORMAT_LAND_PICKER;
        gDropdownItemsArgs[i] = SPR_FLOOR_TEXTURE_GRASS + FloorTextureOrder[i];
        if (FloorTextureOrder[i] == currentSurfaceType)
        {
            defaultIndex = i;
        }
    }

    window_dropdown_show_image(
       w->x + widget->left, w->y + widget->top,
       widget->bottom - widget->top,
       w->colours[2],
       0,
       TERRAIN_COUNT_REGULAR,
       47, 36,
       gAppropriateImageDropdownItemsPerRow[TERRAIN_COUNT_REGULAR]
    );

    gDropdownDefaultIndex = defaultIndex;
}

void land_tool_show_edge_style_dropdown(rct_window * w, rct_widget * widget, uint8 currentEdgeType)
{
    uint8 defaultIndex = 0;
    // Do not show RCT1 edge styles if the player does not have RCT1.
    const uint8 edgeCount = is_csg_loaded() ? TERRAIN_EDGE_COUNT : TERRAIN_EDGE_RCT2_COUNT;

    for (uint8 i = 0; i < edgeCount; i++) {
        gDropdownItemsFormat[i] = DROPDOWN_FORMAT_LAND_PICKER;
        gDropdownItemsArgs[i] = WallTexturePreviews[WallTextureOrder[i]];
        if (WallTextureOrder[i] == currentEdgeType)
            defaultIndex = i;
    }

    window_dropdown_show_image(
       w->x + widget->left, w->y + widget->top,
       widget->bottom - widget->top,
       w->colours[2],
       0,
       edgeCount,
       47, 36,
       gAppropriateImageDropdownItemsPerRow[edgeCount]
    );

    gDropdownDefaultIndex = defaultIndex;
}
