/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathPlaceAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../object/PathAdditionEntry.h"
#include "../ride/RideConstruction.h"
#include "../windows/Intent.h"
#include "../world/ConstructionClearance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Park.h"
#include "../world/QuarterTile.h"
#include "../world/Scenery.h"
#include "../world/TileElementsView.h"
#include "../world/Wall.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"

#include <algorithm>

using namespace OpenRCT2;

FootpathPlaceAction::FootpathPlaceAction(
    const CoordsXYZ& loc, uint8_t slope, ObjectEntryIndex type, ObjectEntryIndex railingsType, Direction direction,
    PathConstructFlags constructFlags)
    : _loc(loc)
    , _slope(slope)
    , _type(type)
    , _railingsType(railingsType)
    , _direction(direction)
    , _constructFlags(constructFlags)
{
}

void FootpathPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("object", _type);
    visitor.Visit("railingsObject", _railingsType);
    visitor.Visit("direction", _direction);
    visitor.Visit("slope", _slope);
    visitor.Visit("constructFlags", _constructFlags);
}

uint16_t FootpathPlaceAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void FootpathPlaceAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_slope) << DS_TAG(_type) << DS_TAG(_railingsType) << DS_TAG(_direction)
           << DS_TAG(_constructFlags);
}

GameActions::Result FootpathPlaceAction::Query() const
{
    auto res = GameActions::Result();
    res.Cost = 0;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = _loc.ToTileCentre();

    gFootpathGroundFlags = 0;

    if (!LocationValid(_loc) || MapIsEdge(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_BUILD_FOOTPATH_HERE, STR_OFF_EDGE_OF_MAP);
    }

    if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || GetGameState().Cheats.sandboxMode) && !MapIsLocationOwned(_loc))
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_BUILD_FOOTPATH_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (_slope & SLOPE_IS_IRREGULAR_FLAG)
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_BUILD_FOOTPATH_HERE, STR_LAND_SLOPE_UNSUITABLE);
    }

    if (_loc.z < kFootpathMinHeight)
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_BUILD_FOOTPATH_HERE, STR_TOO_LOW);
    }

    if (_loc.z > kFootpathMaxHeight)
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_BUILD_FOOTPATH_HERE, STR_TOO_HIGH);
    }

    if (_direction != INVALID_DIRECTION && !DirectionValid(_direction))
    {
        LOG_ERROR("Direction invalid. direction = %u", _direction);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_BUILD_FOOTPATH_HERE, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    auto intent = Intent(INTENT_ACTION_REMOVE_PROVISIONAL_FOOTPATH);
    ContextBroadcastIntent(&intent);

    auto tileElement = MapGetFootpathElementSlope(_loc, _slope);
    if (tileElement == nullptr)
    {
        return ElementInsertQuery(std::move(res));
    }
    return ElementUpdateQuery(tileElement, std::move(res));
}

GameActions::Result FootpathPlaceAction::Execute() const
{
    auto res = GameActions::Result();
    res.Cost = 0;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = _loc.ToTileCentre();

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        FootpathInterruptPeeps(_loc);
    }

    gFootpathGroundFlags = 0;

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        if (_direction != INVALID_DIRECTION && !GetGameState().Cheats.disableClearanceChecks)
        {
            // It is possible, let's remove walls between the old and new piece of path
            auto zLow = _loc.z;
            auto zHigh = zLow + kPathClearance;
            WallRemoveIntersectingWalls(
                { _loc, zLow, zHigh + ((_slope & kTileSlopeRaisedCornersMask) ? 16 : 0) }, DirectionReverse(_direction));
            WallRemoveIntersectingWalls(
                { _loc.x - CoordsDirectionDelta[_direction].x, _loc.y - CoordsDirectionDelta[_direction].y, zLow, zHigh },
                _direction);
        }
    }

    auto tileElement = MapGetFootpathElementSlope(_loc, _slope);
    if (tileElement == nullptr)
    {
        return ElementInsertExecute(std::move(res));
    }
    return ElementUpdateExecute(tileElement, std::move(res));
}

bool FootpathPlaceAction::IsSameAsPathElement(const PathElement* pathElement) const
{
    // Check if both this action and the element is queue
    if (pathElement->IsQueue() != ((_constructFlags & PathConstructFlag::IsQueue) != 0))
        return false;

    auto footpathObj = pathElement->GetLegacyPathEntry();
    if (footpathObj == nullptr)
    {
        if (_constructFlags & PathConstructFlag::IsLegacyPathObject)
        {
            return false;
        }

        return pathElement->GetSurfaceEntryIndex() == _type && pathElement->GetRailingsEntryIndex() == _railingsType;
    }

    if (_constructFlags & PathConstructFlag::IsLegacyPathObject)
    {
        return pathElement->GetLegacyPathEntryIndex() == _type;
    }

    return false;
}

bool FootpathPlaceAction::IsSameAsEntranceElement(const EntranceElement& entranceElement) const
{
    if (entranceElement.HasLegacyPathEntry())
    {
        if (_constructFlags & PathConstructFlag::IsLegacyPathObject)
        {
            return entranceElement.GetLegacyPathEntryIndex() == _type;
        }

        return false;
    }

    if (_constructFlags & PathConstructFlag::IsLegacyPathObject)
    {
        return false;
    }

    return entranceElement.GetSurfaceEntryIndex() == _type;
}

GameActions::Result FootpathPlaceAction::ElementUpdateQuery(PathElement* pathElement, GameActions::Result res) const
{
    if (!IsSameAsPathElement(pathElement))
    {
        res.Cost += 6.00_GBP;
    }

    if (GetFlags() & GAME_COMMAND_FLAG_GHOST && !pathElement->IsGhost())
    {
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_BUILD_FOOTPATH_HERE, STR_NONE);
    }
    return res;
}

GameActions::Result FootpathPlaceAction::ElementUpdateExecute(PathElement* pathElement, GameActions::Result res) const
{
    if (!IsSameAsPathElement(pathElement))
    {
        res.Cost += 6.00_GBP;
    }

    FootpathQueueChainReset();

    if (!(GetFlags() & GAME_COMMAND_FLAG_TRACK_DESIGN))
    {
        FootpathRemoveEdgesAt(_loc, reinterpret_cast<TileElement*>(pathElement));
    }

    if (_constructFlags & PathConstructFlag::IsLegacyPathObject)
    {
        pathElement->SetLegacyPathEntryIndex(_type);
    }
    else
    {
        pathElement->SetSurfaceEntryIndex(_type);
        pathElement->SetRailingsEntryIndex(_railingsType);
    }

    pathElement->SetIsQueue((_constructFlags & PathConstructFlag::IsQueue) != 0);

    auto* elem = pathElement->GetAdditionEntry();
    if (elem != nullptr)
    {
        if (_constructFlags & PathConstructFlag::IsQueue)
        {
            // remove any addition that isn't a TV or a lamp
            if ((elem->flags & PATH_ADDITION_FLAG_IS_QUEUE_SCREEN) == 0 && (elem->flags & PATH_ADDITION_FLAG_LAMP) == 0)
            {
                pathElement->SetIsBroken(false);
                pathElement->SetAddition(0);
            }
        }
        else
        {
            // remove all TVs
            if ((elem->flags & PATH_ADDITION_FLAG_IS_QUEUE_SCREEN) != 0)
            {
                pathElement->SetIsBroken(false);
                pathElement->SetAddition(0);
            }
        }
    }

    RemoveIntersectingWalls(pathElement);
    return res;
}

GameActions::Result FootpathPlaceAction::ElementInsertQuery(GameActions::Result res) const
{
    bool entrancePath = false, entranceIsSamePath = false;

    if (!MapCheckCapacityAndReorganise(_loc))
    {
        return GameActions::Result(GameActions::Status::NoFreeElements, STR_CANT_BUILD_FOOTPATH_HERE, STR_NONE);
    }

    res.Cost = 12.00_GBP;

    QuarterTile quarterTile{ 0b1111, 0 };
    auto zLow = _loc.z;
    auto zHigh = zLow + kPathClearance;
    if (_slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED)
    {
        quarterTile = QuarterTile{ 0b1111, 0b1100 }.Rotate(_slope & kTileElementDirectionMask);
        zHigh += kPathHeightStep;
    }

    auto entranceElement = MapGetParkEntranceElementAt(_loc, false);
    // Make sure the entrance part is the middle
    if (entranceElement != nullptr && (entranceElement->GetSequenceIndex()) == 0)
    {
        entrancePath = true;
        // Make the price the same as replacing a path
        if (IsSameAsEntranceElement(*entranceElement))
            entranceIsSamePath = true;
        else
            res.Cost -= 6.00_GBP;
    }

    // Do not attempt to build a crossing with a queue or a sloped path.
    auto isQueue = _constructFlags & PathConstructFlag::IsQueue;
    auto crossingMode = isQueue || (_slope != kTileSlopeFlat) ? CreateCrossingMode::none : CreateCrossingMode::pathOverTrack;
    auto canBuild = MapCanConstructWithClearAt(
        { _loc, zLow, zHigh }, &MapPlaceNonSceneryClearFunc, quarterTile, GetFlags(), crossingMode);
    if (!entrancePath && canBuild.Error != GameActions::Status::Ok)
    {
        canBuild.ErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
        return canBuild;
    }
    res.Cost += canBuild.Cost;

    const auto clearanceData = canBuild.GetData<ConstructClearResult>();

    gFootpathGroundFlags = clearanceData.GroundFlags;
    if (!GetGameState().Cheats.disableClearanceChecks && (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER))
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_CANT_BUILD_FOOTPATH_HERE, STR_CANT_BUILD_THIS_UNDERWATER);
    }

    auto surfaceElement = MapGetSurfaceElementAt(_loc);
    if (surfaceElement == nullptr)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_BUILD_FOOTPATH_HERE, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
    }
    int32_t supportHeight = zLow - surfaceElement->GetBaseZ();
    res.Cost += supportHeight < 0 ? 20.00_GBP : (supportHeight / kPathHeightStep) * 5.00_GBP;

    // Prevent the place sound from being spammed
    if (entranceIsSamePath)
        res.Cost = 0;

    return res;
}

GameActions::Result FootpathPlaceAction::ElementInsertExecute(GameActions::Result res) const
{
    bool entrancePath = false, entranceIsSamePath = false;

    if (!(GetFlags() & (GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST)))
    {
        FootpathRemoveLitter(_loc);
    }

    res.Cost = 12.00_GBP;

    QuarterTile quarterTile{ 0b1111, 0 };
    auto zLow = _loc.z;
    auto zHigh = zLow + kPathClearance;
    if (_slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED)
    {
        quarterTile = QuarterTile{ 0b1111, 0b1100 }.Rotate(_slope & kTileElementDirectionMask);
        zHigh += kPathHeightStep;
    }

    auto entranceElement = MapGetParkEntranceElementAt(_loc, false);
    // Make sure the entrance part is the middle
    if (entranceElement != nullptr && (entranceElement->GetSequenceIndex()) == 0)
    {
        entrancePath = true;
        // Make the price the same as replacing a path
        if (IsSameAsEntranceElement(*entranceElement))
            entranceIsSamePath = true;
        else
            res.Cost -= 6.00_GBP;
    }

    // Do not attempt to build a crossing with a queue or a sloped.
    auto isQueue = _constructFlags & PathConstructFlag::IsQueue;
    auto crossingMode = isQueue || (_slope != kTileSlopeFlat) ? CreateCrossingMode::none : CreateCrossingMode::pathOverTrack;
    auto canBuild = MapCanConstructWithClearAt(
        { _loc, zLow, zHigh }, &MapPlaceNonSceneryClearFunc, quarterTile, GAME_COMMAND_FLAG_APPLY | GetFlags(), crossingMode);
    if (!entrancePath && canBuild.Error != GameActions::Status::Ok)
    {
        canBuild.ErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
        return canBuild;
    }
    res.Cost += canBuild.Cost;

    const auto clearanceData = canBuild.GetData<ConstructClearResult>();
    gFootpathGroundFlags = clearanceData.GroundFlags;

    auto surfaceElement = MapGetSurfaceElementAt(_loc);
    if (surfaceElement == nullptr)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_BUILD_FOOTPATH_HERE, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
    }
    int32_t supportHeight = zLow - surfaceElement->GetBaseZ();
    res.Cost += supportHeight < 0 ? 20.00_GBP : (supportHeight / kPathHeightStep) * 5.00_GBP;

    if (entrancePath)
    {
        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && !entranceIsSamePath)
        {
            if (_constructFlags & PathConstructFlag::IsLegacyPathObject)
            {
                entranceElement->SetLegacyPathEntryIndex(_type);
            }
            else
            {
                entranceElement->SetSurfaceEntryIndex(_type);
            }
            MapInvalidateTileFull(_loc);
        }
    }
    else
    {
        auto* pathElement = TileElementInsert<PathElement>(_loc, 0b1111);
        Guard::Assert(pathElement != nullptr);

        pathElement->SetClearanceZ(zHigh);
        if (_constructFlags & PathConstructFlag::IsLegacyPathObject)
        {
            pathElement->SetLegacyPathEntryIndex(_type);
        }
        else
        {
            pathElement->SetSurfaceEntryIndex(_type);
            pathElement->SetRailingsEntryIndex(_railingsType);
        }
        pathElement->SetSlopeDirection(_slope & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK);
        pathElement->SetSloped(_slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED);
        pathElement->SetIsQueue(isQueue);
        pathElement->SetAddition(0);
        pathElement->SetRideIndex(RideId::GetNull());
        pathElement->SetAdditionStatus(255);
        pathElement->SetIsBroken(false);
        pathElement->SetGhost(GetFlags() & GAME_COMMAND_FLAG_GHOST);

        FootpathQueueChainReset();

        if (!(GetFlags() & GAME_COMMAND_FLAG_TRACK_DESIGN))
        {
            FootpathRemoveEdgesAt(_loc, pathElement->as<TileElement>());
        }
        if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            AutomaticallySetPeepSpawn();
        }

        RemoveIntersectingWalls(pathElement);
    }

    // Prevent the place sound from being spammed
    if (entranceIsSamePath)
        res.Cost = 0;

    return res;
}

/**
 *
 *  rct2: 0x006A65AD
 */
void FootpathPlaceAction::AutomaticallySetPeepSpawn() const
{
    auto mapSizeUnits = GetMapSizeUnits();
    uint8_t direction = 0;
    if (_loc.x != kCoordsXYStep)
    {
        direction++;
        if (_loc.y != mapSizeUnits.y - kCoordsXYStep)
        {
            direction++;
            if (_loc.x != mapSizeUnits.x - kCoordsXYStep)
            {
                direction++;
                if (_loc.y != kCoordsXYStep)
                    return;
            }
        }
    }

    auto& gameState = GetGameState();
    if (gameState.PeepSpawns.empty())
    {
        gameState.PeepSpawns.emplace_back();
    }
    PeepSpawn* peepSpawn = &gameState.PeepSpawns[0];
    peepSpawn->x = _loc.x + (DirectionOffsets[direction].x * 15) + 16;
    peepSpawn->y = _loc.y + (DirectionOffsets[direction].y * 15) + 16;
    peepSpawn->direction = direction;
    peepSpawn->z = _loc.z;
}

void FootpathPlaceAction::RemoveIntersectingWalls(PathElement* pathElement) const
{
    if (pathElement->IsSloped() && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        auto direction = pathElement->GetSlopeDirection();
        int32_t z = pathElement->GetBaseZ();
        WallRemoveIntersectingWalls({ _loc, z, z + (6 * kCoordsZStep) }, DirectionReverse(direction));
        WallRemoveIntersectingWalls({ _loc, z, z + (6 * kCoordsZStep) }, direction);
        // Removing walls may have made the pointer invalid, so find it again
        pathElement = MapGetFootpathElement(CoordsXYZ(_loc, z));
        if (pathElement == nullptr)
        {
            LOG_ERROR("Something went wrong. Could not refind footpath.");
            return;
        }
    }

    if (!(GetFlags() & GAME_COMMAND_FLAG_TRACK_DESIGN))
        FootpathConnectEdges(_loc, reinterpret_cast<TileElement*>(pathElement), GetFlags());

    FootpathUpdateQueueChains();
    MapInvalidateTileFull(_loc);
}

PathElement* FootpathPlaceAction::MapGetFootpathElementSlope(const CoordsXYZ& footpathPos, int32_t slope) const
{
    const bool isSloped = slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
    const auto slopeDirection = slope & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;

    for (auto* pathElement : TileElementsView<PathElement>(footpathPos))
    {
        if (pathElement->GetBaseZ() != footpathPos.z)
            continue;
        if (pathElement->IsSloped() != isSloped)
            continue;
        if (pathElement->GetSlopeDirection() != slopeDirection)
            continue;
        return pathElement;
    }

    return nullptr;
}
