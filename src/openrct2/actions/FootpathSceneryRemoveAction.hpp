/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

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
#include "GameAction.h"

DEFINE_GAME_ACTION(FootpathSceneryRemoveAction, GAME_COMMAND_REMOVE_FOOTPATH_SCENERY, GameActionResult)
{
private:
    CoordsXYZ _loc;

public:
    FootpathSceneryRemoveAction() = default;
    FootpathSceneryRemoveAction(CoordsXYZ loc)
        : _loc(loc)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc);
    }

    GameActionResult::Ptr Query() const override
    {
        if (!map_is_location_valid({_loc.x, _loc.y}))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_OFF_EDGE_OF_MAP);
        }

        if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode)
            && !map_is_location_owned(_loc.x, _loc.y, _loc.z / 8))
        {
            return MakeResult(GA_ERROR::DISALLOWED, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        if (_loc.z / 8 < 2)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_TOO_LOW);
        }

        if (_loc.z / 8 > 248)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_TOO_HIGH);
        }

        auto tileElement = map_get_footpath_element(_loc.x / 32, _loc.y / 32, _loc.z / 8);
        auto pathElement = tileElement->AsPath();

        if (pathElement == nullptr)
        {
            log_error("Could not find path element.");
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS);
        }
        auto res = MakeResult();
        res->Cost = MONEY(0, 0);
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto tileElement = map_get_footpath_element(_loc.x / 32, _loc.y / 32, _loc.z / 8);
        auto pathElement = tileElement->AsPath();

        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            footpath_interrupt_peeps(_loc.x, _loc.y, _loc.z);
        }

        if (pathElement == nullptr)
        {
            log_error("Could not find path element.");
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS);
        }

        pathElement->SetAddition(0);
        auto res = MakeResult();
        res->Cost = MONEY(0, 0);
        return res;
    }
};
