/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WallRemoveAction.h"

#include "../Cheats.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/WallElement.h"

namespace OpenRCT2::GameActions
{
    WallRemoveAction::WallRemoveAction(const CoordsXYZD& loc)
        : _loc(loc)
    {
    }

    void WallRemoveAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_loc);
    }

    void WallRemoveAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc);
    }

    Result WallRemoveAction::Query(GameState_t& gameState) const
    {
        Result res = Result();
        res.cost = 0;
        res.expenditure = ExpenditureType::landscaping;

        if (!LocationValid(_loc))
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_OFF_EDGE_OF_MAP);
        }

        const bool isGhost = GetFlags().has(CommandFlag::ghost);
        if (!isGhost && gLegacyScene != LegacyScene::scenarioEditor && !gameState.cheats.sandboxMode
            && !MapIsLocationOwned(_loc))
        {
            return Result(Status::notOwned, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        TileElement* wallElement = GetFirstWallElementAt(_loc, isGhost);
        if (wallElement == nullptr)
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res.cost = 0;
        return res;
    }

    Result WallRemoveAction::Execute(GameState_t& gameState) const
    {
        Result res = Result();
        res.cost = 0;
        res.expenditure = ExpenditureType::landscaping;

        const bool isGhost = GetFlags().has(CommandFlag::ghost);

        TileElement* wallElement = GetFirstWallElementAt(_loc, isGhost);
        if (wallElement == nullptr)
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res.position.x = _loc.x + 16;
        res.position.y = _loc.y + 16;
        res.position.z = _loc.z;

        wallElement->RemoveBannerEntry();
        MapInvalidateTileZoom1({ _loc, wallElement->GetBaseZ(), (wallElement->GetBaseZ()) + 72 });
        TileElementRemove(wallElement);

        return res;
    }

    TileElement* WallRemoveAction::GetFirstWallElementAt(const CoordsXYZD& location, bool isGhost) const
    {
        for (auto* wallElement : TileElementsView<WallElement>(location))
        {
            if (wallElement->GetBaseZ() != location.z)
                continue;
            if (wallElement->GetDirection() != location.direction)
                continue;
            if (wallElement->IsGhost() != isGhost)
                continue;

            return wallElement->as<TileElement>();
        }
        return nullptr;
    }
} // namespace OpenRCT2::GameActions
