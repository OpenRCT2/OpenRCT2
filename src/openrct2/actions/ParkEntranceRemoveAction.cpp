/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/tile_element/EntranceElement.h"

namespace OpenRCT2::GameActions
{
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
        return GameAction::GetActionFlags() | Flags::EditorOnly;
    }

    void ParkEntranceRemoveAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc);
    }

    Result ParkEntranceRemoveAction::Query(GameState_t& gameState) const
    {
        if (!isInEditorMode() && !gameState.cheats.sandboxMode)
        {
            return Result(Status::notInEditorMode, STR_CANT_REMOVE_THIS, kStringIdNone);
        }

        auto res = Result();
        res.expenditure = ExpenditureType::landPurchase;
        res.position = _loc;
        res.errorTitle = STR_CANT_REMOVE_THIS;

        if (!LocationValid(_loc))
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_OFF_EDGE_OF_MAP);
        }
        if (ParkEntranceGetIndex(_loc) == -1)
        {
            LOG_ERROR("No park entrance at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, kStringIdNone);
        }
        return res;
    }

    Result ParkEntranceRemoveAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();
        res.expenditure = ExpenditureType::landPurchase;
        res.position = _loc;
        res.errorTitle = STR_CANT_REMOVE_THIS;

        auto entranceIndex = ParkEntranceGetIndex(_loc);
        if (entranceIndex == -1)
        {
            LOG_ERROR("No park entrance at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, kStringIdNone);
        }

        auto& park = gameState.park;
        auto direction = (park.entrances[entranceIndex].direction - 1) & 3;

        // Centre (sign)
        ParkEntranceRemoveSegment(_loc);

        // Left post
        ParkEntranceRemoveSegment(
            { _loc.x + CoordsDirectionDelta[direction].x, _loc.y + CoordsDirectionDelta[direction].y, _loc.z });

        // Right post
        ParkEntranceRemoveSegment(
            { _loc.x - CoordsDirectionDelta[direction].x, _loc.y - CoordsDirectionDelta[direction].y, _loc.z });

        park.entrances.erase(park.entrances.begin() + entranceIndex);
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
} // namespace OpenRCT2::GameActions
