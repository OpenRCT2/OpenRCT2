/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SurfaceSelection.h"

#include "../../Context.h"
#include "../../object/ObjectEntryManager.h"
#include "../../object/ObjectList.h"
#include "../../object/ObjectManager.h"
#include "../../object/TerrainEdgeObject.h"
#include "../../object/TerrainSurfaceObject.h"
#include "../../util/Util.h"
#include "MapGen.h"

#include <algorithm>
#include <iterator>

namespace OpenRCT2::World::MapGenerator
{
    // Randomly chosen base terrains. We rarely want a whole map made out of chequerboard or rock.
    static constexpr std::string_view kBaseTerrain[] = {
        "rct2.terrain_surface.grass", "rct2.terrain_surface.sand", "rct2.terrain_surface.sand_brown",
        "rct2.terrain_surface.dirt",  "rct2.terrain_surface.ice",
    };

    ObjectEntryIndex generateSurfaceTextureId(Settings* settings)
    {
        auto& objectManager = GetContext()->GetObjectManager();

        const auto selectedFloor = TerrainSurfaceObject::GetById(settings->landTexture);
        std::string_view surfaceTexture = selectedFloor != nullptr ? selectedFloor->GetIdentifier() : "";

        if (surfaceTexture.empty())
        {
            std::vector<std::string_view> availableTerrains;
            std::copy_if(
                std::begin(kBaseTerrain), std::end(kBaseTerrain), std::back_inserter(availableTerrains),
                [&](auto terrain) { return objectManager.GetLoadedObject(ObjectEntryDescriptor(terrain)) != nullptr; });

            if (availableTerrains.empty())
                // Fall back to the first available surface texture that is available in the park
                surfaceTexture = TerrainSurfaceObject::GetById(0)->GetIdentifier();
            else
                surfaceTexture = availableTerrains[UtilRand() % availableTerrains.size()];
        }

        auto surfaceTextureId = objectManager.GetLoadedObjectEntryIndex(ObjectEntryDescriptor(surfaceTexture));
        return surfaceTextureId;
    }

    ObjectEntryIndex generateEdgeTextureId(Settings* settings, ObjectEntryIndex surfaceTextureId)
    {
        auto& objectManager = GetContext()->GetObjectManager();

        const auto selectedEdge = TerrainEdgeObject::GetById(settings->edgeTexture);
        std::string_view edgeTexture = selectedEdge != nullptr ? selectedEdge->GetIdentifier() : "";

        if (edgeTexture.empty())
        {
            auto surfaceObject = objectManager.GetLoadedObject(ObjectType::TerrainSurface, surfaceTextureId);
            auto surfaceTexture = surfaceObject->GetIdentifier();

            // Base edge type on surface type
            if (surfaceTexture == "rct2.terrain_surface.dirt")
                edgeTexture = "rct2.terrain_edge.wood_red";
            else if (surfaceTexture == "rct2.terrain_surface.ice")
                edgeTexture = "rct2.terrain_edge.ice";
            else
                edgeTexture = "rct2.terrain_edge.rock";

            // Fall back to the first available edge texture that is available in the park
            if (objectManager.GetLoadedObject(ObjectEntryDescriptor(edgeTexture)) == nullptr)
                edgeTexture = TerrainEdgeObject::GetById(0)->GetIdentifier();
        }

        auto edgeTextureId = objectManager.GetLoadedObjectEntryIndex(ObjectEntryDescriptor(edgeTexture));
        return edgeTextureId;
    }

    ObjectEntryIndex generateBeachTextureId()
    {
        auto& objectManager = GetContext()->GetObjectManager();

        // Figure out what beach texture to use
        std::vector<std::string_view> availableBeachTextures;
        if (objectManager.GetLoadedObject(ObjectEntryDescriptor("rct2.terrain_surface.sand")) != nullptr)
            availableBeachTextures.push_back("rct2.terrain_surface.sand");
        if (objectManager.GetLoadedObject(ObjectEntryDescriptor("rct2.terrain_surface.sand_brown")) != nullptr)
            availableBeachTextures.push_back("rct2.terrain_surface.sand_brown");

        if (availableBeachTextures.empty())
            return kObjectEntryIndexNull;

        std::string_view beachTexture = availableBeachTextures[UtilRand() % availableBeachTextures.size()];
        return objectManager.GetLoadedObjectEntryIndex(ObjectEntryDescriptor(beachTexture));
    }
} // namespace OpenRCT2::World::MapGenerator
