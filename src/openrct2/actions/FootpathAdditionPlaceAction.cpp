/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathAdditionPlaceAction.h"

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
#include "../world/Wall.h"

FootpathAdditionPlaceAction::FootpathAdditionPlaceAction(const CoordsXYZ& loc, ObjectEntryIndex pathItemType)
    : _loc(loc)
    , _pathItemType(pathItemType)
{
}

void FootpathAdditionPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("object", _pathItemType);
}

uint16_t FootpathAdditionPlaceAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void FootpathAdditionPlaceAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_pathItemType);
}

GameActions::Result FootpathAdditionPlaceAction::Query() const
{
    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = _loc;
    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_OFF_EDGE_OF_MAP);
    }

    if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(_loc))
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (_loc.z < FootpathMinHeight)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_TOO_LOW);
    }

    if (_loc.z > FootpathMaxHeight)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_TOO_HIGH);
    }

    auto tileElement = map_get_footpath_element(_loc);
    if (tileElement == nullptr)
    {
        log_error("Could not find path element.");
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_NONE);
    }

    auto pathElement = tileElement->AsPath();
    if (pathElement->IsLevelCrossing(_loc))
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE,
            STR_CANNOT_BUILD_PATH_ADDITIONS_ON_LEVEL_CROSSINGS);
    }

    // No change
    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && pathElement->GetAddition() == _pathItemType && !(pathElement->IsBroken()))
    {
        return res;
    }

    if (_pathItemType != 0)
    {
        auto* pathBitEntry = get_footpath_item_entry(_pathItemType - 1);
        if (pathBitEntry == nullptr)
        {
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_NONE);
        }
        uint16_t sceneryFlags = pathBitEntry->flags;

        if ((sceneryFlags & PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE) && pathElement->IsSloped())
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_CANT_BUILD_THIS_ON_SLOPED_FOOTPATH);
        }

        if ((sceneryFlags & PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE) && pathElement->IsQueue())
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_CANNOT_PLACE_THESE_ON_QUEUE_LINE_AREA);
        }

        if (!(sceneryFlags & (PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER | PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW))
            && (pathElement->GetEdges()) == 0x0F)
        {
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_NONE);
        }

        if ((sceneryFlags & PATH_BIT_FLAG_IS_QUEUE_SCREEN) && !pathElement->IsQueue())
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_CAN_ONLY_PLACE_THESE_ON_QUEUE_AREA);
        }

        res.Cost = pathBitEntry->price;
    }

    // Should place a ghost?
    if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
    {
        // Check if there is something on the path already
        if (pathElement->HasAddition())
        {
            return GameActions::Result(GameActions::Status::ItemAlreadyPlaced, STR_CANT_POSITION_THIS_HERE, STR_NONE);
        }
    }
    return res;
}

GameActions::Result FootpathAdditionPlaceAction::Execute() const
{
    auto res = GameActions::Result();
    res.Position = _loc;
    res.Expenditure = ExpenditureType::Landscaping;

    auto tileElement = map_get_footpath_element(_loc);
    auto pathElement = tileElement->AsPath();

    if (pathElement == nullptr)
    {
        log_error("Could not find path element.");
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_NONE);
    }

    // No change
    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && pathElement->GetAddition() == _pathItemType && !(pathElement->IsBroken())
        && !pathElement->AdditionIsGhost())
    {
        return res;
    }

    if (_pathItemType != 0)
    {
        auto* pathBitEntry = get_footpath_item_entry(_pathItemType - 1);
        if (pathBitEntry == nullptr)
        {
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_NONE);
        }

        res.Cost = pathBitEntry->price;
    }

    if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
    {
        pathElement->SetAdditionIsGhost(true);
    }
    else
    {
        footpath_interrupt_peeps(_loc);
    }

    if ((_pathItemType != 0 && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        || (_pathItemType == 0 && pathElement->AdditionIsGhost()))
    {
        pathElement->SetAdditionIsGhost(false);
    }

    pathElement->SetAddition(_pathItemType);
    pathElement->SetIsBroken(false);
    if (_pathItemType != 0)
    {
        auto* pathBitEntry = get_footpath_item_entry(_pathItemType - 1);
        if (pathBitEntry != nullptr && pathBitEntry->flags & PATH_BIT_FLAG_IS_BIN)
        {
            pathElement->SetAdditionStatus(255);
        }
    }
    map_invalidate_tile_full(_loc);
    return res;
}
