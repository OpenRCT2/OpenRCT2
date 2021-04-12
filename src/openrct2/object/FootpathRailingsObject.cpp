/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathRailingsObject.h"

#include "../core/IStream.hpp"
#include "../core/Json.hpp"

void FootpathRailingsObject::Load()
{
    GetStringTable().Sort();
    NameStringId = language_allocate_object_string(GetName());

    auto numImages = GetImageTable().GetCount();
    if (numImages != 0)
    {
        PreviewImageId = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
        BridgeImageId = PreviewImageId + 37;
        RailingsImageId = PreviewImageId + 1;
    }
}

void FootpathRailingsObject::Unload()
{
    language_free_object_string(NameStringId);
    gfx_object_free_images(PreviewImageId, GetImageTable().GetCount());

    NameStringId = 0;
    PreviewImageId = 0;
    BridgeImageId = 0;
    RailingsImageId = 0;
}

void FootpathRailingsObject::DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const
{
    auto screenCoords = ScreenCoordsXY{ (width - 45) / 2, (height - 34) / 2 };
    gfx_draw_sprite(dpi, PreviewImageId, screenCoords, 0);
}

void FootpathRailingsObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "FootpathObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];
    if (properties.is_object())
    {
        SupportType = ParseSupportType(Json::GetString(properties["supportType"]));
        ScrollingMode = Json::GetNumber<uint8_t>(properties["scrollingMode"]);
        Flags = Json::GetFlags<uint8_t>(
            properties,
            {
                { "hasSupportImages", RAILING_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE },
                { "hasElevatedPathImages", RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS },
            });
    }

    PopulateTablesFromJson(context, root);
}

RailingEntrySupportType FootpathRailingsObject::ParseSupportType(std::string_view s)
{
    if (s == "pole")
        return RailingEntrySupportType::Pole;
    else /* if (s == "box") */
        return RailingEntrySupportType::Box;
}
