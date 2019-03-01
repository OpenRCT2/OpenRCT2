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

DEFINE_GAME_ACTION(FootpathSceneryPlaceAction, GAME_COMMAND_PLACE_FOOTPATH_SCENERY, GameActionResult)
{
private:
    CoordsXYZ _loc;
    uint8_t _pathItemType;

public:
    FootpathSceneryPlaceAction() = default;
    FootpathSceneryPlaceAction(CoordsXYZ loc, uint8_t pathItemType)
        : _loc(loc)
        , _pathItemType(pathItemType)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_pathItemType);
    }

    GameActionResult::Ptr Query() const override
    {
        auto res = MakeResult();
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        res->Position = _loc;
        if (!map_is_location_valid({ _loc.x, _loc.y }))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE, STR_OFF_EDGE_OF_MAP);
        }

        if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode)
            && !map_is_location_owned(_loc.x, _loc.y, _loc.z / 8))
        {
            return MakeResult(GA_ERROR::DISALLOWED, STR_CANT_POSITION_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }

        if (_loc.z / 8 < 2)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE, STR_TOO_LOW);
        }

        if (_loc.z / 8 > 248)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE, STR_TOO_HIGH);
        }

        auto tileElement = map_get_footpath_element(_loc.x / 32, _loc.y / 32, _loc.z / 8);
        auto pathElement = tileElement->AsPath();

        if (pathElement == nullptr)
        {
            log_error("Could not find path element.");
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
        }

        // No change
        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && pathElement->GetAddition() == _pathItemType
            && !(pathElement->IsBroken()))
        {
            if (GetFlags() & GAME_COMMAND_FLAG_4)
                return MakeResult(GA_ERROR::UNKNOWN, STR_CANT_POSITION_THIS_HERE);

            return res;
        }

        if (_pathItemType != 0)
        {
            rct_scenery_entry* sceneryEntry = get_footpath_item_entry(_pathItemType - 1);
            if (sceneryEntry == nullptr)
            {
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
            }
            uint16_t sceneryFlags = sceneryEntry->path_bit.flags;

            if ((sceneryFlags & PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE) && pathElement->IsSloped())
            {
                return MakeResult(
                    GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE, STR_CANT_BUILD_THIS_ON_SLOPED_FOOTPATH);
            }

            if ((sceneryFlags & PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE) && pathElement->IsQueue())
            {
                return MakeResult(
                    GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE, STR_CANNOT_PLACE_THESE_ON_QUEUE_LINE_AREA);
            }

            if (!(sceneryFlags & (PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER | PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW))
                && (pathElement->GetEdges()) == 0x0F)
            {
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
            }

            if ((sceneryFlags & PATH_BIT_FLAG_IS_QUEUE_SCREEN) && !pathElement->IsQueue())
            {
                return MakeResult(
                    GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE, STR_CAN_ONLY_PLACE_THESE_ON_QUEUE_AREA);
            }

            res->Cost = sceneryEntry->path_bit.price;
        }

        if (GetFlags() & GAME_COMMAND_FLAG_4)
            return MakeResult(GA_ERROR::UNKNOWN, STR_CANT_POSITION_THIS_HERE);

        // Should place a ghost?
        if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
        {
            // Check if there is something on the path already
            if (pathElement->HasAddition())
            {
                return MakeResult(GA_ERROR::ITEM_ALREADY_PLACED, STR_CANT_POSITION_THIS_HERE);
            }
        }
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = MakeResult();
        res->Position = _loc;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;

        auto tileElement = map_get_footpath_element(_loc.x / 32, _loc.y / 32, _loc.z / 8);
        auto pathElement = tileElement->AsPath();

        if (pathElement == nullptr)
        {
            log_error("Could not find path element.");
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
        }

        // No change
        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && pathElement->GetAddition() == _pathItemType
            && !(pathElement->IsBroken()) && !pathElement->AdditionIsGhost())
        {
            return res;
        }

        if (_pathItemType != 0)
        {
            rct_scenery_entry* sceneryEntry = get_footpath_item_entry(_pathItemType - 1);
            if (sceneryEntry == nullptr)
            {
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
            }

            res->Cost = sceneryEntry->path_bit.price;
        }

        if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
        {
            pathElement->SetAdditionIsGhost(true);
        }
        else
        {
            footpath_interrupt_peeps(_loc.x, _loc.y, _loc.z);
        }

        if ((_pathItemType != 0 && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
            || (_pathItemType == 0 && pathElement->AdditionIsGhost()))
        {
            pathElement->SetAdditionIsGhost(false);
        }

        pathElement->SetAddition(_pathItemType);
        pathElement->SetIsBroken(false);
        if (_pathItemType != 0)
        {
            rct_scenery_entry* scenery_entry = get_footpath_item_entry(_pathItemType - 1);
            if (scenery_entry != nullptr && scenery_entry->path_bit.flags & PATH_BIT_FLAG_IS_BIN)
            {
                pathElement->SetAdditionStatus(255);
            }
        }
        map_invalidate_tile_full(_loc.x, _loc.y);
        return res;
    }
};
