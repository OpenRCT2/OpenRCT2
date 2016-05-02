#include "../addresses.h"
#include "../audio/audio.h"
#include "../cheats.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../localisation/string_ids.h"
#include "../management/finance.h"
#include "../network/network.h"
#include "../rct1.h"
#include "../util/sawyercoding.h"
#include "../util/util.h"
#include "../windows/error.h"
#include "../world/footpath.h"
#include "../world/scenery.h"
#include "ride.h"
#include "ride_data.h"
#include "track_data.h"
#include "track_design.h"
#include "track.h"

typedef struct {
	rct_map_element map_elements[MAX_MAP_ELEMENTS];
	rct_map_element *tile_pointers[256 * 256];
	rct_map_element *next_free_map_element;
	uint16 map_size_units;
	uint16 map_size_units_minus_2;
	uint16 map_size;
	uint8 current_rotation;
} map_backup;

static rct_track_td6 *track_design_open_from_buffer(uint8 *src, size_t srcLength);

rct_track_td6 *gActiveTrackDesign;
uint8 gTrackDesignPlaceFlags;
bool gTrackDesignSceneryToggle;
rct_xyz16 gTrackPreviewMin;
rct_xyz16 gTrackPreviewMax;
rct_xyz16 gTrackPreviewOrigin;

uint8 byte_F4414E;
static uint8 byte_F440D4;
static uint8 byte_F44150;

static map_backup *track_design_preview_backup_map();
static void track_design_preview_restore_map(map_backup *backup);
static void track_design_preview_clear_map();

static bool td4_track_has_boosters(rct_track_td6* track_design, uint8* track_elements);

static void copy(void *dst, uint8 **src, int length)
{
	memcpy(dst, *src, length);
	*src += length;
}

rct_track_td6 *track_design_open(const utf8 *path)
{
	SDL_RWops *file = SDL_RWFromFile(path, "rb");
	if (file != NULL) {
		// Read whole file into a buffer
		size_t bufferLength = (size_t)SDL_RWsize(file);
		uint8 *buffer = (uint8*)malloc(bufferLength);
		if (buffer == NULL) {
			log_error("Unable to allocate memory for track design file.");
			SDL_RWclose(file);
			return false;
		}
		SDL_RWread(file, buffer, bufferLength, 1);
		SDL_RWclose(file);

		if (!sawyercoding_validate_track_checksum(buffer, bufferLength)) {
			log_error("Track checksum failed.");
			free(buffer);
			return NULL;
		}

		// Decode the track data
		uint8 *decoded = malloc(0x10000);
		size_t decodedLength = sawyercoding_decode_td6(buffer, decoded, bufferLength);
		free(buffer);
		decoded = realloc(decoded, decodedLength);
		if (decoded == NULL) {
			log_error("failed to realloc");
		} else {
			rct_track_td6 *td6 = track_design_open_from_buffer(decoded, decodedLength);
			free(decoded);

			td6->name = track_design_get_name_from_path(path);
			return td6;
		}
	}
	return NULL;
}

static rct_track_td6 *track_design_open_from_buffer(uint8 *src, size_t srcLength)
{
	rct_track_td6 *td6 = calloc(1, sizeof(rct_track_td6));
	uint8 *readPtr = src;

	// Clear top of track_design as this is not loaded from the td4 files
	memset(&td6->track_spine_colour, 0, 67);

	// Read start of track_design
	copy(td6, &readPtr, 32);

	uint8 version = td6->version_and_colour_scheme >> 2;
	if (version > 2) {
		log_error("Unsupported track design.");
		free(td6);
		return NULL;
	}

	// In TD6 there are 32 sets of two byte vehicle colour specifiers
	// In TD4 there are 12 sets so the remaining 20 need to be read
	if (version == 2) {
		copy(&td6->vehicle_colours[12], &readPtr, 40);
	}

	copy(&td6->pad_48, &readPtr, 24);

	// In TD4 (version AA/CF) and TD6 both start actual track data at 0xA3
	if (version > 0) {
		copy(&td6->track_spine_colour, &readPtr, version == 1 ? 140 : 67);
	}

	// Read the actual track data to memory directly after the passed in TD6 struct
	size_t elementDataLength = srcLength - (readPtr - src);
	uint8 *elementData = malloc(elementDataLength);
	if (elementData == NULL) {
		log_error("Unable to allocate memory for TD6 element data.");
		free(td6);
		return NULL;
	}
	copy(elementData, &readPtr, elementDataLength);
	td6->elements = elementData;
	td6->elementsSize = elementDataLength;

	uint8 *final_track_element_location = elementData + elementDataLength;

	// TD4 files require some extra work to be recognised as TD6.
	if (version < 2) {
		// Set any element passed the tracks to 0xFF
		if (td6->type == RIDE_TYPE_MAZE) {
			rct_td6_maze_element* maze_element = (rct_td6_maze_element*)elementData;
			while (maze_element->all != 0) {
				maze_element++;
			}
			maze_element++;
			memset(maze_element, 255, final_track_element_location - (uint8*)maze_element);
		} else {
			rct_td6_track_element* track_element = (rct_td6_track_element*)elementData;
			while (track_element->type != 255) {
				track_element++;
			}
			memset(((uint8*)track_element) + 1, 255, final_track_element_location - (uint8*)track_element);
		}

		// Convert the colours from RCT1 to RCT2
		for (int i = 0; i < 32; i++) {
			rct_vehicle_colour *vehicleColour = &td6->vehicle_colours[i];
			vehicleColour->body_colour = rct1_get_colour(vehicleColour->body_colour);
			vehicleColour->trim_colour = rct1_get_colour(vehicleColour->trim_colour);
		}

		td6->track_spine_colour_rct1 = rct1_get_colour(td6->track_spine_colour_rct1);
		td6->track_rail_colour_rct1 = rct1_get_colour(td6->track_rail_colour_rct1);
		td6->track_support_colour_rct1 = rct1_get_colour(td6->track_support_colour_rct1);
		
		for (int i = 0; i < 4; i++) {
			td6->track_spine_colour[i] = rct1_get_colour(td6->track_spine_colour[i]);
			td6->track_rail_colour[i] = rct1_get_colour(td6->track_rail_colour[i]);
			td6->track_support_colour[i] = rct1_get_colour(td6->track_support_colour[i]);
		}

		// Highest drop height is 1bit = 3/4 a meter in TD6
		// Highest drop height is 1bit = 1/3 a meter in TD4
		// Not sure if this is correct??
		td6->highest_drop_height >>= 1;

		// If it has boosters then sadly track has to be discarded.
		if (td4_track_has_boosters(td6, elementData)) {
			log_error("Track design contains RCT1 boosters which are not yet supported.");
			free(td6->elements);
			free(td6);
			return NULL;
		}

		// Convert RCT1 ride type to RCT2 ride type
		uint8 rct1RideType = td6->type;
		if (rct1RideType == RCT1_RIDE_TYPE_WOODEN_ROLLER_COASTER) {
			td6->type = RIDE_TYPE_WOODEN_ROLLER_COASTER;
		} else if (rct1RideType == RCT1_RIDE_TYPE_STEEL_CORKSCREW_ROLLER_COASTER) {
			if (td6->vehicle_type == RCT1_VEHICLE_TYPE_HYPERCOASTER_TRAIN) {
				if (td6->ride_mode == RCT1_RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE) {
					td6->ride_mode = RIDE_MODE_CONTINUOUS_CIRCUIT;
				}
			}
		}

		// All TD4s that use powered launch use the type that doesn't pass the station.
		if (td6->ride_mode == RCT1_RIDE_MODE_POWERED_LAUNCH) {
			td6->ride_mode = RIDE_MODE_POWERED_LAUNCH;
		}

		// Convert RCT1 vehicle type to RCT2 vehicle type
		rct_object_entry *vehicle_object;
		if (td6->type == RIDE_TYPE_MAZE) {
			vehicle_object = RCT2_ADDRESS(0x0097F66C, rct_object_entry);
		} else {
			int vehicle_type = td6->vehicle_type;
			if (vehicle_type == RCT1_VEHICLE_TYPE_INVERTED_COASTER_TRAIN &&
				td6->type == RIDE_TYPE_INVERTED_ROLLER_COASTER
				) {
				vehicle_type = RCT1_VEHICLE_TYPE_4_ACROSS_INVERTED_COASTER_TRAIN;
			}
			vehicle_object = &RCT2_ADDRESS(0x0097F0DC, rct_object_entry)[vehicle_type];
		}
		memcpy(&td6->vehicle_object, vehicle_object, sizeof(rct_object_entry));

		// Further vehicle colour fixes
		for (int i = 0; i < 32; i++) {
			td6->vehicle_additional_colour[i] = td6->vehicle_colours[i].trim_colour;

			// RCT1 river rapids always had black seats.
			if (rct1RideType == RCT1_RIDE_TYPE_RIVER_RAPIDS) {
				td6->vehicle_colours[i].trim_colour = COLOUR_BLACK;
			}
		}

		td6->space_required_x = 255;
		td6->space_required_y = 255;
		td6->lift_hill_speed_num_circuits = 5;
	}

	td6->var_50 = min(
		td6->var_50,
		RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + 5 + (td6->type * 8), uint8)
	);

	// Set the element helper pointers
	uintptr_t entranceElementsStart;
	if (td6->type == RIDE_TYPE_MAZE) {
		td6->track_elements = NULL;
		td6->maze_elements = (rct_td6_maze_element*)td6->elements;

		rct_td6_maze_element *maze = td6->maze_elements;
		for (; maze->all != 0; maze++) { }
		entranceElementsStart = (uintptr_t)(++maze);
	} else {
		td6->maze_elements = NULL;
		td6->track_elements = (rct_td6_track_element*)td6->elements;

		rct_td6_track_element *track = td6->track_elements;
		for (; track->type != 0xFF; track++) { }
		entranceElementsStart = (uintptr_t)track + 1;
	}

	uintptr_t sceneryElementsStart;
	rct_td6_entrance_element *entranceElement = (rct_td6_entrance_element*)entranceElementsStart;
	td6->entrance_elements = entranceElement;
	for (; entranceElement->z != -1; entranceElement++) { }
	sceneryElementsStart = (uintptr_t)entranceElement + 1;

	rct_td6_scenery_element *sceneryElement = (rct_td6_scenery_element*)sceneryElementsStart;
	td6->scenery_elements = sceneryElement;

	return td6;
}

/**
 *
 *  rct2: 0x00677530
 * Returns true if it has booster track elements
 */
static bool td4_track_has_boosters(rct_track_td6* track_design, uint8* track_elements)
{
	if (track_design->type != RCT1_RIDE_TYPE_HEDGE_MAZE) {
		rct_td6_track_element *track = (rct_td6_track_element*)track_elements;
		for (; track->type != 0xFF; track++) {
			if (track->type == RCT1_TRACK_ELEM_BOOSTER) {
				return true;
			}
		}
	}
	return false;
}

void track_design_dispose(rct_track_td6 *td6)
{
	if (td6 != NULL) {
		free(td6->elements);
		free(td6->name);
		free(td6);
	}
}

uint32 *sub_6AB49A(rct_object_entry* entry)
{
	rct_object_entry* object_list_entry = object_list_find(entry);

	if (object_list_entry == NULL) return NULL;

	// Return the address of the last value of the list entry
	return (((uint32*)object_get_next(object_list_entry)) - 1);
}

/**
 *
 *  rct2: 0x006ABDB0
 */
static void track_design_load_scenery_objects(rct_track_td6 *td6)
{
	uint8 entry_index = RCT2_GLOBAL(0xF44157, uint8);
	rct_object_entry_extended* object_entry = &object_entry_groups[0].entries[entry_index];

	rct_object_entry* copied_entry = RCT2_ADDRESS(0xF43414, rct_object_entry);
	memcpy(copied_entry, object_entry, sizeof(rct_object_entry));

	object_unload_all();
	object_load_chunk(-1, copied_entry, 0);
	uint8 entry_type;
	find_object_in_entry_group(copied_entry, &entry_type, &entry_index);
	RCT2_GLOBAL(0xF44157, uint8) = entry_index;

	rct_td6_scenery_element *scenery = td6->scenery_elements;
	for (; (scenery->scenery_object.flags & 0xFF) != 0xFF; scenery++) {
		if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index)) {
			object_load_chunk(-1, &scenery->scenery_object, 0);
		}
	}

	reset_loaded_objects();
}

/**
 *
 *  rct2: 0x006D247A
 */
static void track_design_mirror_scenery(rct_track_td6 *td6)
{
	rct_td6_scenery_element *scenery = td6->scenery_elements;
	for (; (scenery->scenery_object.flags & 0xFF) != 0xFF; scenery++) {
		uint8 entry_type, entry_index;
		if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index)) {
			entry_type = scenery->scenery_object.flags & 0xF;
			if (entry_type != OBJECT_TYPE_PATHS) {
				continue;
			}
		}

		rct_scenery_entry *scenery_entry = (rct_scenery_entry*)object_entry_groups[entry_type].chunks[entry_index];
		switch (entry_type) {
		case OBJECT_TYPE_LARGE_SCENERY:
		{
			sint16 x1 = 0, x2 = 0, y1 = 0, y2 = 0;
			for (rct_large_scenery_tile* tile = scenery_entry->large_scenery.tiles; tile->x_offset != -1; tile++) {
				if (x1 > tile->x_offset) x1 = tile->x_offset;
				if (x2 < tile->x_offset) x2 = tile->x_offset;
				if (y1 > tile->y_offset) y1 = tile->y_offset;
				if (y2 > tile->y_offset) y2 = tile->y_offset;
			}

			switch (scenery->flags & 3) {
			case 0:
				scenery->y = (-(scenery->y * 32 + y1) - y2) / 32;
				break;
			case 1:
				scenery->x = (scenery->x * 32 + y2 + y1) / 32;
				scenery->y = (-(scenery->y * 32)) / 32;
				scenery->flags ^= (1 << 1);
				break;
			case 2:
				scenery->y = (-(scenery->y * 32 - y2) + y1) / 32;
				break;
			case 3:
				scenery->x = (scenery->x * 32 - y2 - y1) / 32;
				scenery->y = (-(scenery->y * 32)) / 32;
				scenery->flags ^= (1 << 1);
				break;
			}
			break;
		}
		case OBJECT_TYPE_SMALL_SCENERY:
			scenery->y = -scenery->y;

			if (scenery_entry->small_scenery.flags & SMALL_SCENERY_FLAG9) {
				scenery->flags ^= (1 << 0);
				if (!(scenery_entry->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE)) {
					scenery->flags ^= (1 << 2);
				}
				break;
			}
			if (scenery->flags & (1 << 0)) {
				scenery->flags ^= (1 << 1);
			}

			scenery->flags ^= (1 << 2);
			break;

		case OBJECT_TYPE_WALLS:
			scenery->y = -scenery->y;
			if (scenery->flags & (1 << 0)) {
				scenery->flags ^= (1 << 1);
			}
			break;

		case OBJECT_TYPE_PATHS:
			scenery->y = -scenery->y;

			if (scenery->flags & (1 << 5)) {
				scenery->flags ^= (1 << 6);
			}

			uint8 flags = scenery->flags;
			flags = ((flags & (1 << 3)) >> 2) | ((flags & (1 << 1)) << 2);
			scenery->flags &= 0xF5;
			scenery->flags |= flags;
		}
	}
}

/**
 *
 *  rct2: 0x006D2443
 */
static void track_design_mirror_ride(rct_track_td6 *td6)
{
	rct_td6_track_element *track = td6->track_elements;
	for (; track->type != 0xFF; track++) {
		track->type = RCT2_ADDRESS(0x0099EA1C, uint8)[track->type];
	}

	rct_td6_entrance_element *entrance = td6->entrance_elements;
	for (; entrance->z != -1; entrance++) {
		entrance->y = -entrance->y;
		if (entrance->direction & 1) {
			entrance->direction ^= (1 << 1);
		}
	}
}

/**
 *
 *  rct2: 0x006D25FA
 */
static void track_design_mirror_maze(rct_track_td6 *td6)
{
	rct_td6_maze_element *maze = td6->maze_elements;
	for (; maze->all != 0; maze++) {
		maze->y = -maze->y;

		if (maze->type == 0x8 || maze->type == 0x80) {
			if (maze->unk_2 & 1) {
				maze->unk_2 ^= (1 << 1);
			}
			continue;
		}

		uint16 maze_entry = maze->maze_entry;
		uint16 new_entry = 0;
		for (uint8 position = bitscanforward(maze_entry); position != 0xFF; position = bitscanforward(maze_entry)) {
			maze_entry &= ~(1 << position);
			new_entry |= (1 << RCT2_ADDRESS(0x00993EDC, uint8)[position]);
		}
		maze->maze_entry = new_entry;
	}
}

/**
 *
 *  rct2: 0x006D2436
 */
void track_design_mirror(rct_track_td6 *td6)
{
	if (td6->type == RIDE_TYPE_MAZE) {
		track_design_mirror_maze(td6);
	} else {
		track_design_mirror_ride(td6);
	}
	track_design_mirror_scenery(td6);
}

static void track_add_selection_tile(sint16 x, sint16 y)
{
	rct_xy16 *selectionTile = gMapSelectionTiles;
	for (; selectionTile->x != -1; selectionTile++) {
		if (selectionTile->x == x && selectionTile->y == y) {
			return;
		}
		if (selectionTile + 1 >= &gMapSelectionTiles[300]) {
			return;
		}
	}
	selectionTile->x = x;
	selectionTile->y = y;
	selectionTile++;
	selectionTile->x = -1;
}

static void track_design_update_max_min_coordinates(sint16 x, sint16 y, sint16 z)
{
	gTrackPreviewMin.x = min(gTrackPreviewMin.x, x);
	gTrackPreviewMax.x = max(gTrackPreviewMax.x, x);
	gTrackPreviewMin.y = min(gTrackPreviewMin.y, y);
	gTrackPreviewMax.y = max(gTrackPreviewMax.y, y);
	gTrackPreviewMin.z = min(gTrackPreviewMin.z, z);
	gTrackPreviewMax.z = max(gTrackPreviewMax.z, z);
}

/**
 *
 *  rct2: 0x006D0964
 */
int track_place_scenery(rct_td6_scenery_element *scenery_start, uint8 rideIndex, int originX, int originY, int originZ)
{
	for (uint8 mode = 0; mode <= 1; mode++) {
		if ((scenery_start->scenery_object.flags & 0xFF) != 0xFF) {
			byte_F4414E |= 1 << 2;
		}

		if (byte_F4414E & (1 << 7)) {
			continue;
		}

		for (rct_td6_scenery_element *scenery = scenery_start; (scenery->scenery_object.flags & 0xFF) != 0xFF; scenery++) {
			uint8 rotation = _currentTrackPieceDirection;
			rct_xy8 tile = { .x = originX / 32, .y = originY / 32 };
			switch (rotation & 3){
			case MAP_ELEMENT_DIRECTION_WEST:
				tile.x += scenery->x;
				tile.y += scenery->y;
				break;
			case MAP_ELEMENT_DIRECTION_NORTH:
				tile.x += scenery->y;
				tile.y -= scenery->x;
				break;
			case MAP_ELEMENT_DIRECTION_EAST:
				tile.x -= scenery->x;
				tile.y -= scenery->y;
				break;
			case MAP_ELEMENT_DIRECTION_SOUTH:
				tile.x -= scenery->y;
				tile.y += scenery->x;
				break;
			}

			rct_xy16 mapCoord = { .x = tile.x * 32, .y = tile.y * 32 };
			track_design_update_max_min_coordinates(mapCoord.x, mapCoord.y, originZ);

			if (byte_F440D4== 0 &&
				mode == 0){
				uint8 new_tile = 1;
				rct_xy16* selectionTile = gMapSelectionTiles;
				for (; selectionTile->x != -1; selectionTile++){
					if (selectionTile->x == tile.x && selectionTile->y == tile.y){
						new_tile = 0;
						break;
					}
					if (selectionTile + 1 >= &gMapSelectionTiles[300]){
						new_tile = 0;
						break;
					}
				}
				if (new_tile){
					selectionTile->x = tile.x;
					selectionTile->y = tile.y;
					selectionTile++;
					selectionTile->x = -1;
				}
			}

			if (byte_F440D4== 6 && mode == 0) {
				uint8 entry_type, entry_index;
				if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index)) {
					entry_type = scenery->scenery_object.flags & 0xF;
					if (entry_type != OBJECT_TYPE_PATHS) {
						entry_type = 0xFF;
					}
					if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) {
						entry_type = 0xFF;
					}

					entry_index = 0;
					for (rct_path_type *path = g_pathTypeEntries[0];
						entry_index < object_entry_group_counts[OBJECT_TYPE_PATHS];
						path = g_pathTypeEntries[entry_index], entry_index++
					) {
						if (path == (rct_path_type*)-1) {
							continue;
						}
						if (path->flags & (1 << 2)) {
							continue;
						}
					}

					if (entry_index == object_entry_group_counts[OBJECT_TYPE_PATHS]) {
						entry_type = 0xFF;
					}
				}
				int z;
				switch (entry_type) {
				case OBJECT_TYPE_SMALL_SCENERY:
					//bl
					rotation += scenery->flags;
					rotation &= 3;

					//bh
					uint8 quadrant = (scenery->flags >> 2) + _currentTrackPieceDirection;
					quadrant &= 3;

					uint8 bh = rotation | (quadrant << 6) | MAP_ELEMENT_TYPE_SCENERY;

					rct_scenery_entry* small_scenery = g_smallSceneryEntries[entry_index];
					if (!(small_scenery->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE) &&
						(small_scenery->small_scenery.flags & SMALL_SCENERY_FLAG9)){
						bh = bh;
					}
					else if (small_scenery->small_scenery.flags & (SMALL_SCENERY_FLAG9 | SMALL_SCENERY_FLAG24 | SMALL_SCENERY_FLAG25)){
						bh &= 0x3F;
					}

					z = (scenery->z * 8 + originZ) / 8;
					game_do_command(
						mapCoord.x,
						0x69 | bh << 8,
						mapCoord.y,
						(entry_index << 8) | z,
						GAME_COMMAND_REMOVE_SCENERY,
						0,
						0);
					break;
				case OBJECT_TYPE_LARGE_SCENERY:
					z = (scenery->z * 8 + originZ) / 8;
					game_do_command(
						mapCoord.x,
						0x69 | (((rotation + scenery->flags) & 0x3) << 8),
						mapCoord.y,
						z,
						GAME_COMMAND_REMOVE_LARGE_SCENERY,
						0,
						0);
					break;
				case OBJECT_TYPE_WALLS:
					z = (scenery->z * 8 + originZ) / 8;
					game_do_command(
						mapCoord.x,
						0x69,
						mapCoord.y,
						(z << 8) | ((rotation + scenery->flags) & 0x3),
						GAME_COMMAND_REMOVE_FENCE,
						0,
						0);
					break;
				case OBJECT_TYPE_PATHS:
					z = (scenery->z * 8 + originZ) / 8;
					footpath_remove(mapCoord.x, mapCoord.y, z, 0x69);
					break;
				}
			}

			if (byte_F440D4== 3){
				int z = scenery->z * 8 + RCT2_GLOBAL(0x00F440D5, sint16);
				if (z < RCT2_GLOBAL(0x00F44129, sint16)){
					RCT2_GLOBAL(0x00F44129, sint16) = z;
				}
			}

			if (byte_F440D4== 1 ||
				byte_F440D4== 2 ||
				byte_F440D4== 3 ||
				byte_F440D4== 4 ||
				byte_F440D4== 5){

				uint8 entry_type, entry_index;
				if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index)){
					entry_type = scenery->scenery_object.flags & 0xF;
					if (entry_type != OBJECT_TYPE_PATHS){
						byte_F4414E |= 1 << 1;
						continue;
					}

					if (gScreenFlags&SCREEN_FLAGS_TRACK_DESIGNER){
						byte_F4414E |= 1 << 1;
						continue;
					}

					entry_index = 0;
					for (rct_path_type* path = g_pathTypeEntries[0];
						entry_index < object_entry_group_counts[OBJECT_TYPE_PATHS];
						path = g_pathTypeEntries[entry_index], entry_index++){

						if (path == (rct_path_type*)-1)
							continue;
						if (path->flags & (1 << 2))
							continue;
					}

					if (entry_index == object_entry_group_counts[OBJECT_TYPE_PATHS]){
						byte_F4414E |= 1 << 1;
						continue;
					}
				}

				money32 cost;
				sint16 z;
				uint8 bl;

				switch (entry_type){
				case OBJECT_TYPE_SMALL_SCENERY:
					if (mode != 0)
						continue;
					if (byte_F440D4== 3)
						continue;

					rotation += scenery->flags;
					rotation &= 3;
					z = scenery->z * 8 + originZ;
					uint8 quadrant = ((scenery->flags >> 2) + _currentTrackPieceDirection) & 3;

					bl = 0x81;
					if (byte_F440D4== 5)bl = 0xA9;
					if (byte_F440D4== 4)bl = 0xE9;
					if (byte_F440D4== 1)bl = 0x80;

					gGameCommandErrorTitle = STR_CANT_POSITION_THIS_HERE;

					cost = game_do_command(
						mapCoord.x,
						bl | (entry_index << 8),
						mapCoord.y,
						quadrant | (scenery->primary_colour << 8),
						GAME_COMMAND_PLACE_SCENERY,
						rotation | (scenery->secondary_colour << 16),
						z
						);

					if (cost == MONEY32_UNDEFINED)
						cost = 0;
					break;
				case OBJECT_TYPE_LARGE_SCENERY:
					if (mode != 0)
						continue;
					if (byte_F440D4== 3)
						continue;

					rotation += scenery->flags;
					rotation &= 3;

					z = scenery->z * 8 + originZ;

					bl = 0x81;
					if (byte_F440D4== 5)bl = 0xA9;
					if (byte_F440D4== 4)bl = 0xE9;
					if (byte_F440D4== 1)bl = 0x80;

					cost = game_do_command(
						mapCoord.x,
						bl | (rotation << 8),
						mapCoord.y,
						scenery->primary_colour | (scenery->secondary_colour << 8),
						GAME_COMMAND_PLACE_LARGE_SCENERY,
						entry_index,
						z
						);

					if (cost == MONEY32_UNDEFINED)
						cost = 0;
					break;
				case OBJECT_TYPE_WALLS:
					if (mode != 0)
						continue;
					if (byte_F440D4== 3)
						continue;

					z = scenery->z * 8 + originZ;
					rotation += scenery->flags;
					rotation &= 3;

					bl = 1;
					if (byte_F440D4== 5)bl = 0xA9;
					if (byte_F440D4== 4)bl = 105;
					if (byte_F440D4== 1)bl = 0;

					gGameCommandErrorTitle = STR_CANT_BUILD_PARK_ENTRANCE_HERE;

					cost = game_do_command(
						mapCoord.x,
						bl | (entry_index << 8),
						mapCoord.y,
						rotation | (scenery->primary_colour << 8),
						GAME_COMMAND_PLACE_FENCE,
						z,
						scenery->secondary_colour | ((scenery->flags & 0xFC) << 6)
						);

					if (cost == MONEY32_UNDEFINED)
						cost = 0;
					break;
				case OBJECT_TYPE_PATHS:
					if (byte_F440D4== 3)
						continue;

					z = (scenery->z * 8 + originZ) / 8;

					if (mode == 0){
						if (scenery->flags & (1 << 7)){
							//dh
							entry_index |= (1 << 7);
						}

						uint8 bh = ((scenery->flags & 0xF) << rotation);
						bl = bh >> 4;
						bh = (bh | bl) & 0xF;
						bl = (((scenery->flags >> 5) + rotation) & 3) << 5;
						bh |= bl;

						bh |= scenery->flags & 0x90;

						bl = 1;
						if (byte_F440D4== 5)bl = 41;
						if (byte_F440D4== 4)bl = 105;
						if (byte_F440D4== 1)bl = 0;

						gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
						cost = game_do_command(mapCoord.x, bl | (bh << 8), mapCoord.y, z | (entry_index << 8), GAME_COMMAND_PLACE_PATH_FROM_TRACK, 0, 0);
					}
					else{
						if (byte_F440D4== 1)
							continue;

						rct_map_element* map_element = map_get_path_element_at(mapCoord.x / 32, mapCoord.y / 32, z);

						if (map_element == NULL)
							continue;

						sub_6A7594();
						footpath_remove_edges_at(mapCoord.x, mapCoord.y, map_element);

						bl = 1;
						if (byte_F440D4== 5)bl = 41;
						if (byte_F440D4== 4)bl = 105;

						footpath_connect_edges(mapCoord.x, mapCoord.y, map_element, bl);
						sub_6A759F();
						continue;
					}
					break;
				default:
					byte_F4414E |= 1 << 1;
					continue;
					break;
				}
				RCT2_GLOBAL(0x00F440D5, money32) += cost;
				if (byte_F440D4!= 2){
					if (cost == MONEY32_UNDEFINED){
						RCT2_GLOBAL(0x00F440D5, money32) = MONEY32_UNDEFINED;
					}
				}

				if (RCT2_GLOBAL(0x00F440D5, money32) != MONEY32_UNDEFINED)
					continue;

				if (byte_F440D4== 2)
					continue;

				return 0;
			}
		}
	}
	return 1;
}

int track_place_maze(rct_track_td6 *td6, sint16 x, sint16 y, sint16 z, uint8 rideIndex)
{
	if (byte_F440D4== 0){
		gMapSelectionTiles->x = -1;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_X, sint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Y, sint16) = y;

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, sint16) = map_element_height(x, y) & 0xFFFF;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = _currentTrackPieceDirection;
	}

	RCT2_GLOBAL(0x00F440D5, uint32) = 0;

	rct_td6_maze_element *maze = td6->maze_elements;
	for (; maze->all != 0; maze++){
		uint8 rotation = _currentTrackPieceDirection & 3;
		rct_xy16 mapCoord = { .x = maze->x * 32, .y = maze->y * 32 };
		rotate_map_coordinates(&mapCoord.x, &mapCoord.y, rotation);
		mapCoord.x += x;
		mapCoord.y += y;

		track_design_update_max_min_coordinates(mapCoord.x, mapCoord.y, z);

		if (byte_F440D4== 0) {
			track_add_selection_tile(mapCoord.x, mapCoord.y);
		}

		if (byte_F440D4== 1 ||
			byte_F440D4== 2 ||
			byte_F440D4== 4 ||
			byte_F440D4== 5
		) {

			uint8 bl;
			money32 cost = 0;
			uint16 maze_entry;
			switch (maze->type) {
			case 0x08:
				// entrance
				rotation += maze->unk_2;
				rotation &= 3;

				gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

				bl = 1;
				if (byte_F440D4== 4)bl = 0x69;
				if (byte_F440D4== 1){
					cost = game_do_command(mapCoord.x, 0 | rotation << 8, mapCoord.y, (z / 16) & 0xFF, GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, -1, 0);
				} else {
					cost = game_do_command(mapCoord.x, bl | rotation << 8, mapCoord.y, rideIndex, GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, 0, 0);
				}
				if (cost != MONEY32_UNDEFINED){
					byte_F4414E |= (1 << 0);
				}
				break;
			case 0x80:
				// exit
				rotation += maze->unk_2;
				rotation &= 3;

				gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

				bl = 1;
				if (byte_F440D4== 4)bl = 0x69;
				if (byte_F440D4== 1){
					cost = game_do_command(mapCoord.x, 0 | rotation << 8, mapCoord.y, ((z / 16) & 0xFF) | (1 << 8), GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, -1, 0);
				}
				else{
					cost = game_do_command(mapCoord.x, bl | rotation << 8, mapCoord.y, rideIndex | (1 << 8), GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, 0, 0);
				}
				if (cost != MONEY32_UNDEFINED){
					byte_F4414E |= (1 << 0);
				}
				break;
			default:
				maze_entry = rol16(maze->maze_entry, rotation * 4);

				bl = 1;
				if (byte_F440D4== 5)bl = 0x29;
				if (byte_F440D4== 4)bl = 0x69;
				if (byte_F440D4== 1)bl = 0;

				gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

				cost = game_do_command(mapCoord.x, bl | (maze_entry & 0xFF) << 8, mapCoord.y, rideIndex | (maze_entry & 0xFF00), GAME_COMMAND_PLACE_MAZE_DESIGN, z, 0);
				break;
			}

			RCT2_GLOBAL(0x00F440D5, money32) += cost;

			if (cost == MONEY32_UNDEFINED){
				RCT2_GLOBAL(0x00F440D5, money32) = cost;
				return 0;
			}
		}

		if (byte_F440D4== 3){
			if (mapCoord.x < 0) continue;
			if (mapCoord.y < 0) continue;
			if (mapCoord.x >= 256 * 32) continue;
			if (mapCoord.y >= 256 * 32) continue;

			rct_map_element *map_element = map_get_surface_element_at(mapCoord.x / 32, mapCoord.y / 32);
			sint16 map_height = map_element->base_height * 8;
			if (map_element->properties.surface.slope & 0xF) {
				map_height += 16;
				if (map_element->properties.surface.slope & 0x10) {
					map_height += 16;
				}
			}

			if (map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK) {
				sint16 water_height = map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
				water_height *= 16;
				if (water_height > map_height) {
					map_height = water_height;
				}
			}

			sint16 temp_z = z + RCT2_GLOBAL(0x00F440D5, sint16) - map_height;
			if (temp_z < 0) {
				RCT2_GLOBAL(0x00F440D5, sint16) -= temp_z;
			}
		}
	}

	if (byte_F440D4== 6) {
		game_do_command(0, 0x69, 0, rideIndex, GAME_COMMAND_DEMOLISH_RIDE, 0, 0);
	}

	gTrackPreviewOrigin = (rct_xyz16) { x, y, z };
	return 1;
}

int track_place_ride(rct_track_td6 *td6, sint16 x, sint16 y, sint16 z, uint8 rideIndex)
{
	gTrackPreviewOrigin = (rct_xyz16) { x, y, z };
	if (byte_F440D4== 0) {
		gMapSelectionTiles->x = -1;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_X, sint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Y, sint16) = y;

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, sint16) = map_element_height(x, y) & 0xFFFF;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = _currentTrackPieceDirection;
	}

	RCT2_GLOBAL(0x00F440D5, uint32) = 0;
	uint8 rotation = _currentTrackPieceDirection;

	rct_td6_track_element *track = td6->track_elements;
	for (; track->type != 0xFF; track++) {
		uint8 trackType = track->type;
		if (trackType == TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP) {
			trackType = 0xFF;
		}

		track_design_update_max_min_coordinates(x, y, z);

		if (byte_F440D4== 0) {
			for (const rct_preview_track* trackBlock = TrackBlocks[trackType]; trackBlock->index != 0xFF; trackBlock++) {
				rct_xy16 tile = { x, y };
				map_offset_with_rotation(&tile.x, &tile.y, trackBlock->x, trackBlock->y, rotation);
				track_design_update_max_min_coordinates(tile.x, tile.y, z);
				track_add_selection_tile(tile.x, tile.y);
			}
		}

		if (byte_F440D4== 6) {
			const rct_track_coordinates* track_coordinates = &TrackCoordinates[trackType];

			//di
			int temp_z = z;
			temp_z -= track_coordinates->z_begin;
			const rct_preview_track* trackBlock = TrackBlocks[trackType];

			temp_z += trackBlock->z;
			// rotation in bh
			// track_type in dl
			game_do_command(x, 0x69 | ((rotation & 3) << 8), y, trackType, GAME_COMMAND_REMOVE_TRACK, temp_z, 0);
		}

		if (byte_F440D4== 1 ||
			byte_F440D4== 2 ||
			byte_F440D4== 4 ||
			byte_F440D4== 5){
			const rct_track_coordinates* track_coordinates = &TrackCoordinates[trackType];

			//di
			int temp_z = z;
			temp_z -= track_coordinates->z_begin;
			uint32 edi = ((track->flags & 0xF) << 17) |
				((uint32)(track->flags & 0xF) << 28) |
				(((track->flags >> 4) & 0x3) << 24) |
				(temp_z & 0xFFFF);

			int edx = _currentRideIndex | (trackType << 8);
			if (track->flags & 0x80) edx |= 0x10000;
			if (track->flags & 0x40) edx |= 0x20000;

			uint8 bl = 1;
			if (byte_F440D4== 5)bl = 41;
			if (byte_F440D4== 4)bl = 105;
			if (byte_F440D4== 1)bl = 0;

			gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
			money32 cost = game_do_command(x, bl | (rotation << 8), y, edx, GAME_COMMAND_PLACE_TRACK, edi, 0);
			RCT2_GLOBAL(0x00F440D5, money32) += cost;

			if (cost == MONEY32_UNDEFINED){
				RCT2_GLOBAL(0x00F440D5, money32) = cost;
				return 0;
			}
		}

		if (byte_F440D4== 3) {
			int tempZ = z - TrackCoordinates[trackType].z_begin;
			for (const rct_preview_track* trackBlock = TrackBlocks[trackType]; trackBlock->index != 0xFF; trackBlock++) {
				rct_xy16 tile = { x, y };
				map_offset_with_rotation(&tile.x, &tile.y, trackBlock->x, trackBlock->y, rotation);
				if (tile.x < 0 || tile.y < 0 || tile.x >= (256 * 32) || tile.y >= (256 * 32)) {
					continue;
				}

				rct_map_element *mapElement = map_get_surface_element_at(tile.x >> 5, tile.y >> 5);
				if (mapElement == NULL) {
					return 0;
				}

				int height = mapElement->base_height * 8;
				if (mapElement->properties.surface.slope & 0x0F) {
					height += 16;
					if (mapElement->properties.surface.slope & 0x10) {
						height += 16;
					}
				}

				uint8 water_height = 16 * mapElement->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
				if (water_height > 0 && water_height > height) {
					height = water_height;
				}
				int heightDifference = tempZ + RCT2_GLOBAL(0x00F440D5, sint16) + trackBlock->z - height;
				if (heightDifference < 0) {
					RCT2_GLOBAL(0x00F440D5, sint16) -= heightDifference;
				}
			}
		}

		const rct_track_coordinates *track_coordinates = &TrackCoordinates[trackType];
		map_offset_with_rotation(&x, &y, track_coordinates->x, track_coordinates->y, rotation);
		z -= track_coordinates->z_begin;
		z += track_coordinates->z_end;

		rotation = (rotation + track_coordinates->rotation_end - track_coordinates->rotation_begin) & 3;
		if (track_coordinates->rotation_end & (1 << 2)) {
			rotation |= (1 << 2);
		} else {
			x += TileDirectionDelta[rotation].x;
			y += TileDirectionDelta[rotation].y;
		}
	}

	// Entrance elements
	//0x6D06D8
	rct_td6_entrance_element *entrance = td6->entrance_elements;
	for (; entrance->z != -1; entrance++) {
		rotation = _currentTrackPieceDirection & 3;
		x = entrance->x;
		y = entrance->y;
		rotate_map_coordinates(&x, &y, rotation);
		x += gTrackPreviewOrigin.x;
		y += gTrackPreviewOrigin.y;

		track_design_update_max_min_coordinates(x, y, z);

		if (byte_F440D4== 0) {
			track_add_selection_tile(x, y);
		}

		if (byte_F440D4== 1 ||
			byte_F440D4== 2 ||
			byte_F440D4== 4 ||
			byte_F440D4== 5
		) {
			rotation = (rotation + entrance->direction) & 3;
			uint8 isExit = 0;
			if (entrance->direction & (1 << 7)) {
				isExit = 1;
			}

			if (byte_F440D4!= 1){
				rct_xy16 tile = {
					x + TileDirectionDelta[rotation].x,
					y + TileDirectionDelta[rotation].y
				};
				rct_map_element* map_element = map_get_first_element_at(tile.x >> 5, tile.y >> 5);
				z = gTrackPreviewOrigin.z / 8;
				z += (entrance->z == (sint8)0x80) ? -1 : entrance->z;

				do {
					if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_TRACK) continue;
					if (map_element->base_height != z) continue;

					int di = (map_element->properties.track.sequence >> 4) & 0x07;
					uint8 bl = 1;
					if (byte_F440D4== 5) bl = 41;
					if (byte_F440D4== 4) bl = 105;
					if (byte_F440D4== 1) bl = 0;

					gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
					money32 cost = game_do_command(x, bl | (rotation << 8), y, rideIndex | (isExit << 8), GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, di, 0);
					RCT2_GLOBAL(0x00F440D5, money32) += cost;

					if (cost == MONEY32_UNDEFINED) {
						RCT2_GLOBAL(0x00F440D5, money32) = cost;
						return 0;
					}
					byte_F4414E |= (1 << 0);
					break;
				} while (!map_element_is_last_for_tile(map_element++));
			} else {
				//dl
				z = (entrance->z == (sint8)0x80) ? -1 : entrance->z;
				z *= 8;
				z += gTrackPreviewOrigin.z;
				z >>= 4;

				gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
				money32 cost = game_do_command(x, 0 | (rotation << 8), y, z | (isExit << 8), GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, -1, 0);
				if (cost == MONEY32_UNDEFINED) {
					RCT2_GLOBAL(0x00F440D5, money32) = cost;
					return 0;
				} else {
					RCT2_GLOBAL(0x00F440D5, money32) += cost;
					byte_F4414E |= (1 << 0);
				}
			}
		}
	}

	if (byte_F440D4== 6) {
		sub_6CB945(_currentRideIndex);
		rct_ride* ride = get_ride(_currentRideIndex);
		user_string_free(ride->name);
		ride->type = RIDE_TYPE_NULL;
	}
	return 1;
}

/**
 *
 *  rct2: 0x006D235B
 */
void sub_6D235B(uint8 ride_id){
	rct_ride* ride = get_ride(ride_id);
	user_string_free(ride->name);
	ride->type = RIDE_TYPE_NULL;
}

/**
* Places a virtual track. This can involve highlighting the surface tiles and showing the track layout. It is also used by
* the track preview window to place the whole track.
* Depending on the value of bl it modifies the function.
* bl == 0, Draw outlines on the ground
* bl == 1,
* bl == 2,
* bl == 3, Returns the z value of a succesful placement. Only lower 16 bits are the value, the rest may be garbage?
* bl == 4,
* bl == 5, Returns cost to create the track. All 32 bits are used. Places the track. (used by the preview)
* bl == 6, Clear white outlined track.
*  rct2: 0x006D01B3
*/
int sub_6D01B3(rct_track_td6 *td6, uint8 bl, uint8 rideIndex, int x, int y, int z)
{
	byte_F4414E = bl & 0x80;
	byte_F440D4= bl & 0x7F;
	if (gTrackDesignSceneryToggle) {
		byte_F4414E |= 0x80;
	}
	_currentRideIndex = rideIndex;

	gTrackPreviewMin = (rct_xyz16){ x, y, z };
	gTrackPreviewMax = (rct_xyz16){ x, y, z };

	RCT2_GLOBAL(0x00F44129, uint16) = 0;
	uint8 track_place_success = 0;
	if (td6->type == RIDE_TYPE_MAZE) {
		track_place_success = track_place_maze(td6, x, y, z, rideIndex);
	} else {
		track_place_success = track_place_ride(td6, x, y, z, rideIndex);
	}

	// Scenery elements
	rct_td6_scenery_element *scenery = td6->scenery_elements;
	if (track_place_success && scenery != NULL) {
		if (!track_place_scenery(
			scenery,
			rideIndex,
			gTrackPreviewOrigin.x,
			gTrackPreviewOrigin.y,
			gTrackPreviewOrigin.z
		)) {
			return RCT2_GLOBAL(0x00F440D5, money32);
		}
	}

	// 0x6D0FE6
	if (byte_F440D4== 0){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 0x6;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 3);
		map_invalidate_map_selection_tiles();
	}

	if (bl == 3) {
		return RCT2_GLOBAL(0x00F440D5, sint16);
	}
	return RCT2_GLOBAL(0x00F440D5, money32);
}

/**
 *
 *  rct2: 0x006D2189
 * ebx = ride_id
 * cost = edi
 */
bool sub_6D2189(rct_track_td6 *td6, money32 *cost, uint8 *rideId, uint8 *flags)
{
	*flags = 0;

	uint8 entry_type, entry_index;
	if (!find_object_in_entry_group(&td6->vehicle_object, &entry_type, &entry_index)) {
		entry_index = 0xFF;
	}

	uint8 rideIndex;
	uint8 colour;
	uint8 rideCreateFlags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
	if (ride_create_command(td6->type, entry_index, rideCreateFlags, &rideIndex, &colour) == MONEY32_UNDEFINED) {
		return false;
	}

	rct_ride *ride = get_ride(rideIndex);
	const utf8 *ride_name = RCT2_ADDRESS(0x009E3504, const utf8);
	rct_string_id new_ride_name = user_string_allocate(132, ride_name);
	if (new_ride_name != 0) {
		rct_string_id old_name = ride->name;
		ride->name = new_ride_name;
		user_string_free(old_name);
	}

	uint8 version = td6->version_and_colour_scheme >> 2;
	if (version == 2) {
		ride->entrance_style = td6->entrance_style;
	}

	if (version != 0) {
		memcpy(&ride->track_colour_main, &td6->track_spine_colour, 4);
		memcpy(&ride->track_colour_additional, &td6->track_rail_colour, 4);
		memcpy(&ride->track_colour_supports, &td6->track_support_colour, 4);
	} else {
		memset(&ride->track_colour_main, td6->track_spine_colour_rct1, 4);
		memset(&ride->track_colour_additional, td6->track_rail_colour_rct1, 4);
		memset(&ride->track_colour_supports, td6->track_support_colour_rct1, 4);
	}

	RCT2_GLOBAL(0x009D8150, uint8) |= 1;
	uint8 backup_rotation = _currentTrackPieceDirection;
	uint32 backup_park_flags = gParkFlags;
	gParkFlags &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
	int mapSize = gMapSize << 4;

	_currentTrackPieceDirection = 0;
	int z = sub_6D01B3(td6, PTD_OPERATION_GET_PLACE_Z, 0, mapSize, mapSize, 16);

	if (byte_F4414E & 4) {
		*flags |= 2;
	}
	
	z += 16 - RCT2_GLOBAL(0xF44129, uint16);

	int operation = PTD_OPERATION_GET_COST;
	if (byte_F4414E & 2) {
		operation |= 0x80;
		*flags |= 1;
	}

	money32 resultCost = sub_6D01B3(td6, operation, rideIndex, mapSize, mapSize, z);
	gParkFlags = backup_park_flags;

	if (resultCost != MONEY32_UNDEFINED) {
		if (!find_object_in_entry_group(&td6->vehicle_object, &entry_type, &entry_index)){
			*flags |= 4;
		}

		_currentTrackPieceDirection = backup_rotation;
		RCT2_GLOBAL(0x009D8150, uint8) &= ~1;
		*cost = resultCost;
		*rideId = rideIndex;
		return true;
	} else {
		_currentTrackPieceDirection = backup_rotation;
		user_string_free(ride->name);
		ride->type = RIDE_TYPE_NULL;
		RCT2_GLOBAL(0x009D8150, uint8) &= ~1;
		return false;
	}
}

money32 place_track_design(sint16 x, sint16 y, sint16 z, uint8 flags, uint8 *outRideIndex)
{
	*outRideIndex = 255;

	gCommandPosition.x = x + 16;
	gCommandPosition.y = y + 16;
	gCommandPosition.z = z;

	if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)) {
		if (game_is_paused() && !gCheatsBuildInPauseMode) {
			gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
			return MONEY32_UNDEFINED;
		}
	}

	rct_track_td6 *td6 = gActiveTrackDesign;
	rct_object_entry *rideEntryObject = &td6->vehicle_object;

	uint8 entryType, entryIndex;
	if (!find_object_in_entry_group(rideEntryObject, &entryType, &entryIndex)) {
		entryIndex = 0xFF;
	}

	uint8 rideIndex;
	uint8 rideColour;
	money32 createRideResult = ride_create_command(td6->type, entryIndex, GAME_COMMAND_FLAG_APPLY, &rideIndex, &rideColour);
	if (createRideResult == MONEY32_UNDEFINED) {
		gGameCommandErrorTitle = STR_CANT_CREATE_NEW_RIDE_ATTRACTION;
		gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
		return MONEY32_UNDEFINED;
	}

	rct_ride* ride = get_ride(rideIndex);
	if (ride->type == RIDE_TYPE_NULL) {
		log_warning("Invalid game command for track placement, ride id = %d", rideIndex);
		return MONEY32_UNDEFINED;
	}

	money32 cost = 0;
	if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
		byte_F44150 = 0;
		cost = sub_6D01B3(td6, PTD_OPERATION_1, rideIndex, x, y, z);
		if (byte_F4414E & (1 << 1)) {
			byte_F44150 |= 1 << 7;
			cost = sub_6D01B3(td6, 0x80 | PTD_OPERATION_1, rideIndex, x, y, z);
		}
	} else {
		uint8 operation;
		if (flags & GAME_COMMAND_FLAG_GHOST) {
			operation = PTD_OPERATION_4;
		} else {
			operation = PTD_OPERATION_2;
		}
		operation |= byte_F44150;
		cost = sub_6D01B3(td6, operation, rideIndex, x, y, z);
	}

	if (cost == MONEY32_UNDEFINED || !(flags & GAME_COMMAND_FLAG_APPLY)) {
		rct_string_id error_reason = gGameCommandErrorText;
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, rideIndex, GAME_COMMAND_DEMOLISH_RIDE, 0, 0);
		gGameCommandErrorText = error_reason;
		gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
		return cost;
	}

	if (entryIndex != 0xFF) {
		game_do_command(0, GAME_COMMAND_FLAG_APPLY | (2 << 8), 0, rideIndex | (entryIndex << 8), GAME_COMMAND_SET_RIDE_VEHICLES, 0, 0);
	}

	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (td6->ride_mode << 8), 0, rideIndex | (0 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (0 << 8), 0, rideIndex | (td6->number_of_trains << 8), GAME_COMMAND_SET_RIDE_VEHICLES, 0, 0);
	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (1 << 8), 0, rideIndex | (td6->number_of_cars_per_train << 8), GAME_COMMAND_SET_RIDE_VEHICLES, 0, 0);
	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (td6->depart_flags << 8), 0, rideIndex | (1 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (td6->min_waiting_time << 8), 0, rideIndex | (2 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (td6->max_waiting_time << 8), 0, rideIndex | (3 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (td6->var_50 << 8), 0, rideIndex | (4 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
	game_do_command(0, GAME_COMMAND_FLAG_APPLY | ((td6->lift_hill_speed_num_circuits & 0x1F) << 8), 0, rideIndex | (8 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
	
	uint8 num_circuits = td6->lift_hill_speed_num_circuits >> 5;
	if (num_circuits == 0) {
		num_circuits = 1;
	}
	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (num_circuits << 8), 0, rideIndex | (9 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
	
	ride_set_to_default_inspection_interval(rideIndex);
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN;
	ride->colour_scheme_type = td6->version_and_colour_scheme & 3;

	uint8 version = td6->version_and_colour_scheme >> 2;
	if (version >= 2) {
		ride->entrance_style = td6->entrance_style;
	}

	if (version >= 1) {
		for (int i = 0; i < 4; i++) {
			ride->track_colour_main[i] = td6->track_spine_colour[i];
			ride->track_colour_additional[i] = td6->track_rail_colour[i];
			ride->track_colour_supports[i] = td6->track_support_colour[i];
		}
	} else {
		for (int i = 0; i < 4; i++) {
			ride->track_colour_main[i] = td6->track_spine_colour_rct1;
			ride->track_colour_additional[i] = td6->track_rail_colour_rct1;
			ride->track_colour_supports[i] = td6->track_support_colour_rct1;
		}
	}

	for (int i = 0; i < 32; i++) {
		ride->vehicle_colours[i].body_colour = td6->vehicle_colours[i].body_colour;
		ride->vehicle_colours[i].trim_colour = td6->vehicle_colours[i].trim_colour;
		ride->vehicle_colours_extended[i] = td6->vehicle_additional_colour[i];
	}

	ride_set_name(rideIndex, td6->name);

	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
	*outRideIndex = rideIndex;
	return cost;
}

money32 place_maze_design(uint8 flags, uint8 rideIndex, uint16 mazeEntry, sint16 x, sint16 y, sint16 z)
{
	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
	gCommandPosition.x = x + 8;
	gCommandPosition.y = y + 8;
	gCommandPosition.z = z;
	if (!sub_68B044()) {
		return MONEY32_UNDEFINED;
	}

	if ((z & 15) != 0) {
		return MONEY32_UNDEFINED;
	}

	if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)) {
		if (game_is_paused()) {
			gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
			return MONEY32_UNDEFINED;
		}
	}

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		if (!(flags & GAME_COMMAND_FLAG_GHOST)) {
			footpath_remove_litter(x, y, z);
			map_remove_walls_at(floor2(x, 32), floor2(y, 32), z, z + 32);
		}
	}

	if (!gCheatsSandboxMode) {
		if (!map_is_location_owned(floor2(x, 32), floor2(y, 32), z)) {
			return MONEY32_UNDEFINED;
		}
	}

	// Check support height
	if (!gCheatsDisableSupportLimits) {
		rct_map_element *mapElement = map_get_surface_element_at(x >> 5, y >> 5);
		uint8 supportZ = (z + 32) >> 3;
		if (supportZ > mapElement->base_height) {
			uint8 supportHeight = (supportZ - mapElement->base_height) / 2;
			uint8 maxSupportHeight = RideData5[RIDE_TYPE_MAZE].max_height;
			if (supportHeight > maxSupportHeight) {
				gGameCommandErrorText = STR_TOO_HIGH_FOR_SUPPORTS;
				return MONEY32_UNDEFINED;
			}
		}
	}

	money32 cost = 0;
	// Clearance checks
	if (!gCheatsDisableClearanceChecks) {
		int fx = floor2(x, 32);
		int fy = floor2(y, 32);
		int fz0 = z >> 3;
		int fz1 = fz0 + 4;

		if (!map_can_construct_with_clear_at(fx, fy, fz0, fz1, &map_place_non_scenery_clear_func, 15, flags, &cost)) {
			return MONEY32_UNDEFINED;
		}

		uint8 elctgaw = RCT2_GLOBAL(RCT2_ADDRESS_ELEMENT_LOCATION_COMPARED_TO_GROUND_AND_WATER, uint8);
		if (elctgaw & ELEMENT_IS_UNDERWATER) {
			gGameCommandErrorText = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
			return MONEY32_UNDEFINED;
		}
		if (elctgaw & ELEMENT_IS_UNDERGROUND) {
			gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
			return MONEY32_UNDEFINED;
		}
	}

	rct_ride *ride = get_ride(rideIndex);
	
	// Calculate price
	money32 price = 0;
	if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
		price = RideTrackCosts[ride->type].track_price * RCT2_GLOBAL(0x0099DBC8, money32);
		price = (price >> 17) * 10;
	}

	cost += price;

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		if (RCT2_GLOBAL(0x009A8C28, uint8) == 1 && !(flags & GAME_COMMAND_FLAG_GHOST)) {
			rct_xyz16 coord;
			coord.x = x + 8;
			coord.y = y + 8;
			coord.z = z;
			network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
		}

		// Place track element
		int fx = floor2(x, 32);
		int fy = floor2(y, 32);
		int fz = z >> 3;
		rct_map_element *mapElement = map_element_insert(fx >> 5, fy >> 5, fz, 15);
		mapElement->clearance_height = fz + 4;
		mapElement->type = MAP_ELEMENT_TYPE_TRACK;
		mapElement->properties.track.type = 101;
		mapElement->properties.track.ride_index = rideIndex;
		mapElement->properties.track.maze_entry = mazeEntry;
		if (flags & GAME_COMMAND_FLAG_GHOST) {
			mapElement->flags |= MAP_ELEMENT_FLAG_GHOST;
		}

		map_invalidate_element(fx, fy, mapElement);

		ride->maze_tiles++;
		ride->station_heights[0] = mapElement->base_height;
		ride->station_starts[0] = 0;
		if (ride->maze_tiles == 1) {
			ride->overall_view = (fx >> 5) | ((fy >> 5) << 8);
		}
	}

	return cost;
}

/**
 *
 *  rct2: 0x006D13FE
 */
void game_command_place_track_design(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp)
{
	sint16 x = *eax & 0xFFFF;
	sint16 y = *ecx & 0xFFFF;
	sint16 z = *edi & 0xFFFF;
	uint8 flags = *ebx;
	uint8 rideIndex;
	*ebx = place_track_design(x, y, z, flags, &rideIndex);
	*edi = rideIndex;
}

/**
 *
 *  rct2: 0x006CDEE4
 */
void game_command_place_maze_design(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp)
{
	*ebx = place_maze_design(
		*ebx & 0xFF,
		*edx & 0xFF,
		((*ebx >> 8) & 0xFF) | (((*edx >> 8) & 0xFF) << 8),
		*eax & 0xFFFF,
		*ecx & 0xFFFF,
		*edi & 0xFFFF
	);
}

#pragma region Track Design Preview

/**
 *
 *  rct2: 0x006D1EF0
 */
void track_design_draw_preview(rct_track_td6 *td6, uint8 *pixels)
{
	// Make a copy of the map
	map_backup *mapBackup = track_design_preview_backup_map();
	if (mapBackup == NULL) {
		return;
	}
	track_design_preview_clear_map();

	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		track_design_load_scenery_objects(td6);
	}

	money32 cost;
	uint8 rideIndex;
	uint8 flags;
	if (!sub_6D2189(td6, &cost, &rideIndex, &flags)) {
		memset(pixels, 0, TRACK_PREVIEW_IMAGE_SIZE * 4);
		track_design_preview_restore_map(mapBackup);
		return;
	}
	td6->cost = cost;
	td6->track_flags = flags & 7;

	rct_viewport* view = RCT2_ADDRESS(0x9D8161, rct_viewport);
	rct_drawpixelinfo* dpi = RCT2_ADDRESS(0x9D8151, rct_drawpixelinfo);
	int left, top, right, bottom;

	int center_x = (gTrackPreviewMin.x + gTrackPreviewMax.x) / 2 + 16;
	int center_y = (gTrackPreviewMin.y + gTrackPreviewMax.y) / 2 + 16;
	int center_z = (gTrackPreviewMin.z + gTrackPreviewMax.z) / 2;

	int width = gTrackPreviewMax.x - gTrackPreviewMin.x;
	int height = gTrackPreviewMax.y - gTrackPreviewMin.y;
	if (width < height) {
		width = height;
	}

	int zoom_level = 1;
	if (width > 1120) {
		zoom_level = 2;
	}
	if (width > 2240) {
		zoom_level = 3;
	}

	width = 370 << zoom_level;
	height = 217 << zoom_level;

	int x = center_y - center_x - width / 2;
	int y = (center_y + center_x) / 2 - center_z - height / 2;

	gCurrentRotation = 0;

	view->width = 370;
	view->height = 217;
	view->view_width = width;
	view->view_height = height;
	view->x = 0;
	view->y = 0;
	view->view_x = x;
	view->view_y = y;
	view->zoom = zoom_level;
	view->flags = VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_INVISIBLE_SPRITES;

	dpi->zoom_level = zoom_level;
	dpi->x = 0;
	dpi->y = 0;
	dpi->width = 370;
	dpi->height = 217;
	dpi->pitch = 0;
	dpi->bits = pixels;

	top = y;
	left = x;
	bottom = y + height;
	right = x + width;

	viewport_paint(view, dpi, left, top, right, bottom);

	dpi->bits += TRACK_PREVIEW_IMAGE_SIZE;

	gCurrentRotation = 1;
	x = -center_y - center_x - width / 2;
	y = (center_y - center_x) / 2 - center_z - height / 2;

	view->view_x = x;
	view->view_y = y;
	top = y;
	left = x;
	bottom = y + height;
	right = x + width;

	viewport_paint(view, dpi, left, top, right, bottom);

	dpi->bits += TRACK_PREVIEW_IMAGE_SIZE;

	gCurrentRotation = 2;
	x =  center_x - center_y - width / 2;
	y = (-center_y - center_x) / 2 - center_z - height / 2;

	view->view_x = x;
	view->view_y = y;
	top = y;
	left = x;
	bottom = y + height;
	right = x + width;

	viewport_paint(view, dpi, left, top, right, bottom);

	dpi->bits += TRACK_PREVIEW_IMAGE_SIZE;

	gCurrentRotation = 3;
	x = center_x + center_y - width / 2;
	y = (center_x - center_y) / 2 - center_z - height / 2;

	view->view_x = x;
	view->view_y = y;
	top = y;
	left = x;
	bottom = y + height;
	right = x + width;

	viewport_paint(view, dpi, left, top, right, bottom);

	sub_6D235B(rideIndex);
	track_design_preview_restore_map(mapBackup);
}

/**
 * Create a backup of the map as it will be cleared for drawing the track
 * design preview.
 *  rct2: 0x006D1C68
 */
static map_backup *track_design_preview_backup_map()
{
	map_backup *backup = malloc(sizeof(map_backup));
	if (backup != NULL) {
		memcpy(
			backup->map_elements,
			RCT2_ADDRESS(RCT2_ADDRESS_MAP_ELEMENTS, rct_map_element),
			sizeof(backup->map_elements)
		);
		memcpy(
			backup->tile_pointers,
			RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*),
			sizeof(backup->tile_pointers)
		);
		backup->next_free_map_element = gNextFreeMapElement;
		backup->map_size_units = gMapSizeUnits;
		backup->map_size_units_minus_2 = gMapSizeMinus2;
		backup->map_size = gMapSize;
		backup->current_rotation = get_current_rotation();
	}
	return backup;
}

/**
 * Restores the map from a backup.
 *  rct2: 0x006D2378
 */
static void track_design_preview_restore_map(map_backup *backup)
{
	memcpy(
		RCT2_ADDRESS(RCT2_ADDRESS_MAP_ELEMENTS, rct_map_element),
		backup->map_elements,
		sizeof(backup->map_elements)
	);
	memcpy(
		RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*),
		backup->tile_pointers,
		sizeof(backup->tile_pointers)
	);
	gNextFreeMapElement = backup->next_free_map_element;
	gMapSizeUnits = backup->map_size_units;
	gMapSizeMinus2 = backup->map_size_units_minus_2;
	gMapSize = backup->map_size;
	gCurrentRotation = backup->current_rotation;

	free(backup);
}

/**
 * Resets all the map elements to surface tiles for track preview.
 *  rct2: 0x006D1D9A
 */
static void track_design_preview_clear_map()
{
	// These values were previously allocated in backup map but
	// it seems more fitting to place in this function
	gMapSizeUnits = 255 * 32;
	gMapSizeMinus2 = (264 * 32) - 2;
	gMapSize = 256;

	rct_map_element* map_element;
	for (int i = 0; i < MAX_TILE_MAP_ELEMENT_POINTERS; i++) {
		map_element = GET_MAP_ELEMENT(i);
		map_element->type = MAP_ELEMENT_TYPE_SURFACE;
		map_element->flags = MAP_ELEMENT_FLAG_LAST_TILE;
		map_element->base_height = 2;
		map_element->clearance_height = 0;
		map_element->properties.surface.slope = 0;
		map_element->properties.surface.terrain = 0;
		map_element->properties.surface.grass_length = 1;
		map_element->properties.surface.ownership = OWNERSHIP_OWNED;
	}
	map_update_tile_pointers();
}

#pragma endregion
