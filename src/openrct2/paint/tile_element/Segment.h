/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/EnumUtils.hpp"
#include "../../core/FlagHolder.hpp"
#include "../../core/Numerics.hpp"

#include <array>
#include <cstdint>

namespace OpenRCT2
{
    // This controls in which segment of a tile something is drawn.
    // This is from a screen perspective, e.g. top will always represent the part of top of the screen.
    enum class PaintSegment : uint16_t
    {
        top = 0,
        topRight = 1,
        right = 2,
        bottomRight = 3,
        bottom = 4,
        bottomLeft = 5,
        left = 6,
        topLeft = 7,
        centre = 8,
    };
    using PaintSegments = FlagHolder<uint16_t, PaintSegment>;
    constexpr PaintSegments kSegmentsNone{};
    constexpr PaintSegments kSegmentsAll = { PaintSegment::top,      PaintSegment::left,       PaintSegment::right,
                                             PaintSegment::bottom,   PaintSegment::centre,     PaintSegment::topLeft,
                                             PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight };
    constexpr PaintSegments kSegmentsUnimplemented = kSegmentsNone;

    constexpr PaintSegments paintSegmentsRotate(PaintSegments segments, const uint8_t rotation)
    {
        uint8_t outerSegments = segments.holder & 0xFF;
        outerSegments = Numerics::rol8(outerSegments, rotation * 2);
        segments.holder = static_cast<uint16_t>((segments.holder & EnumToFlag(PaintSegment::centre)) | outerSegments);
        return segments;
    }

    constexpr PaintSegments paintSegmentsFlipXAxis(PaintSegments segments)
    {
        uint8_t outerSegments = segments.holder & 0xFF;
        outerSegments = (outerSegments * 0x0202020202 & 0x010884422010) % 1023; // reverse the bits, std::byteswap is c++23
        outerSegments = Numerics::rol8(outerSegments, 3);
        segments.holder = static_cast<uint16_t>((segments.holder & EnumToFlag(PaintSegment::centre)) | outerSegments);
        return segments;
    }

    enum class BlockedSegmentsType : uint8_t
    {
        narrow,
        inverted,
        wide,
    };
    constexpr uint32_t kBlockedSegmentsTypeCount = 3;

    constexpr std::array<PaintSegments, kBlockedSegmentsTypeCount> blockedSegmentsAllTypes(const PaintSegments segments)
    {
        return { segments, segments, segments };
    }

    constexpr std::array<PaintSegments, kBlockedSegmentsTypeCount> blockedSegmentsRotate(
        std::array<PaintSegments, kBlockedSegmentsTypeCount> blockedSegments, const uint8_t rotation)
    {
        for (auto& segments : blockedSegments)
        {
            segments = paintSegmentsRotate(segments, rotation);
        }
        return blockedSegments;
    }

    constexpr std::array<PaintSegments, kBlockedSegmentsTypeCount> blockedSegmentsFlipXAxis(
        std::array<PaintSegments, kBlockedSegmentsTypeCount> blockedSegments)
    {
        for (auto& segments : blockedSegments)
        {
            segments = paintSegmentsFlipXAxis(segments);
        }
        return blockedSegments;
    }
} // namespace OpenRCT2
