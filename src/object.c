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
#include "platform/osinterface.h"
#include "platform/platform.h"
#include "util/sawyercoding.h"

int object_load_entry(const char *path, rct_object_entry *outEntry)
{
	FILE *file;
	
	file = fopen(path, "rb");
	if (file == NULL)
		return 0;

	if (fread(outEntry, sizeof(rct_object_entry), 1, file) != 1) {
		fclose(file);
		return 0;
	}

	fclose(file);
	return 1;
}

int object_load_file(int groupIndex, const rct_object_entry *entry, int* chunkSize, const rct_object_entry *installedObject)
{
	uint8 objectType;
	rct_object_entry openedEntry;
	char path[260];
	FILE *file;

	subsitute_path(path, RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), (char*)installedObject + 16);

	// log_verbose("loading object, %s", path);

	file = fopen(path, "rb");
	if (file == NULL)
		return 0;

	fread(&openedEntry, sizeof(rct_object_entry), 1, file);
	if (!object_entry_compare(&openedEntry, entry)) {
		fclose(file);
		return 0;
	}

	// Get chunk size
	uint8 *installedObject_pointer = (uint8*)installedObject + 16;
	// Skip file name
	while (*installedObject_pointer++);

	// Read chunk size
	*chunkSize = *((uint32*)installedObject_pointer);
	char *chunk;
					
	if (*chunkSize == 0xFFFFFFFF) {
		chunk = rct2_malloc(0x600000);
		*chunkSize = sawyercoding_read_chunk(file, chunk);
		chunk = rct2_realloc(chunk, *chunkSize);
	}
	else {
		chunk = rct2_malloc(*chunkSize);
		*chunkSize = sawyercoding_read_chunk(file, chunk);
	}
	fclose(file);

					

	// Calculate and check checksum
	if (object_calculate_checksum(&openedEntry, chunk, *chunkSize) != openedEntry.checksum) {
		log_error("Object Load failed due to checksum failure.");
		RCT2_GLOBAL(0x00F42BD9, uint8) = 2;
		rct2_free(chunk);
		return 0;
	}

	objectType = openedEntry.flags & 0x0F;

	if (object_paint(objectType, 2, 0, objectType, 0, (int)chunk, 0, 0)) {
		log_error("Object Load failed due to paint failure.");
		RCT2_GLOBAL(0x00F42BD9, uint8) = 3;
		rct2_free(chunk);
		return 0;
	}

	int yyy = RCT2_GLOBAL(0x009ADAF0, uint32);

	if (yyy >= 0x4726E){
		log_error("Object Load failed due to yyy failure.");
		RCT2_GLOBAL(0x00F42BD9, uint8) = 4;
		rct2_free(chunk);
		return 0;
	}
	
	uint8** chunk_list = object_entry_groups[objectType].chunks;
	if (groupIndex == -1) {
		for (groupIndex = 0; chunk_list[groupIndex] != (uint8*)-1; groupIndex++) {
			if (groupIndex + 1 >= object_entry_group_counts[objectType]) {
				log_error("Object Load failed due to too many objects of a certain type.");
				RCT2_GLOBAL(0x00F42BD9, uint8) = 5;
				rct2_free(chunk);
				return 0;
			}
		}
	}
	chunk_list[groupIndex] = chunk;

	rct_object_entry_extended* extended_entry = &object_entry_groups[objectType].entries[groupIndex];

	memcpy(extended_entry, &openedEntry, sizeof(rct_object_entry));
	extended_entry->chunk_size = *chunkSize;

	RCT2_GLOBAL(RCT2_ADDRESS_CURR_OBJECT_CHUNK_POINTER, char*) = chunk;

	if (RCT2_GLOBAL(0x9ADAFD, uint8) != 0)
		object_paint(objectType, 0, groupIndex, objectType, 0, (int)chunk, 0, 0);
	return 1;
}

/**
 * 
 *  rct2: 0x006A985D
 */
int object_load(int groupIndex, rct_object_entry *entry, int* chunkSize)
{
	// Alow chunkSize to be null
	int tempChunkSize;
	if (chunkSize == NULL)
		chunkSize = &tempChunkSize;

	RCT2_GLOBAL(0xF42B64, uint32) = groupIndex;

	if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32) == 0) {
		RCT2_GLOBAL(0xF42BD9, uint8) = 0;
		log_error("Object Load failed due to no items installed check.");
		return 1;
	}

	rct_object_entry *installedObject = object_list_find(entry);
	if (installedObject == NULL) {
		log_error("object not installed");
		return 0;
	}

	if (object_load_file(groupIndex, entry, chunkSize, installedObject))
		return 1;

	return 0;
}

/** rct2: 0x006a9f42 
 *  ebx : file
 *  ebp : entry
 */
int sub_6A9F42(FILE *file, rct_object_entry* entry){
	int eax = 0, entryGroupIndex = 0, type = 0, edx = 0, edi = 0, ebp = (int)entry, chunk = 0;
	RCT2_CALLFUNC_X(0x6A9DA2, &eax, &entryGroupIndex, &type, &edx, &chunk, &edi, &ebp);
	if (eax == 0) return 0;

	object_paint(type, 1, entryGroupIndex, type, edx, chunk, edi, ebp);
	

	rct_object_entry_extended* installed_entry = &object_entry_groups[type].entries[entryGroupIndex];
	uint8* dst_buffer = malloc(0x600000);
	memcpy(dst_buffer, (uint8*)installed_entry, sizeof(rct_object_entry));

	uint32 size_dst = sizeof(rct_object_entry);

	sawyercoding_chunk_header chunkHeader;	
	// Encoding type (not used anymore)
	RCT2_GLOBAL(0x9E3CBD, uint8) = object_entry_group_encoding[type];

	chunkHeader.encoding = object_entry_group_encoding[type];
	chunkHeader.length = installed_entry->chunk_size;

	size_dst += sawyercoding_write_chunk_buffer(dst_buffer + sizeof(rct_object_entry), (uint8*)chunk, chunkHeader);
	fwrite(dst_buffer, 1, size_dst, file);

	free(dst_buffer);
	return 1;
}

/**
*
*  rct2: 0x006AA2B7
*/
int object_load_packed(FILE *file)
{
	object_unload_all();

	rct_object_entry* entry = RCT2_ADDRESS(0xF42B84, rct_object_entry);

	fread((void*)entry, 16, 1, file);

	uint8* chunk = rct2_malloc(0x600000);
	uint32 chunkSize = sawyercoding_read_chunk(file, chunk);
	chunk = rct2_realloc(chunk, chunkSize);
	if (chunk == NULL){
		return 0;
	}

	if (object_calculate_checksum(entry, chunk, chunkSize) != entry->checksum){
		rct2_free(chunk);
		return 0;
	}

	if (object_paint(entry->flags & 0x0F, 2, 0, entry->flags & 0x0F, 0, (int)chunk, 0, 0)) {
		rct2_free(chunk);
		return 0;
	}

	int yyy = RCT2_GLOBAL(0x009ADAF0, uint32);

	if (yyy >= 0x4726E){
		rct2_free(chunk);
		return 0;
	}

	int type = entry->flags & 0x0F;

	// ecx
	int entryGroupIndex = 0;

	for (; entryGroupIndex < object_entry_group_counts[type]; entryGroupIndex++){
		if (object_entry_groups[type].chunks[entryGroupIndex] == (uint8*)-1){
			break;
		}
	}

	if (entryGroupIndex == object_entry_group_counts[type]){
		rct2_free(chunk);
		return 0;
	}

	object_entry_groups[type].chunks[entryGroupIndex] = chunk;
	rct_object_entry_extended* edx = &object_entry_groups[type].entries[entryGroupIndex];
	memcpy(edx, (int*)entry, sizeof(rct_object_entry));
	edx->chunk_size = chunkSize;

	//esi
	rct_object_entry *installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32)){
		for (uint32 i = 0; i < RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); ++i){
			if (object_entry_compare(entry, installedObject)){
				object_unload_all();
				return 0;
			}
			installedObject = object_get_next(installedObject);
		}
	}

	//Installing new data
	//format_string(0x141ED68, 3163, 0);
	//Code for updating progress bar removed.

	char path[260];
	char objectPath[13] = { 0 };
	for (int i = 0; i < 8; ++i){
		if (entry->name[i] != ' ')
			objectPath[i] = toupper(entry->name[i]);
		else
			objectPath[i] = '\0';
	}

	subsitute_path(path, RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), objectPath);
	char* last_char = path + strlen(path);
	strcat(path, ".DAT");

	// 
	for (; platform_file_exists(path);){
		for (char* curr_char = last_char - 1;; --curr_char){
			if (*curr_char == '\\'){
				subsitute_path(path, RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), "00000000.DAT");
				char* last_char = path + strlen(path);
				break;
			}
			if (*curr_char < '0') *curr_char = '0';
			else if (*curr_char == '9') *curr_char = 'A';
			else if (*curr_char == 'Z') *curr_char = '0';
			else (*curr_char)++;
			if (*curr_char != '0') break;
		}
	}

	// Removed progress bar code

	// The following section cannot be finished until 6A9F42 is finished
	// Run the game once with vanila rct2 to not reach this part of code.
	log_verbose("Function might not be finished.");
	FILE* obj_file = fopen(path, "wb");
	if (obj_file){
		// Removed progress bar code
		sub_6A9F42(obj_file, entry);
		fclose(obj_file);
		// Removed progress bar code
		object_unload_all();
		// Removed progress bar code
		return 1;
	}
	else{
		object_unload_all();
		return 0;
	}
	//create file
	//6aa48C
	int eax = 1;//, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0, ebp = 0;
	//RCT2_CALLFUNC_X(0x006AA2B7, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return 1;
}

/**
 * 
 *  rct2: 0x006A9CAF
 */
void object_unload(int groupIndex, rct_object_entry_extended *entry)
{
	RCT2_CALLPROC_X(0x006A9CAF, 0, groupIndex, 0, 0, 0, 0, (int)entry);
}

int object_entry_compare(const rct_object_entry *a, const rct_object_entry *b)
{
	if (a->flags & 0xF0) {
		if ((a->flags & 0x0F) != (b->flags & 0x0F))
			return 0;
		if (*((uint32*)a->name) != *((uint32*)b->name))
			return 0;
		if (*((uint32*)(&a->name[4])) != *((uint32*)(&b->name[4])))
			return 0;
	} else {
		if (a->flags != b->flags)
			return 0;
		if (*((uint32*)a->name) != *((uint32*)b->name))
			return 0;
		if (*((uint32*)(&a->name[4])) != *((uint32*)(&b->name[4])))
			return 0;
		if (a->checksum != b->checksum)
			return 0;
	}

	return 1;
}

int object_calculate_checksum(const rct_object_entry *entry, const char *data, int dataLength)
{
	int i;
	const char *eee = (char*)entry;
	int checksum = 0xF369A75B;
	char *ccc = (char*)&checksum;

	*ccc ^= eee[0];
	checksum = rol32(checksum, 11);
	for (i = 4; i < 12; i++) {
		*ccc ^= eee[i];
		checksum = rol32(checksum, 11);
	}
	for (i = 0; i < dataLength; i++) {
		*ccc ^= data[i];
		checksum = rol32(checksum, 11);
	}

	return checksum;
}

/**
 *  rct2: 0x66B355 part 
 *  If al is 0
 *  chunk : esi
 */
int object_scenario_load_custom_text(char* chunk){
	int ebp = (int)(&((uint32*)chunk)[2]);
	int edx = 0;
	int eax, ebx, ecx, edi;
	RCT2_CALLFUNC_X(0x6A9E24, &eax, &ebx, &ecx, &edx, (int*)&chunk, &edi, &ebp);
	*((uint16*)chunk) = eax;
	edx++;
	RCT2_CALLFUNC_X(0x6A9E24, &eax, &ebx, &ecx, &edx, (int*)&chunk, &edi, &ebp);
	*((uint16*)chunk + 1) = eax;
	edx++;
	RCT2_CALLFUNC_X(0x6A9E24, &eax, &ebx, &ecx, &edx, (int*)&chunk, &edi, &ebp);
	*((uint16*)chunk + 2) = eax;

	if (RCT2_GLOBAL(0x9ADAF4, int) == -1)return 0;
	else *(RCT2_GLOBAL(0x9ADAF4, uint32*)) = 0;
	return 1;
}

int object_paint(int type, int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp)
{
	//if (type == OBJECT_TYPE_SCENARIO_TEXT){
	//	if (eax == 0) return object_scenario_load_custom_text((char*)esi);
	//}
	return RCT2_CALLPROC_X(RCT2_ADDRESS(0x0098D9D4, uint32)[type], eax, ebx, ecx, edx, esi, edi, ebp) & 0x100;
}

/**
 * 
 *  rct2: 0x006A9428
 */
int object_get_scenario_text(rct_object_entry *entry)
{
	// RCT2_CALLPROC_X(0x006A9428, 0, 0, 0, 0, 0, 0, (int)entry); return;

	int i;
	rct_object_entry *installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
	for (i = 0; i < RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, sint32); i++) {
		if (object_entry_compare(installedObject, entry)) {
			char path[260];
			char *objectPath = (char*)installedObject + 16;
			subsitute_path(path, RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), objectPath);

			rct_object_entry openedEntry;
			FILE *file = fopen(path, "rb");
			if (file != NULL) {
				fread(&openedEntry, sizeof(rct_object_entry), 1, file);
				if (object_entry_compare(&openedEntry, entry)) {

					// Get chunk size
					char *pos = (char*)installedObject + 16;
					// Skip file name
					while (*pos++);

					// Read chunk
					int chunkSize = *((uint32*)pos);
					char *chunk;
					if (chunkSize == 0xFFFFFFFF) {
						chunk = malloc(0x600000);
						chunkSize = sawyercoding_read_chunk(file, chunk);
						chunk = realloc(chunk, chunkSize);						
					} else {
						chunk = malloc(chunkSize);
						sawyercoding_read_chunk(file, chunk);
					}
					fclose(file);

					// Calculate and check checksum
					if (object_calculate_checksum(&openedEntry, chunk, chunkSize) != openedEntry.checksum) {
						RCT2_GLOBAL(0x00F42BD9, uint8) = 2;
						free(chunk);
						return 0;
					}

					if (object_paint(openedEntry.flags & 0x0F, 2, 0, 0, 0, (int)chunk, 0, 0)) {
						RCT2_GLOBAL(0x00F42BD9, uint8) = 3;
						free(chunk);
						return 0;
					}

					int yyy = RCT2_GLOBAL(0x009ADAF0, uint32);
					RCT2_GLOBAL(0x009ADAF0, uint32) = 0x726E;
					RCT2_GLOBAL(0x009ADAF8, uint32) = (int)chunk;
					*((rct_object_entry*)0x00F42BC8) = openedEntry;

					RCT2_GLOBAL(0x009ADAFC, uint8) = 255;
					RCT2_GLOBAL(0x009ADAFD, uint8) = 1;
					object_paint(openedEntry.flags & 0x0F, 0, 0, 0, 0, (int)chunk, 0, 0);
					RCT2_GLOBAL(0x009ADAFC, uint8) = 0;
					RCT2_GLOBAL(0x009ADAFD, uint8) = 0;
					RCT2_GLOBAL(0x009ADAF0, uint32) = yyy;
					return 1;
				}
				fclose(file);
			}
		}
		installedObject = object_get_next(installedObject);
	}

	RCT2_GLOBAL(0x00F42BD9, uint8) = 0;
	return 0;
}

/**
 * 
 *  rct2: 0x006A982D
 */
void object_free_scenario_text()
{
	if (RCT2_GLOBAL(0x009ADAF8, void*) != NULL) {
		free(RCT2_GLOBAL(0x009ADAF8, void*));
		RCT2_GLOBAL(0x009ADAF8, void*) = NULL;
	}
}

int object_get_length(rct_object_entry *entry)
{
	return (int)object_get_next(entry) - (int)entry;
}

rct_object_entry *object_get_next(rct_object_entry *entry)
{
	uint8 *pos = (uint8*)entry;

	// Skip sizeof(rct_object_entry)
	pos += 16;

	// Skip filename
	while (*pos++);

	// Skip 
	pos += 4;

	// Skip name
	while (*pos++);

	// Skip size of chunk
	pos += 4;

	// Skip 
	pos += *pos++ * 16;

	// Skip theme objects
	pos += *pos++ * 16;

	// Skip 
	pos += 4;

	return (rct_object_entry*)pos;
}