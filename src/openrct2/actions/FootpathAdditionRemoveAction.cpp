/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathAdditionRemoveAction.h"

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Park.h"
#include "../world/Wall.h"

FootpathAdditionRemoveAction::FootpathAdditionRemoveAction(const CoordsXYZ& loc)
    : _loc(loc)
{
}

void FootpathAdditionRemoveAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
}

uint16_t FootpathAdditionRemoveAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void FootpathAdditionRemoveAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc);
}

GameActions::Result FootpathAdditionRemoveAction::Query() const
{
    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_OFF_EDGE_OF_MAP);
    }

    if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(_loc))
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (_loc.z < FootpathMinHeight)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_TOO_LOW);
    }

    if (_loc.z > FootpathMaxHeight)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_TOO_HIGH);
    }

    auto tileElement = MapGetFootpathElement(_loc);
    if (tileElement == nullptr)
    {
        log_warning("Could not find path element.");
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_NONE);
    }

    auto pathElement = tileElement->AsPath();
    if (pathElement == nullptr)
    {
        log_warning("Could not find path element.");
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_NONE);
    }

    if (!pathElement->AdditionIsGhost() && (GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        log_warning("Tried to remove non ghost during ghost removal.");
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_REMOVE_THIS, STR_NONE);
    }
    auto res = GameActions::Result();
    res.Position = _loc;
    res.Cost = 0.00_GBP;
    return res;
}

GameActions::Result FootpathAdditionRemoveAction::Execute() const
{
    auto tileElement = MapGetFootpathElement(_loc);
    auto pathElement = tileElement->AsPath();

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        FootpathInterruptPeeps(_loc);
    }

    if (pathElement == nullptr)
    {
        log_error("Could not find path element.");
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_NONE);
    }

    pathElement->SetAddition(0);
    map_invalidate_tile_full(_loc);

    auto res = GameActions::Result();
    res.Position = _loc;
    res.Cost = 0.00_GBP;
    return res;
}
