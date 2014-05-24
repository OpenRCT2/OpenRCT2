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

typedef struct {
	uint32 total_files;
	uint32 total_file_size;
	uint32 date_modified_checksum;
	uint32 object_list_size;
	uint32 var_10;
} rct_plugin_header;

/**
 * 
 *  rct2: 0x006A93CD
 */
static void object_list_examine()
{
	int i;
	char *object;

	object = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, char*);
	for (i = 0; i < RCT2_GLOBAL(0x00F42B6C, sint32); i++) {
		if (*object & 0xF0)
			RCT2_GLOBAL(0x00F42BDA, uint8) |= 1;

		// Skip 
		object += 16;

		// Skip filename
		// printf("%d %s : ", i, object);
		do {
			object++;
		} while (*(object - 1) != 0);

		// Skip 
		object += 4;

		// Skip name
		// printf("%s\n", object);
		do {
			object++;
		} while (*(object - 1) != 0);

		// Skip 
		object += 4;

		// Skip 
		object += *object++ * 16;

		// Skip theme objects
		object += *object++ * 16;

		// Skip 
		object += 4;
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
void object_read_and_load_entries(HANDLE hFile)
{
	//
	RCT2_CALLPROC_EBPSAFE(0x006A9CE8);

	int i;
	rct_object_entry *entries;

	// Maximum of 721 object entries in a scenario / saved game
	entries = malloc(721 * sizeof(rct_object_entry));
	sawyercoding_read_chunk(hFile, (uint8*)entries);

	// Load each object
	for (i = 0; i < 721; i++) {
		if (!check_object_entry(&entries[i]))
			continue;

		// 
		int eax;
		int ecx = i;
		int ebx = 0;
		while (ecx >= (eax = RCT2_ADDRESS(0x0098DA00, uint16)[ebx])) {
			ecx -= eax;
			ebx++;
		};

		// Load the obect
		if (!object_load(ecx, &entries[i])) {
			// Failed to load the object
			free(entries);

			//
			RCT2_CALLPROC_EBPSAFE(0x006A9CE8);
			return;
		}
	}

	free(entries);
}

/**
 * 
 *  rct2: 0x006AA2B7
 */
int object_load_packed()
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	RCT2_CALLFUNC_X(0x006AA2B7, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return eax;
}
