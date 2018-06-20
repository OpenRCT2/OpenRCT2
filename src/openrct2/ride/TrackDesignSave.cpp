/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../Context.h"
#include "../Game.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../interface/Viewport.h"
#include "../object/ObjectList.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Wall.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"
#include "TrackDesign.h"
#include "TrackDesignRepository.h"
#include "Station.h"


#define TRACK_MAX_SAVED_TILE_ELEMENTS 1500
#define TRACK_NEARBY_SCENERY_DISTANCE 1
#define TRACK_TD6_MAX_ELEMENTS 8192

bool gTrackDesignSaveMode = false;
uint8_t gTrackDesignSaveRideIndex = 255;

static size_t _trackSavedTileElementsCount;
static rct_tile_element *_trackSavedTileElements[TRACK_MAX_SAVED_TILE_ELEMENTS];

static size_t _trackSavedTileElementsDescCount;
static rct_td6_scenery_element _trackSavedTileElementsDesc[TRACK_MAX_SAVED_TILE_ELEMENTS];

static rct_track_td6 *_trackDesign;
static uint8_t _trackSaveDirection;

static bool track_design_save_should_select_scenery_around(int32_t rideIndex, rct_tile_element *tileElement);
static void track_design_save_select_nearby_scenery_for_tile(int32_t rideIndex, int32_t cx, int32_t cy);
static bool track_design_save_add_tile_element(int32_t interactionType, int32_t x, int32_t y, rct_tile_element *tileElement);
static void track_design_save_remove_tile_element(int32_t interactionType, int32_t x, int32_t y, rct_tile_element *tileElement);
static bool track_design_save_copy_scenery_to_td6(rct_track_td6 *td6);
static rct_track_td6 *track_design_save_to_td6(uint8_t rideIndex);
static bool track_design_save_to_td6_for_maze(uint8_t rideIndex, rct_track_td6 *td6);
static bool track_design_save_to_td6_for_tracked_ride(uint8_t rideIndex, rct_track_td6 *td6);

void track_design_save_init()
{
    _trackSavedTileElementsCount = 0;
    _trackSavedTileElementsDescCount = 0;

    memset(_trackSavedTileElements, 0, sizeof(_trackSavedTileElements));
    memset(_trackSavedTileElementsDesc, 0, sizeof(_trackSavedTileElementsDesc));
}

/**
 *
 *  rct2: 0x006D2B07
 */
void track_design_save_select_tile_element(int32_t interactionType, int32_t x, int32_t y, rct_tile_element *tileElement, bool collect)
{
    if (track_design_save_contains_tile_element(tileElement)) {
        if (!collect) {
            track_design_save_remove_tile_element(interactionType, x, y, tileElement);
        }
    } else {
        if (collect) {
            if (!track_design_save_add_tile_element(interactionType, x, y, tileElement)) {
                context_show_error(
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
void track_design_save_select_nearby_scenery(int32_t rideIndex)
{
    rct_tile_element *tileElement;

    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++) {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++) {
            tileElement = map_get_first_element_at(x, y);
            do {
                if (track_design_save_should_select_scenery_around(rideIndex, tileElement)) {
                    track_design_save_select_nearby_scenery_for_tile(rideIndex, x, y);
                    break;
                }
            } while (!(tileElement++)->IsLastForTile());
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

static void track_design_save_callback(int32_t result, [[maybe_unused]] const utf8 * path)
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
bool track_design_save(uint8_t rideIndex)
{
    Ride* ride = get_ride(rideIndex);

    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)){
        context_show_error(STR_CANT_SAVE_TRACK_DESIGN, gGameCommandErrorText);
        return false;
    }

    if (!ride_has_ratings(ride)) {
        context_show_error(STR_CANT_SAVE_TRACK_DESIGN, gGameCommandErrorText);
        return false;
    }

    _trackDesign = track_design_save_to_td6(rideIndex);
    if (_trackDesign == nullptr) {
        context_show_error(STR_CANT_SAVE_TRACK_DESIGN, gGameCommandErrorText);
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

    auto intent = Intent(WC_LOADSAVE);
    intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_TRACK);
    intent.putExtra(INTENT_EXTRA_PATH, std::string{track_name});
    intent.putExtra(INTENT_EXTRA_CALLBACK, (void *) track_design_save_callback);
    context_open_intent(&intent);

    return true;
}

bool track_design_save_contains_tile_element(const rct_tile_element * tileElement)
{
    for (size_t i = 0; i < _trackSavedTileElementsCount; i++) {
        if (_trackSavedTileElements[i] == tileElement) {
            return true;
        }
    }
    return false;
}

static int32_t tile_element_get_total_element_count(rct_tile_element *tileElement)
{
    int32_t elementCount;
    rct_scenery_entry *sceneryEntry;
    rct_large_scenery_tile *tile;

    switch (tileElement->GetType()) {
    case TILE_ELEMENT_TYPE_PATH:
    case TILE_ELEMENT_TYPE_SMALL_SCENERY:
    case TILE_ELEMENT_TYPE_WALL:
        return 1;

    case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        sceneryEntry = get_large_scenery_entry(scenery_large_get_type(tileElement));
        tile = sceneryEntry->large_scenery.tiles;
        elementCount = 0;
        do {
            tile++;
            elementCount++;
        } while (tile->x_offset != (int16_t)(uint16_t)0xFFFF);
        return elementCount;

    default:
        return 0;
    }
}

/**
 *
 *  rct2: 0x006D2ED2
 */
static bool track_design_save_can_add_tile_element(rct_tile_element *tileElement)
{
    size_t newElementCount = tile_element_get_total_element_count(tileElement);
    if (newElementCount == 0) {
        return false;
    }

    // Get number of spare elements left
    size_t spareSavedElements = TRACK_MAX_SAVED_TILE_ELEMENTS - _trackSavedTileElementsCount;
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
static void track_design_save_push_tile_element(int32_t x, int32_t y, rct_tile_element *tileElement)
{
    if (_trackSavedTileElementsCount < TRACK_MAX_SAVED_TILE_ELEMENTS) {
        _trackSavedTileElements[_trackSavedTileElementsCount++] = tileElement;
        map_invalidate_tile_full(x, y);
    }
}

/**
 *
 *  rct2: 0x006D2FA7
 */
static void track_design_save_push_tile_element_desc(const rct_object_entry * entry, int32_t x, int32_t y, int32_t z, uint8_t flags, uint8_t primaryColour, uint8_t secondaryColour)
{
    rct_td6_scenery_element *item = &_trackSavedTileElementsDesc[_trackSavedTileElementsDescCount++];
    item->scenery_object = *entry;
    item->x = x / 32;
    item->y = y / 32;
    item->z = z;
    item->flags = flags;
    item->primary_colour = primaryColour;
    item->secondary_colour = secondaryColour;
}

static void track_design_save_add_scenery(int32_t x, int32_t y, rct_tile_element *tileElement)
{
    int32_t entryType = tileElement->properties.scenery.type;
    auto entry = object_entry_get_entry(OBJECT_TYPE_SMALL_SCENERY, entryType);

    uint8_t flags = 0;
    flags |= tileElement->type & 3;
    flags |= (tileElement->type & 0xC0) >> 4;

    uint8_t primaryColour = scenery_small_get_primary_colour(tileElement);
    uint8_t secondaryColour = scenery_small_get_secondary_colour(tileElement);

    track_design_save_push_tile_element(x, y, tileElement);
    track_design_save_push_tile_element_desc(entry, x, y, tileElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_add_large_scenery(int32_t x, int32_t y, rct_tile_element *tileElement)
{
    rct_large_scenery_tile *sceneryTiles, *tile;
    int32_t x0, y0, z0, z;
    int32_t direction, sequence;

    int32_t entryType = scenery_large_get_type(tileElement);
    auto entry = object_entry_get_entry(OBJECT_TYPE_LARGE_SCENERY, entryType);
    sceneryTiles = get_large_scenery_entry(entryType)->large_scenery.tiles;

    z = tileElement->base_height;
    direction = tileElement->type & 3;
    sequence = scenery_large_get_sequence(tileElement);

    if (!map_large_scenery_get_origin(x, y, z, direction, sequence, &x0, &y0, &z0, NULL)) {
        return;
    }

    // Iterate through each tile of the large scenery element
    sequence = 0;
    for (tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++) {
        int16_t offsetX = tile->x_offset;
        int16_t offsetY = tile->y_offset;
        rotate_map_coordinates(&offsetX, &offsetY, direction);

        x = x0 + offsetX;
        y = y0 + offsetY;
        z = (z0 + tile->z_offset) / 8;
        tileElement = map_get_large_scenery_segment(x, y, z, direction, sequence);
        if (tileElement != nullptr)
        {
            if (sequence == 0)
            {
                uint8_t flags = tileElement->type & 3;
                uint8_t primaryColour = scenery_large_get_primary_colour(tileElement);
                uint8_t secondaryColour = scenery_large_get_secondary_colour(tileElement);

                track_design_save_push_tile_element_desc(entry, x, y, z, flags, primaryColour, secondaryColour);
            }
            track_design_save_push_tile_element(x, y, tileElement);
        }
    }
}

static void track_design_save_add_wall(int32_t x, int32_t y, rct_tile_element *tileElement)
{
    int32_t entryType = tileElement->properties.wall.type;
    auto entry = object_entry_get_entry(OBJECT_TYPE_WALLS, entryType);

    uint8_t flags = 0;
    flags |= tileElement->type & 3;
    flags |= wall_get_tertiary_colour(tileElement) << 2;

    uint8_t secondaryColour = wall_get_secondary_colour(tileElement);
    uint8_t primaryColour = wall_get_primary_colour(tileElement);

    track_design_save_push_tile_element(x, y, tileElement);
    track_design_save_push_tile_element_desc(entry, x, y, tileElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_add_footpath(int32_t x, int32_t y, rct_tile_element *tileElement)
{
    int32_t entryType = tileElement->properties.path.type >> 4;
    auto entry = object_entry_get_entry(OBJECT_TYPE_PATHS, entryType);

    uint8_t flags = 0;
    flags |= tileElement->properties.path.edges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
    flags |= (tileElement->properties.path.type & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED) << 2;
    flags |= (tileElement->properties.path.type & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK) << 5;
    flags |= (tileElement->type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE) << 7;

    track_design_save_push_tile_element(x, y, tileElement);
    track_design_save_push_tile_element_desc(entry, x, y, tileElement->base_height, flags, 0, 0);
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static bool track_design_save_add_tile_element(int32_t interactionType, int32_t x, int32_t y, rct_tile_element *tileElement)
{
    if (!track_design_save_can_add_tile_element(tileElement)) {
        return false;
    }

    switch (interactionType) {
    case VIEWPORT_INTERACTION_ITEM_SCENERY:
        track_design_save_add_scenery(x, y, tileElement);
        return true;
    case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        track_design_save_add_large_scenery(x, y, tileElement);
        return true;
    case VIEWPORT_INTERACTION_ITEM_WALL:
        track_design_save_add_wall(x, y, tileElement);
        return true;
    case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
        track_design_save_add_footpath(x, y, tileElement);
        return true;
    default:
        return false;
    }
}

/**
 *
 *  rct2: 0x006D2F78
 */
static void track_design_save_pop_tile_element(int32_t x, int32_t y, rct_tile_element *tileElement)
{
    map_invalidate_tile_full(x, y);

    // Find index of map element to remove
    size_t removeIndex = SIZE_MAX;
    for (size_t i = 0; i < _trackSavedTileElementsCount; i++) {
        if (_trackSavedTileElements[i] == tileElement) {
            removeIndex = i;
        }
    }

    // Remove map element from list
    if (removeIndex != SIZE_MAX) {
        size_t remainingNumItems = _trackSavedTileElementsCount - removeIndex - 1;
        if (remainingNumItems > 0) {
            memmove(
                &_trackSavedTileElements[removeIndex],
                &_trackSavedTileElements[removeIndex + 1],
                remainingNumItems * sizeof(rct_tile_element*)
            );
        }
        _trackSavedTileElementsCount--;
        _trackSavedTileElements[_trackSavedTileElementsCount] = nullptr;
    }
}

/**
 *
 *  rct2: 0x006D2FDD
 */
static void track_design_save_pop_tile_element_desc(const rct_object_entry *entry, int32_t x, int32_t y, int32_t z, uint8_t flags)
{
    size_t removeIndex = SIZE_MAX;
    for (size_t i = 0; i < _trackSavedTileElementsDescCount; i++) {
        rct_td6_scenery_element *item = &_trackSavedTileElementsDesc[i];
        if (item->x != x / 32) continue;
        if (item->y != y / 32) continue;
        if (item->z != z) continue;
        if (item->flags != flags) continue;
        if (!object_entry_compare(&item->scenery_object, entry)) continue;

        removeIndex = i;
    }

    if (removeIndex != SIZE_MAX) {
        size_t remainingNumItems = _trackSavedTileElementsDescCount - removeIndex - 1;
        if (remainingNumItems > 0) {
            memmove(
                &_trackSavedTileElementsDesc[removeIndex],
                &_trackSavedTileElementsDesc[removeIndex + 1],
                remainingNumItems * sizeof(rct_td6_scenery_element)
            );
        }
        _trackSavedTileElementsDescCount--;
    }
}

static void track_design_save_remove_scenery(int32_t x, int32_t y, rct_tile_element *tileElement)
{
    int32_t entryType = tileElement->properties.scenery.type;
    auto entry = object_entry_get_entry(OBJECT_TYPE_SMALL_SCENERY, entryType);

    uint8_t flags = 0;
    flags |= tileElement->type & 3;
    flags |= (tileElement->type & 0xC0) >> 4;

    track_design_save_pop_tile_element(x, y, tileElement);
    track_design_save_pop_tile_element_desc(entry, x, y, tileElement->base_height, flags);
}

static void track_design_save_remove_large_scenery(int32_t x, int32_t y, rct_tile_element *tileElement)
{
    rct_large_scenery_tile *sceneryTiles, *tile;
    int32_t x0, y0, z0, z;
    int32_t direction, sequence;

    int32_t entryType = scenery_large_get_type(tileElement);
    auto entry = object_entry_get_entry(OBJECT_TYPE_LARGE_SCENERY, entryType);
    sceneryTiles = get_large_scenery_entry(entryType)->large_scenery.tiles;

    z = tileElement->base_height;
    direction = tileElement->type & 3;
    sequence = scenery_large_get_sequence(tileElement);

    if (!map_large_scenery_get_origin(x, y, z, direction, sequence, &x0, &y0, &z0, NULL)) {
        return;
    }

    // Iterate through each tile of the large scenery element
    sequence = 0;
    for (tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++) {
        int16_t offsetX = tile->x_offset;
        int16_t offsetY = tile->y_offset;
        rotate_map_coordinates(&offsetX, &offsetY, direction);

        x = x0 + offsetX;
        y = y0 + offsetY;
        z = (z0 + tile->z_offset) / 8;
        tileElement = map_get_large_scenery_segment(x, y, z, direction, sequence);
        if (tileElement != nullptr)
        {
            if (sequence == 0)
            {
                uint8_t flags = tileElement->type & 3;
                track_design_save_pop_tile_element_desc(entry, x, y, z, flags);
            }
            track_design_save_pop_tile_element(x, y, tileElement);
        }
    }
}

static void track_design_save_remove_wall(int32_t x, int32_t y, rct_tile_element *tileElement)
{
    int32_t entryType = tileElement->properties.wall.type;
    auto entry = object_entry_get_entry(OBJECT_TYPE_WALLS, entryType);

    uint8_t flags = 0;
    flags |= tileElement->type & 3;
    flags |= wall_get_tertiary_colour(tileElement) << 2;

    track_design_save_pop_tile_element(x, y, tileElement);
    track_design_save_pop_tile_element_desc(entry, x, y, tileElement->base_height, flags);
}

static void track_design_save_remove_footpath(int32_t x, int32_t y, rct_tile_element *tileElement)
{
    int32_t entryType = tileElement->properties.path.type >> 4;
    auto entry = object_entry_get_entry(OBJECT_TYPE_PATHS, entryType);

    uint8_t flags = 0;
    flags |= tileElement->properties.path.edges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
    flags |= (tileElement->properties.path.type & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED) << 2;
    flags |= (tileElement->properties.path.type & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK) << 5;
    flags |= (tileElement->type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE) << 7;

    track_design_save_pop_tile_element(x, y, tileElement);
    track_design_save_pop_tile_element_desc(entry, x, y, tileElement->base_height, flags);
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static void track_design_save_remove_tile_element(int32_t interactionType, int32_t x, int32_t y, rct_tile_element *tileElement)
{
    switch (interactionType) {
    case VIEWPORT_INTERACTION_ITEM_SCENERY:
        track_design_save_remove_scenery(x, y, tileElement);
        break;
    case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        track_design_save_remove_large_scenery(x, y, tileElement);
        break;
    case VIEWPORT_INTERACTION_ITEM_WALL:
        track_design_save_remove_wall(x, y, tileElement);
        break;
    case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
        track_design_save_remove_footpath(x, y, tileElement);
        break;
    }
}

static bool track_design_save_should_select_scenery_around(int32_t rideIndex, rct_tile_element *tileElement)
{
    switch (tileElement->GetType()) {
    case TILE_ELEMENT_TYPE_PATH:
        if ((tileElement->type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE) && tileElement->properties.path.addition_status == rideIndex)
            return true;
        break;
    case TILE_ELEMENT_TYPE_TRACK:
        if (track_element_get_ride_index(tileElement) == rideIndex)
            return true;
        break;
    case TILE_ELEMENT_TYPE_ENTRANCE:
        if (tileElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE)
            break;
        if (tileElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_EXIT)
            break;
        if (tileElement->properties.entrance.ride_index == rideIndex)
            return true;
        break;
    }
    return false;
}

static void track_design_save_select_nearby_scenery_for_tile(int32_t rideIndex, int32_t cx, int32_t cy)
{
    rct_tile_element *tileElement;

    for (int32_t y = cy - TRACK_NEARBY_SCENERY_DISTANCE; y <= cy + TRACK_NEARBY_SCENERY_DISTANCE; y++) {
        for (int32_t x = cx - TRACK_NEARBY_SCENERY_DISTANCE; x <= cx + TRACK_NEARBY_SCENERY_DISTANCE; x++) {
            tileElement = map_get_first_element_at(x, y);
            do {
                int32_t interactionType = VIEWPORT_INTERACTION_ITEM_NONE;
                switch (tileElement->GetType()) {
                case TILE_ELEMENT_TYPE_PATH:
                    if (!(tileElement->type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE))
                        interactionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;
                    else if (tileElement->properties.path.addition_status == rideIndex)
                        interactionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;
                    break;
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                    interactionType = VIEWPORT_INTERACTION_ITEM_SCENERY;
                    break;
                case TILE_ELEMENT_TYPE_WALL:
                    interactionType = VIEWPORT_INTERACTION_ITEM_WALL;
                    break;
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                    interactionType = VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY;
                    break;
                }

                if (interactionType != VIEWPORT_INTERACTION_ITEM_NONE) {
                    if (!track_design_save_contains_tile_element(tileElement)) {
                        track_design_save_add_tile_element(interactionType, x * 32, y * 32, tileElement);
                    }
                }
            } while (!(tileElement++)->IsLastForTile());
        }
    }
}

/* Based on rct2: 0x006D2897 */
static bool track_design_save_copy_scenery_to_td6(rct_track_td6 *td6)
{
    // Copy TD6 scenery elements to new memory and add end marker
    size_t totalSceneryElementsSize = _trackSavedTileElementsDescCount * sizeof(rct_td6_scenery_element);
    td6->scenery_elements = (rct_td6_scenery_element *)malloc(totalSceneryElementsSize + 1);
    memcpy(td6->scenery_elements, _trackSavedTileElementsDesc, totalSceneryElementsSize);
    *((uint8_t*)&td6->scenery_elements[_trackSavedTileElementsDescCount]) = 0xFF;

    // Run an element loop
    for (size_t i = 0; i < _trackSavedTileElementsDescCount; i++) {
        rct_td6_scenery_element *scenery = &td6->scenery_elements[i];

        switch (object_entry_get_type(&scenery->scenery_object)) {
        case OBJECT_TYPE_PATHS:
        {
            uint8_t slope = (scenery->flags & 0x60) >> 5;
            slope -= _trackSaveDirection;

            scenery->flags &= 0x9F;
            scenery->flags |= ((slope & 3) << 5);

            // Direction of connection on path
            uint8_t direction = scenery->flags & 0xF;
            // Rotate the direction by the track direction
            direction = ((direction << 4) >> _trackSaveDirection);

            scenery->flags &= 0xF0;
            scenery->flags |= (direction & 0xF) | (direction >> 4);
            break;
        }
        case OBJECT_TYPE_WALLS:
        {
            uint8_t direction = scenery->flags & 3;
            direction -= _trackSaveDirection;

            scenery->flags &= 0xFC;
            scenery->flags |= (direction & 3);
            break;
        }
        default:
        {
            uint8_t direction = scenery->flags & 3;
            uint8_t quadrant = (scenery->flags & 0x0C) >> 2;

            direction -= _trackSaveDirection;
            quadrant -= _trackSaveDirection;

            scenery->flags &= 0xF0;
            scenery->flags |= (direction & 3) | ((quadrant & 3) << 2);
            break;
        }
        }

        int16_t x = ((uint8_t)scenery->x) * 32 - gTrackPreviewOrigin.x;
        int16_t y = ((uint8_t)scenery->y) * 32 - gTrackPreviewOrigin.y;
        rotate_map_coordinates(&x, &y, (0 - _trackSaveDirection) & 3);
        x /= 32;
        y /= 32;

        if (x > 127 || y > 127 || x < -126 || y < -126){
            context_show_error(STR_CANT_SAVE_TRACK_DESIGN, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY);
            SafeFree(td6->scenery_elements);
            return false;
        }

        scenery->x = (int8_t)x;
        scenery->y = (int8_t)y;

        int32_t z = scenery->z * 8 - gTrackPreviewOrigin.z;
        z /= 8;
        if (z > 127 || z < -126) {
            context_show_error(STR_CANT_SAVE_TRACK_DESIGN, STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY);
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
static rct_track_td6 *track_design_save_to_td6(uint8_t rideIndex)
{
    rct_track_td6 *td6 = (rct_track_td6 *)calloc(1, sizeof(rct_track_td6));
    Ride *ride = get_ride(rideIndex);
    td6->type = ride->type;
    auto object = object_entry_get_entry(OBJECT_TYPE_RIDE, ride->subtype);

    // Note we are only copying rct_object_entry in size and
    // not the extended as we don't need the chunk size.
    memcpy(&td6->vehicle_object, object, sizeof(rct_object_entry));

    td6->ride_mode = ride->mode;

    td6->version_and_colour_scheme =
        (ride->colour_scheme_type & 3) |
        (1 << 3); // Version .TD6

    for (int32_t i = 0; i < RCT12_MAX_VEHICLES_PER_RIDE; i++)
    {
        td6->vehicle_colours[i] = ride->vehicle_colours[i];
        td6->vehicle_additional_colour[i] = ride->vehicle_colours_extended[i];
    }

    for (int32_t i = 0; i < RCT12_NUM_COLOUR_SCHEMES; i++)
    {
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
    td6->max_speed = (int8_t)(ride->max_speed / 65536);
    td6->average_speed = (int8_t)(ride->average_speed / 65536);
    td6->ride_length = ride_get_total_length(ride) / 65536;
    td6->max_positive_vertical_g = ride->max_positive_vertical_g / 32;
    td6->max_negative_vertical_g = ride->max_negative_vertical_g / 32;
    td6->max_lateral_g = ride->max_lateral_g / 32;
    td6->inversions = ride->inversions;
    td6->drops = ride->drops;
    td6->highest_drop_height = ride->highest_drop_height;

    uint16_t total_air_time = (ride->total_air_time * 123) / 1024;
    if (total_air_time > 255) {
        total_air_time = 0;
    }
    td6->total_air_time = (uint8_t)total_air_time;

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
        td6 = nullptr;
    }
    return td6;
}

/**
 *
 *  rct2: 0x006CEAAE
 */
static bool track_design_save_to_td6_for_maze(uint8_t rideIndex, rct_track_td6 *td6)
{
    rct_tile_element *tileElement = nullptr;
    bool mapFound = false;
    int16_t startX = 0;
    int16_t startY = 0;
    for (startY = 0; startY < 8192; startY += 32) {
        for (startX = 0; startX < 8192; startX += 32) {
            tileElement = map_get_first_element_at(startX >> 5, startY >> 5);
            do {
                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;
                if (track_element_get_ride_index(tileElement) == rideIndex){
                    mapFound = true;
                    break;
                }
            } while (!(tileElement++)->IsLastForTile());
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

    gTrackPreviewOrigin = { startX, startY, (int16_t)(tileElement->base_height * 8) };

    size_t numMazeElements = 0;
    td6->maze_elements = (rct_td6_maze_element *)calloc(8192, sizeof(rct_td6_maze_element));
    rct_td6_maze_element *maze = td6->maze_elements;

    // x is defined here as we can start the search
    // on tile start_x, start_y but then the next row
    // must restart on 0
    for (int16_t y = startY, x = startX; y < 8192; y += 32) {
        for (; x < 8192; x += 32) {
            tileElement = map_get_first_element_at(x / 32, y / 32);
            do {
                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK) continue;
                if (track_element_get_ride_index(tileElement) != rideIndex) continue;

                maze->maze_entry = track_element_get_maze_entry(tileElement);
                maze->x = (x - startX) / 32;
                maze->y = (y - startY) / 32;
                maze++;
                numMazeElements++;

                if (numMazeElements >= 2000) {
                    gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
                    SafeFree(td6->maze_elements);
                    return false;
                }
            } while (!(tileElement++)->IsLastForTile());

        }
        x = 0;
    }

    TileCoordsXYZD location = ride_get_entrance_location(rideIndex, 0);

    if (location.isNull())
    {
        gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
        SafeFree(td6->maze_elements);
        return false;
    }

    int16_t x = location.x * 32;
    int16_t y = location.y * 32;

    tileElement = map_get_first_element_at(location.x, location.y);
    do {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE) continue;
        if (tileElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE) continue;
        if (tileElement->properties.entrance.ride_index == rideIndex) break;
    } while (!(tileElement++)->IsLastForTile());
    // Add something that stops this from walking off the end

    uint8_t entrance_direction = tile_element_get_direction(tileElement);
    maze->direction = entrance_direction;
    maze->type = 8;
    maze->x = (int8_t)((x - startX) / 32);
    maze->y = (int8_t)((y - startY) / 32);
    maze++;
    numMazeElements++;

    location = ride_get_entrance_location(rideIndex, 0);
    if (location.isNull())
    {
        gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
        SafeFree(td6->maze_elements);
        return false;
    }

    x = location.x * 32;
    y = location.y * 32;
    tileElement = map_get_first_element_at(location.x, location.y);
    do {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE) continue;
        if (tileElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_EXIT) continue;
        if (tileElement->properties.entrance.ride_index == rideIndex) break;
    } while (!(tileElement++)->IsLastForTile());
    // Add something that stops this from walking off the end

    uint8_t exit_direction = tile_element_get_direction(tileElement);
    maze->direction = exit_direction;
    maze->type = 0x80;
    maze->x = (int8_t)((x - startX) / 32);
    maze->y = (int8_t)((y - startY) / 32);
    maze++;
    maze->all = 0;
    maze++;
    numMazeElements++;

    // Write end marker
    maze->all = 0;
    maze++;
    numMazeElements++;

    // Trim memory
    td6->maze_elements = (rct_td6_maze_element *)realloc(td6->maze_elements, numMazeElements * sizeof(rct_td6_maze_element));

    // Save global vars as they are still used by scenery
    int16_t startZ = gTrackPreviewOrigin.z;
    place_virtual_track(td6, PTD_OPERATION_DRAW_OUTLINES, true, 0, 4096, 4096, 0);
    gTrackPreviewOrigin = { startX, startY, startZ };

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
static bool track_design_save_to_td6_for_tracked_ride(uint8_t rideIndex, rct_track_td6 *td6)
{
    Ride *ride = get_ride(rideIndex);
    CoordsXYE trackElement;

    if (!ride_try_get_origin_element(rideIndex, &trackElement)) {
        gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
        return false;
    }

    ride_get_start_of_track(&trackElement);

    int32_t z = trackElement.element->base_height * 8;
    uint8_t track_type = track_element_get_type(trackElement.element);
    uint8_t direction = tile_element_get_direction(trackElement.element);
    _trackSaveDirection = direction;

    if (sub_6C683D(&trackElement.x, &trackElement.y, &z, direction, track_type, 0, &trackElement.element, 0)) {
        gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
        return 0;
    }

    const rct_track_coordinates *trackCoordinates = &TrackCoordinates[track_element_get_type(trackElement.element)];
    // Used in the following loop to know when we have
    // completed all of the elements and are back at the
    // start.
    rct_tile_element *initialMap = trackElement.element;

    int16_t start_x = trackElement.x;
    int16_t start_y = trackElement.y;
    int16_t start_z = z + trackCoordinates->z_begin;
    gTrackPreviewOrigin = { start_x, start_y, start_z };

    size_t numTrackElements = 0;
    td6->track_elements = (rct_td6_track_element *)calloc(TRACK_TD6_MAX_ELEMENTS, sizeof(rct_td6_track_element));
    rct_td6_track_element *track = td6->track_elements;
    do {
        track->type = track_element_get_type(trackElement.element);
        if (track->type == TRACK_ELEM_255) {
            track->type = TRACK_ELEM_255_ALIAS;
        }

        uint8_t bh;
        if (track_element_has_speed_setting(track->type))
        {
            bh = tile_element_get_brake_booster_speed(trackElement.element) >> 1;
        }
        else
        {
            bh = track_element_get_seat_rotation(trackElement.element);
        }

        uint8_t flags = (trackElement.element->type & (1 << 7)) | bh;
        flags |= track_element_get_colour_scheme(trackElement.element) << 4;
        if (
            RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE &&
            track_element_is_inverted(trackElement.element))
        {
            flags |= TRACK_ELEMENT_FLAG_INVERTED;
        }

        track->flags = flags;
        track++;
        numTrackElements++;

        if (!track_block_get_next(&trackElement, &trackElement, NULL, NULL))
        {
            break;
        }

        z = trackElement.element->base_height * 8;
        direction = tile_element_get_direction(trackElement.element);
        track_type = track_element_get_type(trackElement.element);

        if (sub_6C683D(&trackElement.x, &trackElement.y, &z, direction, track_type, 0, &trackElement.element, 0))
        {
            break;
        }

        if (TRACK_TD6_MAX_ELEMENTS == numTrackElements)
        {
            free(td6->track_elements);
            gGameCommandErrorText = STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
            return false;
        }
    }
    while (trackElement.element != initialMap);

    td6->track_elements = (rct_td6_track_element *)realloc(td6->track_elements, numTrackElements * sizeof(rct_td6_track_element) + 1);
    *((uint8_t*)&td6->track_elements[numTrackElements]) = 0xFF;

    size_t numEntranceElements = 0;
    td6->entrance_elements = (rct_td6_entrance_element *)calloc(32, sizeof(rct_td6_entrance_element));
    rct_td6_entrance_element *entrance = td6->entrance_elements;

    // First entrances, second exits
    for (int32_t i = 0; i < 2; i++) {
        for (int32_t station_index = 0; station_index < RCT12_MAX_STATIONS_PER_RIDE; station_index++) {
            z = ride->station_heights[station_index];

            TileCoordsXYZD location;
            if (i == 0) {
                location = ride_get_entrance_location(rideIndex, station_index);
            } else {
                location = ride_get_exit_location(rideIndex, station_index);
            }

            if (location.isNull()) {
                continue;
            }

            int16_t x = location.x * 32;
            int16_t y = location.y * 32;

            rct_tile_element *tile_element = map_get_first_element_at(x >> 5, y >> 5);
            do {
                if (tile_element->GetType() != TILE_ELEMENT_TYPE_ENTRANCE) continue;
                if (tile_element->base_height == z) break;
            } while (!(tile_element++)->IsLastForTile());
            // Add something that stops this from walking off the end

            uint8_t entrance_direction = tile_element_get_direction(tile_element);
            entrance_direction -= _trackSaveDirection;
            entrance_direction &= TILE_ELEMENT_DIRECTION_MASK;
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
    td6->entrance_elements = (rct_td6_entrance_element *)realloc(td6->entrance_elements, numEntranceElements * sizeof(rct_td6_entrance_element) + 1);
    *((uint8_t*)&td6->entrance_elements[numEntranceElements]) = 0xFF;

    place_virtual_track(td6, PTD_OPERATION_DRAW_OUTLINES, true, 0, 4096, 4096, 0);

    // Resave global vars for scenery reasons.
    gTrackPreviewOrigin = { start_x, start_y, start_z };

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
    if (sceneryElement != nullptr) {
        while (sceneryElement->scenery_object.end_flag != 0xFF) {
            count++;
            sceneryElement++;
        }
    }
    return count;
}

struct auto_buffer {
    void *ptr;
    size_t length;
    size_t capacity;
};

static void auto_buffer_write(auto_buffer *buffer, const void *src, size_t len)
{
    size_t remainingSpace = buffer->capacity - buffer->length;
    if (remainingSpace < len) {
        do {
            buffer->capacity = std::max<size_t>(8, buffer->capacity * 2);
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
    const rct_td6_maze_element EndMarkerForMaze = {};
    const uint8_t EndMarker = 0xFF;

    window_close_construction_windows();

    // Create TD6 data buffer
    auto_buffer td6Buffer = {};
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
    uint8_t *encodedData = (uint8_t *)malloc(0x8000);
    assert(td6Buffer.ptr != nullptr);
    size_t encodedDataLength = sawyercoding_encode_td6((uint8_t*)td6Buffer.ptr, encodedData, td6Buffer.length);

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
