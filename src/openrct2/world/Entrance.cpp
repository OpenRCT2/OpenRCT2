/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Entrance.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../actions/ParkEntranceRemoveAction.h"
#include "../actions/RideEntranceExitPlaceAction.h"
#include "../actions/RideEntranceExitRemoveAction.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../object/FootpathObject.h"
#include "../object/FootpathSurfaceObject.h"
#include "../object/ObjectManager.h"
#include "../ride/RideConstruction.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "Footpath.h"
#include "Map.h"
#include "MapAnimation.h"
#include "Park.h"
#include "tile_element/EntranceElement.h"
#include "tile_element/TrackElement.h"

using namespace OpenRCT2;

bool gParkEntranceGhostExists = false;
CoordsXYZD gParkEntranceGhostPosition = { 0, 0, 0, 0 };

CoordsXYZD gRideEntranceExitGhostPosition;
StationIndex gRideEntranceExitGhostStationIndex;

static money64 RideEntranceExitPlaceGhost(
    RideId rideIndex, const CoordsXY& entranceExitCoords, Direction direction, uint8_t placeType, StationIndex stationNum)
{
    auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(
        entranceExitCoords, direction, rideIndex, stationNum, placeType == ENTRANCE_TYPE_RIDE_EXIT);
    rideEntranceExitPlaceAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST);
    auto res = GameActions::Execute(&rideEntranceExitPlaceAction);

    return res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;
}

/**
 *
 *  rct2: 0x00666F9E
 */
void ParkEntranceRemoveGhost()
{
    if (gParkEntranceGhostExists)
    {
        gParkEntranceGhostExists = false;
        auto parkEntranceRemoveAction = ParkEntranceRemoveAction(gParkEntranceGhostPosition);
        parkEntranceRemoveAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
        GameActions::Execute(&parkEntranceRemoveAction);
    }
}

int32_t ParkEntranceGetIndex(const CoordsXYZ& entrancePos)
{
    int32_t i = 0;
    for (const auto& entrance : GetGameState().Park.Entrances)
    {
        if (entrancePos == entrance)
        {
            return i;
        }
        i++;
    }
    return -1;
}

void ParkEntranceReset()
{
    GetGameState().Park.Entrances.clear();
}

void RideEntranceExitPlaceProvisionalGhost()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT)
    {
        RideEntranceExitPlaceGhost(
            _currentRideIndex, gRideEntranceExitGhostPosition, gRideEntranceExitGhostPosition.direction,
            gRideEntranceExitPlaceType, gRideEntranceExitGhostStationIndex);
    }
}

void RideEntranceExitRemoveGhost()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT)
    {
        auto rideEntranceExitRemove = RideEntranceExitRemoveAction(
            gRideEntranceExitGhostPosition, _currentRideIndex, gRideEntranceExitGhostStationIndex,
            gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_EXIT);

        rideEntranceExitRemove.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
        GameActions::Execute(&rideEntranceExitRemove);
    }
}

/**
 *
 *  rct2: 0x006CA28C
 */
money64 RideEntranceExitPlaceGhost(
    const Ride& ride, const CoordsXY& entranceExitCoords, Direction direction, int32_t placeType, StationIndex stationNum)
{
    RideConstructionRemoveGhosts();
    money64 result = RideEntranceExitPlaceGhost(ride.id, entranceExitCoords, direction, placeType, stationNum);

    if (result != kMoney64Undefined)
    {
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT;
        gRideEntranceExitGhostPosition.x = entranceExitCoords.x;
        gRideEntranceExitGhostPosition.y = entranceExitCoords.y;
        gRideEntranceExitGhostPosition.direction = direction;
        gRideEntranceExitGhostStationIndex = stationNum;
    }
    return result;
}

/**
 * Replaces the outer hedge walls for an entrance placement removal.
 *  rct2: 0x00666D6F
 */
void MazeEntranceHedgeReplacement(const CoordsXYE& entrance)
{
    int32_t direction = entrance.element->GetDirection();
    auto hedgePos = entrance + CoordsDirectionDelta[direction];
    int32_t z = entrance.element->GetBaseZ();
    RideId rideIndex = entrance.element->AsEntrance()->GetRideIndex();

    auto tileElement = MapGetFirstElementAt(hedgePos);
    if (tileElement == nullptr)
        return;
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (tileElement->GetBaseZ() != z)
            continue;
        if (tileElement->AsTrack()->GetTrackType() != TrackElemType::Maze)
            continue;

        // Each maze element is split into 4 sections with 4 different walls
        uint8_t mazeSection = direction * 4;
        // Add the top outer wall
        tileElement->AsTrack()->MazeEntryAdd(1 << ((mazeSection + 9) & 0x0F));
        // Add the bottom outer wall
        tileElement->AsTrack()->MazeEntryAdd(1 << ((mazeSection + 12) & 0x0F));

        MapInvalidateTile({ hedgePos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
        return;
    } while (!(tileElement++)->IsLastForTile());
}

/**
 * Removes the hedge walls for an entrance placement.
 *  rct2: 0x00666CBE
 */
void MazeEntranceHedgeRemoval(const CoordsXYE& entrance)
{
    int32_t direction = entrance.element->GetDirection();
    auto hedgePos = entrance + CoordsDirectionDelta[direction];
    int32_t z = entrance.element->GetBaseZ();
    RideId rideIndex = entrance.element->AsEntrance()->GetRideIndex();

    auto tileElement = MapGetFirstElementAt(hedgePos);
    if (tileElement == nullptr)
        return;
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (tileElement->GetBaseZ() != z)
            continue;
        if (tileElement->AsTrack()->GetTrackType() != TrackElemType::Maze)
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

        MapInvalidateTile({ hedgePos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
        return;
    } while (!(tileElement++)->IsLastForTile());
}

void ParkEntranceFixLocations(void)
{
    auto& gameState = GetGameState();
    // Fix ParkEntrance locations for which the tile_element no longer exists
    gameState.Park.Entrances.erase(
        std::remove_if(
            gameState.Park.Entrances.begin(), gameState.Park.Entrances.end(),
            [](const auto& entrance) { return MapGetParkEntranceElementAt(entrance, false) == nullptr; }),
        gameState.Park.Entrances.end());
}

void ParkEntranceUpdateLocations()
{
    auto& gameState = GetGameState();
    gameState.Park.Entrances.clear();
    TileElementIterator it;
    TileElementIteratorBegin(&it);
    while (TileElementIteratorNext(&it))
    {
        auto entranceElement = it.element->AsEntrance();
        if (entranceElement != nullptr && entranceElement->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE
            && entranceElement->GetSequenceIndex() == 0 && !entranceElement->IsGhost())
        {
            auto entrance = TileCoordsXYZD(it.x, it.y, it.element->BaseHeight, it.element->GetDirection()).ToCoordsXYZD();
            gameState.Park.Entrances.push_back(entrance);
        }
    }
}
