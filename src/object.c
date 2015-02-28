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
#include "util/sawyercoding.h"
#include "drawing/drawing.h"
#include "world/footpath.h"
#include "scenario.h"

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

	log_verbose("loading object, %s", path);

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
	}
	else {
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

/* ebp is made up of 3 pointers: no_elements, unknown, g1_source */
//rct2: 0x006A9ED1
int sub_6A9ED1(uint8_t** ebp)
{
	int result;
	int eax = result = RCT2_GLOBAL(0x9ADAF0, uint32_t);
	int no_elements = ((uint32_t*)(*ebp))[0];

	RCT2_GLOBAL(0x9ADAF0, uint32_t) = no_elements + eax;

	rct_g1_element* g1_dest = &RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[eax];
	rct_g1_element* g1_source = (rct_g1_element*)((*ebp) + 8);
	int ebx = no_elements * sizeof(rct_g1_element) + (uint32)g1_source;

	for (int i = 0; i < no_elements; ++i){
		*g1_dest = *g1_source++;
		g1_dest->offset += ebx;
		g1_dest++;
	}

	(*ebp) += 8;
	(*ebp) += *((uint32_t*)((*ebp) - 4));
	(*ebp) += no_elements * sizeof(rct_g1_element);
	return result;
}

//rct2: 0x006DE83E
int paint_ride_entry(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) != 3)
	{
		if ((flags & 0xFF) != 1)
		{
			if ((flags & 0xFF) <= 1)//0
			{
				uint8_t* ebp = (uint8_t*)(esi + 0x1C2);
				((rct_ride_type*)esi)->name = object_get_localised_text(&ebp, ecx, ebx, 0);
				((rct_ride_type*)esi)->description = object_get_localised_text(&ebp, ecx, ebx, 1);
				object_get_localised_text(&ebp, ecx, ebx, 2);
				((uint32_t*)(esi + 0x1AE))[0] = (uint32_t)ebp;
				if (ebp[0] != 0xFF)
				{
					int a = ebp[0];
					ebp++;
					ebp += a * 3;
				}
				else ebp += 0x61;
				uint8_t* ebp_tmp = ebp;
				int d = 4;
				while (true)
				{
					int a = *ebp;
					ebp++;
					if (a == 0xFF)
					{
						a = ((uint16_t*)ebp)[0];
						ebp += 2;
					}
					ebp += a;
					d--;
					if (d == 0) break;
				}
				int a = sub_6A9ED1(&ebp);
				((uint32_t*)(esi + 4))[0] = a;
				a += 3;
				uint8_t* tmp = ebp;
				ebp = ebp_tmp;
				ebp_tmp = tmp;
				int a_tmp = a;
				int b_tmp = ebx;
				int c_tmp = ecx;
				d = 0;
				int di = 0;
				while (true)
				{
					if (((uint16_t*)(di + esi + 0x26))[0] & 1)
					{
						int al = 1;
						if (((uint16_t*)(di + esi + 0x2E))[0] & 2)
						{
							int ax = ((uint16_t*)(di + esi + 0x2E))[0];
							ax &= 0x820;
							al = 0xD;
							if (ax != 0x820)
							{
								al = 7;
								if (!(((uint16_t*)(di + esi + 0x2E))[0] & 0x20))
								{
									if (!(((uint16_t*)(di + esi + 0x2E))[0] & 0x800))
									{
										al = 5;
										if (((uint16_t*)(di + esi + 0x2E))[0] & 0x200) al = 3;
									}
								}
							}
						}
						((uint8_t*)(di + esi + 0x1D))[0] = al;
						al = 0x20;
						if (!(((uint16_t*)(di + esi + 0x2C))[0] & 0x4000))
						{
							al = 1;
							if (((uint16_t*)(di + esi + 0x2E))[0] & 0x80)
							{
								if (((uint8_t*)(di + esi + 0x2B))[0] != 6)
								{
									al = 2;
									if (!(((uint16_t*)(di + esi + 0x2C))[0] & 0x80)) al = 4;
								}
							}
						}
						if (((uint16_t*)(di + esi + 0x2C))[0] & 0x1000) al = ((uint8_t*)(di + esi + 0x7A))[0];
						((uint8_t*)(di + esi + 0x1C))[0] = al;
						int a = ((uint8_t*)(di + esi + 0x1C))[0];
						int b = ((uint8_t*)(di + esi + 0x1D))[0];
						b *= a;
						((uint16_t*)(di + esi + 0x30))[0] = b;
						a = a_tmp;
						((uint32_t*)(di + esi + 0x32))[0] = a;
						if (((uint8_t*)(di + esi + 0x77))[0] != 4)
						{
							b = ((uint16_t*)(di + esi + 0x30))[0];
							b *= 0x20;
							if (((uint16_t*)(di + esi + 0x2C))[0] & 0x800) b >>= 1;
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x8000) b >>= 3;
							a += b;
							if (((uint16_t*)(di + esi + 0x26))[0] & 2)
							{
								((uint32_t*)(di + esi + 0x3A))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x48;
								if (((uint16_t*)(di + esi + 0x2C))[0] & 0x4000)
								{
									b = ((uint16_t*)(di + esi + 0x30))[0];
									b *= 0x10;
								}
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 4)
							{
								((uint32_t*)(di + esi + 0x3E))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x50;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 8)
							{
								((uint32_t*)(di + esi + 0x42))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x74;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x10)
							{
								((uint32_t*)(di + esi + 0x46))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x18;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x20)
							{
								((uint32_t*)(di + esi + 0x4A))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x50;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x40)
							{
								((uint32_t*)(di + esi + 0x4E))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x28;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x80)
							{
								((uint32_t*)(di + esi + 0x52))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x80;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x100)
							{
								((uint32_t*)(di + esi + 0x56))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x10;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x200)
							{
								((uint32_t*)(di + esi + 0x5A))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x10;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x400)
							{
								((uint32_t*)(di + esi + 0x5E))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x80;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x800)
							{
								((uint32_t*)(di + esi + 0x62))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x10;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x1000)
							{
								((uint32_t*)(di + esi + 0x66))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x50;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x2000)
							{
								((uint32_t*)(di + esi + 0x36))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0xC;
								a += b;
							}
							if (((uint16_t*)(di + esi + 0x26))[0] & 0x4000)
							{
								((uint32_t*)(di + esi + 0x66))[0] = a;
								b = ((uint16_t*)(di + esi + 0x30))[0];
								b *= 0x20;
								a += b;
							}
						}
						else
						{
							b = ((uint16_t*)(di + esi + 0x30))[0];
							b *= 0x24;
							a += b;
						}
						a -= a_tmp;
						((uint32_t*)(di + esi + 0x6A))[0] = a;
						a_tmp += a;
						int c = ((uint8_t*)(di + esi + 0x6E))[0];
						c *= a;
						a_tmp += c;
						if (!(((uint16_t*)(di + esi + 0x2C))[0] & 0x400))
						{
							c = a_tmp;
							b = ((uint32_t*)(di + esi + 0x32))[0];
							c -= b;
							if (((uint16_t*)(di + esi + 0x2C))[0] & 0x2000) c <<= 1;
							int d_tmp = d;
							RCT2_CALLFUNC_X(0x6847BA, &a, &b, &c, &d, &esi, &di, (int*)&ebp);
							if (((uint16_t*)(di + esi + 0x2C))[0] & 0x2000) b += 0x10;
							((uint8_t*)(di + esi + 0x28))[0] = a;
							((uint8_t*)(di + esi + 0x29))[0] = b & 0xFF;
							((uint8_t*)(di + esi + 0x2A))[0] = (b >> 8) & 0xFF;
							d = d_tmp;
						}
						a = ((uint8_t*)ebp)[0];
						ebp++;
						if (a == 0xFF)
						{
							a = ((uint16_t*)ebp)[0];
							ebp += 2;
						}
						((uint32_t*)(di + esi + 0x7B))[0] = (uint32_t)ebp;
						ebp += a;
					}
					d++;
					di += 0x65;
					if (d >= 4) break;
				}
				if (RCT2_GLOBAL(0x9ADAFD, uint8_t) == 0)
				{
					a = 0;
					while (true)
					{
						int d = ((uint8_t*)(a + esi + 0xC))[0];
						if (d != 0xFF)
						{
							uint8_t* di = (uint8_t*)0x9E32F8;
							while (true)
							{
								if (di[0] != 0xFF)
								{
									di++;
									continue;
								}
								d--;
								if (d < 0) break;
								di++;
							}
							d = b_tmp;
							while (true)
							{
								int tmp = di[0];
								di[0] = d;
								d = tmp;
								di++;
								if ((uint32_t)di >= 0x9E34E4) break;
							}
						}
						a++;
						if (a >= 3) break;
					}
				}
				if (RCT2_GLOBAL(0x9ADAF4, uint32_t) != 0xFFFFFFFF) RCT2_GLOBAL(0x9ADAF4, uint16_t*)[0] = 0;
				di = ((uint32_t*)(esi + 0xC))[0] & 0xFFFFFF;
				if (((uint32_t*)(esi + 8))[0] & 0x1000) di |= 0x1000000;
				RCT2_GLOBAL(0xF433DD, uint32_t) = di;
				return 0;// flags;
			}
			else
			{
				int d = 0;
				int di = 0;
				while (true)
				{
					/*if (!(((uint16_t*)(di + esi + 0x26))[0] & 1))*/
					d++;
					di += 0x65;
					if (d >= 4) break;
				}
				if (((uint8_t*)(esi + 0x1B2))[0] > 0x4B) return 1;
				if (((uint8_t*)(esi + 0x1B3))[0] > 0x4B) return 1;
				if (((uint8_t*)(esi + 0x1B4))[0] > 0x4B) return 1;
				return 0;
			}
		}
		else
		{
			((rct_ride_type*)esi)->name = 0;
			((rct_ride_type*)esi)->description = 0;
			((rct_ride_type*)esi)->var_004 = 0;
			int d = 0;
			int di = 0;
			while (true)
			{
				((uint32_t*)(esi + di + 0x32))[0] = 0;
				((uint32_t*)(esi + di + 0x36))[0] = 0;
				((uint32_t*)(esi + di + 0x3A))[0] = 0;
				((uint32_t*)(esi + di + 0x3E))[0] = 0;
				((uint32_t*)(esi + di + 0x42))[0] = 0;
				((uint32_t*)(esi + di + 0x46))[0] = 0;
				((uint32_t*)(esi + di + 0x4A))[0] = 0;
				((uint32_t*)(esi + di + 0x4E))[0] = 0;
				((uint32_t*)(esi + di + 0x52))[0] = 0;
				((uint32_t*)(esi + di + 0x56))[0] = 0;
				((uint32_t*)(esi + di + 0x5A))[0] = 0;
				((uint32_t*)(esi + di + 0x5E))[0] = 0;
				((uint32_t*)(esi + di + 0x66))[0] = 0;
				((uint32_t*)(esi + di + 0x62))[0] = 0;
				((uint32_t*)(esi + di + 0x6A))[0] = 0;
				((uint16_t*)(esi + di + 0x30))[0] = 0;
				if (((uint16_t*)(esi + di + 0x2C))[0] & 0x400)
				{
					((uint8_t*)(esi + di + 0x28))[0] = 0;
					((uint8_t*)(esi + di + 0x29))[0] = 0;
					((uint8_t*)(esi + di + 0x2A))[0] = 0;
				}
				((uint8_t*)(esi + di + 0x1C))[0] = 0;
				((uint8_t*)(esi + di + 0x1D))[0] = 0;
				((uint32_t*)(esi + di + 0x7B))[0] = 0;
				d++;
				di += 0x65;
				if (d >= 4) break;
			}
			((uint32_t*)(esi + 0x1AE))[0] = 0;
			return flags;
		}
	}
	else
	{
		if (!((flags >> 8) & 0xFF))
		{
			int image_id = ((rct_ride_type*)ebp)->var_004;
			if (((rct_ride_type*)ebp)->var_00C == 0xFF)
			{
				image_id++;
				if (((rct_ride_type*)ebp)->var_00D == 0xFF) image_id++;
			}
			gfx_draw_sprite(dpi, image_id, ecx - 56, edx - 56, ebp);
			return flags;
		}
		else
		{
			int width = ((uint16_t*)(esi + 0x30))[0];
			width += ((uint16_t*)(esi + 0x2C))[0];
			width -= ecx;
			width -= 4;
			int format_args = ((rct_ride_type*)ebp)->description;
			if (!(((rct_ride_type*)ebp)->var_008 & 0x1000))
			{
				format_args = ((rct_ride_type*)ebp)->var_00C;
				if ((format_args & 0xFF) == 0xFF)
				{
					format_args = ((rct_ride_type*)ebp)->var_00D;
					if ((format_args & 0xFF) == 0xFF) format_args = ((rct_ride_type*)ebp)->var_00E;
				}
				format_args += 0x200;
			}
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, short) = format_args;
			gfx_draw_string_left_wrapped(dpi, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, ecx, edx + 5, width, 1191, 0);
			return flags;
		}
	}
}

//rct2: 0x006A8621
int paint_path_entry(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) != 3)
	{
		if ((flags & 0xFF) != 1)
		{
			if ((flags & 0xFF) <= 1)//0
			{
				uint8_t* pStringTable = (uint8_t*)(esi + 0xE);
				((rct_path_type*)esi)->pad_00 = object_get_localised_text(&pStringTable, ecx, ebx, 0);
				int image_id = sub_6A9ED1(&pStringTable);
				((rct_path_type*)esi)->image = image_id;
				image_id += 0x6D;
				((rct_path_type*)esi)->pad_06 = image_id;
				if (RCT2_GLOBAL(0x9ADAF4, uint32_t) != 0xFFFFFFFF) RCT2_GLOBAL(0x9ADAF4, uint16_t*)[0] = 0;
				RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, sint16) = 0;
				int b = -1;
				while (true)
				{
					b++;
					if (b >= 0x10) break;
					uint8_t* edi = object_entry_groups[5].chunks[ebx];
					if ((uint32_t)edi == 0xFFFFFFFF) continue;
					if (!(edi[0xB] & 4))
					{
						RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, sint16) = ebx;
						break;
					}
					RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, sint16) = ebx;
				}
				return flags;
			}
			else
			{
				if (((rct_path_type*)esi)->pad_0A >= 2) return 1;//actually the carry bit should be set (stc)
				else return 0;
			}
		}
		else
		{
			((rct_path_type*)esi)->pad_00 = 0;
			((rct_path_type*)esi)->image = 0;
			((rct_path_type*)esi)->pad_06 = 0;
			return flags;
		}
	}
	else
	{
		if (!((flags >> 8) & 0xFF))
		{
			//Draws preview for scenario editor!
			gfx_draw_sprite(dpi, ((rct_path_type*)ebp)->image + 71, ecx - 49, edx - 17, ebp);
			gfx_draw_sprite(dpi, ((rct_path_type*)ebp)->image + 72, ecx + 4, edx - 17, ebp);
		}
		return flags;
	}
}

//rct2: 0x00666E42
int paint_park_entrance_entry(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) != 3)
	{
		if ((flags & 0xFF) != 1)
		{
			if ((flags & 0xFF) <= 1)//0
			{
				uint8_t* pStringTable = (uint8_t*)(esi + 8);
				((rct_string_id*)esi)[0] = object_get_localised_text(&pStringTable, ecx, ebx, 0);
				int image_id = sub_6A9ED1(&pStringTable);
				((uint32_t*)(esi + 2))[0] = image_id;
				if (RCT2_GLOBAL(0x9ADAF4, uint32_t) != 0xFFFFFFFF) RCT2_GLOBAL(0x9ADAF4, uint16_t*)[0] = 0;
				return flags;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			((rct_string_id*)esi)[0] = 0;
			((uint32_t*)(esi + 2))[0] = 0;
			return flags;
		}
	}
	else
	{
		if (!((flags >> 8) & 0xFF))
		{
			dpi = clip_drawpixelinfo(dpi, ecx - 56, 112, edx - 56, 112);
			if (dpi == NULL) return flags;
			int image_id = ((uint32_t*)(ebp + 2))[0];
			gfx_draw_sprite(dpi, image_id + 1, 24, 68, ebp);
			gfx_draw_sprite(dpi, image_id, 56, 84, ebp);
			gfx_draw_sprite(dpi, image_id + 2, 88, 100, ebp);
			rct2_free(dpi);
		}
		return flags;
	}
}

//rct2: 0x006E6E2A
int paint_water_entry(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) != 3)
	{
		if ((flags & 0xFF) != 1)
		{
			if ((flags & 0xFF) <= 1)//0
			{
				uint8_t* pStringTable = (uint8_t*)(esi + 0x10);
				((rct_string_id*)esi)[0] = object_get_localised_text(&pStringTable, ecx, ebx, 0);
				int image_id = sub_6A9ED1(&pStringTable);
				((uint32_t*)(esi + 2))[0] = image_id;
				image_id++;
				((uint32_t*)(esi + 6))[0] = image_id;
				image_id += 3;
				((uint32_t*)(esi + 0xA))[0] = image_id;
				if (RCT2_GLOBAL(0x9ADAF4, uint32_t) != 0xFFFFFFFF) RCT2_GLOBAL(0x9ADAF4, uint16_t*)[0] = 0;
				if (RCT2_GLOBAL(0x9ADAFD, uint8_t) == 0)
				{
					load_palette();
					gfx_invalidate_screen();
				}
				return flags;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			((rct_string_id*)esi)[0] = 0;
			((uint32_t*)(esi + 2))[0] = 0;
			((uint32_t*)(esi + 6))[0] = 0;
			((uint32_t*)(esi + 0xA))[0] = 0;
			return flags;
		}
	}
	else
	{
		if (!((flags >> 8) & 0xFF)) gfx_draw_string_centred(dpi, 3326, ecx, edx, 0, (void*)esi);
		return flags;
	}
}

//rct2: 0x0066B355
int paint_stex_entry(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) != 3)
	{
		if ((flags & 0xFF) != 1)
		{
			if ((flags & 0xFF) <= 1)//0
			{
				uint8_t* pStringTable = (uint8_t*)(esi + 8);
				((rct_stex_entry*)esi)->scenario_name = object_get_localised_text(&pStringTable, ecx, ebx, 0);
				((rct_stex_entry*)esi)->park_name = object_get_localised_text(&pStringTable, ecx, ebx, 1);
				((rct_stex_entry*)esi)->details = object_get_localised_text(&pStringTable, ecx, ebx, 2);
				if (RCT2_GLOBAL(0x9ADAF4, int) != -1) RCT2_GLOBAL(0x9ADAF4, uint16_t*)[0] = 0;
				return flags;
			}
			else//2
			{
				return 0;
			}
		}
		else//1
		{
			((rct_stex_entry*)esi)->scenario_name = 0;
			((rct_stex_entry*)esi)->park_name = 0;
			((rct_stex_entry*)esi)->details = 0;
			return flags;
		}
	}
	else//3
	{
		if (!((flags >> 8) & 0xFF)) gfx_draw_string_centred(dpi, 0xCFE, ecx, edx, 0, (void*)esi);
		else
		{
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, short) = ((rct_stex_entry*)ebp)->details;
			int width = *((uint16_t*)(esi + 0x2C));
			width += *((uint16_t*)(esi + 0x30));
			width -= 4;
			width -= ecx;
			gfx_draw_string_left_wrapped(dpi, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, ecx, edx, width, 3168, 0);
		}
		return flags;
	}
}

int object_paint(int type, int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp)
{
	//if (type == OBJECT_TYPE_SCENARIO_TEXT){
	//	if (eax == 0) return object_scenario_load_custom_text((char*)esi);
	//}
	//return RCT2_CALLPROC_X(RCT2_ADDRESS(0x0098D9D4, uint32)[type], eax, ebx, ecx, edx, esi, edi, ebp) & 0x100;
	//just use the rct2 function as long as this is not complete!
	switch (type)
	{
	case OBJECT_TYPE_RIDE:
		return paint_ride_entry(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_PATHS:
		return paint_path_entry(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_PARK_ENTRANCE:
		return paint_park_entrance_entry(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_WATER:
		return paint_water_entry(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_SCENARIO_TEXT:
		return paint_stex_entry(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	default:
		return RCT2_CALLPROC_X(RCT2_ADDRESS(0x0098D9D4, uint32)[type], eax, ebx, ecx, edx, esi, edi, ebp) & 0x100;
	}
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
					}
					else {
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
		rct2_free(RCT2_GLOBAL(0x009ADAF8, void*));
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

char *object_get_name(rct_object_entry *entry)
{
	uint8 *pos = (uint8*)entry;

	// Skip sizeof(rct_object_entry)
	pos += 16;

	// Skip filename
	while (*pos++);

	// Skip 
	pos += 4;

	return pos;
}