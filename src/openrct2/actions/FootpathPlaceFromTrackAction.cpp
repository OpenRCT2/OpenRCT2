/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathPlaceFromTrackAction.h"

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideConstruction.h"
#include "../world/ConstructionClearance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Park.h"
#include "../world/Surface.h"
#include "../world/Wall.h"

FootpathPlaceFromTrackAction::FootpathPlaceFromTrackAction(
    const CoordsXYZ& loc, uint8_t slope, ObjectEntryIndex type, ObjectEntryIndex railingsType, uint8_t edges,
    PathConstructFlags constructFlags)
    : _loc(loc)
    , _slope(slope)
    , _type(type)
    , _railingsType(railingsType)
    , _edges(edges)
    , _constructFlags(constructFlags)
{
}

void FootpathPlaceFromTrackAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_slope) << DS_TAG(_type) << DS_TAG(_railingsType) << DS_TAG(_edges)
           << DS_TAG(_constructFlags);
}

uint16_t FootpathPlaceFromTrackAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

GameActions::Result FootpathPlaceFromTrackAction::Query() const
{
    auto res = GameActions::Result();
    res.Cost = 0;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = _loc.ToTileCentre();

    gFootpathGroundFlags = 0;

    if (!LocationValid(_loc) || map_is_edge(_loc))
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_OFF_EDGE_OF_MAP);
    }

    if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(_loc))
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (_loc.z < FootpathMinHeight)
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_LOW);
    }

    if (_loc.z > FootpathMaxHeight)
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_HIGH);
    }

    return ElementInsertQuery(std::move(res));
}

GameActions::Result FootpathPlaceFromTrackAction::Execute() const
{
    auto res = GameActions::Result();
    res.Cost = 0;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = _loc.ToTileCentre();

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        footpath_interrupt_peeps(_loc);
    }

    gFootpathGroundFlags = 0;

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    return ElementInsertExecute(std::move(res));
}

GameActions::Result FootpathPlaceFromTrackAction::ElementInsertQuery(GameActions::Result res) const
{
    bool entrancePath = false, entranceIsSamePath = false;

    if (!MapCheckCapacityAndReorganise(_loc))
    {
        return GameActions::Result(
            GameActions::Status::NoFreeElements, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
            STR_TILE_ELEMENT_LIMIT_REACHED);
    }

    res.Cost = MONEY(12, 00);

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
        if (IsSameAsEntranceElement(*entranceElement))
            entranceIsSamePath = true;
        else
            res.Cost -= MONEY(6, 00);
    }

    // Do not attempt to build a crossing with a queue or a sloped path.
    auto isQueue = _constructFlags & PathConstructFlag::IsQueue;
    uint8_t crossingMode = isQueue || (_slope != TILE_ELEMENT_SLOPE_FLAT) ? CREATE_CROSSING_MODE_NONE
                                                                          : CREATE_CROSSING_MODE_PATH_OVER_TRACK;
    auto canBuild = MapCanConstructWithClearAt(
        { _loc, zLow, zHigh }, &map_place_non_scenery_clear_func, quarterTile, GetFlags(), crossingMode);
    if (!entrancePath && canBuild.Error != GameActions::Status::Ok)
    {
        canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
        return canBuild;
    }
    res.Cost += canBuild.Cost;

    const auto clearanceData = canBuild.GetData<ConstructClearResult>();
    gFootpathGroundFlags = clearanceData.GroundFlags;

    if (!gCheatsDisableClearanceChecks && (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER))
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_CANT_BUILD_THIS_UNDERWATER);
    }

    auto surfaceElement = map_get_surface_element_at(_loc);
    if (surfaceElement == nullptr)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }
    int32_t supportHeight = zLow - surfaceElement->GetBaseZ();
    res.Cost += supportHeight < 0 ? MONEY(20, 00) : (supportHeight / PATH_HEIGHT_STEP) * MONEY(5, 00);

    // Prevent the place sound from being spammed
    if (entranceIsSamePath)
        res.Cost = 0;

    return res;
}

GameActions::Result FootpathPlaceFromTrackAction::ElementInsertExecute(GameActions::Result res) const
{
    bool entrancePath = false, entranceIsSamePath = false;

    if (!(GetFlags() & (GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST)))
    {
        footpath_remove_litter(_loc);
    }

    res.Cost = MONEY(12, 00);

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
        if (IsSameAsEntranceElement(*entranceElement))
            entranceIsSamePath = true;
        else
            res.Cost -= MONEY(6, 00);
    }

    // Do not attempt to build a crossing with a queue or a sloped path.
    auto isQueue = _constructFlags & PathConstructFlag::IsQueue;
    uint8_t crossingMode = isQueue || (_slope != TILE_ELEMENT_SLOPE_FLAT) ? CREATE_CROSSING_MODE_NONE
                                                                          : CREATE_CROSSING_MODE_PATH_OVER_TRACK;
    auto canBuild = MapCanConstructWithClearAt(
        { _loc, zLow, zHigh }, &map_place_non_scenery_clear_func, quarterTile, GAME_COMMAND_FLAG_APPLY | GetFlags(),
        crossingMode);
    if (!entrancePath && canBuild.Error != GameActions::Status::Ok)
    {
        canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
        return canBuild;
    }
    res.Cost += canBuild.Cost;

    const auto clearanceData = canBuild.GetData<ConstructClearResult>();
    gFootpathGroundFlags = clearanceData.GroundFlags;

    auto surfaceElement = map_get_surface_element_at(_loc);
    if (surfaceElement == nullptr)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }
    int32_t supportHeight = zLow - surfaceElement->GetBaseZ();
    res.Cost += supportHeight < 0 ? MONEY(20, 00) : (supportHeight / PATH_HEIGHT_STEP) * MONEY(5, 00);

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
            map_invalidate_tile_full(_loc);
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
        pathElement->SetEdges(_edges);
        pathElement->SetCorners(0);
        pathElement->SetGhost(GetFlags() & GAME_COMMAND_FLAG_GHOST);

        map_invalidate_tile_full(_loc);
    }

    // Prevent the place sound from being spammed
    if (entranceIsSamePath)
        res.Cost = 0;

    return res;
}

bool FootpathPlaceFromTrackAction::IsSameAsEntranceElement(const EntranceElement& entranceElement) const
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
