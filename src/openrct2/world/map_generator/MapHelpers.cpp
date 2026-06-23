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
#include "River.h"

#include <algorithm>

namespace OpenRCT2::World::MapGenerator
{
    static uint8_t GetBaseHeightOrZero(int32_t x, int32_t y)
    {
        auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
        return surfaceElement != nullptr ? surfaceElement->baseHeight : 0;
    }

    static bool isRiverTile(const TileCoordsXY& tileCoords, const MapGenCtx& context)
    {
        TileCoordsXY genCoords = worldCoordsToGenCoords(context, tileCoords);
        return context.hydroMaps.has_value() && context.hydroMaps.value().flags[genCoords].has(river);
    }

    /**
     * Not perfect, this still leaves some particular tiles unsmoothed.
     */
    int32_t smoothTileSlopeStrong(const TileCoordsXY tileCoords, const MapGenCtx& context)
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

    template<class T>
    union NeighbourData
    {
        T neighbour[8];
        struct
        {
            T N;
            T NW;
            T W;
            T NE;
            T SW;
            T E;
            T SE;
            T S;
        };
    };

    /**
     * Raises the corners based on the height offset of neighbour tiles.
     * This does not change the base height, unless all corners have been raised.
     * @returns 0 if no edits were made, 1 otherwise
     */
    int32_t smoothTileSlopeWeak(const TileCoordsXY tileCoords, const MapGenCtx& context)
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

        NeighbourData<int32_t> neighbourHeightOffset = {};
        NeighbourData<int32_t> neighbourWaterOffset = {};
        NeighbourData<bool> r = {};

        const bool riverTile = isRiverTile(tileCoords, context);

        // Find the neighbour base heights
        for (int32_t index = 0, y_offset = -1; y_offset <= 1; y_offset++)
        {
            for (int32_t x_offset = -1; x_offset <= 1; x_offset++)
            {
                // Skip self
                if (y_offset == 0 && x_offset == 0)
                    continue;

                auto neighbourCoords = tileCoords + TileCoordsXY{ x_offset, y_offset };
                // Get neighbour height. If the element is not valid (outside of map) assume the same height
                auto* neighbourSurfaceElement = MapGetSurfaceElementAt(neighbourCoords);
                if (neighbourSurfaceElement != nullptr)
                {
                    neighbourHeightOffset.neighbour[index] = neighbourSurfaceElement->baseHeight;
                    neighbourWaterOffset.neighbour[index] = neighbourSurfaceElement->GetWaterHeight();
                }
                else
                {
                    neighbourHeightOffset.neighbour[index] = surfaceElement->baseHeight;
                    neighbourWaterOffset.neighbour[index] = surfaceElement->GetWaterHeight();
                }

                // Make the height relative to the current surface element
                neighbourHeightOffset.neighbour[index] -= surfaceElement->baseHeight;
                neighbourWaterOffset.neighbour[index] -= surfaceElement->GetWaterHeight();
                // Check if this is a river tile
                r.neighbour[index] = isRiverTile(neighbourCoords, context);

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

        // Make sure raising the tile corner doesn't block a river.
        // The first clause covers waterfalls and the other eight situations where one of the two edges connected to this corner
        // is a one tile wide channel.
        // TODO might be possible to reduce?

        const bool waterfallW = (r.W && neighbourWaterOffset.W > 0) || (r.NW && neighbourWaterOffset.NW > 0) || (r.SW && neighbourWaterOffset.SW > 0);
        const bool waterfallN = (r.N && neighbourWaterOffset.N > 0) || (r.NW && neighbourWaterOffset.NW > 0) || (r.NE && neighbourWaterOffset.NE > 0);
        const bool waterfallE = (r.E && neighbourWaterOffset.E > 0) || (r.NE && neighbourWaterOffset.NE > 0) || (r.SE && neighbourWaterOffset.SE > 0);
        const bool waterfallS = (r.S && neighbourWaterOffset.S > 0) || (r.SW && neighbourWaterOffset.SW > 0) || (r.SE && neighbourWaterOffset.SE > 0);

        const bool riverW = !riverTile || (!waterfallW && !(!r.S && !r.W && r.SW) && !(!r.N && !r.W && r.NW) && !(!r.S && !r.NW && r.SW) && !(!r.N && !r.SW && r.NW) && !(!r.W && !r.SE && r.SW) && !(!r.W && !r.NE && r.NW) && !(!r.SW && !r.NE && r.NW) && !(!r.SE && !r.NW && r.SW));
        const bool riverN = !riverTile || (!waterfallN && !(!r.W && !r.N && r.NW) && !(!r.E && !r.N && r.NE) && !(!r.W && !r.NE && r.NW) && !(!r.E && !r.NW && r.NE) && !(!r.N && !r.SW && r.NW) && !(!r.N && !r.SE && r.NE) && !(!r.NW && !r.SE && r.NE) && !(!r.SW && !r.NE && r.NW));
        const bool riverE = !riverTile || (!waterfallE && !(!r.N && !r.E && r.NE) && !(!r.S && !r.E && r.SE) && !(!r.N && !r.SE && r.NE) && !(!r.S && !r.NE && r.SE) && !(!r.E && !r.NW && r.NE) && !(!r.E && !r.SW && r.SE) && !(!r.NE && !r.SW && r.SE) && !(!r.NW && !r.SE && r.NE));
        const bool riverS = !riverTile || (!waterfallS && !(!r.E && !r.S && r.SE) && !(!r.W && !r.S && r.SW) && !(!r.E && !r.SW && r.SE) && !(!r.W && !r.SE && r.SW) && !(!r.S && !r.NE && r.SE) && !(!r.S && !r.NW && r.SW) && !(!r.SE && !r.NW && r.SW) && !(!r.NE && !r.SW && r.SE));

        uint8_t slope = kTileSlopeFlat;
        slope |= (thresholdW >= 1 && riverW) ? SLOPE_W_THRESHOLD_FLAGS : 0;
        slope |= (thresholdN >= 1 && riverN) ? SLOPE_N_THRESHOLD_FLAGS : 0;
        slope |= (thresholdE >= 1 && riverE) ? SLOPE_E_THRESHOLD_FLAGS : 0;
        slope |= (thresholdS >= 1 && riverS) ? SLOPE_S_THRESHOLD_FLAGS : 0;

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

    void applyTileSlopeSmooth(MapGenCtx& context)
    {
        const auto mapSize = context.settings.mapSize;

        SmoothFunction smoothFunc;

        switch (context.settings.slopeSmooth)
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
                    numTilesChanged += smoothFunc({ x, y }, context);
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

    void sharpen(HeightMap& heightMap, int32_t iterations)
    {
        for (auto i = 0; i < iterations; i++)
        {
            HeightMap blurCopy = heightMap;
            smoothGaussian(blurCopy, 7);

            for (auto y = 0; y < heightMap.height; y++)
            {
                for (auto x = 0; x < heightMap.width; x++)
                {
                    const TileCoordsXY pos = {x, y};
                    auto delta = heightMap[pos] - blurCopy[pos];
                    heightMap[pos] = heightMap[pos] + delta;
                }
            }
        }
    }

    void applyHeightMapTransform(MapGenCtx& context)
    {
        auto& settings = context.settings;
        auto& heightMap = context.heightMap;

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
            case HeightMapTransform::sharpen:
                sharpen(heightMap, settings.transformStrength);
                break;
            case HeightMapTransform::bilateral:
                smoothBilateral(heightMap, settings.transformStrength, 0.1f * settings.transformStrength);
                break;
            case HeightMapTransform::erosion:
                simulateErosion(context);
                break;
        }

        if (settings.generateRivers)
        {
            generateRivers(context);
        }
    }

    static TileCoordsXY getWorldCoordsOffset(const MapGenCtx& context)
    {
        const int32_t offsetX = (context.settings.mapSize.x * kRiversOverscanFactor - 1) / 2;
        const int32_t offsetY = (context.settings.mapSize.y * kRiversOverscanFactor - 1) / 2;
        return {offsetX, offsetY};
    }

    bool isInWorldMap(const MapGenCtx& context, const TileCoordsXY& genCoords)
    {
        const TileCoordsXY offset = getWorldCoordsOffset(context);

        const int32_t xMin = offset.x;
        const int32_t xMax = offset.x + context.settings.mapSize.x;
        const int32_t yMin = offset.y;
        const int32_t yMax = offset.y + context.settings.mapSize.y;

        return xMin <= genCoords.x && genCoords.x < xMax && yMin <= genCoords.y && genCoords.y < yMax;
    }

    TileCoordsXY worldCoordsToGenCoords(const MapGenCtx& context, const TileCoordsXY& worldCoords)
    {
        const TileCoordsXY offset = getWorldCoordsOffset(context);
        return {worldCoords.x + offset.x, worldCoords.y + offset.y};
    }

    // can be OOB
    TileCoordsXY genCoordsToWorldCoords(const MapGenCtx& context, const TileCoordsXY& genCoords)
    {
        const TileCoordsXY offset = getWorldCoordsOffset(context);
        return {genCoords.x - offset.x, genCoords.y - offset.y};
    }


    // Ensure height is within [2, 254] and a multiple of 2
    uint8_t quantizeHeight(const float height)
    {
        return static_cast<uint8_t>(std::round(std::clamp(height, 2.0f, 254.0f) * 0.5f) * 2.0f);
    }
} // namespace OpenRCT2::World::MapGenerator
