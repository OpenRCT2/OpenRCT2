/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkEntranceRemoveAction.h"

#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../world/Entrance.h"
#include "../world/Park.h"

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
    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode)
    {
        return GameActions::Result(GameActions::Status::NotInEditorMode, STR_CANT_REMOVE_THIS, STR_NONE);
    }

    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::LandPurchase;
    res.Position = _loc;
    res.ErrorTitle = STR_CANT_REMOVE_THIS;

    auto entranceIndex = ParkEntranceGetIndex(_loc);
    if (!LocationValid(_loc) || entranceIndex == -1)
    {
        log_error("Could not find entrance at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_NONE);
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
        log_error("Could not find entrance at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_NONE);
    }

    auto direction = (gParkEntrances[entranceIndex].direction - 1) & 3;

    // Centre (sign)
    ParkEntranceRemoveSegment(_loc);

    // Left post
    ParkEntranceRemoveSegment(
        { _loc.x + CoordsDirectionDelta[direction].x, _loc.y + CoordsDirectionDelta[direction].y, _loc.z });

    // Right post
    ParkEntranceRemoveSegment(
        { _loc.x - CoordsDirectionDelta[direction].x, _loc.y - CoordsDirectionDelta[direction].y, _loc.z });

    gParkEntrances.erase(gParkEntrances.begin() + entranceIndex);
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
    ParkUpdateFences({ loc.x, loc.y });
}
