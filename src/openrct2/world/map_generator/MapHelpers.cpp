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
#include <numbers>

namespace OpenRCT2::World::MapGenerator
{
    static uint8_t GetBaseHeightOrZero(int32_t x, int32_t y)
    {
        auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
        return surfaceElement != nullptr ? surfaceElement->baseHeight : 0;
    }

    static bool isRiverTile(const MapGenContext& ctx, const TileCoordsXY& tileCoords)
    {
        return ctx.hydroContext.has_value()
            && ctx.hydroContext.value().flags[worldCoordsToGenCoords(ctx, tileCoords)].has(Hydro::river);
    }

    /**
     * Not perfect, this still leaves some particular tiles unsmoothed.
     */
    int32_t smoothTileSlopeStrong(const MapGenContext& ctx, const TileCoordsXY tileCoords)
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
    int32_t smoothTileSlopeWeak(const MapGenContext& ctx, const TileCoordsXY tileCoords)
    {
        auto* const surfaceElement = MapGetSurfaceElementAt(tileCoords);
        if (surfaceElement == nullptr)
            return 0;

        NeighbourData<int32_t> neighbourHeightOffset = {};
        NeighbourData<int32_t> neighbourWaterOffset = {};
        NeighbourData<bool> r = {};

        const bool riverTile = isRiverTile(ctx, tileCoords);

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
                r.neighbour[index] = isRiverTile(ctx, neighbourCoords);

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

        const bool waterfallW = (r.W && neighbourWaterOffset.W > 0) || (r.NW && neighbourWaterOffset.NW > 0)
            || (r.SW && neighbourWaterOffset.SW > 0);
        const bool waterfallN = (r.N && neighbourWaterOffset.N > 0) || (r.NW && neighbourWaterOffset.NW > 0)
            || (r.NE && neighbourWaterOffset.NE > 0);
        const bool waterfallE = (r.E && neighbourWaterOffset.E > 0) || (r.NE && neighbourWaterOffset.NE > 0)
            || (r.SE && neighbourWaterOffset.SE > 0);
        const bool waterfallS = (r.S && neighbourWaterOffset.S > 0) || (r.SW && neighbourWaterOffset.SW > 0)
            || (r.SE && neighbourWaterOffset.SE > 0);

        const bool riverW = !riverTile
            || (!waterfallW && !(!r.S && !r.W && r.SW) && !(!r.N && !r.W && r.NW) && !(!r.S && !r.NW && r.SW)
                && !(!r.N && !r.SW && r.NW) && !(!r.W && !r.SE && r.SW) && !(!r.W && !r.NE && r.NW) && !(!r.SW && !r.NE && r.NW)
                && !(!r.SE && !r.NW && r.SW));
        const bool riverN = !riverTile
            || (!waterfallN && !(!r.W && !r.N && r.NW) && !(!r.E && !r.N && r.NE) && !(!r.W && !r.NE && r.NW)
                && !(!r.E && !r.NW && r.NE) && !(!r.N && !r.SW && r.NW) && !(!r.N && !r.SE && r.NE) && !(!r.NW && !r.SE && r.NE)
                && !(!r.SW && !r.NE && r.NW));
        const bool riverE = !riverTile
            || (!waterfallE && !(!r.N && !r.E && r.NE) && !(!r.S && !r.E && r.SE) && !(!r.N && !r.SE && r.NE)
                && !(!r.S && !r.NE && r.SE) && !(!r.E && !r.NW && r.NE) && !(!r.E && !r.SW && r.SE) && !(!r.NE && !r.SW && r.SE)
                && !(!r.NW && !r.SE && r.NE));
        const bool riverS = !riverTile
            || (!waterfallS && !(!r.E && !r.S && r.SE) && !(!r.W && !r.S && r.SW) && !(!r.E && !r.SW && r.SE)
                && !(!r.W && !r.SE && r.SW) && !(!r.S && !r.NE && r.SE) && !(!r.S && !r.NW && r.SW) && !(!r.SE && !r.NW && r.SW)
                && !(!r.NE && !r.SW && r.SE));

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

    void applyTileSlopeSmooth(MapGenContext& ctx)
    {
        const auto mapSize = ctx.settings.mapSize;

        SmoothFunction smoothFunc;

        switch (ctx.settings.slopeSmooth)
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
                    numTilesChanged += smoothFunc(ctx, { x, y });
                }
            }

            if (numTilesChanged == 0)
                break;
        }
    }

    static float gaussian1d(const float delta, const float sigma)
    {
        const float sigmaSqrd = sigma * sigma;
        const float deltaSqrd = delta * delta;
        return (1.0f / std::sqrt(2.0f * std::numbers::pi * sigmaSqrd)) * std::exp(-1.0f * (deltaSqrd / (2.0f * sigmaSqrd)));
    }

    static float gaussian2d(const float deltaX, const float deltaY, const float sigma)
    {
        const float sigmaSqrd = sigma * sigma;
        const float deltaXSqrd = deltaX * deltaX;
        const float deltaYSqrd = deltaY * deltaY;
        return (1.0f / 2.0f * std::numbers::pi * sigmaSqrd)
            * std::exp(-1.0f * ((deltaXSqrd + deltaYSqrd) / (2.0f * sigmaSqrd)));
    }

    class GaussianKernel
    {
    private:
        int32_t kernelSizeHalf;
        HeightMap kernel;

    public:
        explicit GaussianKernel(const float sigma)
        {
            int32_t kernelSize = static_cast<int32_t>(std::ceil(6.0f * sigma));
            if (kernelSize % 2 == 0)
            {
                kernelSize++;
            }
            kernelSize = std::max(kernelSize, 3);
            kernelSizeHalf = kernelSize / 2;

            kernel = { kernelSize, kernelSize };
            for (int32_t y = 0; y < kernelSize; y++)
            {
                for (int32_t x = 0; x < kernelSize; x++)
                {
                    const int32_t dx = x - kernelSizeHalf;
                    const int32_t dy = y - kernelSizeHalf;
                    kernel[{ x, y }] = gaussian2d(dx, dy, sigma);
                }
            }
        }
        float operator()(const int32_t dx, const int32_t dy)
        {
            return kernel[{ dx + kernelSizeHalf, dy + kernelSizeHalf }];
        }
        int32_t getKernelSizeHalf() const
        {
            return kernelSizeHalf;
        }
    };

    void smoothBox(HeightMap& heightMap, const int32_t iterations)
    {
        for (auto i = 0; i < iterations; i++)
        {
            auto copyHeight = heightMap;
            for (auto y = 0; y < heightMap.height; y++)
            {
                for (auto x = 0; x < heightMap.width; x++)
                {
                    const TileCoordsXY pos = { x, y };
                    float sum = 0;
                    for (auto dy = -1; dy <= 1; dy++)
                    {
                        for (auto dx = -1; dx <= 1; dx++)
                        {
                            const TileCoordsXY deltaPos = pos + TileCoordsXY{ dx, dy };
                            // Use tile height if OOB
                            sum += heightMap.inBounds(deltaPos) ? copyHeight[deltaPos] : copyHeight[pos];
                        }
                    }
                    heightMap[pos] = sum / 9.0f;
                }
            }
        }
    }

    void smoothGaussian(HeightMap& heightMap, const float sigma)
    {
        HeightMap copy = heightMap;
        GaussianKernel kernel(sigma);

        for (int32_t y = 0; y < heightMap.height; y++)
        {
            for (int32_t x = 0; x < heightMap.width; x++)
            {
                const TileCoordsXY pos = { x, y };
                float value = 0.0f;
                float sum = 0.0f;
                for (auto dy = -kernel.getKernelSizeHalf(); dy <= kernel.getKernelSizeHalf(); dy++)
                {
                    for (auto dx = -kernel.getKernelSizeHalf(); dx <= kernel.getKernelSizeHalf(); dx++)
                    {
                        const TileCoordsXY deltaPos = pos + TileCoordsXY{ dx, dy };
                        if (heightMap.inBounds(deltaPos))
                        {
                            const float weight = kernel(dx, dy);
                            value += weight * copy[deltaPos];
                            sum += weight;
                        }
                    }
                }
                heightMap[pos] = value / sum;
            }
        }
    }

    /**
     * See
     *
     * Paris, S., Kornprobst, P., Tumblin, J. and Durand, F., 2009. Bilateral filtering: Theory and applications. Foundations
     * and trends in computer graphics and vision, 4(1), pp.1-73.
     */
    void smoothBilateral(HeightMap& heightMap, const float sigmaSpace, const float sigmaIntensity)
    {
        HeightMap copy = heightMap;
        GaussianKernel kernel(sigmaSpace);

        for (int32_t y = 0; y < heightMap.height; y++)
        {
            for (int32_t x = 0; x < heightMap.width; x++)
            {
                const TileCoordsXY pos = { x, y };

                float value = 0.0f;
                float sum = 0.0f;
                for (int32_t dy = -kernel.getKernelSizeHalf(); dy <= kernel.getKernelSizeHalf(); dy++)
                {
                    for (int32_t dx = -kernel.getKernelSizeHalf(); dx <= kernel.getKernelSizeHalf(); dx++)
                    {
                        const TileCoordsXY deltaPos = pos + TileCoordsXY{ dx, dy };
                        if (heightMap.inBounds(deltaPos))
                        {
                            const float closeness = kernel(dx, dy);

                            const float deltaIntensity = copy[pos] - copy[deltaPos];
                            const float similarity = gaussian1d(deltaIntensity, sigmaIntensity);

                            const float weight = closeness * similarity;
                            value += weight * copy[deltaPos];
                            sum += weight;
                        }
                    }
                }
                heightMap[pos] = value / sum;
            }
        }
    }

    /**
     * see https://docs.gimp.org/en_GB/gimp-filter-unsharp-mask.html#id3062
     */
    void sharpen(HeightMap& heightMap, const int32_t iterations)
    {
        for (auto i = 0; i < iterations; i++)
        {
            HeightMap blurCopy = heightMap;
            smoothGaussian(blurCopy, 0.5f);

            for (auto y = 0; y < heightMap.height; y++)
            {
                for (auto x = 0; x < heightMap.width; x++)
                {
                    const TileCoordsXY pos{ x, y };
                    heightMap[pos] = 2.0f * heightMap[pos] - blurCopy[pos];
                }
            }
        }
    }

    void applyHeightMapTransform(MapGenContext& ctx)
    {
        // these multipliers give reasonable results, haven't done an exhaustive search tho
        constexpr float kGaussianSpaceSigmaScalingFactor = 0.5f;
        constexpr float kGaussianIntensitySigmaScalingFactor = 5.5f;

        auto& settings = ctx.settings;
        auto& heightMap = ctx.heightMap;

        switch (settings.heightmapTransform)
        {
            case HeightMapTransform::none:
                break;
            case HeightMapTransform::box:
                smoothBox(heightMap, settings.transformStrength);
                break;
            case HeightMapTransform::gaussian:
                smoothGaussian(heightMap, kGaussianSpaceSigmaScalingFactor * settings.transformStrength);
                break;
            case HeightMapTransform::sharpen:
                sharpen(heightMap, settings.transformStrength);
                break;
            case HeightMapTransform::bilateral:
                smoothBilateral(
                    heightMap, kGaussianSpaceSigmaScalingFactor * settings.transformStrength,
                    kGaussianIntensitySigmaScalingFactor * settings.transformStrength);
                break;
            case HeightMapTransform::erosion:
                simulateErosion(ctx);
                break;
        }
    }

    TileCoordsXY getWorldCoordsOffset(const Settings& settings, const int32_t overscanFactor)
    {
        const int32_t offsetX = (settings.mapSize.x * (overscanFactor - 1)) / 2;
        const int32_t offsetY = (settings.mapSize.y * (overscanFactor - 1)) / 2;
        return TileCoordsXY{ offsetX, offsetY };
    }

    bool isInWorldMap(const MapGenContext& ctx, const TileCoordsXY& genCoords)
    {
        const int32_t xMin = ctx.overscanOffset.x;
        const int32_t xMax = ctx.overscanOffset.x + ctx.settings.mapSize.x;
        const int32_t yMin = ctx.overscanOffset.y;
        const int32_t yMax = ctx.overscanOffset.y + ctx.settings.mapSize.y;

        return xMin <= genCoords.x && genCoords.x < xMax && yMin <= genCoords.y && genCoords.y < yMax;
    }

    TileCoordsXY worldCoordsToGenCoords(const MapGenContext& ctx, const TileCoordsXY& worldCoords)
    {
        return TileCoordsXY{ worldCoords.x + ctx.overscanOffset.x, worldCoords.y + ctx.overscanOffset.y };
    }

    // can be OOB
    TileCoordsXY genCoordsToWorldCoords(const MapGenContext& ctx, const TileCoordsXY& genCoords)
    {
        return TileCoordsXY{ genCoords.x - ctx.overscanOffset.x, genCoords.y - ctx.overscanOffset.y };
    }

    // Ensure height is within [2, 254] and a multiple of 2
    uint8_t quantizeHeight(const float height)
    {
        return static_cast<uint8_t>(std::round(std::clamp(height, 2.0f, 254.0f) * 0.5f) * 2.0f);
    }

} // namespace OpenRCT2::World::MapGenerator
