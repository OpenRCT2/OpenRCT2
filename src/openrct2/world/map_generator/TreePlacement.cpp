/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TreePlacement.h"

#include "../../GameState.h"
#include "../../core/Guard.hpp"
#include "../../object/ObjectEntryManager.h"
#include "../../object/ObjectList.h"
#include "../../object/ObjectManager.h"
#include "../../object/SmallSceneryEntry.h"
#include "../../object/TerrainSurfaceObject.h"
#include "../../util/Util.h"
#include "../Map.h"
#include "../tile_element/SmallSceneryElement.h"
#include "../tile_element/SurfaceElement.h"
#include "../tile_element/TileElement.h"
#include "MapGen.h"
#include "SimplexNoise.h"

#include <vector>

namespace OpenRCT2::World::MapGenerator
{
    static constexpr const char* kGrassTrees[] = {
        // Dark
        "rct2.scenery_small.tcf",  // Caucasian Fir Tree
        "rct2.scenery_small.trf",  // Red Fir Tree
        "rct2.scenery_small.trf2", // Red Fir Tree
        "rct2.scenery_small.tsp",  // Scots Pine Tree
        "rct2.scenery_small.tmzp", // Montezuma Pine Tree
        "rct2.scenery_small.tap",  // Aleppo Pine Tree
        "rct2.scenery_small.tcrp", // Corsican Pine Tree
        "rct2.scenery_small.tbp",  // Black Poplar Tree

        // Light
        "rct2.scenery_small.tcl", // Cedar of Lebanon Tree
        "rct2.scenery_small.tel", // European Larch Tree
    };

    static constexpr const char* kDesertTrees[] = {
        "rct2.scenery_small.tmp",    // Monkey-Puzzle Tree
        "rct2.scenery_small.thl",    // Honey Locust Tree
        "rct2.scenery_small.th1",    // Canary Palm Tree
        "rct2.scenery_small.th2",    // Palm Tree
        "rct2.scenery_small.tpm",    // Palm Tree
        "rct2.scenery_small.tropt1", // Tree
        "rct2.scenery_small.tbc",    // Cactus
        "rct2.scenery_small.tsc",    // Cactus
    };

    static constexpr const char* kSnowTrees[] = {
        "rct2.scenery_small.tcfs", // Snow-covered Caucasian Fir Tree
        "rct2.scenery_small.tnss", // Snow-covered Norway Spruce Tree
        "rct2.scenery_small.trf3", // Snow-covered Red Fir Tree
        "rct2.scenery_small.trfs", // Snow-covered Red Fir Tree
    };

    static void placeTree(ObjectEntryIndex type, const CoordsXY& loc)
    {
        auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(type);
        if (sceneryEntry == nullptr)
        {
            return;
        }

        int32_t surfaceZ = TileElementHeight(loc.ToTileCentre());

        auto* sceneryElement = TileElementInsert<SmallSceneryElement>({ loc, surfaceZ }, 0b1111);
        Guard::Assert(sceneryElement != nullptr);

        sceneryElement->SetClearanceZ(surfaceZ + sceneryEntry->height);
        sceneryElement->SetDirection(UtilRand() & 3);
        sceneryElement->SetEntryIndex(type);
        sceneryElement->SetAge(0);
        sceneryElement->SetPrimaryColour(COLOUR_YELLOW);
    }

    static bool surfaceTakesGrassTrees(const TerrainSurfaceObject& surface)
    {
        const auto& id = surface.GetIdentifier();
        return id == "rct2.terrain_surface.grass" || id == "rct2.terrain_surface.grass_clumps"
            || id == "rct2.terrain_surface.dirt";
    }

    static bool surfaceTakesSandTrees(const TerrainSurfaceObject& surface)
    {
        const auto& id = surface.GetIdentifier();
        return id == "rct2.terrain_surface.sand" || id == "rct2.terrain_surface.sand_brown"
            || id == "rct2.terrain_surface.sand_red";
    }

    static bool surfaceTakesSnowTrees(const TerrainSurfaceObject& surface)
    {
        const auto& id = surface.GetIdentifier();
        return id == "rct2.terrain_surface.ice";
    }

    template<typename T>
    static bool TryFindTreeInList(std::string_view id, const T& treeList)
    {
        for (size_t j = 0; j < std::size(treeList); j++)
        {
            if (treeList[j] == id)
                return true;
        }
        return false;
    }

    /**
     * Randomly places a selection of preset trees on the map. Picks the right tree for the terrain it is placing it on.
     */
    void placeTrees(Settings* settings)
    {
        std::vector<int32_t> grassTreeIds;
        std::vector<int32_t> desertTreeIds;
        std::vector<int32_t> snowTreeIds;

        for (auto i = 0u; i < getObjectEntryGroupCount(ObjectType::SmallScenery); i++)
        {
            auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(i);
            auto entry = ObjectEntryGetObject(ObjectType::SmallScenery, i);

            if (sceneryEntry == nullptr)
                continue;

            if (TryFindTreeInList(entry->GetIdentifier(), kGrassTrees))
            {
                grassTreeIds.push_back(i);
            }
            else if (TryFindTreeInList(entry->GetIdentifier(), kDesertTrees))
            {
                desertTreeIds.push_back(i);
            }
            else if (TryFindTreeInList(entry->GetIdentifier(), kSnowTrees))
            {
                snowTreeIds.push_back(i);
            }
        }

        // Place trees
        float treeToLandRatio = static_cast<float>(settings->treeToLandRatio) / 100.0f;

        // Randomise simplex noise
        NoiseRand();

        auto& gameState = GetGameState();
        for (int32_t y = 1; y < gameState.MapSize.y - 1; y++)
        {
            for (int32_t x = 1; x < gameState.MapSize.x - 1; x++)
            {
                auto pos = CoordsXY{ x, y } * kCoordsXYStep;
                auto* surfaceElement = MapGetSurfaceElementAt(pos);
                if (surfaceElement == nullptr)
                    continue;

                // Don't place on water
                if (surfaceElement->GetWaterHeight() > 0)
                    continue;

                if (settings->minTreeAltitude > surfaceElement->BaseHeight
                    || settings->maxTreeAltitude < surfaceElement->BaseHeight)
                    continue;

                // On sand surfaces, give the tile a score based on nearby water, to be used to determine whether to spawn
                // vegetation
                float oasisScore = 0.0f;
                ObjectEntryIndex treeObjectEntryIndex = kObjectEntryIndexNull;
                const auto& surfaceStyleObject = *TerrainSurfaceObject::GetById(surfaceElement->GetSurfaceObjectIndex());
                if (surfaceTakesSandTrees(surfaceStyleObject))
                {
                    oasisScore = -0.5f;
                    constexpr auto maxOasisDistance = 4;
                    for (int32_t offsetY = -maxOasisDistance; offsetY <= maxOasisDistance; offsetY++)
                    {
                        for (int32_t offsetX = -maxOasisDistance; offsetX <= maxOasisDistance; offsetX++)
                        {
                            // Get map coord, clamped to the edges
                            const auto offset = CoordsXY{ offsetX * kCoordsXYStep, offsetY * kCoordsXYStep };
                            auto neighbourPos = pos + offset;
                            neighbourPos.x = std::clamp(
                                neighbourPos.x, kCoordsXYStep, kCoordsXYStep * (gameState.MapSize.x - 1));
                            neighbourPos.y = std::clamp(
                                neighbourPos.y, kCoordsXYStep, kCoordsXYStep * (gameState.MapSize.y - 1));

                            const auto neighboutSurface = MapGetSurfaceElementAt(neighbourPos);
                            if (neighboutSurface != nullptr && neighboutSurface->GetWaterHeight() > 0)
                            {
                                float distance = std::sqrt(offsetX * offsetX + offsetY * offsetY);
                                oasisScore += 0.5f / (maxOasisDistance * distance);
                            }
                        }
                    }
                }

                // Use tree:land ratio except when near an oasis
                constexpr static auto randModulo = 0xFFFF;
                if (static_cast<float>(UtilRand() & randModulo) / randModulo > std::max(treeToLandRatio, oasisScore))
                    continue;

                // Use fractal noise to group tiles that are likely to spawn trees together
                float noiseValue = FractalNoise(x, y, 0.025f, 2, 2.0f, 0.65f);
                // Reduces the range to rarely stray further than 0.5 from the mean.
                float noiseOffset = UtilRandNormalDistributed() * 0.25f;
                if (noiseValue + oasisScore < noiseOffset)
                    continue;

                if (!grassTreeIds.empty() && surfaceTakesGrassTrees(surfaceStyleObject))
                {
                    treeObjectEntryIndex = grassTreeIds[UtilRand() % grassTreeIds.size()];
                }
                else if (!desertTreeIds.empty() && surfaceTakesSandTrees(surfaceStyleObject))
                {
                    treeObjectEntryIndex = desertTreeIds[UtilRand() % desertTreeIds.size()];
                }
                else if (!snowTreeIds.empty() && surfaceTakesSnowTrees(surfaceStyleObject))
                {
                    treeObjectEntryIndex = snowTreeIds[UtilRand() % snowTreeIds.size()];
                }

                if (treeObjectEntryIndex != kObjectEntryIndexNull)
                    placeTree(treeObjectEntryIndex, pos);
            }
        }
    }
} // namespace OpenRCT2::World::MapGenerator
