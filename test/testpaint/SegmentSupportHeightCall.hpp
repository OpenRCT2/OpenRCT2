/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>
#include <openrct2/paint/Paint.h>
#include <vector>

struct SegmentSupportCall
{
    uint16_t segments;
    int32_t height;
    int16_t slope;

    bool operator<(const SegmentSupportCall& other) const
    {
        if (height != other.height)
        {
            return height < other.height;
        }

        if (segments != other.segments)
        {
            return segments < other.segments;
        }

        return slope < other.slope;
    }
};

class SegmentSupportHeightCall
{
public:
    static std::vector<SegmentSupportCall> getSegmentCalls(support_height supports[9], uint8_t rotation);
    static bool CallsMatch(std::vector<SegmentSupportCall> tileSegmentSupportCalls[4]);
    static bool CallsEqual(std::vector<SegmentSupportCall> lhs, std::vector<SegmentSupportCall> rhs);
    static bool FindMostCommonSupportCall(std::vector<SegmentSupportCall> calls[4], std::vector<SegmentSupportCall>* out);
};
