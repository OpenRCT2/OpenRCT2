/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RiverUtils.h"

#include "../../../Diagnostic.h"
#include "../../../profiling/Profiling.h"
#include "../MapGen.h"
#include "../MapHelpers.h"
#include "../MapTraversalUtils.h"
#include "HydroTypes.hpp"

namespace OpenRCT2::World::MapGenerator::Hydro
{
    static float getQueueValue(MapGenCtx& context, const TileCoordsXY& pos, QueueMode queueMode)
    {
        switch (queueMode)
        {
            case QueueMode::height:
                return context.heightMap[pos];
            case QueueMode::distance:
                return 0.0f;
        }
        throw std::runtime_error("unsupported queue mode");
    }

    void primeHydroFlagQueue(MapGenCtx& context, TrackingStableTileQueue& queue, const QueueCfg& cfg)
    {
        PROFILED_FUNCTION();
        HydroMaps& hydroMaps = context.hydroMaps.value();

        for (int32_t y = 0; y < context.dimensions.y - 1; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (hydroMaps.flags[left].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(left, getQueueValue(context, left, cfg.mode));
            }

            const TileCoordsXY right{ context.dimensions.x - 1, y };
            if (hydroMaps.flags[right].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(right, getQueueValue(context, right, cfg.mode));
            }
        }

        for (int32_t x = 1; x < context.dimensions.x - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (hydroMaps.flags[top].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(top, getQueueValue(context, top, cfg.mode));
            }

            const TileCoordsXY bottom{ x, context.dimensions.y - 1 };
            if (hydroMaps.flags[bottom].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(bottom, getQueueValue(context, bottom, cfg.mode));
            }
        }
    }

    // TODO can be optimized by keeping the visited set out of the loops in the caller
    static Backref findLateralSetIdentityPos(
        const TileCoordsXY& pos, const BackrefsMap& lateralRefsMap, const BackrefsMap& directionalRefsMap)
    {
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

            if (!directionalRefsMap[currentPos].empty())
            {
                return std::nullopt;
            }

            const size_t currentHash = TileCoordsXYHash{}(currentPos);
            if (currentHash > maxHash)
            {
                maxHashPos = currentPos;
                maxHash = currentHash;
            }

            for (const TileCoordsXY& lateralPos : lateralRefsMap[currentPos])
            {
                if (!visited.contains(lateralPos))
                {
                    queue.emplace(lateralPos);
                    visited.insert(lateralPos);
                }
            }
        }

        return maxHashPos;
    }

    void prepareSourcesAndSinksForPruning(MapGenCtx& context, TileCoordsXYSet& sources, TileCoordsXYSet& sinks)
    {
        PROFILED_FUNCTION();

        HydroMaps& hydroMaps = context.hydroMaps.value();
        BackrefsMap downstreamRefsMap{ context.dimensions };
        BackrefsMap upstreamRefsMap{ context.dimensions };
        BackrefsMap lateralRefsMap{ context.dimensions };

        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            for (int32_t x = 0; x < context.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!hydroMaps.flags[pos].has(river))
                {
                    continue;
                }

                for (const auto& neighbour : kNeighbours)
                {
                    const TileCoordsXY nPos{ pos + neighbour.offset };

                    if (!hydroMaps.flags.inBounds(nPos) || !hydroMaps.flags[nPos].has(river))
                    {
                        continue;
                    }

                    if (context.heightMap[pos] < context.heightMap[nPos]) // nPos is upstream of pos
                    {
                        downstreamRefsMap[nPos].insert(pos);
                        upstreamRefsMap[pos].insert(nPos);
                    }
                    else if (context.heightMap[pos] > context.heightMap[nPos]) // nPos is downstream of pos
                    {
                        downstreamRefsMap[pos].insert(nPos);
                        upstreamRefsMap[nPos].insert(pos);
                    }
                    else // nPos and pos are at the same height
                    {
                        lateralRefsMap[pos].insert(nPos);
                        lateralRefsMap[nPos].insert(pos);
                    }
                }
            }
        }

        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            for (int32_t x = 0; x < context.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!hydroMaps.flags[pos].has(river))
                {
                    continue;
                }

                const auto downstreamCount = downstreamRefsMap[pos].size();
                const auto upstreamCount = upstreamRefsMap[pos].size();
                const auto lateralCount = lateralRefsMap[pos].size();

                // TODO check for orphans here instead/again?

                if (upstreamCount == 0)
                {
                    if (lateralCount == 0)
                    {
                        sources.insert(pos);
                    }
                    else
                    {
                        Backref maybeLateralSetSource = findLateralSetIdentityPos(pos, lateralRefsMap, upstreamRefsMap);
                        if (maybeLateralSetSource.has_value())
                        {
                            sources.insert(maybeLateralSetSource.value());
                        }
                    }
                }
                else if (downstreamCount == 0 && !downstreamRefsMap.onEdge(pos))
                {
                    if (lateralCount == 0)
                    {
                        sinks.insert(pos);
                    }
                    else
                    {
                        Backref maybeLateralSetSource = findLateralSetIdentityPos(pos, lateralRefsMap, downstreamRefsMap);
                        if (maybeLateralSetSource.has_value())
                        {
                            sources.insert(maybeLateralSetSource.value());
                        }
                    }
                }
            }
        }

        BackrefMap skelBackref{ context.dimensions };
        computeHydroFlagBasedBackrefMap(context, skelBackref, skeleton);

        for (const auto& source : sources)
        {
            hydroMaps.flags[source].set(skeleton);
            int32_t pathLength = 0;
            Backref current = skelBackref[source];
            while (current.has_value() && !hydroMaps.flags[current.value()].has(skeleton))
            {
                hydroMaps.flags[current.value()].set(river, skeleton);
                pathLength++;
                current = skelBackref[current.value()];
            }
            LOG_INFO("connected source at (%d, %d) to skeleton, distance %d", source.x, source.y, pathLength);
        }

        for (const auto& sink : sinks)
        {
            hydroMaps.flags[sink].set(skeleton);
            int32_t pathLength = 0;
            Backref current = skelBackref[sink];
            while (current.has_value() && !hydroMaps.flags[current.value()].has(skeleton))
            {
                hydroMaps.flags[current.value()].set(river, skeleton);
                pathLength++;
                current = skelBackref[current.value()];
            }
            LOG_INFO("connected sink at (%d, %d) to skeleton, distance %d", sink.x, sink.y, pathLength);
        }
    }
    void prepareConfluencesAndBifurcationsForPruning(
        MapGenCtx& context, BackrefMap& mainUpstreamMap, BackrefMap& mainDownstreamMap, BackrefsMap& auxUpstreamsMap,
        BackrefsMap& auxDownstreamsMap, BooleanMap& confluenceMap, BooleanMap& bifurcationMap)
    {
        PROFILED_FUNCTION();
        HydroMaps& hydroMaps = context.hydroMaps.value();

        DistanceMap distanceMap{ context.dimensions };
        TrackingStableTileQueue queue{ context.dimensions };
        primeHydroFlagQueue(context, queue, { skeleton, QueueMode::height });

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };

                if (hydroMaps.flags.inBounds(nPos) && hydroMaps.flags[nPos].has(skeleton))
                {
                    if (queue.isMarked(nPos))
                    {
                        if (distanceMap[tile.pos] < distanceMap[nPos]) // nPos is upstream of pos
                        {
                            auxDownstreamsMap[nPos].insert(tile.pos);
                            auxUpstreamsMap[tile.pos].insert(nPos);
                        }
                        if (distanceMap[tile.pos] > distanceMap[nPos]) // nPos is downstream of pos
                        {
                            auxDownstreamsMap[tile.pos].insert(nPos);
                            auxUpstreamsMap[nPos].insert(tile.pos);
                        }
                    }
                    else
                    {
                        mainUpstreamMap[tile.pos] = nPos;
                        auxUpstreamsMap[tile.pos].insert(nPos);

                        mainDownstreamMap[nPos] = tile.pos;
                        auxDownstreamsMap[nPos].insert(tile.pos);

                        const float nDistance = tile.value + 1.0f;
                        queue.emplaceAndSetMarked(nPos, nDistance);
                        distanceMap[nPos] = nDistance;
                    }
                }
            }
        }

        // identify confluences and bifurcations
        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            for (int32_t x = 0; x < context.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!hydroMaps.flags[pos].has(skeleton))
                {
                    continue;
                }

                if (auxDownstreamsMap[pos].size() > 1)
                {
                    bifurcationMap[pos] = true;
                }
                if (auxUpstreamsMap[pos].size() > 1)
                {
                    confluenceMap[pos] = true;
                }
            }
        }
    }
    void traceFromSources(
        const MapGenCtx& context, const TileCoordsXYSet& sources, BackrefsMap& sourceHits, const BackrefsMap& auxDownstreamsMap,
        BooleanMap& confluenceMap, BackrefMap& confluenceMaxSourceMap)
    {
        DistanceMap confluenceMaxDistanceMap{ context.dimensions };
        for (const auto& source : sources)
        {
            TrackingStableTileQueue queue{ context.dimensions };
            queue.emplaceAndSetMarked(source, 0.0f);

            while (!queue.empty())
            {
                const QueueTile tile = queue.top();
                queue.pop();

                sourceHits[tile.pos].insert(source);

                if (confluenceMap[tile.pos])
                {
                    const Backref& maybeMaxSource = confluenceMaxSourceMap[tile.pos];
                    if (!maybeMaxSource.has_value() || tile.value > confluenceMaxDistanceMap[tile.pos])
                    {
                        confluenceMaxSourceMap[tile.pos] = source;
                        confluenceMaxDistanceMap[tile.pos] = tile.value;
                    }
                }

                for (const auto& backref : auxDownstreamsMap[tile.pos])
                {
                    if (!queue.isMarked(backref))
                    {
                        const float bDistance = tile.value + 1.0f;
                        queue.emplaceAndSetMarked(backref, bDistance);
                    }
                }
            }
        }

        // unmark single-source confluences
        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            for (int32_t x = 0; x < context.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (confluenceMap[pos] && sourceHits[pos].size() == 1)
                {
                    confluenceMap[pos] = false;
                }
            }
        }
    }

    void traceFromSinks(
        const MapGenCtx& context, const TileCoordsXYSet& sinks, BackrefsMap& sinkHits,
        const BackrefsMap& auxUpstreamsMap, BooleanMap& bifurcationMap)
    {
        for (const auto& sink : sinks)
        {
            TrackingStableTileQueue queue{ context.dimensions };
            queue.emplaceAndSetMarked(sink, 0.0f);

            while (!queue.empty())
            {
                const QueueTile tile = queue.top();
                queue.pop();

                sinkHits[tile.pos].insert(sink);

                for (const auto& backref : auxUpstreamsMap[tile.pos])
                {
                    if (!queue.isMarked(backref))
                    {
                        const float bDistance = tile.value + 1.0f;
                        queue.emplaceAndSetMarked(backref, bDistance);
                    }
                }
            }
        }

        // unmark single-sink bifurcations
        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            for (int32_t x = 0; x < context.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (bifurcationMap[pos] && sinkHits[pos].size() == 1)
                {
                    bifurcationMap[pos] = false;
                }
            }
        }
    }

    size_t pruneSources(
        MapGenCtx& context, TileCoordsXYSet& sources, const BackrefMap& mainDownstreamMap, const BackrefsMap& auxDownstreamsMap,
        const BooleanMap& confluenceMap, const BackrefsMap& sourceHits, const BackrefMap& confluenceMaxSourceMap)
    {
        HydroMaps& hydroMaps = context.hydroMaps.value();
        // downstream from sources following backrefs until the first confluence to identify prunable streams
        TileCoordsXYSet prunedSources;
        for (const auto& source : sources)
        {
            TileCoordsXYSet pruneCandidates;
            TileCoordsXYSet validConfluence;
            TrackingStableTileQueue pruneQueue{ context.dimensions };
            bool thresholdExceeded = false;
            bool maxSource = false;
            pruneQueue.emplaceAndSetMarked(source, 0.0f);

            while (!pruneQueue.empty())
            {
                QueueTile tile = pruneQueue.top();
                pruneQueue.pop();

                // if the distance from the source is exceeded on any path consider the stream not prunable
                if (tile.value > context.settings.pruneThreshold)
                {
                    thresholdExceeded = true;
                }
                else if (confluenceMap[tile.pos])
                {
                    const auto& maybeMaxSource = confluenceMaxSourceMap[tile.pos];
                    if (maybeMaxSource.has_value() && maybeMaxSource.value() == source)
                    {
                        // if this is the furthest source of any confluence the stream isn't prunable
                        maxSource = true;
                    }
                    else
                    {
                        validConfluence.insert(tile.pos);
                    }
                }
                // continue downstream, enqueue the 'main' backref or backrefs that are only visited by this source
                else
                {
                    pruneCandidates.insert(tile.pos);
                    for (const auto& backref : auxDownstreamsMap[tile.pos])
                    {
                        if (!pruneQueue.isMarked(backref))
                        {
                            if ((mainDownstreamMap[tile.pos].has_value() && mainDownstreamMap[tile.pos].value() == backref)
                                || (sourceHits[backref].contains(source) && sourceHits[backref].size() == 1))
                            {
                                pruneQueue.emplaceAndSetMarked(backref, tile.value + 1.0f);
                            }
                        }
                    }
                }
            }

            // the source and its downstream stream(s) are prunable
            if (!validConfluence.empty() && !thresholdExceeded && !maxSource)
            {
                for (const auto& toPrune : pruneCandidates)
                {
                    hydroMaps.flags[toPrune].unset(skeleton);
                }
                prunedSources.insert(source);
            }
        }
        for (const TileCoordsXY& source : prunedSources)
        {
            sources.erase(source);
        }
        return prunedSources.size();
    }

    size_t pruneSinks(
        MapGenCtx& context, TileCoordsXYSet& sinks, const BackrefMap& mainUpstreamMap, const BackrefsMap& auxUpstreamsMap,
        const BooleanMap& bifurcationMap, const BackrefsMap& sinkHits)
    {
        HydroMaps& hydroMaps = context.hydroMaps.value();
        // upstream from sinks following backrefs until the first bifurcation to identify prunable streams
        TileCoordsXYSet prunedSinks;
        for (const auto& sink : sinks)
        {
            TileCoordsXYSet pruneCandidates;
            TileCoordsXYSet validBifurcations;
            TrackingStableTileQueue pruneQueue{ context.dimensions };
            pruneQueue.emplaceAndSetMarked(sink, 0.0f);

            while (!pruneQueue.empty())
            {
                QueueTile tile = pruneQueue.top();
                pruneQueue.pop();

                if (bifurcationMap[tile.pos])
                {
                    validBifurcations.insert(tile.pos);
                }
                // continue upstream, enqueue the 'main' backref or backrefs that are only visited by this sink
                else
                {
                    pruneCandidates.insert(tile.pos);
                    for (const auto& backref : auxUpstreamsMap[tile.pos])
                    {
                        if (!pruneQueue.isMarked(backref))
                        {
                            if ((mainUpstreamMap[tile.pos].has_value() && mainUpstreamMap[tile.pos].value() == backref)
                                || (sinkHits[backref].contains(sink) && sinkHits[backref].size() == 1))
                            {
                                pruneQueue.emplaceAndSetMarked(backref, tile.value + 1.0f);
                            }
                        }
                    }
                }
            }

            // the sink and its upstream stream(s) are prunable
            if (!validBifurcations.empty())
            {
                for (const auto& toPrune : pruneCandidates)
                {
                    hydroMaps.flags[toPrune].unset(skeleton);
                }
                prunedSinks.insert(sink);
            }
        }
        for (const TileCoordsXY& sink : prunedSinks)
        {
            sinks.erase(sink);
        }
        return prunedSinks.size();
    }

} // namespace OpenRCT2::World::MapGenerator::Hydro
