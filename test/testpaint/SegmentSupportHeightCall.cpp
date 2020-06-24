/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SegmentSupportHeightCall.hpp"

#include <algorithm>
#include <map>
#include <openrct2/paint/tile_element/Paint.TileElement.h>

static bool SortSegmentSupportCalls(SegmentSupportCall lhs, SegmentSupportCall rhs)
{
    if (lhs.height != rhs.height)
    {
        return lhs.height < rhs.height;
    }

    if (lhs.slope != rhs.slope)
    {
        return lhs.slope < rhs.slope;
    }

    return lhs.segments < rhs.segments;
}

std::vector<SegmentSupportCall> SegmentSupportHeightCall::getSegmentCalls(support_height* supports, uint8_t rotation)
{
    uint16_t positionsRemaining = SEGMENTS_ALL;

    for (int i = 0; i < 9; i++)
    {
        if (supports[i].height == 0 && supports[i].slope == 0xFF)
        {
            positionsRemaining &= ~segment_offsets[i];
        }
    }

    std::vector<SegmentSupportCall> calls;

    while (positionsRemaining != 0)
    {
        SegmentSupportCall call = {};
        call.height = -1;
        call.slope = -1;

        support_height referenceSupport = {};

        for (int i = 0; i < 9; i++)
        {
            if (positionsRemaining & segment_offsets[i])
            {
                referenceSupport = supports[i];
                if (supports[i].height != 0)
                {
                    call.height = supports[i].height;
                }
                if (supports[i].slope != 0xFF)
                {
                    call.slope = supports[i].slope;
                }
                break;
            }
        }

        uint16_t positionsMatched = 0;
        for (int i = 0; i < 9; i++)
        {
            if (supports[i].height == referenceSupport.height && supports[i].slope == referenceSupport.slope)
            {
                positionsMatched |= segment_offsets[i];
            }
        }
        positionsRemaining &= ~positionsMatched;

        call.segments = paint_util_rotate_segments(positionsMatched, (4 - rotation) % 4);

        calls.push_back(call);
    }

    if (calls.size() > 1)
    {
        std::sort(calls.begin(), calls.end(), SortSegmentSupportCalls);
    }

    return calls;
}

bool SegmentSupportHeightCall::CallsMatch(std::vector<SegmentSupportCall> tileSegmentSupportCalls[4])
{
    std::vector<SegmentSupportCall> baseCallList = tileSegmentSupportCalls[0];
    for (int i = 1; i < 4; i++)
    {
        if (!CallsEqual(baseCallList, tileSegmentSupportCalls[i]))
        {
            return false;
        }
    }

    return true;
}

bool SegmentSupportHeightCall::CallsEqual(std::vector<SegmentSupportCall> lhs, std::vector<SegmentSupportCall> rhs)
{
    if (lhs.size() != rhs.size())
        return false;
    for (size_t i = 0; i < lhs.size(); ++i)
    {
        if (lhs[i].segments != rhs[i].segments)
            return false;
        if (lhs[i].height != rhs[i].height)
            return false;
        if (lhs[i].slope != rhs[i].slope)
            return false;
    }

    return true;
}

bool SegmentSupportHeightCall::FindMostCommonSupportCall(
    std::vector<SegmentSupportCall> calls[4], std::vector<SegmentSupportCall>* out)
{
    std::map<std::vector<SegmentSupportCall>, int> map;

    for (int i = 0; i < 4; ++i)
    {
        map[calls[i]] += 1;
    }

    if (map.size() == 1)
    {
        (*out) = calls[0];
        return true;
    }

    if (map.size() == 2)
    {
        for (auto&& item : map)
        {
            if (item.second == 3)
            {
                (*out) = item.first;
                return true;
            }
        }

        return false;
    }

    return false;
}
