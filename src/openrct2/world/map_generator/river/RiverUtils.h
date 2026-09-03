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

namespace OpenRCT2::World::MapGenerator::River
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

    void initRiverFlagQueue(MapGenContext& ctx, TrackingStableTileQueue& queue, RiverFlag flag = RiverFlag::river);

    int8_t countRiverInflows(MapGenContext& ctx, const TileCoordsXY& pos);

    float riverWidth(const MapGenContext& ctx, const TileCoordsXY& pos);
    float riverDepth(float width);

    std::array<TileCoordsXY, 2> ordinalNeighbours(const TileCoordsXY& offset);
    bool haveCommonOrdinalNeighbour(const RiverContext& riverCtx, const TileCoordsXY& pos, const TileCoordsXY& offset);

    std::string summarizeRiverStatistics(const MapGenContext& ctx);

    void handleConsistencyRunaway(
        const MapGenContext& ctx, const TileCoordsXY& segment, const TileCoordsXY& pos, size_t segmentSize, bool lowered);

} // namespace OpenRCT2::World::MapGenerator::River
