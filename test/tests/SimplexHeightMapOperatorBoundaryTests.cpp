#include <gtest/gtest.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/map_generator/HeightMap.hpp>
#include <openrct2/world/map_generator/SimplexNoise.h>

void testHeightMapOperator(const int32_t x, const int32_t y, const OpenRCT2::World::MapGenerator::HeightMap& heightMap);

TEST(SimplexHeightMapOperatorBoundaryTests, generateSimplexNoise)
{
    constexpr uint8_t kDensity = 2;
    constexpr uint16_t kMinimumMapSize = 5;  // kMinimumMapSizeTechnical = 5
    constexpr uint16_t kMaximumMapSize = 10; // kMaximumMapSizeTechnical = 1001

    using namespace OpenRCT2::World::MapGenerator;

    for (uint16_t mapSizeY = kMinimumMapSize; mapSizeY < kMaximumMapSize + 1; mapSizeY++)
    {
        for (uint16_t mapSizeX = kMinimumMapSize; mapSizeX < kMaximumMapSize + 1; mapSizeX++)
        {
            auto heightMap = HeightMap{ static_cast<int32_t>(mapSizeX), static_cast<int32_t>(mapSizeY), kDensity };

            // ..

            for (auto y = 0; y < heightMap.height; y++)
            {
                for (auto x = 0; x < heightMap.width; x++)
                {
                    // ..

                    testHeightMapOperator(x, y, heightMap);
                }
            }
        }
    }
}

TEST(SimplexHeightMapOperatorBoundaryTests, smoothHeightMap)
{
    constexpr uint8_t kDensity = 2;
    constexpr uint16_t kMinimumMapSize = 5;  // kMinimumMapSizeTechnical = 5
    constexpr uint16_t kMaximumMapSize = 10; // kMaximumMapSizeTechnical = 1001

    using namespace OpenRCT2::World::MapGenerator;

    for (uint16_t mapSizeY = kMinimumMapSize; mapSizeY < kMaximumMapSize + 1; mapSizeY++)
    {
        for (uint16_t mapSizeX = kMinimumMapSize; mapSizeX < kMaximumMapSize + 1; mapSizeX++)
        {
            auto heightMap = HeightMap{ static_cast<int32_t>(mapSizeX), static_cast<int32_t>(mapSizeY), kDensity };

            for (uint32_t i = 0; i < 2 + (UtilRand() % 6); i++)
            {
                auto copyHeight = heightMap;

                for (auto y = 1; y < heightMap.height - 1; y++)
                {
                    for (auto x = 1; x < heightMap.width - 1; x++)
                    {
                        for (auto yy = -1; yy <= 1; yy++)
                        {
                            for (auto xx = -1; xx <= 1; xx++)
                            {
                                testHeightMapOperator(x + xx, y + yy, copyHeight);
                            }
                        }
                        // ..

                        testHeightMapOperator(x, y, heightMap);
                    }
                }
            }
        }
    }
}

TEST(SimplexHeightMapOperatorBoundaryTests, setMapHeight)
{
    constexpr uint8_t kDensity = 2;
    constexpr uint16_t kMinimumMapSize = 5;  // kMinimumMapSizeTechnical = 5
    constexpr uint16_t kMaximumMapSize = 10; // kMaximumMapSizeTechnical = 1001

    using namespace OpenRCT2::World::MapGenerator;

    for (uint16_t mapSizeY = kMinimumMapSize; mapSizeY < kMaximumMapSize + 1; mapSizeY++)
    {
        for (uint16_t mapSizeX = kMinimumMapSize; mapSizeX < kMaximumMapSize + 1; mapSizeX++)
        {
            auto heightMap = HeightMap{ static_cast<int32_t>(mapSizeX), static_cast<int32_t>(mapSizeY), kDensity };

            for (auto y = 1; y < heightMap.height / heightMap.density - 1; y++)
            {
                for (auto x = 1; x < heightMap.width / heightMap.density - 1; x++)
                {
                    const auto heightX = x * heightMap.density;
                    const auto heightY = y * heightMap.density;

                    testHeightMapOperator(heightX + 0, heightY + 0, heightMap);
                    testHeightMapOperator(heightX + 0, heightY + 1, heightMap);
                    testHeightMapOperator(heightX + 1, heightY + 0, heightMap);
                    testHeightMapOperator(heightX + 1, heightY + 1, heightMap);

                    // ..
                }
            }
        }
    }
}

void testHeightMapOperator(const int32_t x, const int32_t y, const OpenRCT2::World::MapGenerator::HeightMap& heightMap)
{
    ASSERT_LT((static_cast<size_t>(y) * static_cast<size_t>(heightMap.width)) + static_cast<size_t>(x), heightMap.size());
}
