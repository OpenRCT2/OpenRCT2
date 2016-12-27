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
    #include "../localisation/localisation.h"
}

void WaterObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    _legacyType.var_06 = stream->ReadValue<uint32>();
    _legacyType.var_0A = stream->ReadValue<uint32>();
    _legacyType.var_0E = stream->ReadValue<uint16>();

    GetStringTable()->Read(context, stream, OBJ_STRING_ID_NAME);
    GetImageTable()->Read(context, stream);
}

void WaterObject::Load()
{
    GetStringTable()->Sort();
    _legacyType.string_idx = language_allocate_object_string(GetName());
    _legacyType.image_id = gfx_object_allocate_images(GetImageTable()->GetImages(), GetImageTable()->GetCount());
    _legacyType.var_06 = _legacyType.image_id + 1;
    _legacyType.var_0A = _legacyType.image_id + 4;

    load_palette();
    gfx_invalidate_screen();
}

void WaterObject::Unload()
{
    gfx_object_free_images(_legacyType.image_id, GetImageTable()->GetCount());
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
