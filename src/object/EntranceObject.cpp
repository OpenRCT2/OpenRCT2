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
#include "EntranceObject.h"

extern "C"
{
    #include "../drawing/drawing.h"
    #include "../localisation/localisation.h"
}

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
};

void EntranceObject::ReadLegacy(IStream * stream)
{
    _legacyType.string_idx = stream->ReadValue<rct_string_id>();
    _legacyType.image_id = stream->ReadValue<uint32>();
    _legacyType.scrolling_mode = stream->ReadValue<uint8>();
    _legacyType.text_height = stream->ReadValue<uint8>();

    GetStringTable().Read(stream, OBJ_STRING_ID_NAME);
    GetImageTable().Read(stream);
}

void EntranceObject::Load()
{
    _legacyType.string_idx = language_allocate_object_string(GetName());
    _legacyType.image_id = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
}

void EntranceObject::Unload()
{
    language_free_object_string(_legacyType.string_idx);
    gfx_object_free_images(_legacyType.image_id, GetImageTable().GetCount());
}

const utf8 * EntranceObject::GetName()
{
    return GetStringTable().GetString(OBJ_STRING_ID_NAME);
}
