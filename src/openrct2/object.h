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

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "common.h"
#include "drawing/drawing.h"

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
	// OBJECT_SELECTION_FLAG_REQUIRED = (1 << 3),				// Unused feature
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
	union {
		rct_object_entry entry;
		struct {
			uint32 flags;
			char name[8];
			uint32 checksum;
			uint32 chunk_size;
		};
	};
} rct_object_entry_extended;
assert_struct_size(rct_object_entry_extended, 0x14);

extern sint32 object_entry_group_counts[];
extern sint32 object_entry_group_encoding[];

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

void object_list_load();
bool object_load_entries(rct_object_entry* entries);

bool object_entry_is_empty(const rct_object_entry *entry);
bool object_entry_compare(const rct_object_entry *a, const rct_object_entry *b);
sint32 object_calculate_checksum(const rct_object_entry * entry, const void * data, size_t dataLength);
sint32 find_object_in_entry_group(const rct_object_entry* entry, uint8* entry_type, uint8* entry_index);
void object_create_identifier_name(char* string_buffer, size_t size, const rct_object_entry* object);

rct_object_entry *object_list_find_by_name(const char *name);
rct_object_entry *object_list_find(rct_object_entry *entry);

void object_entry_get_name(utf8 * buffer, size_t bufferSize, const rct_object_entry * entry);
void object_entry_get_name_fixed(utf8 * buffer, size_t bufferSize, const rct_object_entry * entry);

#endif
