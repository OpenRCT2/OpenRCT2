/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapGen.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../core/Guard.hpp"
#include "../core/Imaging.h"
#include "../core/String.hpp"
#include "../localisation/StringIds.h"
#include "../object/ObjectEntryManager.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/SmallSceneryEntry.h"
#include "../object/TerrainEdgeObject.h"
#include "../object/TerrainSurfaceObject.h"
#include "../platform/Platform.h"
#include "../util/Util.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TileElement.h"
#include "Map.h"
#include "MapHelpers.h"
#include "Scenery.h"

#include <cmath>
#include <cstring>
#include <iterator>
#include <vector>

using namespace OpenRCT2;

#pragma region Height map struct

static struct
{
    uint32_t width = 0;
    uint32_t height = 0;
    std::vector<uint8_t> mono_bitmap;
} _heightMapData;

#pragma endregion Height map struct

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
static void MapGenGenerateFromHeightmapImage(MapGenSettings* settings);

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

static float FractalNoise(int32_t x, int32_t y, float frequency, int32_t octaves, float lacunarity, float persistence);
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

static ObjectEntryIndex MapGenSurfaceTextureId(MapGenSettings* settings)
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

static ObjectEntryIndex MapGenEdgeTextureId(MapGenSettings* settings, ObjectEntryIndex surfaceTextureId)
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

#pragma region Noise

/**
 * Simplex Noise Algorithm with Fractional Brownian Motion
 * Based on:
 *   - https://code.google.com/p/simplexnoise/
 *   - https://code.google.com/p/fractalterraingeneration/wiki/Fractional_Brownian_Motion
 */

static float Generate(float x, float y);
static int32_t FastFloor(float x);
static float Grad(int32_t hash, float x, float y);

static uint8_t perm[512];

static void NoiseRand()
{
    for (auto& i : perm)
    {
        i = UtilRand() & 0xFF;
    }
}

static float FractalNoise(int32_t x, int32_t y, float frequency, int32_t octaves, float lacunarity, float persistence)
{
    float total = 0.0f;
    float amplitude = persistence;
    for (int32_t i = 0; i < octaves; i++)
    {
        total += Generate(x * frequency, y * frequency) * amplitude;
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    return total;
}

static float Generate(float x, float y)
{
    const float F2 = 0.366025403f; // F2 = 0.5*(sqrt(3.0)-1.0)
    const float G2 = 0.211324865f; // G2 = (3.0-sqrt(3.0))/6.0

    float n0, n1, n2; // Noise contributions from the three corners

    // Skew the input space to determine which simplex cell we're in
    float s = (x + y) * F2; // Hairy factor for 2D
    float xs = x + s;
    float ys = y + s;
    int32_t i = FastFloor(xs);
    int32_t j = FastFloor(ys);

    float t = static_cast<float>(i + j) * G2;
    float X0 = i - t; // Unskew the cell origin back to (x,y) space
    float Y0 = j - t;
    float x0 = x - X0; // The x,y distances from the cell origin
    float y0 = y - Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int32_t i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0)
    {
        i1 = 1;
        j1 = 0;
    } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    else
    {
        i1 = 0;
        j1 = 1;
    } // upper triangle, YX order: (0,0)->(0,1)->(1,1)

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
    float y1 = y0 - j1 + G2;
    float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
    float y2 = y0 - 1.0f + 2.0f * G2;

    // Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
    int32_t ii = i % 256;
    int32_t jj = j % 256;

    // Calculate the contribution from the three corners
    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 < 0.0f)
    {
        n0 = 0.0f;
    }
    else
    {
        t0 *= t0;
        n0 = t0 * t0 * Grad(perm[ii + perm[jj]], x0, y0);
    }

    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if (t1 < 0.0f)
    {
        n1 = 0.0f;
    }
    else
    {
        t1 *= t1;
        n1 = t1 * t1 * Grad(perm[ii + i1 + perm[jj + j1]], x1, y1);
    }

    float t2 = 0.5f - x2 * x2 - y2 * y2;
    if (t2 < 0.0f)
    {
        n2 = 0.0f;
    }
    else
    {
        t2 *= t2;
        n2 = t2 * t2 * Grad(perm[ii + 1 + perm[jj + 1]], x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 40.0f * (n0 + n1 + n2); // TODO: The scale factor is preliminary!
}

static int32_t FastFloor(float x)
{
    return (x > 0) ? (static_cast<int32_t>(x)) : ((static_cast<int32_t>(x)) - 1);
}

static float Grad(int32_t hash, float x, float y)
{
    int32_t h = hash & 7;    // Convert low 3 bits of hash code
    float u = h < 4 ? x : y; // into 8 simple gradient directions,
    float v = h < 4 ? y : x; // and compute the dot product with (x,y).
    return ((h & 1) != 0 ? -u : u) + ((h & 2) != 0 ? -2.0f * v : 2.0f * v);
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

#pragma endregion

#pragma region Heightmap

/**
 * Return the tile coordinate that matches the given pixel of a heightmap
 */
static TileCoordsXY MapgenHeightmapCoordToTileCoordsXY(uint32_t x, uint32_t y)
{
    // The height map does not include the empty tiles around the map, so we add 1.
    return TileCoordsXY(static_cast<int32_t>(y + 1), static_cast<int32_t>(x + 1));
}

bool MapGenLoadHeightmapImage(const utf8* path)
{
    auto format = Imaging::GetImageFormatFromPath(path);
    if (format == IMAGE_FORMAT::PNG)
    {
        // Promote to 32-bit
        format = IMAGE_FORMAT::PNG_32;
    }

    try
    {
        auto image = Imaging::ReadFromFile(path, format);
        auto width = std::min<uint32_t>(image.Width, kMaximumMapSizePractical);
        auto height = std::min<uint32_t>(image.Height, kMaximumMapSizePractical);
        if (width != image.Width || height != image.Height)
        {
            ContextShowError(STR_HEIGHT_MAP_ERROR, STR_ERROR_HEIGHT_MAP_TOO_BIG, {});
        }

        // Allocate memory for the height map values, one byte pixel
        _heightMapData.mono_bitmap.resize(width * height);
        _heightMapData.width = width;
        _heightMapData.height = height;

        // Copy average RGB value to mono bitmap
        constexpr auto numChannels = 4;
        const auto pitch = image.Stride;
        const auto pixels = image.Pixels.data();
        for (uint32_t x = 0; x < _heightMapData.width; x++)
        {
            for (uint32_t y = 0; y < _heightMapData.height; y++)
            {
                const auto red = pixels[x * numChannels + y * pitch];
                const auto green = pixels[x * numChannels + y * pitch + 1];
                const auto blue = pixels[x * numChannels + y * pitch + 2];
                _heightMapData.mono_bitmap[x + y * _heightMapData.width] = (red + green + blue) / 3;
            }
        }
        return true;
    }
    catch (const std::exception& e)
    {
        switch (format)
        {
            case IMAGE_FORMAT::BITMAP:
                ContextShowError(STR_HEIGHT_MAP_ERROR, STR_ERROR_READING_BITMAP, {});
                break;
            case IMAGE_FORMAT::PNG_32:
                ContextShowError(STR_HEIGHT_MAP_ERROR, STR_ERROR_READING_PNG, {});
                break;
            default:
                LOG_ERROR("Unable to load height map image: %s", e.what());
                break;
        }
        return false;
    }
}

/**
 * Frees the memory used to store the selected height map
 */
void MapGenUnloadHeightmapImage()
{
    _heightMapData.mono_bitmap.clear();
    _heightMapData.width = 0;
    _heightMapData.height = 0;
}

/**
 * Applies box blur to the surface N times
 */
static void MapGenSmoothHeightmap(std::vector<uint8_t>& src, int32_t strength)
{
    // Create buffer to store one channel
    std::vector<uint8_t> dest(src.size());

    for (int32_t i = 0; i < strength; i++)
    {
        // Calculate box blur value to all pixels of the surface
        for (uint32_t y = 0; y < _heightMapData.height; y++)
        {
            for (uint32_t x = 0; x < _heightMapData.width; x++)
            {
                uint32_t heightSum = 0;

                // Loop over neighbour pixels, all of them have the same weight
                for (int8_t offsetX = -1; offsetX <= 1; offsetX++)
                {
                    for (int8_t offsetY = -1; offsetY <= 1; offsetY++)
                    {
                        // Clamp x and y so they stay within the image
                        // This assumes the height map is not tiled, and increases the weight of the edges
                        const int32_t readX = std::clamp<int32_t>(x + offsetX, 0, _heightMapData.width - 1);
                        const int32_t readY = std::clamp<int32_t>(y + offsetY, 0, _heightMapData.height - 1);
                        heightSum += src[readX + readY * _heightMapData.width];
                    }
                }

                // Take average
                dest[x + y * _heightMapData.width] = heightSum / 9;
            }
        }

        // Now apply the blur to the source pixels
        for (uint32_t y = 0; y < _heightMapData.height; y++)
        {
            for (uint32_t x = 0; x < _heightMapData.width; x++)
            {
                src[x + y * _heightMapData.width] = dest[x + y * _heightMapData.width];
            }
        }
    }
}

static void MapGenGenerateFromHeightmapImage(MapGenSettings* settings)
{
    Guard::Assert(!_heightMapData.mono_bitmap.empty(), "No height map loaded");
    Guard::Assert(settings->heightmapHigh != settings->heightmapLow, "Low and high setting cannot be the same");

    // Make a copy of the original height map that we can edit
    auto dest = _heightMapData.mono_bitmap;

    // Get technical map size, +2 for the black tiles around the map
    auto maxWidth = static_cast<int32_t>(_heightMapData.width + 2);
    auto maxHeight = static_cast<int32_t>(_heightMapData.height + 2);
    MapInit({ maxHeight, maxWidth });

    if (settings->smooth_height_map)
    {
        MapGenSmoothHeightmap(dest, settings->smooth_strength);
    }

    uint8_t maxValue = 255;
    uint8_t minValue = 0;

    if (settings->normalize_height)
    {
        // Get highest and lowest pixel value
        maxValue = 0;
        minValue = 0xff;
        for (uint32_t y = 0; y < _heightMapData.height; y++)
        {
            for (uint32_t x = 0; x < _heightMapData.width; x++)
            {
                uint8_t value = dest[x + y * _heightMapData.width];
                maxValue = std::max(maxValue, value);
                minValue = std::min(minValue, value);
            }
        }

        if (minValue == maxValue)
        {
            ContextShowError(STR_HEIGHT_MAP_ERROR, STR_ERROR_CANNOT_NORMALIZE, {});
            return;
        }
    }

    Guard::Assert(maxValue > minValue, "Input range is invalid");
    Guard::Assert(settings->heightmapHigh > settings->heightmapLow, "Output range is invalid");

    const auto surfaceTextureId = MapGenSurfaceTextureId(settings);
    const auto edgeTextureId = MapGenEdgeTextureId(settings, surfaceTextureId);

    const uint8_t rangeIn = maxValue - minValue;
    const uint8_t rangeOut = (settings->heightmapHigh - settings->heightmapLow) * 2;

    for (uint32_t y = 0; y < _heightMapData.height; y++)
    {
        for (uint32_t x = 0; x < _heightMapData.width; x++)
        {
            // The x and y axis are flipped in the world, so this uses y for x and x for y.
            auto tileCoords = MapgenHeightmapCoordToTileCoordsXY(x, y);
            auto* const surfaceElement = MapGetSurfaceElementAt(tileCoords);
            if (surfaceElement == nullptr)
                continue;

            // Read value from bitmap, and convert its range
            uint8_t value = dest[x + y * _heightMapData.width];
            value = static_cast<uint8_t>(static_cast<float>(value - minValue) / rangeIn * rangeOut)
                + (settings->heightmapLow * 2);
            surfaceElement->BaseHeight = value;

            // Floor to even number
            surfaceElement->BaseHeight /= 2;
            surfaceElement->BaseHeight *= 2;
            surfaceElement->ClearanceHeight = surfaceElement->BaseHeight;

            // Set textures
            surfaceElement->SetSurfaceObjectIndex(surfaceTextureId);
            surfaceElement->SetEdgeObjectIndex(edgeTextureId);

            // Set water level
            if (surfaceElement->BaseHeight < settings->waterLevel)
            {
                surfaceElement->SetWaterHeight(settings->waterLevel * kCoordsZStep);
            }
        }
    }

    // Smooth tile edges
    if (settings->smoothTileEdges)
    {
        // Keep smoothing the entire map until no tiles are changed anymore
        while (true)
        {
            uint32_t numTilesChanged = 0;
            for (uint32_t y = 0; y < _heightMapData.height; y++)
            {
                for (uint32_t x = 0; x < _heightMapData.width; x++)
                {
                    auto tileCoords = MapgenHeightmapCoordToTileCoordsXY(x, y);
                    numTilesChanged += TileSmooth(tileCoords);
                }
            }

            if (numTilesChanged == 0)
                break;
        }
    }
}

#pragma endregion
