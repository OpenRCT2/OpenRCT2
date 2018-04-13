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

#include <cstring>

#include "../Context.h"
#include "../core/Math.hpp"
#include "../core/Util.hpp"
#include "../Game.h"
#include "../object/Object.h"
#include "ObjectList.h"
#include "ObjectRepository.h"
#include "ObjectManager.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"

// 98DA00
// clang-format off
sint32 object_entry_group_counts[] = {
    MAX_RIDE_OBJECTS,          // rides
    MAX_SMALL_SCENERY_OBJECTS, // small scenery
    MAX_LARGE_SCENERY_OBJECTS, // large scenery
    MAX_WALL_SCENERY_OBJECTS,  // walls
    MAX_BANNER_OBJECTS,        // banners
    MAX_PATH_OBJECTS,          // paths
    MAX_PATH_ADDITION_OBJECTS, // path bits
    MAX_SCENERY_GROUP_OBJECTS, // scenery sets
    MAX_PARK_ENTRANCE_OBJECTS, // park entrance
    MAX_WATER_OBJECTS,         // water
    MAX_SCENARIO_TEXT_OBJECTS  // scenario text
};

// 98DA2C
sint32 object_entry_group_encoding[] = {
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_ROTATE
};
// clang-format on

bool object_entry_is_empty(const rct_object_entry *entry)
{
    uint64 a, b;
    memcpy(&a, (uint8 *)entry, 8);
    memcpy(&b, (uint8 *)entry + 8, 8);

    if (a == 0xFFFFFFFFFFFFFFFF && b == 0xFFFFFFFFFFFFFFFF) return true;
    if (a == 0 && b == 0) return true;
    return false;
}

uint8 object_entry_get_type(const rct_object_entry * objectEntry)
{
    return (objectEntry->flags & 0x0F);
}

uint8 object_entry_get_source_game(const rct_object_entry * objectEntry)
{
    return (objectEntry->flags & 0xF0) >> 4;
}

/**
 *
 *  rct2: 0x006AB344
 */
void object_create_identifier_name(char* string_buffer, size_t size, const rct_object_entry* object)
{
    snprintf(string_buffer, size, "%.8s/%4X%4X", object->name, object->flags, object->checksum);
}

/**
 *
 *  rct2: 0x006A9DA2
 * bl = entry_index
 * ecx = entry_type
 */
bool find_object_in_entry_group(const rct_object_entry * entry, uint8 * entry_type, uint8 * entry_index)
{
    sint32 objectType = object_entry_get_type(entry);
    if (objectType >= OBJECT_TYPE_COUNT)
    {
        return false;
    }

    auto maxObjects = object_entry_group_counts[objectType];
    for (sint32 i = 0; i < maxObjects; i++)
    {
        if (object_entry_get_chunk(objectType, i) != nullptr)
        {
            auto thisEntry = object_entry_get_entry(objectType, i);
            if (object_entry_compare(thisEntry, entry))
            {
                *entry_type = objectType;
                *entry_index = i;
                return true;
            }
        }
    }
    return false;
}

void get_type_entry_index(size_t index, uint8 * outObjectType, uint8 * outEntryIndex)
{
    uint8 objectType = OBJECT_TYPE_RIDE;
    for (size_t groupCount : object_entry_group_counts)
    {
        if (index >= groupCount) {
            index -= groupCount;
            objectType++;
        } else {
            break;
        }
    }

    if (outObjectType != nullptr) *outObjectType = objectType;
    if (outEntryIndex != nullptr) *outEntryIndex = (uint8)index;
}

const rct_object_entry * get_loaded_object_entry(size_t index)
{
    uint8 objectType, entryIndex;
    get_type_entry_index(index, &objectType, &entryIndex);

    return object_entry_get_entry(objectType, entryIndex);
}

void * get_loaded_object_chunk(size_t index)
{
    uint8 objectType, entryIndex;
    get_type_entry_index(index, &objectType, &entryIndex);
    return object_entry_get_chunk(objectType, entryIndex);
}

void object_entry_get_name_fixed(utf8 * buffer, size_t bufferSize, const rct_object_entry * entry)
{
    bufferSize = Math::Min((size_t)DAT_NAME_LENGTH + 1, bufferSize);
    memcpy(buffer, entry->name, bufferSize - 1);
    buffer[bufferSize - 1] = 0;
}

void * object_entry_get_chunk(sint32 objectType, size_t index)
{
    size_t objectIndex = index;
    for (sint32 i = 0; i < objectType; i++)
    {
        objectIndex += object_entry_group_counts[i];
    }

    void * result = nullptr;
    auto objectMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objectMgr->GetLoadedObject(objectIndex);
    if (obj != nullptr)
    {
        result = obj->GetLegacyData();
    }
    return result;
}

const rct_object_entry * object_entry_get_entry(sint32 objectType, size_t index)
{
    const rct_object_entry * result = nullptr;
    auto objectMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objectMgr->GetLoadedObject(objectType, index);
    if (obj != nullptr)
    {
        result = obj->GetObjectEntry();
    }
    return result;
}
