/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Entrance.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../actions/RideEntranceExitPlaceAction.hpp"
#include "../actions/RideEntranceExitRemoveAction.hpp"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "Footpath.h"
#include "Map.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Sprite.h"

#include <algorithm>

bool gParkEntranceGhostExists = false;
LocationXYZ16 gParkEntranceGhostPosition = { 0, 0, 0 };
uint8_t gParkEntranceGhostDirection = 0;
std::vector<CoordsXYZD> gParkEntrances;

CoordsXYZD gRideEntranceExitGhostPosition;
uint8_t gRideEntranceExitGhostStationIndex;

static void ParkEntranceRemoveSegment(int32_t x, int32_t y, int32_t z)
{
    EntranceElement* tileElement;

    tileElement = map_get_park_entrance_element_at(x, y, z, true);
    if (tileElement == nullptr)
    {
        return;
    }

    map_invalidate_tile(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
    tileElement->Remove();
    update_park_fences({ x, y });
}

static money32 ParkEntranceRemove(int16_t x, int16_t y, uint8_t z, uint8_t flags)
{
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

    auto entranceIndex = park_entrance_get_index(x, y, z * 16);
    if (entranceIndex == -1)
    {
        return 0;
    }

    auto direction = (gParkEntrances[entranceIndex].direction - 1) & 3;

    // Centre (sign)
    ParkEntranceRemoveSegment(x, y, z * 2);

    // Left post
    ParkEntranceRemoveSegment(x + CoordsDirectionDelta[direction].x, y + CoordsDirectionDelta[direction].y, z * 2);

    // Right post
    ParkEntranceRemoveSegment(x - CoordsDirectionDelta[direction].x, y - CoordsDirectionDelta[direction].y, z * 2);

    gParkEntrances.erase(gParkEntrances.begin() + entranceIndex);
    return 0;
}

static money32 RideEntranceExitPlaceGhost(
    ride_id_t rideIndex, int16_t x, int16_t y, uint8_t direction, uint8_t placeType, uint8_t stationNum)
{
    auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(
        { x, y }, direction, rideIndex, stationNum, placeType == ENTRANCE_TYPE_RIDE_EXIT);
    rideEntranceExitPlaceAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST);
    auto res = GameActions::Execute(&rideEntranceExitPlaceAction);

    return res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
}

/**
 *
 *  rct2: 0x00666A63
 */
void game_command_remove_park_entrance(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, [[maybe_unused]] int32_t* edi,
    [[maybe_unused]] int32_t* ebp)
{
    *ebx = ParkEntranceRemove(*eax & 0xFFFF, *ecx & 0xFFFF, *edx & 0xFF, *ebx & 0xFF);
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
            gParkEntranceGhostPosition.x, GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_APPLY,
            gParkEntranceGhostPosition.y, gParkEntranceGhostPosition.z, GAME_COMMAND_REMOVE_PARK_ENTRANCE, 0, 0);
    }
}

int32_t park_entrance_get_index(int32_t x, int32_t y, int32_t z)
{
    int32_t i = 0;
    for (const auto& entrance : gParkEntrances)
    {
        if (x == entrance.x && y == entrance.y && z == entrance.z)
        {
            return i;
        }
        i++;
    }
    return -1;
}

void reset_park_entrance()
{
    gParkEntrances.clear();
}

void ride_entrance_exit_place_provisional_ghost()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT)
    {
        RideEntranceExitPlaceGhost(
            _currentRideIndex, gRideEntranceExitGhostPosition.x, gRideEntranceExitGhostPosition.y,
            gRideEntranceExitGhostPosition.direction, gRideEntranceExitPlaceType, gRideEntranceExitGhostStationIndex);
    }
}

void ride_entrance_exit_remove_ghost()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT)
    {
        auto rideEntranceExitRemove = RideEntranceExitRemoveAction(
            { gRideEntranceExitGhostPosition.x, gRideEntranceExitGhostPosition.y }, _currentRideIndex,
            gRideEntranceExitGhostStationIndex, gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_EXIT);

        rideEntranceExitRemove.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
        GameActions::Execute(&rideEntranceExitRemove);
    }
}

/**
 *
 *  rct2: 0x006CA28C
 */
money32 ride_entrance_exit_place_ghost(
    Ride* ride, int32_t x, int32_t y, int32_t direction, int32_t placeType, int32_t stationNum)
{
    ride_construction_remove_ghosts();
    money32 result = RideEntranceExitPlaceGhost(ride->id, x, y, direction, placeType, stationNum);

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
 * Replaces the outer hedge walls for an entrance placement removal.
 *  rct2: 0x00666D6F
 */
void maze_entrance_hedge_replacement(int32_t x, int32_t y, TileElement* tileElement)
{
    int32_t direction = tileElement->GetDirection();
    x += CoordsDirectionDelta[direction].x;
    y += CoordsDirectionDelta[direction].y;
    int32_t z = tileElement->base_height;
    ride_id_t rideIndex = tileElement->AsEntrance()->GetRideIndex();

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (tileElement->base_height != z)
            continue;
        if (tileElement->AsTrack()->GetTrackType() != TRACK_ELEM_MAZE)
            continue;

        // Each maze element is split into 4 sections with 4 different walls
        uint8_t mazeSection = direction * 4;
        // Add the top outer wall
        tileElement->AsTrack()->MazeEntryAdd(1 << ((mazeSection + 9) & 0x0F));
        // Add the bottom outer wall
        tileElement->AsTrack()->MazeEntryAdd(1 << ((mazeSection + 12) & 0x0F));

        map_invalidate_tile(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
        return;
    } while (!(tileElement++)->IsLastForTile());
}

/**
 * Removes the hedge walls for an entrance placement.
 *  rct2: 0x00666CBE
 */
void maze_entrance_hedge_removal(int32_t x, int32_t y, TileElement* tileElement)
{
    int32_t direction = tileElement->GetDirection();
    x += CoordsDirectionDelta[direction].x;
    y += CoordsDirectionDelta[direction].y;
    int32_t z = tileElement->base_height;
    ride_id_t rideIndex = tileElement->AsEntrance()->GetRideIndex();

    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (tileElement->base_height != z)
            continue;
        if (tileElement->AsTrack()->GetTrackType() != TRACK_ELEM_MAZE)
            continue;

        // Each maze element is split into 4 sections with 4 different walls
        uint8_t mazeSection = direction * 4;
        // Remove the top outer wall
        tileElement->AsTrack()->MazeEntrySubtract(1 << ((mazeSection + 9) & 0x0F));
        // Remove the bottom outer wall
        tileElement->AsTrack()->MazeEntrySubtract(1 << ((mazeSection + 12) & 0x0F));
        // Remove the intersecting wall
        tileElement->AsTrack()->MazeEntrySubtract(1 << ((mazeSection + 10) & 0x0F));
        // Remove the top hedge section
        tileElement->AsTrack()->MazeEntrySubtract(1 << ((mazeSection + 11) & 0x0F));
        // Remove the bottom hedge section
        tileElement->AsTrack()->MazeEntrySubtract(1 << ((mazeSection + 15) & 0x0F));

        map_invalidate_tile(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
        return;
    } while (!(tileElement++)->IsLastForTile());
}

void fix_park_entrance_locations(void)
{
    // Fix gParkEntrance locations for which the tile_element no longer exists
    gParkEntrances.erase(
        std::remove_if(
            gParkEntrances.begin(), gParkEntrances.end(),
            [](const auto& entrance) {
                return map_get_park_entrance_element_at(entrance.x, entrance.y, entrance.z >> 3, false) == nullptr;
            }),
        gParkEntrances.end());
}

uint8_t EntranceElement::GetStationIndex() const
{
    return (index & MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK) >> 4;
}

void EntranceElement::SetStationIndex(uint8_t stationIndex)
{
    index &= ~MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK;
    index |= (stationIndex << 4);
}

uint8_t EntranceElement::GetEntranceType() const
{
    return entranceType;
}

void EntranceElement::SetEntranceType(uint8_t newType)
{
    entranceType = newType;
}

ride_id_t EntranceElement::GetRideIndex() const
{
    return rideIndex;
}

void EntranceElement::SetRideIndex(ride_id_t newRideIndex)
{
    rideIndex = newRideIndex;
}

uint8_t EntranceElement::GetSequenceIndex() const
{
    return index & 0xF;
}

void EntranceElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    index &= ~0xF;
    index |= (newSequenceIndex & 0xF);
}

uint8_t EntranceElement::GetPathType() const
{
    return pathType;
}

void EntranceElement::SetPathType(uint8_t newPathType)
{
    pathType = newPathType;
}
