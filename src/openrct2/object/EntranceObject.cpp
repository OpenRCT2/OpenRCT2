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

#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "EntranceObject.h"
#include "ObjectJsonHelpers.h"

void EntranceObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    _legacyType.scrolling_mode = stream->ReadValue<uint8>();
    _legacyType.text_height = stream->ReadValue<uint8>();

    GetStringTable()->Read(context, stream, OBJ_STRING_ID_NAME);
    GetImageTable()->Read(context, stream);

    // Fix issue #1705: The Medieval entrance from Time Twister has a straight banner,
    // but scrolls its text as if it a curved one.
    if (String::Equals(GetIdentifier(), "MEDIENTR"))
    {
        _legacyType.scrolling_mode = 32;
        _legacyType.text_height += 1;
    }
}

void EntranceObject::Load()
{
    GetStringTable()->Sort();
    _legacyType.string_idx = language_allocate_object_string(GetName());
    _legacyType.image_id = gfx_object_allocate_images(GetImageTable()->GetImages(), GetImageTable()->GetCount());
}

void EntranceObject::Unload()
{
    language_free_object_string(_legacyType.string_idx);
    gfx_object_free_images(_legacyType.image_id, GetImageTable()->GetCount());

    _legacyType.string_idx = 0;
    _legacyType.image_id = 0;
}

void EntranceObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    sint32 x = width / 2;
    sint32 y = height / 2;

    uint32 imageId = _legacyType.image_id;
    gfx_draw_sprite(dpi, imageId + 1, x - 32, y + 14, 0);
    gfx_draw_sprite(dpi, imageId + 0, x +  0, y + 28, 0);
    gfx_draw_sprite(dpi, imageId + 2, x + 32, y + 44, 0);
}

void EntranceObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");
    _legacyType.scrolling_mode = json_integer_value(json_object_get(properties, "scrollingMode"));
    _legacyType.text_height = json_integer_value(json_object_get(properties, "textHeight"));

    ObjectJsonHelpers::LoadStrings(root, *GetStringTable());
    ObjectJsonHelpers::LoadImages(root, *GetImageTable());
}
