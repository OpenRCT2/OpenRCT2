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
#include "FootpathObject.h"

extern "C"
{
    #include "../drawing/drawing.h"
    #include "../localisation/localisation.h"
    #include "../world/footpath.h"
}

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
};

void FootpathObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    _legacyType.string_idx = stream->ReadValue<rct_string_id>();
    _legacyType.image = stream->ReadValue<uint32>();
    _legacyType.bridge_image = stream->ReadValue<uint32>();
    _legacyType.var_0A = stream->ReadValue<uint8>();
    _legacyType.flags = stream->ReadValue<uint8>();
    _legacyType.scrolling_mode = stream->ReadValue<uint8>();
    stream->Seek(1, STREAM_SEEK_CURRENT);

    GetStringTable()->Read(context, stream, OBJ_STRING_ID_NAME);
    GetImageTable()->Read(context, stream);
}

void FootpathObject::Load()
{
    _legacyType.string_idx = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable()->GetImages(), GetImageTable()->GetCount());
    _legacyType.bridge_image = _legacyType.image + 109;
}

void FootpathObject::Unload()
{
    language_free_object_string(_legacyType.string_idx);
    gfx_object_free_images(_legacyType.image, GetImageTable()->GetCount());
}

const utf8 * FootpathObject::GetName() const
{
    const utf8 * name = GetStringTable()->GetString(OBJ_STRING_ID_NAME);
    return name != nullptr ? name : "";
}
