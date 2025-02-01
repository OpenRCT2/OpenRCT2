/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Map.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui::Windows;

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
ObjectEntryIndex gLandToolTerrainSurface;
ObjectEntryIndex gLandToolTerrainEdge;

uint32_t LandTool::SizeToSpriteIndex(uint16_t size)
{
    if (size <= kLandToolMaximumSizeWithSprite)
    {
        return toolSizeSpriteIndices[size];
    }

    return 0xFFFFFFFF;
}

void LandTool::ShowSurfaceStyleDropdown(WindowBase* w, Widget* widget, ObjectEntryIndex currentSurfaceType)
{
    auto& objManager = GetContext()->GetObjectManager();

    auto defaultIndex = 0;
    auto itemIndex = 0;
    for (size_t i = 0; i < kMaxTerrainSurfaceObjects; i++)
    {
        const auto surfaceObj = objManager.GetLoadedObject<TerrainSurfaceObject>(i);
        // If fallback images are loaded, the RCT1 styles will just look like copies of already existing styles, so hide them.
        if (surfaceObj != nullptr && !surfaceObj->UsesFallbackImages())
        {
            auto imageId = ImageId(surfaceObj->IconImageId);
            if (surfaceObj->Colour != TerrainSurfaceObject::kNoValue)
                imageId = imageId.WithPrimary(surfaceObj->Colour);

            gDropdownItems[itemIndex].Format = Dropdown::kFormatLandPicker;
            Dropdown::SetImage(itemIndex, imageId);
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

ObjectEntryIndex LandTool::GetSurfaceStyleFromDropdownIndex(size_t index)
{
    auto& objManager = GetContext()->GetObjectManager();
    auto itemIndex = 0U;
    for (size_t i = 0; i < kMaxTerrainSurfaceObjects; i++)
    {
        const auto surfaceObj = objManager.GetLoadedObject<TerrainSurfaceObject>(i);
        // If fallback images are loaded, the RCT1 styles will just look like copies of already existing styles, so hide them.
        if (surfaceObj != nullptr && !surfaceObj->UsesFallbackImages())
        {
            if (itemIndex == index)
            {
                return static_cast<ObjectEntryIndex>(i);
            }
            itemIndex++;
        }
    }
    return kObjectEntryIndexNull;
}

void LandTool::ShowEdgeStyleDropdown(WindowBase* w, Widget* widget, ObjectEntryIndex currentEdgeType)
{
    auto& objManager = GetContext()->GetObjectManager();

    auto defaultIndex = 0;
    auto itemIndex = 0;
    for (size_t i = 0; i < kMaxTerrainEdgeObjects; i++)
    {
        const auto edgeObj = objManager.GetLoadedObject<TerrainEdgeObject>(i);
        // If fallback images are loaded, the RCT1 styles will just look like copies of already existing styles, so hide them.
        if (edgeObj != nullptr && !edgeObj->UsesFallbackImages())
        {
            gDropdownItems[itemIndex].Format = Dropdown::kFormatLandPicker;
            Dropdown::SetImage(itemIndex, ImageId(edgeObj->IconImageId));
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

ObjectEntryIndex LandTool::GetEdgeStyleFromDropdownIndex(size_t index)
{
    auto& objManager = GetContext()->GetObjectManager();
    auto itemIndex = 0U;
    for (size_t i = 0; i < kMaxTerrainEdgeObjects; i++)
    {
        const auto edgeObj = objManager.GetLoadedObject<TerrainEdgeObject>(i);
        // If fallback images are loaded, the RCT1 styles will just look like copies of already existing styles, so hide them.
        if (edgeObj != nullptr && !edgeObj->UsesFallbackImages())
        {
            if (itemIndex == index)
            {
                return static_cast<ObjectEntryIndex>(i);
            }
            itemIndex++;
        }
    }
    return kObjectEntryIndexNull;
}
