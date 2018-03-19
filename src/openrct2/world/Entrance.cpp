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

#include "../network/network.h"
#include "../OpenRCT2.h"

#include "Entrance.h"
#include "Footpath.h"
#include "Map.h"
#include "MapAnimation.h"
#include "Park.h"
#include "../Cheats.h"
#include "../Game.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Track.h"
#include "../ride/Station.h"

bool gParkEntranceGhostExists = false;
LocationXYZ16 gParkEntranceGhostPosition = { 0, 0, 0 };
uint8 gParkEntranceGhostDirection = 0;
CoordsXYZD gParkEntrances[MAX_PARK_ENTRANCES];

CoordsXYZD gRideEntranceExitGhostPosition;
uint8 gRideEntranceExitGhostStationIndex;

static void ParkEntranceRemoveSegment(sint32 x, sint32 y, sint32 z)
{
    rct_tile_element *tileElement;

    tileElement = map_get_park_entrance_element_at(x, y, z, true);
    if (tileElement == nullptr)
    {
        return;
    }

    map_invalidate_tile(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
    tile_element_remove(tileElement);
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

    entranceIndex = park_entrance_get_index(x, y, z * 16);
    if (entranceIndex == -1)
    {
        return 0;
    }

    gParkEntrances[entranceIndex].x = LOCATION_NULL;
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

static money32 RideEntranceExitPlace(sint16 x,
    sint16 y,
    sint16 z,
    uint8 direction,
    uint8 flags,
    uint8 rideIndex,
    uint8 stationNum,
    bool isExit)
{
    // Remember when in unknown station num mode rideIndex is unknown and z is set
    // When in known station num mode rideIndex is known and z is unknown

    money32 cost = 0;
    gCommandPosition.x = x;
    gCommandPosition.y = y;

    if (!map_check_free_elements_and_reorganise(1))
    {
        return MONEY32_UNDEFINED;
    }

    if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) && game_is_paused() && !gCheatsBuildInPauseMode)
    {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        return MONEY32_UNDEFINED;
    }

    if (stationNum == 0xFF)
    {
        z *= 16;
        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            return MONEY32_UNDEFINED;
        }

        if (!gCheatsSandboxMode && !map_is_location_owned(x, y, z))
        {
            return MONEY32_UNDEFINED;
        }

        sint16 clear_z = z / 8 + (isExit ? 5 : 7);

        if (!gCheatsDisableClearanceChecks &&
            !map_can_construct_with_clear_at(x, y, z / 8, clear_z, &map_place_non_scenery_clear_func, 0xF, flags, &cost, CREATE_CROSSING_MODE_NONE))
        {
            return MONEY32_UNDEFINED;
        }

        if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
        {
            gGameCommandErrorText = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
            return MONEY32_UNDEFINED;
        }

        if (z / 8 > 244)
        {
            gGameCommandErrorText = STR_TOO_HIGH;
            return MONEY32_UNDEFINED;
        }
    }
    else
    {
        if (rideIndex >= MAX_RIDES)
        {
            log_warning("Invalid game command for ride %u", rideIndex);
            return MONEY32_UNDEFINED;
        }

        Ride* ride = get_ride(rideIndex);
        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command for ride %u", rideIndex);
            return MONEY32_UNDEFINED;
        }

        if (ride->status != RIDE_STATUS_CLOSED)
        {
            gGameCommandErrorText = STR_MUST_BE_CLOSED_FIRST;
            return MONEY32_UNDEFINED;
        }

        if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
        {
            gGameCommandErrorText = STR_NOT_ALLOWED_TO_MODIFY_STATION;
            return MONEY32_UNDEFINED;
        }

        ride_clear_for_construction(rideIndex);
        ride_remove_peeps(rideIndex);

        bool requiresRemove = false;
        LocationXY16 removeCoord = { 0, 0 };

        if (isExit)
        {
            const auto exit = ride_get_exit_location(rideIndex, stationNum);
            if (!exit.isNull())
            {
                if (flags & GAME_COMMAND_FLAG_GHOST)
                {
                    gGameCommandErrorText = 0;
                    return MONEY32_UNDEFINED;
                }

                removeCoord.x = exit.x * 32;
                removeCoord.y = exit.y * 32;
                requiresRemove = true;
            }
        }
        else
        {
            const auto entrance = ride_get_entrance_location(rideIndex, stationNum);
            if (!entrance.isNull())
            {
                if (flags & GAME_COMMAND_FLAG_GHOST)
                {
                    gGameCommandErrorText = 0;
                    return MONEY32_UNDEFINED;
                }

                removeCoord.x = entrance.x * 32;
                removeCoord.y = entrance.y * 32;
                requiresRemove = true;
            }
        }

        if (requiresRemove)
        {
            money32 success = game_do_command(
                removeCoord.x,
                flags,
                removeCoord.y,
                rideIndex,
                GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT,
                stationNum,
                0
            );

            if (success == MONEY32_UNDEFINED)
            {
                return MONEY32_UNDEFINED;
            }
        }

        z = ride->station_heights[stationNum] * 8;
        gCommandPosition.z = z;

        if ((flags & GAME_COMMAND_FLAG_APPLY) &&
            !(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) &&
            !(flags & GAME_COMMAND_FLAG_GHOST))
        {
            footpath_remove_litter(x, y, z);
            wall_remove_at_z(x, y, z);
        }

        if (!gCheatsSandboxMode && !map_is_location_owned(x, y, z))
        {
            return MONEY32_UNDEFINED;
        }

        sint8 clear_z = (z / 8) + (isExit ? 5 : 7);

        if (!gCheatsDisableClearanceChecks &&
            !map_can_construct_with_clear_at(x, y, z / 8, clear_z, &map_place_non_scenery_clear_func, 0xF, flags, &cost, CREATE_CROSSING_MODE_NONE))
        {
            return MONEY32_UNDEFINED;
        }

        if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
        {
            gGameCommandErrorText = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
            return MONEY32_UNDEFINED;
        }

        if (z / 8 > 244)
        {
            gGameCommandErrorText = STR_TOO_HIGH;
            return MONEY32_UNDEFINED;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            LocationXYZ16 coord;
            coord.x = x + 16;
            coord.y = y + 16;
            coord.z = tile_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);

            rct_tile_element* tileElement = tile_element_insert(x / 32, y / 32, z / 8, 0xF);
            assert(tileElement != nullptr);
            tileElement->clearance_height = clear_z;
            tileElement->properties.entrance.type = isExit;
            tileElement->properties.entrance.index = stationNum << 4;
            tileElement->properties.entrance.ride_index = rideIndex;
            tileElement->type = TILE_ELEMENT_TYPE_ENTRANCE | direction;

            if (flags & GAME_COMMAND_FLAG_GHOST)
            {
                tileElement->flags |= TILE_ELEMENT_FLAG_GHOST;
            }

            if (isExit)
            {
                ride_set_exit_location(ride, stationNum, { x / 32, y / 32, z / 8, (uint8)tile_element_get_direction(tileElement)});
            }
            else
            {
                ride_set_entrance_location(ride, stationNum, { x / 32, y / 32, z / 8, (uint8)tile_element_get_direction(tileElement)});
                ride->last_peep_in_queue[stationNum] = SPRITE_INDEX_NULL;
                ride->queue_length[stationNum] = 0;

                map_animation_create(MAP_ANIMATION_TYPE_RIDE_ENTRANCE, x, y, z / 8);
            }

            footpath_queue_chain_reset();

            if (!(flags & GAME_COMMAND_FLAG_GHOST))
            {
                maze_entrance_hedge_removal(x, y, tileElement);
            }

            footpath_connect_edges(x, y, tileElement, flags);
            footpath_update_queue_chains();

            map_invalidate_tile_full(x, y);
        }
    }

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    return cost;
}

static money32 RideEntranceExitRemove(sint16 x, sint16 y, uint8 rideIndex, uint8 stationNum, uint8 flags)
{
    if (rideIndex >= MAX_RIDES)
    {
        log_warning("Invalid game command for ride %u", rideIndex);
        return MONEY32_UNDEFINED;
    }

    Ride* ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid ride id %u for entrance/exit removal", rideIndex);
        return MONEY32_UNDEFINED;
    }

    if (!(flags & GAME_COMMAND_FLAG_GHOST))
    {
        if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) &&
            game_is_paused() &&
            !gCheatsBuildInPauseMode)
        {
            gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
            return MONEY32_UNDEFINED;
        }
    }

    if (ride->status != RIDE_STATUS_CLOSED)
    {
        gGameCommandErrorText = STR_MUST_BE_CLOSED_FIRST;
        return MONEY32_UNDEFINED;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
    {
        gGameCommandErrorText = STR_NOT_ALLOWED_TO_MODIFY_STATION;
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        ride_clear_for_construction(rideIndex);
        ride_remove_peeps(rideIndex);
        invalidate_test_results(rideIndex);

        bool found = false;
        rct_tile_element* tileElement = map_get_first_element_at(x / 32, y / 32);
        if (tileElement == nullptr)
        {
            log_warning("Invalid coordinates for entrance/exit removal x = %d, y = %d", x, y);
            return MONEY32_UNDEFINED;
        }

        do
        {
            if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;

            if (tile_element_get_ride_index(tileElement) != rideIndex)
                continue;

            if (tile_element_get_station(tileElement) != stationNum)
                continue;

            if (flags & GAME_COMMAND_FLAG_5 && !(tileElement->flags & TILE_ELEMENT_FLAG_GHOST))
                continue;

            found = true;
            break;
        }
        while (!tile_element_is_last_for_tile(tileElement++));

        if (!found)
        {
            return MONEY32_UNDEFINED;
        }

        LocationXYZ16 coord;
        coord.x = x + 16;
        coord.y = y + 16;
        coord.z = tile_element_height(coord.x, coord.y);
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);

        footpath_queue_chain_reset();
        maze_entrance_hedge_replacement(x, y, tileElement);
        footpath_remove_edges_at(x, y, tileElement);

        bool isExit = tileElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_EXIT;

        tile_element_remove(tileElement);

        if (isExit)
        {
            ride_clear_exit_location(ride, stationNum);
        }
        else
        {
            ride_clear_entrance_location(ride, stationNum);
        }

        footpath_update_queue_chains();

        map_invalidate_tile_full(x, y);
    }

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    return 0;
}

static money32 RideEntranceExitPlaceGhost(uint8 rideIndex, sint16 x, sint16 y, uint8 direction, uint8 placeType, uint8 stationNum)
{
    return game_do_command(
        x,
        (GAME_COMMAND_FLAG_APPLY |
         GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED |
         GAME_COMMAND_FLAG_5 |
         GAME_COMMAND_FLAG_GHOST) |
         (direction << 8),
        y,
        rideIndex | (placeType << 8),
        GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT,
        stationNum,
        0
    );
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

sint32 park_entrance_get_index(sint32 x, sint32 y, sint32 z)
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
    for (auto &parkEntrance : gParkEntrances)
    {
        parkEntrance.x = LOCATION_NULL;
    }
}


void ride_entrance_exit_place_provisional_ghost()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT) {
        RideEntranceExitPlaceGhost(_currentRideIndex,
                                       gRideEntranceExitGhostPosition.x,
                                       gRideEntranceExitGhostPosition.y,
                                       gRideEntranceExitGhostPosition.direction,
                                       gRideEntranceExitPlaceType,
                                       gRideEntranceExitGhostStationIndex);
    }
}

void ride_entrance_exit_remove_ghost()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT) {
        game_do_command(
            gRideEntranceExitGhostPosition.x,
            (GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_APPLY),
            gRideEntranceExitGhostPosition.y,
            _currentRideIndex,
            GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT,
            gRideEntranceExitGhostStationIndex,
            0
        );
    }
}


/**
 *
 *  rct2: 0x006CA28C
 */
money32 ride_entrance_exit_place_ghost(sint32 rideIndex,
                                        sint32 x,
                                        sint32 y,
                                        sint32 direction,
                                        sint32 placeType,
                                        sint32 stationNum)
{
    ride_construction_remove_ghosts();
    money32 result = RideEntranceExitPlaceGhost(rideIndex, x, y, direction, placeType, stationNum);

    if (result != MONEY32_UNDEFINED)
    {
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT;
        gRideEntranceExitGhostPosition.x = x;
        gRideEntranceExitGhostPosition.y = y;
        gRideEntranceExitGhostPosition.direction = direction;
        gRideEntranceExitGhostStationIndex = stationNum & 0xFF;
    }
    return result;
}

/**
 *
 *  rct2: 0x006660A8
 */
void game_command_place_ride_entrance_or_exit(sint32 *eax,
                                              sint32 *ebx,
                                              sint32 *ecx,
                                              sint32 *edx,
                                              sint32 *esi,
                                              sint32 *edi,
                                              sint32 *ebp)
{
    *ebx = RideEntranceExitPlace(
        *eax & 0xFFFF,
        *ecx & 0xFFFF,
        *edx & 0xFF,
        (*ebx >> 8) & 0xFF,
        *ebx & 0xFF,
        *edx & 0xFF,
        *edi & 0xFF,
        ((*edx >> 8) & 0xFF) != 0
        );
}

/**
 *
 *  rct2: 0x0066640B
 */
void game_command_remove_ride_entrance_or_exit(sint32 *eax,
                                               sint32 *ebx,
                                               sint32 *ecx,
                                               sint32 *edx,
                                               sint32 *esi,
                                               sint32 *edi,
                                               sint32 *ebp)
{
    *ebx = RideEntranceExitRemove(
        *eax & 0xFFFF,
        *ecx & 0xFFFF,
        *edx & 0xFF,
        *edi & 0xFF,
        *ebx & 0xFF
    );
}

/**
 * Replaces the outer hedge walls for an entrance placement removal.
 *  rct2: 0x00666D6F
 */
void maze_entrance_hedge_replacement(sint32 x, sint32 y, rct_tile_element *tileElement)
{
    sint32 direction = tile_element_get_direction(tileElement);
    x += TileDirectionDelta[direction].x;
    y += TileDirectionDelta[direction].y;
    sint32 z = tileElement->base_height;
    sint32 rideIndex = track_element_get_ride_index(tileElement);

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->type != TILE_ELEMENT_TYPE_TRACK) continue;
        if (track_element_get_ride_index(tileElement) != rideIndex) continue;
        if (tileElement->base_height != z) continue;
        if (track_element_get_type(tileElement) != TRACK_ELEM_MAZE) continue;

        // Each maze element is split into 4 sections with 4 different walls
        uint8 mazeSection = direction * 4;
        // Add the top outer wall
        tileElement->properties.track.maze_entry |= (1 << ((mazeSection + 9) & 0x0F));
        // Add the bottom outer wall
        tileElement->properties.track.maze_entry |= (1 << ((mazeSection + 12) & 0x0F));

        map_invalidate_tile(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
        return;
    } while (!tile_element_is_last_for_tile(tileElement++));
}

/**
 * Removes the hedge walls for an entrance placement.
 *  rct2: 0x00666CBE
 */
void maze_entrance_hedge_removal(sint32 x, sint32 y, rct_tile_element *tileElement)
{
    sint32 direction = tile_element_get_direction(tileElement);
    x += TileDirectionDelta[direction].x;
    y += TileDirectionDelta[direction].y;
    sint32 z = tileElement->base_height;
    sint32 rideIndex = track_element_get_ride_index(tileElement);

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (tileElement->type != TILE_ELEMENT_TYPE_TRACK) continue;
        if (track_element_get_ride_index(tileElement) != rideIndex) continue;
        if (tileElement->base_height != z) continue;
        if (track_element_get_type(tileElement) != TRACK_ELEM_MAZE) continue;

        // Each maze element is split into 4 sections with 4 different walls
        uint8 mazeSection = direction * 4;
        // Remove the top outer wall
        tileElement->properties.track.maze_entry &= ~(1 << ((mazeSection + 9) & 0x0F));
        // Remove the bottom outer wall
        tileElement->properties.track.maze_entry &= ~(1 << ((mazeSection + 12) & 0x0F));
        // Remove the intersecting wall
        tileElement->properties.track.maze_entry &= ~(1 << ((mazeSection + 10) & 0x0F));
        // Remove the top hedge section
        tileElement->properties.track.maze_entry &= ~(1 << ((mazeSection + 11) & 0x0F));
        // Remove the bottom hedge section
        tileElement->properties.track.maze_entry &= ~(1 << ((mazeSection + 15) & 0x0F));

        map_invalidate_tile(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
        return;
    } while (!tile_element_is_last_for_tile(tileElement++));
}

void fix_park_entrance_locations(void)
{
    // Fix gParkEntrance locations for which the tile_element no longer exists
    for (auto &entrance : gParkEntrances)
    {
        if (entrance.x == LOCATION_NULL)
            continue;

        if (map_get_park_entrance_element_at(
            entrance.x,
            entrance.y,
            entrance.z >> 3, false) == nullptr)
        {
            entrance.x = LOCATION_NULL;
        }
    }
}

