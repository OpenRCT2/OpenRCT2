#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "GameAction.h"

#include "../Cheats.h"
#include "../interface/Window.h"
#include "../world/Wall.h"

struct WallRemoveAction : public GameActionBase<GAME_COMMAND_REMOVE_WALL, GameActionResult>
{
private:
    sint32 _x;
    sint32 _y;
    uint8 _baseHeight;
    uint8 _direction;

public:
    WallRemoveAction() {}
    WallRemoveAction(sint16 x, sint16 y, uint8 baseHeight, uint8 direction) :
        _x(x),
        _y(y),
        _baseHeight(baseHeight),
        _direction(direction)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _x << _y << _baseHeight << _direction;
    }

    GameActionResult::Ptr Query() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->Cost = 0;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;

        if (!map_is_location_valid(_x, _y))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        const bool isGhost = GetFlags() & GAME_COMMAND_FLAG_GHOST;
        if (!isGhost &&
            !(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) &&
            !gCheatsSandboxMode &&
            !map_is_location_owned(_x, _y, _baseHeight * 8))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::NOT_OWNED, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        rct_tile_element * wallElement = GetFirstWallElementAt(_x, _y, _baseHeight, _direction, isGhost);
        if (wallElement == nullptr)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res->Cost = 0;
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->Cost = 0;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;

        const bool isGhost = GetFlags() & GAME_COMMAND_FLAG_GHOST;

        rct_tile_element * wallElement = GetFirstWallElementAt(_x, _y, _baseHeight, _direction, isGhost);
        if (wallElement == nullptr)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res->Position.x = _x + 16;
        res->Position.y = _y + 16;
        res->Position.z = tile_element_height(res->Position.x, res->Position.y);

        tile_element_remove_banner_entry(wallElement);
        map_invalidate_tile_zoom1(_x, _y, wallElement->base_height * 8, (wallElement->base_height * 8) + 72);
        tile_element_remove(wallElement);

        return res;
    }

private:
    rct_tile_element * GetFirstWallElementAt(sint32 x, sint32 y, uint8 baseZ, uint8 direction, bool isGhost) const
    {
        rct_tile_element * tileElement = map_get_first_element_at(x / 32, y / 32);
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            {
                continue;
            }
            if (tileElement->base_height != baseZ) continue;
            if (tileElement->GetDirection() != direction) continue;
            if (tileElement->IsGhost() != isGhost) continue;
            return tileElement;
        } while (!tile_element_is_last_for_tile(tileElement++));
        return nullptr;
    }
};
