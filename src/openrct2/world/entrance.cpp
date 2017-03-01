#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../rct2.h"

extern "C"
{
    #include "entrance.h"
    #include "footpath.h"
    #include "map.h"
    #include "park.h"
    #include "../cheats.h"
    #include "../game.h"
    #include "../management/finance.h"
    #include "../localisation/string_ids.h"
}

bool gParkEntranceGhostExists;
rct_xyz16 gParkEntranceGhostPosition;
uint8 gParkEntranceGhostDirection;
money32 gParkEntranceGhostPrice;

rct_xyzd16 gParkEntrances[MAX_PARK_ENTRANCES];

static money32 ParkEntrancePlace(sint32 flags, sint16 x, sint16 y, uint8 z, uint8 direction) 
{
    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode) 
    {
        return MONEY32_UNDEFINED;
    }

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

    gCommandPosition.x = x;
    gCommandPosition.y = y;
    gCommandPosition.z = z * 16;

    if (!map_check_free_elements_and_reorganise(3)) 
    {
        return MONEY32_UNDEFINED;
    }

    if (x <= 32 || y <= 32 || x >= (gMapSizeUnits - 32) || y >= (gMapSizeUnits - 32)) 
    {
        gGameCommandErrorText = STR_TOO_CLOSE_TO_EDGE_OF_MAP;
        return MONEY32_UNDEFINED;
    }

    sint8 entranceNum = -1;
    for (uint8 i = 0; i < MAX_PARK_ENTRANCES; ++i) 
    {
        if (gParkEntrances[i].x == MAP_LOCATION_NULL) 
        {
            entranceNum = i;
            break;
        }
    }

    if (entranceNum == -1) 
    {
        gGameCommandErrorText = STR_ERR_TOO_MANY_PARK_ENTRANCES;
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY) 
    {
        gParkEntrances[entranceNum].x = x;
        gParkEntrances[entranceNum].y = y;
        gParkEntrances[entranceNum].z = z  * 16;
        gParkEntrances[entranceNum].direction = direction;
    }

    sint8 zLow = z * 2;
    sint8 zHigh = zLow + 12;

    for (uint8 index = 0; index < 3; index++) 
    {
        if (index == 1)
        {
            x += TileDirectionDelta[(direction - 1) & 0x3].x;
            y += TileDirectionDelta[(direction - 1) & 0x3].y;
        }
        else if (index == 2) 
        {
            x += TileDirectionDelta[(direction + 1) & 0x3].x * 2;
            y += TileDirectionDelta[(direction + 1) & 0x3].y * 2;
        }

        if (!gCheatsDisableClearanceChecks) 
        {
            if (!map_can_construct_at(x, y, zLow, zHigh, 0xF)) 
            {
                return MONEY32_UNDEFINED;
            }
        }

        // Check that entrance element does not already exist at this location
        rct_map_element* entranceElement = map_get_park_entrance_element_at(x, y, zLow, false);
        if (entranceElement != NULL)
        {
            return MONEY32_UNDEFINED;
        }

        if (!(flags & GAME_COMMAND_FLAG_APPLY)) 
        {
            continue;
        }

        if (!(flags & GAME_COMMAND_FLAG_GHOST)) 
        {
            rct_map_element* surfaceElement = map_get_surface_element_at(x / 32, y / 32);
            surfaceElement->properties.surface.ownership = 0;
        }

        rct_map_element* newElement = map_element_insert(x / 32, y / 32, zLow, 0xF);
        assert(newElement != NULL);
        newElement->clearance_height = zHigh;

        if (flags & GAME_COMMAND_FLAG_GHOST) 
        {
            newElement->flags |= MAP_ELEMENT_FLAG_GHOST;
        }

        newElement->type = MAP_ELEMENT_TYPE_ENTRANCE;
        newElement->type |= direction;
        newElement->properties.entrance.index = index;
        newElement->properties.entrance.type = ENTRANCE_TYPE_PARK_ENTRANCE;
        newElement->properties.entrance.path_type = gFootpathSelectedId;

        if (!(flags & GAME_COMMAND_FLAG_GHOST)) 
        {
            footpath_connect_edges(x, y, newElement, 1);
        }

        update_park_fences(x, y);
        update_park_fences(x - 32, y);
        update_park_fences(x + 32, y);
        update_park_fences(x, y - 32);
        update_park_fences(x, y + 32);

        map_invalidate_tile(x, y, newElement->base_height * 8, newElement->clearance_height * 8);

        if (index == 0) 
        {
            map_animation_create(MAP_ANIMATION_TYPE_PARK_ENTRANCE, x, y, zLow);
        }
    }
    return 0;
}

static void ParkEntranceRemoveSegment(sint32 x, sint32 y, sint32 z)
{
    rct_map_element *mapElement;

    mapElement = map_get_park_entrance_element_at(x, y, z, true);
    if (mapElement == NULL)
    {
        return;
    }

    map_invalidate_tile(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
    map_element_remove(mapElement);
    update_park_fences(x, y);
}

static money32 ParkEntranceRemove(sint16 x, sint16 y, uint8 z, uint8 flags)
{
    sint32 entranceIndex, direction;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode) 
    {
        return MONEY32_UNDEFINED;
    }

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;
    gCommandPosition.x = x;
    gCommandPosition.y = y;
    gCommandPosition.z = z * 16;

    if (!(flags & GAME_COMMAND_FLAG_APPLY)) 
    {
        return 0;
    }

    entranceIndex = park_get_entrance_index(x, y, z * 16);
    if (entranceIndex == -1) 
    {
        return 0;
    }

    gParkEntrances[entranceIndex].x = MAP_LOCATION_NULL;
    direction = (gParkEntrances[entranceIndex].direction - 1) & 3;

    // Centre (sign)
    ParkEntranceRemoveSegment(x, y, z * 2);

    // Left post
    ParkEntranceRemoveSegment(
        x + TileDirectionDelta[direction].x,
        y + TileDirectionDelta[direction].y,
        z * 2
    );

    // Right post
    ParkEntranceRemoveSegment(
        x - TileDirectionDelta[direction].x,
        y - TileDirectionDelta[direction].y,
        z * 2
    );

    return 0;
}


extern "C"
{
    /**
     *
     *  rct2: 0x006666E7
     */
    void game_command_place_park_entrance(sint32* eax, 
                                          sint32* ebx, 
                                          sint32* ecx, 
                                          sint32* edx, 
                                          sint32* esi, 
                                          sint32* edi, 
                                          sint32* ebp) 
    {
        *ebx = ParkEntrancePlace(
            *ebx & 0xFF,
            *eax & 0xFFFF,
            *ecx & 0xFFFF,
            *edx & 0xFF,
            (*ebx >> 8) & 0xFF);
    }

    /**
     *
     *  rct2: 0x00666A63
     */ 
    void game_command_remove_park_entrance(sint32 *eax, 
                                           sint32 *ebx, 
                                           sint32 *ecx, 
                                           sint32 *edx, 
                                           sint32 *esi, 
                                           sint32 *edi, 
                                           sint32 *ebp)
    {
        *ebx = ParkEntranceRemove(
            *eax & 0xFFFF,
            *ecx & 0xFFFF,
            *edx & 0xFF,
            *ebx & 0xFF);
    }

    /**
     *
     *  rct2: 0x00666F9E
     */
    void park_entrance_remove_ghost()
    {
        if (gParkEntranceGhostExists) 
        {
            gParkEntranceGhostExists = false;
            game_do_command(
                gParkEntranceGhostPosition.x,
                GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_APPLY,
                gParkEntranceGhostPosition.y,
                gParkEntranceGhostPosition.z,
                GAME_COMMAND_REMOVE_PARK_ENTRANCE,
                0,
                0
            );
        }
    }

    /**
     *
     *  rct2: 0x00666F4E
     */
    money32 park_entrance_place_ghost(sint32 x, sint32 y, sint32 z, sint32 direction)
    {
        money32 result;

        park_entrance_remove_ghost();
        result = game_do_command(
            x,
            GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | 
            GAME_COMMAND_FLAG_5 | 
            GAME_COMMAND_FLAG_GHOST | 
            GAME_COMMAND_FLAG_APPLY | 
            (direction << 8),
            y,
            z,
            GAME_COMMAND_PLACE_PARK_ENTRANCE,
            0,
            0
        );

        if (result != MONEY32_UNDEFINED) 
        {
            gParkEntranceGhostPosition.x = x;
            gParkEntranceGhostPosition.y = y;
            gParkEntranceGhostPosition.z = z;
            gParkEntranceGhostDirection = direction;
            gParkEntranceGhostExists = true;
        }
        return result;
    }

    
    sint32 park_get_entrance_index(sint32 x, sint32 y, sint32 z)
    {
        sint32 i;

        for (i = 0; i < MAX_PARK_ENTRANCES; i++) 
        {
            if (x == gParkEntrances[i].x &&
                y == gParkEntrances[i].y &&
                z == gParkEntrances[i].z) 
            {
                return i;
            }
        }

        return -1;
    }

    void reset_park_entrance()
    {
        for (sint32 i = 0; i < MAX_PARK_ENTRANCES; i++) 
        {
            gParkEntrances[i].x = MAP_LOCATION_NULL;
        }
    }
}