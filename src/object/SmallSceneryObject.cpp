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
#include "SmallSceneryObject.h"

extern "C"
{
    #include "../drawing/drawing.h"
    #include "../localisation/localisation.h"
}

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
};

SmallSceneryObject::~SmallSceneryObject()
{
    Memory::Free(_var10data);
}

void SmallSceneryObject::ReadLegacy(IStream * stream)
{
    _legacyType.name = stream->ReadValue<rct_string_id>();
    _legacyType.image = stream->ReadValue<uint32>();

    _legacyType.small_scenery.flags = stream->ReadValue<uint32>();
    _legacyType.small_scenery.height = stream->ReadValue<uint8>();
    _legacyType.small_scenery.tool_id = stream->ReadValue<uint8>();
    _legacyType.small_scenery.price = stream->ReadValue<sint16>();
    _legacyType.small_scenery.removal_price = stream->ReadValue<sint16>();
    _legacyType.small_scenery.var_10 = stream->ReadValue<uint32>();
    _legacyType.small_scenery.var_14 = stream->ReadValue<uint16>();
    _legacyType.small_scenery.var_16 = stream->ReadValue<uint16>();
    _legacyType.small_scenery.var_18 = stream->ReadValue<uint16>();
    _legacyType.small_scenery.scenery_tab_id = 0xFF;

    StringTable.Read(stream, OBJ_STRING_ID_NAME);

    _sceneryTabEntry = stream->ReadValue<rct_object_entry>();

    if (_legacyType.small_scenery.flags & SMALL_SCENERY_FLAG16)
    {
        _var10data = ReadVar10(stream);
    }

    ImageTable.Read(stream);
}

void SmallSceneryObject::Load()
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

    if (_legacyType.small_scenery.flags & SMALL_SCENERY_FLAG16)
    {
        _legacyType.small_scenery.var_10 = (uint32)_var10data;
    }
}

void SmallSceneryObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, ImageTable.GetCount());
}

const utf8 * SmallSceneryObject::GetName()
{
    return StringTable.GetString(OBJ_STRING_ID_NAME);
}

uint8 * SmallSceneryObject::ReadVar10(IStream * stream)
{
    uint8 b;
    auto data = std::vector<uint8>();
    data.push_back(stream->ReadValue<uint8>());
    while ((b = stream->ReadValue<uint8>()) != 0xFF)
    {
        data.push_back(b);
    }
    data.push_back(b);
    return Memory::Duplicate(data.data(), data.size());
}
