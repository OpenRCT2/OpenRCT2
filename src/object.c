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
#include "world/water.h"
#include "world/entrance.h"
#include "world/scenery.h"
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

	if (RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_NO_IMAGES, uint32) >= 0x4726E){
		log_error("Object Load failed due to too many images loaded.");
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
int write_object_file(FILE *file, rct_object_entry* entry){
	uint8 entryGroupIndex = 0, type = 0;
	uint8* chunk = 0;

	if (!find_object_in_entry_group(entry, &type, &entryGroupIndex))return 0;

	chunk = object_entry_groups[type].chunks[entryGroupIndex];

	object_paint(type, 1, entryGroupIndex, type, 0, (int)chunk, 0, 0);

	rct_object_entry_extended* installed_entry = &object_entry_groups[type].entries[entryGroupIndex];
	uint8* dst_buffer = malloc(0x600000);
	memcpy(dst_buffer, installed_entry, sizeof(rct_object_entry));

	uint32 size_dst = sizeof(rct_object_entry);

	sawyercoding_chunk_header chunkHeader;
	// Encoding type (not used anymore)
	RCT2_GLOBAL(0x9E3CBD, uint8) = object_entry_group_encoding[type];

	chunkHeader.encoding = object_entry_group_encoding[type];
	chunkHeader.length = installed_entry->chunk_size;

	size_dst += sawyercoding_write_chunk_buffer(dst_buffer + sizeof(rct_object_entry), chunk, chunkHeader);
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

	rct_object_entry entry;

	fread(&entry, 16, 1, file);

	uint8* chunk = rct2_malloc(0x600000);
	uint32 chunkSize = sawyercoding_read_chunk(file, chunk);
	chunk = rct2_realloc(chunk, chunkSize);

	if (chunk == NULL){
		log_error("Failed to allocate memory for packed object.");
		return 0;
	}

	if (object_calculate_checksum(&entry, chunk, chunkSize) != entry.checksum){
		log_error("Checksum missmatch from packed object: %.8s", entry.name);
		rct2_free(chunk);
		return 0;
	}

	int type = entry.flags & 0x0F;

	if (object_paint(type, 2, 0, type, 0, (int)chunk, 0, 0)) {
		log_error("Packed object failed paint test.");
		rct2_free(chunk);
		return 0;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_NO_IMAGES, uint32) >= 0x4726E){
		log_error("Packed object has too many images.");
		rct2_free(chunk);
		return 0;
	}

	int entryGroupIndex = 0;

	for (; entryGroupIndex < object_entry_group_counts[type]; entryGroupIndex++){
		if (object_entry_groups[type].chunks[entryGroupIndex] == (uint8*)-1){
			break;
		}
	}

	if (entryGroupIndex == object_entry_group_counts[type]){
		// This should never occur. Objects are not loaded before installing a 
		// packed object. So there is only one object loaded at this point.
		log_error("Too many objects of the same type loaded.");
		rct2_free(chunk);
		return 0;
	}

	// Copy the entry into the relevant entry group.
	object_entry_groups[type].chunks[entryGroupIndex] = chunk;
	rct_object_entry_extended* extended_entry = &object_entry_groups[type].entries[entryGroupIndex];
	memcpy(extended_entry, &entry, sizeof(rct_object_entry));
	extended_entry->chunk_size = chunkSize;

	// Ensure the entry does not already exist.
	rct_object_entry *installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
	if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32)){
		for (uint32 i = 0; i < RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); ++i){
			if (object_entry_compare(&entry, installedObject)){
				object_unload_all();
				return 0;
			}
			installedObject = object_get_next(installedObject);
		}
	}

	// Convert the entry name to a upper case path name
	char path[260];
	char objectPath[9] = { 0 };
	for (int i = 0; i < 8; ++i){
		if (entry.name[i] != ' ')
			objectPath[i] = toupper(entry.name[i]);
		else
			objectPath[i] = '\0';
	}

	subsitute_path(path, RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), objectPath);
	// Require pointer to start of filename
	char* last_char = path + strlen(path);
	strcat(path, ".DAT");

	// Check that file does not exist
	// Adjust filename if it does.
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

	// Actually write the object to the file
	FILE* obj_file = fopen(path, "wb");
	if (obj_file){
		uint8 result = write_object_file(obj_file, &entry);

		fclose(obj_file);
		object_unload_all();

		return result;
	}

	object_unload_all();
	return 0;
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
	// If an official object don't bother checking checksum
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
	int image_start_no = RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_NO_IMAGES, uint32_t);
	
	// First dword of chunk is no_images
	int no_images = *((uint32_t*)(*chunk));
	*chunk += 4;
	// Second dword of chunk is length of image data
	int length_of_data = *((uint32_t*)(*chunk));
	*chunk += 4;

	RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_NO_IMAGES, uint32_t) = no_images + image_start_no;

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

/* rct2: 0x006E3466 */
int paint_small_scenery(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) == 0){
		// Object Load

		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;
		uint8* chunk = (uint8*)(esi + 0x1C);

		scenery_type->name = object_get_localised_text(&chunk, ecx, ebx, 0);

		scenery_type->small_scenery.scenery_tab_id = 0xFF;

		if (*chunk != 0xFF){
			uint8 entry_type, entry_index;
			if (find_object_in_entry_group((rct_object_entry*)chunk, &entry_type, &entry_index)){
				scenery_type->small_scenery.scenery_tab_id = entry_index;
			}
		}

		chunk += sizeof(rct_object_entry);

		if (scenery_type->small_scenery.flags & SMALL_SCENERY_FLAG16){
			scenery_type->small_scenery.var_10 = (uint32)chunk;
			while (*++chunk != 0xFF);
			chunk++;
		}

		scenery_type->image = object_chunk_load_image_directory(&chunk);

		if (RCT2_GLOBAL(0x9ADAF4, uint32) != 0xFFFFFFFF) *RCT2_GLOBAL(0x9ADAF4, uint16*) = 0;
	}
	else if ((flags & 0xFF) == 1){
		// Object Unload

		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;
		scenery_type->name = 0;
		scenery_type->image = 0;
		scenery_type->small_scenery.var_10 = 0;
		scenery_type->small_scenery.scenery_tab_id = 0;
	}
	else if ((flags & 0xFF) == 2){
		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;

		if (scenery_type->small_scenery.price <= 0)return 1;

		if (scenery_type->small_scenery.removal_price > 0) return 0;

		// Make sure you don't make a profit when placing then removing.
		if (-scenery_type->small_scenery.removal_price > scenery_type->small_scenery.price)return 1;

		return 0;
	}
	else if ((flags & 0xFF) == 3){
		int x = ecx, y = edx;
		if (!((flags >> 8) & 0xFF))
		{
			rct_scenery_entry* scenery_type = (rct_scenery_entry*)ebp;

			dpi = clip_drawpixelinfo(dpi, x - 56, 112, y - 56, 112);
			if (dpi == NULL) return flags;

			int image_id = scenery_type->image;

			if (scenery_type->small_scenery.flags & SMALL_SCENERY_HAS_PRIMARY_COLOUR){
				image_id |= 0x20D00000;

				if (scenery_type->small_scenery.flags & SMALL_SCENERY_HAS_SECONDARY_COLOUR)
					image_id |= 0x92000000;
			}

			x = 56;
			y = scenery_type->small_scenery.height / 4 + 78;

			if (scenery_type->small_scenery.flags & SMALL_SCENERY_FLAG1){
				if (scenery_type->small_scenery.flags & SMALL_SCENERY_FLAG2){
					y -= 12;
				}
			}

			gfx_draw_sprite(dpi, image_id, x, y, 0);

			if (scenery_type->small_scenery.flags & SMALL_SCENERY_FLAG10){
				image_id = scenery_type->image + 0x44500004;

				if (scenery_type->small_scenery.flags & SMALL_SCENERY_HAS_SECONDARY_COLOUR)
					image_id |= 0x92000000;

				gfx_draw_sprite(dpi, image_id, x, y, 0);
			}

			if (scenery_type->small_scenery.flags & SMALL_SCENERY_FLAG8){
				image_id = scenery_type->image + 4;

				if (scenery_type->small_scenery.flags & SMALL_SCENERY_HAS_SECONDARY_COLOUR)
					image_id |= 0x92000000;

				gfx_draw_sprite(dpi, image_id, x, y, 0);
			}

			rct2_free(dpi);
		}
	}
	return flags;
}

/* rct2: 0x006B92A7 */
int paint_large_scenery(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) == 0){
		// Object Load

		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;
		uint8* chunk = (uint8*)(esi + 0x1A);

		scenery_type->name = object_get_localised_text(&chunk, ecx, ebx, 0);

		scenery_type->large_scenery.scenery_tab_id = 0xFF;

		if (*chunk != 0xFF){
			uint8 entry_type, entry_index;
			if (find_object_in_entry_group((rct_object_entry*)chunk, &entry_type, &entry_index)){
				scenery_type->large_scenery.scenery_tab_id = entry_index;
			}
		}

		chunk += sizeof(rct_object_entry);

		if (scenery_type->large_scenery.flags & (1<<2)){
			scenery_type->large_scenery.var_12 = (uint32)chunk;
			chunk += 1038;
		}

		scenery_type->large_scenery.var_0C = (uint32)chunk;

		while (*((uint16*)chunk) != 0xFFFF){
			chunk += 9;
		}

		chunk += 2;

		int image_id = object_chunk_load_image_directory(&chunk);
		if (scenery_type->large_scenery.flags & (1 << 2)){
			scenery_type->large_scenery.var_16 = image_id;

			uint8* edx = (uint8*)scenery_type->large_scenery.var_12;
			if (!(edx[0xC] & 1)){
				image_id += edx[0xD] * 4;
			}
			else{
				image_id += edx[0xD] * 2;
			}
		}
		scenery_type->image = image_id;

		if (RCT2_GLOBAL(0x9ADAF4, uint32) != 0xFFFFFFFF) *RCT2_GLOBAL(0x9ADAF4, uint16*) = 0;
	}
	else if ((flags & 0xFF) == 1){
		// Object Unload

		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;
		scenery_type->name = 0;
		scenery_type->image = 0;
		scenery_type->large_scenery.var_0C = 0;
		scenery_type->large_scenery.scenery_tab_id = 0;
		scenery_type->large_scenery.var_12 = 0;
		scenery_type->large_scenery.var_16 = 0;
	}
	else if ((flags & 0xFF) == 2){
		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;

		if (scenery_type->large_scenery.price <= 0)return 1;

		if (scenery_type->large_scenery.removal_price > 0) return 0;

		// Make sure you don't make a profit when placing then removing.
		if (-scenery_type->large_scenery.removal_price > scenery_type->large_scenery.price)return 1;

		return 0;
	}
	else if ((flags & 0xFF) == 3){
		int x = ecx, y = edx;
		if (!((flags >> 8) & 0xFF))
		{
			rct_scenery_entry* scenery_type = (rct_scenery_entry*)ebp;

			int image_id = scenery_type->image | 0xB2D00000;

			gfx_draw_sprite(dpi, image_id, x, y - 39, 0);
		}
	}
	return flags;
}

/* rct2: 0x006E5A25 */
int paint_wall(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) == 0){
		// Object Load

		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;
		uint8* chunk = (uint8*)(esi + 0xE);

		scenery_type->name = object_get_localised_text(&chunk, ecx, ebx, 0);

		scenery_type->wall.scenery_tab_id = 0xFF;

		if (*chunk != 0xFF){
			uint8 entry_type, entry_index;
			if (find_object_in_entry_group((rct_object_entry*)chunk, &entry_type, &entry_index)){
				scenery_type->wall.scenery_tab_id = entry_index;
			}
		}

		chunk += sizeof(rct_object_entry);

		scenery_type->image = object_chunk_load_image_directory(&chunk);

		if (RCT2_GLOBAL(0x9ADAF4, uint32) != 0xFFFFFFFF) *RCT2_GLOBAL(0x9ADAF4, uint16*) = 0;
	}
	else if ((flags & 0xFF) == 1){
		// Object Unload

		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;
		scenery_type->name = 0;
		scenery_type->image = 0;
		scenery_type->wall.scenery_tab_id = 0;
	}
	else if ((flags & 0xFF) == 2){
		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;

		if (scenery_type->wall.price <= 0)return 1;

		return 0;
	}
	else if ((flags & 0xFF) == 3){
		int x = ecx, y = edx;

		if (!((flags >> 8) & 0xFF))
		{
			rct_scenery_entry* scenery_type = (rct_scenery_entry*)ebp;

			dpi = clip_drawpixelinfo(dpi, x - 56, 112, y - 56, 112);
			if (dpi == NULL) return flags;

			int image_id = scenery_type->image;


			image_id |= 0x20D00000;

			if (scenery_type->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
				image_id |= 0x92000000;


			x = 70;
			y = scenery_type->wall.height * 2 + 72;

			gfx_draw_sprite(dpi, image_id, x, y, 0);

			if (scenery_type->wall.flags & WALL_SCENERY_FLAG2){
				image_id = scenery_type->image + 0x44500006;

				gfx_draw_sprite(dpi, image_id, x, y, 0);
			}
			else if (scenery_type->wall.flags & WALL_SCENERY_FLAG5){
				image_id++;

				gfx_draw_sprite(dpi, image_id, x, y, 0);
			}
			rct2_free(dpi);
		}
	}
	return flags;
}

/* rct2: 0x006BA84E */
int paint_banner(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) == 0){
		// Object Load

		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;
		uint8* chunk = (uint8*)(esi + 0xC);

		scenery_type->name = object_get_localised_text(&chunk, ecx, ebx, 0);

		scenery_type->banner.scenery_tab_id = 0xFF;

		if (*chunk != 0xFF){
			uint8 entry_type, entry_index;
			if (find_object_in_entry_group((rct_object_entry*)chunk, &entry_type, &entry_index)){
				scenery_type->banner.scenery_tab_id = entry_index;
			}
		}

		chunk += sizeof(rct_object_entry);

		scenery_type->image = object_chunk_load_image_directory(&chunk);

		if (RCT2_GLOBAL(0x9ADAF4, uint32) != 0xFFFFFFFF) *RCT2_GLOBAL(0x9ADAF4, uint16*) = 0;
	}
	else if ((flags & 0xFF) == 1){
		// Object Unload

		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;
		scenery_type->name = 0;
		scenery_type->image = 0;
		scenery_type->banner.scenery_tab_id = 0;
	}
	else if ((flags & 0xFF) == 2){
		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;
			if (scenery_type->banner.price <= 0)return 1;

		return 0;
	}
	else if ((flags & 0xFF) == 3){
		int x = ecx, y = edx;

		if (!((flags >> 8) & 0xFF))
		{
			rct_scenery_entry* scenery_type = (rct_scenery_entry*)ebp;

			int image_id = scenery_type->image;

			image_id |= 0x20D00000;


			gfx_draw_sprite(dpi, image_id, x, y, 0);

			gfx_draw_sprite(dpi, image_id + 1, x, y, 0);
		}
	}
	return flags;
}

//rct2: 0x006A8621
int paint_path_entry(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) == 0){
		// Object Load

		rct_path_type* path_type = (rct_path_type*)esi;
		// String table starts after path entry
		// Note there are 2 spare bytes after
		// the path entry.
		uint8* chunk = (uint8*)(esi + 0xE);

		// Only 1 string table for paths
		path_type->string_idx = object_get_localised_text(&chunk, ecx, ebx, 0);

		int image_id = object_chunk_load_image_directory(&chunk);
		path_type->image = image_id;
		path_type->bridge_image = image_id + 109;

		if (RCT2_GLOBAL(0x9ADAF4, uint32) != 0xFFFFFFFF) *RCT2_GLOBAL(0x9ADAF4, uint16*) = 0;

		RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, sint16) = 0;
		// Set the default path for when opening footpath window
		for (int i = 0; i < object_entry_group_counts[OBJECT_TYPE_PATHS]; ++i){
			rct_path_type* path_type_entry = (rct_path_type*)object_entry_groups[OBJECT_TYPE_PATHS].chunks[i];
			if ((uint32)path_type_entry == 0xFFFFFFFF) continue;
			if (!(path_type_entry->flags & 4))
			{
				RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, sint16) = i;
				break;
			}
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, sint16) = i;

		}
	}
	else if ((flags & 0xFF) == 1){
		// Object Unload

		rct_path_type* path_type = (rct_path_type*)esi;
		path_type->string_idx = 0;
		path_type->image = 0;
		path_type->bridge_image = 0;
	}
	else if ((flags & 0xFF) == 2){

		rct_path_type* path_type = (rct_path_type*)esi;
		if (path_type->var_0A >= 2) return 1;//actually the carry bit should be set (stc)
		else return 0;
	}
	else if ((flags & 0xFF) == 3){
		rct_path_type* path_type = (rct_path_type*)ebp;
		if (!((flags >> 8) & 0xFF))
		{
			//Draws preview for scenario editor!
			gfx_draw_sprite(dpi, path_type->image + 71, ecx - 49, edx - 17, ebp);
			gfx_draw_sprite(dpi, path_type->image + 72, ecx + 4, edx - 17, ebp);
		}

	}
	return flags;
}

/* rct2: 0x006A86E2 */
int paint_path_bit(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) == 0){
		// Object Load

		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;
		uint8* chunk = (uint8*)(esi + 0xE);

		scenery_type->name = object_get_localised_text(&chunk, ecx, ebx, 0);

		scenery_type->path_bit.scenery_tab_id = 0xFF;

		if (*chunk != 0xFF){
			uint8 entry_type, entry_index;
			if (find_object_in_entry_group((rct_object_entry*)chunk, &entry_type, &entry_index)){
				scenery_type->path_bit.scenery_tab_id = entry_index;
			}
		}

		chunk += sizeof(rct_object_entry);

		scenery_type->image = object_chunk_load_image_directory(&chunk);

		if (RCT2_GLOBAL(0x9ADAF4, uint32) != 0xFFFFFFFF) *RCT2_GLOBAL(0x9ADAF4, uint16*) = 0;
	}
	else if ((flags & 0xFF) == 1){
		// Object Unload

		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;
		scenery_type->name = 0;
		scenery_type->image = 0;
		scenery_type->path_bit.scenery_tab_id = 0;
	}
	else if ((flags & 0xFF) == 2){
		rct_scenery_entry* scenery_type = (rct_scenery_entry*)esi;

			if (scenery_type->path_bit.price <= 0)return 1;

		return 0;
	}
	else if ((flags & 0xFF) == 3){
		int x = ecx, y = edx;

		if (!((flags >> 8) & 0xFF))
		{
			rct_scenery_entry* scenery_type = (rct_scenery_entry*)ebp;

			int image_id = scenery_type->image;

			x -= 22;
			y -= 24;

			gfx_draw_sprite(dpi, image_id, x, y, 0);
		}
	}
	return flags;
}

/* rct2: 0x006B93AA */
int paint_scenery_set(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) == 0){
		// Object Load

		rct_scenery_set_entry* scenery_set = (rct_scenery_set_entry*)esi;
		uint8* chunk = (uint8*)(esi + sizeof(rct_scenery_set_entry));

		scenery_set->name = object_get_localised_text(&chunk, ecx, ebx, 0);

		rct_object_entry* entry_objects = NULL;
		uint8* eax = RCT2_GLOBAL(0x9ADAF4, uint8*);
		if ((uint32)eax != 0xFFFFFFFF){
			*((uint16*)eax) = 0;
			entry_objects = (rct_object_entry*)(eax + 2);
		}

		scenery_set->entry_count = 0;
		scenery_set->var_107 = 0;

		for (; *chunk != 0xFF; chunk += sizeof(rct_object_entry)){
			scenery_set->var_107++;

			if (entry_objects != NULL){
				memcpy(entry_objects, chunk, sizeof(rct_object_entry));
				entry_objects++;
				(*(eax + 1))++;
			}
			uint8 entry_type;
			uint8 entry_index = 0;
			if (!find_object_in_entry_group((rct_object_entry*)chunk, &entry_type, &entry_index))
				continue;

			uint16 scenery_entry = entry_index;

			switch (entry_type){
			case OBJECT_TYPE_SMALL_SCENERY:
				break;
			case OBJECT_TYPE_LARGE_SCENERY:
				scenery_entry |= 0x300;
				break;
			case OBJECT_TYPE_WALLS:
				scenery_entry |= 0x200;
				break;
			case OBJECT_TYPE_PATH_BITS:
				scenery_entry |= 0x100;
				break;
			default:
				scenery_entry |= 0x400;
				break;
			}

			scenery_set->scenery_entries[scenery_set->entry_count++] = scenery_entry;
		}

		chunk++;

		scenery_set->image = object_chunk_load_image_directory(&chunk);
	}
	else if ((flags & 0xFF) == 1){
		// Object Unload

		rct_scenery_set_entry* scenery_set = (rct_scenery_set_entry*)esi;
		scenery_set->name = 0;
		scenery_set->image = 0;
		scenery_set->entry_count = 0;
		scenery_set->var_107 = 0;

		memset(scenery_set->scenery_entries, 0, 256);
	}
	else if ((flags & 0xFF) == 2){
		return 0;
	}
	else if ((flags & 0xFF) == 3){
		int x = ecx, y = edx;

		rct_scenery_set_entry* scenery_set = (rct_scenery_set_entry*)ebp;

		if (!((flags >> 8) & 0xFF))
		{
			int image_id = scenery_set->image;

			image_id += 0x20600001;

			gfx_draw_sprite(dpi, image_id, x - 15, y - 14, 0);
		}
		else{
			RCT2_GLOBAL(0x13CE952, uint16) = scenery_set->var_107;

			gfx_draw_string_left(dpi, 3167, RCT2_ADDRESS(0x13CE952, void), 0, x, y);
		}
	}
	return flags;
}


//rct2: 0x00666E42
int paint_park_entrance_entry(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) == 0){
		// Object Load

		rct_entrance_type* entrance_type = (rct_entrance_type*)esi;
		uint8* pStringTable = (uint8*)(esi + sizeof(rct_entrance_type));

		entrance_type->string_idx = object_get_localised_text(&pStringTable, ecx, ebx, 0);

		entrance_type->image_id = object_chunk_load_image_directory(&pStringTable);

		if (RCT2_GLOBAL(0x9ADAF4, uint32) != 0xFFFFFFFF) *RCT2_GLOBAL(0x9ADAF4, uint16*) = 0;
	}
	else if ((flags & 0xFF) == 1){
		// Object Unload

		rct_entrance_type* entrance_type = (rct_entrance_type*)esi;
		entrance_type->string_idx = 0;
		entrance_type->image_id = 0;
	}
	else if ((flags & 0xFF) == 2){
		return 0;
	}
	else if ((flags & 0xFF) == 3){
		int x = ecx, y = edx;
		if (!((flags >> 8) & 0xFF))
		{
			rct_entrance_type* entrance_type = (rct_entrance_type*)ebp;

			dpi = clip_drawpixelinfo(dpi, x - 56, 112, y - 56, 112);
			if (dpi == NULL) return flags;

			int image_id = entrance_type->image_id;

			gfx_draw_sprite(dpi, image_id + 1, 24, 68, ebp);
			gfx_draw_sprite(dpi, image_id, 56, 84, ebp);
			gfx_draw_sprite(dpi, image_id + 2, 88, 100, ebp);

			rct2_free(dpi);
		}
	}
	return flags;
}

//rct2: 0x006E6E2A
int paint_water_entry(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) == 0){
		// Object Load

		rct_water_type* water_type = (rct_water_type*)esi;

		uint8_t* pStringTable = (uint8_t*)(esi + sizeof(rct_water_type));
		water_type->string_idx = object_get_localised_text(&pStringTable, ecx, ebx, 0);

		int image_id = object_chunk_load_image_directory(&pStringTable);
		water_type->image_id = image_id;
		water_type->var_06 = image_id + 1;
		water_type->var_0A = image_id + 4;

		if (RCT2_GLOBAL(0x9ADAF4, uint32) != 0xFFFFFFFF) *RCT2_GLOBAL(0x9ADAF4, uint16*) = 0;

		if (RCT2_GLOBAL(0x9ADAFD, uint8_t) == 0)
		{
			load_palette();
			gfx_invalidate_screen();
		}
	}
	else if ((flags & 0xFF) == 1){
		// Object Unload

		rct_water_type* water_type = (rct_water_type*)esi;
		water_type->string_idx = 0;
		water_type->image_id = 0;
		water_type->var_06 = 0;
		water_type->var_0A = 0;
	}
	else if ((flags & 0xFF) == 2){
		return 0;
	}
	else if ((flags & 0xFF) == 3){
		if (!((flags >> 8) & 0xFF)) 
			gfx_draw_string_centred(dpi, 3326, ecx, edx, 0, (void*)esi);
	}
	return flags;
}

//rct2: 0x0066B355
int paint_stex_entry(int flags, int ebx, int ecx, int edx, rct_drawpixelinfo* dpi, int esi, int ebp)
{
	if ((flags & 0xFF) == 0){
		// Object Load

		rct_stex_entry* stex_entry = (rct_stex_entry*)esi;
		uint8_t* pStringTable = (uint8_t*)(esi + 8);
		stex_entry->scenario_name = object_get_localised_text(&pStringTable, ecx, ebx, 0);
		stex_entry->park_name = object_get_localised_text(&pStringTable, ecx, ebx, 1);
		stex_entry->details = object_get_localised_text(&pStringTable, ecx, ebx, 2);
		if (RCT2_GLOBAL(0x9ADAF4, int) != -1) RCT2_GLOBAL(0x9ADAF4, uint16_t*)[0] = 0;
	}
	else if ((flags & 0xFF) == 1){
		// Object Unload

		rct_stex_entry* stex_entry = (rct_stex_entry*)esi;
		stex_entry->scenario_name = 0;
		stex_entry->park_name = 0;
		stex_entry->details = 0;
	}
	else if ((flags & 0xFF) == 2){
		return 0;
	}
	else if ((flags & 0xFF) == 3){
		int x = ecx, y = edx;
		rct_stex_entry* stex_entry = (rct_stex_entry*)ebp;
		rct_window* w = (rct_window*)esi;

		if (!((flags >> 8) & 0xFF)) {
			gfx_draw_string_centred(dpi, 0xCFE, x, y, 0, NULL);
		}
		else{
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, short) = stex_entry->details;
			int width = w->x + w->width - 4 - x;
			gfx_draw_string_left_wrapped(dpi, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, x, y, width, 3168, 0);
		}
	}
	return flags;
}

int object_paint(int type, int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp)
{
	//return RCT2_CALLPROC_X(RCT2_ADDRESS(0x0098D9D4, uint32)[type], eax, ebx, ecx, edx, esi, edi, ebp) & 0x100;
	switch (type)
	{
	case OBJECT_TYPE_RIDE:
		return paint_ride_entry(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);			
	case OBJECT_TYPE_SMALL_SCENERY:
		return paint_small_scenery(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_LARGE_SCENERY:
		return paint_large_scenery(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_WALLS:
		return paint_wall(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_BANNERS:
		return paint_banner(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_PATHS:
		return paint_path_entry(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);	
	case OBJECT_TYPE_PATH_BITS:
		return paint_path_bit(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_SCENERY_SETS:
		return paint_scenery_set(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_PARK_ENTRANCE:
		return paint_park_entrance_entry(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_WATER:
		return paint_water_entry(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	case OBJECT_TYPE_SCENARIO_TEXT:
		return paint_stex_entry(eax, ebx, ecx, edx, (rct_drawpixelinfo*)edi, esi, ebp);
	default:
		assert(false);
		return 0;
	}
}

/**
 *
 *  rct2: 0x006A9428
 */
int object_get_scenario_text(rct_object_entry *entry)
{
	// RCT2_CALLPROC_X(0x006A9428, 0, 0, 0, 0, 0, 0, (int)entry); return;

	rct_object_entry *installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	installedObject = object_list_find(entry);
	
	if (installedObject == NULL){
		log_error("Object not found: %.8s", entry->name);
		RCT2_GLOBAL(0x00F42BD9, uint8) = 0;
		return 0;
	}

	char path[260];
	char *objectPath = (char*)installedObject + 16;
	subsitute_path(path, RCT2_ADDRESS(RCT2_ADDRESS_OBJECT_DATA_PATH, char), objectPath);

	rct_object_entry openedEntry;
	FILE *file = fopen(path, "rb");
	if (file != NULL) {
		fread(&openedEntry, sizeof(rct_object_entry), 1, file);
		if (object_entry_compare(&openedEntry, entry)) {

			// Skip over the object entry
			char *pos = (char*)installedObject + sizeof(rct_object_entry);
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
				log_error("Opened object failed calculated checksum.");
				RCT2_GLOBAL(0x00F42BD9, uint8) = 2;
				free(chunk);
				return 0;
			}

			if (object_paint(openedEntry.flags & 0x0F, 2, 0, 0, 0, (int)chunk, 0, 0)) {
				// This is impossible for STEX entries to fail.
				log_error("Opened object failed paitn test.");
				RCT2_GLOBAL(0x00F42BD9, uint8) = 3;
				free(chunk);
				return 0;
			}

			// Save the real total images.
			int total_no_images = RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_NO_IMAGES, uint32);

			// This is being changed to force the images to be loaded into a different
			// image id.
			RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_NO_IMAGES, uint32) = 0x726E;
			RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TEXT_TEMP_CHUNK, uint32) = (int)chunk;
			// Not used anywhere.
			RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TEXT_TEMP_OBJECT, rct_object_entry) = openedEntry;

			// Tell text to be loaded into a different address
			RCT2_GLOBAL(0x009ADAFC, uint8) = 255;
			// Not used??
			RCT2_GLOBAL(0x009ADAFD, uint8) = 1;
			object_paint(openedEntry.flags & 0x0F, 0, 0, 0, 0, (int)chunk, 0, 0);			
			// Tell text to be loaded into normal address
			RCT2_GLOBAL(0x009ADAFC, uint8) = 0;
			// Not used??
			RCT2_GLOBAL(0x009ADAFD, uint8) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_NO_IMAGES, uint32) = total_no_images;
			return 1;
		}
		log_error("Opened object didn't match.");
		fclose(file);
		return 0;
	}
	log_error("File failed to open.");
	RCT2_GLOBAL(0x00F42BD9, uint8) = 0;
	return 0;
}

/**
 *
 *  rct2: 0x006A982D
 */
void object_free_scenario_text()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TEXT_TEMP_CHUNK, void*) != NULL) {
		rct2_free(RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TEXT_TEMP_CHUNK, void*));
		RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TEXT_TEMP_CHUNK, void*) = NULL;
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

	// Skip no of images
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

	// Skip no of images
	pos += 4;

	return pos;
}