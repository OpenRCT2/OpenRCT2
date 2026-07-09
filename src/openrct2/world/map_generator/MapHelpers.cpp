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
    static bool isRiverTile(const MapGenContext& ctx, const TileCoordsXY& tileCoords)
    {
        return ctx.riverContext.has_value()
            && ctx.riverContext.value().flags[worldCoordsToGenCoords(ctx, tileCoords)].has(River::river);
    }

    /**
     * Make sure raising the tile edges won't block a river.
     * The first clause covers waterfalls and the other eight situations where one of the two edges connected to this corner
     * is a one tile wide channel.
     * TODO should be possible to express this in a more readable way
     */
    static NeighbourData<bool> wouldNotBlockRiver(
        const bool riverTile, const NeighbourData<bool>& r, const NeighbourData<int32_t>& waterDeltas)
    {
        NeighbourData<bool> result = {};

        const bool waterfallW = (r.W && waterDeltas.W > 0) || (r.NW && waterDeltas.NW > 0) || (r.SW && waterDeltas.SW > 0);
        const bool waterfallN = (r.N && waterDeltas.N > 0) || (r.NW && waterDeltas.NW > 0) || (r.NE && waterDeltas.NE > 0);
        const bool waterfallE = (r.E && waterDeltas.E > 0) || (r.NE && waterDeltas.NE > 0) || (r.SE && waterDeltas.SE > 0);
        const bool waterfallS = (r.S && waterDeltas.S > 0) || (r.SW && waterDeltas.SW > 0) || (r.SE && waterDeltas.SE > 0);

        result.W = !riverTile
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

        result.NE = result.N && result.E;
        result.NW = result.N && result.W;
        result.SE = result.S && result.E;
        result.SW = result.S && result.W;

        return result;
    }

    /**
     * Not perfect, this still leaves some particular tiles unsmoothed.
     * TODO the river compat is more invasive here compared to the weak smooth, check if this can be refactored
     */
    int32_t smoothTileSlopeStrong(const MapGenContext& ctx, const TileCoordsXY tileCoords)
    {
        const auto surfaceElement = MapGetSurfaceElementAt(tileCoords);
        if (surfaceElement == nullptr)
        {
            return 0;
        }

        int8_t proposedHeight = surfaceElement->baseHeight;
        int8_t proposedSlope = kTileSlopeFlat;
        bool riverTile = isRiverTile(ctx, tileCoords);
        int8_t tileWaterLevel = static_cast<int8_t>(surfaceElement->GetWaterHeight() / kCoordsZStep);
        std::optional<MapDirection> doubleCorner = std::nullopt;

        // gather neighbour info
        NeighbourData<uint8_t> neighbourHeights = {};
        NeighbourData<int32_t> neighbourWaterDeltas = {};
        NeighbourData<bool> neighbourIsRiver = {};
        for (const Neighbour& neighbour : kNeighbours)
        {
            const auto neighbourPos = tileCoords + neighbour.offset;
            const auto neighbourSurfaceElement = MapGetSurfaceElementAt(neighbourPos);
            if (neighbourSurfaceElement == nullptr)
            {
                neighbourHeights.direction[neighbour.direction] = 0;
                neighbourWaterDeltas.direction[neighbour.direction] = 0;
                neighbourIsRiver.direction[neighbour.direction] = false;
            }
            else
            {
                neighbourHeights.direction[neighbour.direction] = neighbourSurfaceElement->baseHeight;
                neighbourWaterDeltas.direction[neighbour.direction] = neighbourSurfaceElement->GetWaterHeight()
                    - surfaceElement->GetWaterHeight();
                neighbourIsRiver.direction[neighbour.direction] = isRiverTile(ctx, neighbourPos);
            }
        }

        const auto riverNotBlocked = wouldNotBlockRiver(riverTile, neighbourIsRiver, neighbourWaterDeltas);

        // Raise tile to the height of the highest ordinal neighbour - 2
        uint8_t ordinalNeighbourMaxHeight = proposedHeight;
        for (const Neighbour& neighbour : kNeighboursOrdinal)
        {
            ordinalNeighbourMaxHeight = std::max(ordinalNeighbourMaxHeight, neighbourHeights.direction[neighbour.direction]);
        }
        if (proposedHeight < ordinalNeighbourMaxHeight - 2)
        {
            proposedHeight = ordinalNeighbourMaxHeight - 2;
        }

        // Check cardinal neighbours
        uint8_t cardinalNeighbourMaxHeight = proposedHeight;
        for (const Neighbour& neighbour : kNeighboursCardinal)
        {
            cardinalNeighbourMaxHeight = std::max(cardinalNeighbourMaxHeight, neighbourHeights.direction[neighbour.direction]);
        }

        if (cardinalNeighbourMaxHeight >= proposedHeight + 4)
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

                    if (highestOnOpposingOrdinal > proposedHeight)
                    {
                        proposedHeight = highestOnOpposingOrdinal;
                        canCompensate = false;
                    }
                }
            }

            if (cardinalNeighbourDoubleCornerCount == 1 && canCompensate)
            {
                if (proposedHeight < cardinalNeighbourMaxHeight - 4)
                {
                    proposedHeight = cardinalNeighbourMaxHeight - 4;
                }
                if (neighbourHeights.N == cardinalNeighbourMaxHeight && neighbourHeights.S <= neighbourHeights.N - 4
                    && !neighbourIsRiver.N && !neighbourIsRiver.NE && !neighbourIsRiver.NW)
                    doubleCorner = North;
                else if (
                    neighbourHeights.W == cardinalNeighbourMaxHeight && neighbourHeights.E <= neighbourHeights.W - 4
                    && !neighbourIsRiver.W && !neighbourIsRiver.SW && !neighbourIsRiver.NW)
                    doubleCorner = West;
                else if (
                    neighbourHeights.S == cardinalNeighbourMaxHeight && neighbourHeights.N <= neighbourHeights.S - 4
                    && !neighbourIsRiver.S && !neighbourIsRiver.SE && !neighbourIsRiver.SW)
                    doubleCorner = South;
                else if (
                    neighbourHeights.E == cardinalNeighbourMaxHeight && neighbourHeights.W <= neighbourHeights.E - 4
                    && !neighbourIsRiver.E && !neighbourIsRiver.NE && !neighbourIsRiver.SE)
                    doubleCorner = East;
            }
            else
            {
                if (proposedHeight < cardinalNeighbourMaxHeight - 2)
                {
                    proposedHeight = cardinalNeighbourMaxHeight - 2;
                }
            }
        }

        if (doubleCorner.has_value())
        {
            proposedSlope |= kTileSlopeDiagonalFlag;
            switch (doubleCorner.value())
            {
                case North:
                    proposedSlope |= kTileSlopeNCornerDown;
                    break;
                case West:
                    proposedSlope |= kTileSlopeWCornerDown;
                    break;
                case South:
                    proposedSlope |= kTileSlopeSCornerDown;
                    break;
                case East:
                    proposedSlope |= kTileSlopeECornerDown;
                    break;
                default:
                    break;
            }
        }
        else
        {
            // Cardinals
            if (neighbourHeights.S > proposedHeight && riverNotBlocked.S)
                proposedSlope |= kTileSlopeNCornerUp;

            if (neighbourHeights.E > proposedHeight && riverNotBlocked.E)
                proposedSlope |= kTileSlopeWCornerUp;

            if (neighbourHeights.W > proposedHeight && riverNotBlocked.W)
                proposedSlope |= kTileSlopeECornerUp;

            if (neighbourHeights.N > proposedHeight && riverNotBlocked.N)
                proposedSlope |= kTileSlopeSCornerUp;

            // Ordinals
            if (neighbourHeights.SW > proposedHeight && riverNotBlocked.SW)
                proposedSlope |= kTileSlopeNESideUp;

            if (neighbourHeights.NE > proposedHeight && riverNotBlocked.NE)
                proposedSlope |= kTileSlopeSWSideUp;

            if (neighbourHeights.NW > proposedHeight && riverNotBlocked.NW)
                proposedSlope |= kTileSlopeSESideUp;

            if (neighbourHeights.SE > proposedHeight && riverNotBlocked.SE)
                proposedSlope |= kTileSlopeNWSideUp;

            // If all corners are raised, reset slope and increment height
            if (proposedSlope == kTileSlopeRaisedCornersMask)
            {
                proposedSlope = kTileSlopeFlat;
                proposedHeight += 2;
            }
        }

        if (riverTile && proposedHeight > tileWaterLevel - 2)
        {
            proposedHeight = tileWaterLevel - 2;
        }

        int32_t raisedLand = 0;
        if (proposedHeight > surfaceElement->baseHeight)
        {
            raisedLand = 1;
            surfaceElement->baseHeight = proposedHeight;
            surfaceElement->clearanceHeight = proposedHeight;
        }

        surfaceElement->setSlope(proposedSlope);

        return raisedLand;
    }

    /**
     * Raises the corners based on the height offset of neighbouring tiles.
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

        NeighbourData<int32_t> neighbourHeightDeltas = {};
        NeighbourData<int32_t> neighbourWaterDeltas = {};
        NeighbourData<bool> neighbourIsRiver = {};

        const bool riverTile = isRiverTile(ctx, tileCoords);

        for (const Neighbour& neighbour : kNeighbours)
        {
            auto neighbourCoords = tileCoords + neighbour.offset;
            // Get neighbour height. If the element is not valid (outside of map) assume the same height
            auto* neighbourSurfaceElement = MapGetSurfaceElementAt(neighbourCoords);
            if (neighbourSurfaceElement != nullptr)
            {
                neighbourHeightDeltas.direction[neighbour.direction] = neighbourSurfaceElement->baseHeight;
                neighbourWaterDeltas.direction[neighbour.direction] = neighbourSurfaceElement->GetWaterHeight();
            }
            else
            {
                neighbourHeightDeltas.direction[neighbour.direction] = surfaceElement->baseHeight;
                neighbourWaterDeltas.direction[neighbour.direction] = surfaceElement->GetWaterHeight();
            }

            // Make the height relative to the current surface element
            neighbourHeightDeltas.direction[neighbour.direction] -= surfaceElement->baseHeight;
            neighbourWaterDeltas.direction[neighbour.direction] -= surfaceElement->GetWaterHeight();

            // Check if this is a river tile
            neighbourIsRiver.direction[neighbour.direction] = isRiverTile(ctx, neighbourCoords);
        }

        // Count number from the three tiles that is currently higher
        int8_t thresholdW = std::clamp(neighbourHeightDeltas.SW, 0, 1) + std::clamp(neighbourHeightDeltas.W, 0, 1)
            + std::clamp(neighbourHeightDeltas.NW, 0, 1);
        int8_t thresholdN = std::clamp(neighbourHeightDeltas.NW, 0, 1) + std::clamp(neighbourHeightDeltas.N, 0, 1)
            + std::clamp(neighbourHeightDeltas.NE, 0, 1);
        int8_t thresholdE = std::clamp(neighbourHeightDeltas.NE, 0, 1) + std::clamp(neighbourHeightDeltas.E, 0, 1)
            + std::clamp(neighbourHeightDeltas.SE, 0, 1);
        int8_t thresholdS = std::clamp(neighbourHeightDeltas.SE, 0, 1) + std::clamp(neighbourHeightDeltas.S, 0, 1)
            + std::clamp(neighbourHeightDeltas.SW, 0, 1);

        const auto riverNotBlocked = wouldNotBlockRiver(riverTile, neighbourIsRiver, neighbourWaterDeltas);

        uint8_t slope = kTileSlopeFlat;
        slope |= (thresholdW >= 1 && riverNotBlocked.W) ? kTileSlopeECornerUp : 0;
        slope |= (thresholdN >= 1 && riverNotBlocked.N) ? kTileSlopeSCornerUp : 0;
        slope |= (thresholdE >= 1 && riverNotBlocked.E) ? kTileSlopeWCornerUp : 0;
        slope |= (thresholdS >= 1 && riverNotBlocked.S) ? kTileSlopeNCornerUp : 0;

        // Set diagonal when three corners (one corner down) have been raised, and the middle one can be raised one more
        if ((slope == kTileSlopeWCornerDown && neighbourHeightDeltas.W >= 4)
            || (slope == kTileSlopeSCornerDown && neighbourHeightDeltas.S >= 4)
            || (slope == kTileSlopeECornerDown && neighbourHeightDeltas.E >= 4)
            || (slope == kTileSlopeNCornerDown && neighbourHeightDeltas.N >= 4))
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

    void applyHeightMapFilter(MapGenContext& ctx)
    {
        // these multipliers give reasonable results, haven't done an exhaustive search tho
        constexpr float kGaussianSpaceSigmaScalingFactor = 0.5f;
        constexpr float kGaussianIntensitySigmaScalingFactor = 5.5f;

        auto& settings = ctx.settings;
        auto& heightMap = ctx.heightMap;

        switch (settings.filter.type)
        {
            case Filter::none:
                break;
            case Filter::box:
                smoothBox(heightMap, settings.filter.strength);
                break;
            case Filter::gaussian:
                smoothGaussian(heightMap, kGaussianSpaceSigmaScalingFactor * settings.filter.strength);
                break;
            case Filter::sharpen:
                sharpen(heightMap, settings.filter.strength);
                break;
            case Filter::bilateral:
                smoothBilateral(
                    heightMap, kGaussianSpaceSigmaScalingFactor * settings.filter.strength,
                    kGaussianIntensitySigmaScalingFactor * settings.filter.strength);
                break;
            case Filter::erosion:
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
        const int32_t xMin = ctx.overscanOffset.x + 1;
        const int32_t xMax = ctx.overscanOffset.x + ctx.settings.mapSize.x - 1;
        const int32_t yMin = ctx.overscanOffset.y + 1;
        const int32_t yMax = ctx.overscanOffset.y + ctx.settings.mapSize.y - 1;

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

    void completeDistanceMap(DistanceMap& distanceMap, TrackingStableTileQueue& queue)
    {
        while (!queue.empty())
        {
            QueueTile tile = queue.top();
            queue.pop();

            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };

                const float distance = tile.value
                    + sqrt(neighbour.offset.x * neighbour.offset.x + neighbour.offset.y * neighbour.offset.y);

                if (!distanceMap.inBounds(nPos) || queue.isMarked(nPos) || distance >= distanceMap[nPos])
                {
                    continue;
                }

                distanceMap[nPos] = distance;
                queue.emplaceAndMark(nPos, distance);
            }
        }
    }

    void initZeroDistance(const TileCoordsXY& pos, DistanceMap& distanceMap, TrackingStableTileQueue& queue)
    {
        distanceMap[pos] = 0.0f;
        queue.emplaceAndMark(pos, 0.0f);
    }

    void computeRiverFlagBasedDistanceMap(
        const MapGenContext& ctx, DistanceMap& distanceMap, const River::RiverFlag flag, bool invert)
    {
        distanceMap = DistanceMap{ ctx.dimensions };
        distanceMap.fill(std::numeric_limits<float>::infinity());

        if (!ctx.riverContext.has_value())
        {
            return;
        }

        const auto& riverCtx = ctx.riverContext.value();
        TrackingStableTileQueue queue{ ctx.dimensions };

        for (int32_t y = 0; y < distanceMap.height; y++)
        {
            for (int32_t x = 0; x < distanceMap.width; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (riverCtx.flags[pos].has(flag) == !invert)
                {
                    initZeroDistance(pos, distanceMap, queue);
                }
            }
        }

        completeDistanceMap(distanceMap, queue);
    }

} // namespace OpenRCT2::World::MapGenerator
