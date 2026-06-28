/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "BaseMap.hpp"

#include <queue>

namespace OpenRCT2::World::MapGenerator
{

    struct QueueTile
    {
        TileCoordsXY pos;
        float value;
        uint32_t orderIdx;

        friend bool operator<(const QueueTile& lhs, const QueueTile& rhs)
        {
            if (lhs.value < rhs.value)
                return true;
            if (rhs.value < lhs.value)
                return false;
            return lhs.orderIdx < rhs.orderIdx;
        }
        friend bool operator<=(const QueueTile& lhs, const QueueTile& rhs)
        {
            return !(rhs < lhs);
        }
        friend bool operator>(const QueueTile& lhs, const QueueTile& rhs)
        {
            return rhs < lhs;
        }
        friend bool operator>=(const QueueTile& lhs, const QueueTile& rhs)
        {
            return !(lhs < rhs);
        }
    };

    using StableTileQueueBase = std::priority_queue<QueueTile, std::vector<QueueTile>, std::greater<QueueTile>>;
    class StableTileQueue : public StableTileQueueBase
    {
    private:
        uint32_t _insertIdx = 0;

    public:
        void push() = delete;

        template<class... Args>
        void emplace(Args... args)
        {
            StableTileQueueBase::emplace(std::forward<Args>(args)..., _insertIdx++);
        }
    };

    class TrackingStableTileQueue : public StableTileQueue
    {
    private:
        BooleanMap _visited;

    public:
        explicit TrackingStableTileQueue(const TileCoordsXY& dimensions)
            : _visited(dimensions)
        {
        }

        bool visited(const TileCoordsXY& pos)
        {
            return _visited[pos];
        }

        void visit(const TileCoordsXY& pos)
        {
            _visited[pos] = true;
        }

        template<class... Args>
        void emplaceAndVisit(const TileCoordsXY pos, Args... args)
        {
            emplace(pos, std::forward<Args>(args)...);
            visit(pos);
        }
    };

} // namespace OpenRCT2::World::MapGenerator
