/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Cheats.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../OpenRCT2.h"
#include "../ride/TrackDesign.h"
#include "Footpath.h"
#include "Map.h"
#include "Park.h"
#include "Scenery.h"
#include "SmallScenery.h"
#include "MapAnimation.h"
#include "Surface.h"

static money32 SmallSceneryRemove(int16_t x, int16_t y, uint8_t baseHeight, uint8_t quadrant, uint8_t sceneryType, uint8_t flags)
{
    if (!map_is_location_valid({x, y}))
    {
        return MONEY32_UNDEFINED;
    }
    money32 cost;

    rct_scenery_entry *entry = get_small_scenery_entry(sceneryType);
    if (entry == nullptr)
    {
        log_warning("Invalid game command for scenery removal, scenery_type = %u", sceneryType);
        return MONEY32_UNDEFINED;
    }
    cost = entry->small_scenery.removal_price * 10;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = baseHeight * 8;

    if (!(flags & GAME_COMMAND_FLAG_GHOST) && 
        game_is_paused() && 
        !gCheatsBuildInPauseMode)
    {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        return MONEY32_UNDEFINED;
    }

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && 
        !(flags & GAME_COMMAND_FLAG_GHOST) && 
        !gCheatsSandboxMode)
    {
        // Check if allowed to remove item
        if (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL)
        {
            if (entry->small_scenery.height > 64)
            {
                gGameCommandErrorText = STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY;
                return MONEY32_UNDEFINED;
            }
        }

        // Check if the land is owned
        if (!map_is_location_owned(x, y, gCommandPosition.z))
        {
            return MONEY32_UNDEFINED;
        }
    }

    bool sceneryFound = false;
    rct_tile_element* tileElement = map_get_first_element_at(x / 32, y / 32);
    do {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
            continue;
        if ((tileElement->type >> 6) != quadrant)
            continue;
        if (tileElement->base_height != baseHeight)
            continue;
        if (tileElement->properties.scenery.type != sceneryType)
            continue;
        if ((flags & GAME_COMMAND_FLAG_GHOST) && 
            !(tileElement->flags & TILE_ELEMENT_FLAG_GHOST))
            continue;

        sceneryFound = true;
        break;
    } while (!(tileElement++)->IsLastForTile());

    if (!sceneryFound)
    {
        return 0;
    }

    // Remove element
    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST))
        {
            LocationXYZ16 coord;
            coord.x = x + 16;
            coord.y = y + 16;
            coord.z = tile_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        map_invalidate_tile_full(x, y);
        tile_element_remove(tileElement);
    }
    return (gParkFlags & PARK_FLAGS_NO_MONEY) ? 0 : cost;
}

static money32 SmallScenerySetColour(int16_t x, int16_t y, uint8_t baseHeight, uint8_t quadrant, uint8_t sceneryType, uint8_t primaryColour, uint8_t secondaryColour, uint8_t flags)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    int32_t z = baseHeight * 8;
    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
    {
        if (!map_is_location_owned(x, y, z))
        {
            return MONEY32_UNDEFINED;
        }
    }

    rct_tile_element *tileElement = map_get_small_scenery_element_at(x, y, baseHeight, sceneryType, quadrant);

    if (tileElement == nullptr)
    {
        return 0;
    }

    if ((flags & GAME_COMMAND_FLAG_GHOST) && !(tileElement->flags & TILE_ELEMENT_FLAG_GHOST))
    {
        return 0;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        scenery_small_set_primary_colour(tileElement, primaryColour);
        scenery_small_set_secondary_colour(tileElement, secondaryColour);

        map_invalidate_tile_full(x, y);
    }

    return 0;
}

static money32 SmallSceneryPlace(int16_t x,
    int16_t y,
    int16_t targetHeight,
    uint8_t quadrant,
    uint8_t rotation,
    uint8_t sceneryType,
    uint8_t primaryColour,
    uint8_t secondaryColour,
    uint8_t flags)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;

    money32 clearCost = 0;
    bool isOnWater = false;
    bool supportsRequired = false;
    if (targetHeight != 0)
    {
        supportsRequired = true;
    }
    int32_t baseHeight = tile_element_height(x, y);
    // If on water
    if (baseHeight & 0xFFFF0000)
    {
        baseHeight >>= 16;
    }
    gCommandPosition.x = x;
    gCommandPosition.y = y;
    gCommandPosition.z = baseHeight;
    if (targetHeight != 0)
    {
        baseHeight = targetHeight;
        gCommandPosition.z = baseHeight;
    }
    gCommandPosition.x += 16;
    gCommandPosition.y += 16;

    if (game_is_paused() && !gCheatsBuildInPauseMode)
    {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        return MONEY32_UNDEFINED;
    }

    if (!map_check_free_elements_and_reorganise(1))
    {
        return MONEY32_UNDEFINED;
    }

    if (!byte_9D8150 && (x > gMapSizeMaxXY || y > gMapSizeMaxXY))
    {
        return MONEY32_UNDEFINED;
    }

    rct_scenery_entry* sceneryEntry = get_small_scenery_entry(sceneryType);
    if (sceneryEntry == nullptr)
    {
        return MONEY32_UNDEFINED;
    }

    if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE) ||
        !scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_DIAGONAL))
    {
        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_DIAGONAL | SMALL_SCENERY_FLAG_HALF_SPACE | SMALL_SCENERY_FLAG_THREE_QUARTERS))
        {
            quadrant = 0;
        }
    }

    // Check if sub tile height is any different compared to actual surface tile height
    int32_t x2 = x;
    int32_t y2 = y;
    if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE))
    {
        x2 += 16;
        y2 += 16;
    }
    else
    {
        x2 += ScenerySubTileOffsets[quadrant & 3].x - 1;
        y2 += ScenerySubTileOffsets[quadrant & 3].y - 1;
    }
    baseHeight = tile_element_height(x2, y2);
    // If on water
    if (baseHeight & 0xFFFF0000)
    {
        // base_height2 is now the water height
        baseHeight >>= 16;
        if (targetHeight == 0)
        {
            isOnWater = true;
        }
    }
    if (targetHeight == 0)
    {
        targetHeight = baseHeight;
    }

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) &&
        !gCheatsSandboxMode &&
        !map_is_location_owned(x, y, targetHeight))
    {

        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY && !(flags & GAME_COMMAND_FLAG_GHOST))
    {
        footpath_remove_litter(x, y, targetHeight);
        if (!gCheatsDisableClearanceChecks && (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_NO_WALLS)))
        {
            wall_remove_at(x, y, targetHeight, targetHeight + sceneryEntry->small_scenery.height);
        }
    }

    rct_tile_element* surfaceElement = map_get_surface_element_at({x, y});

    if (surfaceElement != nullptr && !gCheatsDisableClearanceChecks && surface_get_water_height(surfaceElement) > 0)
    {
        int32_t water_height = (surface_get_water_height(surfaceElement) * 16) - 1;
        if (water_height > targetHeight)
        {
            gGameCommandErrorText = STR_CANT_BUILD_THIS_UNDERWATER;
            return MONEY32_UNDEFINED;
        }
    }

    if (!gCheatsDisableClearanceChecks && !(scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_STACKABLE)))
    {
        if (isOnWater)
        {
            gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_LAND;
            return MONEY32_UNDEFINED;
        }

        if (surfaceElement != nullptr && surface_get_water_height(surfaceElement) > 0)
        {
            if ((surface_get_water_height(surfaceElement) * 16) > targetHeight)
            {
                gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_LAND;
                return MONEY32_UNDEFINED;
            }
        }
    }

    if (!gCheatsDisableClearanceChecks &&
        (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_REQUIRE_FLAT_SURFACE)) &&
        !supportsRequired &&
        !isOnWater &&
        surfaceElement != nullptr &&
        (surfaceElement->properties.surface.slope & TILE_ELEMENT_SURFACE_SLOPE_MASK))
    {

        gGameCommandErrorText = STR_LEVEL_LAND_REQUIRED;
        return MONEY32_UNDEFINED;
    }

    if (!gCheatsDisableSupportLimits &&
        !(scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_STACKABLE)) &&
        supportsRequired)
    {

        if (!isOnWater)
        {
            if (surfaceElement != nullptr)
            {
                if (surface_get_water_height(surfaceElement) ||
                    (surfaceElement->base_height * 8) != targetHeight)
                {

                    gGameCommandErrorText = STR_LEVEL_LAND_REQUIRED;
                    return MONEY32_UNDEFINED;
                }
            }

        }
        else
        {
            gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_LAND;
            return MONEY32_UNDEFINED;
        }
    }

    int32_t zLow = targetHeight / 8;
    int32_t zHigh = zLow + ceil2(sceneryEntry->small_scenery.height, 8) / 8;
    uint8_t collisionQuadrants = 0xF;
    uint8_t blSupports = 0;
    if (!(scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE)))
    {
        collisionQuadrants = 1 << (quadrant ^ 2);
    }
    if (!(scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HALF_SPACE)))
    {
        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_DIAGONAL) &&
            scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE))
        {
            if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_THREE_QUARTERS))
            {
                collisionQuadrants = 0xF & rol8(0xBB, ((quadrant ^ 2) + rotation) & 3);
            }
            else
            {
                collisionQuadrants = 0xA >> ((quadrant + rotation) & 1);
            }
        }
    }
    else
    {
        collisionQuadrants = 0xF & rol8(0x33, ((quadrant ^ 2) + rotation) & 3);
    }
    if (!supportsRequired)
    {
        blSupports |= 0xF0;
    }

    if (!gCheatsDisableClearanceChecks &&
        !map_can_construct_with_clear_at(
            x,
            y,
            zLow,
            zHigh,
            &map_place_scenery_clear_func,
            blSupports | collisionQuadrants,
            flags,
            &clearCost,
            CREATE_CROSSING_MODE_NONE))
    {
        return MONEY32_UNDEFINED;
    }

    gSceneryGroundFlags = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);

    money32 cost = (sceneryEntry->small_scenery.price * 10) + clearCost;
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        cost = 0;
    }

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
    {
        return cost;
    }

    if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST))
    {
        LocationXYZ16 coord;
        coord.x = x + 16;
        coord.y = y + 16;
        coord.z = tile_element_height(coord.x, coord.y);
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
    }

    rct_tile_element* newElement = tile_element_insert(x / 32, y / 32, zLow, collisionQuadrants);
    assert(newElement != nullptr);
    gSceneryTileElement = newElement;
    uint8_t type = quadrant << 6;
    type |= TILE_ELEMENT_TYPE_SMALL_SCENERY;
    type |= rotation;
    newElement->type = type;
    newElement->properties.scenery.type = sceneryType;
    newElement->properties.scenery.age = 0;
    scenery_small_set_primary_colour(newElement, primaryColour);
    scenery_small_set_secondary_colour(newElement, secondaryColour);
    newElement->clearance_height = newElement->base_height + ((sceneryEntry->small_scenery.height + 7) / 8);

    if (supportsRequired)
    {
        scenery_small_set_supports_needed(newElement);
    }

    if (flags & GAME_COMMAND_FLAG_GHOST)
    {
        newElement->flags |= TILE_ELEMENT_FLAG_GHOST;
    }

    map_invalidate_tile_full(x, y);
    if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_ANIMATED))
    {
        map_animation_create(2, x, y, newElement->base_height);
    }

    return cost;
}

/**
 *
 *  rct2: 0x006E0E01
 */
void game_command_remove_scenery(
    int32_t *                  eax,
    int32_t *                  ebx,
    int32_t *                  ecx,
    int32_t *                  edx,
    [[maybe_unused]] int32_t * esi,
    [[maybe_unused]] int32_t * edi,
    [[maybe_unused]] int32_t * ebp)
{
    *ebx = SmallSceneryRemove(
        *eax & 0xFFFF,
        *ecx & 0xFFFF,
        *edx & 0xFF,
        ((*ebx >> 8) & 0xFF) >> 6,
        (*edx >> 8) & 0xFF,
        *ebx & 0xFF
    );
}

/**
 *
 *  rct2: 0x006E0F26
 */
void game_command_set_scenery_colour(
    int32_t *                  eax,
    int32_t *                  ebx,
    int32_t *                  ecx,
    int32_t *                  edx,
    [[maybe_unused]] int32_t * esi,
    [[maybe_unused]] int32_t * edi,
    int32_t *                  ebp)
{
    *ebx = SmallScenerySetColour(
        *eax & 0xFFFF,
        *ecx & 0xFFFF,
        *edx & 0xFF,
        ((*ebx >> 8) & 0xFF) >> 6,
        (*edx >> 8) & 0xFF,
        *ebp & 0xFF,
        (*ebp >> 8) & 0xFF,
        *ebx & 0xFF
    );
}

/**
 *
 *  rct2: 0x006E0D6E, 0x006B8D88
 */
int32_t map_place_scenery_clear_func(rct_tile_element** tile_element, int32_t x, int32_t y, uint8_t flags, money32* price)
{
    if ((*tile_element)->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
        return 1;

    if (!(flags & GAME_COMMAND_FLAG_PATH_SCENERY))
        return 1;

    rct_scenery_entry* scenery = get_small_scenery_entry((*tile_element)->properties.scenery.type);

    if (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL)
    {
        if (scenery->small_scenery.height > 64)
            return 1;
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        *price += scenery->small_scenery.removal_price * 10;

    if (flags & GAME_COMMAND_FLAG_GHOST)
        return 0;

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
        return 0;

    map_invalidate_tile(x, y, (*tile_element)->base_height * 8, (*tile_element)->clearance_height * 8);

    tile_element_remove(*tile_element);

    (*tile_element)--;
    return 0;
}

/**
 *
 *  rct2: 0x006C5A4F, 0x006CDE57, 0x006A6733, 0x0066637E
 */
int32_t map_place_non_scenery_clear_func(rct_tile_element** tile_element, int32_t x, int32_t y, uint8_t flags, money32* price)
{
    if ((*tile_element)->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
        return 1;

    rct_scenery_entry* scenery = get_small_scenery_entry((*tile_element)->properties.scenery.type);

    if (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL)
    {
        if (scenery->small_scenery.height > 64)
            return 1;
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        *price += scenery->small_scenery.removal_price * 10;

    if (flags & GAME_COMMAND_FLAG_GHOST)
        return 0;

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
        return 0;

    map_invalidate_tile(x, y, (*tile_element)->base_height * 8, (*tile_element)->clearance_height * 8);

    tile_element_remove(*tile_element);

    (*tile_element)--;
    return 0;
}

/**
 *
 *  rct2: 0x006E08F4
 */
void game_command_place_scenery(
    int32_t * eax, int32_t * ebx, int32_t * ecx, int32_t * edx, [[maybe_unused]] int32_t * esi, int32_t * edi, int32_t * ebp)
{
    *ebx = SmallSceneryPlace(
        *eax & 0xFFFF,
        *ecx & 0xFFFF,
        *ebp & 0xFFFF,
        *edx & 0xFF,
        *edi & 0xFF,
        (*ebx >> 8) & 0xFF,
        (*edx >> 8) & 0xFF,
        (*edi >> 16) & 0xFF,
        *ebx & 0xFF
    );
}

int32_t scenery_small_get_primary_colour(const rct_tile_element * tileElement)
{
    return tileElement->properties.scenery.colour_1 & TILE_ELEMENT_COLOUR_MASK;
}

int32_t scenery_small_get_secondary_colour(const rct_tile_element * tileElement)
{
    return tileElement->properties.scenery.colour_2 & TILE_ELEMENT_COLOUR_MASK;
}

void scenery_small_set_primary_colour(rct_tile_element * tileElement, uint32_t colour)
{
    assert(colour <= 31);
    tileElement->properties.scenery.colour_1 &= ~TILE_ELEMENT_COLOUR_MASK;
    tileElement->properties.scenery.colour_1 |= colour;
}

void scenery_small_set_secondary_colour(rct_tile_element * tileElement, uint32_t colour)
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

bool scenery_small_entry_has_flag(const rct_scenery_entry * sceneryEntry, uint32_t flags)
{
    return (bool)(sceneryEntry->small_scenery.flags & flags);
}

