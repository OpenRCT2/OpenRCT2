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

#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "addresses.h"
#include "object.h"
#include "sawyercoding.h"

int object_entry_compare(rct_object_entry *a, rct_object_entry *b);
int object_calculate_checksum(rct_object_entry *entry, char *data, int dataLength);
int object_paint(int type, int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp);
rct_object_entry *object_get_next(rct_object_entry *entry);
/**
 * 
 *  rct2: 0x006A985D
 */
int object_load(int groupIndex, rct_object_entry *entry)
{
	RCT2_GLOBAL(0xF42B64, uint32) = groupIndex;

	//part of 6a9866
	rct_object_entry *installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	if (!(RCT2_GLOBAL(0xF42B6C, uint32))){
		RCT2_GLOBAL(0xF42BD9, uint8) = 0;
		return 1;
	}
	for (int i = 0; i < RCT2_GLOBAL(0x00F42B6C, sint32); i++) {
		if (object_entry_compare(installedObject, entry)){

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
					do {
						pos++;
					} while (*(pos - 1) != 0);

					// Read chunk
					int chunkSize = *((uint32*)pos);
					char *chunk;
					
					if (chunkSize == 0xFFFFFFFF) {
						chunk = rct2_malloc(0x600000);
						chunkSize = sawyercoding_read_chunk_variable(file, &chunk);
						chunk = rct2_realloc(chunk, chunkSize);
					}
					else {
						chunk = rct2_malloc(chunkSize);
						chunkSize = sawyercoding_read_chunk_variable(file, &chunk);
					}
					fclose(file);

					// Calculate and check checksum
					if (object_calculate_checksum(&openedEntry, chunk, chunkSize) != openedEntry.checksum) {
						RCT2_GLOBAL(0x00F42BD9, uint8) = 2;
						rct2_free(chunk);
						return 0;
					}

					if (object_paint(openedEntry.flags & 0x0F, 2, 0, openedEntry.flags & 0x0F, 0, (int)chunk, 0, 0)) {
						RCT2_GLOBAL(0x00F42BD9, uint8) = 3;
						rct2_free(chunk);
						return 0;
					}

					int yyy = RCT2_GLOBAL(0x009ADAF0, uint32);

					if (yyy >= 0x4726E){
						RCT2_GLOBAL(0x00F42BD9, uint8) = 4;
						rct2_free(chunk);
						return 0;
					}
					//B84 is openedEntry
					int ebp = openedEntry.flags & 0x0F;
					int esi = RCT2_ADDRESS(0x98D97C, uint32)[ebp * 2];
					int ecx = groupIndex;
					if (ecx == -1){
						for (int ecx = 0; ((sint32*)esi)[ecx] != -1; ecx++){
							if ((ecx + 1) >= RCT2_ADDRESS(0x98DA00, uint16)[ebp]){
								RCT2_GLOBAL(0x00F42BD9, uint8) = 5;
								rct2_free(chunk);
								return 0;
							}
						}
					}
					((char**)esi)[ecx] = chunk;

					int* edx = (int*)( ecx * 20 + RCT2_ADDRESS(0x98D980, uint32)[ebp * 2]);
					memcpy(edx, (int*)&openedEntry, 20);
					if (RCT2_GLOBAL(0x9ADAFD, uint8) == 0)return 1;
					object_paint(ecx, 0, ecx, ebp, 0, (int)chunk, 0, 0);
					return 1;
				}
				fclose(file);
			}
		}
		installedObject = object_get_next(installedObject);
	}
	//6a991f
	object_load(groupIndex, entry);

	return !(RCT2_CALLPROC_X(0x006A985D, 0, 0, groupIndex, 0, 0, 0, (int)entry) & 0x400);
}

/**
 * 
 *  rct2: 0x006A9CAF
 */
void object_unload(int groupIndex, rct_object_entry_extended *entry)
{
	RCT2_CALLPROC_X(0x006A9CAF, 0, groupIndex, 0, 0, 0, 0, (int)entry);
}

static int object_entry_compare(rct_object_entry *a, rct_object_entry *b)
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

int object_calculate_checksum(rct_object_entry *entry, char *data, int dataLength)
{
	int i;
	char *eee = (char*)entry;
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
	if (type == 10){
		if (eax == 0) return object_scenario_load_custom_text((char*)esi);
	}
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
	for (i = 0; i < RCT2_GLOBAL(0x00F42B6C, sint32); i++) {
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
					do {
						pos++;
					} while (*(pos - 1) != 0);

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
	char *pos = (char*)entry;

	// Skip sizeof(rct_object_entry)
	pos += 16;

	// Skip filename
	do {
		pos++;
	} while (*(pos - 1) != 0);

	// Skip 
	pos += 4;

	// Skip name
	do {
		pos++;
	} while (*(pos - 1) != 0);

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