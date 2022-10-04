/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "MazePlaceTrackAction.h"

#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../ride/TrackData.h"
#include "../ride/gentle/Maze.h"
#include "../world/ConstructionClearance.h"

using namespace OpenRCT2::TrackMetaData;

MazePlaceTrackAction::MazePlaceTrackAction(const CoordsXYZ& location, RideId rideIndex, uint16_t mazeEntry)
    : _loc(location)
    , _rideIndex(rideIndex)
    , _mazeEntry(mazeEntry)
{
}

void MazePlaceTrackAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("ride", _rideIndex);
    visitor.Visit("mazeEntry", _mazeEntry);
}

void MazePlaceTrackAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_loc) << DS_TAG(_rideIndex) << DS_TAG(_mazeEntry);
}

GameActions::Result MazePlaceTrackAction::Query() const
{
    auto res = GameActions::Result();

    res.Position = _loc + CoordsXYZ{ 8, 8, 0 };
    res.Expenditure = ExpenditureType::RideConstruction;
    res.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
    if ((_loc.z & 0xF) != 0)
    {
        res.Error = GameActions::Status::Unknown;
        res.ErrorMessage = STR_INVALID_HEIGHT;
        return res;
    }

    if (!LocationValid(_loc) || (!map_is_location_owned(_loc) && !gCheatsSandboxMode))
    {
        res.Error = GameActions::Status::NotOwned;
        res.ErrorMessage = STR_LAND_NOT_OWNED_BY_PARK;
        return res;
    }

    if (!MapCheckCapacityAndReorganise(_loc))
    {
        res.Error = GameActions::Status::NoFreeElements;
        res.ErrorMessage = STR_TILE_ELEMENT_LIMIT_REACHED;
        return res;
    }
    auto surfaceElement = map_get_surface_element_at(_loc);
    if (surfaceElement == nullptr)
    {
        res.Error = GameActions::Status::Unknown;
        res.ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
        return res;
    }

    auto baseHeight = _loc.z;
    auto clearanceHeight = _loc.z + MAZE_CLEARANCE_HEIGHT;

    auto heightDifference = baseHeight - surfaceElement->GetBaseZ();
    if (heightDifference >= 0 && !gCheatsDisableSupportLimits)
    {
        heightDifference /= COORDS_Z_PER_TINY_Z;

        auto* ride = get_ride(_rideIndex);
        const auto& rtd = ride->GetRideTypeDescriptor();
        if (heightDifference > rtd.Heights.MaxHeight)
        {
            res.Error = GameActions::Status::TooHigh;
            res.ErrorMessage = STR_TOO_HIGH_FOR_SUPPORTS;
            return res;
        }
    }

    auto canBuild = MapCanConstructWithClearAt(
        { _loc.ToTileStart(), baseHeight, clearanceHeight }, &MapPlaceNonSceneryClearFunc, { 0b1111, 0 }, GetFlags());
    if (canBuild.Error != GameActions::Status::Ok)
    {
        canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
        return canBuild;
    }

    const auto clearanceData = canBuild.GetData<ConstructClearResult>();
    if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER)
    {
        res.Error = GameActions::Status::NoClearance;
        res.ErrorMessage = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
        return res;
    }

    if (clearanceData.GroundFlags & ELEMENT_IS_UNDERGROUND)
    {
        res.Error = GameActions::Status::NoClearance;
        res.ErrorMessage = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
        return res;
    }

    auto ride = get_ride(_rideIndex);
    if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
    {
        res.Error = GameActions::Status::InvalidParameters;
        res.ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
        return res;
    }

    res.Cost = MazeCalculateCost(canBuild.Cost, *ride, _loc);

    return res;
}

GameActions::Result MazePlaceTrackAction::Execute() const
{
    auto res = GameActions::Result();

    res.Position = _loc + CoordsXYZ{ 8, 8, 0 };
    res.Expenditure = ExpenditureType::RideConstruction;
    res.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        res.Error = GameActions::Status::InvalidParameters;
        res.ErrorMessage = STR_NONE;
        return res;
    }

    uint32_t flags = GetFlags();
    if (!(flags & GAME_COMMAND_FLAG_GHOST))
    {
        footpath_remove_litter(_loc);
        wall_remove_at({ _loc.ToTileStart(), _loc.z, _loc.z + 32 });
    }

    auto baseHeight = _loc.z;
    auto clearanceHeight = _loc.z + MAZE_CLEARANCE_HEIGHT;

    auto canBuild = MapCanConstructWithClearAt(
        { _loc.ToTileStart(), baseHeight, clearanceHeight }, &MapPlaceNonSceneryClearFunc, { 0b1111, 0 },
        GetFlags() | GAME_COMMAND_FLAG_APPLY);
    if (canBuild.Error != GameActions::Status::Ok)
    {
        canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
        return canBuild;
    }

    res.Cost = MazeCalculateCost(canBuild.Cost, *ride, _loc);

    auto startLoc = _loc.ToTileStart();

    auto* trackElement = TileElementInsert<TrackElement>(_loc, 0b1111);
    Guard::Assert(trackElement != nullptr);

    trackElement->SetClearanceZ(clearanceHeight);
    trackElement->SetTrackType(TrackElemType::Maze);
    trackElement->SetRideType(ride->type);
    trackElement->SetRideIndex(_rideIndex);
    trackElement->SetMazeEntry(_mazeEntry);
    trackElement->SetGhost(flags & GAME_COMMAND_FLAG_GHOST);

    map_invalidate_tile_full(startLoc);

    ride->maze_tiles++;
    ride->GetStation().SetBaseZ(trackElement->GetBaseZ());
    ride->GetStation().Start = { 0, 0 };

    if (ride->maze_tiles == 1)
    {
        ride->overall_view = startLoc;
    }

    return res;
}
