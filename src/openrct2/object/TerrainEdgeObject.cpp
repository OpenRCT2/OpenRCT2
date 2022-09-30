/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TerrainEdgeObject.h"

#include "../Context.h"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/Image.h"
#include "../localisation/Localisation.h"
#include "ObjectManager.h"

void TerrainEdgeObject::Load()
{
    GetStringTable().Sort();
    NameStringId = language_allocate_object_string(GetName());
    IconImageId = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());

    // First image is icon followed by edge images
    BaseImageId = IconImageId + 1;
}

void TerrainEdgeObject::Unload()
{
    language_free_object_string(NameStringId);
    gfx_object_free_images(IconImageId, GetImageTable().GetCount());

    NameStringId = 0;
    IconImageId = 0;
    BaseImageId = 0;
}

void TerrainEdgeObject::DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const
{
    auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };

    auto imageId = ImageId(BaseImageId + 5);
    gfx_draw_sprite(dpi, imageId, screenCoords + ScreenCoordsXY{ 8, -8 });
    gfx_draw_sprite(dpi, imageId, screenCoords + ScreenCoordsXY{ 8, 8 });
}

void TerrainEdgeObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "TerrainEdgeObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];

    if (properties.is_object())
    {
        HasDoors = Json::GetBoolean(properties["hasDoors"]);
    }

    PopulateTablesFromJson(context, root);
}

TerrainEdgeObject* TerrainEdgeObject::GetById(ObjectEntryIndex entryIndex)
{
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto* obj = objMgr.GetLoadedObject(ObjectType::TerrainEdge, entryIndex);
    return static_cast<TerrainEdgeObject*>(obj);
}
