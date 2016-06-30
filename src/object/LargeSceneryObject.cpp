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
#include "LargeSceneryObject.h"

extern "C"
{
    #include "../drawing/drawing.h"
    #include "../localisation/localisation.h"
}

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
};

LargeSceneryObject::~LargeSceneryObject()
{
    Memory::Free(_3dFont);
    Memory::Free(_tiles);
}

void LargeSceneryObject::ReadLegacy(IStream * stream)
{
    _legacyType.name = stream->ReadValue<rct_string_id>();
    _legacyType.image = stream->ReadValue<uint32>();

    _legacyType.large_scenery.tool_id = stream->ReadValue<uint8>();
    _legacyType.large_scenery.flags = stream->ReadValue<uint8>();
    _legacyType.large_scenery.price = stream->ReadValue<sint16>();
    _legacyType.large_scenery.removal_price = stream->ReadValue<sint16>();
    stream->Seek(4, STREAM_SEEK_CURRENT);
    _legacyType.large_scenery.scenery_tab_id = 0xFF;
    _legacyType.large_scenery.var_11 = stream->ReadValue<uint8>();
    stream->Seek(5, STREAM_SEEK_CURRENT);

    GetStringTable()->Read(stream, OBJ_STRING_ID_NAME);

    _sceneryTabEntry = stream->ReadValue<rct_object_entry>();

    if (_legacyType.large_scenery.flags & (1 << 2))
    {
        _3dFont = Memory::Allocate<rct_large_scenery_text>();
        stream->Read(_3dFont);
        _legacyType.large_scenery.text = _3dFont;
    }

    _tiles = ReadTiles(stream);

    GetImageTable()->Read(stream);
}

void LargeSceneryObject::Load()
{
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable()->GetImages(), GetImageTable()->GetCount());

    _legacyType.large_scenery.tiles = _tiles;

    _legacyType.large_scenery.scenery_tab_id = 0xFF;
    if ((_sceneryTabEntry.flags & 0xFF) != 0xFF)
    {
        uint8 entryType, entryIndex;
        if (find_object_in_entry_group(&_sceneryTabEntry, &entryType, &entryIndex))
        {
            _legacyType.large_scenery.scenery_tab_id = entryIndex;
        }
    }

    if (_legacyType.large_scenery.flags & (1 << 2))
    {
        _legacyType.large_scenery.text_image = _legacyType.image;
        if (_3dFont->var_C & (1 << 0))
        {
            _legacyType.image += (_3dFont->var_C >> 8) * 2;
        }
        else
        {
            _legacyType.image += (_3dFont->var_C >> 8) * 4;
        }
    }
}

void LargeSceneryObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable()->GetCount());
}

const utf8 * LargeSceneryObject::GetName() const
{
    const utf8 * name = GetStringTable()->GetString(OBJ_STRING_ID_NAME);
    return name != nullptr ? name : "";
}

rct_large_scenery_tile * LargeSceneryObject::ReadTiles(IStream * stream)
{
    auto tiles = std::vector<rct_large_scenery_tile>();

    uint16 tilesEndMarker;
    while ((tilesEndMarker = stream->ReadValue<uint16>()) != 0xFFFF)
    {
        stream->Seek(-2, STREAM_SEEK_CURRENT);
        auto tile = stream->ReadValue<rct_large_scenery_tile>();
        tiles.push_back(tile);
    }

    return Memory::DuplicateArray(tiles.data(), tiles.size());
}
