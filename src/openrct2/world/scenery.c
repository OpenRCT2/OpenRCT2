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
#include "../network/network.h"
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

rct_tile_element *gSceneryTileElement;
uint8 gSceneryTileElementType;

money32 gSceneryPlaceCost;
sint16 gSceneryPlaceObject;
sint16 gSceneryPlaceZ;
uint8 gSceneryPlacePathType;
uint8 gSceneryPlacePathSlope;
uint8 gSceneryPlaceRotation;

uint8 gSceneryGhostType;
LocationXYZ16 gSceneryGhostPosition;
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
const LocationXY8 ScenerySubTileOffsets[] = {
    {  7,  7 },
    {  7, 23 },
    { 23, 23 },
    { 23,  7 }
};

void scenery_increase_age(sint32 x, sint32 y, rct_tile_element *tileElement);

void scenery_update_tile(sint32 x, sint32 y)
{
    rct_tile_element *tileElement;

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tile_element_is_ghost(tileElement))
                continue;
        }

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_SMALL_SCENERY) {
            scenery_update_age(x, y, tileElement);
        } else if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_PATH) {
            if (footpath_element_has_path_scenery(tileElement) && !footpath_element_path_scenery_is_ghost(tileElement)) {
                rct_scenery_entry *sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tileElement));
                if (sceneryEntry != NULL) {
                    if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER) {
                        jumping_fountain_begin(JUMPING_FOUNTAIN_TYPE_WATER, x, y, tileElement);
                    }
                    else if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW) {
                        jumping_fountain_begin(JUMPING_FOUNTAIN_TYPE_SNOW, x, y, tileElement);
                    }
                }
            }
        }
    } while (!tile_element_is_last_for_tile(tileElement++));
}

/**
 *
 *  rct2: 0x006E33D9
 */
void scenery_update_age(sint32 x, sint32 y, rct_tile_element *tileElement)
{
    rct_tile_element *tileElementAbove;
    rct_scenery_entry *sceneryEntry;

    sceneryEntry = get_small_scenery_entry(tileElement->properties.scenery.type);
    if (sceneryEntry == NULL)
    {
        return;
    }

    if (gCheatsDisablePlantAging &&
        (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_CAN_BE_WATERED)) {
        return;
    }

    if (
        !(sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_CAN_BE_WATERED) ||
        (gClimateCurrentWeather < WEATHER_RAIN) ||
        (tileElement->properties.scenery.age < 5)
    ) {
        scenery_increase_age(x, y, tileElement);
        return;
    }

    // Check map elements above, presumably to see if map element is blocked from rain
    tileElementAbove = tileElement;
    while (!(tileElementAbove->flags & 7)) {

        tileElementAbove++;

        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (tile_element_is_ghost(tileElementAbove))
            continue;

        switch (tile_element_get_type(tileElementAbove)) {
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        case TILE_ELEMENT_TYPE_ENTRANCE:
        case TILE_ELEMENT_TYPE_PATH:
            map_invalidate_tile_zoom1(x, y, tileElementAbove->base_height * 8, tileElementAbove->clearance_height * 8);
            scenery_increase_age(x, y, tileElement);
            return;
        case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            sceneryEntry = get_small_scenery_entry(tileElementAbove->properties.scenery.type);
            if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_VOFFSET_CENTRE) {
                scenery_increase_age(x, y, tileElement);
                return;
            }
            break;
        }
    }

    // Reset age / water plant
    tileElement->properties.scenery.age = 0;
    map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
}

void scenery_increase_age(sint32 x, sint32 y, rct_tile_element *tileElement)
{
    if (tileElement->flags & SMALL_SCENERY_FLAG_ANIMATED)
        return;

    if (tileElement->properties.scenery.age < 255) {
        tileElement->properties.scenery.age++;
        map_invalidate_tile_zoom1(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
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

    if (gSceneryGhostType & SCENERY_ENTRY_FLAG_0){
        gSceneryGhostType &= ~SCENERY_ENTRY_FLAG_0;
        game_do_command(
            x,
            105 | (gSceneryTileElementType << 8),
            y,
            z | (gSceneryPlaceObject << 8),
            GAME_COMMAND_REMOVE_SCENERY,
            0,
            0);
    }

    if (gSceneryGhostType & SCENERY_ENTRY_FLAG_1)
    {
        gSceneryGhostType &= ~SCENERY_ENTRY_FLAG_1;
        rct_tile_element * tile_element = map_get_first_element_at(x / 32, y / 32);

        do
        {
            if (tile_element_get_type(tile_element) != TILE_ELEMENT_TYPE_PATH)
                continue;

            if (tile_element->base_height != z)
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
        } while (!tile_element_is_last_for_tile(tile_element++));
    }

    if (gSceneryGhostType & SCENERY_ENTRY_FLAG_2){
        gSceneryGhostType &= ~SCENERY_ENTRY_FLAG_2;
        game_do_command(
            x,
            105 | (gSceneryTileElementType << 8),
            y,
            gSceneryGhostWallRotation |(z << 8),
            GAME_COMMAND_REMOVE_WALL,
            0,
            0);
    }

    if (gSceneryGhostType & SCENERY_ENTRY_FLAG_3){
        gSceneryGhostType &= ~SCENERY_ENTRY_FLAG_3;
        game_do_command(
            x,
            105 | (gSceneryPlaceRotation << 8),
            y,
            z,
            GAME_COMMAND_REMOVE_LARGE_SCENERY,
            0,
            0);
    }

    if (gSceneryGhostType & SCENERY_ENTRY_FLAG_4){
        gSceneryGhostType &= ~SCENERY_ENTRY_FLAG_4;
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

rct_scenery_group_entry *get_scenery_group_entry(sint32 entryIndex)
{
    if (entryIndex >= object_entry_group_counts[OBJECT_TYPE_SCENERY_GROUP]) {
        return NULL;
    }
    return (rct_scenery_group_entry*)gSceneryGroupEntries[entryIndex];
}

sint32 get_scenery_id_from_entry_index(uint8 objectType, sint32 entryIndex)
{
    switch (objectType) {
    case OBJECT_TYPE_SMALL_SCENERY: return entryIndex + SCENERY_SMALL_SCENERY_ID_MIN;
    case OBJECT_TYPE_PATH_BITS:     return entryIndex + SCENERY_PATH_SCENERY_ID_MIN;
    case OBJECT_TYPE_WALLS:         return entryIndex + SCENERY_WALLS_ID_MIN;
    case OBJECT_TYPE_LARGE_SCENERY: return entryIndex + SCENERY_LARGE_SCENERY_ID_MIN;
    case OBJECT_TYPE_BANNERS:       return entryIndex + SCENERY_BANNERS_ID_MIN;
    default:                        return -1;
    }
}

sint32 scenery_small_get_primary_colour(const rct_tile_element * tileElement)
{
    return tileElement->properties.scenery.colour_1 & TILE_ELEMENT_COLOUR_MASK;
}

sint32 scenery_small_get_secondary_colour(const rct_tile_element * tileElement)
{
    return tileElement->properties.scenery.colour_2 & TILE_ELEMENT_COLOUR_MASK;
}

void scenery_small_set_primary_colour(rct_tile_element * tileElement, uint32 colour)
{
    assert(colour <= 31);
    tileElement->properties.scenery.colour_1 &= ~TILE_ELEMENT_COLOUR_MASK;
    tileElement->properties.scenery.colour_1 |= colour;
}

void scenery_small_set_secondary_colour(rct_tile_element * tileElement, uint32 colour)
{
    assert(colour <= 31);
    tileElement->properties.scenery.colour_2 &= ~TILE_ELEMENT_COLOUR_MASK;
    tileElement->properties.scenery.colour_2 |= colour;
}

bool scenery_small_get_supports_needed(const rct_tile_element * tileElement)
{
    return (bool)(tileElement->properties.scenery.colour_1 & MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS);
}

void scenery_small_set_supports_needed(rct_tile_element * tileElement)
{
    tileElement->properties.scenery.colour_1 |= MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS;
}

colour_t scenery_large_get_primary_colour(const rct_tile_element * tileElement)
{
    return tileElement->properties.scenerymultiple.colour[0] & TILE_ELEMENT_COLOUR_MASK;
}

colour_t scenery_large_get_secondary_colour(const rct_tile_element * tileElement)
{
    return tileElement->properties.scenerymultiple.colour[1] & TILE_ELEMENT_COLOUR_MASK;
}

void scenery_large_set_primary_colour(rct_tile_element * tileElement, colour_t colour)
{
    assert(colour <= 31);
    tileElement->properties.scenerymultiple.colour[0] &= ~TILE_ELEMENT_COLOUR_MASK;
    tileElement->properties.scenerymultiple.colour[0] |= colour;
}

void scenery_large_set_secondary_colour(rct_tile_element * tileElement, colour_t colour)
{
    assert(colour <= 31);
    tileElement->properties.scenerymultiple.colour[1] &= ~TILE_ELEMENT_COLOUR_MASK;
    tileElement->properties.scenerymultiple.colour[1] |= colour;
}

sint32 scenery_large_get_banner_id(rct_tile_element * tileElement)
{
    return (tileElement->type & 0xC0) |
        (((tileElement->properties.scenerymultiple.colour[0]) &~ TILE_ELEMENT_COLOUR_MASK) >> 2) |
        (((tileElement->properties.scenerymultiple.colour[1]) &~ TILE_ELEMENT_COLOUR_MASK) >> 5);
}

void scenery_large_set_banner_id(rct_tile_element * tileElement, uint8 bannerIndex)
{
    tileElement->type |= bannerIndex & 0xC0;
    tileElement->properties.scenerymultiple.colour[0] |= (bannerIndex & 0x38) << 2;
    tileElement->properties.scenerymultiple.colour[1] |= (bannerIndex & 7) << 5;
}

sint32 scenery_large_get_type(rct_tile_element * tileElement)
{
    return (tileElement->properties.scenerymultiple.type & TILE_ELEMENT_LARGE_TYPE_MASK);
}

sint32 scenery_large_get_sequence(rct_tile_element * tileElement)
{
    return (tileElement->properties.scenerymultiple.type >> 10);
}

void scenery_large_set_type(rct_tile_element * tileElement, uint16 type)
{
    tileElement->properties.scenerymultiple.type &= ~TILE_ELEMENT_LARGE_TYPE_MASK;
    tileElement->properties.scenerymultiple.type |= (type & TILE_ELEMENT_LARGE_TYPE_MASK);
}

void scenery_large_set_sequence(rct_tile_element * tileElement, uint16 sequence)
{
    tileElement->properties.scenerymultiple.type &= TILE_ELEMENT_LARGE_TYPE_MASK;
    tileElement->properties.scenerymultiple.type |= (sequence << 10);
}
