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

struct FootpathRemoveAction : public GameActionBase<GAME_COMMAND_REMOVE_PATH, GameActionResult>
{
private:
    int32_t _x;
    int32_t _y;
    int32_t _z;

public:
    FootpathRemoveAction() = default;
    FootpathRemoveAction(int32_t x, int32_t y, int32_t z)
        : _x(x)
        , _y(y)
        , _z(z)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _x << _y << _z;
    }

    GameActionResult::Ptr Query() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->Cost = 0;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        res->Position = { _x + 16, _y + 16, _z * 8 };

        if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(_x, _y, _z * 8))
        {
            return MakeResult(GA_ERROR::NOT_OWNED, STR_CANT_REMOVE_FOOTPATH_FROM_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }

        rct_tile_element* footpathElement = GetFootpathElement();
        res->Cost = GetRefundPrice(footpathElement);

        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->Cost = 0;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        res->Position = { _x + 16, _y + 16, _z * 8 };

        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            footpath_interrupt_peeps(_x, _y, _z * 8);
            footpath_remove_litter(_x, _y, _z * 8);
        }

        rct_tile_element* footpathElement = GetFootpathElement();
        if (footpathElement != nullptr)
        {
            footpath_queue_chain_reset();
            remove_banners_at_element(_x, _y, footpathElement);
            footpath_remove_edges_at(_x, _y, footpathElement);
            map_invalidate_tile_full(_x, _y);
            tile_element_remove(footpathElement);
            footpath_update_queue_chains();
        }

        res->Cost = GetRefundPrice(footpathElement);

        return res;
    }

private:
    rct_tile_element* GetFootpathElement() const
    {
        bool getGhostPath = GetFlags() & GAME_COMMAND_FLAG_GHOST;

        rct_tile_element* tileElement = map_get_footpath_element(_x / 32, _y / 32, _z);
        rct_tile_element* footpathElement = nullptr;
        if (tileElement != nullptr)
        {
            if (getGhostPath && tile_element_is_ghost(tileElement) == false)
            {
                while (!(tileElement++)->IsLastForTile())
                {
                    if (tileElement->type != TILE_ELEMENT_TYPE_PATH && !tile_element_is_ghost(tileElement))
                    {
                        continue;
                    }
                    footpathElement = tileElement;
                    break;
                }
            }
            else
            {
                footpathElement = tileElement;
            }
        }

        return footpathElement;
    }

    money32 GetRefundPrice(rct_tile_element* footpathElement) const
    {
        money32 cost = -MONEY(10, 00);

        bool isNotOwnedByPark = (GetFlags() & GAME_COMMAND_FLAG_5);
        bool moneyDisabled = (gParkFlags & PARK_FLAGS_NO_MONEY);
        bool isGhost = (footpathElement == nullptr) || (tile_element_is_ghost(footpathElement));

        if (isNotOwnedByPark || moneyDisabled || isGhost)
        {
            cost = 0;
        }

        return cost;
    }
};
