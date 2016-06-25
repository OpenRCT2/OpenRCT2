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
#include "WallObject.h"

extern "C"
{
    #include "../drawing/drawing.h"
    #include "../localisation/localisation.h"
}

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
};

void WallObject::ReadLegacy(IStream * stream)
{
    _legacyType.name = stream->ReadValue<rct_string_id>();
    _legacyType.image = stream->ReadValue<uint32>();

    _legacyType.wall.tool_id = stream->ReadValue<uint8>();
    _legacyType.wall.flags = stream->ReadValue<uint8>();
    _legacyType.wall.height = stream->ReadValue<uint8>();
    _legacyType.wall.flags2 = stream->ReadValue<uint8>();
    _legacyType.wall.price = stream->ReadValue<uint16>();
    _legacyType.wall.scenery_tab_id = stream->ReadValue<uint8>();
    _legacyType.wall.var_0D = stream->ReadValue<uint8>();

    StringTable.Read(stream, OBJ_STRING_ID_NAME);

    _sceneryTabEntry = stream->ReadValue<rct_object_entry>();

    ImageTable.Read(stream);
}

void WallObject::Load()
{
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(ImageTable.GetImages(), ImageTable.GetCount());

    _legacyType.small_scenery.scenery_tab_id = 0xFF;
    if ((_sceneryTabEntry.flags & 0xFF) != 0xFF)
    {
        uint8 entryType, entryIndex;
        if (find_object_in_entry_group(&_sceneryTabEntry, &entryType, &entryIndex))
        {
            _legacyType.small_scenery.scenery_tab_id = entryIndex;
        }
    }
}

void WallObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, ImageTable.GetCount());
}

const utf8 * WallObject::GetName()
{
    return StringTable.GetString(OBJ_STRING_ID_NAME);
}
