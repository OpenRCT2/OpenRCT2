#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
#include "../core/Memory.hpp"
#include "BannerObject.h"

extern "C"
{
    #include "../drawing/drawing.h"
    #include "../localisation/localisation.h"
}

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
};

void BannerObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    _legacyType.name = stream->ReadValue<rct_string_id>();
    _legacyType.image = stream->ReadValue<uint32>();

    _legacyType.banner.scrolling_mode = stream->ReadValue<uint8>();
    _legacyType.banner.flags = stream->ReadValue<uint8>();
    _legacyType.banner.price = stream->ReadValue<uint8>();
    _legacyType.banner.scenery_tab_id = stream->ReadValue<uint8>();

    GetStringTable()->Read(context, stream, OBJ_STRING_ID_NAME);

    _sceneryTabEntry = stream->ReadValue<rct_object_entry>();

    GetImageTable()->Read(context, stream);
}

void BannerObject::Load()
{
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable()->GetImages(), GetImageTable()->GetCount());

    _legacyType.banner.scenery_tab_id = 0xFF;
    if ((_sceneryTabEntry.flags & 0xFF) != 0xFF)
    {
        uint8 entryType, entryIndex;
        if (find_object_in_entry_group(&_sceneryTabEntry, &entryType, &entryIndex))
        {
            _legacyType.banner.scenery_tab_id = entryIndex;
        }
    }
}

void BannerObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable()->GetCount());
}

void BannerObject::DrawPreview(rct_drawpixelinfo * dpi) const
{
    int x = dpi->width / 2;
    int y = dpi->height / 2;

    uint32 imageId = 0x20D00000 | _legacyType.image;
    gfx_draw_sprite(dpi, imageId + 0, x - 12, y + 8, 0);
    gfx_draw_sprite(dpi, imageId + 1, x - 12, y + 8, 0);
}

const utf8 * BannerObject::GetName() const
{
    const utf8 * name = GetStringTable()->GetString(OBJ_STRING_ID_NAME);
    return name != nullptr ? name : "";
}
