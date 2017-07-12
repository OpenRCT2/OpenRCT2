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

#include "game.h"
#include "object.h"
#include "object_list.h"
#include "object/ObjectRepository.h"
#include "util/sawyercoding.h"
#include "util/util.h"

// 98DA00
sint32 object_entry_group_counts[] = {
    128,    // rides
    252,    // small scenery
    128,    // large scenery
    128,    // walls
    32,     // banners
    16,     // paths
    15,     // path bits
    19,     // scenery sets
    1,      // park entrance
    1,      // water
    1       // scenario text
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

#ifdef NO_RCT2
    rct_ride_entry              *gRideEntries[128];
    rct_small_scenery_entry     *gSmallSceneryEntries[252];
    rct_large_scenery_entry     *gLargeSceneryEntries[128];
    rct_wall_scenery_entry      *gWallSceneryEntries[128];
    rct_banner                  *gBannerSceneryEntries[32];
    rct_footpath_entry          *gFootpathEntries[16];
    rct_path_bit_scenery_entry  *gFootpathAdditionEntries[15];
    rct_scenery_set_entry       *gSceneryGroupEntries[19];
    rct_entrance_type           *gParkEntranceEntries[1];
    rct_water_type              *gWaterEntries[1];
    rct_stex_entry              *gStexEntries[1];
#endif

#ifdef NO_RCT2
    rct_object_entry_extended _objectEntriesRides[128];
    rct_object_entry_extended _objectEntriesSmallScenery[252];
    rct_object_entry_extended _objectEntriesLargeScenery[128];
    rct_object_entry_extended _objectEntriesWalls[128];
    rct_object_entry_extended _objectEntriesBanners[32];
    rct_object_entry_extended _objectEntriesFootpaths[16];
    rct_object_entry_extended _objectEntriesFootpathAdditions[15];
    rct_object_entry_extended _objectEntriesSceneryGroups[19];
    rct_object_entry_extended _objectEntriesParkEntrances[1];
    rct_object_entry_extended _objectEntriesWaters[1];
    rct_object_entry_extended _objectEntriesStexs[1];
#else
    #define _objectEntriesRides             RCT2_ADDRESS(0x00F3F03C             ,rct_object_entry_extended)
    #define _objectEntriesSmallScenery      RCT2_ADDRESS(0x00F3F03C + (128 * 20),rct_object_entry_extended)
    #define _objectEntriesLargeScenery      RCT2_ADDRESS(0x00F3F03C + (380 * 20),rct_object_entry_extended)
    #define _objectEntriesWalls             RCT2_ADDRESS(0x00F3F03C + (508 * 20),rct_object_entry_extended)
    #define _objectEntriesBanners           RCT2_ADDRESS(0x00F3F03C + (636 * 20),rct_object_entry_extended)
    #define _objectEntriesFootpaths         RCT2_ADDRESS(0x00F3F03C + (668 * 20),rct_object_entry_extended)
    #define _objectEntriesFootpathAdditions RCT2_ADDRESS(0x00F3F03C + (684 * 20),rct_object_entry_extended)
    #define _objectEntriesSceneryGroups     RCT2_ADDRESS(0x00F3F03C + (699 * 20),rct_object_entry_extended)
    #define _objectEntriesParkEntrances     RCT2_ADDRESS(0x00F3F03C + (718 * 20),rct_object_entry_extended)
    #define _objectEntriesWaters            RCT2_ADDRESS(0x00F3F03C + (719 * 20),rct_object_entry_extended)
    #define _objectEntriesStexs             RCT2_ADDRESS(0x00F3F03C + (720 * 20),rct_object_entry_extended)
#endif // NO_RCT2


// 0x98D97C chunk address', 0x98D980 object_entries
const rct_object_entry_group object_entry_groups[] = {
    (void**)(gRideEntries               ), _objectEntriesRides, // rides
    (void**)(gSmallSceneryEntries       ), _objectEntriesSmallScenery,  // small scenery    0x009AD1A4, 0xF2FA3C
    (void**)(gLargeSceneryEntries       ), _objectEntriesLargeScenery,  // large scenery    0x009AD594, 0xF40DEC
    (void**)(gWallSceneryEntries        ), _objectEntriesWalls, // walls            0x009AD794, 0xF417EC
    (void**)(gBannerSceneryEntries      ), _objectEntriesBanners,   // banners          0x009AD994, 0xF421EC
    (void**)(gFootpathEntries           ), _objectEntriesFootpaths, // paths            0x009ADA14, 0xF4246C
    (void**)(gFootpathAdditionEntries   ), _objectEntriesFootpathAdditions, // path bits        0x009ADA54, 0xF425AC
    (void**)(gSceneryGroupEntries       ), _objectEntriesSceneryGroups, // scenery sets     0x009ADA90, 0xF426D8
    (void**)(gParkEntranceEntries       ), _objectEntriesParkEntrances, // park entrance    0x009ADADC, 0xF42854
    (void**)(gWaterEntries              ), _objectEntriesWaters,    // water            0x009ADAE0, 0xF42868
    (void**)(gStexEntries               ), _objectEntriesStexs, // scenario text    0x009ADAE4, 0xF4287C
};

bool object_entry_is_empty(const rct_object_entry *entry)
{
    uint64 a, b;
    memcpy(&a, (uint8 *)entry, 8);
    memcpy(&b, (uint8 *)entry + 8, 8);

    if (a == 0xFFFFFFFFFFFFFFFF && b == 0xFFFFFFFFFFFFFFFF) return true;
    if (a == 0 && b == 0) return true;
    return false;
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
sint32 find_object_in_entry_group(const rct_object_entry* entry, uint8* entry_type, uint8* entry_index){
    *entry_type = entry->flags & 0xF;

    rct_object_entry_group entry_group = object_entry_groups[*entry_type];
    for (*entry_index = 0;
        *entry_index < object_entry_group_counts[*entry_type];
        ++(*entry_index),
        entry_group.chunks++,
        entry_group.entries++){

        if (*entry_group.chunks == (uint8*)-1) continue;

        if (object_entry_compare((rct_object_entry*)entry_group.entries, entry))break;
    }

    if (*entry_index == object_entry_group_counts[*entry_type])return 0;
    return 1;
}

void get_type_entry_index(size_t index, uint8 * outObjectType, uint8 * outEntryIndex)
{
    uint8 objectType = OBJECT_TYPE_RIDE;
    for (size_t i = 0; i < OBJECT_ENTRY_GROUP_COUNT; i++) {
        size_t groupCount = object_entry_group_counts[i];
        if (index >= groupCount) {
            index -= groupCount;
            objectType++;
        } else {
            break;
        }
    }

    if (outObjectType != NULL) *outObjectType = objectType;
    if (outEntryIndex != NULL) *outEntryIndex = (uint8)index;
}

const rct_object_entry * get_loaded_object_entry(size_t index)
{
    uint8 objectType, entryIndex;
    get_type_entry_index(index, &objectType, &entryIndex);

    rct_object_entry * entry = (rct_object_entry *)&(object_entry_groups[objectType].entries[entryIndex]);
    return entry;
}

void * get_loaded_object_chunk(size_t index)
{
    uint8 objectType, entryIndex;
    get_type_entry_index(index, &objectType, &entryIndex);

    void *entry = object_entry_groups[objectType].chunks[entryIndex];
    return entry;
}

void object_entry_get_name(utf8 * buffer, size_t bufferSize, const rct_object_entry * entry)
{
    size_t nameLength = 8;
    const char * end = memchr(entry->name, ' ', 8);
    if (end != NULL)
    {
        nameLength = (size_t)(end - entry->name);
    }

    bufferSize = min(nameLength + 1, bufferSize);
    memcpy(buffer, entry->name, bufferSize - 1);
    buffer[bufferSize - 1] = 0;
}

void object_entry_get_name_fixed(utf8 * buffer, size_t bufferSize, const rct_object_entry * entry)
{
    bufferSize = min(9, bufferSize);
    memcpy(buffer, entry->name, bufferSize - 1);
    buffer[bufferSize - 1] = 0;
}
