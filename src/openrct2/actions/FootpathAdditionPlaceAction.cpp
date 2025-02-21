/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathAdditionPlaceAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../object/ObjectEntryManager.h"
#include "../object/PathAdditionEntry.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/tile_element/PathElement.h"

using namespace OpenRCT2;

FootpathAdditionPlaceAction::FootpathAdditionPlaceAction(const CoordsXYZ& loc, ObjectEntryIndex pathItemType)
    : _loc(loc)
    , _entryIndex(pathItemType)
{
}

void FootpathAdditionPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("object", _entryIndex);
}

uint16_t FootpathAdditionPlaceAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void FootpathAdditionPlaceAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_entryIndex);
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

    if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || GetGameState().Cheats.sandboxMode) && !MapIsLocationOwned(_loc))
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (_loc.z < kFootpathMinHeight)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_TOO_LOW);
    }

    if (_loc.z > kFootpathMaxHeight)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_TOO_HIGH);
    }

    auto tileElement = MapGetFootpathElement(_loc);
    if (tileElement == nullptr)
    {
        LOG_ERROR("No path element at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_ERR_PATH_ELEMENT_NOT_FOUND);
    }

    auto pathElement = tileElement->AsPath();
    if (pathElement->IsLevelCrossing(_loc))
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE,
            STR_CANNOT_BUILD_PATH_ADDITIONS_ON_LEVEL_CROSSINGS);
    }

    // No change
    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && pathElement->GetAdditionEntryIndex() == _entryIndex
        && !(pathElement->IsBroken()))
    {
        return res;
    }

    auto* pathAdditionEntry = OpenRCT2::ObjectManager::GetObjectEntry<PathAdditionEntry>(_entryIndex);
    if (pathAdditionEntry == nullptr)
    {
        LOG_ERROR("Unknown footpath addition entry for entryIndex %d", _entryIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
    }
    uint16_t sceneryFlags = pathAdditionEntry->flags;

    if ((sceneryFlags & PATH_ADDITION_FLAG_DONT_ALLOW_ON_SLOPE) && pathElement->IsSloped())
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_CANT_BUILD_THIS_ON_SLOPED_FOOTPATH);
    }

    if ((sceneryFlags & PATH_ADDITION_FLAG_DONT_ALLOW_ON_QUEUE) && pathElement->IsQueue())
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_CANNOT_PLACE_THESE_ON_QUEUE_LINE_AREA);
    }

    if (!(sceneryFlags & (PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_WATER | PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_SNOW))
        && (pathElement->GetEdges()) == 0x0F)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_CAN_ONLY_BE_PLACED_ON_PATH_EDGES);
    }

    if ((sceneryFlags & PATH_ADDITION_FLAG_IS_QUEUE_SCREEN) && !pathElement->IsQueue())
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_CAN_ONLY_PLACE_THESE_ON_QUEUE_AREA);
    }

    res.Cost = pathAdditionEntry->price;

    // Should place a ghost?
    if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
    {
        // Check if there is something on the path already
        if (pathElement->HasAddition())
        {
            return GameActions::Result(GameActions::Status::ItemAlreadyPlaced, STR_CANT_POSITION_THIS_HERE, kStringIdNone);
        }
    }
    return res;
}

GameActions::Result FootpathAdditionPlaceAction::Execute() const
{
    auto res = GameActions::Result();
    res.Position = _loc;
    res.Expenditure = ExpenditureType::Landscaping;

    auto tileElement = MapGetFootpathElement(_loc);
    auto pathElement = tileElement->AsPath();

    if (pathElement == nullptr)
    {
        LOG_ERROR("No path element at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_ERR_PATH_ELEMENT_NOT_FOUND);
    }

    // No change
    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && pathElement->GetAdditionEntryIndex() == _entryIndex
        && !(pathElement->IsBroken()) && !pathElement->AdditionIsGhost())
    {
        return res;
    }

    auto* pathAdditionEntry = OpenRCT2::ObjectManager::GetObjectEntry<PathAdditionEntry>(_entryIndex);
    if (pathAdditionEntry == nullptr)
    {
        LOG_ERROR("Unknown footpath addition entry for entryIndex %d", _entryIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
    }

    res.Cost = pathAdditionEntry->price;

    if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
    {
        pathElement->SetAdditionIsGhost(true);
    }
    else
    {
        FootpathInterruptPeeps(_loc);
        pathElement->SetAdditionIsGhost(false);
    }

    pathElement->SetAdditionEntryIndex(_entryIndex);
    pathElement->SetIsBroken(false);
    if (pathAdditionEntry->flags & PATH_ADDITION_FLAG_IS_BIN)
    {
        pathElement->SetAdditionStatus(255);
    }
    MapInvalidateTileFull(_loc);
    return res;
}
