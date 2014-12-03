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
	uint32 extended;
} rct_object_entry_extended;

extern int object_entry_group_counts[];
extern int object_entry_group_encoding[];

int object_load_entry(const char *path, rct_object_entry *outEntry);
void object_list_load();
void set_load_objects_fail_reason();
int object_read_and_load_entries(FILE *file);
int object_load_packed(FILE *file);
void object_unload_all();

int object_load(int groupIndex, rct_object_entry *entry, int* chunk_size);
void object_unload(int groupIndex, rct_object_entry_extended *entry);
int object_get_scenario_text(rct_object_entry *entry);
void object_free_scenario_text();
int object_get_length(rct_object_entry *entry);
rct_object_entry *object_get_next(rct_object_entry *entry);
int object_calculate_checksum(rct_object_entry *entry, char *data, int dataLength);
int object_paint(int type, int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp);
int sub_6A9F42(FILE *file, rct_object_entry* entry);

#endif
