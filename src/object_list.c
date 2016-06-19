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

#include "addresses.h"
#include "config.h"
#include "game.h"
#include "localisation/localisation.h"
#include "object.h"
#include "object_list.h"
#include "platform/platform.h"
#include "rct1.h"
#include "ride/track.h"
#include "ride/track_design.h"
#include "util/sawyercoding.h"
#include "world/entrance.h"
#include "world/footpath.h"
#include "world/scenery.h"
#include "world/water.h"

#define PLUGIN_VERSION 4
#define FILTER_VERSION 1

typedef struct rct_plugin_header {
	uint16 version;
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

#if NO_RCT2
	rct_ride_entry				*gRideEntries[128];
	rct_small_scenery_entry		*gSmallSceneryEntries[252];
	rct_large_scenery_entry		*gLargeSceneryEntries[128];
	rct_wall_scenery_entry		*gWallSceneryEntries[128];
	rct_banner					*gBannerSceneryEntries[32];
	rct_footpath_entry			*gFootpathEntries[16];
	rct_path_bit_scenery_entry	*gFootpathAdditionEntries[15];
	rct_scenery_set_entry		*gSceneryGroupEntries[19];
	rct_entrance_type			*gParkEntranceEntries[1];
	rct_water_type				*gWaterEntries[1];
	rct_stex_entry				*gStexEntries[1];
#endif


// 0x98D97C chunk address', 0x98D980 object_entries
const rct_object_entry_group object_entry_groups[] = {
	(void**)(gRideEntries				), (rct_object_entry_extended*)(0x00F3F03C             ),	// rides
	(void**)(gSmallSceneryEntries		), (rct_object_entry_extended*)(0x00F3F03C + (128 * 20)),	// small scenery	0x009AD1A4, 0xF2FA3C
	(void**)(gLargeSceneryEntries		), (rct_object_entry_extended*)(0x00F3F03C + (380 * 20)),	// large scenery	0x009AD594, 0xF40DEC
	(void**)(gWallSceneryEntries		), (rct_object_entry_extended*)(0x00F3F03C + (508 * 20)),	// walls			0x009AD794, 0xF417EC
	(void**)(gBannerSceneryEntries		), (rct_object_entry_extended*)(0x00F3F03C + (636 * 20)),	// banners			0x009AD994, 0xF421EC
	(void**)(gFootpathEntries			), (rct_object_entry_extended*)(0x00F3F03C + (668 * 20)),	// paths			0x009ADA14, 0xF4246C
	(void**)(gFootpathAdditionEntries	), (rct_object_entry_extended*)(0x00F3F03C + (684 * 20)),	// path bits		0x009ADA54, 0xF425AC
	(void**)(gSceneryGroupEntries		), (rct_object_entry_extended*)(0x00F3F03C + (699 * 20)),	// scenery sets		0x009ADA90, 0xF426D8
	(void**)(gParkEntranceEntries		), (rct_object_entry_extended*)(0x00F3F03C + (718 * 20)),	// park entrance	0x009ADADC, 0xF42854
	(void**)(gWaterEntries				), (rct_object_entry_extended*)(0x00F3F03C + (719 * 20)),	// water			0x009ADAE0, 0xF42868
	(void**)(gStexEntries				), (rct_object_entry_extended*)(0x00F3F03C + (720 * 20))	// scenario text	0x009ADAE4, 0xF4287C
};

static int object_list_cache_load(int totalFiles, uint64 totalFileSize, int fileDateModifiedChecksum);
static int object_list_cache_save(int fileCount, uint64 totalFileSize, int fileDateModifiedChecksum, int currentItemOffset);

void object_list_create_hash_table();
static uint32 install_object_entry(rct_object_entry* entry, rct_object_entry* installed_entry, const char* path, rct_object_filters* filter);
static void load_object_filter(rct_object_entry* entry, uint8* chunk, rct_object_filters* filter);

static rct_object_filters *_installedObjectFilters = NULL;

rct_stex_entry *gStexTempChunk;

uint32 gInstalledObjectsCount;
rct_object_entry *gInstalledObjects;
uint32 gNumInstalledRCT2Objects;
uint32 gNumInstalledCustomObjects;

void *gLastLoadedObjectChunkData;

static void get_plugin_path(utf8 *outPath)
{
	platform_get_user_directory(outPath, NULL);
	strcat(outPath, "plugin.dat");
}

static uintptr_t object_get_length_cached(const rct_object_entry **entryCache, const size_t index)
{
	return (uintptr_t)entryCache[index + 1] - (uintptr_t)entryCache[index];
}

static rct_object_entry **_entryCache = NULL;

static int object_comparator(const void *left, const void *right)
{
	const size_t leftIndex  = *(const size_t *)left;
	const size_t rightIndex = *(const size_t *)right;
	const char *leftName  = object_get_name(_entryCache[leftIndex]);
	const char *rightName = object_get_name(_entryCache[rightIndex]);
	return strcmp(leftName, rightName);
}

static void object_list_sort()
{
	rct_object_entry **objectBuffer, *newBuffer, *entry, *destEntry;
	rct_object_filters *newFilters = NULL, *destFilter = NULL;
	int numObjects, bufferSize;
	size_t entrySize;

	objectBuffer = &gInstalledObjects;
	numObjects = gInstalledObjectsCount;


	_entryCache = malloc((numObjects + 1)* sizeof(rct_object_entry*));
	size_t *sortLUT = malloc((numObjects + 1) * sizeof(size_t));
	entry = *objectBuffer;
	// This loop initialises entry cache, so it doesn't have to be called 17M
	// times, but only a few thousand.
	int i = 0;
	do {
		_entryCache[i] = entry;
		sortLUT[i] = i;
	} while (i++ < numObjects && (entry = object_get_next(entry)));
	qsort(sortLUT, numObjects, sizeof(size_t), object_comparator);

	// Get buffer size
	bufferSize = (uintptr_t)entry - (uintptr_t)*objectBuffer;

	// Create new buffer
	newBuffer = (rct_object_entry*)malloc(bufferSize);
	destEntry = newBuffer;
	if (_installedObjectFilters) {
		newFilters = malloc(numObjects * sizeof(rct_object_filters));
		destFilter = newFilters;
	}

	// Copy over sorted objects
	for (int i = 0; i < numObjects; i++) {
		entrySize = object_get_length_cached((const rct_object_entry **)_entryCache, sortLUT[i]);
		memcpy(destEntry, _entryCache[sortLUT[i]], entrySize);
		destEntry = (rct_object_entry*)((uintptr_t)destEntry + entrySize);
		if (_installedObjectFilters)
			destFilter[i] = _installedObjectFilters[sortLUT[i]];
	}
	free(_entryCache);
	free(sortLUT);

	// Replace old buffer
	free(*objectBuffer);
	*objectBuffer = newBuffer;
	if (_installedObjectFilters) {
		free(_installedObjectFilters);
		_installedObjectFilters = newFilters;
	}
}

static uint32 object_list_count_custom_objects()
{
	uint32 numCustomObjects = 0;
	rct_object_entry *object = gInstalledObjects;
	for (uint32 i = 0; i < gInstalledObjectsCount; i++) {
		if ((object->flags & 0xF0) == 0) {
			numCustomObjects++;
		}
		object = object_get_next(object);
	}

	gNumInstalledCustomObjects = numCustomObjects;
	return numCustomObjects;
}

/**
 *
 *  rct2: 0x006A93CD
 */
static void object_list_examine()
{
	object_list_count_custom_objects();
	object_list_sort();
	object_list_create_hash_table();
}

/**
 *
 *  rct2: 0x006A9FC0
 */
void reset_loaded_objects()
{
	reset_type_to_ride_entry_index_map();

	gTotalNoImages = 0xF26E;

	for (int type = 0; type < 11; ++type){
		for (int j = 0; j < object_entry_group_counts[type]; j++){
			uint8* chunk = object_entry_groups[type].chunks[j];
			if (chunk != (uint8*)-1)
				object_load(type, chunk, j);
		}
	}
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
	enumFileHandle = platform_enumerate_files_begin(gRCT2AddressObjectDataPath);
	if (enumFileHandle == INVALID_HANDLE)
		return 0;

	while (platform_enumerate_files_next(enumFileHandle, &enumFileInfo)) {
		totalFiles++;
		totalFileSize += enumFileInfo.size;
		fileDateModifiedChecksum ^=
			(uint32)(enumFileInfo.last_modified >> 32) ^
			(uint32)(enumFileInfo.last_modified & 0xFFFFFFFF);
		fileDateModifiedChecksum = ror32(fileDateModifiedChecksum, 5);
	}
	platform_enumerate_files_end(enumFileHandle);

	*outTotalFiles = totalFiles;
	*outTotalFileSize = totalFileSize;
	*outFileDateModifiedChecksum = fileDateModifiedChecksum;
	return 1;
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

	int ok = object_list_query_directory(&totalFiles, &totalFileSize, &fileDateModifiedChecksum);
	if (ok != 1) {
		return;
	}

	// Would move this into cache load, but its used further on
	totalFiles = ror32(totalFiles, 24);
	totalFiles = (totalFiles & ~0xFF) | 1;
	totalFiles = rol32(totalFiles, 24);

	if (object_list_cache_load(totalFiles, totalFileSize, fileDateModifiedChecksum)) {
		return;
	}

	// Dispose installed object list
	reset_loaded_objects();
	SafeFree(gInstalledObjects);

	gInstalledObjectsCount = 0;
	gInstalledObjects = (rct_object_entry*)malloc(4096);
	if (gInstalledObjects == NULL) {
		log_error("Failed to allocate memory for object list");
		rct2_exit_reason(835, 3162);
		return;
	}

	uint32 fileCount = 0;
	uint32 objectCount = 0;
	size_t currentEntryOffset = 0;
	gNumInstalledRCT2Objects = 0;

	log_verbose("building cache of available objects...");

	if (_installedObjectFilters) {
		free(_installedObjectFilters);
		_installedObjectFilters = NULL;
	}

	enumFileHandle = platform_enumerate_files_begin(gRCT2AddressObjectDataPath);
	if (enumFileHandle != INVALID_HANDLE) {
		size_t installedObjectsCapacity = 4096;
		while (platform_enumerate_files_next(enumFileHandle, &enumFileInfo)) {
			fileCount++;

			if ((installedObjectsCapacity - currentEntryOffset) <= 2842){
				installedObjectsCapacity += 4096;
				gInstalledObjects = (rct_object_entry*)realloc(gInstalledObjects, installedObjectsCapacity);
				if (gInstalledObjects == NULL) {
					log_error("Failed to allocate memory for object list");
					rct2_exit_reason(835, 3162);
					return;
				}
			}

			char path[MAX_PATH];
			substitute_path(path, gRCT2AddressObjectDataPath, enumFileInfo.path);

			rct_object_entry entry;
			if (object_load_entry(path, &entry)) {
				_installedObjectFilters = realloc(_installedObjectFilters, sizeof(rct_object_filters) * (objectCount + 1));

				rct_object_entry *installedEntry = (rct_object_entry*)((size_t)gInstalledObjects + currentEntryOffset);
				rct_object_filters filter;
				size_t newEntrySize = install_object_entry(&entry, installedEntry, enumFileInfo.path, &filter);
				if (newEntrySize != 0) {
					_installedObjectFilters[objectCount] = filter;
					objectCount++;
					currentEntryOffset += newEntrySize;
				}
			}
		}
		platform_enumerate_files_end(enumFileHandle);
	}

	reset_loaded_objects();

	object_list_cache_save(fileCount, totalFileSize, fileDateModifiedChecksum, currentEntryOffset);

	// Reload track list
	ride_list_item ride_list;
	ride_list.entry_index = 0xFC;
	ride_list.type = 0xFC;
	// track_load_list(ride_list);

	object_list_examine();
}

static int object_list_cache_load(int totalFiles, uint64 totalFileSize, int fileDateModifiedChecksum)
{
	char path[MAX_PATH];
	SDL_RWops *file;
	rct_plugin_header pluginHeader;
	uint32 filterVersion = 0;

	log_verbose("loading object list cache (plugin.dat)");

	get_plugin_path(path);
	file = SDL_RWFromFile(path, "rb");
	if (file == NULL) {
		log_verbose("Unable to load %s", path);
		return 0;
	}

	if (SDL_RWread(file, &pluginHeader, sizeof(rct_plugin_header), 1) == 1) {
		// Check if object repository has changed in anyway
		if (
			pluginHeader.version == PLUGIN_VERSION &&
			pluginHeader.total_files == totalFiles &&
			pluginHeader.total_file_size == totalFileSize &&
			pluginHeader.date_modified_checksum == fileDateModifiedChecksum
		) {
			// Dispose installed object list
			SafeFree(gInstalledObjects);

			// Read installed object list
			gInstalledObjects = (rct_object_entry*)malloc(pluginHeader.object_list_size);
			if (SDL_RWread(file, gInstalledObjects, pluginHeader.object_list_size, 1) == 1) {
				gInstalledObjectsCount = pluginHeader.object_list_no_items;

				if (pluginHeader.object_list_no_items != (pluginHeader.total_files & 0xFFFFFF))
					log_error("Potential mismatch in file numbers. Possible corrupt file. Consider deleting plugin.dat.");

				if (SDL_RWread(file, &filterVersion, sizeof(filterVersion), 1) == 1) {
					if (filterVersion == FILTER_VERSION) {
						if (_installedObjectFilters != NULL) {
							free(_installedObjectFilters);
						}
						_installedObjectFilters = malloc(sizeof(rct_object_filters) * pluginHeader.object_list_no_items);
						if (SDL_RWread(file, _installedObjectFilters, sizeof(rct_object_filters) * pluginHeader.object_list_no_items, 1) == 1) {
							SDL_RWclose(file);
							reset_loaded_objects();
							object_list_examine();
							return 1;
						}
					}
				}
				log_info("Filter version updated... updating object list cache");
			}
		}
		else if (pluginHeader.version != PLUGIN_VERSION) {
			log_info("Object list cache version different... updating");
		}
		else if (pluginHeader.total_files != totalFiles) {
			int fileCount = totalFiles - pluginHeader.total_files;
			if (fileCount < 0) {
				log_info("%d object removed... updating object list cache", abs(fileCount));
			} else {
				log_info("%d object added... updating object list cache", fileCount);
			}
		} else if (pluginHeader.total_file_size != totalFileSize) {
			log_info("Objects files size changed... updating object list cache");
		} else if (pluginHeader.date_modified_checksum != fileDateModifiedChecksum) {
			log_info("Objects files have been updated... updating object list cache");
		}

		SDL_RWclose(file);
		return 0;
	}

	SDL_RWclose(file);

	log_error("loading object list cache failed");
	return 0;
}

static int object_list_cache_save(int fileCount, uint64 totalFileSize, int fileDateModifiedChecksum, int currentItemOffset)
{
	utf8 path[MAX_PATH];
	SDL_RWops *file;
	rct_plugin_header pluginHeader;
	uint32 filterVersion = FILTER_VERSION;

	log_verbose("saving object list cache (plugin.dat)");

	pluginHeader.version = PLUGIN_VERSION;
	pluginHeader.total_files = fileCount | 0x01000000;
	pluginHeader.total_file_size = (uint32)totalFileSize;
	pluginHeader.date_modified_checksum = fileDateModifiedChecksum;
	pluginHeader.object_list_size = currentItemOffset;
	pluginHeader.object_list_no_items = gInstalledObjectsCount;

	get_plugin_path(path);
	file = SDL_RWFromFile(path,"wb");
	if (file == NULL) {
		log_error("Failed to save %s", path);
		return 0;
	}

	SDL_RWwrite(file, &pluginHeader, sizeof(rct_plugin_header), 1);
	SDL_RWwrite(file, gInstalledObjects, pluginHeader.object_list_size, 1);
	SDL_RWwrite(file, &filterVersion, sizeof(filterVersion), 1);
	SDL_RWwrite(file, _installedObjectFilters, sizeof(rct_object_filters) * gInstalledObjectsCount, 1);
	SDL_RWclose(file);
	return 1;
}

int check_object_entry(rct_object_entry *entry)
{
	uint32 *dwords = (uint32*)entry;
	return (0xFFFFFFFF & dwords[0] & dwords[1] & dwords[2] & dwords[3]) + 1 != 0;
}

/**
 *
 *  rct2: 0x006AB344
 */
void object_create_identifier_name(char* string_buffer, const rct_object_entry* object)
{
	for (uint8 i = 0; i < 8; ++i){
		if (object->name[i] != ' '){
			*string_buffer++ = object->name[i];
		}
	}

	*string_buffer++ = '/';

	for (uint8 i = 0; i < 4; ++i){
		uint8 flag_part = (object->flags >> (i * 8)) & 0xFF;
		*string_buffer++ = RCT2_ADDRESS(0x0098DA64, char)[flag_part >> 4];
		*string_buffer++ = RCT2_ADDRESS(0x0098DA64, char)[flag_part & 0xF];
	}

	for (uint8 i = 0; i < 4; ++i){
		uint8 checksum_part = (object->checksum >> (i * 8)) & 0xFF;
		*string_buffer++ = RCT2_ADDRESS(0x0098DA64, char)[checksum_part >> 4];
		*string_buffer++ = RCT2_ADDRESS(0x0098DA64, char)[checksum_part & 0xF];
	}
	*string_buffer++ = '\0';
}

/**
 *
 *  rct2: 0x675827
 */
void set_load_objects_fail_reason()
{
	rct_object_entry *object;
	memcpy(&object, gCommonFormatArgs, sizeof(rct_object_entry*));
	
	int expansion = (object->flags & 0xFF) >> 4;
	if (expansion == 0 ||
		expansion == 8 ||
		RCT2_GLOBAL(RCT2_ADDRESS_EXPANSION_FLAGS, uint16) & (1 << expansion)
	) {
		char* string_buffer = RCT2_ADDRESS(0x9BC677, char);

		format_string(string_buffer, STR_MISSING_OBJECT_DATA_ID, 0);

		object_create_identifier_name(string_buffer, object);
		gErrorType = ERROR_TYPE_FILE_LOAD;
		gErrorStringId = STR_PLACEHOLDER;
		return;
	}

	rct_string_id expansionNameId;
	switch(expansion) {
		case 1: // Wacky Worlds
			expansionNameId = STR_OBJECT_FILTER_WW;
			break;
		case 2: // Time Twister
			expansionNameId = STR_OBJECT_FILTER_TT;
			break;
		default:
			gErrorType = ERROR_TYPE_FILE_LOAD;
			gErrorStringId = STR_REQUIRES_AN_ADDON_PACK;
			return;
	}

	char* string_buffer = RCT2_ADDRESS(0x9BC677, char);

	format_string(string_buffer, STR_REQUIRES_THE_FOLLOWING_ADDON_PACK, &expansionNameId);

	gErrorType = ERROR_TYPE_FILE_LOAD;
	gErrorStringId = STR_PLACEHOLDER;
}

/**
 *
 *  rct2: 0x006AA0C6
 */
bool object_read_and_load_entries(SDL_RWops* rw)
{
	// Read all the object entries
	rct_object_entry *entries = malloc(OBJECT_ENTRY_COUNT * sizeof(rct_object_entry));
	sawyercoding_read_chunk(rw, (uint8*)entries);
	bool result = object_load_entries(entries);
	free(entries);
	return result;
}

bool object_load_entries(rct_object_entry* entries)
{
	log_verbose("loading required objects");

	object_unload_all();

	bool loadFailed = false;
	// Load each object
	for (int i = 0; i < OBJECT_ENTRY_COUNT; i++) {
		if (!check_object_entry(&entries[i])) {
			continue;
		}

		// Get entry group index
		int entryGroupIndex = i;
		for (int j = 0; j < countof(object_entry_group_counts); j++) {
			if (entryGroupIndex < object_entry_group_counts[j])
				break;
			entryGroupIndex -= object_entry_group_counts[j];
		}

		// Load the obect
		if (!object_load_chunk(entryGroupIndex, &entries[i], NULL)) {
			log_error("failed to load entry: %.8s", entries[i].name);
			memcpy(gCommonFormatArgs, &entries[i], sizeof(rct_object_entry));
			loadFailed = true;
		}
	}

	if (loadFailed) {
		object_unload_all();
		return false;
	}

	log_verbose("finished loading required objects");
	return true;
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
			if (object_entry_groups[i].chunks[j] != (uint8*)0xFFFFFFFF)
				object_unload_chunk((rct_object_entry*)&object_entry_groups[i].entries[j]);

	reset_loaded_objects();
}



uint32 _installedObjectHashTableSize;
rct_object_entry ** _installedObjectHashTable = NULL;

uint32 _installedObjectHashTableCollisions;

uint32 object_get_hash_code(rct_object_entry *object)
{
	uint32 hash = 5381;
	for (int i = 0; i < 8; i++)
		hash = ((hash << 5) + hash) + object->name[i];

	return hash;
}

void object_list_create_hash_table()
{
	rct_object_entry *installedObject;
	int numInstalledObjects = gInstalledObjectsCount;

	if (_installedObjectHashTable != NULL)
		free(_installedObjectHashTable);

	_installedObjectHashTableSize = max(8192, numInstalledObjects * 4);
	_installedObjectHashTable = calloc(_installedObjectHashTableSize, sizeof(rct_object_entry*));
	_installedObjectHashTableCollisions = 0;

	installedObject = gInstalledObjects;
	for (int i = 0; i < numInstalledObjects; i++) {
		uint32 hash = object_get_hash_code(installedObject);
		uint32 index = hash % _installedObjectHashTableSize;

		// Find empty slot
		while (_installedObjectHashTable[index] != NULL) {
			_installedObjectHashTableCollisions++;
			index++;
			if (index >= _installedObjectHashTableSize) index = 0;
		}

		// Set hash table slot
		_installedObjectHashTable[index] = installedObject;

		// Next installed object
		installedObject = object_get_next(installedObject);
	}
}

/**
 *
 *  rct2: 0x006A9DA2
 * bl = entry_index
 * ecx = entry_type
 */
int find_object_in_entry_group(rct_object_entry* entry, uint8* entry_type, uint8* entry_index){
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

rct_object_entry *object_list_find_by_name(const char * name)
{
	rct_object_entry entry;
	memcpy(entry.name, name, 8);

	uint32 hash = object_get_hash_code(&entry);
	uint32 index = hash % _installedObjectHashTableSize;

	while (_installedObjectHashTable[index] != NULL) {
		if (memcmp(_installedObjectHashTable[index]->name, entry.name, 8) == 0)
			return _installedObjectHashTable[index];

		index++;
		if (index >= _installedObjectHashTableSize) index = 0;
	}

	return NULL;
}

rct_object_entry *object_list_find(rct_object_entry *entry)
{
	uint32 hash = object_get_hash_code(entry);
	uint32 index = hash % _installedObjectHashTableSize;

	while (_installedObjectHashTable[index] != NULL) {
		if (object_entry_compare( _installedObjectHashTable[index], entry))
			return _installedObjectHashTable[index];

		index++;
		if (index >= _installedObjectHashTableSize) index = 0;
	}

	return NULL;
}

rct_string_id object_get_name_string_id(rct_object_entry *entry, const void *chunk)
{
	int objectType = entry->flags & 0x0F;
	switch (objectType) {
	case OBJECT_TYPE_RIDE:
		return ((rct_ride_entry*)chunk)->name;
	case OBJECT_TYPE_SMALL_SCENERY:
	case OBJECT_TYPE_LARGE_SCENERY:
	case OBJECT_TYPE_WALLS:
	case OBJECT_TYPE_BANNERS:
	case OBJECT_TYPE_PATH_BITS:
		return ((rct_scenery_entry*)chunk)->name;
	case OBJECT_TYPE_PATHS:
		return ((rct_footpath_entry*)chunk)->string_idx;
	case OBJECT_TYPE_SCENERY_SETS:
		return ((rct_scenery_set_entry*)chunk)->name;
	case OBJECT_TYPE_PARK_ENTRANCE:
		return ((rct_entrance_type*)chunk)->string_idx;
	case OBJECT_TYPE_WATER:
		return ((rct_water_type*)chunk)->string_idx;
	case OBJECT_TYPE_SCENARIO_TEXT:
		return ((rct_stex_entry*)chunk)->scenario_name;
	default:
		return STR_NONE;
	}
}

/**
 * Installs an  object_entry at the desired installed_entry address
 * Returns the size of the new entry. Will return 0 on failure.
 */
static uint32 install_object_entry(rct_object_entry* entry, rct_object_entry* installed_entry, const char* path, rct_object_filters* filter){
	uint8* installed_entry_pointer = (uint8*) installed_entry;

	/** Copy all known information into the install entry **/
	memcpy(installed_entry_pointer, entry, sizeof(rct_object_entry));
	installed_entry_pointer += sizeof(rct_object_entry);

	strcpy((char *)installed_entry_pointer, path);
	while (*installed_entry_pointer++);

	// Chunk size is set to unknown
	*((sint32*)installed_entry_pointer) = -1;
	// No unknown objects set to 0
	*(installed_entry_pointer + 4) = 0;
	// No theme objects set to 0
	*((sint32*)(installed_entry_pointer + 5)) = 0;
	*((uint16*)(installed_entry_pointer + 9)) = 0;
	*((uint32*)(installed_entry_pointer + 11)) = 0;

	gTotalNoImages = 0xF26E;

	gInstalledObjectsCount++;

	// This is a variable used by object_load to decide if it should
	// use object_paint on the entry.
	RCT2_GLOBAL(0x009ADAFD, uint8) = 1;

	// Probably used by object paint.
	RCT2_GLOBAL(0x009ADAF4, uint32) = 0xF42BDB;

	/** Use object_load_file to fill in missing chunk information **/
	int chunk_size;
	if (!object_load_file(-1, entry, &chunk_size, installed_entry)){
		log_error("Object Load File failed. Potentially corrupt file: %.8s", entry->name);
		RCT2_GLOBAL(0x009ADAF4, sint32) = -1;
		RCT2_GLOBAL(0x009ADAFD, uint8) = 0;
		gInstalledObjectsCount--;
		return 0;
	}

	// See above note
	RCT2_GLOBAL(0x009ADAF4, sint32) = -1;
	RCT2_GLOBAL(0x009ADAFD, uint8) = 0;

	if ((entry->flags & 0xF0) == 0x80) {
		gNumInstalledRCT2Objects++;
		if (gNumInstalledRCT2Objects > 772){
			log_error("Incorrect number of vanilla RCT2 objects.");
			gNumInstalledRCT2Objects--;
			gInstalledObjectsCount--;
			object_unload_chunk(entry);
			return 0;
		}
	}
	*((sint32*)installed_entry_pointer) = chunk_size;
	installed_entry_pointer += 4;

	uint8* chunk = (uint8*)gLastLoadedObjectChunkData; // Loaded in object_load

	load_object_filter(entry, chunk, filter);

	// Always extract only the vehicle type, since the track type is always displayed in the left column, to prevent duplicate track names.
	rct_string_id nameStringId = object_get_name_string_id(entry, chunk);
	if (nameStringId == STR_NONE) {
		nameStringId = (rct_string_id)RCT2_GLOBAL(RCT2_ADDRESS_CURR_OBJECT_BASE_STRING_ID, uint32);
	}

	strcpy((char *)installed_entry_pointer, language_get_string(nameStringId));
	while (*installed_entry_pointer++);

	// This is deceptive. Due to setting the total no images earlier to 0xF26E
	// this is actually the no_images in this entry.
	*((uint32*)installed_entry_pointer) = gTotalNoImages - 0xF26E;
	installed_entry_pointer += 4;

	uint8* esi = RCT2_ADDRESS(0x00F42BDB, uint8);
	uint8 num_unk_objects = *esi++;
	*installed_entry_pointer++ = num_unk_objects;
	if (num_unk_objects > 0) {
		memcpy(installed_entry_pointer, esi, num_unk_objects * sizeof(rct_object_entry));
		installed_entry_pointer += num_unk_objects * sizeof(rct_object_entry);
		esi += num_unk_objects * sizeof(rct_object_entry);
	}

	uint8 no_theme_objects = *esi++;
	*installed_entry_pointer++ = no_theme_objects;
	if (no_theme_objects > 0) {
		memcpy(installed_entry_pointer, esi, no_theme_objects * sizeof(rct_object_entry));
		installed_entry_pointer += no_theme_objects * sizeof(rct_object_entry);
	}

	*((uint32*)installed_entry_pointer) = RCT2_GLOBAL(0x00F433DD, uint32);
	installed_entry_pointer += 4;

	uint32 size_of_object = installed_entry_pointer - (uint8*)installed_entry;

	object_unload_chunk(entry);

	return size_of_object;
}

static void load_object_filter(rct_object_entry* entry, uint8* chunk, rct_object_filters* filter)
{
	rct_ride_entry *rideType;
	rct_ride_filters *rideFilter;

	switch (entry->flags & 0xF) {
	case OBJECT_TYPE_RIDE:
		rideType = ((rct_ride_entry*)chunk);
		rideFilter = &(filter->ride);

		rideFilter->category[0] = rideType->category[0];
		rideFilter->category[1] = rideType->category[1];

		for (int i = 0; i < 3; i++) {
			rideFilter->ride_type = rideType->ride_type[i];
			if (rideFilter->ride_type != 255)
				break;
		}
		break;
	case OBJECT_TYPE_SMALL_SCENERY:
	case OBJECT_TYPE_LARGE_SCENERY:
	case OBJECT_TYPE_WALLS:
	case OBJECT_TYPE_BANNERS:
	case OBJECT_TYPE_PATHS:
	case OBJECT_TYPE_PATH_BITS:
	case OBJECT_TYPE_SCENERY_SETS:
	case OBJECT_TYPE_PARK_ENTRANCE:
	case OBJECT_TYPE_WATER:
	case OBJECT_TYPE_SCENARIO_TEXT:
		break;
	}
}

rct_object_filters *get_object_filter(int index)
{
	return &_installedObjectFilters[index];
}

void object_list_init()
{
	for (uint8 objectType = 0; objectType < OBJECT_ENTRY_GROUP_COUNT; objectType++) {
		for (size_t i = 0; i < (size_t)object_entry_group_counts[objectType]; i++) {
			object_entry_groups[objectType].chunks[i] = (void*)-1;
		}
	}
}

void *get_loaded_object_entry(size_t index)
{
	uint8 objectType = OBJECT_TYPE_RIDE;
	for (size_t i = 0; i < OBJECT_ENTRY_GROUP_COUNT; i++) {
		size_t groupCount = object_entry_group_counts[i];
		if (index >= groupCount) {
			index -= object_entry_group_counts[i];
			objectType++;
		} else {
			break;
		}
	}

	void *entry = object_entry_groups[objectType].chunks[index];
	return entry;
}
