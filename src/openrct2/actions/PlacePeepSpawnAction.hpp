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
#include "../localisation/StringIds.h"
#include "../OpenRCT2.h"
#include "GameAction.h"

#include "../Cheats.h"
#include "../management/Finance.h"
#include "../world/Park.h"
#include "../world/Footpath.h"

static sint32 _nextPeepSpawnIndex = 0;

struct PlacePeepSpawnAction : public GameActionBase<GAME_COMMAND_PLACE_PEEP_SPAWN, GameActionResult>
{
private:
    CoordsXYZD _location;

public:
    PlacePeepSpawnAction() {}
    PlacePeepSpawnAction(CoordsXYZD location) :
        _location(location)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameActionBase::GetActionFlags() | GA_FLAGS::EDITOR_ONLY | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _location.x << _location.y << _location.z << _location.direction;
    }

    GameActionResult::Ptr Query() const override
    {
        if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::NOT_IN_EDITOR_MODE, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_NONE);
        }

        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

        gCommandPosition.x = _location.x;
        gCommandPosition.y = _location.y;
        gCommandPosition.z = _location.z / 8;

        if (!map_check_free_elements_and_reorganise(3))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::NO_FREE_ELEMENTS, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_NONE);
        }

        if (_location.x <= 16 || _location.y <= 16 || _location.x >= (gMapSizeUnits - 16) || _location.y >= (gMapSizeUnits - 16))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_OFF_EDGE_OF_MAP);
        }

        rct_tile_element *mapElement, *surfaceMapElement;
        // Verify footpath exists at location, and retrieve coordinates
        mapElement = map_get_path_element_at(_location.x >> 5, _location.y >> 5, _location.z / 8);
        if (mapElement == nullptr) {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_CAN_ONLY_BE_BUILT_ACROSS_PATHS);
        }

        // Verify location is unowned
        surfaceMapElement = map_get_surface_element_at(_location.x >> 5, _location.y >> 5);
        if (surfaceMapElement == nullptr) {
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_NONE);
        }
        if (surfaceMapElement->properties.surface.ownership & 0xF0) {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_ERR_MUST_BE_OUTSIDE_PARK_BOUNDARIES);
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

        gCommandPosition.x = _location.x;
        gCommandPosition.y = _location.y;
        gCommandPosition.z = _location.z / 8;

        // Find empty or next appropriate peep spawn to use
        sint32 peepSpawnIndex = -1;
        for (sint32 i = 0; i < MAX_PEEP_SPAWNS; i++) {
            if (gPeepSpawns[i].x == PEEP_SPAWN_UNDEFINED) {
                peepSpawnIndex = i;
                break;
            }
        }

        // If no empty peep spawns exist, use peep spawn next to last one set.
        if (peepSpawnIndex == -1)
        {
            peepSpawnIndex = _nextPeepSpawnIndex;
            _nextPeepSpawnIndex = (peepSpawnIndex + 1) % MAX_PEEP_SPAWNS;

            // Before the new location is set, clear the old location
            sint32 prevX = gPeepSpawns[peepSpawnIndex].x;
            gPeepSpawns[peepSpawnIndex].x = PEEP_SPAWN_UNDEFINED;

            map_invalidate_tile_full(prevX, gPeepSpawns[peepSpawnIndex].y);
        }

        // Shift the spawn point to the middle of the tile
        sint32 middleX, middleY;
        middleX = _location.x + 16 + (word_981D6C[_location.direction].x * 15);
        middleY = _location.y + 16 + (word_981D6C[_location.direction].y * 15);

        // Set peep spawn
        gPeepSpawns[peepSpawnIndex].x = middleX;
        gPeepSpawns[peepSpawnIndex].y = middleY;
        gPeepSpawns[peepSpawnIndex].z = _location.z;
        gPeepSpawns[peepSpawnIndex].direction = _location.direction;

        // Invalidate tile
        map_invalidate_tile_full(_location.x, _location.y);

        return std::make_unique<GameActionResult>();
    }
};
