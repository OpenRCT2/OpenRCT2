/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Surface.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2/Input.h>
#include <openrct2/drawing/Drawing.h>

// clang-format off
static uint16_t toolSizeSpriteIndices[] =
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

static uint32_t FloorTextureOrder[] =
{
    TERRAIN_SAND_DARK, TERRAIN_SAND_LIGHT,  TERRAIN_DIRT,      TERRAIN_GRASS_CLUMPS, TERRAIN_GRASS,
    TERRAIN_ROCK,      TERRAIN_SAND,        TERRAIN_MARTIAN,   TERRAIN_CHECKERBOARD, TERRAIN_ICE,
    TERRAIN_GRID_RED,  TERRAIN_GRID_YELLOW, TERRAIN_GRID_BLUE, TERRAIN_GRID_GREEN
};

uint32_t WallTextureOrder[] =
{
    TERRAIN_EDGE_ROCK,         TERRAIN_EDGE_WOOD_RED,    TERRAIN_EDGE_WOOD_BLACK, TERRAIN_EDGE_ICE,          TERRAIN_EDGE_BRICK,
    TERRAIN_EDGE_GREY,         TERRAIN_EDGE_YELLOW,      TERRAIN_EDGE_RED,        TERRAIN_EDGE_PURPLE,       TERRAIN_EDGE_GREEN,
    TERRAIN_EDGE_IRON,         TERRAIN_EDGE_STONE_BROWN, TERRAIN_EDGE_STONE_GREY, TERRAIN_EDGE_SKYSCRAPER_A, TERRAIN_EDGE_SKYSCRAPER_B,
    0, 0
};

uint32_t WallTexturePreviews[] =
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

uint16_t gLandToolSize;
money32 gLandToolRaiseCost;
money32 gLandToolLowerCost;
uint8_t gLandToolTerrainSurface;
uint8_t gLandToolTerrainEdge;
money32 gWaterToolRaiseCost;
money32 gWaterToolLowerCost;

uint32_t land_tool_size_to_sprite_index(uint16_t size)
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

void land_tool_show_surface_style_dropdown(rct_window * w, rct_widget * widget, uint8_t currentSurfaceType)
{
    uint8_t defaultIndex = 0;

    for (uint8_t i = 0; i < TERRAIN_COUNT_REGULAR; i++)
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

void land_tool_show_edge_style_dropdown(rct_window * w, rct_widget * widget, uint8_t currentEdgeType)
{
    uint8_t defaultIndex = 0;
    // Do not show RCT1 edge styles if the player does not have RCT1.
    const uint8_t edgeCount = is_csg_loaded() ? TERRAIN_EDGE_COUNT : TERRAIN_EDGE_RCT2_COUNT;

    for (uint8_t i = 0; i < edgeCount; i++) {
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
