/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepSpawnPlaceAction.h"

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Footpath.h"
#include "../world/Park.h"
#include "../world/Surface.h"
namespace OpenRCT2
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
        return GameActionBase::GetActionFlags() | GameActions::Flags::EditorOnly | GameActions::Flags::AllowWhilePaused;
    }

    void PeepSpawnPlaceAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_location.x) << DS_TAG(_location.y) << DS_TAG(_location.z) << DS_TAG(_location.direction);
    }

    GameActions::Result PeepSpawnPlaceAction::Query() const
    {
        if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode)
        {
            return GameActions::Result(GameActions::Status::NotInEditorMode, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_NONE);
        }

        auto res = GameActions::Result();
        res.Expenditure = ExpenditureType::LandPurchase;
        res.Position = _location;

        auto mapSizeUnits = GetMapSizeUnits() - CoordsXY{ 16, 16 };
        if (!LocationValid(_location) || _location.x <= 16 || _location.y <= 16 || _location.x >= mapSizeUnits.x
            || _location.y >= mapSizeUnits.y)
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_OFF_EDGE_OF_MAP);
        }

        // Verify footpath exists at location, and retrieve coordinates
        auto pathElement = MapGetPathElementAt(TileCoordsXYZ{ _location });
        if (pathElement == nullptr)
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_CAN_ONLY_BE_BUILT_ACROSS_PATHS);
        }

        // Verify location is unowned
        auto surfaceMapElement = MapGetSurfaceElementAt(_location);
        if (surfaceMapElement == nullptr)
        {
            return GameActions::Result(GameActions::Status::Unknown, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE, STR_NONE);
        }
        if (surfaceMapElement->GetOwnership() != OWNERSHIP_UNOWNED)
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PEEP_SPAWN_HERE,
                STR_ERR_MUST_BE_OUTSIDE_PARK_BOUNDARIES);
        }

        return res;
    }

    GameActions::Result PeepSpawnPlaceAction::Execute() const
    {
        auto res = GameActions::Result();
        res.Expenditure = ExpenditureType::LandPurchase;
        res.Position = _location;

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
                MapInvalidateTileFull(spawn);
                return res;
            }
        }

        // If we have reached our max peep spawns, remove the oldest spawns
        while (gPeepSpawns.size() >= OpenRCT2::Limits::MaxPeepSpawns)
        {
            PeepSpawn oldestSpawn = *gPeepSpawns.begin();
            gPeepSpawns.erase(gPeepSpawns.begin());
            MapInvalidateTileFull(oldestSpawn);
        }

        // Set peep spawn
        gPeepSpawns.push_back(spawn);

        // Invalidate tile
        MapInvalidateTileFull(_location);

        return res;
    }
} // namespace OpenRCT2
