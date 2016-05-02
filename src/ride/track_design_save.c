#include "../audio/audio.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../localisation/string_ids.h"
#include "../interface/viewport.h"
#include "../util/sawyercoding.h"
#include "../windows/error.h"
#include "../world/scenery.h"
#include "ride_data.h"
#include "track.h"
#include "track_data.h"
#include "track_design.h"

#define TRACK_MAX_SAVED_MAP_ELEMENTS 1500

static size_t _trackSavedMapElementsCount;
#ifdef NO_RCT2
	static rct_map_element *_trackSavedMapElements[TRACK_MAX_SAVED_MAP_ELEMENTS];
#else
	static rct_map_element **_trackSavedMapElements = (rct_map_element**)0x00F63674;
#endif

static bool track_design_save_should_select_scenery_around(int rideIndex, rct_map_element *mapElement);
static void track_design_save_select_nearby_scenery_for_tile(int rideIndex, int cx, int cy);
static bool track_design_save_contains_map_element(rct_map_element *mapElement);
static bool track_design_save_add_map_element(int interactionType, int x, int y, rct_map_element *mapElement);
static void track_design_save_remove_map_element(int interactionType, int x, int y, rct_map_element *mapElement);
static bool track_design_save_copy_scenery_to_track(uint8 **track_pointer);
static bool track_design_save_to_td6(rct_track_td6 *td6, uint8 rideIndex);
static bool track_design_save_to_td6_for_maze(uint8 rideIndex, rct_track_td6 *td6, uint8 *trackElements);
static bool track_design_save_to_td6_for_tracked_ride(uint8 rideIndex, rct_track_td6 *td6, uint8 *trackElements);
static bool track_design_save_to_file(rct_track_td6 *track_design, utf8 *path);

void track_design_save_init()
{
	_trackSavedMapElementsCount = 0;
	memset(_trackSavedMapElements, 0, sizeof(_trackSavedMapElements));
#ifndef NO_RCT2
	_trackSavedMapElements[0] = (rct_map_element*)-1;
#endif
}

/**
 *
 *  rct2: 0x006D2B07
 */
void track_design_save_toggle_map_element(int interactionType, int x, int y, rct_map_element *mapElement)
{
	if (track_design_save_contains_map_element(mapElement)) {
		track_design_save_remove_map_element(interactionType, x, y, mapElement);
	} else {
		if (!track_design_save_add_map_element(interactionType, x, y, mapElement)) {
			window_error_open(
				STR_SAVE_TRACK_SCENERY_UNABLE_TO_SELECT_ADDITIONAL_ITEM_OF_SCENERY,
				STR_SAVE_TRACK_SCENERY_TOO_MANY_ITEMS_SELECTED
			);
		}
	}
}

/**
 *
 *  rct2: 0x006D303D
 */
void track_design_save_select_nearby_scenery(int rideIndex)
{
	rct_map_element *mapElement;

	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++) {
			mapElement = map_get_first_element_at(x, y);
			do {
				if (track_design_save_should_select_scenery_around(rideIndex, mapElement)) {
					track_design_save_select_nearby_scenery_for_tile(rideIndex, x, y);
					break;
				}
			} while (!map_element_is_last_for_tile(mapElement++));
		}
	}
	gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x006D3026
 */
void track_design_save_reset_scenery()
{
	RCT2_GLOBAL(0x009DA193, uint8) = 255;
	track_design_save_init();
	gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x006D2804, 0x006D264D
 */
bool track_design_save(uint8 rideIndex)
{
	rct_ride* ride = get_ride(rideIndex);

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)){
		window_error_open(STR_CANT_SAVE_TRACK_DESIGN, gGameCommandErrorText);
		return false;
	}

	if (ride->ratings.excitement == (ride_rating)0xFFFF){
		window_error_open(STR_CANT_SAVE_TRACK_DESIGN, gGameCommandErrorText);
		return false;
	}

	if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK)) {
		window_error_open(STR_CANT_SAVE_TRACK_DESIGN, gGameCommandErrorText);
		return false;
	}

	rct_track_td6 *td6 = RCT2_ADDRESS(0x009D8178, rct_track_td6);
	if (!track_design_save_to_td6(td6, rideIndex)) {
		window_error_open(STR_CANT_SAVE_TRACK_DESIGN, gGameCommandErrorText);
		return false;
	}

	uint8 *trackPointer = RCT2_GLOBAL(0x00F44058, uint8*);
	if (RCT2_GLOBAL(0x009DEA6F, uint8) & 1) {
		if (!track_design_save_copy_scenery_to_track(&trackPointer)) {
			return false;
		}
	}

	while (trackPointer < RCT2_ADDRESS(0x009DE217, uint8))*trackPointer++ = 0;

	char track_name[MAX_PATH];
	// Get track name
	format_string(track_name, ride->name, &ride->name_arguments);

	char path[MAX_PATH];
	substitute_path(path, RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), track_name);

	// Save track design
	format_string(RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char), 2306, NULL);

	// Track design files
	format_string(RCT2_ADDRESS(0x141EE68, char), 2305, NULL);

	// Show save dialog
	utf8 initialDirectory[MAX_PATH];
	{
		strcpy(initialDirectory, path);
		utf8 *a = strrchr(initialDirectory, '/');
		utf8 *b = strrchr(initialDirectory, '\\');
		utf8 *c = max(a, b);
		if (c != NULL) {
			*c = '\0';
		}
	}

	file_dialog_desc desc;
	memset(&desc, 0, sizeof(desc));
	desc.type = FD_SAVE;
	desc.title = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, utf8);
	desc.initial_directory = initialDirectory;
	desc.default_filename = path;
	desc.filters[0].name = language_get_string(STR_OPENRCT2_TRACK_DESIGN_FILE);
	desc.filters[0].pattern = "*.td6";

	audio_pause_sounds();
	bool result = platform_open_common_file_dialog(path, &desc);
	audio_unpause_sounds();

	if (!result) {
		ride_list_item item = { .type = 0xFD, .entry_index = 0 };
		// track_load_list(item);
		return true;
	}

	track_design_save_to_file(RCT2_ADDRESS(0x009D8178, rct_track_td6), path);

	ride_list_item item = { .type = 0xFC, .entry_index = 0 };
	// track_load_list(item);
	gfx_invalidate_screen();
	return true;
}

static bool track_design_save_contains_map_element(rct_map_element *mapElement)
{
	for (size_t i = 0; i < _trackSavedMapElementsCount; i++) {
		if (_trackSavedMapElements[i] == mapElement) {
			return true;
		}
	}
	return false;
}

static int map_element_get_total_element_count(rct_map_element *mapElement)
{
	int elementCount;
	rct_scenery_entry *sceneryEntry;
	rct_large_scenery_tile *tile;

	switch (map_element_get_type(mapElement)) {
	case MAP_ELEMENT_TYPE_PATH:
	case MAP_ELEMENT_TYPE_SCENERY:
	case MAP_ELEMENT_TYPE_FENCE:
		return 1;

	case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
		sceneryEntry = g_largeSceneryEntries[mapElement->properties.scenerymultiple.type & 0x3FF];
		tile = sceneryEntry->large_scenery.tiles;
		elementCount = 0;
		do {
			tile++;
			elementCount++;
		} while (tile->x_offset != (sint16)0xFFFF);
		return elementCount;

	default:
		return 0;
	}
}

static bool track_design_save_scenery_is_null(rct_td6_scenery_element *trackScenery)
{
	return *((uint8*)trackScenery) == 0xFF;
}

static void track_design_save_scenery_set_to_null(rct_td6_scenery_element *trackScenery)
{
	*((uint8*)trackScenery) = 0xFF;
}

/**
 *
 *  rct2: 0x006D2ED2
 */
static bool track_design_save_can_add_map_element(rct_map_element *mapElement)
{
	int newElementCount = map_element_get_total_element_count(mapElement);
	if (newElementCount == 0) {
		return false;
	}

	// Get number of spare elements left
	int spareSavedElements = TRACK_MAX_SAVED_MAP_ELEMENTS - _trackSavedMapElementsCount;
	if (newElementCount > spareSavedElements) {
		// No more spare saved elements left
		return false;
	}

	// Probably checking for spare elements in the TD6 struct
	rct_td6_scenery_element *trackScenery = (rct_td6_scenery_element*)0x009DA193;
	while (!track_design_save_scenery_is_null(trackScenery)) { trackScenery++; }
	if (trackScenery >= (rct_td6_scenery_element*)0x9DE207) {
		return false;
	}

	return true;
}

/**
 *
 *  rct2: 0x006D2F4C
 */
static void track_design_save_push_map_element(int x, int y, rct_map_element *mapElement)
{
	if (_trackSavedMapElementsCount < TRACK_MAX_SAVED_MAP_ELEMENTS) {
		_trackSavedMapElements[_trackSavedMapElementsCount++] = mapElement;
		map_invalidate_tile_full(x, y);

#ifndef NO_RCT2
		_trackSavedMapElements[_trackSavedMapElementsCount] = (rct_map_element*)-1;
#endif
	}
}

/**
 *
 *  rct2: 0x006D2FA7
 */
static void track_design_save_push_map_element_desc(rct_object_entry *entry, int x, int y, int z, uint8 flags, uint8 primaryColour, uint8 secondaryColour)
{
	rct_td6_scenery_element *item = (rct_td6_scenery_element*)0x009DA193;
	while (!track_design_save_scenery_is_null(item)) { item++; }

	item->scenery_object = *entry;
	item->x = x / 32;
	item->y = y / 32;
	item->z = z;
	item->flags = flags;
	item->primary_colour = primaryColour;
	item->secondary_colour = secondaryColour;

	track_design_save_scenery_set_to_null(item + 1);
}

static void track_design_save_add_scenery(int x, int y, rct_map_element *mapElement)
{
	int entryType = mapElement->properties.scenery.type;
	rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_SMALL_SCENERY].entries[entryType];

	uint8 flags = 0;
	flags |= mapElement->type & 3;
	flags |= (mapElement->type & 0xC0) >> 4;

	uint8 primaryColour = mapElement->properties.scenery.colour_1 & 0x1F;
	uint8 secondaryColour = mapElement->properties.scenery.colour_2 & 0x1F;

	track_design_save_push_map_element(x, y, mapElement);
	track_design_save_push_map_element_desc(entry, x, y, mapElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_add_large_scenery(int x, int y, rct_map_element *mapElement)
{
	rct_large_scenery_tile *sceneryTiles, *tile;
	int x0, y0, z0, z;
	int direction, sequence;

	int entryType = mapElement->properties.scenerymultiple.type & 0x3FF;
	rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_LARGE_SCENERY].entries[entryType];
	sceneryTiles = g_largeSceneryEntries[entryType]->large_scenery.tiles;

	z = mapElement->base_height;
	direction = mapElement->type & 3;
	sequence = mapElement->properties.scenerymultiple.type >> 10;

	if (!map_large_scenery_get_origin(x, y, z, direction, sequence, &x0, &y0, &z0, NULL)) {
		return;
	}

	// Iterate through each tile of the large scenery element
	sequence = 0;
	for (tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++) {
		sint16 offsetX = tile->x_offset;
		sint16 offsetY = tile->y_offset;
		rotate_map_coordinates(&offsetX, &offsetY, direction);

		x = x0 + offsetX;
		y = y0 + offsetY;
		z = (z0 + tile->z_offset) / 8;
		mapElement = map_get_large_scenery_segment(x, y, z, direction, sequence);
		if (mapElement != NULL) {
			if (sequence == 0) {
				uint8 flags = mapElement->type & 3;
				uint8 primaryColour = mapElement->properties.scenerymultiple.colour[0] & 0x1F;
				uint8 secondaryColour = mapElement->properties.scenerymultiple.colour[1] & 0x1F;

				track_design_save_push_map_element_desc(entry, x, y, z, flags, primaryColour, secondaryColour);
			}
			track_design_save_push_map_element(x, y, mapElement);
		}
	}
}

static void track_design_save_add_wall(int x, int y, rct_map_element *mapElement)
{
	int entryType = mapElement->properties.fence.type;
	rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_WALLS].entries[entryType];

	uint8 flags = 0;
	flags |= mapElement->type & 3;
	flags |= mapElement->properties.fence.item[0] << 2;

	uint8 secondaryColour = ((mapElement->flags & 0x60) >> 2) | (mapElement->properties.fence.item[1] >> 5);
	uint8 primaryColour = mapElement->properties.fence.item[1] & 0x1F;

	track_design_save_push_map_element(x, y, mapElement);
	track_design_save_push_map_element_desc(entry, x, y, mapElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_add_footpath(int x, int y, rct_map_element *mapElement)
{
	int entryType = mapElement->properties.path.type >> 4;
	rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_PATHS].entries[entryType];

	uint8 flags = 0;
	flags |= mapElement->properties.path.edges & 0x0F;
	flags |= (mapElement->properties.path.type & 4) << 2;
	flags |= (mapElement->properties.path.type & 3) << 5;
	flags |= (mapElement->type & 1) << 7;

	track_design_save_push_map_element(x, y, mapElement);
	track_design_save_push_map_element_desc(entry, x, y, mapElement->base_height, flags, 0, 0);
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static bool track_design_save_add_map_element(int interactionType, int x, int y, rct_map_element *mapElement)
{
	if (!track_design_save_can_add_map_element(mapElement)) {
		return false;
	}

	switch (interactionType) {
	case VIEWPORT_INTERACTION_ITEM_SCENERY:
		track_design_save_add_scenery(x, y, mapElement);
		return true;
	case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
		track_design_save_add_large_scenery(x, y, mapElement);
		return true;
	case VIEWPORT_INTERACTION_ITEM_WALL:
		track_design_save_add_wall(x, y, mapElement);
		return true;
	case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
		track_design_save_add_footpath(x, y, mapElement);
		return true;
	default:
		return false;
	}
}

/**
 *
 *  rct2: 0x006D2F78
 */
static void track_design_save_pop_map_element(int x, int y, rct_map_element *mapElement)
{
	map_invalidate_tile_full(x, y);

	// Find index of map element to remove
	size_t removeIndex = SIZE_MAX;
	for (size_t i = 0; i < _trackSavedMapElementsCount; i++) {
		if (_trackSavedMapElements[i] == mapElement) {
			removeIndex = i;
		}
	}

	// Remove map element from list
	if (removeIndex != SIZE_MAX) {
		size_t remainingNumItems = _trackSavedMapElementsCount - removeIndex - 1;
		if (remainingNumItems > 0) {
			memmove(
				&_trackSavedMapElements[removeIndex],
				&_trackSavedMapElements[removeIndex + 1],
				remainingNumItems * sizeof(rct_map_element*)
			);
		}
		_trackSavedMapElementsCount--;
#ifdef NO_RCT2
		_trackSavedMapElements[_trackSavedMapElementsCount] = NULL;
#else
		_trackSavedMapElements[_trackSavedMapElementsCount] = (rct_map_element*)-1;
#endif
	}
}

/**
 *
 *  rct2: 0x006D2FDD
 */
static void track_design_save_pop_map_element_desc(rct_object_entry *entry, int x, int y, int z, uint8 flags, uint8 primaryColour, uint8 secondaryColour)
{
	int removeIndex = -1;
	int totalItems = 0;

	rct_td6_scenery_element *items = (rct_td6_scenery_element*)0x009DA193;
	rct_td6_scenery_element *item = items;
	for (; !track_design_save_scenery_is_null(item); item++, totalItems++) {
		if (item->x != x / 32) continue;
		if (item->y != y / 32) continue;
		if (item->z != z) continue;
		if (item->flags != flags) continue;
		if (!object_entry_compare(&item->scenery_object, entry)) continue;

		removeIndex = totalItems;
	}

	if (removeIndex == -1) {
		return;
	}

	// Remove item and shift rest up one item
	if (removeIndex < totalItems - 1) {
		memmove(&items[removeIndex], &items[removeIndex + 1], (totalItems - removeIndex - 1) * sizeof(rct_td6_scenery_element));
	}
	track_design_save_scenery_set_to_null(&items[totalItems - 1]);
}

static void track_design_save_remove_scenery(int x, int y, rct_map_element *mapElement)
{
	int entryType = mapElement->properties.scenery.type;
	rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_SMALL_SCENERY].entries[entryType];

	uint8 flags = 0;
	flags |= mapElement->type & 3;
	flags |= (mapElement->type & 0xC0) >> 4;

	uint8 primaryColour = mapElement->properties.scenery.colour_1 & 0x1F;
	uint8 secondaryColour = mapElement->properties.scenery.colour_2 & 0x1F;

	track_design_save_pop_map_element(x, y, mapElement);
	track_design_save_pop_map_element_desc(entry, x, y, mapElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_remove_large_scenery(int x, int y, rct_map_element *mapElement)
{
	rct_large_scenery_tile *sceneryTiles, *tile;
	int x0, y0, z0, z;
	int direction, sequence;

	int entryType = mapElement->properties.scenerymultiple.type & 0x3FF;
	rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_LARGE_SCENERY].entries[entryType];
	sceneryTiles = g_largeSceneryEntries[entryType]->large_scenery.tiles;

	z = mapElement->base_height;
	direction = mapElement->type & 3;
	sequence = mapElement->properties.scenerymultiple.type >> 10;

	if (!map_large_scenery_get_origin(x, y, z, direction, sequence, &x0, &y0, &z0, NULL)) {
		return;
	}

	// Iterate through each tile of the large scenery element
	sequence = 0;
	for (tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++) {
		sint16 offsetX = tile->x_offset;
		sint16 offsetY = tile->y_offset;
		rotate_map_coordinates(&offsetX, &offsetY, direction);

		x = x0 + offsetX;
		y = y0 + offsetY;
		z = (z0 + tile->z_offset) / 8;
		mapElement = map_get_large_scenery_segment(x, y, z, direction, sequence);
		if (mapElement != NULL) {
			if (sequence == 0) {
				uint8 flags = mapElement->type & 3;
				uint8 primaryColour = mapElement->properties.scenerymultiple.colour[0] & 0x1F;
				uint8 secondaryColour = mapElement->properties.scenerymultiple.colour[1] & 0x1F;

				track_design_save_pop_map_element_desc(entry, x, y, z, flags, primaryColour, secondaryColour);
			}
			track_design_save_pop_map_element(x, y, mapElement);
		}
	}
}

static void track_design_save_remove_wall(int x, int y, rct_map_element *mapElement)
{
	int entryType = mapElement->properties.fence.type;
	rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_WALLS].entries[entryType];

	uint8 flags = 0;
	flags |= mapElement->type & 3;
	flags |= mapElement->properties.fence.item[0] << 2;

	uint8 secondaryColour = ((mapElement->flags & 0x60) >> 2) | (mapElement->properties.fence.item[1] >> 5);
	uint8 primaryColour = mapElement->properties.fence.item[1] & 0x1F;

	track_design_save_pop_map_element(x, y, mapElement);
	track_design_save_pop_map_element_desc(entry, x, y, mapElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_remove_footpath(int x, int y, rct_map_element *mapElement)
{
	int entryType = mapElement->properties.path.type >> 4;
	rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_PATHS].entries[entryType];

	uint8 flags = 0;
	flags |= mapElement->properties.path.edges & 0x0F;
	flags |= (mapElement->properties.path.type & 4) << 2;
	flags |= (mapElement->properties.path.type & 3) << 5;
	flags |= (mapElement->type & 1) << 7;

	track_design_save_pop_map_element(x, y, mapElement);
	track_design_save_pop_map_element_desc(entry, x, y, mapElement->base_height, flags, 0, 0);
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static void track_design_save_remove_map_element(int interactionType, int x, int y, rct_map_element *mapElement)
{
	switch (interactionType) {
	case VIEWPORT_INTERACTION_ITEM_SCENERY:
		track_design_save_remove_scenery(x, y, mapElement);
		break;
	case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
		track_design_save_remove_large_scenery(x, y, mapElement);
		break;
	case VIEWPORT_INTERACTION_ITEM_WALL:
		track_design_save_remove_wall(x, y, mapElement);
		break;
	case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
		track_design_save_remove_footpath(x, y, mapElement);
		break;
	}
}

static bool track_design_save_should_select_scenery_around(int rideIndex, rct_map_element *mapElement)
{
	switch (map_element_get_type(mapElement)) {
	case MAP_ELEMENT_TYPE_PATH:
		if ((mapElement->type & 1) && mapElement->properties.path.addition_status == rideIndex)
			return true;
		break;
	case MAP_ELEMENT_TYPE_TRACK:
		if (mapElement->properties.track.ride_index == rideIndex)
			return true;
		break;
	case MAP_ELEMENT_TYPE_ENTRANCE:
		if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE)
			break;
		if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_EXIT)
			break;
		if (mapElement->properties.entrance.ride_index == rideIndex)
			return true;
		break;
	}
	return false;
}

static void track_design_save_select_nearby_scenery_for_tile(int rideIndex, int cx, int cy)
{
	rct_map_element *mapElement;

	for (int y = cy - 1; y <= cy + 1; y++) {
		for (int x = cx - 1; x <= cx + 1; x++) {
			mapElement = map_get_first_element_at(x, y);
			do {
				int interactionType = VIEWPORT_INTERACTION_ITEM_NONE;
				switch (map_element_get_type(mapElement)) {
				case MAP_ELEMENT_TYPE_PATH:
					if (!(mapElement->type & 1))
						interactionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;
					else if (mapElement->properties.path.addition_status == rideIndex)
						interactionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;
					break;
				case MAP_ELEMENT_TYPE_SCENERY:
					interactionType = VIEWPORT_INTERACTION_ITEM_SCENERY;
					break;
				case MAP_ELEMENT_TYPE_FENCE:
					interactionType = VIEWPORT_INTERACTION_ITEM_WALL;
					break;
				case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
					interactionType = VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY;
					break;
				}

				if (interactionType != VIEWPORT_INTERACTION_ITEM_NONE) {
					if (!track_design_save_contains_map_element(mapElement)) {
						track_design_save_add_map_element(interactionType, x * 32, y * 32, mapElement);
					}
				}
			} while (!map_element_is_last_for_tile(mapElement++));
		}
	}
}

/* Based on rct2: 0x006D2897 */
static bool track_design_save_copy_scenery_to_track(uint8 **track_pointer)
{
	rct_td6_scenery_element *track_scenery = (rct_td6_scenery_element*)(*track_pointer - 1);
	rct_td6_scenery_element* scenery_source = RCT2_ADDRESS(0x009DA193, rct_td6_scenery_element);

	while (true) {
		memcpy(track_scenery, scenery_source, sizeof(rct_td6_scenery_element));
		if ((track_scenery->scenery_object.flags & 0xFF) == 0xFF) break;

		//0x00F4414D is direction of track?
		if ((track_scenery->scenery_object.flags & 0xF) == OBJECT_TYPE_PATHS){

			uint8 slope = (track_scenery->flags & 0x60) >> 5;
			slope -= RCT2_GLOBAL(0x00F4414D, uint8);

			track_scenery->flags &= 0x9F;
			track_scenery->flags |= ((slope & 3) << 5);

			// Direction of connection on path
			uint8 direction = track_scenery->flags & 0xF;
			// Rotate the direction by the track direction
			direction = ((direction << 4) >> RCT2_GLOBAL(0x00F4414D, uint8));

			track_scenery->flags &= 0xF0;
			track_scenery->flags |= (direction & 0xF) | (direction >> 4);

		} else if ((track_scenery->scenery_object.flags & 0xF) == OBJECT_TYPE_WALLS){
			uint8 direction = track_scenery->flags & 3;

			direction -= RCT2_GLOBAL(0x00F4414D, uint8);

			track_scenery->flags &= 0xFC;
			track_scenery->flags |= (direction & 3);
		} else {
			uint8 direction = track_scenery->flags & 3;
			uint8 quadrant = (track_scenery->flags & 0xC) >> 2;

			direction -= RCT2_GLOBAL(0x00F4414D, uint8);
			quadrant -= RCT2_GLOBAL(0x00F4414D, uint8);

			track_scenery->flags &= 0xF0;
			track_scenery->flags |= (direction & 3) | ((quadrant & 3) << 2);
		}

		sint16 x = ((uint8)track_scenery->x) * 32 - RCT2_GLOBAL(0x00F44142, sint16);
		sint16 y = ((uint8)track_scenery->y) * 32 - RCT2_GLOBAL(0x00F44144, sint16);
		rotate_map_coordinates(&x, &y, RCT2_GLOBAL(0x00F4414D, uint8));
		x /= 32;
		y /= 32;

		if (x > 127 || y > 127 || x < -126 || y < -126){
			window_error_open(3346, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY);
			return false;
		}

		track_scenery->x = (sint8)x;
		track_scenery->y = (sint8)y;

		int z = track_scenery->z * 8 - RCT2_GLOBAL(0xF44146, sint16);
		z /= 8;
		if (z > 127 || z < -126){
			window_error_open(3346, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY);
			return false;
		}

		track_scenery->z = z;

		track_scenery++;
		scenery_source++;
	}

	*track_pointer = (uint8*)track_scenery;
	//Skip end of scenery elements byte
	(*track_pointer)++;
	return true;
}

/**
 *
 *  rct2: 0x006CE44F
 */
static bool track_design_save_to_td6(rct_track_td6 *td6, uint8 rideIndex)
{
	rct_ride *ride = get_ride(rideIndex);
	td6->type = ride->type;
	rct_object_entry_extended *object = &object_entry_groups[OBJECT_TYPE_RIDE].entries[ride->subtype];

	// Note we are only copying rct_object_entry in size and
	// not the extended as we don't need the chunk size.
	memcpy(&td6->vehicle_object, object, sizeof(rct_object_entry));

	td6->ride_mode = ride->mode;

	td6->version_and_colour_scheme =
		(ride->colour_scheme_type & 3) |
		(1 << 3); // Version .TD6

	for (int i = 0; i < 32; i++) {
		td6->vehicle_colours[i] = ride->vehicle_colours[i];
		td6->vehicle_additional_colour[i] = ride->vehicle_colours_extended[i];
	}

	for (int i = 0; i < 4; i++) {
		td6->track_spine_colour[i] = ride->track_colour_main[i];
		td6->track_rail_colour[i] = ride->track_colour_additional[i];
		td6->track_support_colour[i] = ride->track_colour_supports[i];
	}

	td6->depart_flags = ride->depart_flags;
	td6->number_of_trains = ride->num_vehicles;
	td6->number_of_cars_per_train = ride->num_cars_per_train;
	td6->min_waiting_time = ride->min_waiting_time;
	td6->max_waiting_time = ride->max_waiting_time;
	td6->var_50 = ride->operation_option;
	td6->lift_hill_speed_num_circuits =
		ride->lift_hill_speed |
		(ride->num_circuits << 5);

	td6->entrance_style = ride->entrance_style;
	td6->max_speed = (sint8)(ride->max_speed / 65536);
	td6->average_speed = (sint8)(ride->average_speed / 65536);
	td6->ride_length = ride_get_total_length(ride) / 65536;
	td6->max_positive_vertical_g = ride->max_positive_vertical_g / 32;
	td6->max_negative_vertical_g = ride->max_negative_vertical_g / 32;
	td6->max_lateral_g = ride->max_lateral_g / 32;
	td6->inversions = ride->inversions;
	td6->drops = ride->drops;
	td6->highest_drop_height = ride->highest_drop_height;

	uint16 total_air_time = (ride->total_air_time * 123) / 1024;
	if (total_air_time > 255) {
		total_air_time = 0;
	}
	td6->total_air_time = (uint8)total_air_time;

	td6->excitement = ride->ratings.excitement / 10;
	td6->intensity = ride->ratings.intensity / 10;
	td6->nausea = ride->ratings.nausea / 10;

	td6->upkeep_cost = ride->upkeep_cost;
	td6->flags = 0;
	td6->flags2 = 0;

	uint8 *trackElements = td6->elements;
	memset(trackElements, 0, 8000);

	if (td6->type == RIDE_TYPE_MAZE) {
		return track_design_save_to_td6_for_maze(rideIndex, td6, trackElements);
	} else {
		return track_design_save_to_td6_for_tracked_ride(rideIndex, td6, trackElements);
	}
}

/**
 *
 *  rct2: 0x006CEAAE
 */
static bool track_design_save_to_td6_for_maze(uint8 rideIndex, rct_track_td6 *td6, uint8 *trackElements)
{
	rct_map_element *mapElement = NULL;
	bool mapFound = false;
	sint16 startX, startY;
	for (startY = 0; startY < 8192; startY += 32) {
		for (startX = 0; startX < 8192; startX += 32) {
			mapElement = map_get_first_element_at(startX >> 5, startY >> 5);
			do {
				if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
					continue;
				if (mapElement->properties.track.ride_index == rideIndex){
					mapFound = true;
					break;
				}
			} while (!map_element_is_last_for_tile(mapElement++));
			if (mapFound) {
				break;
			}
		}
		if (mapFound) {
			break;
		}
	}

	if (mapFound == 0) {
		gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
		return false;
	}

	RCT2_GLOBAL(0x00F44142, sint16) = startX;
	RCT2_GLOBAL(0x00F44144, sint16) = startY;
	RCT2_GLOBAL(0x00F44146, sint16) = mapElement->base_height * 8;

	rct_td6_maze_element *maze = (rct_td6_maze_element*)trackElements;

	// x is defined here as we can start the search
	// on tile start_x, start_y but then the next row
	// must restart on 0
	for (sint16 y = startY, x = startX; y < 8192; y += 32) {
		for (; x < 8192; x += 32) {
			mapElement = map_get_first_element_at(x / 32, y / 32);
			do {
				if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;
				if (mapElement->properties.track.ride_index != rideIndex) continue;

				maze->maze_entry = mapElement->properties.track.maze_entry;
				maze->x = (x - startX) / 32;
				maze->y = (y - startY) / 32;
				maze++;

				if (maze >= RCT2_ADDRESS(0x009DA151, rct_td6_maze_element)) {
					gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
					return false;
				}
			} while (!map_element_is_last_for_tile(mapElement++));

		}
		x = 0;
	}

	rct_ride *ride = get_ride(rideIndex);
	uint16 location = ride->entrances[0];

	if (location == 0xFFFF) {
		gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
		return false;
	}

	sint16 x = (location & 0xFF) * 32;
	sint16 y = ((location & 0xFF00) >> 8) * 32;

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE) continue;
		if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE) continue;
		if (mapElement->properties.entrance.ride_index == rideIndex) break;
	} while (!map_element_is_last_for_tile(mapElement++));
	// Add something that stops this from walking off the end

	uint8 entrance_direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
	maze->unk_2 = entrance_direction;
	maze->type = 8;
	maze->x = (sint8)((x - startX) / 32);
	maze->y = (sint8)((y - startY) / 32);
	maze++;

	location = ride->exits[0];
	if (location == 0xFFFF) {
		gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
		return 0;
	}

	x = (location & 0xFF) * 32;
	y = ((location & 0xFF00) >> 8) * 32;
	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE) continue;
		if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_EXIT) continue;
		if (mapElement->properties.entrance.ride_index == rideIndex) break;
	} while (!map_element_is_last_for_tile(mapElement++));
	// Add something that stops this from walking off the end

	uint8 exit_direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
	maze->unk_2 = exit_direction;
	maze->type = 0x80;
	maze->x = (sint8)((x - startX) / 32);
	maze->y = (sint8)((y - startY) / 32);
	maze++;
	maze->all = 0;
	maze++;

	trackElements = (uint8*)maze;
	*trackElements++ = 0xFF;

	RCT2_GLOBAL(0x00F44058, uint8*) = trackElements;

	// Save global vars as they are still used by scenery
	sint16 start_z = RCT2_GLOBAL(0x00F44146, sint16);
	sub_6D01B3(td6, PTD_OPERATION_DRAW_OUTLINES, 0, 4096, 4096, 0);
	RCT2_GLOBAL(0x00F44142, sint16) = startX;
	RCT2_GLOBAL(0x00F44144, sint16) = startY;
	RCT2_GLOBAL(0x00F44146, sint16) = start_z;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, sint16) &= 0xFFF9;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, sint16) &= 0xFFF7;

	td6->space_required_x = ((gTrackPreviewMax.x - gTrackPreviewMin.x) / 32) + 1;
	td6->space_required_y = ((gTrackPreviewMax.y - gTrackPreviewMin.y) / 32) + 1;
	return true;
}

/**
 *
 *  rct2: 0x006CE68D
 */
static bool track_design_save_to_td6_for_tracked_ride(uint8 rideIndex, rct_track_td6 *td6, uint8 *trackElements)
{
	rct_ride *ride = get_ride(rideIndex);
	rct_xy_element trackElement;
	track_begin_end trackBeginEnd;

	if (!ride_try_get_origin_element(rideIndex, &trackElement)) {
		gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
		return false;
	}

	int z = 0;
	// Find the start of the track.
	// It has to do this as a backwards loop incase this is an incomplete track.
	if (track_block_get_previous(trackElement.x, trackElement.y, trackElement.element, &trackBeginEnd)) {
		rct_map_element* initial_map = trackElement.element;
		do {
			rct_xy_element lastGood = {
				.element = trackBeginEnd.begin_element,
				.x = trackBeginEnd.begin_x,
				.y = trackBeginEnd.begin_y
			};

			if (!track_block_get_previous(trackBeginEnd.end_x, trackBeginEnd.end_y, trackBeginEnd.begin_element, &trackBeginEnd)) {
				trackElement = lastGood;
				break;
			}
		} while (initial_map != trackBeginEnd.begin_element);
	}

	z = trackElement.element->base_height * 8;
	uint8 track_type = trackElement.element->properties.track.type;
	uint8 direction = trackElement.element->type & MAP_ELEMENT_DIRECTION_MASK;
	RCT2_GLOBAL(0x00F4414D, uint8) = direction;

	if (sub_6C683D(&trackElement.x, &trackElement.y, &z, direction, track_type, 0, &trackElement.element, 0)) {
		gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
		return 0;
	}

	const rct_track_coordinates *trackCoordinates = &TrackCoordinates[trackElement.element->properties.track.type];
	// Used in the following loop to know when we have
	// completed all of the elements and are back at the
	// start.
	rct_map_element *initialMap = trackElement.element;

	sint16 start_x = trackElement.x;
	sint16 start_y = trackElement.y;
	sint16 start_z = z + trackCoordinates->z_begin;
	RCT2_GLOBAL(0x00F44142, sint16) = start_x;
	RCT2_GLOBAL(0x00F44144, sint16) = start_y;
	RCT2_GLOBAL(0x00F44146, sint16) = start_z;

	rct_td6_track_element *track = (rct_td6_track_element*)trackElements;
	do{
		track->type = trackElement.element->properties.track.type;
		if (track->type == 0xFF) {
			track->type = 101;
		}

		if (track->type == TRACK_ELEM_LEFT_VERTICAL_LOOP ||
			track->type == TRACK_ELEM_RIGHT_VERTICAL_LOOP
		) {
			td6->flags |= (1 << 7);
		}

		if (track->type == TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP ||
			track->type == TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP ||
			track->type == TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN ||
			track->type == TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN
		) {
			td6->flags |= (1 << 17);
		}

		if (track->type == TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN ||
			track->type == TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN ||
			track->type == TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP ||
			track->type == TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP
		) {
			td6->flags |= (1 << 29);
		}

		if (track->type == TRACK_ELEM_HALF_LOOP_UP ||
			track->type == TRACK_ELEM_HALF_LOOP_DOWN
		) {
			td6->flags |= (1 << 18);
		}

		if (track->type == TRACK_ELEM_LEFT_CORKSCREW_UP ||
			track->type == TRACK_ELEM_RIGHT_CORKSCREW_UP ||
			track->type == TRACK_ELEM_LEFT_CORKSCREW_DOWN ||
			track->type == TRACK_ELEM_RIGHT_CORKSCREW_DOWN
		) {
			td6->flags |= (1 << 19);
		}

		if (track->type == TRACK_ELEM_WATER_SPLASH) {
			td6->flags |= (1 << 27);
		}

		if (track->type == TRACK_ELEM_POWERED_LIFT) {
			td6->flags |= (1 << 30);
		}

		if (track->type == TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP ||
			track->type == TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP ||
			track->type == TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN ||
			track->type == TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN
		) {
			td6->flags |= (1 << 31);
		}

		if (track->type == TRACK_ELEM_LOG_FLUME_REVERSER) {
			td6->flags2 |= TRACK_FLAGS2_CONTAINS_LOG_FLUME_REVERSER;
		}

		uint8 bh;
		if (track->type == TRACK_ELEM_BRAKES) {
			bh = trackElement.element->properties.track.sequence >> 4;
		} else {
			bh = trackElement.element->properties.track.colour >> 4;
		}

		uint8 flags = (trackElement.element->type & (1 << 7)) | bh;
		flags |= (trackElement.element->properties.track.colour & 3) << 4;
		if (
			RideData4[ride->type].flags & RIDE_TYPE_FLAG4_3 &&
			trackElement.element->properties.track.colour & (1 << 2)
		) {
			flags |= (1 << 6);
		}

		track->flags = flags;
		track++;

		if (!track_block_get_next(&trackElement, &trackElement, NULL, NULL)) {
			break;
		}

		z = trackElement.element->base_height * 8;
		direction = trackElement.element->type & MAP_ELEMENT_DIRECTION_MASK;
		track_type = trackElement.element->properties.track.type;

		if (sub_6C683D(&trackElement.x, &trackElement.y, &z, direction, track_type, 0, &trackElement.element, 0)) {
			break;
		}
	} while (trackElement.element != initialMap);

	trackElements = (uint8*)track;
	// Mark the elements as finished.
	*trackElements++ = 0xFF;

	rct_td6_entrance_element *entrance = (rct_td6_entrance_element*)trackElements;

	// First entrances, second exits
	for (int i = 0; i < 2; i++) {
		for (int station_index = 0; station_index < 4; station_index++) {
			z = ride->station_heights[station_index];

			uint16 location;
			if (i == 0) {
				location = ride->entrances[station_index];
			} else {
				location = ride->exits[station_index];
			}

			if (location == 0xFFFF) {
				continue;
			}

			sint16 x = (location & 0xFF) * 32;
			sint16 y = ((location & 0xFF00) >> 8) * 32;

			rct_map_element *map_element = map_get_first_element_at(x >> 5, y >> 5);
			do {
				if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_ENTRANCE) continue;
				if (map_element->base_height == z) break;
			} while (!map_element_is_last_for_tile(map_element++));
			// Add something that stops this from walking off the end

			uint8 entrance_direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
			entrance_direction -= RCT2_GLOBAL(0x00F4414D, uint8);
			entrance_direction &= MAP_ELEMENT_DIRECTION_MASK;
			entrance->direction = entrance_direction;

			x -= RCT2_GLOBAL(0x00F44142, sint16);
			y -= RCT2_GLOBAL(0x00F44144, sint16);

			rotate_map_coordinates(&x, &y, RCT2_GLOBAL(0x00F4414D, uint8));
			entrance->x = x;
			entrance->y = y;

			z *= 8;
			z -= RCT2_GLOBAL(0x00F44146, sint16);
			z /= 8;

			if (z > 127 || z < -126) {
				gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
				return 0;
			}

			if (z == 0xFF) {
				z = 0x80;
			}

			entrance->z = z;

			// If this is the exit version
			if (i == 1) {
				entrance->direction |= (1 << 7);
			}
			entrance++;
		}
	}

	trackElements = (uint8*)entrance;
	*trackElements++ = 0xFF;
	*trackElements++ = 0xFF;

	RCT2_GLOBAL(0x00F44058, uint8*) = trackElements;

	sub_6D01B3(td6, PTD_OPERATION_DRAW_OUTLINES, 0, 4096, 4096, 0);

	// Resave global vars for scenery reasons.
	RCT2_GLOBAL(0x00F44142, sint16) = start_x;
	RCT2_GLOBAL(0x00F44144, sint16) = start_y;
	RCT2_GLOBAL(0x00F44146, sint16) = start_z;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, sint16) &= 0xFFF9;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, sint16) &= 0xFFF7;

	td6->space_required_x = ((gTrackPreviewMax.x - gTrackPreviewMin.x) / 32) + 1;
	td6->space_required_y = ((gTrackPreviewMax.y - gTrackPreviewMin.y) / 32) + 1;
	return true;
}

/**
 *
 *  rct2: 0x006771DC but not really its branched from that
 *  quite far.
 */
static bool track_design_save_to_file(rct_track_td6 *track_design, utf8 *path)
{
	window_close_construction_windows();

	// Encode TD6 data
	uint8 *encodedData = malloc(0x8000);
	int encodedDataLength = sawyercoding_encode_td6((uint8 *)track_design, encodedData, 0x609F);

	// Save encoded TD6 data to file
	bool result;
	log_verbose("saving track %s", path);
	SDL_RWops *file = SDL_RWFromFile(path, "wb");
	if (file != NULL) {
		SDL_RWwrite(file, encodedData, encodedDataLength, 1);
		SDL_RWclose(file);
		result = true;
	} else {
		log_error("Failed to save %s", path);
		result = false;
	}

	free(encodedData);
	return result;
}
