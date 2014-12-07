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

#include "addresses.h"
#include "localisation/localisation.h"
#include "object.h"
#include "platform/platform.h"
#include "platform/osinterface.h"
#include "ride/track.h"
#include "util/sawyercoding.h"

#define OBJECT_ENTRY_GROUP_COUNT 11
#define OBJECT_ENTRY_COUNT 721

typedef struct {
	uint32 total_files;
	uint32 total_file_size;
	uint32 date_modified_checksum;
	uint32 object_list_size;
	uint32 object_list_no_items;
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

// 98DA2C
int object_entry_group_encoding[] = {
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

static int object_list_cache_load(int totalFiles, uint64 totalFileSize, int fileDateModifiedChecksum);
static int object_list_cache_save(int fileCount, uint64 totalFileSize, int fileDateModifiedChecksum, int currentItemOffset);

void object_list_create_hash_table();

static void get_plugin_path(char *path)
{
	char *homePath = osinterface_get_orct2_homefolder();
	sprintf(path, "%s%c%s", homePath, osinterface_get_path_separator(), "plugin.dat");
	free(homePath);
}

/**
 * 
 *  rct2: 0x006A93CD
 */
static void object_list_examine()
{
	int i;
	rct_object_entry *object;

	object = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
	for (i = 0; i < RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, sint32); i++) {
		if (object->flags & 0xF0)
			RCT2_GLOBAL(0x00F42BDA, uint8) |= 1;

		object = object_get_next(object);
	}

	// Create a search index
	object_list_create_hash_table();
}

/**
 * 
 *  rct2: 0x006DED68
 */
void reset_9E32F8()
{
	uint8* edi = RCT2_ADDRESS(0x009E32F8, uint8);
	memset(edi, 0xFF, 90);
}

void sub_6A9FC0()
{
	reset_9E32F8();

	RCT2_GLOBAL(0x009ADAF0, uint32) = 0xF26E;

	for (int type = 0; type < 11; ++type){
		for (int j = 0; j < object_entry_group_counts[type]; j++){
			rct_object_entry* entry = (rct_object_entry*)object_entry_groups[type].data[j];
			if (entry != (rct_object_entry*)0xFFFFFFFF)
				object_paint(type, 0, 0, 0, 0, (int)entry, 0, 0);
		}
	}
}

int object_copy(uint8 *destination_object, uint8 *installed_object)
{
	uint8 *original_dest = destination_object;

	memcpy(destination_object, installed_object, sizeof(rct_object_entry));
	destination_object += sizeof(rct_object_entry);
	installed_object += sizeof(rct_object_entry);

	do {
		*destination_object++ = *installed_object++;
	} while (*(destination_object - 1));

	*((sint32*)destination_object) = *((sint32*)installed_object);
	destination_object += 4;
	installed_object += 4;

	do {
		*destination_object++ = *installed_object++;
	} while (*(destination_object - 1));

	*((sint32*)destination_object) = *((sint32*)installed_object);
	destination_object += 4;
	installed_object += 4;

	uint8 no_obj_unk = *installed_object++;
	*destination_object++ = no_obj_unk;

	memcpy(destination_object, installed_object, no_obj_unk * sizeof(rct_object_entry));
	destination_object += no_obj_unk * sizeof(rct_object_entry);
	installed_object += no_obj_unk * sizeof(rct_object_entry);

	uint8 no_obj_theme = *installed_object++;
	*destination_object++ = no_obj_theme;

	memcpy(destination_object, installed_object, no_obj_theme*sizeof(rct_object_entry));
	destination_object += no_obj_theme * sizeof(rct_object_entry);
	installed_object += no_obj_theme * sizeof(rct_object_entry);

	*((sint32*)destination_object) = *((sint32*)installed_object);
	destination_object += 4;
	installed_object += 4;

	return destination_object - original_dest;
}

static int object_list_query_directory(int *outTotalFiles, uint64 *outTotalFileSize, int *outFileDateModifiedChecksum)
{
	int enumFileHandle, totalFiles, fileDateModifiedChecksum;
	uint64 totalFileSize;
	file_info enumFileInfo;

	totalFiles = 0;
	totalFileSize = 0;
	fileDateModifiedChecksum = 0;

	// Enumerate through each object in the directory
	enumFileHandle = platform_enumerate_files_begin(RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char));
	if (enumFileHandle != INVALID_HANDLE) {
		while (platform_enumerate_files_next(enumFileHandle, &enumFileInfo)) {
			totalFiles++;
			totalFileSize += enumFileInfo.size;
			fileDateModifiedChecksum ^=
				(uint32)(enumFileInfo.last_modified >> 32) ^
				(uint32)(enumFileInfo.last_modified & 0xFFFFFFFF);
			fileDateModifiedChecksum = ror32(fileDateModifiedChecksum, 5);
		}
		platform_enumerate_files_end(enumFileHandle);
	}

	*outTotalFiles = totalFiles;
	*outTotalFileSize = totalFileSize;
	*outFileDateModifiedChecksum = fileDateModifiedChecksum;
}

/**
 * 
 *  rct2: 0x006A8B40
 */
void object_list_load()
{
	int enumFileHandle, totalFiles, fileDateModifiedChecksum;
	uint64 totalFileSize;
	file_info enumFileInfo;

	object_list_query_directory(&totalFiles, &totalFileSize, &fileDateModifiedChecksum);

	// Would move this into cache load, but its used further on
	totalFiles = ror32(totalFiles, 24);
	totalFiles = (totalFiles & ~0xFF) | 1;
	totalFiles = rol32(totalFiles, 24);

	if (object_list_cache_load(totalFiles, totalFileSize, fileDateModifiedChecksum))
		return;

	// Reload object list

	if (RCT2_GLOBAL(0x9AA00D, uint8) != 0)
		RCT2_GLOBAL(0x9AA00D, uint8) = 0;

	sub_6A9FC0();

	// Dispose installed object list
	if (RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, sint32) != -1) {
		rct2_free(RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, void*));
		RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, sint32) = -1;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, void*) = rct2_malloc(4096);
	if (RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, int) == -1){
		RCT2_CALLPROC_X(0x006E3838, 0x343, 0xC5A, 0, 0, 0, 0, 0);
		return;
	}

	uint32 fileCount = 0;
	uint32 current_item_offset = 0;

	log_verbose("building cache of available objects...");

	enumFileHandle = platform_enumerate_files_begin(RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char));
	if (enumFileHandle != INVALID_HANDLE) {
		uint32 installed_buffer_size = 0x1000;

		while (platform_enumerate_files_next(enumFileHandle, &enumFileInfo)) {
			fileCount++;

			if ((installed_buffer_size - current_item_offset) <= 2842){
				installed_buffer_size += 0x1000;
				RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, void*) = rct2_realloc(RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, void*), installed_buffer_size);
				if (RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, int) == -1){
					RCT2_CALLPROC_X(0x006E3838, 0x343, 0xC5A, 0, 0, 0, 0, 0);
					return;
				}
			}

			char path[MAX_PATH];
			subsitute_path(path, RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), enumFileInfo.path);

			rct_object_entry* entry = RCT2_ADDRESS(0x00F42B74, rct_object_entry);
			if (!object_load_entry(path, entry))
				continue;

			RCT2_GLOBAL(0xF42BC4, uint32) = current_item_offset;

			uint8* installed_entry_pointer = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, uint8*) + current_item_offset;

			memcpy(installed_entry_pointer, entry, sizeof(rct_object_entry));
			installed_entry_pointer += sizeof(rct_object_entry);

			strcpy(installed_entry_pointer, enumFileInfo.path);
			while (*installed_entry_pointer++);

			// Chunk size is set to unknown
			*((sint32*)installed_entry_pointer) = -1;
			*(installed_entry_pointer + 4) = 0;
			*((sint32*)(installed_entry_pointer + 5)) = 0;
			*((uint16*)(installed_entry_pointer + 9)) = 0;
			*((uint32*)(installed_entry_pointer + 11)) = 0;

			RCT2_GLOBAL(0x9ADAF0, uint32) = 0xF26E;

			RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32)++;

			// This is a variable used by object_load to decide if it should
			// use object_paint on the entry.
			RCT2_GLOBAL(0x009ADAFD, uint8) = 1;

			// Probably used by object paint.
			RCT2_GLOBAL(0x009ADAF4, uint32) = 0xF42BDB;

			int chunk_size;
			if (!object_load(-1, entry, &chunk_size)){
				RCT2_GLOBAL(0x009ADAF4, sint32) = -1;
				RCT2_GLOBAL(0x009ADAFD, uint8) = 0;
				RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32)--;
				continue;
			}

			int objectType = entry->flags & 0xF;

			// See above note
			RCT2_GLOBAL(0x009ADAF4, sint32) = -1;
			RCT2_GLOBAL(0x009ADAFD, uint8) = 0;

			if ((entry->flags & 0xF0) == 0x80) {
				RCT2_GLOBAL(0x00F42B70, uint32)++;
				if (RCT2_GLOBAL(0x00F42B70, uint32) > 772){
					RCT2_GLOBAL(0x00F42B70, uint32)--;
					RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32)--;
					continue;
				}
			}
			*((sint32*)installed_entry_pointer) = chunk_size;
			installed_entry_pointer += 4;

			uint8* chunk = RCT2_GLOBAL(RCT2_ADDRESS_CURR_OBJECT_CHUNK_POINTER, uint8*); // Loaded in object_load

			// When made of two parts i.e Wooden Roller Coaster (Dream Woodie Cars);
			if (objectType == 0 && !(*((uint32*)(chunk + 8)) & 0x1000)) {
				rct_string_id obj_string = chunk[12];
				if (obj_string == 0xFF){
					obj_string = chunk[13];
					if (obj_string == 0xFF) {
						obj_string = chunk[14];
					}
				}

				obj_string += 2;
				format_string(installed_entry_pointer, obj_string, 0);
				strcat(installed_entry_pointer, "\t (");
				strcat(installed_entry_pointer, language_get_string(RCT2_GLOBAL(0x00F42BBC, uint32)));
				strcat(installed_entry_pointer, ")");
				while (*installed_entry_pointer++);
			} else{
				strcpy(installed_entry_pointer, language_get_string(RCT2_GLOBAL(0x00F42BBC, uint32)));
				while (*installed_entry_pointer++);
			}
			*((uint32*)installed_entry_pointer) = RCT2_GLOBAL(0x009ADAF0, uint32) - 0xF26E;
			installed_entry_pointer += 4;

			uint8* esi = RCT2_ADDRESS(0x00F42BDB, uint8);
			uint8 num_unk_objects = *esi++;
			*installed_entry_pointer++ = num_unk_objects;
			if (num_unk_objects > 0) {
				memcpy(installed_entry_pointer, esi, num_unk_objects * sizeof(rct_object_entry));
				installed_entry_pointer += num_unk_objects * sizeof(rct_object_entry);
			}

			uint8 no_theme_objects  = *esi++;
			*installed_entry_pointer++ = no_theme_objects ;
			if (no_theme_objects > 0) {
				memcpy(installed_entry_pointer, esi, no_theme_objects * sizeof(rct_object_entry));
				installed_entry_pointer += no_theme_objects * sizeof(rct_object_entry);
			}

			*((uint32*)installed_entry_pointer) = RCT2_GLOBAL(0x00F433DD, uint32);
			installed_entry_pointer += 4;

			int size_of_object = installed_entry_pointer - RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, uint8*) - current_item_offset;

			object_unload(objectType, (rct_object_entry_extended*)entry);

			// Return pointer to start of entry
			installed_entry_pointer -= size_of_object;

			uint8* copied_entry = RCT2_ADDRESS(0x0140E9AC, uint8);

			size_of_object = object_copy(copied_entry, installed_entry_pointer);

			RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32)--;
			copied_entry += sizeof(rct_object_entry);
			// Skip filename
			while (*copied_entry++);

			// Skip 
			copied_entry += 4;

			installed_entry_pointer = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, uint8*);

			for (uint32 i = 0; i < RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); ++i){

				uint8* temp_installed_entry = installed_entry_pointer;
				temp_installed_entry += sizeof(rct_object_entry);

				// Skip filename
				while (*temp_installed_entry++);

				// Skip 
				temp_installed_entry += 4;

				if (strcmp(temp_installed_entry, copied_entry) <= 0)break;

				installed_entry_pointer = (uint8*)(object_get_next((rct_object_entry*)installed_entry_pointer));
			}

			// Difference to new location
			int numBytesToMove = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, uint8*) + current_item_offset - installed_entry_pointer;

			uint8 *curr_location = installed_entry_pointer;
			uint8 *move_location = installed_entry_pointer + size_of_object;

			if (numBytesToMove > 0)
				memmove(move_location, curr_location, numBytesToMove);

			copied_entry = RCT2_ADDRESS(0x0140E9AC, uint8);
			memcpy(installed_entry_pointer, copied_entry, size_of_object);
			current_item_offset += size_of_object;
			RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32)++;
		}
		platform_enumerate_files_end(enumFileHandle);
	}

	sub_6A9FC0();

	object_list_cache_save(fileCount, totalFileSize, fileDateModifiedChecksum, current_item_offset);

	// 
	ride_list_item ride_list;
	ride_list.entry_index = 0xFC;
	ride_list.type = 0xFC;
	track_load_list(ride_list);

	object_list_examine();
}

static int object_list_cache_load(int totalFiles, uint64 totalFileSize, int fileDateModifiedChecksum)
{
	char path[MAX_PATH];
	FILE *file;
	rct_plugin_header pluginHeader;

	log_verbose("loading object list cache (plugin.dat)");

	get_plugin_path(path);
	file = fopen(path, "rb");
	if (file == NULL) {
		log_verbose("Unable to load %s", path);
		return 0;
	}

	if (fread(&pluginHeader, sizeof(rct_plugin_header), 1, file) == 1) {
		// Check if object repository has changed in anyway
		if (
			pluginHeader.total_files == totalFiles &&
			pluginHeader.total_file_size == totalFileSize &&
			pluginHeader.date_modified_checksum == fileDateModifiedChecksum
		) {
			// Dispose installed object list
			if (RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, sint32) != -1) {
				rct2_free(RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, void*));
				RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, sint32) = -1;
			}

			// Read installed object list
			RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, void*) = rct2_malloc(pluginHeader.object_list_size);
			if (fread(RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, void*), pluginHeader.object_list_size, 1, file) == 1) {
				RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32) = pluginHeader.object_list_no_items;

				fclose(file);
				sub_6A9FC0();
				object_list_examine();
				return 1;
			}
		}
	}

	fclose(file);

	log_error("loading object list cache failed");
	return 0;
}

static int object_list_cache_save(int fileCount, uint64 totalFileSize, int fileDateModifiedChecksum, int currentItemOffset)
{
	char path[MAX_PATH];
	FILE *file;
	rct_plugin_header pluginHeader;
	
	log_verbose("saving object list cache (plugin.dat)");

	pluginHeader.total_files = fileCount | 0x01000000;
	pluginHeader.total_file_size = (uint32)totalFileSize;
	pluginHeader.date_modified_checksum = fileDateModifiedChecksum;
	pluginHeader.object_list_size = currentItemOffset;
	pluginHeader.object_list_no_items = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32);

	get_plugin_path(path);
	file = fopen(path,"wb");
	if (file == NULL) {
		log_error("Failed to save %s", path);
		return 0;
	}

	fwrite(&pluginHeader, sizeof(rct_plugin_header), 1, file);
	fwrite(RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, uint8*), pluginHeader.object_list_size, 1, file);
	fclose(file);
	return 1;
}

static int check_object_entry(rct_object_entry *entry)
{
	uint32 *dwords = (uint32*)entry;
	return (0xFFFFFFFF & dwords[0] & dwords[1] & dwords[2] & dwords[3]) + 1 != 0;
}

/* rct2: 0x675827 */
void set_load_objects_fail_reason(){
	rct_object_entry* object = RCT2_ADDRESS(0x13CE952, rct_object_entry);
	int expansion = (object->flags & 0xFF) >> 4;
	if (expansion == 0
		|| expansion == 8
		|| RCT2_GLOBAL(0x9AB4C0, uint16) & (1 << expansion)){

		char* string_buffer = RCT2_ADDRESS(0x9BC677, char);

		format_string(string_buffer, 3323, 0); //Missing object data, ID:

		RCT2_CALLPROC_X(0x6AB344, 0, 0, 0, 0, 0, (int)string_buffer, 0x13CE952);
		RCT2_GLOBAL(0x9AC31B, uint8) = 0xFF;
		RCT2_GLOBAL(0x9AC31C, uint16) = 3165;
		return;
	}

	char* exapansion_name = &RCT2_ADDRESS(RCT2_ADDRESS_EXPANSION_NAMES, char)[128 * expansion];
	if (*exapansion_name == '\0'){
		RCT2_GLOBAL(0x9AC31B, uint8) = 0xFF;
		RCT2_GLOBAL(0x9AC31C, uint16) = 3325;
		return;
	}

	char* string_buffer = RCT2_ADDRESS(0x9BC677, char);

	format_string(string_buffer, 3324, 0); // Requires expansion pack
	strcat(string_buffer, exapansion_name);
	RCT2_GLOBAL(0x9AC31B, uint8) = 0xFF;
	RCT2_GLOBAL(0x9AC31C, uint16) = 3165;
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

	log_verbose("loading required objects");

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
		if (!object_load(entryGroupIndex, &entries[i], NULL)) {
			log_error("failed to load entry: %.8s", entries[i].name);
			memcpy((char*)0x13CE952, &entries[i], sizeof(rct_object_entry));
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

	sub_6A9FC0();
}



uint32 _installedObjectHashTableSize;
rct_object_entry ** _installedObjectHashTable = NULL;
uint32 _installedObjectHashTableCollisions;

uint32 object_get_hash_code(rct_object_entry *object)
{
	uint32 hash = 5381;
	uint8 *byte = (uint8*)object;
	int i;

	for (i = 0; i < 8; i++)
        hash = ((hash << 5) + hash) + object->name[i];

    return hash;
}

void object_list_create_hash_table()
{
	rct_object_entry *installedObject;
	int numInstalledObjects = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, sint32);

	if (_installedObjectHashTable != NULL)
		free(_installedObjectHashTable);

	_installedObjectHashTableSize = max(8192, numInstalledObjects * 4);
	_installedObjectHashTable = calloc(_installedObjectHashTableSize, sizeof(rct_object_entry*));
	_installedObjectHashTableCollisions = 0;

	installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
	for (int i = 0; i < numInstalledObjects; i++) {
		uint32 hash = object_get_hash_code(installedObject);
		uint32 index = hash % _installedObjectHashTableSize;

		// Find empty slot
		while (_installedObjectHashTable[index] != NULL) {
			_installedObjectHashTableCollisions++;
			index++;
		}

		// Set hash table slot
		_installedObjectHashTable[index] = installedObject;

		// Next installde object
		installedObject = object_get_next(installedObject);
	}
}

rct_object_entry *object_list_find(rct_object_entry *entry)
{
	uint32 hash = object_get_hash_code(entry);
	uint32 index = hash % _installedObjectHashTableSize;

	while (_installedObjectHashTable[index] != NULL) {
		if (object_entry_compare(entry, _installedObjectHashTable[index]))
			return _installedObjectHashTable[index];

		index++;
	}

	return NULL;
}