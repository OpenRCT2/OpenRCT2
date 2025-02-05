/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../drawing/ImageId.hpp"
#include "../../world/Location.hpp"

#include <cstdint>

namespace OpenRCT2
{
    enum class TrackElemType : uint16_t;
}

enum class WoodenSupportType : uint8_t
{
    Truss = 0,
    Mine = 1,
};

enum class WoodenSupportSubType : uint8_t
{
    NeSw = 0,
    NwSe = 1,
    Corner0 = 2,
    Corner1 = 3,
    Corner2 = 4,
    Corner3 = 5,

    Null,
};

enum class WoodenSupportTransitionType : uint8_t
{
    None = 255,
    FlatToUp25Deg = 0,
    Up25DegToFlat = 1,
    Up25Deg = 2,
    Up25DegToUp60Deg = 3,
    Up60DegToUp25Deg = 4,
    Up60Deg = 5,

    FlatToUp60Deg = 7,
    Up60DegToFlat = 8,
    Scenery = 9,
    FlatToUp60DegLongBaseSeq0 = 10,
    FlatToUp60DegLongBaseSeq1 = 11,
    FlatToUp60DegLongBaseSeq2 = 12,
    FlatToUp60DegLongBaseSeq3 = 13,
    Up60DegToFlatLongBaseSeq0 = 14,
    Up60DegToFlatLongBaseSeq1 = 15,
    Up60DegToFlatLongBaseSeq2 = 16,
    Up60DegToFlatLongBaseSeq3 = 17,

    // Inverted versions
    Down25DegToFlat = 18,
    FlatToDown25Deg = 19,
    Down25Deg = 20,
    Down60DegToDown25Deg = 21,
    Down25DegToDown60Deg = 22,
    Down60Deg = 23,

    Down60DegToFlat = 25,
    FlatToDown60Deg = 26,

    Down60DegToFlatLongBaseSeq3 = 28,
    Down60DegToFlatLongBaseSeq2 = 29,
    Down60DegToFlatLongBaseSeq1 = 30,
    Down60DegToFlatLongBaseSeq0 = 31,
    FlatToDown60DegLongBaseSeq3 = 32,
    FlatToDown60DegLongBaseSeq2 = 33,
    FlatToDown60DegLongBaseSeq1 = 34,
    FlatToDown60DegLongBaseSeq0 = 35,
};

constexpr uint8_t kUninvertedWoodenSupportTransitionTypeCount = 18;

struct FootpathPaintInfo;
struct PaintSession;

bool WoodenASupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::None, Direction direction = 0,
    const bool covered = false, const Direction coverDirection = 0);
bool WoodenASupportsPaintSetupRotated(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, Direction direction, int32_t height,
    ImageId imageTemplate, WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::None,
    const bool covered = false, const Direction coverDirection = 0);
bool WoodenBSupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::None, Direction direction = 0,
    const bool covered = false, const Direction coverDirection = 0);
bool WoodenBSupportsPaintSetupRotated(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, Direction direction, int32_t height,
    ImageId imageTemplate, WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::None,
    const bool covered = false, const Direction coverDirection = 0);
bool PathBoxSupportsPaintSetup(
    PaintSession& session, WoodenSupportSubType supportType, bool isSloped, Direction slopeRotation, int32_t height,
    ImageId imageTemplate, const FootpathPaintInfo& pathPaintInfo);
