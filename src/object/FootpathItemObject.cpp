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
#include "FootpathItemObject.h"

extern "C"
{
    #include "../drawing/drawing.h"
    #include "../localisation/localisation.h"
}

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
};

void FootpathItemObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    _legacyType.name = stream->ReadValue<rct_string_id>();
    _legacyType.image = stream->ReadValue<uint32>();
    _legacyType.path_bit.flags = stream->ReadValue<uint16>();
    _legacyType.path_bit.draw_type = stream->ReadValue<uint8>();
    _legacyType.path_bit.tool_id = stream->ReadValue<uint8>();
    _legacyType.path_bit.price = stream->ReadValue<sint16>();
    _legacyType.path_bit.scenery_tab_id = stream->ReadValue<uint8>();

    GetStringTable()->Read(context, stream, OBJ_STRING_ID_NAME);

    _sceneryTabEntry = stream->ReadValue<rct_object_entry>();

    GetImageTable()->Read(context, stream);
}

void FootpathItemObject::Load()
{
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable()->GetImages(), GetImageTable()->GetCount());

    _legacyType.path_bit.scenery_tab_id = 0xFF;
    if ((_sceneryTabEntry.flags & 0xFF) != 0xFF)
    {
        uint8 entryType, entryIndex;
        if (find_object_in_entry_group(&_sceneryTabEntry, &entryType, &entryIndex))
        {
            _legacyType.path_bit.scenery_tab_id = entryIndex;
        }
    }
}

void FootpathItemObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable()->GetCount());
}

void FootpathItemObject::DrawPreview(rct_drawpixelinfo * dpi) const
{
    int x = dpi->width / 2;
    int y = dpi->height / 2;
    gfx_draw_sprite(dpi, _legacyType.image, x - 22, y - 24, 0);
}

const utf8 * FootpathItemObject::GetName() const
{
    const utf8 * name = GetStringTable()->GetString(OBJ_STRING_ID_NAME);
    return name != nullptr ? name : "";
}
