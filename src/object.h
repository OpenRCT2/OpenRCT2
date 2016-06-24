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

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "common.h"
#include "drawing/drawing.h"
#include "platform/platform.h"

// First 0xF of rct_object_entry->flags
typedef enum{
	OBJECT_TYPE_RIDE,
	OBJECT_TYPE_SMALL_SCENERY,
	OBJECT_TYPE_LARGE_SCENERY,
	OBJECT_TYPE_WALLS,
	OBJECT_TYPE_BANNERS,
	OBJECT_TYPE_PATHS,
	OBJECT_TYPE_PATH_BITS,
	OBJECT_TYPE_SCENERY_SETS,
	OBJECT_TYPE_PARK_ENTRANCE,
	OBJECT_TYPE_WATER,
	OBJECT_TYPE_SCENARIO_TEXT
}OBJECT_TYPE;

typedef enum{
	OBJECT_SELECTION_FLAG_SELECTED = (1 << 0),
	OBJECT_SELECTION_FLAG_2 = (1 << 1),
	OBJECT_SELECTION_FLAG_IN_USE = (1 << 2),
	OBJECT_SELECTION_FLAG_REQUIRED = (1 << 3),
	OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED = (1 << 4),
	OBJECT_SELECTION_FLAG_6 = (1 << 5),
	OBJECT_SELECTION_FLAG_7 = (1 << 6),
	OBJECT_SELECTION_FLAG_8 = (1 << 7),
}OBJECT_SELECTION_FLAGS;

#define OBJECT_ENTRY_GROUP_COUNT 11
#define OBJECT_ENTRY_COUNT 721

#pragma pack(push, 1)
/**
 * Object entry structure.
 * size: 0x10
 */
typedef struct rct_object_entry {
	uint32 flags;
	char name[8];
	uint32 checksum;
} rct_object_entry;
assert_struct_size(rct_object_entry, 0x10);

/**
 * Object entry structure extended.
 * size: 0x14
 */
typedef struct rct_object_entry_extended {
	uint32 flags;
	char name[8];
	uint32 checksum;
	uint32 chunk_size;
} rct_object_entry_extended;
assert_struct_size(rct_object_entry_extended, 0x14);

extern int object_entry_group_counts[];
extern int object_entry_group_encoding[];

typedef struct rct_object_entry_group {
	void **chunks;
	rct_object_entry_extended *entries;
} rct_object_entry_group;
#ifdef PLATFORM_32BIT
assert_struct_size(rct_object_entry_group, 8);
#endif

typedef struct rct_ride_filters {
	uint8 category[2];
	uint8 ride_type;
} rct_ride_filters;
assert_struct_size(rct_ride_filters, 3);

typedef struct rct_object_filters {
	union {
		rct_ride_filters ride;
	};
} rct_object_filters;
assert_struct_size(rct_object_filters, 3);
#pragma pack(pop)

extern const rct_object_entry_group object_entry_groups[];
extern char gTempObjectLoadName[9];

extern uint32 gInstalledObjectsCount;
extern rct_object_entry *gInstalledObjects;
extern uint32 gNumInstalledRCT2Objects;
extern uint32 gNumInstalledCustomObjects;
extern uint32 gTotalNoImages;

extern void *gLastLoadedObjectChunkData;

int object_load_entry(const utf8 *path, rct_object_entry *outEntry);
void object_list_load();
void set_load_objects_fail_reason();
bool object_read_and_load_entries(SDL_RWops* rw);
bool object_load_entries(rct_object_entry* entries);
int object_load_packed(SDL_RWops* rw);
void object_unload_all();

int check_object_entry(rct_object_entry *entry);
int object_load_file(int groupIndex, const rct_object_entry *entry, int* chunkSize, const rct_object_entry *installedObject);
int object_load_chunk(int groupIndex, rct_object_entry *entry, int* chunk_size);
void object_unload_chunk(rct_object_entry *entry);
int object_get_scenario_text(rct_object_entry *entry);
void object_free_scenario_text();
uintptr_t object_get_length(const rct_object_entry *entry);
int object_entry_compare(const rct_object_entry *a, const rct_object_entry *b);
int object_calculate_checksum(const rct_object_entry *entry, const uint8 *data, int dataLength);
rct_object_entry *object_get_next(const rct_object_entry *entry);
int write_object_file(SDL_RWops* rw, rct_object_entry* entry);
void reset_loaded_objects();
int find_object_in_entry_group(rct_object_entry* entry, uint8* entry_type, uint8* entry_index);
void object_create_identifier_name(char* string_buffer, const rct_object_entry* object);

rct_object_entry *object_list_find_by_name(const char *name);
rct_object_entry *object_list_find(rct_object_entry *entry);

char *object_get_name(rct_object_entry *entry);

rct_object_filters *get_object_filter(int index);

bool object_load(int type, void *objectEntry, uint32 entryIndex);
void object_unload(int type, void *objectEntry);
bool object_test(int type, void *objectEntry);
void object_paint(int type, void *objectEntry, rct_drawpixelinfo *dpi, sint32 x, sint32 y);
rct_string_id object_desc(int type, void *objectEntry);

#endif
