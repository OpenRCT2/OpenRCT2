/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

constexpr size_t TRACK_MAX_SAVED_TILE_ELEMENTS = 1500;
constexpr int32_t TRACK_NEARBY_SCENERY_DISTANCE = 1;

bool gTrackDesignSaveMode = false;
ride_id_t gTrackDesignSaveRideIndex = RIDE_ID_NULL;

std::vector<const TileElement*> _trackSavedTileElements;
std::vector<TrackDesignSceneryElement> _trackSavedTileElementsDesc;

static bool track_design_save_should_select_scenery_around(ride_id_t rideIndex, TileElement* tileElement);
static void track_design_save_select_nearby_scenery_for_tile(ride_id_t rideIndex, int32_t cx, int32_t cy);
static bool track_design_save_add_tile_element(int32_t interactionType, const CoordsXY& loc, TileElement* tileElement);
static void track_design_save_remove_tile_element(int32_t interactionType, const CoordsXY& loc, TileElement* tileElement);

void track_design_save_init()
{
    _trackSavedTileElements.clear();
    _trackSavedTileElementsDesc.clear();
}

/**
 *
 *  rct2: 0x006D2B07
 */
void track_design_save_select_tile_element(int32_t interactionType, const CoordsXY& loc, TileElement* tileElement, bool collect)
{
    if (track_design_save_contains_tile_element(tileElement))
    {
        if (!collect)
        {
            track_design_save_remove_tile_element(interactionType, loc, tileElement);
        }
    }
    else
    {
        if (collect)
        {
            if (!track_design_save_add_tile_element(interactionType, loc, tileElement))
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
    tile_element_iterator it;
    tile_element_iterator_begin(&it);
    do
    {
        if (track_design_save_should_select_scenery_around(rideIndex, it.element))
        {
            track_design_save_select_nearby_scenery_for_tile(rideIndex, it.x, it.y);
        }
    } while (tile_element_iterator_next(&it));

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
            } while (tile->x_offset != static_cast<int16_t>(static_cast<uint16_t>(0xFFFF)));
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
static void track_design_save_push_tile_element(const CoordsXY& loc, TileElement* tileElement)
{
    if (_trackSavedTileElements.size() < TRACK_MAX_SAVED_TILE_ELEMENTS)
    {
        _trackSavedTileElements.push_back(tileElement);
        map_invalidate_tile_full(loc);
    }
}

/**
 *
 *  rct2: 0x006D2FA7
 */
static void track_design_save_push_tile_element_desc(
    const rct_object_entry* entry, const CoordsXYZ& loc, uint8_t flags, uint8_t primaryColour, uint8_t secondaryColour)
{
    auto tileLoc = TileCoordsXYZ(loc);
    TrackDesignSceneryElement item{};
    item.scenery_object = *entry;
    item.x = tileLoc.x;
    item.y = tileLoc.y;
    item.z = tileLoc.z;
    item.flags = flags;
    item.primary_colour = primaryColour;
    item.secondary_colour = secondaryColour;

    _trackSavedTileElementsDesc.push_back(item);
}

static void track_design_save_add_scenery(const CoordsXY& loc, SmallSceneryElement* sceneryElement)
{
    int32_t entryType = sceneryElement->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_SMALL_SCENERY, entryType);

    uint8_t flags = 0;
    flags |= sceneryElement->GetDirection();
    flags |= sceneryElement->GetSceneryQuadrant() << 2;

    uint8_t primaryColour = sceneryElement->GetPrimaryColour();
    uint8_t secondaryColour = sceneryElement->GetSecondaryColour();

    track_design_save_push_tile_element(loc, reinterpret_cast<TileElement*>(sceneryElement));
    track_design_save_push_tile_element_desc(
        entry, { loc.x, loc.y, sceneryElement->GetBaseZ() }, flags, primaryColour, secondaryColour);
}

static void track_design_save_add_large_scenery(const CoordsXY& loc, LargeSceneryElement* tileElement)
{
    rct_large_scenery_tile *sceneryTiles, *tile;
    int32_t direction, sequence;

    if (tileElement == nullptr)
    {
        log_warning("Null tile element");
        return;
    }

    int32_t entryType = tileElement->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_LARGE_SCENERY, entryType);
    sceneryTiles = get_large_scenery_entry(entryType)->large_scenery.tiles;

    int32_t z = tileElement->base_height;
    direction = tileElement->GetDirection();
    sequence = tileElement->GetSequenceIndex();

    auto sceneryOrigin = map_large_scenery_get_origin(
        { loc.x, loc.y, z << 3, static_cast<Direction>(direction) }, sequence, nullptr);
    if (!sceneryOrigin)
    {
        return;
    }

    // Iterate through each tile of the large scenery element
    sequence = 0;
    for (tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++)
    {
        CoordsXY offsetPos{ tile->x_offset, tile->y_offset };
        auto rotatedOffsetPos = offsetPos.Rotate(direction);

        CoordsXYZ tileLoc = { sceneryOrigin->x + rotatedOffsetPos.x, sceneryOrigin->y + rotatedOffsetPos.y,
                              sceneryOrigin->z + tile->z_offset };
        auto largeElement = map_get_large_scenery_segment({ tileLoc, static_cast<Direction>(direction) }, sequence);
        if (largeElement != nullptr)
        {
            if (sequence == 0)
            {
                uint8_t flags = largeElement->GetDirection();
                uint8_t primaryColour = largeElement->GetPrimaryColour();
                uint8_t secondaryColour = largeElement->GetSecondaryColour();

                track_design_save_push_tile_element_desc(entry, tileLoc, flags, primaryColour, secondaryColour);
            }
            track_design_save_push_tile_element({ tileLoc.x, tileLoc.y }, reinterpret_cast<TileElement*>(largeElement));
        }
    }
}

static void track_design_save_add_wall(const CoordsXY& loc, WallElement* wallElement)
{
    int32_t entryType = wallElement->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_WALLS, entryType);

    uint8_t flags = 0;
    flags |= wallElement->GetDirection();
    flags |= wallElement->GetTertiaryColour() << 2;

    uint8_t secondaryColour = wallElement->GetSecondaryColour();
    uint8_t primaryColour = wallElement->GetPrimaryColour();

    track_design_save_push_tile_element(loc, reinterpret_cast<TileElement*>(wallElement));
    track_design_save_push_tile_element_desc(
        entry, { loc.x, loc.y, wallElement->GetBaseZ() }, flags, primaryColour, secondaryColour);
}

static void track_design_save_add_footpath(const CoordsXY& loc, PathElement* pathElement)
{
    int32_t entryType = pathElement->GetSurfaceEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_PATHS, entryType);

    uint8_t flags = 0;
    flags |= pathElement->GetEdges();
    flags |= (pathElement->GetSlopeDirection()) << 5;
    if (pathElement->IsSloped())
        flags |= 0b00010000;
    if (pathElement->IsQueue())
        flags |= 1 << 7;

    track_design_save_push_tile_element(loc, reinterpret_cast<TileElement*>(pathElement));
    track_design_save_push_tile_element_desc(entry, { loc.x, loc.y, pathElement->GetBaseZ() }, flags, 0, 0);
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static bool track_design_save_add_tile_element(int32_t interactionType, const CoordsXY& loc, TileElement* tileElement)
{
    if (!track_design_save_can_add_tile_element(tileElement))
    {
        return false;
    }

    switch (interactionType)
    {
        case VIEWPORT_INTERACTION_ITEM_SCENERY:
            track_design_save_add_scenery(loc, tileElement->AsSmallScenery());
            return true;
        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
            track_design_save_add_large_scenery(loc, tileElement->AsLargeScenery());
            return true;
        case VIEWPORT_INTERACTION_ITEM_WALL:
            track_design_save_add_wall(loc, tileElement->AsWall());
            return true;
        case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
            track_design_save_add_footpath(loc, tileElement->AsPath());
            return true;
        default:
            return false;
    }
}

/**
 *
 *  rct2: 0x006D2F78
 */
static void track_design_save_pop_tile_element(const CoordsXY& loc, TileElement* tileElement)
{
    map_invalidate_tile_full(loc);

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
static void track_design_save_pop_tile_element_desc(const rct_object_entry* entry, const CoordsXYZ& loc, uint8_t flags)
{
    size_t removeIndex = SIZE_MAX;
    auto tileLoc = TileCoordsXYZ(loc);
    for (size_t i = 0; i < _trackSavedTileElementsDesc.size(); i++)
    {
        TrackDesignSceneryElement* item = &_trackSavedTileElementsDesc[i];
        if (item->x != tileLoc.x)
            continue;
        if (item->y != tileLoc.y)
            continue;
        if (item->z != tileLoc.z)
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

static void track_design_save_remove_scenery(const CoordsXY& loc, SmallSceneryElement* sceneryElement)
{
    int32_t entryType = sceneryElement->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_SMALL_SCENERY, entryType);

    uint8_t flags = 0;
    flags |= sceneryElement->GetDirection();
    flags |= sceneryElement->GetSceneryQuadrant() << 2;

    track_design_save_pop_tile_element(loc, reinterpret_cast<TileElement*>(sceneryElement));
    track_design_save_pop_tile_element_desc(entry, { loc.x, loc.y, sceneryElement->GetBaseZ() }, flags);
}

static void track_design_save_remove_large_scenery(const CoordsXY& loc, LargeSceneryElement* tileElement)
{
    rct_large_scenery_tile *sceneryTiles, *tile;
    int32_t direction, sequence;

    if (tileElement == nullptr)
    {
        log_warning("Null tile element");
        return;
    }

    int32_t entryType = tileElement->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_LARGE_SCENERY, entryType);
    sceneryTiles = get_large_scenery_entry(entryType)->large_scenery.tiles;

    int32_t z = tileElement->base_height;
    direction = tileElement->GetDirection();
    sequence = tileElement->GetSequenceIndex();

    auto sceneryOrigin = map_large_scenery_get_origin(
        { loc.x, loc.y, z << 3, static_cast<Direction>(direction) }, sequence, nullptr);
    if (!sceneryOrigin)
    {
        return;
    }

    // Iterate through each tile of the large scenery element
    sequence = 0;
    for (tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++)
    {
        CoordsXY offsetPos{ tile->x_offset, tile->y_offset };
        auto rotatedOffsetPos = offsetPos.Rotate(direction);

        CoordsXYZ tileLoc = { sceneryOrigin->x + rotatedOffsetPos.x, sceneryOrigin->y + rotatedOffsetPos.y,
                              sceneryOrigin->z + tile->z_offset };
        auto largeElement = map_get_large_scenery_segment({ tileLoc, static_cast<Direction>(direction) }, sequence);
        if (largeElement != nullptr)
        {
            if (sequence == 0)
            {
                uint8_t flags = largeElement->GetDirection();
                track_design_save_pop_tile_element_desc(entry, tileLoc, flags);
            }
            track_design_save_pop_tile_element({ tileLoc.x, tileLoc.y }, reinterpret_cast<TileElement*>(largeElement));
        }
    }
}

static void track_design_save_remove_wall(const CoordsXY& loc, WallElement* wallElement)
{
    int32_t entryType = wallElement->GetEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_WALLS, entryType);

    uint8_t flags = 0;
    flags |= wallElement->GetDirection();
    flags |= wallElement->GetTertiaryColour() << 2;

    track_design_save_pop_tile_element(loc, reinterpret_cast<TileElement*>(wallElement));
    track_design_save_pop_tile_element_desc(entry, { loc.x, loc.y, wallElement->GetBaseZ() }, flags);
}

static void track_design_save_remove_footpath(const CoordsXY& loc, PathElement* pathElement)
{
    int32_t entryType = pathElement->GetSurfaceEntryIndex();
    auto entry = object_entry_get_entry(OBJECT_TYPE_PATHS, entryType);

    uint8_t flags = 0;
    flags |= pathElement->GetEdges();
    if (pathElement->IsSloped())
        flags |= (1 << 4);
    flags |= (pathElement->GetSlopeDirection()) << 5;
    if (pathElement->IsQueue())
        flags |= (1 << 7);

    track_design_save_pop_tile_element(loc, reinterpret_cast<TileElement*>(pathElement));
    track_design_save_pop_tile_element_desc(entry, { loc.x, loc.y, pathElement->GetBaseZ() }, flags);
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static void track_design_save_remove_tile_element(int32_t interactionType, const CoordsXY& loc, TileElement* tileElement)
{
    switch (interactionType)
    {
        case VIEWPORT_INTERACTION_ITEM_SCENERY:
            track_design_save_remove_scenery(loc, tileElement->AsSmallScenery());
            break;
        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
            track_design_save_remove_large_scenery(loc, tileElement->AsLargeScenery());
            break;
        case VIEWPORT_INTERACTION_ITEM_WALL:
            track_design_save_remove_wall(loc, tileElement->AsWall());
            break;
        case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
            track_design_save_remove_footpath(loc, tileElement->AsPath());
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
            tileElement = map_get_first_element_at(TileCoordsXY{ x, y }.ToCoordsXY());
            if (tileElement == nullptr)
                continue;
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
                        track_design_save_add_tile_element(interactionType, TileCoordsXY(x, y).ToCoordsXY(), tileElement);
                    }
                }
            } while (!(tileElement++)->IsLastForTile());
        }
    }
}
