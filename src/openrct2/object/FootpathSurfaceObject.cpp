/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathSurfaceObject.h"

#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../drawing/Image.h"
#include "../object/ObjectRepository.h"

void FootpathSurfaceObject::Load()
{
    GetStringTable().Sort();
    NameStringId = LanguageAllocateObjectString(GetName());

    auto numImages = GetImageTable().GetCount();
    if (numImages != 0)
    {
        PreviewImageId = GfxObjectAllocateImages(GetImageTable().GetImages(), GetImageTable().GetCount());
        BaseImageId = PreviewImageId + 1;
    }

    _descriptor.Name = NameStringId;
    _descriptor.Image = BaseImageId;
    _descriptor.PreviewImage = PreviewImageId;
    _descriptor.Flags = Flags;
}

void FootpathSurfaceObject::Unload()
{
    LanguageFreeObjectString(NameStringId);
    GfxObjectFreeImages(PreviewImageId, GetImageTable().GetCount());

    NameStringId = 0;
    PreviewImageId = 0;
    BaseImageId = 0;
}

void FootpathSurfaceObject::DrawPreview(DrawPixelInfo* dpi, int32_t width, int32_t height) const
{
    auto screenCoords = ScreenCoordsXY{ width / 2 - 16, height / 2 };
    GfxDrawSprite(dpi, ImageId(BaseImageId + 3), screenCoords);
    GfxDrawSprite(dpi, ImageId(BaseImageId + 16), { screenCoords.x + 32, screenCoords.y - 16 });
    GfxDrawSprite(dpi, ImageId(BaseImageId + 8), { screenCoords.x + 32, screenCoords.y + 16 });
}

void FootpathSurfaceObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "FootpathSurfaceObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];
    if (properties.is_object())
    {
        Flags = Json::GetFlags<uint8_t>(
            properties,
            {
                { "editorOnly", FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR },
                { "isQueue", FOOTPATH_ENTRY_FLAG_IS_QUEUE },
                { "noSlopeRailings", FOOTPATH_ENTRY_FLAG_NO_SLOPE_RAILINGS },
            });
    }

    PopulateTablesFromJson(context, root);
}

void FootpathSurfaceObject::SetRepositoryItem(ObjectRepositoryItem* item) const
{
    item->FootpathSurfaceInfo.Flags = Flags;
}
