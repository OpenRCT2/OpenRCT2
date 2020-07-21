/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Footpath.h"
#include "../world/Park.h"
#include "../world/Surface.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(PlacePeepSpawnAction, GAME_COMMAND_PLACE_PEEP_SPAWN, GameActionResult)
{
private:
    CoordsXYZD _location;

public:
    PlacePeepSpawnAction()
    {
    }
    PlacePeepSpawnAction(const CoordsXYZD& location)
        : _location(location)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameActionBase::GetActionFlags() | GA_FLAGS::EDITOR_ONLY | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_location.x) << DS_TAG(_location.y) << DS_TAG(_location.z) << DS_TAG(_location.direction);
    }

    GameActionResult::Ptr Query() const override
    {
        if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode)
        {
            return std::make_unique<GameActionResult>(
                GA_ERROR::NOT_IN_EDITOR_MODE, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_NONE);
        }

        auto res = std::make_unique<GameActionResult>();
        res->Expenditure = ExpenditureType::LandPurchase;
        res->Position = _location;

        if (!map_check_free_elements_and_reorganise(3))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::NO_FREE_ELEMENTS, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_NONE);
        }

        if (!LocationValid(_location) || _location.x <= 16 || _location.y <= 16 || _location.x >= (gMapSizeUnits - 16)
            || _location.y >= (gMapSizeUnits - 16))
        {
            return std::make_unique<GameActionResult>(
                GA_ERROR::INVALID_PARAMETERS, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_OFF_EDGE_OF_MAP);
        }

        // Verify footpath exists at location, and retrieve coordinates
        auto pathElement = map_get_path_element_at(TileCoordsXYZ{ _location });
        if (pathElement == nullptr)
        {
            return std::make_unique<GameActionResult>(
                GA_ERROR::INVALID_PARAMETERS, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_CAN_ONLY_BE_BUILT_ACROSS_PATHS);
        }

        // Verify location is unowned
        auto surfaceMapElement = map_get_surface_element_at(_location);
        if (surfaceMapElement == nullptr)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_NONE);
        }
        if (surfaceMapElement->GetOwnership() != OWNERSHIP_UNOWNED)
        {
            return std::make_unique<GameActionResult>(
                GA_ERROR::INVALID_PARAMETERS, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_ERR_MUST_BE_OUTSIDE_PARK_BOUNDARIES);
        }

        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = std::make_unique<GameActionResult>();
        res->Expenditure = ExpenditureType::LandPurchase;
        res->Position = _location;

        // Shift the spawn point to the edge of the tile
        auto spawnPos = CoordsXY{ _location.ToTileCentre() }
            + CoordsXY{ DirectionOffsets[_location.direction].x * 15, DirectionOffsets[_location.direction].y * 15 };

        PeepSpawn spawn;
        spawn.x = spawnPos.x;
        spawn.y = spawnPos.y;
        spawn.z = _location.z;
        spawn.direction = _location.direction;

        // When attempting to place a peep spawn on a tile that already contains it,
        // remove that peep spawn instead.
        if (!gPeepSpawns.empty())
        {
            // When searching for existing spawns, ignore the direction.
            auto foundSpawn = std::find_if(gPeepSpawns.begin(), gPeepSpawns.end(), [spawn](const CoordsXYZ& existingSpawn) {
                {
                    return existingSpawn.ToTileStart() == spawn.ToTileStart();
                }
            });

            if (foundSpawn != std::end(gPeepSpawns))
            {
                gPeepSpawns.erase(foundSpawn);
                map_invalidate_tile_full(spawn);
                return res;
            }
        }

        // If we have reached our max peep spawns, remove the oldest spawns
        while (gPeepSpawns.size() >= MAX_PEEP_SPAWNS)
        {
            PeepSpawn oldestSpawn = *gPeepSpawns.begin();
            gPeepSpawns.erase(gPeepSpawns.begin());
            map_invalidate_tile_full(oldestSpawn);
        }

        // Set peep spawn
        gPeepSpawns.push_back(spawn);

        // Invalidate tile
        map_invalidate_tile_full(_location);

        return res;
    }
};
