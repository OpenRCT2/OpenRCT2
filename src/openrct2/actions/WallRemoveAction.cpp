/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WallRemoveAction.h"

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Location.hpp"
#include "../world/TileElementsView.h"
#include "../world/Wall.h"

namespace OpenRCT2
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

    GameActions::Result WallRemoveAction::Query() const
    {
        GameActions::Result res = GameActions::Result();
        res.Cost = 0;
        res.Expenditure = ExpenditureType::Landscaping;

        if (!LocationValid(_loc))
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        const bool isGhost = GetFlags() & GAME_COMMAND_FLAG_GHOST;
        if (!isGhost && !(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode && !MapIsLocationOwned(_loc))
        {
            return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        TileElement* wallElement = GetFirstWallElementAt(_loc, isGhost);
        if (wallElement == nullptr)
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res.Cost = 0;
        return res;
    }

    GameActions::Result WallRemoveAction::Execute() const
    {
        GameActions::Result res = GameActions::Result();
        res.Cost = 0;
        res.Expenditure = ExpenditureType::Landscaping;

        const bool isGhost = GetFlags() & GAME_COMMAND_FLAG_GHOST;

        TileElement* wallElement = GetFirstWallElementAt(_loc, isGhost);
        if (wallElement == nullptr)
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res.Position.x = _loc.x + 16;
        res.Position.y = _loc.y + 16;
        res.Position.z = TileElementHeight(res.Position);

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
} // namespace OpenRCT2
