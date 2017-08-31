#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../audio/audio.h"
#include "../config/Config.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../localisation/string_ids.h"
#include "../interface/viewport.h"
#include "../util/sawyercoding.h"
#include "../util/util.h"
#include "../windows/error.h"
#include "../world/scenery.h"
#include "ride_data.h"
#include "track.h"
#include "track_data.h"
#include "TrackDesign.h"
#include "TrackDesignRepository.h"

#define TRACK_MAX_SAVED_MAP_ELEMENTS 1500
#define TRACK_NEARBY_SCENERY_DISTANCE 1

bool gTrackDesignSaveMode = false;
uint8 gTrackDesignSaveRideIndex = 255;

static size_t _trackSavedMapElementsCount;
#ifdef NO_RCT2
    static rct_map_element *_trackSavedMapElements[TRACK_MAX_SAVED_MAP_ELEMENTS];
#else
    static rct_map_element **_trackSavedMapElements = (rct_map_element**)0x00F63674;
#endif

static size_t _trackSavedMapElementsDescCount;
static rct_td6_scenery_element _trackSavedMapElementsDesc[TRACK_MAX_SAVED_MAP_ELEMENTS];

static rct_track_td6 *_trackDesign;
static uint8 _trackSaveDirection;

static bool track_design_save_should_select_scenery_around(sint32 rideIndex, rct_map_element *mapElement);
static void track_design_save_select_nearby_scenery_for_tile(sint32 rideIndex, sint32 cx, sint32 cy);
static bool track_design_save_add_map_element(sint32 interactionType, sint32 x, sint32 y, rct_map_element *mapElement);
static void track_design_save_remove_map_element(sint32 interactionType, sint32 x, sint32 y, rct_map_element *mapElement);
static bool track_design_save_copy_scenery_to_td6(rct_track_td6 *td6);
static rct_track_td6 *track_design_save_to_td6(uint8 rideIndex);
static bool track_design_save_to_td6_for_maze(uint8 rideIndex, rct_track_td6 *td6);
static bool track_design_save_to_td6_for_tracked_ride(uint8 rideIndex, rct_track_td6 *td6);

void track_design_save_init()
{
    _trackSavedMapElementsCount = 0;
#ifdef NO_RCT2
    memset(_trackSavedMapElements, 0, sizeof(_trackSavedMapElements));
#else
    memset(_trackSavedMapElements, 0, sizeof(rct_map_element*) * TRACK_MAX_SAVED_MAP_ELEMENTS);
    _trackSavedMapElements[0] = (rct_map_element*)-1;
#endif

    _trackSavedMapElementsDescCount = 0;
    memset(_trackSavedMapElementsDesc, 0xFF, sizeof(_trackSavedMapElementsDesc));
}

/**
 *
 *  rct2: 0x006D2B07
 */
void track_design_save_select_map_element(sint32 interactionType, sint32 x, sint32 y, rct_map_element *mapElement, bool collect)
{
    if (track_design_save_contains_map_element(mapElement)) {
        if (!collect) {
            track_design_save_remove_map_element(interactionType, x, y, mapElement);
        }
    } else {
        if (collect) {
            if (!track_design_save_add_map_element(interactionType, x, y, mapElement)) {
                window_error_open(
                    STR_SAVE_TRACK_SCENERY_UNABLE_TO_SELECT_ADDITIONAL_ITEM_OF_SCENERY,
                    STR_SAVE_TRACK_SCENERY_TOO_MANY_ITEMS_SELECTED
                );
            }
        }
    }
}

/**
 *
 *  rct2: 0x006D303D
 */
void track_design_save_select_nearby_scenery(sint32 rideIndex)
{
    rct_map_element *mapElement;

    for (sint32 y = 0; y < 256; y++) {
        for (sint32 x = 0; x < 256; x++) {
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
    track_design_save_init();
    gfx_invalidate_screen();
}

static void track_design_save_callback(sint32 result, const utf8 * path)
{
    free(_trackDesign->track_elements);
    free(_trackDesign->entrance_elements);
    free(_trackDesign->scenery_elements);
    free(_trackDesign);

    if (result == MODAL_RESULT_OK) {
        track_repository_scan();
    }
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

    if (!ride_has_ratings(ride)) {
        window_error_open(STR_CANT_SAVE_TRACK_DESIGN, gGameCommandErrorText);
        return false;
    }

    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK)) {
        window_error_open(STR_CANT_SAVE_TRACK_DESIGN, gGameCommandErrorText);
        return false;
    }

    _trackDesign = track_design_save_to_td6(rideIndex);
    if (_trackDesign == NULL) {
        window_error_open(STR_CANT_SAVE_TRACK_DESIGN, gGameCommandErrorText);
        return false;
    }

    if (gTrackDesignSaveMode) {
        if (!track_design_save_copy_scenery_to_td6(_trackDesign)) {
            free(_trackDesign->track_elements);
            free(_trackDesign->entrance_elements);
            free(_trackDesign);
            return false;
        }
    }

    utf8 track_name[256];
    format_string(track_name, sizeof(track_name), ride->name, &ride->name_arguments);

    window_loadsave_open(LOADSAVETYPE_TRACK | LOADSAVETYPE_SAVE, track_name);
    window_loadsave_set_loadsave_callback(track_design_save_callback);

    return true;
}

bool track_design_save_contains_map_element(rct_map_element *mapElement)
{
    for (size_t i = 0; i < _trackSavedMapElementsCount; i++) {
        if (_trackSavedMapElements[i] == mapElement) {
            return true;
        }
    }
    return false;
}

static sint32 map_element_get_total_element_count(rct_map_element *mapElement)
{
    sint32 elementCount;
    rct_scenery_entry *sceneryEntry;
    rct_large_scenery_tile *tile;

    switch (map_element_get_type(mapElement)) {
    case MAP_ELEMENT_TYPE_PATH:
    case MAP_ELEMENT_TYPE_SCENERY:
    case MAP_ELEMENT_TYPE_WALL:
        return 1;

    case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
        sceneryEntry = get_large_scenery_entry(mapElement->properties.scenerymultiple.type & 0x3FF);
        tile = sceneryEntry->large_scenery.tiles;
        elementCount = 0;
        do {
            tile++;
            elementCount++;
        } while (tile->x_offset != (sint16)(uint16)0xFFFF);
        return elementCount;

    default:
        return 0;
    }
}

/**
 *
 *  rct2: 0x006D2ED2
 */
static bool track_design_save_can_add_map_element(rct_map_element *mapElement)
{
    size_t newElementCount = map_element_get_total_element_count(mapElement);
    if (newElementCount == 0) {
        return false;
    }

    // Get number of spare elements left
    size_t spareSavedElements = TRACK_MAX_SAVED_MAP_ELEMENTS - _trackSavedMapElementsCount;
    if (newElementCount > spareSavedElements) {
        // No more spare saved elements left
        return false;
    }

    return true;
}

/**
 *
 *  rct2: 0x006D2F4C
 */
static void track_design_save_push_map_element(sint32 x, sint32 y, rct_map_element *mapElement)
{
    if (_trackSavedMapElementsCount < TRACK_MAX_SAVED_MAP_ELEMENTS) {
        _trackSavedMapElements[_trackSavedMapElementsCount++] = mapElement;
        map_invalidate_tile_full(x, y);

#ifndef NO_RCT2
        // Required as drawing still uses 0xFFFFFFFF as a list terminator
        _trackSavedMapElements[_trackSavedMapElementsCount] = (rct_map_element*)-1;
#endif
    }
}

/**
 *
 *  rct2: 0x006D2FA7
 */
static void track_design_save_push_map_element_desc(rct_object_entry *entry, sint32 x, sint32 y, sint32 z, uint8 flags, uint8 primaryColour, uint8 secondaryColour)
{
    rct_td6_scenery_element *item = &_trackSavedMapElementsDesc[_trackSavedMapElementsDescCount++];
    item->scenery_object = *entry;
    item->x = x / 32;
    item->y = y / 32;
    item->z = z;
    item->flags = flags;
    item->primary_colour = primaryColour;
    item->secondary_colour = secondaryColour;
}

static void track_design_save_add_scenery(sint32 x, sint32 y, rct_map_element *mapElement)
{
    sint32 entryType = mapElement->properties.scenery.type;
    rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_SMALL_SCENERY].entries[entryType];

    uint8 flags = 0;
    flags |= mapElement->type & 3;
    flags |= (mapElement->type & 0xC0) >> 4;

    uint8 primaryColour = scenery_small_get_primary_colour(mapElement);
    uint8 secondaryColour = scenery_small_get_secondary_colour(mapElement);

    track_design_save_push_map_element(x, y, mapElement);
    track_design_save_push_map_element_desc(entry, x, y, mapElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_add_large_scenery(sint32 x, sint32 y, rct_map_element *mapElement)
{
    rct_large_scenery_tile *sceneryTiles, *tile;
    sint32 x0, y0, z0, z;
    sint32 direction, sequence;

    sint32 entryType = mapElement->properties.scenerymultiple.type & 0x3FF;
    rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_LARGE_SCENERY].entries[entryType];
    sceneryTiles = get_large_scenery_entry(entryType)->large_scenery.tiles;

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

static void track_design_save_add_wall(sint32 x, sint32 y, rct_map_element *mapElement)
{
    sint32 entryType = mapElement->properties.wall.type;
    rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_WALLS].entries[entryType];

    uint8 flags = 0;
    flags |= mapElement->type & 3;
    flags |= mapElement->properties.wall.colour_3 << 2;

    uint8 secondaryColour = wall_element_get_secondary_colour(mapElement);
    uint8 primaryColour = mapElement->properties.wall.colour_1 & 0x1F;

    track_design_save_push_map_element(x, y, mapElement);
    track_design_save_push_map_element_desc(entry, x, y, mapElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_add_footpath(sint32 x, sint32 y, rct_map_element *mapElement)
{
    sint32 entryType = mapElement->properties.path.type >> 4;
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
static bool track_design_save_add_map_element(sint32 interactionType, sint32 x, sint32 y, rct_map_element *mapElement)
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
static void track_design_save_pop_map_element(sint32 x, sint32 y, rct_map_element *mapElement)
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
static void track_design_save_pop_map_element_desc(rct_object_entry *entry, sint32 x, sint32 y, sint32 z, uint8 flags, uint8 primaryColour, uint8 secondaryColour)
{
    size_t removeIndex = SIZE_MAX;
    for (size_t i = 0; i < _trackSavedMapElementsDescCount; i++) {
        rct_td6_scenery_element *item = &_trackSavedMapElementsDesc[i];
        if (item->x != x / 32) continue;
        if (item->y != y / 32) continue;
        if (item->z != z) continue;
        if (item->flags != flags) continue;
        if (!object_entry_compare(&item->scenery_object, entry)) continue;

        removeIndex = i;
    }

    if (removeIndex != SIZE_MAX) {
        size_t remainingNumItems = _trackSavedMapElementsDescCount - removeIndex - 1;
        if (remainingNumItems > 0) {
            memmove(
                &_trackSavedMapElementsDesc[removeIndex],
                &_trackSavedMapElementsDesc[removeIndex + 1],
                remainingNumItems * sizeof(rct_td6_scenery_element)
            );
        }
        _trackSavedMapElementsDescCount--;
    }
}

static void track_design_save_remove_scenery(sint32 x, sint32 y, rct_map_element *mapElement)
{
    sint32 entryType = mapElement->properties.scenery.type;
    rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_SMALL_SCENERY].entries[entryType];

    uint8 flags = 0;
    flags |= mapElement->type & 3;
    flags |= (mapElement->type & 0xC0) >> 4;

    uint8 primaryColour = scenery_small_get_primary_colour(mapElement);
    uint8 secondaryColour = scenery_small_get_secondary_colour(mapElement);

    track_design_save_pop_map_element(x, y, mapElement);
    track_design_save_pop_map_element_desc(entry, x, y, mapElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_remove_large_scenery(sint32 x, sint32 y, rct_map_element *mapElement)
{
    rct_large_scenery_tile *sceneryTiles, *tile;
    sint32 x0, y0, z0, z;
    sint32 direction, sequence;

    sint32 entryType = mapElement->properties.scenerymultiple.type & 0x3FF;
    rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_LARGE_SCENERY].entries[entryType];
    sceneryTiles = get_large_scenery_entry(entryType)->large_scenery.tiles;

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

static void track_design_save_remove_wall(sint32 x, sint32 y, rct_map_element *mapElement)
{
    sint32 entryType = mapElement->properties.wall.type;
    rct_object_entry *entry = (rct_object_entry*)&object_entry_groups[OBJECT_TYPE_WALLS].entries[entryType];

    uint8 flags = 0;
    flags |= mapElement->type & 3;
    flags |= mapElement->properties.wall.colour_3 << 2;

    uint8 secondaryColour = wall_element_get_secondary_colour(mapElement);
    uint8 primaryColour = mapElement->properties.wall.colour_1 & 0x1F;

    track_design_save_pop_map_element(x, y, mapElement);
    track_design_save_pop_map_element_desc(entry, x, y, mapElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_remove_footpath(sint32 x, sint32 y, rct_map_element *mapElement)
{
    sint32 entryType = mapElement->properties.path.type >> 4;
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
static void track_design_save_remove_map_element(sint32 interactionType, sint32 x, sint32 y, rct_map_element *mapElement)
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

static bool track_design_save_should_select_scenery_around(sint32 rideIndex, rct_map_element *mapElement)
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

static void track_design_save_select_nearby_scenery_for_tile(sint32 rideIndex, sint32 cx, sint32 cy)
{
    rct_map_element *mapElement;

    for (sint32 y = cy - TRACK_NEARBY_SCENERY_DISTANCE; y <= cy + TRACK_NEARBY_SCENERY_DISTANCE; y++) {
        for (sint32 x = cx - TRACK_NEARBY_SCENERY_DISTANCE; x <= cx + TRACK_NEARBY_SCENERY_DISTANCE; x++) {
            mapElement = map_get_first_element_at(x, y);
            do {
                sint32 interactionType = VIEWPORT_INTERACTION_ITEM_NONE;
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
                case MAP_ELEMENT_TYPE_WALL:
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
static bool track_design_save_copy_scenery_to_td6(rct_track_td6 *td6)
{
    // Copy TD6 scenery elements to new memory and add end marker
    size_t totalSceneryElementsSize = _trackSavedMapElementsDescCount * sizeof(rct_td6_scenery_element);
    td6->scenery_elements = malloc(totalSceneryElementsSize + 1);
    memcpy(td6->scenery_elements, _trackSavedMapElementsDesc, totalSceneryElementsSize);
    *((uint8*)&td6->scenery_elements[_trackSavedMapElementsDescCount]) = 0xFF;

    // Run an element loop
    for (size_t i = 0; i < _trackSavedMapElementsDescCount; i++) {
        rct_td6_scenery_element *scenery = &td6->scenery_elements[i];

        switch (scenery->scenery_object.flags & 0x0F) {
        case OBJECT_TYPE_PATHS:
        {
            uint8 slope = (scenery->flags & 0x60) >> 5;
            slope -= _trackSaveDirection;

            scenery->flags &= 0x9F;
            scenery->flags |= ((slope & 3) << 5);

            // Direction of connection on path
            uint8 direction = scenery->flags & 0xF;
            // Rotate the direction by the track direction
            direction = ((direction << 4) >> _trackSaveDirection);

            scenery->flags &= 0xF0;
            scenery->flags |= (direction & 0xF) | (direction >> 4);
            break;
        }
        case OBJECT_TYPE_WALLS:
        {
            uint8 direction = scenery->flags & 3;
            direction -= _trackSaveDirection;

            scenery->flags &= 0xFC;
            scenery->flags |= (direction & 3);
            break;
        }
        default:
        {
            uint8 direction = scenery->flags & 3;
            uint8 quadrant = (scenery->flags & 0x0C) >> 2;

            direction -= _trackSaveDirection;
            quadrant -= _trackSaveDirection;

            scenery->flags &= 0xF0;
            scenery->flags |= (direction & 3) | ((quadrant & 3) << 2);
            break;
        }
        }

        sint16 x = ((uint8)scenery->x) * 32 - gTrackPreviewOrigin.x;
        sint16 y = ((uint8)scenery->y) * 32 - gTrackPreviewOrigin.y;
        rotate_map_coordinates(&x, &y, (0 - _trackSaveDirection) & 3);
        x /= 32;
        y /= 32;

        if (x > 127 || y > 127 || x < -126 || y < -126){
            window_error_open(STR_CANT_SAVE_TRACK_DESIGN, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY);
            SafeFree(td6->scenery_elements);
            return false;
        }

        scenery->x = (sint8)x;
        scenery->y = (sint8)y;

        sint32 z = scenery->z * 8 - gTrackPreviewOrigin.z;
        z /= 8;
        if (z > 127 || z < -126) {
            window_error_open(STR_CANT_SAVE_TRACK_DESIGN, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY);
            SafeFree(td6->scenery_elements);
            return false;
        }
        scenery->z = z;
    }

    return true;
}

/**
 *
 *  rct2: 0x006CE44F
 */
static rct_track_td6 *track_design_save_to_td6(uint8 rideIndex)
{
    rct_track_td6 *td6 = calloc(1, sizeof(rct_track_td6));
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

    for (sint32 i = 0; i < 32; i++) {
        td6->vehicle_colours[i] = ride->vehicle_colours[i];
        td6->vehicle_additional_colour[i] = ride->vehicle_colours_extended[i];
    }

    for (sint32 i = 0; i < RCT12_NUM_COLOUR_SCHEMES; i++) {
        td6->track_spine_colour[i] = ride->track_colour_main[i];
        td6->track_rail_colour[i] = ride->track_colour_additional[i];
        td6->track_support_colour[i] = ride->track_colour_supports[i];
    }

    td6->depart_flags = ride->depart_flags;
    td6->number_of_trains = ride->num_vehicles;
    td6->number_of_cars_per_train = ride->num_cars_per_train;
    td6->min_waiting_time = ride->min_waiting_time;
    td6->max_waiting_time = ride->max_waiting_time;
    td6->operation_setting = ride->operation_option;
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

    bool result;
    if (td6->type == RIDE_TYPE_MAZE) {
        result = track_design_save_to_td6_for_maze(rideIndex, td6);
    } else {
        result = track_design_save_to_td6_for_tracked_ride(rideIndex, td6);
    }

    if (!result) {
        track_design_dispose(td6);
        td6 = NULL;
    }
    return td6;
}

/**
 *
 *  rct2: 0x006CEAAE
 */
static bool track_design_save_to_td6_for_maze(uint8 rideIndex, rct_track_td6 *td6)
{
    rct_map_element *mapElement = NULL;
    bool mapFound = false;
    sint16 startX = 0;
    sint16 startY = 0;
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

    gTrackPreviewOrigin = (rct_xyz16) { startX, startY, mapElement->base_height * 8 };

    size_t numMazeElements = 0;
    td6->maze_elements = calloc(8192, sizeof(rct_td6_maze_element));
    rct_td6_maze_element *maze = td6->maze_elements;

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
                numMazeElements++;

                if (numMazeElements >= 2000) {
                    gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
                    SafeFree(td6->maze_elements);
                    return false;
                }
            } while (!map_element_is_last_for_tile(mapElement++));

        }
        x = 0;
    }

    rct_ride *ride = get_ride(rideIndex);
    rct_xy8 location = ride->entrances[0];

    if (location.xy == RCT_XY8_UNDEFINED) {
        gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
        SafeFree(td6->maze_elements);
        return false;
    }

    sint16 x = location.x * 32;
    sint16 y = location.y * 32;

    mapElement = map_get_first_element_at(location.x, location.y);
    do {
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE) continue;
        if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE) continue;
        if (mapElement->properties.entrance.ride_index == rideIndex) break;
    } while (!map_element_is_last_for_tile(mapElement++));
    // Add something that stops this from walking off the end

    uint8 entrance_direction = map_element_get_direction(mapElement);
    maze->direction = entrance_direction;
    maze->type = 8;
    maze->x = (sint8)((x - startX) / 32);
    maze->y = (sint8)((y - startY) / 32);
    maze++;
    numMazeElements++;

    location = ride->exits[0];
    if (location.xy == RCT_XY8_UNDEFINED) {
        gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
        SafeFree(td6->maze_elements);
        return 0;
    }

    x = location.x * 32;
    y = location.y * 32;
    mapElement = map_get_first_element_at(location.x, location.y);
    do {
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE) continue;
        if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_EXIT) continue;
        if (mapElement->properties.entrance.ride_index == rideIndex) break;
    } while (!map_element_is_last_for_tile(mapElement++));
    // Add something that stops this from walking off the end

    uint8 exit_direction = map_element_get_direction(mapElement);
    maze->direction = exit_direction;
    maze->type = 0x80;
    maze->x = (sint8)((x - startX) / 32);
    maze->y = (sint8)((y - startY) / 32);
    maze++;
    maze->all = 0;
    maze++;
    numMazeElements++;

    // Write end marker
    maze->all = 0;
    maze++;
    numMazeElements++;

    // Trim memory
    td6->maze_elements = realloc(td6->maze_elements, numMazeElements * sizeof(rct_td6_maze_element));

    // Save global vars as they are still used by scenery
    sint16 startZ = gTrackPreviewOrigin.z;
    place_virtual_track(td6, PTD_OPERATION_DRAW_OUTLINES, true, 0, 4096, 4096, 0);
    gTrackPreviewOrigin = (rct_xyz16) { startX, startY, startZ };

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;

    td6->space_required_x = ((gTrackPreviewMax.x - gTrackPreviewMin.x) / 32) + 1;
    td6->space_required_y = ((gTrackPreviewMax.y - gTrackPreviewMin.y) / 32) + 1;
    return true;
}

/**
 *
 *  rct2: 0x006CE68D
 */
static bool track_design_save_to_td6_for_tracked_ride(uint8 rideIndex, rct_track_td6 *td6)
{
    rct_ride *ride = get_ride(rideIndex);
    rct_xy_element trackElement;
    track_begin_end trackBeginEnd;

    if (!ride_try_get_origin_element(rideIndex, &trackElement)) {
        gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
        return false;
    }

    sint32 z = 0;
    // Find the start of the track.
    // It has to do this as a backwards loop in case this is an incomplete track.
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
    uint8 direction = map_element_get_direction(trackElement.element);
    _trackSaveDirection = direction;

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
    gTrackPreviewOrigin = (rct_xyz16) { start_x, start_y, start_z };

    size_t numTrackElements = 0;
    td6->track_elements = calloc(8192, sizeof(rct_td6_track_element));
    rct_td6_track_element *track = td6->track_elements;
    do {
        track->type = trackElement.element->properties.track.type;
        if (track->type == TRACK_ELEM_255) {
            track->type = TRACK_ELEM_255_ALIAS;
        }

        uint8 bh;
        if (track_element_has_speed_setting(track->type)) {
            bh = map_element_get_brake_booster_speed(trackElement.element) >> 1;
        } else {
            bh = trackElement.element->properties.track.colour >> 4;
        }

        uint8 flags = (trackElement.element->type & (1 << 7)) | bh;
        flags |= (trackElement.element->properties.track.colour & 3) << 4;
        if (
            RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE &&
            trackElement.element->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED
        ) {
            flags |= TRACK_ELEMENT_FLAG_INVERTED;
        }

        track->flags = flags;
        track++;
        numTrackElements++;

        if (!track_block_get_next(&trackElement, &trackElement, NULL, NULL)) {
            break;
        }

        z = trackElement.element->base_height * 8;
        direction = map_element_get_direction(trackElement.element);
        track_type = trackElement.element->properties.track.type;

        if (sub_6C683D(&trackElement.x, &trackElement.y, &z, direction, track_type, 0, &trackElement.element, 0)) {
            break;
        }
    } while (trackElement.element != initialMap);
    td6->track_elements = realloc(td6->track_elements, numTrackElements * sizeof(rct_td6_track_element) + 1);
    *((uint8*)&td6->track_elements[numTrackElements]) = 0xFF;

    size_t numEntranceElements = 0;
    td6->entrance_elements = calloc(32, sizeof(rct_td6_entrance_element));
    rct_td6_entrance_element *entrance = td6->entrance_elements;

    // First entrances, second exits
    for (sint32 i = 0; i < 2; i++) {
        for (sint32 station_index = 0; station_index < RCT12_MAX_STATIONS_PER_RIDE; station_index++) {
            z = ride->station_heights[station_index];

            rct_xy8 location;
            if (i == 0) {
                location = ride->entrances[station_index];
            } else {
                location = ride->exits[station_index];
            }

            if (location.xy == RCT_XY8_UNDEFINED) {
                continue;
            }

            sint16 x = location.x * 32;
            sint16 y = location.y * 32;

            rct_map_element *map_element = map_get_first_element_at(x >> 5, y >> 5);
            do {
                if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_ENTRANCE) continue;
                if (map_element->base_height == z) break;
            } while (!map_element_is_last_for_tile(map_element++));
            // Add something that stops this from walking off the end

            uint8 entrance_direction = map_element_get_direction(map_element);
            entrance_direction -= _trackSaveDirection;
            entrance_direction &= MAP_ELEMENT_DIRECTION_MASK;
            entrance->direction = entrance_direction;

            x -= gTrackPreviewOrigin.x;
            y -= gTrackPreviewOrigin.y;

            // Rotate entrance coordinates backwards to the correct direction
            rotate_map_coordinates(&x, &y, (0 - _trackSaveDirection) & 3);
            entrance->x = x;
            entrance->y = y;

            z *= 8;
            z -= gTrackPreviewOrigin.z;
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
            numEntranceElements++;
        }
    }
    td6->entrance_elements = realloc(td6->entrance_elements, numEntranceElements * sizeof(rct_td6_entrance_element) + 1);
    *((uint8*)&td6->entrance_elements[numEntranceElements]) = 0xFF;

    place_virtual_track(td6, PTD_OPERATION_DRAW_OUTLINES, true, 0, 4096, 4096, 0);

    // Resave global vars for scenery reasons.
    gTrackPreviewOrigin = (rct_xyz16) { start_x, start_y, start_z };

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;

    td6->space_required_x = ((gTrackPreviewMax.x - gTrackPreviewMin.x) / 32) + 1;
    td6->space_required_y = ((gTrackPreviewMax.y - gTrackPreviewMin.y) / 32) + 1;
    return true;
}

static size_t track_design_get_maze_elements_count(rct_track_td6 *td6)
{
    size_t count = 0;
    rct_td6_maze_element *mazeElement = td6->maze_elements;
    while (mazeElement->all != 0) {
        count++;
        mazeElement++;
    }
    return count;
}

static size_t track_design_get_track_elements_count(rct_track_td6 *td6)
{
    size_t count = 0;
    rct_td6_track_element *trackElement = td6->track_elements;
    while (trackElement->type != 0xFF) {
        count++;
        trackElement++;
    }
    return count;
}

static size_t track_design_get_entrance_elements_count(rct_track_td6 *td6)
{
    size_t count = 0;
    rct_td6_entrance_element *entranceElement = td6->entrance_elements;
    while (entranceElement->z != -1) {
        count++;
        entranceElement++;
    }
    return count;
}

static size_t track_design_get_scenery_elements_count(rct_track_td6 *td6)
{
    size_t count = 0;
    rct_td6_scenery_element *sceneryElement = td6->scenery_elements;
    if (sceneryElement != NULL) {
        while (sceneryElement->scenery_object.end_flag != 0xFF) {
            count++;
            sceneryElement++;
        }
    }
    return count;
}

typedef struct auto_buffer {
    void *ptr;
    size_t length;
    size_t capacity;
} auto_buffer;

static void auto_buffer_write(auto_buffer *buffer, const void *src, size_t len)
{
    size_t remainingSpace = buffer->capacity - buffer->length;
    if (remainingSpace < len) {
        do {
            buffer->capacity = max(8, buffer->capacity * 2);
            remainingSpace = buffer->capacity - buffer->length;
        } while (remainingSpace < len);

        buffer->ptr = realloc(buffer->ptr, buffer->capacity);
    }
    memcpy((void*)((uintptr_t)buffer->ptr + buffer->length), src, len);
    buffer->length += len;
}

/**
 *
 *  rct2: 0x006771DC but not really its branched from that
 *  quite far.
 */
bool track_design_save_to_file(const utf8 *path)
{
    rct_track_td6 *td6 = _trackDesign;
    const rct_td6_maze_element EndMarkerForMaze = { 0 };
    const uint8 EndMarker = 0xFF;

    window_close_construction_windows();

    // Create TD6 data buffer
    auto_buffer td6Buffer = { 0 };
    auto_buffer_write(&td6Buffer, td6, 0xA3);
    if (td6->type == RIDE_TYPE_MAZE) {
        auto_buffer_write(&td6Buffer, td6->maze_elements, track_design_get_maze_elements_count(td6) * sizeof(rct_td6_maze_element));
        auto_buffer_write(&td6Buffer, &EndMarkerForMaze, sizeof(EndMarkerForMaze));
    } else {
        auto_buffer_write(&td6Buffer, td6->track_elements, track_design_get_track_elements_count(td6) * sizeof(rct_td6_track_element));
        auto_buffer_write(&td6Buffer, &EndMarker, sizeof(EndMarker));
        auto_buffer_write(&td6Buffer, td6->entrance_elements, track_design_get_entrance_elements_count(td6) * sizeof(rct_td6_entrance_element));
        auto_buffer_write(&td6Buffer, &EndMarker, sizeof(EndMarker));
    }
    auto_buffer_write(&td6Buffer, td6->scenery_elements, track_design_get_scenery_elements_count(td6) * sizeof(rct_td6_scenery_element));
    auto_buffer_write(&td6Buffer, &EndMarker, sizeof(EndMarker));

    // Encode TD6 data
    uint8 *encodedData = malloc(0x8000);
    assert(td6Buffer.ptr != NULL);
    size_t encodedDataLength = sawyercoding_encode_td6((uint8*)td6Buffer.ptr, encodedData, td6Buffer.length);

    // Save encoded TD6 data to file
    bool result;
    log_verbose("saving track %s", path);
    result = writeentirefile(path, encodedData, encodedDataLength);
    if (!result) {
        log_error("Failed to save %s", path);
    }

    free(encodedData);
    free(td6Buffer.ptr);
    return result;
}
