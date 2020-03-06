/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Surface.h>

using namespace OpenRCT2;

// clang-format off
static uint16_t toolSizeSpriteIndices[] =
{
    SPR_LAND_TOOL_SIZE_0,
    SPR_LAND_TOOL_SIZE_1,
    SPR_LAND_TOOL_SIZE_2,
    SPR_LAND_TOOL_SIZE_3,
    SPR_LAND_TOOL_SIZE_4,
    SPR_LAND_TOOL_SIZE_5,
    SPR_G2_LAND_TOOL_SIZE_6,
    SPR_LAND_TOOL_SIZE_7,
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

void land_tool_show_surface_style_dropdown(rct_window* w, rct_widget* widget, uint8_t currentSurfaceType)
{
    auto& objManager = GetContext()->GetObjectManager();

    auto defaultIndex = 0;
    auto itemIndex = 0;
    for (size_t i = 0; i < MAX_TERRAIN_SURFACE_OBJECTS; i++)
    {
        const auto surfaceObj = static_cast<TerrainSurfaceObject*>(objManager.GetLoadedObject(OBJECT_TYPE_TERRAIN_SURFACE, i));
        if (surfaceObj != nullptr)
        {
            gDropdownItemsFormat[itemIndex] = DROPDOWN_FORMAT_LAND_PICKER;
            gDropdownItemsArgs[itemIndex] = surfaceObj->IconImageId;
            if (surfaceObj->Colour != 255)
            {
                gDropdownItemsArgs[itemIndex] |= SPRITE_ID_PALETTE_COLOUR_1(surfaceObj->Colour);
            }
            if (i == currentSurfaceType)
            {
                defaultIndex = itemIndex;
            }
            itemIndex++;
        }
    }
    uint32_t surfaceCount = itemIndex;

    window_dropdown_show_image(
        w->windowPos.x + widget->left, w->windowPos.y + widget->top, widget->bottom - widget->top, w->colours[2], 0,
        surfaceCount, 47, 36, dropdown_get_appropriate_image_dropdown_items_per_row(surfaceCount));

    gDropdownDefaultIndex = defaultIndex;
}

void land_tool_show_edge_style_dropdown(rct_window* w, rct_widget* widget, uint8_t currentEdgeType)
{
    auto& objManager = GetContext()->GetObjectManager();

    auto defaultIndex = 0;
    auto itemIndex = 0;
    for (size_t i = 0; i < MAX_TERRAIN_EDGE_OBJECTS; i++)
    {
        const auto edgeObj = static_cast<TerrainEdgeObject*>(objManager.GetLoadedObject(OBJECT_TYPE_TERRAIN_EDGE, i));
        if (edgeObj != nullptr && edgeObj->NumImagesLoaded > 1)
        {
            gDropdownItemsFormat[itemIndex] = DROPDOWN_FORMAT_LAND_PICKER;
            gDropdownItemsArgs[itemIndex] = edgeObj->IconImageId;
            if (i == currentEdgeType)
            {
                defaultIndex = itemIndex;
            }
            itemIndex++;
        }
    }
    uint32_t edgeCount = itemIndex;
    auto itemsPerRow = dropdown_get_appropriate_image_dropdown_items_per_row(edgeCount);

    window_dropdown_show_image(
        w->windowPos.x + widget->left, w->windowPos.y + widget->top, widget->bottom - widget->top, w->colours[2], 0, edgeCount,
        47, 36, itemsPerRow);

    gDropdownDefaultIndex = defaultIndex;
}
