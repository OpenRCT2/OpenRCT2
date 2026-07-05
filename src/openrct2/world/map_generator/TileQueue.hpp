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
    template<class T>
    struct QueueTile
    {
        TileCoordsXY pos;
        T value;
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

    template<class T>
    using StableTileQueueBase = std::priority_queue<QueueTile<T>, std::vector<QueueTile<T>>, std::greater<QueueTile<T>>>;

    template<class T>
    class StableTileQueue : public StableTileQueueBase<T>
    {
    private:
        uint32_t insertIdx = 0;

    public:
        void push() = delete;

        template<class... Args>
        void emplace(Args... args)
        {
            StableTileQueueBase<T>::emplace(std::forward<Args>(args)..., insertIdx++);
        }
    };

    template<class T>
    class TrackingStableTileQueue : public StableTileQueue<T>
    {
    private:
        BooleanMap marked;

    public:
        explicit TrackingStableTileQueue(const TileCoordsXY& dimensions)
            : StableTileQueue<T>(), marked{ dimensions }
        {
        }

        bool isMarked(const TileCoordsXY& pos)
        {
            return marked[pos];
        }

        void mark(const TileCoordsXY& pos)
        {
            marked[pos] = true;
        }

        template<class... Args>
        void emplaceAndMark(const TileCoordsXY pos, Args... args)
        {
            StableTileQueue<T>::emplace(pos, std::forward<Args>(args)...);
            mark(pos);
        }
    };

    using TrackingStableHeightTileQueue = TrackingStableTileQueue<float>;
    using TrackingStableTileDistanceTileQueue = TrackingStableTileQueue<uint32_t>;
    using TrackingStableEuclidianDistanceTileQueue = TrackingStableTileQueue<float>;
} // namespace OpenRCT2::World::MapGenerator
