/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathAdditionRemoveAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/tile_element/PathElement.h"

namespace OpenRCT2::GameActions
{
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

    Result FootpathAdditionRemoveAction::Query(GameState_t& gameState) const
    {
        if (!LocationValid(_loc))
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_OFF_EDGE_OF_MAP);
        }

        if (!(gLegacyScene == LegacyScene::scenarioEditor || gameState.cheats.sandboxMode) && !MapIsLocationOwned(_loc))
        {
            return Result(Status::disallowed, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        if (_loc.z < kFootpathMinHeight)
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_TOO_LOW);
        }

        if (_loc.z > kFootpathMaxHeight)
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_TOO_HIGH);
        }

        auto tileElement = MapGetFootpathElement(_loc);
        if (tileElement == nullptr)
        {
            LOG_ERROR("No path element at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_ERR_PATH_ELEMENT_NOT_FOUND);
        }

        auto pathElement = tileElement->AsPath();
        if (pathElement == nullptr)
        {
            LOG_ERROR("No path element at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_ERR_PATH_ELEMENT_NOT_FOUND);
        }

        if (!pathElement->AdditionIsGhost() && (GetFlags().has(CommandFlag::ghost)))
        {
            LOG_WARNING("Tried to remove non ghost during ghost removal.");
            return Result(Status::disallowed, STR_CANT_REMOVE_THIS, kStringIdNone);
        }
        auto res = Result();
        res.position = _loc;
        res.cost = 0.00_GBP;
        return res;
    }

    Result FootpathAdditionRemoveAction::Execute(GameState_t& gameState) const
    {
        auto* pathElement = MapGetFootpathElement(_loc);
        if (!GetFlags().has(CommandFlag::ghost))
        {
            FootpathInterruptPeeps(_loc);
        }

        if (pathElement == nullptr)
        {
            LOG_ERROR("No path element at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_ERR_PATH_ELEMENT_NOT_FOUND);
        }

        pathElement->SetAddition(0);
        MapInvalidateTileFull(_loc);

        auto res = Result();
        res.position = _loc;
        res.cost = 0.00_GBP;
        return res;
    }
} // namespace OpenRCT2::GameActions
