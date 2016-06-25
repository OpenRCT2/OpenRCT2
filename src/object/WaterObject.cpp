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
#include "WaterObject.h"

extern "C"
{
    #include "../addresses.h"
    #include "../localisation/localisation.h"
}

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
};

void WaterObject::ReadLegacy(IStream * stream)
{
    _legacyType.string_idx = stream->ReadValue<rct_string_id>();
    _legacyType.image_id = stream->ReadValue<uint32>();
    _legacyType.var_06 = stream->ReadValue<uint32>();
    _legacyType.var_0A = stream->ReadValue<uint32>();
    _legacyType.var_0E = stream->ReadValue<uint16>();

    StringTable.Read(stream, OBJ_STRING_ID_NAME);
    ImageTable.Read(stream);
}

void WaterObject::Load()
{
    _legacyType.string_idx = language_allocate_object_string(GetName());
    _legacyType.image_id = gfx_object_allocate_images(ImageTable.GetImages(), ImageTable.GetCount());
    _legacyType.var_06 = _legacyType.image_id + 1;
    _legacyType.var_0A = _legacyType.image_id + 4;

    if (RCT2_GLOBAL(0x009ADAFD, uint8) == 0)
    {
        load_palette();
        gfx_invalidate_screen();
    }
}

void WaterObject::Unload()
{
    language_free_object_string(_legacyType.string_idx);
}

const utf8 * WaterObject::GetName()
{
    return StringTable.GetString(OBJ_STRING_ID_NAME);
}
