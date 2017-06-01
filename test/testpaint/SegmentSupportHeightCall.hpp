#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <vector>

#include <openrct2/common.h>

extern "C" {
    #include <openrct2/paint/paint.h>
}

struct SegmentSupportCall
{
    uint16 segments;
    sint32 height;
    sint16 slope;

    bool operator<(const SegmentSupportCall &other) const {
        if (height != other.height) {
            return height < other.height;
        }

        if (segments != other.segments) {
            return segments < other.segments;
        }

        return slope < other.slope;
    }
};

class SegmentSupportHeightCall {
public:
    static std::vector<SegmentSupportCall> getSegmentCalls(support_height supports[9], uint8 rotation);
    static bool CallsMatch(std::vector<SegmentSupportCall> tileSegmentSupportCalls[4]);
    static bool CallsEqual(std::vector<SegmentSupportCall> lhs, std::vector<SegmentSupportCall> rhs);
    static bool FindMostCommonSupportCall(std::vector<SegmentSupportCall> calls[4], std::vector<SegmentSupportCall> * out);
};
