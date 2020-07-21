/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TerrainEdgeObject.h"

#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "ObjectJsonHelpers.h"

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

    uint32_t imageId = BaseImageId;
    gfx_draw_sprite(dpi, imageId + 5, screenCoords + ScreenCoordsXY{ 8, -8 }, 0);
    gfx_draw_sprite(dpi, imageId + 5, screenCoords + ScreenCoordsXY{ 8, 8 }, 0);
}

void TerrainEdgeObject::ReadJson(IReadObjectContext* context, const json_t* root)
{
    auto properties = json_object_get(root, "properties");
    HasDoors = ObjectJsonHelpers::GetBoolean(properties, "hasDoors", false);

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
    NumImagesLoaded = GetImageTable().GetCount();
}
