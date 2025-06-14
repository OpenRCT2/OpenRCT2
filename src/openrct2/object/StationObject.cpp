/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StationObject.h"

#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../world/Banner.h"

using namespace OpenRCT2;

void StationObject::Load()
{
    GetStringTable().Sort();
    NameStringId = LanguageAllocateObjectString(GetName());

    auto numImages = GetImageTable().GetCount();
    if (numImages != 0)
    {
        BaseImageId = LoadImages();

        uint32_t shelterOffset = (Flags & StationObjectFlags::isTransparent) ? 32 : 16;
        if (numImages > shelterOffset)
        {
            ShelterImageId = BaseImageId + shelterOffset;
        }
    }
}

void StationObject::Unload()
{
    LanguageFreeObjectString(NameStringId);
    UnloadImages();

    NameStringId = 0;
    BaseImageId = kImageIndexUndefined;
    ShelterImageId = kImageIndexUndefined;
}

void StationObject::DrawPreview(RenderTarget& rt, int32_t width, int32_t height) const
{
    auto screenCoords = ScreenCoordsXY{ width / 2, (height / 2) + 16 };

    auto colour0 = COLOUR_LIGHT_BLUE;
    auto colour1 = COLOUR_BORDEAUX_RED;
    auto tcolour0 = colour0;

    auto imageId = ImageId(BaseImageId);
    auto tImageId = ImageId(BaseImageId + 16).WithTransparency(tcolour0);
    if (Flags & StationObjectFlags::hasPrimaryColour)
    {
        imageId = imageId.WithPrimary(colour0);
    }
    if (Flags & StationObjectFlags::hasSecondaryColour)
    {
        imageId = imageId.WithSecondary(colour1);
    }

    GfxDrawSprite(rt, imageId, screenCoords);
    if (Flags & StationObjectFlags::isTransparent)
    {
        GfxDrawSprite(rt, tImageId, screenCoords);
    }

    GfxDrawSprite(rt, imageId.WithIndexOffset(4), screenCoords);
    if (Flags & StationObjectFlags::isTransparent)
    {
        GfxDrawSprite(rt, tImageId.WithIndexOffset(4), screenCoords);
    }
}

void StationObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "StationObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];

    if (properties.is_object())
    {
        Height = Json::GetNumber<int32_t>(properties["height"]);
        ScrollingMode = Json::GetNumber<uint8_t>(properties["scrollingMode"], kScrollingModeNone);
        Flags = Json::GetFlags<uint32_t>(
            properties,
            {
                { "hasPrimaryColour", StationObjectFlags::hasPrimaryColour },
                { "hasSecondaryColour", StationObjectFlags::hasSecondaryColour },
                { "isTransparent", StationObjectFlags::isTransparent },
                { "noPlatforms", StationObjectFlags::noPlatforms },
                { "hasShelter", StationObjectFlags::hasShelter },
            });
    }

    PopulateTablesFromJson(context, root);
}
