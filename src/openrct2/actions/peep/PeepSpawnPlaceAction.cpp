/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepSpawnPlaceAction.h"

#include "../Cheats.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/tile_element/SurfaceElement.h"

namespace OpenRCT2::GameActions
{
    PeepSpawnPlaceAction::PeepSpawnPlaceAction(const CoordsXYZD& location)
        : _location(location)
    {
    }

    void PeepSpawnPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_location);
    }

    uint16_t PeepSpawnPlaceAction::GetActionFlags() const
    {
        return GameActionBase::GetActionFlags() | Flags::EditorOnly | Flags::AllowWhilePaused;
    }

    void PeepSpawnPlaceAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_location.x) << DS_TAG(_location.y) << DS_TAG(_location.z) << DS_TAG(_location.direction);
    }

    Result PeepSpawnPlaceAction::Query(GameState_t& gameState) const
    {
        if (!isInEditorMode() && !gameState.cheats.sandboxMode)
        {
            return Result(Status::notInEditorMode, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, kStringIdNone);
        }

        auto res = Result();
        res.expenditure = ExpenditureType::landPurchase;
        res.position = _location;

        auto mapSizeUnits = GetMapSizeUnits() - CoordsXY{ 16, 16 };
        if (!LocationValid(_location) || _location.x <= 16 || _location.y <= 16 || _location.x >= mapSizeUnits.x
            || _location.y >= mapSizeUnits.y)
        {
            return Result(Status::invalidParameters, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_OFF_EDGE_OF_MAP);
        }

        // Verify footpath exists at location, and retrieve coordinates
        auto pathElement = MapGetPathElementAt(TileCoordsXYZ{ _location });
        if (pathElement == nullptr)
        {
            return Result(Status::invalidParameters, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_CAN_ONLY_BE_BUILT_ON_PATHS);
        }

        // Verify location is unowned
        auto surfaceMapElement = MapGetSurfaceElementAt(_location);
        if (surfaceMapElement == nullptr)
        {
            return Result(Status::unknown, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
        }
        if (surfaceMapElement->GetOwnership() != OWNERSHIP_UNOWNED)
        {
            return Result(
                Status::invalidParameters, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_ERR_MUST_BE_OUTSIDE_PARK_BOUNDARIES);
        }

        return res;
    }

    Result PeepSpawnPlaceAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();
        res.expenditure = ExpenditureType::landPurchase;
        res.position = _location;

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
        if (!gameState.peepSpawns.empty())
        {
            // When searching for existing spawns, ignore the direction.
            auto foundSpawn = std::find_if(
                gameState.peepSpawns.begin(), gameState.peepSpawns.end(), [spawn](const CoordsXYZ& existingSpawn) {
                    {
                        return existingSpawn.ToTileStart() == spawn.ToTileStart();
                    }
                });

            if (foundSpawn != std::end(gameState.peepSpawns))
            {
                gameState.peepSpawns.erase(foundSpawn);
                MapInvalidateTileFull(spawn);
                return res;
            }
        }

        // If we have reached our max peep spawns, remove the oldest spawns
        while (gameState.peepSpawns.size() >= Limits::kMaxPeepSpawns)
        {
            PeepSpawn oldestSpawn = *gameState.peepSpawns.begin();
            gameState.peepSpawns.erase(gameState.peepSpawns.begin());
            MapInvalidateTileFull(oldestSpawn);
        }

        // Set peep spawn
        gameState.peepSpawns.push_back(spawn);

        // Invalidate tile
        MapInvalidateTileFull(_location);

        return res;
    }
} // namespace OpenRCT2::GameActions
