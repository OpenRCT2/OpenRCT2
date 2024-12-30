/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapGen.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../core/Guard.hpp"
#include "../../core/String.hpp"
#include "../../object/ObjectEntryManager.h"
#include "../../object/ObjectList.h"
#include "../../object/ObjectManager.h"
#include "../../object/SmallSceneryEntry.h"
#include "../../object/TerrainEdgeObject.h"
#include "../../object/TerrainSurfaceObject.h"
#include "../../platform/Platform.h"
#include "../../util/Util.h"
#include "../Map.h"
#include "../Scenery.h"
#include "../tile_element/Slope.h"
#include "../tile_element/SmallSceneryElement.h"
#include "../tile_element/SurfaceElement.h"
#include "../tile_element/TileElement.h"
#include "MapHelpers.h"
#include "PngTerrainGenerator.h"
#include "SimplexNoise.h"

#include <vector>

using namespace OpenRCT2;

#pragma region Random objects

static constexpr const char* GrassTrees[] = {
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

static constexpr const char* DesertTrees[] = {
    "rct2.scenery_small.tmp",    // Monkey-Puzzle Tree
    "rct2.scenery_small.thl",    // Honey Locust Tree
    "rct2.scenery_small.th1",    // Canary Palm Tree
    "rct2.scenery_small.th2",    // Palm Tree
    "rct2.scenery_small.tpm",    // Palm Tree
    "rct2.scenery_small.tropt1", // Tree
    "rct2.scenery_small.tbc",    // Cactus
    "rct2.scenery_small.tsc",    // Cactus
};

static constexpr const char* SnowTrees[] = {
    "rct2.scenery_small.tcfs", // Snow-covered Caucasian Fir Tree
    "rct2.scenery_small.tnss", // Snow-covered Norway Spruce Tree
    "rct2.scenery_small.trf3", // Snow-covered Red Fir Tree
    "rct2.scenery_small.trfs", // Snow-covered Red Fir Tree
};

#pragma endregion

// Randomly chosen base terrains. We rarely want a whole map made out of chequerboard or rock.
static constexpr std::string_view BaseTerrain[] = {
    "rct2.terrain_surface.grass", "rct2.terrain_surface.sand", "rct2.terrain_surface.sand_brown",
    "rct2.terrain_surface.dirt",  "rct2.terrain_surface.ice",
};

static void MapGenGenerateBlank(MapGenSettings* settings);
static void MapGenGenerateSimplex(MapGenSettings* settings);

static void MapGenPlaceTrees(MapGenSettings* settings);
static void MapGenAddBeaches(MapGenSettings* settings);

void MapGenGenerate(MapGenSettings* settings)
{
    // First, generate the height map
    switch (settings->algorithm)
    {
        case MapGenAlgorithm::blank:
            MapGenGenerateBlank(settings);
            break;

        case MapGenAlgorithm::simplexNoise:
            MapGenGenerateSimplex(settings);
            break;

        case MapGenAlgorithm::heightmapImage:
            MapGenGenerateFromHeightmapImage(settings);
            break;
    }

    // Add beaches?
    if (settings->beaches)
        MapGenAddBeaches(settings);

    // Place trees?
    if (settings->trees)
        MapGenPlaceTrees(settings);
}

static void MapGenSetWaterLevel(int32_t waterLevel);
static void MapGenSmoothHeight(int32_t iterations);
static void MapGenSetHeight(MapGenSettings* settings);

static void MapGenSimplex(MapGenSettings* settings);

static TileCoordsXY _heightSize;
static uint8_t* _height;

static int32_t GetHeight(int32_t x, int32_t y)
{
    if (x >= 0 && y >= 0 && x < _heightSize.x && y < _heightSize.y)
        return _height[x + y * _heightSize.x];

    return 0;
}

static void SetHeight(int32_t x, int32_t y, int32_t height)
{
    if (x >= 0 && y >= 0 && x < _heightSize.x && y < _heightSize.y)
        _height[x + y * _heightSize.x] = height;
}

ObjectEntryIndex MapGenSurfaceTextureId(MapGenSettings* settings)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();

    const auto selectedFloor = TerrainSurfaceObject::GetById(settings->landTexture);
    std::string_view surfaceTexture = selectedFloor != nullptr ? selectedFloor->GetIdentifier() : "";

    if (surfaceTexture.empty())
    {
        std::vector<std::string_view> availableTerrains;
        std::copy_if(std::begin(BaseTerrain), std::end(BaseTerrain), std::back_inserter(availableTerrains), [&](auto terrain) {
            return objectManager.GetLoadedObject(ObjectEntryDescriptor(terrain)) != nullptr;
        });

        if (availableTerrains.empty())
            // Fall back to the first available surface texture that is available in the park
            surfaceTexture = TerrainSurfaceObject::GetById(0)->GetIdentifier();
        else
            surfaceTexture = availableTerrains[UtilRand() % availableTerrains.size()];
    }

    auto surfaceTextureId = objectManager.GetLoadedObjectEntryIndex(ObjectEntryDescriptor(surfaceTexture));
    return surfaceTextureId;
}

ObjectEntryIndex MapGenEdgeTextureId(MapGenSettings* settings, ObjectEntryIndex surfaceTextureId)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();

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

static void MapGenResetSurfaces(MapGenSettings* settings)
{
    MapClearAllElements();
    MapInit(settings->mapSize);

    const auto surfaceTextureId = MapGenSurfaceTextureId(settings);
    const auto edgeTextureId = MapGenEdgeTextureId(settings, surfaceTextureId);

    for (auto y = 1; y < settings->mapSize.y - 1; y++)
    {
        for (auto x = 1; x < settings->mapSize.x - 1; x++)
        {
            auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
            if (surfaceElement != nullptr)
            {
                surfaceElement->SetSurfaceObjectIndex(surfaceTextureId);
                surfaceElement->SetEdgeObjectIndex(edgeTextureId);
                surfaceElement->BaseHeight = settings->heightmapLow;
                surfaceElement->ClearanceHeight = settings->heightmapLow;
            }
        }
    }
}

static void MapGenGenerateBlank(MapGenSettings* settings)
{
    MapGenResetSurfaces(settings);
    MapGenSetWaterLevel(settings->waterLevel);
}

static void MapGenGenerateSimplex(MapGenSettings* settings)
{
    MapGenResetSurfaces(settings);

    // Create the temporary height map and initialise
    auto& mapSize = settings->mapSize;
    _heightSize = { mapSize.x * 2, mapSize.y * 2 };
    _height = new uint8_t[_heightSize.y * _heightSize.x];
    std::fill_n(_height, _heightSize.y * _heightSize.x, 0x00);

    MapGenSimplex(settings);
    MapGenSmoothHeight(2 + (UtilRand() % 6));

    // Set the game map to the height map
    MapGenSetHeight(settings);
    delete[] _height;

    if (settings->smoothTileEdges)
    {
        // Set the tile slopes so that there are no cliffs
        while (MapSmooth(1, 1, mapSize.x - 1, mapSize.y - 1))
        {
        }
    }

    // Add the water
    MapGenSetWaterLevel(settings->waterLevel);
}

static void MapGenAddBeaches(MapGenSettings* settings)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();

    // Figure out what beach texture to use
    std::vector<std::string_view> availableBeachTextures;
    if (objectManager.GetLoadedObject(ObjectEntryDescriptor("rct2.terrain_surface.sand")) != nullptr)
        availableBeachTextures.push_back("rct2.terrain_surface.sand");
    if (objectManager.GetLoadedObject(ObjectEntryDescriptor("rct2.terrain_surface.sand_brown")) != nullptr)
        availableBeachTextures.push_back("rct2.terrain_surface.sand_brown");

    if (availableBeachTextures.empty())
        return;

    std::string_view beachTexture = availableBeachTextures[UtilRand() % availableBeachTextures.size()];
    auto beachTextureId = objectManager.GetLoadedObjectEntryIndex(ObjectEntryDescriptor(beachTexture));

    // Add sandy beaches
    const auto& mapSize = settings->mapSize;
    for (auto y = 1; y < mapSize.y - 1; y++)
    {
        for (auto x = 1; x < mapSize.x - 1; x++)
        {
            auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });

            if (surfaceElement != nullptr && surfaceElement->BaseHeight < settings->waterLevel + 6)
                surfaceElement->SetSurfaceObjectIndex(beachTextureId);
        }
    }
}

static void MapGenPlaceTree(ObjectEntryIndex type, const CoordsXY& loc)
{
    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(type);
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

static bool MapGenSurfaceTakesGrassTrees(const TerrainSurfaceObject& surface)
{
    const auto& id = surface.GetIdentifier();
    return id == "rct2.terrain_surface.grass" || id == "rct2.terrain_surface.grass_clumps" || id == "rct2.terrain_surface.dirt";
}

static bool MapGenSurfaceTakesSandTrees(const TerrainSurfaceObject& surface)
{
    const auto& id = surface.GetIdentifier();
    return id == "rct2.terrain_surface.sand" || id == "rct2.terrain_surface.sand_brown"
        || id == "rct2.terrain_surface.sand_red";
}

static bool MapGenSurfaceTakesSnowTrees(const TerrainSurfaceObject& surface)
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
static void MapGenPlaceTrees(MapGenSettings* settings)
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

        if (TryFindTreeInList(entry->GetIdentifier(), GrassTrees))
        {
            grassTreeIds.push_back(i);
        }
        else if (TryFindTreeInList(entry->GetIdentifier(), DesertTrees))
        {
            desertTreeIds.push_back(i);
        }
        else if (TryFindTreeInList(entry->GetIdentifier(), SnowTrees))
        {
            snowTreeIds.push_back(i);
        }
    }

    // Place trees
    float treeToLandRatio = static_cast<float>(settings->treeToLandRatio) / 100.0f;

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
            ObjectEntryIndex treeObjectEntryIndex = OBJECT_ENTRY_INDEX_NULL;
            const auto& surfaceStyleObject = *TerrainSurfaceObject::GetById(surfaceElement->GetSurfaceObjectIndex());
            if (MapGenSurfaceTakesSandTrees(surfaceStyleObject))
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
                        neighbourPos.x = std::clamp(neighbourPos.x, kCoordsXYStep, kCoordsXYStep * (gameState.MapSize.x - 1));
                        neighbourPos.y = std::clamp(neighbourPos.y, kCoordsXYStep, kCoordsXYStep * (gameState.MapSize.y - 1));

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

            if (!grassTreeIds.empty() && MapGenSurfaceTakesGrassTrees(surfaceStyleObject))
            {
                treeObjectEntryIndex = grassTreeIds[UtilRand() % grassTreeIds.size()];
            }
            else if (!desertTreeIds.empty() && MapGenSurfaceTakesSandTrees(surfaceStyleObject))
            {
                treeObjectEntryIndex = desertTreeIds[UtilRand() % desertTreeIds.size()];
            }
            else if (!snowTreeIds.empty() && MapGenSurfaceTakesSnowTrees(surfaceStyleObject))
            {
                treeObjectEntryIndex = snowTreeIds[UtilRand() % snowTreeIds.size()];
            }

            if (treeObjectEntryIndex != OBJECT_ENTRY_INDEX_NULL)
                MapGenPlaceTree(treeObjectEntryIndex, pos);
        }
    }
}

/**
 * Sets each tile's water level to the specified water level if underneath that water level.
 */
static void MapGenSetWaterLevel(int32_t waterLevel)
{
    auto& gameState = GetGameState();
    for (int32_t y = 1; y < gameState.MapSize.y - 1; y++)
    {
        for (int32_t x = 1; x < gameState.MapSize.x - 1; x++)
        {
            auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
            if (surfaceElement != nullptr && surfaceElement->BaseHeight < waterLevel)
                surfaceElement->SetWaterHeight(waterLevel * kCoordsZStep);
        }
    }
}

/**
 * Smooths the height map.
 */
static void MapGenSmoothHeight(int32_t iterations)
{
    int32_t i, x, y, xx, yy, avg;
    int32_t arraySize = _heightSize.y * _heightSize.x * sizeof(uint8_t);
    uint8_t* copyHeight = new uint8_t[arraySize];

    for (i = 0; i < iterations; i++)
    {
        std::memcpy(copyHeight, _height, arraySize);
        for (y = 1; y < _heightSize.y - 1; y++)
        {
            for (x = 1; x < _heightSize.x - 1; x++)
            {
                avg = 0;
                for (yy = -1; yy <= 1; yy++)
                {
                    for (xx = -1; xx <= 1; xx++)
                    {
                        avg += copyHeight[(y + yy) * _heightSize.x + (x + xx)];
                    }
                }
                avg /= 9;
                SetHeight(x, y, avg);
            }
        }
    }

    delete[] copyHeight;
}

/**
 * Sets the height of the actual game map tiles to the height map.
 */
static void MapGenSetHeight(MapGenSettings* settings)
{
    int32_t x, y, heightX, heightY;

    for (y = 1; y < _heightSize.y / 2 - 1; y++)
    {
        for (x = 1; x < _heightSize.x / 2 - 1; x++)
        {
            heightX = x * 2;
            heightY = y * 2;

            uint8_t q00 = GetHeight(heightX + 0, heightY + 0);
            uint8_t q01 = GetHeight(heightX + 0, heightY + 1);
            uint8_t q10 = GetHeight(heightX + 1, heightY + 0);
            uint8_t q11 = GetHeight(heightX + 1, heightY + 1);

            uint8_t baseHeight = (q00 + q01 + q10 + q11) / 4;

            auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
            if (surfaceElement == nullptr)
                continue;
            surfaceElement->BaseHeight = std::max(2, baseHeight * 2);

            // If base height is below water level, lower it to create more natural shorelines
            if (surfaceElement->BaseHeight >= 4 && surfaceElement->BaseHeight <= settings->waterLevel)
                surfaceElement->BaseHeight -= 2;

            surfaceElement->ClearanceHeight = surfaceElement->BaseHeight;

            uint8_t currentSlope = surfaceElement->GetSlope();

            if (q00 > baseHeight)
                currentSlope |= kTileSlopeSCornerUp;
            if (q01 > baseHeight)
                currentSlope |= kTileSlopeWCornerUp;
            if (q10 > baseHeight)
                currentSlope |= kTileSlopeECornerUp;
            if (q11 > baseHeight)
                currentSlope |= kTileSlopeNCornerUp;

            surfaceElement->SetSlope(currentSlope);
        }
    }
}

static void MapGenSimplex(MapGenSettings* settings)
{
    float freq = settings->simplex_base_freq / 100.0f * (1.0f / _heightSize.x);
    int32_t octaves = settings->simplex_octaves;

    int32_t low = settings->heightmapLow / 2;
    int32_t high = settings->heightmapHigh / 2 - low;

    NoiseRand();
    for (int32_t y = 0; y < _heightSize.y; y++)
    {
        for (int32_t x = 0; x < _heightSize.x; x++)
        {
            float noiseValue = std::clamp(FractalNoise(x, y, freq, octaves, 2.0f, 0.65f), -1.0f, 1.0f);
            float normalisedNoiseValue = (noiseValue + 1.0f) / 2.0f;

            SetHeight(x, y, low + static_cast<int32_t>(normalisedNoiseValue * high));
        }
    }
}
