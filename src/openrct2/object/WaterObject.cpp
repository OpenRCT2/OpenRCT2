#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma warning(disable : 4706) // assignment within conditional expression

#include <memory>
#include "../core/IStream.hpp"
#include "../localisation/Language.h"
#include "../localisation/StringIds.h"
#include "../OpenRCT2.h"
#include "ObjectJsonHelpers.h"
#include "WaterObject.h"

void WaterObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(14, STREAM_SEEK_CURRENT);
    _legacyType.flags = stream->ReadValue<uint16>();

    GetStringTable().Read(context, stream, OBJ_STRING_ID_NAME);
    GetImageTable().Read(context, stream);
}

void WaterObject::Load()
{
    GetStringTable().Sort();
    _legacyType.string_idx = language_allocate_object_string(GetName());
    _legacyType.image_id = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
    _legacyType.palette_index_1 = _legacyType.image_id + 1;
    _legacyType.palette_index_2 = _legacyType.image_id + 4;

    load_palette();
}

void WaterObject::Unload()
{
    gfx_object_free_images(_legacyType.image_id, GetImageTable().GetCount());
    language_free_object_string(_legacyType.string_idx);

    _legacyType.string_idx = 0;
}

void WaterObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    // Write (no image)
    sint32 x = width / 2;
    sint32 y = height / 2;
    gfx_draw_string_centred(dpi, STR_WINDOW_NO_IMAGE, x, y, COLOUR_BLACK, nullptr);
}

void WaterObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");
    _legacyType.flags = ObjectJsonHelpers::GetFlags<uint16>(properties, {
        { "allowDucks", WATER_FLAGS_ALLOW_DUCKS }});

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());

    // Images which are actually palette data
    static const char * paletteNames[] =
    {
        "general",
        "waves-0",
        "waves-1",
        "waves-2",
        "sparkles-0",
        "sparkles-1",
        "sparkles-2"
    };
    for (auto paletteName : paletteNames)
    {
        auto jPalettes = json_object_get(properties, "palettes");
        if (jPalettes != nullptr)
        {
            auto jPalette = json_object_get(jPalettes, paletteName);
            if (jPalette != nullptr)
            {
                ReadJsonPalette(jPalette);
            }
        }
    }
}

void WaterObject::ReadJsonPalette(const json_t * jPalette)
{
    auto paletteStartIndex = json_integer_value(json_object_get(jPalette, "index"));
    auto jColours = json_object_get(jPalette, "colours");
    auto numColours = json_array_size(jColours);

    auto data = std::make_unique<uint8[]>(numColours);
    size_t dataIndex = 0;

    size_t index;
    const json_t * jColour;
    json_array_foreach(jColours, index, jColour)
    {
        auto szColour = json_string_value(jColour);
        if (szColour != nullptr)
        {
            auto colour = ParseColour(szColour);
            data[dataIndex + 0] = colour;
            data[dataIndex + 1] = (colour >> 8) & 0xFF;
            data[dataIndex + 2] = (colour >> 16) & 0xFF;
        }
        dataIndex += 3;
    }

    rct_g1_element g1 = { 0 };
    g1.offset = data.get();
    g1.x_offset = (sint16)paletteStartIndex;

    auto &imageTable = GetImageTable();
    imageTable.AddImage(&g1);
}

uint32 WaterObject::ParseColour(const std::string &s) const
{
    uint8 r = 0;
    uint8 g = 0;
    uint8 b = 0;
    if (s[0] == '#' && s.size() == 7)
    {
        // Expect #RRGGBB
        r = std::stoul(s.substr(1, 2), nullptr, 16) & 0xFF;
        g = std::stoul(s.substr(3, 2), nullptr, 16) & 0xFF;
        b = std::stoul(s.substr(5, 2), nullptr, 16) & 0xFF;
    }
    return (b << 16) | (g << 8) | r;
}
