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
#include "SceneryGroupObject.h"

extern "C"
{
    #include "../drawing/drawing.h"
    #include "../localisation/localisation.h"
}

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
};

SceneryGroupObject::~SceneryGroupObject()
{
    Memory::Free(_items);
}

void SceneryGroupObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    _legacyType.name = stream->ReadValue<rct_string_id>();
    _legacyType.image = stream->ReadValue<uint32>();
    stream->Seek(0x80 * 2, STREAM_SEEK_CURRENT);
    _legacyType.entry_count = stream->ReadValue<uint8>();
    _legacyType.var_107 = stream->ReadValue<uint8>();
    _legacyType.var_108 = stream->ReadValue<uint8>();
    _legacyType.pad_109 = stream->ReadValue<uint8>();
    _legacyType.var_10A = stream->ReadValue<uint32>();

    GetStringTable()->Read(context, stream, OBJ_STRING_ID_NAME);
    ReadItems(stream);
    GetImageTable()->Read(context, stream);

    _legacyType.var_107 = _numItems;
}

void SceneryGroupObject::Load()
{
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable()->GetImages(), GetImageTable()->GetCount());

    _legacyType.entry_count = 0;
    for (uint32 i = 0; i < _numItems; i++)
    {
        uint8 entryType;
        uint8 entryIndex;
        if (find_object_in_entry_group(&_items[i], &entryType, &entryIndex))
        {
            uint16 sceneryEntry = entryIndex;
            switch (entryType) {
            case OBJECT_TYPE_SMALL_SCENERY:                        break;
            case OBJECT_TYPE_LARGE_SCENERY: sceneryEntry |= 0x300; break;
            case OBJECT_TYPE_WALLS:         sceneryEntry |= 0x200; break;
            case OBJECT_TYPE_PATH_BITS:     sceneryEntry |= 0x100; break;
            default:                        sceneryEntry |= 0x400; break;
            }

            _legacyType.scenery_entries[_legacyType.entry_count] = sceneryEntry;
            _legacyType.entry_count++;
        }
    }
}

void SceneryGroupObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable()->GetCount());
}

void SceneryGroupObject::DrawPreview(rct_drawpixelinfo * dpi) const
{
    int x = dpi->width / 2;
    int y = dpi->height / 2;

    uint32 imageId = _legacyType.image + 0x20600001;
    gfx_draw_sprite(dpi, imageId, x - 15, y - 14, 0);
}

const utf8 * SceneryGroupObject::GetName() const
{
    const utf8 * name = GetStringTable()->GetString(OBJ_STRING_ID_NAME);
    return name != nullptr ? name : "";
}

void SceneryGroupObject::ReadItems(IStream * stream)
{
    auto items = std::vector<rct_object_entry>();

    uint8 endMarker;
    while ((endMarker = stream->ReadValue<uint8>()) != 0xFF)
    {
        stream->Seek(-1, STREAM_SEEK_CURRENT);
        rct_object_entry entry = stream->ReadValue<rct_object_entry>();
        items.push_back(entry);
    }

    _numItems = items.size();
    _items = Memory::DuplicateArray(items.data(), items.size());
}
