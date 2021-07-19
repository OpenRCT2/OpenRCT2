/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
ObjectEntryIndex gLandToolTerrainSurface;
ObjectEntryIndex gLandToolTerrainEdge;
money32 gWaterToolRaiseCost;
money32 gWaterToolLowerCost;

uint32_t LandTool::SizeToSpriteIndex(uint16_t size)
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

void LandTool::ShowSurfaceStyleDropdown(rct_window* w, rct_widget* widget, ObjectEntryIndex currentSurfaceType)
{
    auto& objManager = GetContext()->GetObjectManager();

    auto defaultIndex = 0;
    auto itemIndex = 0;
    for (size_t i = 0; i < MAX_TERRAIN_SURFACE_OBJECTS; i++)
    {
        const auto surfaceObj = static_cast<TerrainSurfaceObject*>(objManager.GetLoadedObject(ObjectType::TerrainSurface, i));
        // NumImagesLoaded can be 1 for RCT1 surfaces if the user does not have RCT1 linked.
        if (surfaceObj != nullptr && surfaceObj->NumImagesLoaded > 1)
        {
            gDropdownItemsFormat[itemIndex] = Dropdown::FormatLandPicker;
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

    WindowDropdownShowImage(
        w->windowPos.x + widget->left, w->windowPos.y + widget->top, widget->height(), w->colours[2], 0, surfaceCount, 47, 36,
        DropdownGetAppropriateImageDropdownItemsPerRow(surfaceCount));

    gDropdownDefaultIndex = defaultIndex;
}

void LandTool::ShowEdgeStyleDropdown(rct_window* w, rct_widget* widget, ObjectEntryIndex currentEdgeType)
{
    auto& objManager = GetContext()->GetObjectManager();

    auto defaultIndex = 0;
    auto itemIndex = 0;
    for (size_t i = 0; i < MAX_TERRAIN_EDGE_OBJECTS; i++)
    {
        const auto edgeObj = static_cast<TerrainEdgeObject*>(objManager.GetLoadedObject(ObjectType::TerrainEdge, i));
        if (edgeObj != nullptr && edgeObj->NumImagesLoaded > 1)
        {
            gDropdownItemsFormat[itemIndex] = Dropdown::FormatLandPicker;
            gDropdownItemsArgs[itemIndex] = edgeObj->IconImageId;
            if (i == currentEdgeType)
            {
                defaultIndex = itemIndex;
            }
            itemIndex++;
        }
    }
    uint32_t edgeCount = itemIndex;
    auto itemsPerRow = DropdownGetAppropriateImageDropdownItemsPerRow(edgeCount);

    WindowDropdownShowImage(
        w->windowPos.x + widget->left, w->windowPos.y + widget->top, widget->height(), w->colours[2], 0, edgeCount, 47, 36,
        itemsPerRow);

    gDropdownDefaultIndex = defaultIndex;
}
