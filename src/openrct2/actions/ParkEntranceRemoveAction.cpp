/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkEntranceRemoveAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../world/Entrance.h"
#include "../world/Park.h"
#include "../world/tile_element/EntranceElement.h"

using namespace OpenRCT2;

ParkEntranceRemoveAction::ParkEntranceRemoveAction(const CoordsXYZ& loc)
    : _loc(loc)
{
}

void ParkEntranceRemoveAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
}

uint16_t ParkEntranceRemoveAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::EditorOnly;
}

void ParkEntranceRemoveAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc);
}

GameActions::Result ParkEntranceRemoveAction::Query() const
{
    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !GetGameState().Cheats.sandboxMode)
    {
        return GameActions::Result(GameActions::Status::NotInEditorMode, STR_CANT_REMOVE_THIS, kStringIdNone);
    }

    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::LandPurchase;
    res.Position = _loc;
    res.ErrorTitle = STR_CANT_REMOVE_THIS;

    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_OFF_EDGE_OF_MAP);
    }
    if (ParkEntranceGetIndex(_loc) == -1)
    {
        LOG_ERROR("No park entrance at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, kStringIdNone);
    }
    return res;
}

GameActions::Result ParkEntranceRemoveAction::Execute() const
{
    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::LandPurchase;
    res.Position = _loc;
    res.ErrorTitle = STR_CANT_REMOVE_THIS;

    auto entranceIndex = ParkEntranceGetIndex(_loc);
    if (entranceIndex == -1)
    {
        LOG_ERROR("No park entrance at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, kStringIdNone);
    }

    auto& gameState = GetGameState();
    auto direction = (gameState.Park.Entrances[entranceIndex].direction - 1) & 3;

    // Centre (sign)
    ParkEntranceRemoveSegment(_loc);

    // Left post
    ParkEntranceRemoveSegment(
        { _loc.x + CoordsDirectionDelta[direction].x, _loc.y + CoordsDirectionDelta[direction].y, _loc.z });

    // Right post
    ParkEntranceRemoveSegment(
        { _loc.x - CoordsDirectionDelta[direction].x, _loc.y - CoordsDirectionDelta[direction].y, _loc.z });

    gameState.Park.Entrances.erase(gameState.Park.Entrances.begin() + entranceIndex);
    return res;
}

void ParkEntranceRemoveAction::ParkEntranceRemoveSegment(const CoordsXYZ& loc) const
{
    auto entranceElement = MapGetParkEntranceElementAt(loc, true);
    if (entranceElement == nullptr)
    {
        return;
    }

    MapInvalidateTile({ loc, entranceElement->GetBaseZ(), entranceElement->GetClearanceZ() });
    entranceElement->Remove();
    Park::UpdateFences({ loc.x, loc.y });
}
