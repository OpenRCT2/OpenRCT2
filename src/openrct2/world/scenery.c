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

#include "../game.h"
#include "../common.h"
#include "../localisation/localisation.h"
#include "../scenario/scenario.h"
#include "../cheats.h"
#include "../object_list.h"
#include "Climate.h"
#include "Fountain.h"
#include "map.h"
#include "park.h"
#include "scenery.h"
#include "footpath.h"

uint8 gWindowSceneryActiveTabIndex;
uint16 gWindowSceneryTabSelections[20];
uint8 gWindowSceneryClusterEnabled;
uint8 gWindowSceneryPaintEnabled;
uint8 gWindowSceneryRotation;
colour_t gWindowSceneryPrimaryColour;
colour_t gWindowScenerySecondaryColour;
colour_t gWindowSceneryTertiaryColour;
bool gWindowSceneryEyedropperEnabled;

rct_map_element *gSceneryMapElement;
uint8 gSceneryMapElementType;

money32 gSceneryPlaceCost;
sint16 gSceneryPlaceObject;
sint16 gSceneryPlaceZ;
uint8 gSceneryPlacePathType;
uint8 gSceneryPlacePathSlope;
uint8 gSceneryPlaceRotation;

uint8 gSceneryGhostType;
rct_xyz16 gSceneryGhostPosition;
uint32 gSceneryGhostPathObjectType;
uint8 gSceneryGhostWallRotation;

sint16 gSceneryShiftPressed;
sint16 gSceneryShiftPressX;
sint16 gSceneryShiftPressY;
sint16 gSceneryShiftPressZOffset;

sint16 gSceneryCtrlPressed;
sint16 gSceneryCtrlPressZ;

uint8 gSceneryGroundFlags;

money32 gClearSceneryCost;

// rct2: 0x009A3E74
const rct_xy8 ScenerySubTileOffsets[] = {
    {  7,  7 },
    {  7, 23 },
    { 23, 23 },
    { 23,  7 }
};

void scenery_increase_age(sint32 x, sint32 y, rct_map_element *mapElement);

void scenery_update_tile(sint32 x, sint32 y)
{
    rct_map_element *mapElement;

    mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SCENERY) {
            scenery_update_age(x, y, mapElement);
        } else if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH) {
            if (footpath_element_has_path_scenery(mapElement) && !footpath_element_path_scenery_is_ghost(mapElement)) {
                rct_scenery_entry *sceneryEntry;
                sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(mapElement));
                if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER) {
                    jumping_fountain_begin(JUMPING_FOUNTAIN_TYPE_WATER, x, y, mapElement);
                } else if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW) {
                    jumping_fountain_begin(JUMPING_FOUNTAIN_TYPE_SNOW, x, y, mapElement);
                }
            }
        }
    } while (!map_element_is_last_for_tile(mapElement++));
}

/**
 *
 *  rct2: 0x006E33D9
 */
void scenery_update_age(sint32 x, sint32 y, rct_map_element *mapElement)
{
    rct_map_element *mapElementAbove;
    rct_scenery_entry *sceneryEntry;

    sceneryEntry = get_small_scenery_entry(mapElement->properties.scenery.type);
    if (gCheatsDisablePlantAging &&
        (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_CAN_BE_WATERED)) {
        return;
    }

    if (
        !(sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_CAN_BE_WATERED) ||
        (gClimateCurrentWeather < WEATHER_RAIN) ||
        (mapElement->properties.scenery.age < 5)
    ) {
        scenery_increase_age(x, y, mapElement);
        return;
    }

    // Check map elements above, presumably to see if map element is blocked from rain
    mapElementAbove = mapElement;
    while (!(mapElementAbove->flags & 7)) {
        mapElementAbove++;

        switch (map_element_get_type(mapElementAbove)) {
        case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
        case MAP_ELEMENT_TYPE_ENTRANCE:
        case MAP_ELEMENT_TYPE_PATH:
            map_invalidate_tile_zoom1(x, y, mapElementAbove->base_height * 8, mapElementAbove->clearance_height * 8);
            scenery_increase_age(x, y, mapElement);
            return;
        case MAP_ELEMENT_TYPE_SCENERY:
            sceneryEntry = get_small_scenery_entry(mapElementAbove->properties.scenery.type);
            if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_VOFFSET_CENTRE) {
                scenery_increase_age(x, y, mapElement);
                return;
            }
            break;
        }
    }

    // Reset age / water plant
    mapElement->properties.scenery.age = 0;
    map_invalidate_tile_zoom1(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
}

void scenery_increase_age(sint32 x, sint32 y, rct_map_element *mapElement)
{
    if (mapElement->flags & SMALL_SCENERY_FLAG_ANIMATED)
        return;

    if (mapElement->properties.scenery.age < 255) {
        mapElement->properties.scenery.age++;
        map_invalidate_tile_zoom1(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
    }
}

/**
 *
 *  rct2: 0x006E2712
 */
void scenery_remove_ghost_tool_placement(){
    sint16 x, y, z;

    x = gSceneryGhostPosition.x;
    y = gSceneryGhostPosition.y;
    z = gSceneryGhostPosition.z;

    if (gSceneryGhostType & (1 << 0)){
        gSceneryGhostType &= ~(1 << 0);
        game_do_command(
            x,
            105 | (gSceneryMapElementType << 8),
            y,
            z | (gSceneryPlaceObject << 8),
            GAME_COMMAND_REMOVE_SCENERY,
            0,
            0);
    }

    if (gSceneryGhostType & (1 << 1)){
        gSceneryGhostType &= ~(1 << 1);
        rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

        do{
            if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_PATH)
                continue;

            if (map_element->base_height != z)
                continue;

            game_do_command(
                x,
                233 | (gSceneryPlacePathSlope << 8),
                y,
                z | (gSceneryPlacePathType << 8),
                GAME_COMMAND_PLACE_PATH,
                gSceneryGhostPathObjectType & 0xFFFF0000,
                0);
            break;
        } while (!map_element_is_last_for_tile(map_element++));
    }

    if (gSceneryGhostType & (1 << 2)){
        gSceneryGhostType &= ~(1 << 2);
        game_do_command(
            x,
            105 | (gSceneryMapElementType << 8),
            y,
            gSceneryGhostWallRotation |(z << 8),
            GAME_COMMAND_REMOVE_WALL,
            0,
            0);
    }

    if (gSceneryGhostType & (1 << 3)){
        gSceneryGhostType &= ~(1 << 3);
        game_do_command(
            x,
            105 | (gSceneryPlaceRotation << 8),
            y,
            z,
            GAME_COMMAND_REMOVE_LARGE_SCENERY,
            0,
            0);
    }

    if (gSceneryGhostType & (1 << 4)){
        gSceneryGhostType &= ~(1 << 4);
        game_do_command(
            x,
            105,
            y,
            z | (gSceneryPlaceRotation << 8),
            GAME_COMMAND_REMOVE_BANNER,
            0,
            0);
    }
}

rct_scenery_entry *get_small_scenery_entry(sint32 entryIndex)
{
    if (entryIndex >= object_entry_group_counts[OBJECT_TYPE_SMALL_SCENERY]) {
        return NULL;
    }
    return (rct_scenery_entry*)gSmallSceneryEntries[entryIndex];
}

rct_scenery_entry *get_large_scenery_entry(sint32 entryIndex)
{
    if (entryIndex >= object_entry_group_counts[OBJECT_TYPE_LARGE_SCENERY]) {
        return NULL;
    }
    return (rct_scenery_entry*)gLargeSceneryEntries[entryIndex];
}

rct_scenery_entry *get_wall_entry(sint32 entryIndex)
{
    if (entryIndex >= object_entry_group_counts[OBJECT_TYPE_WALLS]) {
        return NULL;
    }
    return (rct_scenery_entry*)gWallSceneryEntries[entryIndex];
}

rct_scenery_entry *get_banner_entry(sint32 entryIndex)
{
    if (entryIndex >= object_entry_group_counts[OBJECT_TYPE_BANNERS]) {
        return NULL;
    }
    return (rct_scenery_entry*)gBannerSceneryEntries[entryIndex];
}

rct_scenery_entry *get_footpath_item_entry(sint32 entryIndex)
{
    if (entryIndex >= object_entry_group_counts[OBJECT_TYPE_PATH_BITS]) {
        return NULL;
    }
    return (rct_scenery_entry*)gFootpathAdditionEntries[entryIndex];
}

rct_scenery_set_entry *get_scenery_group_entry(sint32 entryIndex)
{
    if (entryIndex >= object_entry_group_counts[OBJECT_TYPE_SCENERY_SETS]) {
        return NULL;
    }
    return (rct_scenery_set_entry*)gSceneryGroupEntries[entryIndex];
}

sint32 get_scenery_id_from_entry_index(uint8 objectType, sint32 entryIndex)
{
    switch (objectType) {
    case OBJECT_TYPE_SMALL_SCENERY: return entryIndex;
    case OBJECT_TYPE_PATH_BITS:     return entryIndex + 0x100;
    case OBJECT_TYPE_WALLS:         return entryIndex + 0x200;
    case OBJECT_TYPE_LARGE_SCENERY: return entryIndex + 0x300;
    case OBJECT_TYPE_BANNERS:       return entryIndex + 0x400;
    default:                        return -1;
    }
}
