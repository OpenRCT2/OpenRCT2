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
#include "ride/ride.h"
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

/* rct2: 0x006A9ED1 */
int object_chunk_load_image_directory(uint8_t** chunk)
{
	int image_start_no = RCT2_GLOBAL(0x9ADAF0, uint32_t);
	
	// First dword of chunk is no_images
	int no_images = *((uint32_t*)(*chunk));
	*chunk += 4;
	// Second dword of chunk is length of image data
	int length_of_data = *((uint32_t*)(*chunk));
	*chunk += 4;

	RCT2_GLOBAL(0x9ADAF0, uint32_t) = no_images + image_start_no;

	rct_g1_element* g1_dest = &RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[image_start_no];

	// After length of data is the start of all g1 element structs
	rct_g1_element* g1_source = (rct_g1_element*)(*chunk);

	// After the g1 element structs is the actual images.
	uint8* image_offset = no_images * sizeof(rct_g1_element) + (uint8*)g1_source;

	for (int i = 0; i < no_images; ++i){
		*g1_dest = *g1_source++;
		g1_dest->offset += (uint32)image_offset;
		g1_dest++;
	}

	*chunk = ((uint8*)g1_source) + length_of_data;
	return image_start_no;
}

/* rct2: 0x006DE83E */
int paint_ride_entry(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) == 0){
		// esi
		rct_ride_type* ride_type = (rct_ride_type*)esi;

		// After ride_type is 3 string tables
		uint8_t* chunk = (uint8*)(ride_type + 1);
		ride_type->name = object_get_localised_text(&chunk, ecx, ebx, 0);
		ride_type->description = object_get_localised_text(&chunk, ecx, ebx, 1);
		object_get_localised_text(&chunk, ecx, ebx, 2);
		// Offset to Unknown struct
		ride_type->var_1AE = (uint32_t)chunk;

		// If Unknown struct size is 0xFF then there are 32 3 byte structures
		uint8 unknown_size = *chunk++;
		if (unknown_size != 0xFF)
		{
			chunk += unknown_size * 3;
		}
		else {
			chunk += 0x60;
		}

		uint8* peep_loading_positions = chunk;
		// Peep loading positions variable size
		// 4 different vehicle subtypes are available
		for (int i = 0; i < 4; ++i){
			uint16 no_peep_positions = *chunk++;
			// If no_peep_positions is 0xFF then no_peep_positions is a word
			if (no_peep_positions == 0xFF)
			{
				no_peep_positions = *((uint16_t*)chunk);
				chunk += 2;
			}
			chunk += no_peep_positions;
		}

		int images_offset = object_chunk_load_image_directory(&chunk);
		ride_type->images_offset = images_offset;

		int cur_vehicle_images_offset = images_offset + 3;

		for (int i = 0; i < 4; ++i){
			rct_ride_type_vehicle* rideVehicleEntry = &ride_type->vehicles[i];

			if (rideVehicleEntry->var_0C & 1){
				int al = 1;
				if (rideVehicleEntry->var_14 & 2)
				{
					al = 13;
					if ((rideVehicleEntry->var_14 & 0x820) != 0x820)
					{
						al = 7;
						if (!(rideVehicleEntry->var_14 & 0x20))
						{
							if (!(rideVehicleEntry->var_14 & 0x800))
							{
								al = 5;
								if (rideVehicleEntry->var_14 & 0x200) al = 3;
							}
						}
					}
				}
				rideVehicleEntry->var_03 = al;
				// 0x6DE90B

				al = 0x20;
				if (!(rideVehicleEntry->var_12 & 0x4000))
				{
					al = 1;
					if (rideVehicleEntry->var_14 & 0x80)
					{
						if (rideVehicleEntry->var_11 != 6)
						{
							al = 2;
							if (!(rideVehicleEntry->var_12 & 0x80)) al = 4;
						}
					}
				}
				if (rideVehicleEntry->var_12 & 0x1000) al = rideVehicleEntry->var_60;
				rideVehicleEntry->var_02 = al;
				// 0x6DE946

				rideVehicleEntry->var_16 = rideVehicleEntry->var_02 * rideVehicleEntry->var_03;
				rideVehicleEntry->base_image_id = cur_vehicle_images_offset;
				int image_index = rideVehicleEntry->base_image_id;

				if (rideVehicleEntry->var_5D != 4){
					int b = rideVehicleEntry->var_16 * 32;

					if (rideVehicleEntry->var_12 & 0x800) b /= 2;
					if (rideVehicleEntry->var_0C & 0x8000) b /= 8;

					image_index += b;

					// Incline 25
					if (rideVehicleEntry->var_0C & 0x2){
						rideVehicleEntry->var_20 = image_index;
						b = rideVehicleEntry->var_16 * 72;
						if (rideVehicleEntry->var_12 & 0x4000)
							b = rideVehicleEntry->var_16 * 16;

						image_index += b;
					}

					// Incline 60
					if (rideVehicleEntry->var_0C & 0x4){
						rideVehicleEntry->var_24 = image_index;
						b = rideVehicleEntry->var_16 * 80;
						image_index += b;
					}
					// Verticle
					if (rideVehicleEntry->var_0C & 0x8){
						rideVehicleEntry->var_28 = image_index;
						b = rideVehicleEntry->var_16 * 116;
						image_index += b;
					}
					// Unknown
					if (rideVehicleEntry->var_0C & 0x10){
						rideVehicleEntry->var_2C = image_index;
						b = rideVehicleEntry->var_16 * 24;
						image_index += b;
					}

					// Bank
					if (rideVehicleEntry->var_0C & 0x20){
						rideVehicleEntry->var_30 = image_index;
						b = rideVehicleEntry->var_16 * 80;
						image_index += b;
					}

					if (rideVehicleEntry->var_0C & 0x40){
						rideVehicleEntry->var_34 = image_index;
						b = rideVehicleEntry->var_16 * 40;
						image_index += b;
					}

					// Track half? Up/Down
					if (rideVehicleEntry->var_0C & 0x80){
						rideVehicleEntry->var_38 = image_index;
						b = rideVehicleEntry->var_16 * 128;
						image_index += b;
					}
					// Unknown
					if (rideVehicleEntry->var_0C & 0x100){
						rideVehicleEntry->var_3C = image_index;
						b = rideVehicleEntry->var_16 * 16;
						image_index += b;
					}
					// Unknown
					if (rideVehicleEntry->var_0C & 0x200){
						rideVehicleEntry->var_40 = image_index;
						b = rideVehicleEntry->var_16 * 16;
						image_index += b;
					}

					if (rideVehicleEntry->var_0C & 0x400){
						rideVehicleEntry->var_44 = image_index;
						b = rideVehicleEntry->var_16 * 128;
						image_index += b;
					}

					if (rideVehicleEntry->var_0C & 0x800){
						rideVehicleEntry->var_48 = image_index;
						b = rideVehicleEntry->var_16 * 16;
						image_index += b;
					}

					if (rideVehicleEntry->var_0C & 0x1000){
						rideVehicleEntry->var_4C = image_index;
						b = rideVehicleEntry->var_16 * 80;
						image_index += b;
					}
					// Unknown
					if (rideVehicleEntry->var_0C & 0x2000){
						rideVehicleEntry->var_1C = image_index;
						b = rideVehicleEntry->var_16 * 12;
						image_index += b;
					}

					if (rideVehicleEntry->var_0C & 0x4000){
						// Same offset as above???
						rideVehicleEntry->var_4C = image_index;
						b = rideVehicleEntry->var_16 * 32;
						image_index += b;
					}

				}
				else{
					image_index += rideVehicleEntry->var_16 * 36;
				}
				// No vehicle images
				rideVehicleEntry->no_vehicle_images = image_index - cur_vehicle_images_offset;

				// Move the offset over this vehicles images. Including peeps
				cur_vehicle_images_offset = image_index + rideVehicleEntry->no_seating_rows * rideVehicleEntry->no_vehicle_images;
				// 0x6DEB0D

				if (!(rideVehicleEntry->var_12 & 0x400)){
					int ecx = cur_vehicle_images_offset - rideVehicleEntry->base_image_id;
					if (rideVehicleEntry->var_12 & 0x2000){
						ecx *= 2;
					}

					int bl, bh, eax = 0;
					{
						int ebx = rideVehicleEntry->base_image_id;
						int edx = 0, esi = 0, ebp = 0, edi = 0;
						RCT2_CALLFUNC_X(0x6847BA, &eax, &ebx, &ecx, &edx, &esi, &ebp, &edi);
						bl = ebx & 0xFF;
						bh = (ebx >> 8) & 0xFF;
					}

					if (rideVehicleEntry->var_12 & 0x2000){
						bl += 16;
					}

					rideVehicleEntry->var_0E = eax & 0xFF;
					rideVehicleEntry->var_0F = bl;
					rideVehicleEntry->var_10 = bh;
				}

				uint16 no_positions = *peep_loading_positions++;
				if (no_positions == 0xFF)
				{
					no_positions = *((uint16*)peep_loading_positions);
					peep_loading_positions += 2;
				}
				rideVehicleEntry->var_61 = (uint32)peep_loading_positions;
				peep_loading_positions += no_positions;
			}
		}

		// 0x6DEB71
		if (RCT2_GLOBAL(0x9ADAFD, uint8_t) == 0)
		{
			for (int i = 0; i < 3; ++i){
				sint16 dl = (&ride_type->var_00C)[i];

				if (dl == 0xFF)continue;

				uint8 *typeToRideEntryIndexMap = RCT2_ADDRESS(0x009E32F8, uint8);

				while (dl >= 0){
					if (*typeToRideEntryIndexMap++ == 0xFF)dl--;
				}

				typeToRideEntryIndexMap--;
				uint8 previous_entry = ebx;
				while (typeToRideEntryIndexMap < RCT2_ADDRESS(0x9E34E4, uint8)){
					uint8 backup_entry = *typeToRideEntryIndexMap;
					*typeToRideEntryIndexMap++ = previous_entry;
					previous_entry = backup_entry;
				}
			}
		}

		// 0x6DEBAA
		if (RCT2_GLOBAL(0x9ADAF4, sint32) != 0xFFFFFFFF) *RCT2_GLOBAL(0x9ADAF4, uint16*) = 0;

		int di = ride_type->var_00C | (ride_type->var_00D << 8) | (ride_type->var_00E << 16);

		if (ride_type->var_008 & 0x1000) di |= 0x1000000;

		RCT2_GLOBAL(0xF433DD, uint32) = di;
		return 0;// flags;
	}
	else if ((flags & 0xFF) == 1){
		// Object Unload

		rct_ride_type* ride_type = (rct_ride_type*)esi;
		ride_type->name = 0;
		ride_type->description = 0;
		ride_type->images_offset = 0;

		for (int i = 0; i < 4; ++i){
			rct_ride_type_vehicle* rideVehicleEntry = &ride_type->vehicles[i];

			rideVehicleEntry->base_image_id = 0;
			rideVehicleEntry->var_1C = 0;
			rideVehicleEntry->var_20 = 0;
			rideVehicleEntry->var_24 = 0;
			rideVehicleEntry->var_28 = 0;
			rideVehicleEntry->var_2C = 0;
			rideVehicleEntry->var_30 = 0;
			rideVehicleEntry->var_34 = 0;
			rideVehicleEntry->var_38 = 0;
			rideVehicleEntry->var_3C = 0;
			rideVehicleEntry->var_40 = 0;
			rideVehicleEntry->var_44 = 0;
			rideVehicleEntry->var_48 = 0;
			rideVehicleEntry->var_4C = 0;
			rideVehicleEntry->no_vehicle_images = 0;
			rideVehicleEntry->var_16 = 0;

			if (rideVehicleEntry->var_12 & 0x400){
				rideVehicleEntry->var_0E = 0;
				rideVehicleEntry->var_0F = 0;
				rideVehicleEntry->var_10 = 0;
			}
			rideVehicleEntry->var_02 = 0;
			rideVehicleEntry->var_03 = 0;
			rideVehicleEntry->var_61 = 0;
		}

		ride_type->var_1AE = 0;
		return flags;
	}
	else if ((flags & 0xFF) == 2){

		rct_ride_type* ride_type = (rct_ride_type*)esi;

		if (ride_type->excitement_multipler > 75) return 1;
		if (ride_type->intensity_multipler > 75) return 1;
		if (ride_type->nausea_multipler > 75) return 1;
		return 0;
	}
	else if ((flags & 0xFF) == 3){
		// Paint object picture and description

		rct_ride_type* ride_type = (rct_ride_type*)ebp;
		int x = ecx, y = edx;

		if (!((flags >> 8) & 0xFF))
		{
			int image_id = ride_type->images_offset;
			if (ride_type->var_00C == 0xFF)
			{
				image_id++;
				if (ride_type->var_00D == 0xFF) image_id++;
			}
			gfx_draw_sprite(dpi, image_id, x - 56, y - 56, ebp);
			return flags;
		}
		else
		{	
			rct_window* w = (rct_window*)esi;
			int width = w->x + w->width - x - 4;

			int format_args = ride_type->description;
			if (!(ride_type->var_008 & 0x1000))
			{
				format_args = ride_type->var_00C;
				if ((format_args & 0xFF) == 0xFF)
				{
					format_args = ride_type->var_00D;
					if ((format_args & 0xFF) == 0xFF) format_args = ride_type->var_00E;
				}
				format_args += 0x200;
			}
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, short) = format_args;
			gfx_draw_string_left_wrapped(dpi, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, x, y + 5, width, 1191, 0);
			return flags;
		}
	}

	return flags;
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

				int image_id = object_chunk_load_image_directory(&pStringTable);
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
				int image_id = object_chunk_load_image_directory(&pStringTable);
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
				int image_id = object_chunk_load_image_directory(&pStringTable);
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