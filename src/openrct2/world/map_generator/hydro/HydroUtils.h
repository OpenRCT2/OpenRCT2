/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../MapGen.h"
#include "../TileQueue.hpp"

namespace OpenRCT2::World::MapGenerator::Hydro
{

    enum class ConsistencyOperation
    {
        raise,
        lower,
        remove
    };

    struct SegmentKey
    {
        TileCoordsXY pos;
        size_t size;
        ConsistencyOperation operation;

        friend bool operator==(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            return lhs.pos == rhs.pos && lhs.size == rhs.size && lhs.operation == rhs.operation;
        }
        friend bool operator!=(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            return !(lhs == rhs);
        }
        friend bool operator<(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            if (lhs.size < rhs.size)
            {
                return true;
            }
            if (rhs.size < lhs.size)
            {
                return false;
            }
            return lhs.operation < rhs.operation;
        }
        friend bool operator<=(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            return !(rhs < lhs);
        }
        friend bool operator>(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            return rhs < lhs;
        }
        friend bool operator>=(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            return !(lhs < rhs);
        }
    };

    struct SegmentKeyHash
    {
        size_t operator()(const SegmentKey& s) const noexcept
        {
            size_t hash = 0;
            Util::Hash::update(hash, s.pos.x);
            Util::Hash::update(hash, s.pos.y);
            Util::Hash::update(hash, s.size);
            Util::Hash::update(hash, s.operation);
            return hash;
        }
    };


    using QueueInitPosCallback =  std::optional<std::function<void(const TileCoordsXY&)>>;

    void primeHydroFlagHeightQueue(MapGenContext& ctx, TrackingStableHeightTileQueue& queue, HydroFlag flag = HydroFlag::river, const QueueInitPosCallback& callback = std::nullopt);
    void primeHydroFlagDistanceQueue(MapGenContext& ctx, TrackingStableTileDistanceTileQueue& queue, HydroFlag flag = HydroFlag::river, const QueueInitPosCallback& callback = std::nullopt);
    void findSourcesAndSinks(MapGenContext& ctx, TileCoordsXYSet& sources, TileCoordsXYSet& sinks);

    std::array<TileCoordsXY, 2> ordinalNeighbours(const TileCoordsXY& offset);
    bool haveCommonOrdinalNeighbour(const HydroContext& hydroCtx, const TileCoordsXY& pos, const TileCoordsXY& offset);

    std::string summarizeHydroStatistics(const MapGenContext& ctx);

} // namespace OpenRCT2::World::MapGenerator::Hydro
