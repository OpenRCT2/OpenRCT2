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
#include "WallObject.h"

#include "../drawing/Drawing.h"
#include "../localisation/Language.h"

void WallObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    _legacyType.wall.tool_id = stream->ReadValue<uint8>();
    _legacyType.wall.flags = stream->ReadValue<uint8>();
    _legacyType.wall.height = stream->ReadValue<uint8>();
    _legacyType.wall.flags2 = stream->ReadValue<uint8>();
    _legacyType.wall.price = stream->ReadValue<uint16>();
    _legacyType.wall.scenery_tab_id = stream->ReadValue<uint8>();
    _legacyType.wall.scrolling_mode = stream->ReadValue<uint8>();

    GetStringTable().Read(context, stream, OBJ_STRING_ID_NAME);

    rct_object_entry sgEntry = stream->ReadValue<rct_object_entry>();
    SetPrimarySceneryGroup(&sgEntry);

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.wall.price <= 0)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Price can not be free or negative.");
    }
}

void WallObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
}

void WallObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable().GetCount());

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void WallObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    sint32 x = width / 2;
    sint32 y = height / 2;

    x += 14;
    y += (_legacyType.wall.height * 2) + 16;

    uint32 imageId = 0x20D00000 | _legacyType.image;
    if (_legacyType.wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
    {
        imageId |= 0x92000000;
    }

    gfx_draw_sprite(dpi, imageId, x, y, 0);

    if (_legacyType.wall.flags & WALL_SCENERY_HAS_GLASS)
    {
        imageId = _legacyType.image + 0x44500006;
        gfx_draw_sprite(dpi, imageId, x, y, 0);
    }
    else if (_legacyType.wall.flags & WALL_SCENERY_IS_DOOR)
    {
        imageId++;
        gfx_draw_sprite(dpi, imageId, x, y, 0);
    }
}
