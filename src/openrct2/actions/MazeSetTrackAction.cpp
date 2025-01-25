/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MazeSetTrackAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/MazeCost.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../world/ConstructionClearance.h"
#include "../world/Footpath.h"
#include "../world/Park.h"
#include "../world/Wall.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"

using namespace OpenRCT2;

using namespace OpenRCT2::TrackMetaData;

// clang-format off
/** rct2: 0x00993CE9 */
static constexpr uint8_t kByte993CE9[] = {
    0xFF, 0xE0, 0xFF,
    14, 0, 1, 2,
    6, 2, 4, 5,
    9, 10, 6, 8,
    12, 13, 14, 10,
};

/** rct2: 0x00993CFC */
static constexpr uint8_t kByte993CFC[] = {
    5, 12, 0xFF, 0xFF,
    9, 0, 0xFF, 0xFF,
    13, 4, 0xFF, 0xFF,
    1, 8, 0xFF, 0xFF,
};

/** rct2: 0x00993D0C */
static constexpr uint8_t kByte993D0C[] = {
    3, 0, 0xFF, 0xFF,
    0, 1, 0xFF, 0xFF,
    1, 2, 0xFF, 0xFF,
    2, 3, 0xFF, 0xFF,
};
// clang-format on

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

    if (!LocationValid(_loc))
    {
        res.Error = GameActions::Status::InvalidParameters;
        res.ErrorMessage = STR_OFF_EDGE_OF_MAP;
        return res;
    }
    auto& gameState = GetGameState();
    if (!MapIsLocationOwned(_loc) && !gameState.Cheats.sandboxMode)
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
    auto surfaceElement = MapGetSurfaceElementAt(_loc);
    if (surfaceElement == nullptr)
    {
        res.Error = GameActions::Status::Unknown;
        res.ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
        return res;
    }

    auto baseHeight = _loc.z;
    auto clearanceHeight = _loc.z + 32;

    auto heightDifference = baseHeight - surfaceElement->GetBaseZ();
    if (heightDifference >= 0 && !gameState.Cheats.disableSupportLimits)
    {
        heightDifference /= kCoordsZPerTinyZ;

        auto* ride = GetRide(_rideIndex);
        const auto& rtd = ride->GetRideTypeDescriptor();
        if (heightDifference > rtd.Heights.MaxHeight)
        {
            res.Error = GameActions::Status::TooHigh;
            res.ErrorMessage = STR_TOO_HIGH_FOR_SUPPORTS;
            return res;
        }
    }

    TileElement* tileElement = MapGetTrackElementAtOfTypeFromRide(_loc, TrackElemType::Maze, _rideIndex);
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

        auto ride = GetRide(_rideIndex);
        if (ride == nullptr || !RideTypeIsValid(ride->type))
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex);
            res.Error = GameActions::Status::NoClearance;
            res.ErrorMessage = STR_ERR_RIDE_NOT_FOUND;
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

    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %u", _rideIndex);
        res.Error = GameActions::Status::InvalidParameters;
        res.ErrorMessage = STR_ERR_RIDE_NOT_FOUND;
        return res;
    }

    uint32_t flags = GetFlags();
    if (!(flags & GAME_COMMAND_FLAG_GHOST))
    {
        FootpathRemoveLitter(_loc);
        WallRemoveAt({ _loc.ToTileStart(), _loc.z, _loc.z + 32 });
    }

    auto tileElement = MapGetTrackElementAtOfTypeFromRide(_loc, TrackElemType::Maze, _rideIndex);
    if (tileElement == nullptr)
    {
        res.Cost = MazeCalculateCost(0, *ride, _loc);

        auto startLoc = _loc.ToTileStart();

        auto* trackElement = TileElementInsert<TrackElement>(_loc, 0b1111);
        Guard::Assert(trackElement != nullptr);

        trackElement->SetClearanceZ(_loc.z + kMazeClearanceHeight);
        trackElement->SetTrackType(TrackElemType::Maze);
        trackElement->SetRideType(ride->type);
        trackElement->SetRideIndex(_rideIndex);
        trackElement->SetMazeEntry(0xFFFF);
        trackElement->SetGhost(flags & GAME_COMMAND_FLAG_GHOST);

        tileElement = trackElement->as<TileElement>();

        MapInvalidateTileFull(startLoc);

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
                segmentOffset = kByte993CE9[(_loc.direction + segmentOffset)];
                tileElement->AsTrack()->MazeEntrySubtract(1 << segmentOffset);

                uint8_t temp_edx = kByte993CFC[segmentOffset];
                if (temp_edx != 0xFF)
                {
                    auto previousElementLoc = CoordsXY{ _loc }.ToTileStart() - CoordsDirectionDelta[_loc.direction];

                    TileElement* previousTileElement = MapGetTrackElementAtOfTypeFromRide(
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

                tileElement = MapGetTrackElementAtOfTypeFromRide({ previousSegment, _loc.z }, TrackElemType::Maze, _rideIndex);

                MapInvalidateTileFull(previousSegment.ToTileStart());
                if (tileElement == nullptr)
                {
                    LOG_ERROR("No surface found");
                    res.Error = GameActions::Status::Unknown;
                    res.ErrorMessage = STR_ERR_SURFACE_ELEMENT_NOT_FOUND;
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

                    uint32_t direction1 = kByte993D0C[segmentBit];
                    auto nextElementLoc = previousSegment.ToTileStart() + CoordsDirectionDelta[direction1];

                    TileElement* tmp_tileElement = MapGetTrackElementAtOfTypeFromRide(
                        { nextElementLoc, _loc.z }, TrackElemType::Maze, _rideIndex);

                    if (tmp_tileElement != nullptr)
                    {
                        uint8_t edx11 = kByte993CFC[segmentBit];
                        tmp_tileElement->AsTrack()->MazeEntryAdd(1 << (edx11));
                    }

                    segmentBit--;
                } while ((segmentBit & 0x3) != 0x3);
            }
            break;
    }

    MapInvalidateTile({ _loc.ToTileStart(), tileElement->GetBaseZ(), tileElement->GetClearanceZ() });

    if ((tileElement->AsTrack()->GetMazeEntry() & 0x8888) == 0x8888)
    {
        TileElementRemove(tileElement);
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
