/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "HydroUtils.h"

#include "../../../profiling/Profiling.h"
#include "../MapGen.h"
#include "../MapHelpers.h"
#include "../MapTraversalUtils.h"
#include "HydroTypes.hpp"

namespace OpenRCT2::World::MapGenerator::Hydro
{
    void primeHydroFlagHeightQueue(
         MapGenContext& ctx, TrackingStableHeightTileQueue& queue, HydroFlag flag, const QueueInitPosCallback& callback)
    {
        HydroContext& hydroCtx = ctx.hydroContext.value();

        for (int32_t y = 0; y < ctx.dimensions.y - 1; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (hydroCtx.flags[left].has(flag))
            {
                queue.emplaceAndMark(left, ctx.heightMap[left]);
                if (callback.has_value())
                {
                    callback.value()(left);
                }
            }

            const TileCoordsXY right{ ctx.dimensions.x - 1, y };
            if (hydroCtx.flags[right].has(flag))
            {
                queue.emplaceAndMark(right, ctx.heightMap[right]);
                if (callback.has_value())
                {
                    callback.value()(right);
                }
            }
        }

        for (int32_t x = 1; x < ctx.dimensions.x - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (hydroCtx.flags[top].has(flag))
            {
                queue.emplaceAndMark(top, ctx.heightMap[top]);
                if (callback.has_value())
                {
                    callback.value()(top);
                }
            }

            const TileCoordsXY bottom{ x, ctx.dimensions.y - 1 };
            if (hydroCtx.flags[bottom].has(flag))
            {
                queue.emplaceAndMark(bottom, ctx.heightMap[bottom]);
                if (callback.has_value())
                {
                    callback.value()(bottom);
                }
            }
        }
    }
    void primeHydroFlagDistanceQueue(
        MapGenContext& ctx, TrackingStableTileDistanceTileQueue& queue, HydroFlag flag, const QueueInitPosCallback& callback)
    {
        HydroContext& hydroCtx = ctx.hydroContext.value();

        for (int32_t y = 0; y < ctx.dimensions.y - 1; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (hydroCtx.flags[left].has(flag))
            {
                queue.emplaceAndMark(left, 0);
                if (callback.has_value())
                {
                    callback.value()(left);
                }
            }

            const TileCoordsXY right{ ctx.dimensions.x - 1, y };
            if (hydroCtx.flags[right].has(flag))
            {
                queue.emplaceAndMark(right, 0);
                if (callback.has_value())
                {
                    callback.value()(right);
                }
            }
        }

        for (int32_t x = 1; x < ctx.dimensions.x - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (hydroCtx.flags[top].has(flag))
            {
                queue.emplaceAndMark(top, 0);
                if (callback.has_value())
                {
                    callback.value()(top);
                }
            }

            const TileCoordsXY bottom{ x, ctx.dimensions.y - 1 };
            if (hydroCtx.flags[bottom].has(flag))
            {
                queue.emplaceAndMark(bottom, 0);
                if (callback.has_value())
                {
                    callback.value()(bottom);
                }
            }
        }
    }

    /**
     * Reduce the river mask to a skeleton by running a line thinning algorithm.
     *
     * based on
     *
     * Chen, Y.S. and Hsu, W.H., 1988. A modified fast parallel algorithm for thinning digital patterns. Pattern Recognition
     * Letters, 7(2), pp.99-106.
     */
    void constructRiverSkeletonMap(MapGenContext& ctx, BooleanMap& skeletonMap)
    {
        PROFILED_FUNCTION();
        HydroContext& hydroCtx = ctx.hydroContext.value();
        bool firstSubiteration = true;
        std::vector<TileCoordsXY> toClear;

        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                skeletonMap[pos] = hydroCtx.flags[pos].has(river);
            }
        }

        while (true)
        {
            // the algorithm doesn't handle edge coords, resulting in 'T' shapes being left behind, should be ok for this.
            for (int32_t y = 1; y < ctx.dimensions.y - 1; y++)
            {
                for (int32_t x = 1; x < ctx.dimensions.x - 1; x++)
                {
                    TileCoordsXY p1{ x, y };

                    if (!skeletonMap[p1])
                    {
                        continue;
                    }

                    union
                    {
                        bool pN[8];
                        struct
                        {
                            bool p2, p3, p4, p5, p6, p7, p8, p9;
                        };
                        // coordinates differ from the paper to account for inverted x-axis of rct
                    } nb = { .p2 = skeletonMap[p1 + TileCoordsXY{ 0, -1 }],
                             .p3 = skeletonMap[p1 + TileCoordsXY{ 1, -1 }],
                             .p4 = skeletonMap[p1 + TileCoordsXY{ 1, 0 }],
                             .p5 = skeletonMap[p1 + TileCoordsXY{ 1, 1 }],
                             .p6 = skeletonMap[p1 + TileCoordsXY{ 0, 1 }],
                             .p7 = skeletonMap[p1 + TileCoordsXY{ -1, 1 }],
                             .p8 = skeletonMap[p1 + TileCoordsXY{ -1, 0 }],
                             .p9 = skeletonMap[p1 + TileCoordsXY{ -1, -1 }] };

                    int32_t bOfP1 = 0; // count neighbours
                    for (const auto& n : nb.pN)
                    {
                        if (n)
                        {
                            bOfP1 += 1;
                        }
                    }

                    if (!(2 <= bOfP1 && bOfP1 <= 7))
                    {
                        continue; // condition a
                    }

                    int32_t aOfP1 = 0; // count clockwise 0->1 transitions
                    for (size_t i = 0; i < 8; ++i)
                    {
                        if (!nb.pN[i] && nb.pN[(i + 1) % 8])
                        {
                            aOfP1 += 1;
                        }
                    }

                    if (aOfP1 != 1 && aOfP1 != 2)
                    {
                        continue; // condition b
                    }

                    if (firstSubiteration)
                    {
                        if (aOfP1 == 1)
                        {
                            if (!((!nb.p2 || !nb.p4 || !nb.p6) && (!nb.p4 || !nb.p6 || !nb.p8)))
                            {
                                continue; // condition {c, d}
                            }
                        }
                        else
                        {
                            if (!(((nb.p2 && nb.p4) && (!nb.p6 && !nb.p7 && !nb.p8))
                                  || ((nb.p4 && nb.p6) && (!nb.p2 && !nb.p8 && !nb.p9))))
                            {
                                continue; // condition {f, g}
                            }
                        }
                    }
                    else
                    {
                        if (aOfP1 == 1)
                        {
                            if (!((!nb.p2 || !nb.p4 || !nb.p8) && (!nb.p2 || !nb.p6 || !nb.p8)))
                            {
                                continue; // condition {c', d'}
                            }
                        }
                        else
                        {
                            if (!(((nb.p2 && nb.p8) && (!nb.p4 && !nb.p5 && !nb.p6))
                                  || ((nb.p6 && nb.p8) && (!nb.p2 && !nb.p3 && !nb.p4))))
                            {
                                continue; // condition {f', g'}
                            }
                        }
                    }

                    toClear.push_back(p1);
                }
            }

            if (toClear.empty())
            {
                break;
            }

            for (const auto& pos : toClear)
            {
                skeletonMap[pos] = false;
            }

            toClear.clear();
            firstSubiteration = !firstSubiteration; // alternate between subiterations
        }
    }

    // TODO can be optimized by keeping the visited set out of the loops in the caller
    static Backref findLateralSetIdentityPos(
        const MapGenContext& ctx, const TileCoordsXY& pos, const MapDirectionMaskMap& directionalRefsMap)
    {
        const HydroContext& hydroCtx = ctx.hydroContext.value();

        TileCoordsXY maxHashPos = pos;
        size_t maxHash = TileCoordsXYHash{}(pos);

        TileCoordsXYSet visited;
        std::queue<TileCoordsXY> queue;
        queue.emplace(pos);
        visited.insert(pos);

        while (!queue.empty())
        {
            const TileCoordsXY currentPos{ queue.front() };
            queue.pop();

            if (!directionalRefsMap[currentPos].isEmpty())
            {
                return std::nullopt;
            }

            const size_t currentHash = TileCoordsXYHash{}(currentPos);
            if (currentHash > maxHash)
            {
                maxHashPos = currentPos;
                maxHash = currentHash;
            }

            for (const Neighbour& neighbour : kNeighbours)
            {
                const TileCoordsXY& lateralPos = currentPos + neighbour.offset;
                if (hydroCtx.flowsLateral[currentPos].has(neighbour.direction) && !visited.contains(lateralPos))
                {
                    queue.emplace(lateralPos);
                    visited.insert(lateralPos);
                }
            }
        }

        return maxHashPos;
    }

    void findSourcesAndSinks(MapGenContext& ctx, TileCoordsXYSet& sources, TileCoordsXYSet& sinks)
    {
        PROFILED_FUNCTION();

        HydroContext& hydroCtx = ctx.hydroContext.value();

        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!hydroCtx.flags[pos].has(river))
                {
                    continue;
                }

                auto downstreamCount = 0;
                auto upstreamCount = 0;
                auto lateralCount = 0;

                for (const Neighbour& neighbour : kNeighbours)
                {
                    const TileCoordsXY nPos = pos + neighbour.offset;

                    if (!hydroCtx.flags.inBounds(nPos) || !hydroCtx.flags[nPos].has(river))
                    {
                        continue;
                    }

                    if (hydroCtx.flowsOut[pos].has(neighbour.direction))
                    {
                        downstreamCount++;
                    }
                    if (hydroCtx.flowsIn[pos].has(neighbour.direction))
                    {
                        upstreamCount++;
                    }
                    if (hydroCtx.flowsLateral[pos].has(neighbour.direction))
                    {
                        lateralCount++;
                    }
                }

                if (upstreamCount == 0)
                {
                    if (lateralCount == 0)
                    {
                        sources.insert(pos);
                    }
                    else
                    {
                        Backref maybeLateralSetSource = findLateralSetIdentityPos(ctx, pos, hydroCtx.flowsIn);
                        if (maybeLateralSetSource.has_value())
                        {
                            sources.insert(maybeLateralSetSource.value());
                        }
                    }
                }
                else if (downstreamCount == 0 && !hydroCtx.flags.onEdge(pos))
                {
                    if (lateralCount == 0)
                    {
                        sinks.insert(pos);
                    }
                    else
                    {
                        Backref maybeLateralSetSource = findLateralSetIdentityPos(ctx, pos, hydroCtx.flowsOut);
                        if (maybeLateralSetSource.has_value())
                        {
                            sources.insert(maybeLateralSetSource.value());
                        }
                    }
                }
            }
        }
    }

    /**
     * Returns the two ordinal neighbours for the given cardinal offset.
     * Uses the game coordinate convention, i.e. the diagonal neighbours are cardinal directions.
     */
    std::array<TileCoordsXY, 2> ordinalNeighbours(const TileCoordsXY& offset)
    {
        return {
            TileCoordsXY{ 0, offset.y },
            TileCoordsXY{ offset.x, 0 },
        };
    }

    /**
     * Checks if the tile at the offset from the given position share an ordinal neighbour with the given flag.
     * Uses the game coordinate convention, i.e. the diagonal neighbours are cardinal directions.
     */
    bool haveCommonOrdinalNeighbour(const HydroContext& hydroCtx, const TileCoordsXY& pos, const TileCoordsXY& offset)
    {
        for (const TileCoordsXY& ordinalOffset : ordinalNeighbours(offset))
        {
            const TileCoordsXY sharedOrdinalPos{ pos + ordinalOffset };
            if (hydroCtx.flags[sharedOrdinalPos].has(river))
            {
                return true;
            }
        }
        return false;
    }

    std::string summarizeHydroStatistics(const MapGenContext& ctx)
    {
        const auto& stats = ctx.hydroContext.value().stats;

        const auto pitSummary = std::format(
            "\n[breach or fill]\n"
            "    pits {}\n"
            "    breach successes {}\n"
            "    tiles breached {}\n"
            "    tiles filled {}\n",
            stats.pitsFound, stats.pitsBreachSuccess, stats.pitsBreachedTiles, stats.pitsFilledTiles);

        const auto flowSummary = std::format(
            "\n[flow aggregation]\n"
            "    catchment max {}\n",
            stats.flowAggMax);

        const auto pruningSummary = std::format(
            "\n[pruning]\n"
            "    sinks removed {}\n"
            "    sources removed {} of {}\n"
            "    iterations {}\n"
            "    deadlocks resolved {}\n",
            stats.pruneSinksFound, stats.pruneSourcesRemoved, stats.pruneSourcesFound, stats.pruneIterations,
            stats.pruneDeadlocks);

        const auto widthAdjust = std::format(
            "\n[width adjustment]\n"
            "    river tiles added {}\n",
            stats.widthAdjustNewTiles);

        const auto ensureCardinal = std::format(
            "\n[ensure cardinal]\n"
            "    river tiles added {}\n",
            stats.ensureCardinalNewTiles);

        const auto skeletonSummary = std::format(
            "\n[skeleton mask]\n"
            "    river tiles removed {}\n",
            stats.skeletonRemovedTiles);

        const auto bankIndentationsSummary = std::format(
            "\n[bank indentations]\n"
            "    removed {}\n",
            stats.bankIndentationsRemoved);

        const auto consistencySummary = std::format(
            "\n[consistency]\n"
            "    segments raised {} (max size {})\n"
            "    segments lowered {} (max size {})\n"
            "    segments deleted {} (max size {})\n"
            "    segments iterations {}\n"
            "    banks raised {}\n",
            stats.consistencySegmentsRaised, stats.consistencySegmentsRaisedMaxSize, stats.consistencySegmentsLowered,
            stats.consistencySegmentsLoweredMaxSize, stats.consistencySegmentsRemoved, stats.consistencySegmentsRemovedMaxSize,
            stats.consistencySegmentsIterations, stats.consistencyBanksRaised);

        return pitSummary + flowSummary + pruningSummary + widthAdjust + ensureCardinal + skeletonSummary
            + bankIndentationsSummary + consistencySummary;
    }

} // namespace OpenRCT2::World::MapGenerator::Hydro
