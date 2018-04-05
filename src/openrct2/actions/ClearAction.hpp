#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#pragma once

#include <algorithm>
#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/LargeScenery.h"
#include "../world/Location.hpp"
#include "GameAction.h"

using namespace OpenRCT2;

using ClearableItems = uint8;
namespace CLEARABLE_ITEMS
{
    constexpr ClearableItems SCENERY_SMALL = 1 << 0;
    constexpr ClearableItems SCENERY_LARGE = 1 << 1;
    constexpr ClearableItems SCENERY_FOOTPATH = 1 << 2;
}

struct ClearAction : public GameActionBase<GAME_COMMAND_CLEAR_SCENERY, GameActionResult>
{
private:
    MapRange _range;
    ClearableItems _itemsToClear;

public:
    ClearAction() { }
    ClearAction(MapRange range, ClearableItems itemsToClear)
        : _range(range),
          _itemsToClear(itemsToClear)
    {
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);
        stream << _range << _itemsToClear;
    }

    GameActionResult::Ptr Query() const override
    {
        return QueryExecute(false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return QueryExecute(true);
    }

private:
    GameActionResult::Ptr CreateResult() const
    {
        auto result = MakeResult();
        result->ErrorTitle = STR_UNABLE_TO_REMOVE_ALL_SCENERY_FROM_HERE;
        result->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;

        auto x = (_range.GetLeft() + _range.GetRight()) / 2 + 16;
        auto y = (_range.GetTop() + _range.GetBottom()) / 2 + 16;
        auto z = tile_element_height(x, y);
        result->Position = CoordsXYZ(x, y, z);

        return result;
    }

    GameActionResult::Ptr QueryExecute(bool executing) const
    {
        auto result = CreateResult();

        auto noValidTiles = true;
        auto error = GA_ERROR::OK;
        rct_string_id errorMessage = STR_NONE;
        money32 totalCost = 0;

        auto x0 = std::max(_range.GetLeft(), 32);
        auto y0 = std::max(_range.GetTop(), 32);
        auto x1 = std::min(_range.GetRight(), (sint32)gMapSizeMaxXY);
        auto y1 = std::min(_range.GetBottom(), (sint32)gMapSizeMaxXY);
        for (sint32 y = y0; y <= y1; y += 32)
        {
            for (sint32 x = x0; x <= x1; x += 32)
            {
                if (map_can_clear_at(x, y))
                {
                    auto cost = ClearSceneryFromTile(floor2(x, 32), floor2(y, 32), true);
                    if (cost != MONEY32_UNDEFINED)
                    {
                        noValidTiles = false;
                        totalCost += cost;
                    }
                }
                else
                {
                    error = GA_ERROR::NOT_OWNED;
                    errorMessage = STR_LAND_NOT_OWNED_BY_PARK;
                }
            }
        }

        if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_LARGE)
        {
            ResetClearLargeSceneryFlag();
        }

        if (noValidTiles)
        {
            result->Error = error;
            result->ErrorMessage = errorMessage;
        }
        result->Cost = totalCost;
        return result;
    }

    money32 ClearSceneryFromTile(sint32 x, sint32 y, bool executing) const
    {
        rct_tile_element * tileElement = nullptr;
        money32 totalCost = 0;
        bool tileEdited;
        do
        {
            tileEdited = false;
            tileElement = map_get_first_element_at(x / 32, y / 32);
            do
            {
                auto type = tileElement->GetType();
                switch (type) {
                case TILE_ELEMENT_TYPE_PATH:
                    if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_FOOTPATH)
                    {
                        sint32 eax = x;
                        sint32 ebx = executing ? 1 : 0;
                        sint32 ecx = y;
                        sint32 edx = tileElement->base_height;
                        sint32 edi = 0, ebp = 0;
                        auto cost = game_do_command(eax, ebx, ecx, edx, GAME_COMMAND_REMOVE_PATH, edi, ebp);
                        if (cost == MONEY32_UNDEFINED)
                            return MONEY32_UNDEFINED;

                        totalCost += cost;
                        tileEdited = true;
                    }
                    break;
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                    if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_SMALL)
                    {
                        sint32 eax = x;
                        sint32 ebx = (tileElement->type << 8) | (executing ? 1 : 0);
                        sint32 ecx = y;
                        sint32 edx = (tileElement->properties.scenery.type << 8) | (tileElement->base_height);
                        sint32 edi = 0, ebp = 0;
                        auto cost = game_do_command(eax, ebx, ecx, edx, GAME_COMMAND_REMOVE_SCENERY, edi, ebp);
                        if (cost == MONEY32_UNDEFINED)
                        {
                            return MONEY32_UNDEFINED;
                        }
                        totalCost += cost;
                        tileEdited = true;
                    }
                    break;
                case TILE_ELEMENT_TYPE_WALL:
                    if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_SMALL)
                    {
                        sint32 eax = x;
                        sint32 ebx = executing ? 1 : 0;
                        sint32 ecx = y;
                        sint32 edx = (tileElement->base_height << 8) | (tile_element_get_direction(tileElement));
                        sint32 edi = 0, ebp = 0;
                        auto cost = game_do_command(eax, ebx, ecx, edx, GAME_COMMAND_REMOVE_WALL, edi, ebp);
                        if (cost == MONEY32_UNDEFINED)
                        {
                            return MONEY32_UNDEFINED;
                        }
                        totalCost += cost;
                        tileEdited = true;
                    }
                    break;
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                    if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_LARGE)
                    {
                        sint32 eax = x;
                        sint32 ebx = (executing ? 1 : 0) | ((tile_element_get_direction(tileElement)) << 8);
                        sint32 ecx = y;
                        sint32 edx = tileElement->base_height | (scenery_large_get_sequence(tileElement) << 8);
                        sint32 edi = 0, ebp = 0;
                        auto cost = game_do_command(eax, ebx | (1 << 7), ecx, edx, GAME_COMMAND_REMOVE_LARGE_SCENERY, edi, ebp);
                        if (cost == MONEY32_UNDEFINED)
                        {
                            return MONEY32_UNDEFINED;
                        }
                        totalCost += cost;
                        tileEdited = true;
                    }
                    break;
                }
            }
            while (!tileEdited && !tile_element_is_last_for_tile(tileElement++));
        }
        while (tileEdited);
        return totalCost;
    }

    /**
     * Function to clear the flag that is set to prevent cost duplication
     * when using the clear scenery tool with large scenery.
     */
    static void ResetClearLargeSceneryFlag()
    {
        // TODO: Improve efficiency of this
        for (sint32 y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
        {
            for (sint32 x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
            {
                auto tileElement = map_get_first_element_at(x, y);
                do
                {
                    if (tileElement->GetType() == TILE_ELEMENT_TYPE_LARGE_SCENERY)
                    {
                        tileElement->flags &= ~(1 << 6);
                    }
                }
                while (!tile_element_is_last_for_tile(tileElement++));
            }
        }
    }
};
