/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapHelpers.h"

#include "../Map.h"
#include "../tile_element/Slope.h"
#include "../tile_element/SurfaceElement.h"
#include "BaseMap.hpp"
#include "Erosion.h"

#include <algorithm>

namespace OpenRCT2::World::MapGenerator
{
    static uint8_t GetBaseHeightOrZero(int32_t x, int32_t y)
    {
        auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
        return surfaceElement != nullptr ? surfaceElement->baseHeight : 0;
    }

    /**
     * Not perfect, this still leaves some particular tiles unsmoothed.
     */
    int32_t smoothTileSlopeStrong(const TileCoordsXY tileCoords)
    {
        auto surfaceElement = MapGetSurfaceElementAt(tileCoords);
        if (surfaceElement == nullptr)
            return 0;

        surfaceElement->setSlope(kTileSlopeFlat);

        auto x = tileCoords.x, y = tileCoords.y;
        auto raisedLand = 0;

        // Raise to edge height - 2
        uint8_t highest = surfaceElement->baseHeight;
        highest = std::max(highest, GetBaseHeightOrZero(x - 1, y + 0));
        highest = std::max(highest, GetBaseHeightOrZero(x + 1, y + 0));
        highest = std::max(highest, GetBaseHeightOrZero(x + 0, y - 1));
        highest = std::max(highest, GetBaseHeightOrZero(x + 0, y + 1));
        if (surfaceElement->baseHeight < highest - 2)
        {
            raisedLand = 1;
            surfaceElement->baseHeight = surfaceElement->clearanceHeight = highest - 2;
        }

        // Check corners
        int32_t doubleCorner = -1;
        uint8_t cornerHeights[4] = {
            GetBaseHeightOrZero(x - 1, y - 1),
            GetBaseHeightOrZero(x + 1, y - 1),
            GetBaseHeightOrZero(x + 1, y + 1),
            GetBaseHeightOrZero(x - 1, y + 1),
        };

        highest = surfaceElement->baseHeight;
        for (std::size_t i = 0; i < std::size(cornerHeights); i++)
            highest = std::max(highest, cornerHeights[i]);

        if (highest >= surfaceElement->baseHeight + 4)
        {
            int32_t count = 0;
            int32_t canCompensate = 1;
            for (std::size_t i = 0; i < std::size(cornerHeights); i++)
            {
                if (cornerHeights[i] == highest)
                {
                    count++;

                    // Check if surrounding corners aren't too high. The current tile
                    // can't compensate for all the height differences anymore if it has
                    // the extra height slope.
                    int32_t highestOnLowestSide;
                    switch (i)
                    {
                        default:
                        case 0:
                            highestOnLowestSide = std::max(GetBaseHeightOrZero(x + 1, y), GetBaseHeightOrZero(x, y + 1));
                            break;
                        case 1:
                            highestOnLowestSide = std::max(GetBaseHeightOrZero(x - 1, y), GetBaseHeightOrZero(x, y + 1));
                            break;
                        case 2:
                            highestOnLowestSide = std::max(GetBaseHeightOrZero(x - 1, y), GetBaseHeightOrZero(x, y - 1));
                            break;
                        case 3:
                            highestOnLowestSide = std::max(GetBaseHeightOrZero(x + 1, y), GetBaseHeightOrZero(x, y - 1));
                            break;
                    }

                    if (highestOnLowestSide > surfaceElement->baseHeight)
                    {
                        surfaceElement->baseHeight = surfaceElement->clearanceHeight = highestOnLowestSide;
                        raisedLand = 1;
                        canCompensate = 0;
                    }
                }
            }

            if (count == 1 && canCompensate)
            {
                if (surfaceElement->baseHeight < highest - 4)
                {
                    surfaceElement->baseHeight = surfaceElement->clearanceHeight = highest - 4;
                    raisedLand = 1;
                }
                if (cornerHeights[0] == highest && cornerHeights[2] <= cornerHeights[0] - 4)
                    doubleCorner = 0;
                else if (cornerHeights[1] == highest && cornerHeights[3] <= cornerHeights[1] - 4)
                    doubleCorner = 1;
                else if (cornerHeights[2] == highest && cornerHeights[0] <= cornerHeights[2] - 4)
                    doubleCorner = 2;
                else if (cornerHeights[3] == highest && cornerHeights[1] <= cornerHeights[3] - 4)
                    doubleCorner = 3;
            }
            else
            {
                if (surfaceElement->baseHeight < highest - 2)
                {
                    surfaceElement->baseHeight = surfaceElement->clearanceHeight = highest - 2;
                    raisedLand = 1;
                }
            }
        }

        if (doubleCorner != -1)
        {
            uint8_t slope = surfaceElement->getSlope() | kTileSlopeDiagonalFlag;
            switch (doubleCorner)
            {
                case 0:
                    slope |= kTileSlopeNCornerDown;
                    break;
                case 1:
                    slope |= kTileSlopeWCornerDown;
                    break;
                case 2:
                    slope |= kTileSlopeSCornerDown;
                    break;
                case 3:
                    slope |= kTileSlopeECornerDown;
                    break;
            }
            surfaceElement->setSlope(slope);
        }
        else
        {
            uint8_t slope = surfaceElement->getSlope();
            // Corners
            auto surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x + 1, y + 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->baseHeight > surfaceElement->baseHeight)
                slope |= kTileSlopeNCornerUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x - 1, y + 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->baseHeight > surfaceElement->baseHeight)
                slope |= kTileSlopeWCornerUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x + 1, y - 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->baseHeight > surfaceElement->baseHeight)
                slope |= kTileSlopeECornerUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x - 1, y - 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->baseHeight > surfaceElement->baseHeight)
                slope |= kTileSlopeSCornerUp;

            // Sides
            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x + 1, y + 0 });
            if (surfaceElement2 != nullptr && surfaceElement2->baseHeight > surfaceElement->baseHeight)
                slope |= kTileSlopeNESideUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x - 1, y + 0 });
            if (surfaceElement2 != nullptr && surfaceElement2->baseHeight > surfaceElement->baseHeight)
                slope |= kTileSlopeSWSideUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x + 0, y - 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->baseHeight > surfaceElement->baseHeight)
                slope |= kTileSlopeSESideUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x + 0, y + 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->baseHeight > surfaceElement->baseHeight)
                slope |= kTileSlopeNWSideUp;

            // Raise
            if (slope == kTileSlopeRaisedCornersMask)
            {
                slope = kTileSlopeFlat;
                surfaceElement->baseHeight = surfaceElement->clearanceHeight += 2;
            }
            surfaceElement->setSlope(slope);
        }

        return raisedLand;
    }

    /**
     * Raises the corners based on the height offset of neighbour tiles.
     * This does not change the base height, unless all corners have been raised.
     * @returns 0 if no edits were made, 1 otherwise
     */
    int32_t smoothTileSlopeWeak(const TileCoordsXY tileCoords)
    {
        auto* const surfaceElement = MapGetSurfaceElementAt(tileCoords);
        if (surfaceElement == nullptr)
            return 0;

        // +-----+-----+-----+
        // |  W  | NW  |  N  |
        // |  2  |  1  |  0  |
        // +-----+-----+-----+
        // | SW  |  _  | NE  |
        // |  4  |     |  3  |
        // +-----+-----+-----+
        // |  S  | SE  |  E  |
        // |  7  |  6  |  5  |
        // +-----+-----+-----+

        union
        {
            int32_t baseheight[8];
            struct
            {
                int32_t N;
                int32_t NW;
                int32_t W;
                int32_t NE;
                int32_t SW;
                int32_t E;
                int32_t SE;
                int32_t S;
            };
        } neighbourHeightOffset = {};

        // Find the neighbour base heights
        for (int32_t index = 0, y_offset = -1; y_offset <= 1; y_offset++)
        {
            for (int32_t x_offset = -1; x_offset <= 1; x_offset++)
            {
                // Skip self
                if (y_offset == 0 && x_offset == 0)
                    continue;

                // Get neighbour height. If the element is not valid (outside of map) assume the same height
                auto* neighbourSurfaceElement = MapGetSurfaceElementAt(tileCoords + TileCoordsXY{ x_offset, y_offset });
                neighbourHeightOffset.baseheight[index] = neighbourSurfaceElement != nullptr
                    ? neighbourSurfaceElement->baseHeight
                    : surfaceElement->baseHeight;

                // Make the height relative to the current surface element
                neighbourHeightOffset.baseheight[index] -= surfaceElement->baseHeight;

                index++;
            }
        }

        // Count number from the three tiles that is currently higher
        int8_t thresholdW = std::clamp(neighbourHeightOffset.SW, 0, 1) + std::clamp(neighbourHeightOffset.W, 0, 1)
            + std::clamp(neighbourHeightOffset.NW, 0, 1);
        int8_t thresholdN = std::clamp(neighbourHeightOffset.NW, 0, 1) + std::clamp(neighbourHeightOffset.N, 0, 1)
            + std::clamp(neighbourHeightOffset.NE, 0, 1);
        int8_t thresholdE = std::clamp(neighbourHeightOffset.NE, 0, 1) + std::clamp(neighbourHeightOffset.E, 0, 1)
            + std::clamp(neighbourHeightOffset.SE, 0, 1);
        int8_t thresholdS = std::clamp(neighbourHeightOffset.SE, 0, 1) + std::clamp(neighbourHeightOffset.S, 0, 1)
            + std::clamp(neighbourHeightOffset.SW, 0, 1);

        uint8_t slope = kTileSlopeFlat;
        slope |= (thresholdW >= 1) ? SLOPE_W_THRESHOLD_FLAGS : 0;
        slope |= (thresholdN >= 1) ? SLOPE_N_THRESHOLD_FLAGS : 0;
        slope |= (thresholdE >= 1) ? SLOPE_E_THRESHOLD_FLAGS : 0;
        slope |= (thresholdS >= 1) ? SLOPE_S_THRESHOLD_FLAGS : 0;

        // Set diagonal when three corners (one corner down) have been raised, and the middle one can be raised one more
        if ((slope == kTileSlopeWCornerDown && neighbourHeightOffset.W >= 4)
            || (slope == kTileSlopeSCornerDown && neighbourHeightOffset.S >= 4)
            || (slope == kTileSlopeECornerDown && neighbourHeightOffset.E >= 4)
            || (slope == kTileSlopeNCornerDown && neighbourHeightOffset.N >= 4))
        {
            slope |= kTileSlopeDiagonalFlag;
        }

        // Check if the calculated slope is the same already
        uint8_t currentSlope = surfaceElement->getSlope();
        if (currentSlope == slope)
        {
            return 0;
        }

        if ((slope & kTileSlopeRaisedCornersMask) == kTileSlopeRaisedCornersMask)
        {
            // All corners are raised, raise the entire tile instead.
            surfaceElement->setSlope(kTileSlopeFlat);
            surfaceElement->baseHeight = (surfaceElement->clearanceHeight += 2);
            if (surfaceElement->getWaterHeight() <= surfaceElement->getBaseZ())
            {
                surfaceElement->setWaterHeight(0);
            }
        }
        else
        {
            // Apply the slope to this tile
            surfaceElement->setSlope(slope);

            // Set correct clearance height
            if (slope & kTileSlopeDiagonalFlag)
                surfaceElement->clearanceHeight = surfaceElement->baseHeight + 4;
            else if (slope & kTileSlopeRaisedCornersMask)
                surfaceElement->clearanceHeight = surfaceElement->baseHeight + 2;
        }

        return 1;
    }

    void applyTileSlopeSmooth(const Settings& settings)
    {
        const auto mapSize = settings.mapSize;

        SmoothFunction smoothFunc;

        switch (settings.slopeSmooth)
        {
            case SlopeSmooth::none:
                return;
            case SlopeSmooth::weak:
                smoothFunc = smoothTileSlopeWeak;
                break;
            case SlopeSmooth::strong:
                smoothFunc = smoothTileSlopeStrong;
                break;
        }

        while (true)
        {
            auto numTilesChanged = 0;

            for (auto y = 1; y < mapSize.y - 1; y++)
            {
                for (auto x = 1; x < mapSize.x - 1; x++)
                {
                    numTilesChanged += smoothFunc({ x, y });
                }
            }

            if (numTilesChanged == 0)
                break;
        }
    }

    static float gaussian(float delta, float sigma)
    {
        return std::exp(-0.5f * std::pow(delta / sigma, 2.0f));
    }

    void smoothBox(HeightMap& heightMap, int32_t iterations)
    {
        for (auto i = 0; i < iterations; i++)
        {
            auto copyHeight = heightMap;
            for (auto y = 0; y < heightMap.height; y++)
            {
                for (auto x = 0; x < heightMap.width; x++)
                {
                    auto avg = 0;
                    for (auto yy = -1; yy <= 1; yy++)
                    {
                        for (auto xx = -1; xx <= 1; xx++)
                        {
                            auto dx = x + xx;
                            auto dy = y + yy;

                            // Use tile height if OOB
                            auto oob = dy < 0 || dx < 0 || dy >= heightMap.width || dx >= heightMap.height;
                            avg += oob ? copyHeight[{ x, y }] : copyHeight[{ dx, dy }];
                        }
                    }
                    avg /= 9.0f;
                    heightMap[{ x, y }] = avg;
                }
            }
        }
    }

    void smoothGaussian(HeightMap& heightMap, float sigma)
    {
        HeightMap temp(heightMap.width, heightMap.height, heightMap.density);
        HeightMap norm(heightMap.width, heightMap.height, heightMap.density);

        temp.fill(0.0f);
        norm.fill(1.0f);

        const uint16_t d = static_cast<uint16_t>(2.0f * sigma + 1.0f);
        const uint16_t dHalf = d / 2;

        for (auto y = 0; y < heightMap.height; y++)
        {
            for (auto x = 0; x < heightMap.width; x++)
            {
                const TileCoordsXY pos = {x, y};

                for (auto dy = -dHalf; dy <= dHalf; dy++)
                {
                    for (auto dx = -dHalf; dx <= dHalf; dx++)
                    {
                        TileCoordsXY deltaPos = pos + TileCoordsXY{dx, dy};

                        if (deltaPos.x < 0 || deltaPos.x >= heightMap.width || deltaPos.y < 0 || deltaPos.y >= heightMap.height)
                        {
                            deltaPos = pos;
                        }

                        const float weight = gaussian(dx*dx + dy*dy, sigma);

                        temp[pos] += weight * heightMap[deltaPos];
                        norm[pos] += weight;
                    }
                }
            }
        }

        for (auto y = 0; y < heightMap.height; y++)
        {
            for (auto x = 0; x < heightMap.width; x++)
            {
                const TileCoordsXY pos = {x, y};
                heightMap[pos] = temp[pos] / norm[pos];
            }
        }
    }

    /**
 * Based on https://homepages.inf.ed.ac.uk/rbf/CVonline/LOCAL_COPIES/MANDUCHI1/Bilateral_Filtering.html
 */
    void smoothBilateral(HeightMap& heightMap, float sigmaSpace, float sigmaIntensity)
    {
        HeightMap temp(heightMap.width, heightMap.height, heightMap.density);
        HeightMap norm(heightMap.width, heightMap.height, heightMap.density);

        temp.fill(0.0f);
        norm.fill(1.0f);

        const uint16_t d = static_cast<uint16_t>(2.0f * sigmaSpace + 1.0f);
        const uint16_t dHalf = d / 2;

        for (auto y = 0; y < heightMap.height; y++)
        {
            for (auto x = 0; x < heightMap.width; x++)
            {
                const TileCoordsXY pos = {x, y};

                for (auto dy = -dHalf; dy <= dHalf; dy++)
                {
                    for (auto dx = -dHalf; dx <= dHalf; dx++)
                    {
                        TileCoordsXY deltaPos = pos + TileCoordsXY{dx, dy};

                        if (deltaPos.x < 0 || deltaPos.x >= heightMap.width || deltaPos.y < 0 || deltaPos.y >= heightMap.height)
                        {
                            deltaPos = pos;
                        }

                        const float closeness = gaussian(dx*dx + dy*dy, sigmaSpace);
                        const float similarity = gaussian(std::pow(heightMap[pos] - heightMap[deltaPos], 2.0f) / kMaxTileElementHeight, sigmaIntensity);

                        const float weight = closeness * similarity;

                        temp[pos] += weight * heightMap[deltaPos];
                        norm[pos] += weight;
                    }
                }
            }
        }

        for (auto y = 0; y < heightMap.height; y++)
        {
            for (auto x = 0; x < heightMap.width; x++)
            {
                const TileCoordsXY pos = {x, y};
                heightMap[pos] = temp[pos] / norm[pos];
            }
        }
    }

    void applyHeightMapTransform(HeightMap& heightMap, const Settings& settings)
    {
        switch (settings.heightmapTransform)
        {
            case HeightMapTransform::none:
                break;
            case HeightMapTransform::box:
                smoothBox(heightMap, settings.transformStrength);
                break;
            case HeightMapTransform::gaussian:
                smoothGaussian(heightMap, settings.transformStrength);
                break;
            case HeightMapTransform::bilateral:
                smoothBilateral(heightMap, settings.transformStrength, settings.transformStrength);
                break;
            case HeightMapTransform::erosion:
                simulateErosion(heightMap, settings);
                break;
        }
    }
} // namespace OpenRCT2::World::MapGenerator
