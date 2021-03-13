/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathPlaceAction.h"

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"
#include "../world/TileElementsView.h"
#include "../world/Wall.h"

using namespace OpenRCT2;

FootpathPlaceAction::FootpathPlaceAction(const CoordsXYZ& loc, uint8_t slope, ObjectEntryIndex type, Direction direction)
    : _loc(loc)
    , _slope(slope)
    , _type(type)
    , _direction(direction)
{
}

void FootpathPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("object", _type);
    visitor.Visit("direction", _direction);
    visitor.Visit("slope", _slope);
}

uint16_t FootpathPlaceAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void FootpathPlaceAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_slope) << DS_TAG(_type) << DS_TAG(_direction);
}

GameActions::Result::Ptr FootpathPlaceAction::Query() const
{
    GameActions::Result::Ptr res = std::make_unique<GameActions::Result>();
    res->Cost = 0;
    res->Expenditure = ExpenditureType::Landscaping;
    res->Position = _loc.ToTileCentre();

    gFootpathGroundFlags = 0;

    if (!LocationValid(_loc) || map_is_edge(_loc))
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_BUILD_FOOTPATH_HERE, STR_OFF_EDGE_OF_MAP);
    }

    if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(_loc))
    {
        return MakeResult(GameActions::Status::Disallowed, STR_CANT_BUILD_FOOTPATH_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (_slope & SLOPE_IS_IRREGULAR_FLAG)
    {
        return MakeResult(GameActions::Status::Disallowed, STR_CANT_BUILD_FOOTPATH_HERE, STR_LAND_SLOPE_UNSUITABLE);
    }

    if (_loc.z < FootpathMinHeight)
    {
        return MakeResult(GameActions::Status::Disallowed, STR_CANT_BUILD_FOOTPATH_HERE, STR_TOO_LOW);
    }

    if (_loc.z > FootpathMaxHeight)
    {
        return MakeResult(GameActions::Status::Disallowed, STR_CANT_BUILD_FOOTPATH_HERE, STR_TOO_HIGH);
    }

    if (_direction != INVALID_DIRECTION && !direction_valid(_direction))
    {
        log_error("Direction invalid. direction = %u", _direction);
        return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_BUILD_FOOTPATH_HERE);
    }

    footpath_provisional_remove();
    auto tileElement = map_get_footpath_element_slope(_loc, _slope);
    if (tileElement == nullptr)
    {
        return ElementInsertQuery(std::move(res));
    }
    else
    {
        return ElementUpdateQuery(tileElement, std::move(res));
    }
}

GameActions::Result::Ptr FootpathPlaceAction::Execute() const
{
    GameActions::Result::Ptr res = std::make_unique<GameActions::Result>();
    res->Cost = 0;
    res->Expenditure = ExpenditureType::Landscaping;
    res->Position = _loc.ToTileCentre();

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        footpath_interrupt_peeps(_loc);
    }

    gFootpathGroundFlags = 0;

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        if (_direction != INVALID_DIRECTION && !gCheatsDisableClearanceChecks)
        {
            // It is possible, let's remove walls between the old and new piece of path
            auto zLow = _loc.z;
            auto zHigh = zLow + PATH_CLEARANCE;
            wall_remove_intersecting_walls(
                { _loc, zLow, zHigh + ((_slope & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK) ? 16 : 0) },
                direction_reverse(_direction));
            wall_remove_intersecting_walls(
                { _loc.x - CoordsDirectionDelta[_direction].x, _loc.y - CoordsDirectionDelta[_direction].y, zLow, zHigh },
                _direction);
        }
    }

    auto tileElement = map_get_footpath_element_slope(_loc, _slope);
    if (tileElement == nullptr)
    {
        return ElementInsertExecute(std::move(res));
    }
    else
    {
        return ElementUpdateExecute(tileElement, std::move(res));
    }
}

GameActions::Result::Ptr FootpathPlaceAction::ElementUpdateQuery(PathElement* pathElement, GameActions::Result::Ptr res) const
{
    const int32_t newFootpathType = (_type & (FOOTPATH_PROPERTIES_TYPE_MASK >> 4));
    const bool newPathIsQueue = ((_type >> 7) == 1);
    if (pathElement->GetSurfaceEntryIndex() != newFootpathType || pathElement->IsQueue() != newPathIsQueue)
    {
        res->Cost += MONEY(6, 00);
    }

    if (GetFlags() & GAME_COMMAND_FLAG_GHOST && !pathElement->IsGhost())
    {
        return MakeResult(GameActions::Status::Unknown, STR_CANT_BUILD_FOOTPATH_HERE);
    }
    return res;
}

GameActions::Result::Ptr FootpathPlaceAction::ElementUpdateExecute(PathElement* pathElement, GameActions::Result::Ptr res) const
{
    const int32_t newFootpathType = (_type & (FOOTPATH_PROPERTIES_TYPE_MASK >> 4));
    const bool newPathIsQueue = ((_type >> 7) == 1);
    if (pathElement->GetSurfaceEntryIndex() != newFootpathType || pathElement->IsQueue() != newPathIsQueue)
    {
        res->Cost += MONEY(6, 00);
    }

    footpath_queue_chain_reset();

    if (!(GetFlags() & GAME_COMMAND_FLAG_PATH_SCENERY))
    {
        footpath_remove_edges_at(_loc, reinterpret_cast<TileElement*>(pathElement));
    }

    pathElement->SetSurfaceEntryIndex(_type & ~FOOTPATH_ELEMENT_INSERT_QUEUE);
    bool isQueue = _type & FOOTPATH_ELEMENT_INSERT_QUEUE;
    pathElement->SetIsQueue(isQueue);

    rct_scenery_entry* elem = pathElement->GetAdditionEntry();
    if (elem != nullptr)
    {
        if (isQueue)
        {
            // remove any addition that isn't a TV or a lamp
            if ((elem->path_bit.flags & PATH_BIT_FLAG_IS_QUEUE_SCREEN) == 0 && (elem->path_bit.flags & PATH_BIT_FLAG_LAMP) == 0)
            {
                pathElement->SetIsBroken(false);
                pathElement->SetAddition(0);
            }
        }
        else
        {
            // remove all TVs
            if ((elem->path_bit.flags & PATH_BIT_FLAG_IS_QUEUE_SCREEN) != 0)
            {
                pathElement->SetIsBroken(false);
                pathElement->SetAddition(0);
            }
        }
    }

    RemoveIntersectingWalls(pathElement);
    return res;
}

GameActions::Result::Ptr FootpathPlaceAction::ElementInsertQuery(GameActions::Result::Ptr res) const
{
    bool entrancePath = false, entranceIsSamePath = false;

    if (!map_check_free_elements_and_reorganise(1))
    {
        return MakeResult(GameActions::Status::NoFreeElements, STR_CANT_BUILD_FOOTPATH_HERE);
    }

    res->Cost = MONEY(12, 00);

    QuarterTile quarterTile{ 0b1111, 0 };
    auto zLow = _loc.z;
    auto zHigh = zLow + PATH_CLEARANCE;
    if (_slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED)
    {
        quarterTile = QuarterTile{ 0b1111, 0b1100 }.Rotate(_slope & TILE_ELEMENT_DIRECTION_MASK);
        zHigh += PATH_HEIGHT_STEP;
    }

    auto entranceElement = map_get_park_entrance_element_at(_loc, false);
    // Make sure the entrance part is the middle
    if (entranceElement != nullptr && (entranceElement->GetSequenceIndex()) == 0)
    {
        entrancePath = true;
        // Make the price the same as replacing a path
        if (entranceElement->GetPathType() == (_type & 0xF))
            entranceIsSamePath = true;
        else
            res->Cost -= MONEY(6, 00);
    }

    // Do not attempt to build a crossing with a queue or a sloped.
    uint8_t crossingMode = (_type & FOOTPATH_ELEMENT_INSERT_QUEUE) || (_slope != TILE_ELEMENT_SLOPE_FLAT)
        ? CREATE_CROSSING_MODE_NONE
        : CREATE_CROSSING_MODE_PATH_OVER_TRACK;
    if (!entrancePath
        && !map_can_construct_with_clear_at(
            { _loc, zLow, zHigh }, &map_place_non_scenery_clear_func, quarterTile, GetFlags(), &res->Cost, crossingMode))
    {
        return MakeResult(
            GameActions::Status::NoClearance, STR_CANT_BUILD_FOOTPATH_HERE, gGameCommandErrorText, gCommonFormatArgs);
    }

    gFootpathGroundFlags = gMapGroundFlags;
    if (!gCheatsDisableClearanceChecks && (gMapGroundFlags & ELEMENT_IS_UNDERWATER))
    {
        return MakeResult(GameActions::Status::Disallowed, STR_CANT_BUILD_FOOTPATH_HERE, STR_CANT_BUILD_THIS_UNDERWATER);
    }

    auto surfaceElement = map_get_surface_element_at(_loc);
    if (surfaceElement == nullptr)
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_BUILD_FOOTPATH_HERE);
    }
    int32_t supportHeight = zLow - surfaceElement->GetBaseZ();
    res->Cost += supportHeight < 0 ? MONEY(20, 00) : (supportHeight / PATH_HEIGHT_STEP) * MONEY(5, 00);

    // Prevent the place sound from being spammed
    if (entranceIsSamePath)
        res->Cost = 0;

    return res;
}

GameActions::Result::Ptr FootpathPlaceAction::ElementInsertExecute(GameActions::Result::Ptr res) const
{
    bool entrancePath = false, entranceIsSamePath = false;

    if (!(GetFlags() & (GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST)))
    {
        footpath_remove_litter(_loc);
    }

    res->Cost = MONEY(12, 00);

    QuarterTile quarterTile{ 0b1111, 0 };
    auto zLow = _loc.z;
    auto zHigh = zLow + PATH_CLEARANCE;
    if (_slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED)
    {
        quarterTile = QuarterTile{ 0b1111, 0b1100 }.Rotate(_slope & TILE_ELEMENT_DIRECTION_MASK);
        zHigh += PATH_HEIGHT_STEP;
    }

    auto entranceElement = map_get_park_entrance_element_at(_loc, false);
    // Make sure the entrance part is the middle
    if (entranceElement != nullptr && (entranceElement->GetSequenceIndex()) == 0)
    {
        entrancePath = true;
        // Make the price the same as replacing a path
        if (entranceElement->GetPathType() == (_type & 0xF))
            entranceIsSamePath = true;
        else
            res->Cost -= MONEY(6, 00);
    }

    // Do not attempt to build a crossing with a queue or a sloped.
    uint8_t crossingMode = (_type & FOOTPATH_ELEMENT_INSERT_QUEUE) || (_slope != TILE_ELEMENT_SLOPE_FLAT)
        ? CREATE_CROSSING_MODE_NONE
        : CREATE_CROSSING_MODE_PATH_OVER_TRACK;
    if (!entrancePath
        && !map_can_construct_with_clear_at(
            { _loc, zLow, zHigh }, &map_place_non_scenery_clear_func, quarterTile, GAME_COMMAND_FLAG_APPLY | GetFlags(),
            &res->Cost, crossingMode))
    {
        return MakeResult(
            GameActions::Status::NoClearance, STR_CANT_BUILD_FOOTPATH_HERE, gGameCommandErrorText, gCommonFormatArgs);
    }

    gFootpathGroundFlags = gMapGroundFlags;

    auto surfaceElement = map_get_surface_element_at(_loc);
    if (surfaceElement == nullptr)
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_BUILD_FOOTPATH_HERE);
    }
    int32_t supportHeight = zLow - surfaceElement->GetBaseZ();
    res->Cost += supportHeight < 0 ? MONEY(20, 00) : (supportHeight / PATH_HEIGHT_STEP) * MONEY(5, 00);

    if (entrancePath)
    {
        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && !entranceIsSamePath)
        {
            // Set the path type but make sure it's not a queue as that will not show up
            entranceElement->SetPathType(_type & 0x7F);
            map_invalidate_tile_full(_loc);
        }
    }
    else
    {
        auto* pathElement = TileElementInsert<PathElement>(_loc, 0b1111);
        Guard::Assert(pathElement != nullptr);

        pathElement->SetClearanceZ(zHigh);
        pathElement->SetSurfaceEntryIndex(_type & ~FOOTPATH_ELEMENT_INSERT_QUEUE);
        pathElement->SetSlopeDirection(_slope & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK);
        pathElement->SetSloped(_slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED);
        pathElement->SetIsQueue(_type & FOOTPATH_ELEMENT_INSERT_QUEUE);
        pathElement->SetAddition(0);
        pathElement->SetRideIndex(RIDE_ID_NULL);
        pathElement->SetAdditionStatus(255);
        pathElement->SetIsBroken(false);
        pathElement->SetGhost(GetFlags() & GAME_COMMAND_FLAG_GHOST);

        footpath_queue_chain_reset();

        if (!(GetFlags() & GAME_COMMAND_FLAG_PATH_SCENERY))
        {
            footpath_remove_edges_at(_loc, pathElement->as<TileElement>());
        }
        if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            AutomaticallySetPeepSpawn();
        }

        RemoveIntersectingWalls(pathElement);
    }

    // Prevent the place sound from being spammed
    if (entranceIsSamePath)
        res->Cost = 0;

    return res;
}

/**
 *
 *  rct2: 0x006A65AD
 */
void FootpathPlaceAction::AutomaticallySetPeepSpawn() const
{
    uint8_t direction = 0;
    if (_loc.x != 32)
    {
        direction++;
        if (_loc.y != gMapSizeUnits - 32)
        {
            direction++;
            if (_loc.x != gMapSizeUnits - 32)
            {
                direction++;
                if (_loc.y != 32)
                    return;
            }
        }
    }

    if (gPeepSpawns.empty())
    {
        gPeepSpawns.emplace_back();
    }
    PeepSpawn* peepSpawn = &gPeepSpawns[0];
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
        wall_remove_intersecting_walls({ _loc, z, z + (6 * COORDS_Z_STEP) }, direction_reverse(direction));
        wall_remove_intersecting_walls({ _loc, z, z + (6 * COORDS_Z_STEP) }, direction);
        // Removing walls may have made the pointer invalid, so find it again
        auto tileElement = map_get_footpath_element(CoordsXYZ(_loc, z));
        if (tileElement == nullptr)
        {
            log_error("Something went wrong. Could not refind footpath.");
            return;
        }
        pathElement = tileElement->AsPath();
    }

    if (!(GetFlags() & GAME_COMMAND_FLAG_PATH_SCENERY))
        footpath_connect_edges(_loc, reinterpret_cast<TileElement*>(pathElement), GetFlags());

    footpath_update_queue_chains();
    map_invalidate_tile_full(_loc);
}

PathElement* FootpathPlaceAction::map_get_footpath_element_slope(const CoordsXYZ& footpathPos, int32_t slope) const
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
