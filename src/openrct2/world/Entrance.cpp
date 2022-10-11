/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

#include <algorithm>

bool gParkEntranceGhostExists = false;
CoordsXYZD gParkEntranceGhostPosition = { 0, 0, 0, 0 };
std::vector<CoordsXYZD> gParkEntrances;

CoordsXYZD gRideEntranceExitGhostPosition;
StationIndex gRideEntranceExitGhostStationIndex;

static money32 RideEntranceExitPlaceGhost(
    RideId rideIndex, const CoordsXY& entranceExitCoords, Direction direction, uint8_t placeType, StationIndex stationNum)
{
    auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(
        entranceExitCoords, direction, rideIndex, stationNum, placeType == ENTRANCE_TYPE_RIDE_EXIT);
    rideEntranceExitPlaceAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST);
    auto res = GameActions::Execute(&rideEntranceExitPlaceAction);

    return res.Error == GameActions::Status::Ok ? res.Cost : MONEY32_UNDEFINED;
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
    for (const auto& entrance : gParkEntrances)
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
    gParkEntrances.clear();
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
money32 RideEntranceExitPlaceGhost(
    Ride* ride, const CoordsXY& entranceExitCoords, Direction direction, int32_t placeType, StationIndex stationNum)
{
    ride_construction_remove_ghosts();
    money32 result = RideEntranceExitPlaceGhost(ride->id, entranceExitCoords, direction, placeType, stationNum);

    if (result != MONEY32_UNDEFINED)
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

        map_invalidate_tile({ hedgePos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
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

        map_invalidate_tile({ hedgePos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
        return;
    } while (!(tileElement++)->IsLastForTile());
}

void ParkEntranceFixLocations(void)
{
    // Fix gParkEntrance locations for which the tile_element no longer exists
    gParkEntrances.erase(
        std::remove_if(
            gParkEntrances.begin(), gParkEntrances.end(),
            [](const auto& entrance) { return map_get_park_entrance_element_at(entrance, false) == nullptr; }),
        gParkEntrances.end());
}

void ParkEntranceUpdateLocations()
{
    gParkEntrances.clear();
    tile_element_iterator it;
    tile_element_iterator_begin(&it);
    while (tile_element_iterator_next(&it))
    {
        auto entranceElement = it.element->AsEntrance();
        if (entranceElement != nullptr && entranceElement->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE
            && entranceElement->GetSequenceIndex() == 0 && !entranceElement->IsGhost())
        {
            auto entrance = TileCoordsXYZD(it.x, it.y, it.element->base_height, it.element->GetDirection()).ToCoordsXYZD();
            gParkEntrances.push_back(entrance);
        }
    }
}

StationIndex EntranceElement::GetStationIndex() const
{
    return stationIndex;
}

void EntranceElement::SetStationIndex(StationIndex newStationIndex)
{
    stationIndex = newStationIndex;
}

uint8_t EntranceElement::GetEntranceType() const
{
    return entranceType;
}

void EntranceElement::SetEntranceType(uint8_t newType)
{
    entranceType = newType;
}

RideId EntranceElement::GetRideIndex() const
{
    return rideIndex;
}

void EntranceElement::SetRideIndex(RideId newRideIndex)
{
    rideIndex = newRideIndex;
}

uint8_t EntranceElement::GetSequenceIndex() const
{
    return SequenceIndex & 0xF;
}

void EntranceElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    SequenceIndex &= ~0xF;
    SequenceIndex |= (newSequenceIndex & 0xF);
}

bool EntranceElement::HasLegacyPathEntry() const
{
    return (flags2 & ENTRANCE_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY) != 0;
}

ObjectEntryIndex EntranceElement::GetLegacyPathEntryIndex() const
{
    if (HasLegacyPathEntry())
        return PathType;

    return OBJECT_ENTRY_INDEX_NULL;
}

const FootpathObject* EntranceElement::GetLegacyPathEntry() const
{
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    return static_cast<FootpathObject*>(objMgr.GetLoadedObject(ObjectType::Paths, GetLegacyPathEntryIndex()));
}

void EntranceElement::SetLegacyPathEntryIndex(ObjectEntryIndex newPathType)
{
    PathType = newPathType;
    flags2 |= ENTRANCE_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
}

ObjectEntryIndex EntranceElement::GetSurfaceEntryIndex() const
{
    if (HasLegacyPathEntry())
        return OBJECT_ENTRY_INDEX_NULL;

    return PathType;
}

const FootpathSurfaceObject* EntranceElement::GetSurfaceEntry() const
{
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    return static_cast<FootpathSurfaceObject*>(objMgr.GetLoadedObject(ObjectType::FootpathSurface, GetSurfaceEntryIndex()));
}

void EntranceElement::SetSurfaceEntryIndex(ObjectEntryIndex newIndex)
{
    PathType = newIndex;
    flags2 &= ~ENTRANCE_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
}

const PathSurfaceDescriptor* EntranceElement::GetPathSurfaceDescriptor() const
{
    if (HasLegacyPathEntry())
    {
        const auto* legacyPathEntry = GetLegacyPathEntry();
        if (legacyPathEntry == nullptr)
            return nullptr;

        return &legacyPathEntry->GetPathSurfaceDescriptor();
    }

    const auto* surfaceEntry = GetSurfaceEntry();
    if (surfaceEntry == nullptr)
        return nullptr;

    return &surfaceEntry->GetDescriptor();
}
