/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MazeSetTrackAction.h"

#include "../Cheats.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/gentle/Maze.h"
#include "../world/ConstructionClearance.h"
#include "../world/Footpath.h"
#include "../world/Park.h"

using namespace OpenRCT2::TrackMetaData;

MazeSetTrackAction::MazeSetTrackAction(const CoordsXYZD& location, bool initialPlacement, RideId rideIndex, uint8_t mode)
    : _loc(location)
    , _initialPlacement(initialPlacement)
    , _rideIndex(rideIndex)
    , _mode(mode)
{
}

void MazeSetTrackAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("ride", _rideIndex);
    visitor.Visit("mode", _mode);
    visitor.Visit("isInitialPlacement", _initialPlacement);
}

void MazeSetTrackAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_loc) << DS_TAG(_loc.direction) << DS_TAG(_initialPlacement) << DS_TAG(_rideIndex) << DS_TAG(_mode);
}

GameActions::Result MazeSetTrackAction::Query() const
{
    auto res = GameActions::Result();

    res.Position = _loc + CoordsXYZ{ 8, 8, 0 };
    res.Expenditure = ExpenditureType::RideConstruction;
    res.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
    if ((_loc.z & 0xF) != 0 && _mode == GC_SET_MAZE_TRACK_BUILD)
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
    auto clearanceHeight = _loc.z + 32;

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

    TileElement* tileElement = map_get_track_element_at_of_type_from_ride(_loc, TrackElemType::Maze, _rideIndex);
    if (tileElement == nullptr)
    {
        if (_mode != GC_SET_MAZE_TRACK_BUILD)
        {
            res.Error = GameActions::Status::Unknown;
            res.ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
            return res;
        }
        auto constructResult = MapCanConstructAt({ _loc.ToTileStart(), baseHeight, clearanceHeight }, { 0b1111, 0 });
        if (constructResult.Error != GameActions::Status::Ok)
        {
            constructResult.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            return constructResult;
        }

        const auto clearanceData = constructResult.GetData<ConstructClearResult>();
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
        if (ride == nullptr || ride->type == RIDE_CRASH_TYPE_NONE)
        {
            res.Error = GameActions::Status::NoClearance;
            res.ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
            return res;
        }

        res.Cost = MazeCalculateCost(constructResult.Cost, *ride, _loc);

        return res;
    }

    return GameActions::Result();
}

GameActions::Result MazeSetTrackAction::Execute() const
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

    auto tileElement = map_get_track_element_at_of_type_from_ride(_loc, TrackElemType::Maze, _rideIndex);
    if (tileElement == nullptr)
    {
        res.Cost = MazeCalculateCost(0, *ride, _loc);

        auto startLoc = _loc.ToTileStart();

        auto* trackElement = TileElementInsert<TrackElement>(_loc, 0b1111);
        Guard::Assert(trackElement != nullptr);

        trackElement->SetClearanceZ(_loc.z + MAZE_CLEARANCE_HEIGHT);
        trackElement->SetTrackType(TrackElemType::Maze);
        trackElement->SetRideType(ride->type);
        trackElement->SetRideIndex(_rideIndex);
        trackElement->SetMazeEntry(0xFFFF);
        trackElement->SetGhost(flags & GAME_COMMAND_FLAG_GHOST);

        tileElement = trackElement->as<TileElement>();

        map_invalidate_tile_full(startLoc);

        ride->maze_tiles++;
        ride->GetStation().SetBaseZ(tileElement->GetBaseZ());
        ride->GetStation().Start = { 0, 0 };

        if (_initialPlacement && !(flags & GAME_COMMAND_FLAG_GHOST))
        {
            ride->overall_view = startLoc;
        }
    }

    switch (_mode)
    {
        case GC_SET_MAZE_TRACK_BUILD:
        {
            uint8_t segmentOffset = MazeGetSegmentBit(_loc);

            tileElement->AsTrack()->MazeEntrySubtract(1 << segmentOffset);

            if (!_initialPlacement)
            {
                segmentOffset = byte_993CE9[(_loc.direction + segmentOffset)];
                tileElement->AsTrack()->MazeEntrySubtract(1 << segmentOffset);

                uint8_t temp_edx = byte_993CFC[segmentOffset];
                if (temp_edx != 0xFF)
                {
                    auto previousElementLoc = CoordsXY{ _loc }.ToTileStart() - CoordsDirectionDelta[_loc.direction];

                    TileElement* previousTileElement = map_get_track_element_at_of_type_from_ride(
                        { previousElementLoc, _loc.z }, TrackElemType::Maze, _rideIndex);

                    if (previousTileElement != nullptr)
                    {
                        previousTileElement->AsTrack()->MazeEntrySubtract(1 << temp_edx);
                    }
                    else
                    {
                        tileElement->AsTrack()->MazeEntryAdd(1 << segmentOffset);
                    }
                }
            }

            break;
        }

        case GC_SET_MAZE_TRACK_MOVE:
            break;

        case GC_SET_MAZE_TRACK_FILL:
            if (!_initialPlacement)
            {
                auto previousSegment = CoordsXY{ _loc.x - CoordsDirectionDelta[_loc.direction].x / 2,
                                                 _loc.y - CoordsDirectionDelta[_loc.direction].y / 2 };

                tileElement = map_get_track_element_at_of_type_from_ride(
                    { previousSegment, _loc.z }, TrackElemType::Maze, _rideIndex);

                map_invalidate_tile_full(previousSegment.ToTileStart());
                if (tileElement == nullptr)
                {
                    log_error("No surface found");
                    res.Error = GameActions::Status::Unknown;
                    res.ErrorMessage = STR_NONE;
                    return res;
                }

                uint32_t segmentBit = MazeGetSegmentBit(previousSegment);

                tileElement->AsTrack()->MazeEntryAdd(1 << segmentBit);
                segmentBit--;
                tileElement->AsTrack()->MazeEntryAdd(1 << segmentBit);
                segmentBit = (segmentBit - 4) & 0x0F;
                tileElement->AsTrack()->MazeEntryAdd(1 << segmentBit);
                segmentBit = (segmentBit + 3) & 0x0F;

                do
                {
                    tileElement->AsTrack()->MazeEntryAdd(1 << segmentBit);

                    uint32_t direction1 = byte_993D0C[segmentBit];
                    auto nextElementLoc = previousSegment.ToTileStart() + CoordsDirectionDelta[direction1];

                    TileElement* tmp_tileElement = map_get_track_element_at_of_type_from_ride(
                        { nextElementLoc, _loc.z }, TrackElemType::Maze, _rideIndex);

                    if (tmp_tileElement != nullptr)
                    {
                        uint8_t edx11 = byte_993CFC[segmentBit];
                        tmp_tileElement->AsTrack()->MazeEntryAdd(1 << (edx11));
                    }

                    segmentBit--;
                } while ((segmentBit & 0x3) != 0x3);
            }
            break;
    }

    map_invalidate_tile({ _loc.ToTileStart(), tileElement->GetBaseZ(), tileElement->GetClearanceZ() });

    if ((tileElement->AsTrack()->GetMazeEntry() & 0x8888) == 0x8888)
    {
        tile_element_remove(tileElement);
        ride->ValidateStations();
        ride->maze_tiles--;
    }

    return res;
}

uint8_t MazeSetTrackAction::MazeGetSegmentBit(const CoordsXY& coords) const
{
    uint8_t minorX = coords.x & 0x1F;
    uint8_t minorY = coords.y & 0x1F;

    if (minorX == 0 && minorY == 0)
    {
        return 3;
    }

    if (minorY == 16 && minorX == 16)
    {
        return 11;
    }

    if (minorY == 0)
    {
        return 15;
    }

    return 7;
}
