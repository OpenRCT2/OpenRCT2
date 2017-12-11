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
#include "../core/Memory.hpp"
#include "ObjectManager.h"
#include "ObjectRepository.h"
#include "SceneryGroupObject.h"

#include "../drawing/Drawing.h"
#include "../localisation/Language.h"

void SceneryGroupObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    stream->Seek(0x80 * 2, STREAM_SEEK_CURRENT);
    _legacyType.entry_count = stream->ReadValue<uint8>();
    _legacyType.pad_107 = stream->ReadValue<uint8>();
    _legacyType.priority = stream->ReadValue<uint8>();
    _legacyType.pad_109 = stream->ReadValue<uint8>();
    _legacyType.entertainer_costumes = stream->ReadValue<uint32>();

    GetStringTable()->Read(context, stream, OBJ_STRING_ID_NAME);
    _items = ReadItems(stream);
    GetImageTable()->Read(context, stream);
}

void SceneryGroupObject::Load()
{
    GetStringTable().Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
    _legacyType.entry_count = 0;
}

void SceneryGroupObject::Unload()
{
    language_free_object_string(_legacyType.name);
    gfx_object_free_images(_legacyType.image, GetImageTable().GetCount());

    _legacyType.name = 0;
    _legacyType.image = 0;
}

void SceneryGroupObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    sint32 x = width / 2;
    sint32 y = height / 2;

    uint32 imageId = _legacyType.image + 0x20600001;
    gfx_draw_sprite(dpi, imageId, x - 15, y - 14, 0);
}

void SceneryGroupObject::UpdateEntryIndexes()
{
    IObjectRepository * objectRepository = GetObjectRepository();
    IObjectManager * objectManager = GetObjectManager();

    _legacyType.entry_count = 0;
    for (const auto &objectEntry : _items)
    {
        auto ori = objectRepository->FindObject(&objectEntry);
        if (ori == nullptr) continue;
        if (ori->LoadedObject == nullptr) continue;

        uint16 sceneryEntry = objectManager->GetLoadedObjectEntryIndex(ori->LoadedObject);
        Guard::Assert(sceneryEntry != UINT8_MAX, GUARD_LINE);

        uint8 objectType = objectEntry.flags & 0x0F;
        switch (objectType) {
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

void SceneryGroupObject::SetRepositoryItem(ObjectRepositoryItem * item) const
{
    Memory::Free(item->ThemeObjects);

    item->NumThemeObjects = (uint16)_items.size();
    item->ThemeObjects = Memory::AllocateArray<rct_object_entry>(_items.size());
    for (size_t i = 0; i < _items.size(); i++)
    {
        item->ThemeObjects[i] = _items[i];
    }
}

std::vector<rct_object_entry> SceneryGroupObject::ReadItems(IStream * stream)
{
    auto items = std::vector<rct_object_entry>();
    while (stream->ReadValue<uint8>() != 0xFF)
    {
        stream->Seek(-1, STREAM_SEEK_CURRENT);
        rct_object_entry entry = stream->ReadValue<rct_object_entry>();
        items.push_back(entry);
    }
    return items;
}
