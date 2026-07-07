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
    static uint8_t GetBaseHeightOrZero(const TileCoordsXY& pos)
    {
        const auto surfaceElement = MapGetSurfaceElementAt(pos);
        return surfaceElement != nullptr ? surfaceElement->baseHeight : 0;
    }

    static bool isRiverTile(const MapGenContext& ctx, const TileCoordsXY& tileCoords)
    {
        return ctx.riverContext.has_value()
            && ctx.riverContext.value().flags[worldCoordsToGenCoords(ctx, tileCoords)].has(River::river);
    }

    /**
     * Not perfect, this still leaves some particular tiles unsmoothed.
     */
    int32_t smoothTileSlopeStrong(const MapGenContext& ctx, const TileCoordsXY tileCoords)
    {
        const auto surfaceElement = MapGetSurfaceElementAt(tileCoords);
        if (surfaceElement == nullptr)
        {
            return 0;
        }
        auto raisedLand = 0;
        surfaceElement->setSlope(kTileSlopeFlat);

        // populate neighbour heights
        NeighbourData<uint8_t> neighbourHeights={};
        for (const Neighbour& neighbour : kNeighbours)
        {
            neighbourHeights.direction[neighbour.direction] = GetBaseHeightOrZero(tileCoords + neighbour.offset);
        }

        // Raise tile to the height of the highest ordinal neighbour - 2
        uint8_t highestNeighbourOrdinal = surfaceElement->baseHeight;
        for (const Neighbour& neighbour : kNeighboursOrdinal)
        {
            highestNeighbourOrdinal = std::max(highestNeighbourOrdinal, neighbourHeights.direction[neighbour.direction]);
        }

        if (surfaceElement->baseHeight < highestNeighbourOrdinal - 2)
        {
            raisedLand = 1;
            surfaceElement->baseHeight = highestNeighbourOrdinal - 2;
            surfaceElement->clearanceHeight = highestNeighbourOrdinal - 2;
        }

        // Check cardinal neighbours
        std::optional<MapDirection> doubleCorner = std::nullopt;

        uint8_t cardinalNeighbourMaxHeight = surfaceElement->baseHeight;
        for (const Neighbour& neighbour : kNeighboursCardinal)
        {
            cardinalNeighbourMaxHeight = std::max(cardinalNeighbourMaxHeight, neighbourHeights.direction[neighbour.direction]);
        }

        if (cardinalNeighbourMaxHeight >= surfaceElement->baseHeight + 4)
        {
            int32_t cardinalNeighbourDoubleCornerCount = 0;
            bool canCompensate = true;
            for (const Neighbour& neighbour : kNeighboursCardinal)
            {
                if (neighbourHeights.direction[neighbour.direction] == cardinalNeighbourMaxHeight)
                {
                    cardinalNeighbourDoubleCornerCount++;

                    // Check if surrounding corners aren't too high. The current tile
                    // can't compensate for all the height differences anymore if it has
                    // the extra height slope.
                    int32_t highestOnOpposingOrdinal;
                    switch (neighbour.direction)
                    {
                        default:
                        case North:
                            highestOnOpposingOrdinal = std::max(neighbourHeights.SE, neighbourHeights.SW);
                            break;
                        case West:
                            highestOnOpposingOrdinal = std::max(neighbourHeights.NE, neighbourHeights.SE);
                            break;
                        case South:
                            highestOnOpposingOrdinal = std::max(neighbourHeights.NE, neighbourHeights.NW);
                            break;
                        case East:
                            highestOnOpposingOrdinal = std::max(neighbourHeights.NW, neighbourHeights.SW);
                            break;
                    }

                    if (highestOnOpposingOrdinal > surfaceElement->baseHeight)
                    {
                        surfaceElement->baseHeight = highestOnOpposingOrdinal;
                        surfaceElement->clearanceHeight = highestOnOpposingOrdinal;
                        raisedLand = 1;
                        canCompensate = false;
                    }
                }
            }

            if (cardinalNeighbourDoubleCornerCount == 1 && canCompensate)
            {
                if (surfaceElement->baseHeight < cardinalNeighbourMaxHeight - 4)
                {
                    surfaceElement->baseHeight = surfaceElement->clearanceHeight = cardinalNeighbourMaxHeight - 4;
                    raisedLand = 1;
                }
                if (neighbourHeights.N == cardinalNeighbourMaxHeight && neighbourHeights.S <= neighbourHeights.N - 4)
                    doubleCorner = North;
                else if (neighbourHeights.W == cardinalNeighbourMaxHeight && neighbourHeights.E <= neighbourHeights.W - 4)
                    doubleCorner = West;
                else if (neighbourHeights.S == cardinalNeighbourMaxHeight && neighbourHeights.N <= neighbourHeights.S - 4)
                    doubleCorner = South;
                else if (neighbourHeights.E == cardinalNeighbourMaxHeight && neighbourHeights.W <= neighbourHeights.E - 4)
                    doubleCorner = East;
            }
            else
            {
                if (surfaceElement->baseHeight < cardinalNeighbourMaxHeight - 2)
                {
                    surfaceElement->baseHeight = surfaceElement->clearanceHeight = cardinalNeighbourMaxHeight - 2;
                    raisedLand = 1;
                }
            }
        }

        if (doubleCorner.has_value())
        {
            uint8_t slope = surfaceElement->getSlope() | kTileSlopeDiagonalFlag;
            switch (doubleCorner.value())
            {
                case North:
                    slope |= kTileSlopeNCornerDown;
                    break;
                case West:
                    slope |= kTileSlopeWCornerDown;
                    break;
                case South:
                    slope |= kTileSlopeSCornerDown;
                    break;
                case East:
                    slope |= kTileSlopeECornerDown;
                    break;
                default:
                    break;
            }
            surfaceElement->setSlope(slope);
        }
        else
        {
            uint8_t slope = surfaceElement->getSlope();
            // Cardinals
            if (neighbourHeights.S > surfaceElement->baseHeight)
                slope |= kTileSlopeNCornerUp;

            if (neighbourHeights.E > surfaceElement->baseHeight)
                slope |= kTileSlopeWCornerUp;

            if (neighbourHeights.W > surfaceElement->baseHeight)
                slope |= kTileSlopeECornerUp;

            if (neighbourHeights.N > surfaceElement->baseHeight)
                slope |= kTileSlopeSCornerUp;

            // Ordinals
            if (neighbourHeights.SW > surfaceElement->baseHeight)
                slope |= kTileSlopeNESideUp;

            if (neighbourHeights.NE > surfaceElement->baseHeight)
                slope |= kTileSlopeSWSideUp;

            if (neighbourHeights.NW > surfaceElement->baseHeight)
                slope |= kTileSlopeSESideUp;

            if (neighbourHeights.SE > surfaceElement->baseHeight)
                slope |= kTileSlopeNWSideUp;

            // If all corners are raised, set flat slope and increment height
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
     * Make sure raising the tile edges won't block a river.
     * The first clause covers waterfalls and the other eight situations where one of the two edges connected to this corner
     * is a one tile wide channel.
     * TODO should be possible to express this in a more readable way
     */
    static NeighbourData<bool> wontBlockRiver(
        const bool riverTile, const NeighbourData<bool>& r, const NeighbourData<int32_t>& waterDelta)
    {
        NeighbourData<bool> result = {};

        const bool waterfallW = (r.W && waterDelta.W > 0) || (r.NW && waterDelta.NW > 0) || (r.SW && waterDelta.SW > 0);
        const bool waterfallN = (r.N && waterDelta.N > 0) || (r.NW && waterDelta.NW > 0) || (r.NE && waterDelta.NE > 0);
        const bool waterfallE = (r.E && waterDelta.E > 0) || (r.NE && waterDelta.NE > 0) || (r.SE && waterDelta.SE > 0);
        const bool waterfallS = (r.S && waterDelta.S > 0) || (r.SW && waterDelta.SW > 0) || (r.SE && waterDelta.SE > 0);

        result.W  = !riverTile
            || (!waterfallW && !(!r.S && !r.W && r.SW) && !(!r.N && !r.W && r.NW) && !(!r.S && !r.NW && r.SW)
                && !(!r.N && !r.SW && r.NW) && !(!r.W && !r.SE && r.SW) && !(!r.W && !r.NE && r.NW) && !(!r.SW && !r.NE && r.NW)
                && !(!r.SE && !r.NW && r.SW));
        result.N = !riverTile
            || (!waterfallN && !(!r.W && !r.N && r.NW) && !(!r.E && !r.N && r.NE) && !(!r.W && !r.NE && r.NW)
                && !(!r.E && !r.NW && r.NE) && !(!r.N && !r.SW && r.NW) && !(!r.N && !r.SE && r.NE) && !(!r.NW && !r.SE && r.NE)
                && !(!r.SW && !r.NE && r.NW));
       result.E = !riverTile
            || (!waterfallE && !(!r.N && !r.E && r.NE) && !(!r.S && !r.E && r.SE) && !(!r.N && !r.SE && r.NE)
                && !(!r.S && !r.NE && r.SE) && !(!r.E && !r.NW && r.NE) && !(!r.E && !r.SW && r.SE) && !(!r.NE && !r.SW && r.SE)
                && !(!r.NW && !r.SE && r.NE));
        result.S = !riverTile
            || (!waterfallS && !(!r.E && !r.S && r.SE) && !(!r.W && !r.S && r.SW) && !(!r.E && !r.SW && r.SE)
                && !(!r.W && !r.SE && r.SW) && !(!r.S && !r.NE && r.SE) && !(!r.S && !r.NW && r.SW) && !(!r.SE && !r.NW && r.SW)
                && !(!r.NE && !r.SW && r.SE));

        return result;
    }

    /**
     * Raises the corners based on the height offset of neighbour tiles.
     * This does not change the base height, unless all corners have been raised.
     * @returns 0 if no edits were made, 1 otherwise
     */
    int32_t smoothTileSlopeWeak(const MapGenContext& ctx, const TileCoordsXY tileCoords)
    {
        auto* const surfaceElement = MapGetSurfaceElementAt(tileCoords);
        if (surfaceElement == nullptr)
        {
            return 0;
        }

        NeighbourData<int32_t> neighbourHeightOffset = {};
        NeighbourData<int32_t> neighbourWaterOffset = {};
        NeighbourData<bool> neighbourIsRiver = {};

        const bool riverTile = isRiverTile(ctx, tileCoords);

        for (const Neighbour& neighbour : kNeighbours)
        {
            auto neighbourCoords = tileCoords + neighbour.offset;
            // Get neighbour height. If the element is not valid (outside of map) assume the same height
            auto* neighbourSurfaceElement = MapGetSurfaceElementAt(neighbourCoords);
            if (neighbourSurfaceElement != nullptr)
            {
                neighbourHeightOffset.direction[neighbour.direction] = neighbourSurfaceElement->baseHeight;
                neighbourWaterOffset.direction[neighbour.direction] = neighbourSurfaceElement->GetWaterHeight();
            }
            else
            {
                neighbourHeightOffset.direction[neighbour.direction] = surfaceElement->baseHeight;
                neighbourWaterOffset.direction[neighbour.direction] = surfaceElement->GetWaterHeight();
            }

            // Make the height relative to the current surface element
            neighbourHeightOffset.direction[neighbour.direction] -= surfaceElement->baseHeight;
            neighbourWaterOffset.direction[neighbour.direction] -= surfaceElement->GetWaterHeight();

            // Check if this is a river tile
            neighbourIsRiver.direction[neighbour.direction] = isRiverTile(ctx, neighbourCoords);
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

        const auto riverNotBlocked = wontBlockRiver(riverTile, neighbourIsRiver, neighbourWaterOffset);

        uint8_t slope = kTileSlopeFlat;
        slope |= (thresholdW >= 1 && riverNotBlocked.W) ? SLOPE_W_THRESHOLD_FLAGS : 0;
        slope |= (thresholdN >= 1 && riverNotBlocked.N) ? SLOPE_N_THRESHOLD_FLAGS : 0;
        slope |= (thresholdE >= 1 && riverNotBlocked.E) ? SLOPE_E_THRESHOLD_FLAGS : 0;
        slope |= (thresholdS >= 1 && riverNotBlocked.S) ? SLOPE_S_THRESHOLD_FLAGS : 0;

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
