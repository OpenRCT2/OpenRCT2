/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StationObject.h"

#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../world/Banner.h"

void StationObject::Load()
{
    GetStringTable().Sort();
    NameStringId = LanguageAllocateObjectString(GetName());

    auto numImages = GetImageTable().GetCount();
    if (numImages != 0)
    {
        BaseImageId = LoadImages();

        uint32_t shelterOffset = (Flags & STATION_OBJECT_FLAGS::IS_TRANSPARENT) ? 32 : 16;
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
    BaseImageId = ImageIndexUndefined;
    ShelterImageId = ImageIndexUndefined;
}

void StationObject::DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const
{
    auto screenCoords = ScreenCoordsXY{ width / 2, (height / 2) + 16 };

    auto colour0 = COLOUR_LIGHT_BLUE;
    auto colour1 = COLOUR_BORDEAUX_RED;
    auto tcolour0 = colour0;

    auto imageId = ImageId(BaseImageId);
    auto tImageId = ImageId(BaseImageId + 16).WithTransparency(tcolour0);
    if (Flags & STATION_OBJECT_FLAGS::HAS_PRIMARY_COLOUR)
    {
        imageId = imageId.WithPrimary(colour0);
    }
    if (Flags & STATION_OBJECT_FLAGS::HAS_SECONDARY_COLOUR)
    {
        imageId = imageId.WithSecondary(colour1);
    }

    GfxDrawSprite(&dpi, imageId, screenCoords);
    if (Flags & STATION_OBJECT_FLAGS::IS_TRANSPARENT)
    {
        GfxDrawSprite(&dpi, tImageId, screenCoords);
    }

    GfxDrawSprite(&dpi, imageId.WithIndexOffset(4), screenCoords);
    if (Flags & STATION_OBJECT_FLAGS::IS_TRANSPARENT)
    {
        GfxDrawSprite(&dpi, tImageId.WithIndexOffset(4), screenCoords);
    }
}

void StationObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "StationObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];

    if (properties.is_object())
    {
        Height = Json::GetNumber<int32_t>(properties["height"]);
        ScrollingMode = Json::GetNumber<uint8_t>(properties["scrollingMode"], SCROLLING_MODE_NONE);
        Flags = Json::GetFlags<uint32_t>(
            properties,
            {
                { "hasPrimaryColour", STATION_OBJECT_FLAGS::HAS_PRIMARY_COLOUR },
                { "hasSecondaryColour", STATION_OBJECT_FLAGS::HAS_SECONDARY_COLOUR },
                { "isTransparent", STATION_OBJECT_FLAGS::IS_TRANSPARENT },
                { "noPlatforms", STATION_OBJECT_FLAGS::NO_PLATFORMS },
                { "hasShelter", STATION_OBJECT_FLAGS::HAS_SHELTER },
            });
    }

    PopulateTablesFromJson(context, root);
}
