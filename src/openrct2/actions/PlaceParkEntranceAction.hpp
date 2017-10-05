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

#include "../core/MemoryStream.h"
#include "../localisation/string_ids.h"
#include "../OpenRCT2.h"
#include "GameAction.h"

#include "../cheats.h"
#include "../world/entrance.h"
#include "../world/park.h"
#include "../world/footpath.h"

class PlaceParkEntranceGameActionResult final : public GameActionResult
{
public:
    PlaceParkEntranceGameActionResult() : GameActionResult(GA_ERROR::OK, 0) {}
    PlaceParkEntranceGameActionResult(GA_ERROR error, rct_string_id message) : GameActionResult(error, message)
    {
        ErrorTitle = STR_CANT_BUILD_PARK_ENTRANCE_HERE;
    }
};

struct PlaceParkEntranceAction : public GameActionBase<GAME_COMMAND_PLACE_PARK_ENTRANCE, PlaceParkEntranceGameActionResult>
{
private:
    sint16 _x;
    sint16 _y;
    sint16 _z;
    uint8 _direction;

public:
    PlaceParkEntranceAction() {}
    PlaceParkEntranceAction(sint16 x, sint16 y, sint16 z, sint16 direction) :
        _x(x),
        _y(y),
        _z(z),
        _direction(direction)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameActionBase::GetActionFlags() | GA_FLAGS::EDITOR_ONLY;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _x << _y << _z << _direction;
    }

    GameActionResult::Ptr Query() const override
    {
        if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode)
        {
            return std::make_unique<PlaceParkEntranceGameActionResult>(GA_ERROR::NOT_IN_EDITOR_MODE, STR_NONE);
        }

        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

        gCommandPosition.x = _x;
        gCommandPosition.y = _y;
        gCommandPosition.z = _z * 16;

        if (!map_check_free_elements_and_reorganise(3))
        {
            return std::make_unique<PlaceParkEntranceGameActionResult>(GA_ERROR::NO_FREE_ELEMENTS, STR_NONE);
        }

        if (_x <= 32 || _y <= 32 || _x >= (gMapSizeUnits - 32) || _y >= (gMapSizeUnits - 32))
        {
            return std::make_unique<PlaceParkEntranceGameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_TOO_CLOSE_TO_EDGE_OF_MAP);
        }

        sint8 entranceNum = -1;
        for (uint8 i = 0; i < MAX_PARK_ENTRANCES; ++i)
        {
            if (gParkEntrances[i].x == MAP_LOCATION_NULL)
            {
                entranceNum = i;
                break;
            }
        }

        if (entranceNum == -1)
        {
            return std::make_unique<PlaceParkEntranceGameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_ERR_TOO_MANY_PARK_ENTRANCES);
        }

        sint8 zLow = _z * 2;
        sint8 zHigh = zLow + 12;
        rct_xy16 entranceLoc = { _x, _y };
        for (uint8 index = 0; index < 3; index++)
        {
            if (index == 1)
            {
                entranceLoc.x += TileDirectionDelta[(_direction - 1) & 0x3].x;
                entranceLoc.y += TileDirectionDelta[(_direction - 1) & 0x3].y;
            }
            else if (index == 2)
            {
                entranceLoc.x += TileDirectionDelta[(_direction + 1) & 0x3].x * 2;
                entranceLoc.y += TileDirectionDelta[(_direction + 1) & 0x3].y * 2;
            }

            if (!gCheatsDisableClearanceChecks)
            {
                if (!map_can_construct_at(entranceLoc.x, entranceLoc.y, zLow, zHigh, 0xF))
                {
                    return std::make_unique<PlaceParkEntranceGameActionResult>(GA_ERROR::NO_CLEARANCE, STR_NONE);
                }
            }

            // Check that entrance element does not already exist at this location
            rct_map_element* entranceElement = map_get_park_entrance_element_at(entranceLoc.x, entranceLoc.y, zLow, false);
            if (entranceElement != NULL)
            {
                return std::make_unique<PlaceParkEntranceGameActionResult>(GA_ERROR::ITEM_ALREADY_PLACED, STR_NONE);
            }
        }

        return std::make_unique<PlaceParkEntranceGameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        uint32 flags = GetFlags();

        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

        gCommandPosition.x = _x;
        gCommandPosition.y = _y;
        gCommandPosition.z = _z * 16;

        sint8 entranceNum = -1;
        for (uint8 i = 0; i < MAX_PARK_ENTRANCES; ++i)
        {
            if (gParkEntrances[i].x == MAP_LOCATION_NULL)
            {
                entranceNum = i;
                break;
            }
        }

        Guard::Assert(entranceNum != -1);

        gParkEntrances[entranceNum].x = _x;
        gParkEntrances[entranceNum].y = _y;
        gParkEntrances[entranceNum].z = _z * 16;
        gParkEntrances[entranceNum].direction = _direction;

        sint8 zLow = _z * 2;
        sint8 zHigh = zLow + 12;
        rct_xy16 entranceLoc = { _x, _y };
        for (uint8 index = 0; index < 3; index++)
        {
            if (index == 1)
            {
                entranceLoc.x += TileDirectionDelta[(_direction - 1) & 0x3].x;
                entranceLoc.y += TileDirectionDelta[(_direction - 1) & 0x3].y;
            }
            else if (index == 2)
            {
                entranceLoc.x += TileDirectionDelta[(_direction + 1) & 0x3].x * 2;
                entranceLoc.y += TileDirectionDelta[(_direction + 1) & 0x3].y * 2;
            }

            if (!(flags & GAME_COMMAND_FLAG_GHOST))
            {
                rct_map_element* surfaceElement = map_get_surface_element_at(entranceLoc.x / 32, entranceLoc.y / 32);
                surfaceElement->properties.surface.ownership = 0;
            }

            rct_map_element* newElement = map_element_insert(entranceLoc.x / 32, entranceLoc.y / 32, zLow, 0xF);
            Guard::Assert(newElement != NULL);
            newElement->clearance_height = zHigh;

            if (flags & GAME_COMMAND_FLAG_GHOST)
            {
                newElement->flags |= MAP_ELEMENT_FLAG_GHOST;
            }

            newElement->type = MAP_ELEMENT_TYPE_ENTRANCE;
            newElement->type |= _direction;
            newElement->properties.entrance.index = index;
            newElement->properties.entrance.type = ENTRANCE_TYPE_PARK_ENTRANCE;
            newElement->properties.entrance.path_type = gFootpathSelectedId;

            if (!(flags & GAME_COMMAND_FLAG_GHOST))
            {
                footpath_connect_edges(entranceLoc.x, entranceLoc.y, newElement, 1);
            }

            update_park_fences(entranceLoc.x, entranceLoc.y);
            update_park_fences(entranceLoc.x - 32, entranceLoc.y);
            update_park_fences(entranceLoc.x + 32, entranceLoc.y);
            update_park_fences(entranceLoc.x, entranceLoc.y - 32);
            update_park_fences(entranceLoc.x, entranceLoc.y + 32);

            map_invalidate_tile(entranceLoc.x, entranceLoc.y, newElement->base_height * 8, newElement->clearance_height * 8);

            if (index == 0)
            {
                map_animation_create(MAP_ANIMATION_TYPE_PARK_ENTRANCE, entranceLoc.x, entranceLoc.y, zLow);
            }
        }

        return std::make_unique<PlaceParkEntranceGameActionResult>();
    }
};
