/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

MazePlaceTrackAction::MazePlaceTrackAction(const CoordsXYZ& location, NetworkRideId_t rideIndex, uint16_t mazeEntry)
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

GameActions::Result::Ptr MazePlaceTrackAction::Query() const
{
    auto res = std::make_unique<GameActions::Result>();

    res->Position = _loc + CoordsXYZ{ 8, 8, 0 };
    res->Expenditure = ExpenditureType::RideConstruction;
    res->ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
    if (!map_check_free_elements_and_reorganise(1))
    {
        res->Error = GameActions::Status::NoFreeElements;
        res->ErrorMessage = STR_TILE_ELEMENT_LIMIT_REACHED;
        return res;
    }
    if ((_loc.z & 0xF) != 0)
    {
        res->Error = GameActions::Status::Unknown;
        res->ErrorMessage = STR_CONSTRUCTION_ERR_UNKNOWN;
        return res;
    }

    if (!LocationValid(_loc) || (!map_is_location_owned(_loc) && !gCheatsSandboxMode))
    {
        res->Error = GameActions::Status::NotOwned;
        res->ErrorMessage = STR_LAND_NOT_OWNED_BY_PARK;
        return res;
    }

    auto surfaceElement = map_get_surface_element_at(_loc);
    if (surfaceElement == nullptr)
    {
        res->Error = GameActions::Status::Unknown;
        res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
        return res;
    }

    auto baseHeight = _loc.z;
    auto clearanceHeight = _loc.z + MAZE_CLEARANCE_HEIGHT;

    auto heightDifference = baseHeight - surfaceElement->GetBaseZ();
    if (heightDifference >= 0 && !gCheatsDisableSupportLimits)
    {
        heightDifference /= COORDS_Z_PER_TINY_Z;

        if (heightDifference > GetRideTypeDescriptor(RIDE_TYPE_MAZE).Heights.MaxHeight)
        {
            res->Error = GameActions::Status::TooHigh;
            res->ErrorMessage = STR_TOO_HIGH_FOR_SUPPORTS;
            return res;
        }
    }

    money32 clearCost = 0;

    if (!map_can_construct_with_clear_at(
            { _loc.ToTileStart(), baseHeight, clearanceHeight }, &map_place_non_scenery_clear_func, { 0b1111, 0 }, GetFlags(),
            &clearCost, CREATE_CROSSING_MODE_NONE))
    {
        return MakeResult(
            GameActions::Status::NoClearance, res->ErrorTitle.GetStringId(), gGameCommandErrorText, gCommonFormatArgs);
    }

    if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
    {
        res->Error = GameActions::Status::NoClearance;
        res->ErrorMessage = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
        return res;
    }

    if (gMapGroundFlags & ELEMENT_IS_UNDERGROUND)
    {
        res->Error = GameActions::Status::NoClearance;
        res->ErrorMessage = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
        return res;
    }

    auto ride = get_ride(_rideIndex);
    if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
    {
        res->Error = GameActions::Status::InvalidParameters;
        res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
        return res;
    }

    money32 price = (((ride->GetRideTypeDescriptor().BuildCosts.TrackPrice * TrackPricing[TrackElemType::Maze]) >> 16));
    res->Cost = clearCost + price / 2 * 10;

    return res;
}

GameActions::Result::Ptr MazePlaceTrackAction::Execute() const
{
    auto res = std::make_unique<GameActions::Result>();

    res->Position = _loc + CoordsXYZ{ 8, 8, 0 };
    res->Expenditure = ExpenditureType::RideConstruction;
    res->ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        res->Error = GameActions::Status::InvalidParameters;
        res->ErrorMessage = STR_NONE;
        return res;
    }

    if (!map_check_free_elements_and_reorganise(1))
    {
        res->Error = GameActions::Status::NoFreeElements;
        res->ErrorMessage = STR_NONE;
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

    money32 clearCost = 0;
    if (!map_can_construct_with_clear_at(
            { _loc.ToTileStart(), baseHeight, clearanceHeight }, &map_place_non_scenery_clear_func, { 0b1111, 0 },
            GetFlags() | GAME_COMMAND_FLAG_APPLY, &clearCost, CREATE_CROSSING_MODE_NONE))
    {
        return MakeResult(
            GameActions::Status::NoClearance, res->ErrorTitle.GetStringId(), gGameCommandErrorText, gCommonFormatArgs);
    }

    money32 price = (((ride->GetRideTypeDescriptor().BuildCosts.TrackPrice * TrackPricing[TrackElemType::Maze]) >> 16));
    res->Cost = clearCost + price / 2 * 10;

    auto startLoc = _loc.ToTileStart();

    auto* trackElement = TileElementInsert<TrackElement>(_loc, 0b1111);
    Guard::Assert(trackElement != nullptr);

    trackElement->SetClearanceZ(clearanceHeight);
    trackElement->SetTrackType(TrackElemType::Maze);
    trackElement->SetRideIndex(_rideIndex);
    trackElement->SetMazeEntry(_mazeEntry);
    trackElement->SetGhost(flags & GAME_COMMAND_FLAG_GHOST);

    map_invalidate_tile_full(startLoc);

    ride->maze_tiles++;
    ride->stations[0].SetBaseZ(trackElement->GetBaseZ());
    ride->stations[0].Start = { 0, 0 };

    if (ride->maze_tiles == 1)
    {
        ride->overall_view = startLoc;
    }

    return res;
}
