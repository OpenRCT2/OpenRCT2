/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../Game.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../interface/Viewport.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
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
#include "Station.h"
#include "Track.h"
#include "TrackData.h"
#include "TrackDesign.h"
#include "TrackDesignRepository.h"

#include <algorithm>

#define TRACK_MAX_SAVED_TILE_ELEMENTS 1500
#define TRACK_NEARBY_SCENERY_DISTANCE 1
#define TRACK_TD6_MAX_ELEMENTS 8192

bool gTrackDesignSaveMode = false;
ride_id_t gTrackDesignSaveRideIndex = RIDE_ID_NULL;

std::vector<const TileElement*> _trackSavedTileElements;
std::vector<rct_td6_scenery_element> _trackSavedTileElementsDesc;

static bool track_design_save_should_select_scenery_around(ride_id_t rideIndex, TileElement* tileElement);
static void track_design_save_select_nearby_scenery_for_tile(ride_id_t rideIndex, int32_t cx, int32_t cy);
static bool track_design_save_add_tile_element(int32_t interactionType, int32_t x, int32_t y, TileElement* tileElement);
static void track_design_save_remove_tile_element(int32_t interactionType, int32_t x, int32_t y, TileElement* tileElement);

void track_design_save_init()
{
    _trackSavedTileElements.clear();
    _trackSavedTileElementsDesc.clear();
}

/**
 *
 *  rct2: 0x006D2B07
 */
void track_design_save_select_tile_element(
    int32_t interactionType, int32_t x, int32_t y, TileElement* tileElement, bool collect)
{
    if (track_design_save_contains_tile_element(tileElement))
    {
        if (!collect)
        {
            track_design_save_remove_tile_element(interactionType, x, y, tileElement);
        }
    }
    else
    {
        if (collect)
        {
            if (!track_design_save_add_tile_element(interactionType, x, y, tileElement))
            {
                context_show_error(
                    STR_SAVE_TRACK_SCENERY_UNABLE_TO_SELECT_ADDITIONAL_ITEM_OF_SCENERY,
                    STR_SAVE_TRACK_SCENERY_TOO_MANY_ITEMS_SELECTED);
            }
        }
    }
}

/**
 *
 *  rct2: 0x006D303D
 */
void track_design_save_select_nearby_scenery(ride_id_t rideIndex)
{
    TileElement* tileElement;

    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            tileElement = map_get_first_element_at(x, y);
            do
            {
                if (track_design_save_should_select_scenery_around(rideIndex, tileElement))
                {
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

bool track_design_save_contains_tile_element(const TileElement* tileElement)
{
    for (auto& tile : _trackSavedTileElements)
    {
        if (tile == tileElement)
        {
            return true;
        }
    }
    return false;
}

static int32_t tile_element_get_total_element_count(TileElement* tileElement)
{
    int32_t elementCount;
    rct_scenery_entry* sceneryEntry;
    rct_large_scenery_tile* tile;

    switch (tileElement->GetType())
    {
        case TILE_ELEMENT_TYPE_PATH:
        case TILE_ELEMENT_TYPE_SMALL_SCENERY:
        case TILE_ELEMENT_TYPE_WALL:
            return 1;

        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            tile = sceneryEntry->large_scenery.tiles;
            elementCount = 0;
            do
            {
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
static bool track_design_save_can_add_tile_element(TileElement* tileElement)
{
    size_t newElementCount = tile_element_get_total_element_count(tileElement);
    if (newElementCount == 0)
    {
        return false;
    }

    // Get number of spare elements left
    size_t spareSavedElements = TRACK_MAX_SAVED_TILE_ELEMENTS - _trackSavedTileElements.size();
    if (newElementCount > spareSavedElements)
    {
        // No more spare saved elements left
        return false;
    }

    return true;
}

/**
 *
 *  rct2: 0x006D2F4C
 */
static void track_design_save_push_tile_element(int32_t x, int32_t y, TileElement* tileElement)
{
    if (_trackSavedTileElements.size() < TRACK_MAX_SAVED_TILE_ELEMENTS)
    {
        _trackSavedTileElements.push_back(tileElement);
        map_invalidate_tile_full(x, y);
    }
}

/**
 *
 *  rct2: 0x006D2FA7
 */
static void track_design_save_push_tile_element_desc(
    const rct_object_entry* entry, int32_t x, int32_t y, int32_t z, uint8_t flags, uint8_t primaryColour,
    uint8_t secondaryColour)
{
    rct_td6_scenery_element item{};
    item.scenery_object = *entry;
    item.x = x / 32;
    item.y = y / 32;
    item.z = z;
    item.flags = flags;
    item.primary_colour = primaryColour;
    item.secondary_colour = secondaryColour;

    _trackSavedTileElementsDesc.push_back(item);
}

static void track_design_save_add_scenery(int32_t x, int32_t y, TileElement* tileElement)
{
    SmallSceneryElement* sceneryElement = tileElement->AsSmallScenery();
    int32_t entryType = sceneryElement->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_SMALL_SCENERY, entryType);

    uint8_t flags = 0;
    flags |= tileElement->GetDirection();
    flags |= tileElement->AsSmallScenery()->GetSceneryQuadrant() << 2;

    uint8_t primaryColour = sceneryElement->GetPrimaryColour();
    uint8_t secondaryColour = sceneryElement->GetSecondaryColour();

    track_design_save_push_tile_element(x, y, tileElement);
    track_design_save_push_tile_element_desc(entry, x, y, tileElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_add_large_scenery(int32_t x, int32_t y, LargeSceneryElement* tileElement)
{
    rct_large_scenery_tile *sceneryTiles, *tile;
    int32_t x0, y0, z0, z;
    int32_t direction, sequence;

    int32_t entryType = tileElement->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_LARGE_SCENERY, entryType);
    sceneryTiles = get_large_scenery_entry(entryType)->large_scenery.tiles;

    z = tileElement->base_height;
    direction = tileElement->GetDirection();
    sequence = tileElement->GetSequenceIndex();

    if (!map_large_scenery_get_origin(x, y, z, direction, sequence, &x0, &y0, &z0, nullptr))
    {
        return;
    }

    // Iterate through each tile of the large scenery element
    sequence = 0;
    for (tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++)
    {
        int16_t offsetX = tile->x_offset;
        int16_t offsetY = tile->y_offset;
        rotate_map_coordinates(&offsetX, &offsetY, direction);

        x = x0 + offsetX;
        y = y0 + offsetY;
        z = (z0 + tile->z_offset) / 8;
        auto largeElement = map_get_large_scenery_segment(x, y, z, direction, sequence);
        if (largeElement != nullptr)
        {
            if (sequence == 0)
            {
                uint8_t flags = largeElement->GetDirection();
                uint8_t primaryColour = largeElement->GetPrimaryColour();
                uint8_t secondaryColour = largeElement->GetSecondaryColour();

                track_design_save_push_tile_element_desc(entry, x, y, z, flags, primaryColour, secondaryColour);
            }
            track_design_save_push_tile_element(x, y, reinterpret_cast<TileElement*>(largeElement));
        }
    }
}

static void track_design_save_add_wall(int32_t x, int32_t y, TileElement* tileElement)
{
    int32_t entryType = tileElement->AsWall()->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_WALLS, entryType);

    uint8_t flags = 0;
    flags |= tileElement->GetDirection();
    flags |= tileElement->AsWall()->GetTertiaryColour() << 2;

    uint8_t secondaryColour = tileElement->AsWall()->GetSecondaryColour();
    uint8_t primaryColour = tileElement->AsWall()->GetPrimaryColour();

    track_design_save_push_tile_element(x, y, tileElement);
    track_design_save_push_tile_element_desc(entry, x, y, tileElement->base_height, flags, primaryColour, secondaryColour);
}

static void track_design_save_add_footpath(int32_t x, int32_t y, TileElement* tileElement)
{
    int32_t entryType = tileElement->AsPath()->GetPathEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_PATHS, entryType);

    uint8_t flags = 0;
    flags |= tileElement->AsPath()->GetEdges();
    flags |= (tileElement->AsPath()->GetSlopeDirection()) << 5;
    if (tileElement->AsPath()->IsSloped())
        flags |= 0b00010000;
    if (tileElement->AsPath()->IsQueue())
        flags |= 1 << 7;

    track_design_save_push_tile_element(x, y, tileElement);
    track_design_save_push_tile_element_desc(entry, x, y, tileElement->base_height, flags, 0, 0);
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static bool track_design_save_add_tile_element(int32_t interactionType, int32_t x, int32_t y, TileElement* tileElement)
{
    if (!track_design_save_can_add_tile_element(tileElement))
    {
        return false;
    }

    switch (interactionType)
    {
        case VIEWPORT_INTERACTION_ITEM_SCENERY:
            track_design_save_add_scenery(x, y, tileElement);
            return true;
        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
            track_design_save_add_large_scenery(x, y, tileElement->AsLargeScenery());
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
static void track_design_save_pop_tile_element(int32_t x, int32_t y, TileElement* tileElement)
{
    map_invalidate_tile_full(x, y);

    // Find index of map element to remove
    size_t removeIndex = SIZE_MAX;
    for (size_t i = 0; i < _trackSavedTileElements.size(); i++)
    {
        if (_trackSavedTileElements[i] == tileElement)
        {
            removeIndex = i;
        }
    }

    if (removeIndex != SIZE_MAX)
    {
        _trackSavedTileElements.erase(_trackSavedTileElements.begin() + removeIndex);
    }
}

/**
 *
 *  rct2: 0x006D2FDD
 */
static void track_design_save_pop_tile_element_desc(
    const rct_object_entry* entry, int32_t x, int32_t y, int32_t z, uint8_t flags)
{
    size_t removeIndex = SIZE_MAX;
    for (size_t i = 0; i < _trackSavedTileElementsDesc.size(); i++)
    {
        rct_td6_scenery_element* item = &_trackSavedTileElementsDesc[i];
        if (item->x != x / 32)
            continue;
        if (item->y != y / 32)
            continue;
        if (item->z != z)
            continue;
        if (item->flags != flags)
            continue;
        if (!object_entry_compare(&item->scenery_object, entry))
            continue;

        removeIndex = i;
    }

    if (removeIndex != SIZE_MAX)
    {
        _trackSavedTileElementsDesc.erase(_trackSavedTileElementsDesc.begin() + removeIndex);
    }
}

static void track_design_save_remove_scenery(int32_t x, int32_t y, TileElement* tileElement)
{
    int32_t entryType = tileElement->AsSmallScenery()->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_SMALL_SCENERY, entryType);

    uint8_t flags = 0;
    flags |= tileElement->GetDirection();
    flags |= tileElement->AsSmallScenery()->GetSceneryQuadrant() << 2;

    track_design_save_pop_tile_element(x, y, tileElement);
    track_design_save_pop_tile_element_desc(entry, x, y, tileElement->base_height, flags);
}

static void track_design_save_remove_large_scenery(int32_t x, int32_t y, LargeSceneryElement* tileElement)
{
    rct_large_scenery_tile *sceneryTiles, *tile;
    int32_t x0, y0, z0, z;
    int32_t direction, sequence;

    int32_t entryType = tileElement->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_LARGE_SCENERY, entryType);
    sceneryTiles = get_large_scenery_entry(entryType)->large_scenery.tiles;

    z = tileElement->base_height;
    direction = tileElement->GetDirection();
    sequence = tileElement->GetSequenceIndex();

    if (!map_large_scenery_get_origin(x, y, z, direction, sequence, &x0, &y0, &z0, nullptr))
    {
        return;
    }

    // Iterate through each tile of the large scenery element
    sequence = 0;
    for (tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++)
    {
        int16_t offsetX = tile->x_offset;
        int16_t offsetY = tile->y_offset;
        rotate_map_coordinates(&offsetX, &offsetY, direction);

        x = x0 + offsetX;
        y = y0 + offsetY;
        z = (z0 + tile->z_offset) / 8;
        auto largeElement = map_get_large_scenery_segment(x, y, z, direction, sequence);
        if (largeElement != nullptr)
        {
            if (sequence == 0)
            {
                uint8_t flags = largeElement->GetDirection();
                track_design_save_pop_tile_element_desc(entry, x, y, z, flags);
            }
            track_design_save_pop_tile_element(x, y, reinterpret_cast<TileElement*>(largeElement));
        }
    }
}

static void track_design_save_remove_wall(int32_t x, int32_t y, TileElement* tileElement)
{
    int32_t entryType = tileElement->AsWall()->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_WALLS, entryType);

    uint8_t flags = 0;
    flags |= tileElement->GetDirection();
    flags |= tileElement->AsWall()->GetTertiaryColour() << 2;

    track_design_save_pop_tile_element(x, y, tileElement);
    track_design_save_pop_tile_element_desc(entry, x, y, tileElement->base_height, flags);
}

static void track_design_save_remove_footpath(int32_t x, int32_t y, TileElement* tileElement)
{
    int32_t entryType = tileElement->AsPath()->GetPathEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_PATHS, entryType);

    uint8_t flags = 0;
    flags |= tileElement->AsPath()->GetEdges();
    if (tileElement->AsPath()->IsSloped())
        flags |= (1 << 4);
    flags |= (tileElement->AsPath()->GetSlopeDirection()) << 5;
    if (tileElement->AsPath()->IsQueue())
        flags |= (1 << 7);

    track_design_save_pop_tile_element(x, y, tileElement);
    track_design_save_pop_tile_element_desc(entry, x, y, tileElement->base_height, flags);
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static void track_design_save_remove_tile_element(int32_t interactionType, int32_t x, int32_t y, TileElement* tileElement)
{
    switch (interactionType)
    {
        case VIEWPORT_INTERACTION_ITEM_SCENERY:
            track_design_save_remove_scenery(x, y, tileElement);
            break;
        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
            track_design_save_remove_large_scenery(x, y, tileElement->AsLargeScenery());
            break;
        case VIEWPORT_INTERACTION_ITEM_WALL:
            track_design_save_remove_wall(x, y, tileElement);
            break;
        case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
            track_design_save_remove_footpath(x, y, tileElement);
            break;
    }
}

static bool track_design_save_should_select_scenery_around(ride_id_t rideIndex, TileElement* tileElement)
{
    switch (tileElement->GetType())
    {
        case TILE_ELEMENT_TYPE_PATH:
            if (tileElement->AsPath()->IsQueue() && tileElement->AsPath()->GetRideIndex() == rideIndex)
                return true;
            break;
        case TILE_ELEMENT_TYPE_TRACK:
            if (tileElement->AsTrack()->GetRideIndex() == rideIndex)
                return true;
            break;
        case TILE_ELEMENT_TYPE_ENTRANCE:
            // FIXME: This will always break and return false!
            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE)
                break;
            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_EXIT)
                break;
            if (tileElement->AsEntrance()->GetRideIndex() == rideIndex)
                return true;
            break;
    }
    return false;
}

static void track_design_save_select_nearby_scenery_for_tile(ride_id_t rideIndex, int32_t cx, int32_t cy)
{
    TileElement* tileElement;

    for (int32_t y = cy - TRACK_NEARBY_SCENERY_DISTANCE; y <= cy + TRACK_NEARBY_SCENERY_DISTANCE; y++)
    {
        for (int32_t x = cx - TRACK_NEARBY_SCENERY_DISTANCE; x <= cx + TRACK_NEARBY_SCENERY_DISTANCE; x++)
        {
            tileElement = map_get_first_element_at(x, y);
            do
            {
                int32_t interactionType = VIEWPORT_INTERACTION_ITEM_NONE;
                switch (tileElement->GetType())
                {
                    case TILE_ELEMENT_TYPE_PATH:
                        if (!tileElement->AsPath()->IsQueue())
                            interactionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;
                        else if (tileElement->AsPath()->GetRideIndex() == rideIndex)
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

                if (interactionType != VIEWPORT_INTERACTION_ITEM_NONE)
                {
                    if (!track_design_save_contains_tile_element(tileElement))
                    {
                        track_design_save_add_tile_element(interactionType, x * 32, y * 32, tileElement);
                    }
                }
            } while (!(tileElement++)->IsLastForTile());
        }
    }
}
