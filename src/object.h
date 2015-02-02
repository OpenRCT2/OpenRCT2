/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "common.h"

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

/**
 * Object entry structure.
 * size: 0x10
 */
typedef struct {
	uint32 flags;
	char name[8];
	uint32 checksum;
} rct_object_entry;

/**
 * Object entry structure extended.
 * size: 0x14
 */
typedef struct {
	uint32 flags;
	char name[8];
	uint32 checksum;
	uint32 chunk_size;
} rct_object_entry_extended;

extern int object_entry_group_counts[];
extern int object_entry_group_encoding[];

typedef struct {
	uint8 **chunks; 
	rct_object_entry_extended *entries;
} rct_object_entry_group;

extern rct_object_entry_group object_entry_groups[];

int object_load_entry(const char *path, rct_object_entry *outEntry);
void object_list_load();
void set_load_objects_fail_reason();
int object_read_and_load_entries(FILE *file);
int object_load_packed(FILE *file);
void object_unload_all();

int object_load(int groupIndex, rct_object_entry *entry, int* chunk_size);
int object_load_file(int groupIndex, const rct_object_entry *entry, int* chunkSize, const rct_object_entry *installedObject);
void object_unload(int groupIndex, rct_object_entry_extended *entry);
int object_get_scenario_text(rct_object_entry *entry);
void object_free_scenario_text();
int object_get_length(rct_object_entry *entry);
int object_entry_compare(const rct_object_entry *a, const rct_object_entry *b);
int object_calculate_checksum(const rct_object_entry *entry, const char *data, int dataLength);
int object_paint(int type, int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp);
rct_object_entry *object_get_next(rct_object_entry *entry);
int sub_6A9F42(FILE *file, rct_object_entry* entry);
int find_object_in_entry_group(rct_object_entry* entry, uint8* entry_type, uint8* entry_index);

rct_object_entry *object_list_find(rct_object_entry *entry);

#endif