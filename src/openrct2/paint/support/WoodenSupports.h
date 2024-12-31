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
    FlatToUp25DegRailway = 9,
    Up25DegToFlatRailway = 10,
    Up25DegRailway = 11,
    Scenery = 12,
    FlatToUp60DegLongBaseSeq0 = 13,
    FlatToUp60DegLongBaseSeq1 = 14,
    FlatToUp60DegLongBaseSeq2 = 15,
    FlatToUp60DegLongBaseSeq3 = 16,
    Up60DegToFlatLongBaseSeq0 = 17,
    Up60DegToFlatLongBaseSeq1 = 18,
    Up60DegToFlatLongBaseSeq2 = 19,
    Up60DegToFlatLongBaseSeq3 = 20,

    // Inverted versions
    Down25DegToFlat = 21,
    FlatToDown25Deg = 22,
    Down25Deg = 23,
    Down60DegToDown25Deg = 24,
    Down25DegToDown60Deg = 25,
    Down60Deg = 26,
    Down60DegToFlat = 28,
    FlatToDown60Deg = 29,
    Down25DegToFlatRailway = 30,
    FlatToDown25DegRailway = 31,
    Down25DegRailway = 32,

    Down60DegToFlatLongBaseSeq3 = 34,
    Down60DegToFlatLongBaseSeq2 = 35,
    Down60DegToFlatLongBaseSeq1 = 36,
    Down60DegToFlatLongBaseSeq0 = 37,
    FlatToDown60DegLongBaseSeq3 = 38,
    FlatToDown60DegLongBaseSeq2 = 39,
    FlatToDown60DegLongBaseSeq1 = 40,
    FlatToDown60DegLongBaseSeq0 = 41,
};

struct FootpathPaintInfo;
struct PaintSession;

bool WoodenASupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::None, Direction direction = 0);
bool WoodenASupportsPaintSetupRotated(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, Direction direction, int32_t height,
    ImageId imageTemplate, WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::None);
bool WoodenBSupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::None, Direction direction = 0);
bool WoodenBSupportsPaintSetupRotated(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, Direction direction, int32_t height,
    ImageId imageTemplate, WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::None);
bool PathBoxSupportsPaintSetup(
    PaintSession& session, WoodenSupportSubType supportType, bool isSloped, Direction slopeRotation, int32_t height,
    ImageId imageTemplate, const FootpathPaintInfo& pathPaintInfo);
bool DrawSupportForSequenceA(
    PaintSession& session, WoodenSupportType supportType, OpenRCT2::TrackElemType trackType, uint8_t sequence,
    Direction direction, int32_t height, ImageId imageTemplate);
bool DrawSupportForSequenceB(
    PaintSession& session, WoodenSupportType supportType, OpenRCT2::TrackElemType trackType, uint8_t sequence,
    Direction direction, int32_t height, ImageId imageTemplate);
