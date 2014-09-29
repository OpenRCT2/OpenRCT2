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

#include <windows.h>
#include "addresses.h"
#include "object.h"
#include "sawyercoding.h"

#define OBJECT_ENTRY_GROUP_COUNT 11
#define OBJECT_ENTRY_COUNT 721

typedef struct {
	uint32 total_files;
	uint32 total_file_size;
	uint32 date_modified_checksum;
	uint32 object_list_size;
	uint32 var_10;
} rct_plugin_header;

// 98DA00
int object_entry_group_counts[] = {
	128,	// rides
	252,	// small scenery
	128,	// large scenery
	128,	// walls
	32,		// banners
	16,		// paths
	15,		// path bits
	19,		// scenery sets
	1,		// park entrance
	1,		// water
	1		// scenario text
};

struct { void **data; rct_object_entry_extended *entries; } object_entry_groups[] = {
	(void**)(0x009ACFA4            ), (rct_object_entry_extended*)(0x00F3F03C             ),	// rides
	(void**)(0x009ACFA4 + (128 * 4)), (rct_object_entry_extended*)(0x00F3F03C + (128 * 20)),	// small scenery	
	(void**)(0x009ACFA4 + (380 * 4)), (rct_object_entry_extended*)(0x00F3F03C + (380 * 20)),	// large scenery
	(void**)(0x009ACFA4 + (508 * 4)), (rct_object_entry_extended*)(0x00F3F03C + (508 * 20)),	// walls
	(void**)(0x009ACFA4 + (636 * 4)), (rct_object_entry_extended*)(0x00F3F03C + (636 * 20)),	// banners
	(void**)(0x009ACFA4 + (668 * 4)), (rct_object_entry_extended*)(0x00F3F03C + (668 * 20)),	// paths
	(void**)(0x009ACFA4 + (684 * 4)), (rct_object_entry_extended*)(0x00F3F03C + (684 * 20)),	// path bits
	(void**)(0x009ACFA4 + (699 * 4)), (rct_object_entry_extended*)(0x00F3F03C + (699 * 20)),	// scenery sets
	(void**)(0x009ACFA4 + (718 * 4)), (rct_object_entry_extended*)(0x00F3F03C + (718 * 20)),	// park entrance
	(void**)(0x009ACFA4 + (719 * 4)), (rct_object_entry_extended*)(0x00F3F03C + (719 * 20)),	// water
	(void**)(0x009ACFA4 + (720 * 4)), (rct_object_entry_extended*)(0x00F3F03C + (720 * 20))		// scenario text
};

/**
 * 
 *  rct2: 0x006A93CD
 */
static void object_list_examine()
{
	int i;
	rct_object_entry *object;

	object = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
	for (i = 0; i < RCT2_GLOBAL(0x00F42B6C, sint32); i++) {
		if (object->flags & 0xF0)
			RCT2_GLOBAL(0x00F42BDA, uint8) |= 1;

		object = object_get_next(object);
	}
}

/**
 * 
 *  rct2: 0x006A8B40
 */
void object_list_load()
{
	HANDLE hFindFile;
	WIN32_FIND_DATAA findFileData;
	int totalFiles = 0, totalFileSize = 0, fileDateModifiedChecksum = 0;

	// Enumerate through each object in the directory
	hFindFile = FindFirstFile(RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), &findFileData);
	if (hFindFile != INVALID_HANDLE_VALUE) {
		do {
			totalFiles++;
			totalFileSize += findFileData.nFileSizeLow;
			fileDateModifiedChecksum ^=
				findFileData.ftLastWriteTime.dwLowDateTime ^
				findFileData.ftLastWriteTime.dwHighDateTime;
			fileDateModifiedChecksum = ror32(fileDateModifiedChecksum, 5);
		} while (FindNextFile(hFindFile, &findFileData));
		FindClose(hFindFile);
	}

	totalFiles = ror32(totalFiles, 24);
	totalFiles = (totalFiles & ~0xFF) | 1;
	totalFiles = rol32(totalFiles, 24);

	// Read plugin header
	rct_plugin_header pluginHeader;
	FILE *file = fopen(get_file_path(PATH_ID_PLUGIN), "rb");
	if (file != NULL) {
		if (fread(&pluginHeader, sizeof(pluginHeader), 1, file) == 1) {
			// Check if object repository has changed in anyway
			if (
				totalFiles == pluginHeader.total_files &&
				totalFileSize == pluginHeader.total_file_size &&
				fileDateModifiedChecksum == pluginHeader.date_modified_checksum
			) {
				// Dispose installed object list
				if (RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, sint32) != -1) {
					rct2_free(RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, void*));
					RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, sint32) = -1;
				}

				// Read installed object list
				RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, void*) = rct2_malloc(pluginHeader.object_list_size);
				if (fread(RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, void*), pluginHeader.object_list_size, 1, file) == 1) {
					RCT2_GLOBAL(0x00F42B6C, uint32) = pluginHeader.var_10;

					fclose(file);
					RCT2_CALLPROC_EBPSAFE(0x006A9FC0);
					object_list_examine();
					return;
				}
			}
		}
		fclose(file);
	}

	// Reload object list
	RCT2_GLOBAL(0x00F42B94, uint32) = totalFiles;
	RCT2_GLOBAL(0x00F42B98, uint32) = totalFileSize;
	RCT2_GLOBAL(0x00F42B9C, uint32) = fileDateModifiedChecksum;
	RCT2_CALLPROC_EBPSAFE(0x006A8D8F);
}

static int check_object_entry(rct_object_entry *entry)
{
	uint32 *dwords = (uint32*)entry;
	return (0xFFFFFFFF & dwords[0] & dwords[1] & dwords[2] & dwords[3]) + 1 != 0;
}

/**
 * 
 *  rct2: 0x006AA0C6
 */
int object_read_and_load_entries(FILE *file)
{
	object_unload_all();

	int i, j;
	rct_object_entry *entries;

	// Read all the object entries
	entries = malloc(OBJECT_ENTRY_COUNT * sizeof(rct_object_entry));
	sawyercoding_read_chunk(file, (uint8*)entries);

	// Load each object
	for (i = 0; i < OBJECT_ENTRY_COUNT; i++) {
		if (!check_object_entry(&entries[i]))
			continue;

		// Get entry group index
		int entryGroupIndex = i;
		for (j = 0; j < countof(object_entry_group_counts); j++) {
			if (entryGroupIndex < object_entry_group_counts[j])
				break;
			entryGroupIndex -= object_entry_group_counts[j];
		}

		// Load the obect
		if (!object_load(entryGroupIndex, &entries[i])) {
			// Failed to load the object
			//Destroy progress bar

			memcpy((char*)0x13CE952, entries[i].name, 8);
			free(entries);
			object_unload_all();
			RCT2_GLOBAL(0x14241BC, uint32) = 0;
			return 0;
		}
	}

	free(entries);
	return 1;
}



/**
 * 
 *  rct2: 0x006A9CE8
 */
void object_unload_all()
{
	int i, j;

	for (i = 0; i < OBJECT_ENTRY_GROUP_COUNT; i++)
		for (j = 0; j < object_entry_group_counts[i]; j++)
			if (object_entry_groups[i].data[j] != (void**)0xFFFFFFFF)
				object_unload(j, &object_entry_groups[i].entries[j]);
}
