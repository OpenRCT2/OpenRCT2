/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../world/Entrance.h"
#include "../world/Park.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(ParkEntranceRemoveAction, GAME_COMMAND_REMOVE_PARK_ENTRANCE, GameActionResult)
{
private:
    CoordsXYZ _loc;

public:
    ParkEntranceRemoveAction() = default;

    ParkEntranceRemoveAction(const CoordsXYZ& loc)
        : _loc(loc)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::EDITOR_ONLY;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc);
    }

    GameActionResult::Ptr Query() const override
    {
        if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode)
        {
            return MakeResult(GA_ERROR::NOT_IN_EDITOR_MODE, STR_CANT_REMOVE_THIS);
        }

        auto res = MakeResult();
        res->Expenditure = ExpenditureType::LandPurchase;
        res->Position = _loc;
        res->ErrorTitle = STR_CANT_REMOVE_THIS;

        auto entranceIndex = park_entrance_get_index(_loc);
        if (!LocationValid(_loc) || entranceIndex == -1)
        {
            log_error("Could not find entrance at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS);
        }
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = MakeResult();
        res->Expenditure = ExpenditureType::LandPurchase;
        res->Position = _loc;
        res->ErrorTitle = STR_CANT_REMOVE_THIS;

        auto entranceIndex = park_entrance_get_index(_loc);
        if (entranceIndex == -1)
        {
            log_error("Could not find entrance at x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS);
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

private:
    void ParkEntranceRemoveSegment(const CoordsXYZ& loc) const
    {
        auto entranceElement = map_get_park_entrance_element_at(loc, true);
        if (entranceElement == nullptr)
        {
            return;
        }

        map_invalidate_tile({ loc, entranceElement->GetBaseZ(), entranceElement->GetClearanceZ() });
        entranceElement->Remove();
        update_park_fences({ loc.x, loc.y });
    }
};
