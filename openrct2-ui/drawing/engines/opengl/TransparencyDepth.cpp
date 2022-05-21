/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

#    include "TransparencyDepth.h"

#    include <algorithm>
#    include <map>
#    include <vector>

/*
 * Structure to store locations of vertical bounding box edge.
 */
struct XData
{
    int32_t xposition;
    bool begin;
    int32_t top, bottom;
};
using SweepLine = std::vector<XData>;

/*
 * Creates a list of vertical bounding box edges, stored as xdata and sorted
 * from left to right. If multiple edges are at the same x coordinate, Then
 * edges for boxes to the left will appear before edges for boxes to the right.
 */
static inline SweepLine CreateXList(const RectCommandBatch& transparent)
{
    SweepLine x_sweep;
    x_sweep.reserve(transparent.size() * 2);

    for (const DrawRectCommand& command : transparent)
    {
        int32_t left = std::min(std::max(command.bounds.x, command.clip.x), command.clip.z);
        int32_t top = std::min(std::max(command.bounds.y, command.clip.y), command.clip.w);
        int32_t right = std::min(std::max(command.bounds.z, command.clip.x), command.clip.z);
        int32_t bottom = std::min(std::max(command.bounds.w, command.clip.y), command.clip.w);

        assert(left <= right);
        assert(top <= bottom);
        if (left == right)
            continue;
        if (top == bottom)
            continue;

        x_sweep.push_back({ left, true, top, bottom });
        x_sweep.push_back({ right, false, top, bottom });
    }

    std::sort(x_sweep.begin(), x_sweep.end(), [](const XData& a, const XData& b) -> bool {
        if (a.xposition != b.xposition)
            return a.xposition < b.xposition;

        return !a.begin && b.begin;
    });

    return x_sweep;
}

/*
 * Structure that stores intervals. YData.count stores how many intervals have
 * an endpoint at this position, and YData.depth stores how many intervals
 * intersect on the left limit of this point. In other words, IntervalTree
 * stores half-closed intervals, with the left endpoint open, and the right
 * endpoint closed. The IntervalTree uses std::map because it stores the values
 * sorted, and we can traverse it in order using its bidirectional iterators.
 */
struct YData
{
    int32_t count, depth;
};
using IntervalTree = std::map<int32_t, YData>;

/*
 * Inserts the interval's top endpoint into the interval tree. If the endpoint
 * already exists in the interval tree, it stacks the endpoints.
 */
static inline IntervalTree::iterator InsertTopEndpoint(IntervalTree& y_intersect, int32_t top)
{
    auto top_in = y_intersect.insert({ top, { 1, 0 } });
    IntervalTree::iterator top_it = top_in.first;
    if (top_in.second)
    {
        IntervalTree::iterator top_next = std::next(top_it);
        if (top_next != y_intersect.end())
        {
            top_it->second.depth = top_next->second.depth;
        }
    }
    else
    {
        ++top_it->second.count;
    }
    return top_it;
}

/*
 * Inserts the interval's bottom endpoint into the interval tree. If the
 * endpoint already exists in the interval tree, it stacks the endpoint.
 * This function can produce a new maximum depth.
 */
static inline IntervalTree::iterator InsertBottomEndpoint(IntervalTree& y_intersect, int32_t bottom)
{
    auto bottom_in = y_intersect.insert({ bottom, { 1, 1 } });
    IntervalTree::iterator bottom_it = bottom_in.first;
    if (bottom_in.second)
    {
        IntervalTree::iterator bottom_next = std::next(bottom_it);
        if (bottom_next != y_intersect.end())
        {
            bottom_it->second.depth = bottom_next->second.depth + 1;
        }
    }
    else
    {
        ++bottom_it->second.count;
        ++bottom_it->second.depth;
    }
    return bottom_it;
}

/*
 * Removes the interval's top endpoint, handling stacked endpoints.
 */
static inline void RemoveTopEndpoint(IntervalTree& y_intersect, IntervalTree::iterator top_it)
{
    if (top_it->second.count == 1)
    {
        y_intersect.erase(top_it);
    }
    else
    {
        --top_it->second.count;
    }
}

/*
 * Removes the interval's bottom endpoint, handling stacked endpoints.
 */
static inline void RemoveBottomEndpoint(IntervalTree& y_intersect, IntervalTree::iterator bottom_it)
{
    if (bottom_it->second.count == 1)
    {
        y_intersect.erase(bottom_it);
    }
    else
    {
        --bottom_it->second.count;
        --bottom_it->second.depth;
    }
}

/*
 * Determines an approximation of the number of depth peeling iterations needed
 * to render the command batch. It will never underestimate the number of
 * iterations, but it can overestimate, usually by no more than +2.
 */
int32_t MaxTransparencyDepth(const RectCommandBatch& transparent)
{
    int32_t max_depth = 1;
    SweepLine x_sweep = CreateXList(transparent);
    IntervalTree y_intersect{};

    for (const XData& x : x_sweep)
    {
        if (x.begin)
        {
            IntervalTree::iterator top_it = InsertTopEndpoint(y_intersect, x.top);
            IntervalTree::iterator bottom_it = InsertBottomEndpoint(y_intersect, x.bottom);
            max_depth = std::max(max_depth, bottom_it->second.depth);

            /*
             * Increment the depth for endpoings that intersect this interval
             */
            for (IntervalTree::iterator it = std::next(top_it); it != bottom_it && it != std::end(y_intersect); ++it)
            {
                max_depth = std::max(max_depth, ++it->second.depth);
            }
        }
        else
        {
            IntervalTree::iterator top_it = y_intersect.find(x.top);
            IntervalTree::iterator bottom_it = y_intersect.find(x.bottom);

            /*
             * Decrement the depth for endpoings that intersected this interval
             */
            for (IntervalTree::iterator it = std::next(top_it); it != bottom_it && it != std::end(y_intersect); ++it)
            {
                --it->second.depth;
            }

            RemoveTopEndpoint(y_intersect, top_it);
            RemoveBottomEndpoint(y_intersect, bottom_it);
        }
    }

    return max_depth;
}

#endif /* DISABLE_OPENGL */
