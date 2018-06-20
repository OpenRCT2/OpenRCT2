/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../OpenRCT2.h"
#include "GameAction.h"

#include "../Cheats.h"
#include "../management/Finance.h"
#include "../world/Entrance.h"
#include "../world/Park.h"
#include "../world/Footpath.h"
#include "../world/MapAnimation.h"
#include "../world/Sprite.h"

struct PlaceParkEntranceAction : public GameActionBase<GAME_COMMAND_PLACE_PARK_ENTRANCE, GameActionResult>
{
private:
    int16_t _x;
    int16_t _y;
    int16_t _z;
    uint8_t _direction;

public:
    PlaceParkEntranceAction() {}
    PlaceParkEntranceAction(int16_t x, int16_t y, int16_t z, int16_t direction) :
        _x(x),
        _y(y),
        _z(z),
        _direction(direction)
    {
    }

    uint16_t GetActionFlags() const override
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
            return std::make_unique<GameActionResult>(GA_ERROR::NOT_IN_EDITOR_MODE, STR_CANT_BUILD_PARK_ENTRANCE_HERE, STR_NONE);
        }

        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

        gCommandPosition.x = _x;
        gCommandPosition.y = _y;
        gCommandPosition.z = _z * 16;

        if (!map_check_free_elements_and_reorganise(3))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::NO_FREE_ELEMENTS, STR_CANT_BUILD_PARK_ENTRANCE_HERE, STR_NONE);
        }

        if (_x <= 32 || _y <= 32 || _x >= (gMapSizeUnits - 32) || _y >= (gMapSizeUnits - 32))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_BUILD_PARK_ENTRANCE_HERE, STR_TOO_CLOSE_TO_EDGE_OF_MAP);
        }

        int8_t entranceNum = -1;
        for (uint8_t i = 0; i < MAX_PARK_ENTRANCES; ++i)
        {
            if (gParkEntrances[i].x == LOCATION_NULL)
            {
                entranceNum = i;
                break;
            }
        }

        if (entranceNum == -1)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_BUILD_PARK_ENTRANCE_HERE, STR_ERR_TOO_MANY_PARK_ENTRANCES);
        }

        int8_t zLow = _z * 2;
        int8_t zHigh = zLow + 12;
        LocationXY16 entranceLoc = { _x, _y };
        for (uint8_t index = 0; index < 3; index++)
        {
            if (index == 1)
            {
                entranceLoc.x += CoordsDirectionDelta[(_direction - 1) & 0x3].x;
                entranceLoc.y += CoordsDirectionDelta[(_direction - 1) & 0x3].y;
            }
            else if (index == 2)
            {
                entranceLoc.x += CoordsDirectionDelta[(_direction + 1) & 0x3].x * 2;
                entranceLoc.y += CoordsDirectionDelta[(_direction + 1) & 0x3].y * 2;
            }

            if (!gCheatsDisableClearanceChecks)
            {
                if (!map_can_construct_at(entranceLoc.x, entranceLoc.y, zLow, zHigh, 0xF))
                {
                    return std::make_unique<GameActionResult>(GA_ERROR::NO_CLEARANCE, STR_CANT_BUILD_PARK_ENTRANCE_HERE, STR_NONE);
                }
            }

            // Check that entrance element does not already exist at this location
            rct_tile_element* entranceElement = map_get_park_entrance_element_at(entranceLoc.x, entranceLoc.y, zLow, false);
            if (entranceElement != nullptr)
            {
                return std::make_unique<GameActionResult>(GA_ERROR::ITEM_ALREADY_PLACED, STR_CANT_BUILD_PARK_ENTRANCE_HERE, STR_NONE);
            }
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        uint32_t flags = GetFlags();

        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

        gCommandPosition.x = _x;
        gCommandPosition.y = _y;
        gCommandPosition.z = _z * 16;

        int8_t entranceNum = -1;
        for (uint8_t i = 0; i < MAX_PARK_ENTRANCES; ++i)
        {
            if (gParkEntrances[i].x == LOCATION_NULL)
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

        int8_t zLow = _z * 2;
        int8_t zHigh = zLow + 12;
        CoordsXY entranceLoc = { _x, _y };
        for (uint8_t index = 0; index < 3; index++)
        {
            if (index == 1)
            {
                entranceLoc.x += CoordsDirectionDelta[(_direction - 1) & 0x3].x;
                entranceLoc.y += CoordsDirectionDelta[(_direction - 1) & 0x3].y;
            }
            else if (index == 2)
            {
                entranceLoc.x += CoordsDirectionDelta[(_direction + 1) & 0x3].x * 2;
                entranceLoc.y += CoordsDirectionDelta[(_direction + 1) & 0x3].y * 2;
            }

            if (!(flags & GAME_COMMAND_FLAG_GHOST))
            {
                rct_tile_element* surfaceElement = map_get_surface_element_at(entranceLoc);
                surfaceElement->properties.surface.ownership = 0;
            }

            rct_tile_element* newElement = tile_element_insert(entranceLoc.x / 32, entranceLoc.y / 32, zLow, 0xF);
            Guard::Assert(newElement != nullptr);
            newElement->clearance_height = zHigh;

            if (flags & GAME_COMMAND_FLAG_GHOST)
            {
                newElement->flags |= TILE_ELEMENT_FLAG_GHOST;
            }

            newElement->SetType(TILE_ELEMENT_TYPE_ENTRANCE);
            newElement->SetDirection(_direction);
            newElement->properties.entrance.index = index;
            newElement->properties.entrance.type = ENTRANCE_TYPE_PARK_ENTRANCE;
            newElement->properties.entrance.path_type = gFootpathSelectedId;

            if (!(flags & GAME_COMMAND_FLAG_GHOST))
            {
                footpath_connect_edges(entranceLoc.x, entranceLoc.y, newElement, 1);
            }

            update_park_fences(entranceLoc);
            update_park_fences({entranceLoc.x - 32, entranceLoc.y});
            update_park_fences({entranceLoc.x + 32, entranceLoc.y});
            update_park_fences({entranceLoc.x, entranceLoc.y - 32});
            update_park_fences({entranceLoc.x, entranceLoc.y + 32});

            map_invalidate_tile(entranceLoc.x, entranceLoc.y, newElement->base_height * 8, newElement->clearance_height * 8);

            if (index == 0)
            {
                map_animation_create(MAP_ANIMATION_TYPE_PARK_ENTRANCE, entranceLoc.x, entranceLoc.y, zLow);
            }
        }

        return std::make_unique<GameActionResult>();
    }
};
