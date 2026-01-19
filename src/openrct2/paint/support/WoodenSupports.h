/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    truss = 0,
    mine = 1,
};

enum class WoodenSupportSubType : uint8_t
{
    neSw = 0,
    nwSe = 1,
    corner0 = 2,
    corner1 = 3,
    corner2 = 4,
    corner3 = 5,

    null,
};

enum class WoodenSupportTransitionType : uint8_t
{
    none = 255,
    flatToUp25Deg = 0,
    up25DegToFlat = 1,
    up25Deg = 2,
    up25DegToUp60Deg = 3,
    up60DegToUp25Deg = 4,
    up60Deg = 5,
    up25even = 6,
    flatToUp60Deg = 7,
    up60DegToFlat = 8,
    flatToUp25DegRailway = 9,
    up25DegToFlatRailway = 10,
    up25DegRailway = 11,
    scenery = 12,
    flatToUp60DegLongBaseSeq0 = 13,
    flatToUp60DegLongBaseSeq1 = 14,
    flatToUp60DegLongBaseSeq2 = 15,
    flatToUp60DegLongBaseSeq3 = 16,
    up60DegToFlatLongBaseSeq0 = 17,
    up60DegToFlatLongBaseSeq1 = 18,
    up60DegToFlatLongBaseSeq2 = 19,
    up60DegToFlatLongBaseSeq3 = 20,
};
constexpr int32_t kWoodenSupportTransitionTypeCount = 21;

struct PaintSession;
struct PathRailingsDescriptor;

bool WoodenASupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::none, Direction direction = 0);
bool WoodenASupportsPaintSetupRotated(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, Direction direction, int32_t height,
    ImageId imageTemplate, WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::none);
bool WoodenBSupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::none, Direction direction = 0);
bool WoodenBSupportsPaintSetupRotated(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, Direction direction, int32_t height,
    ImageId imageTemplate, WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::none);
bool PathBoxSupportsPaintSetup(
    PaintSession& session, WoodenSupportSubType supportType, bool isSloped, Direction slopeRotation, int32_t height,
    ImageId imageTemplate, const PathRailingsDescriptor& pathPaintInfo);
bool DrawSupportForSequenceA(
    PaintSession& session, WoodenSupportType supportType, OpenRCT2::TrackElemType trackType, uint8_t sequence,
    Direction direction, int32_t height, ImageId imageTemplate);
bool DrawSupportForSequenceB(
    PaintSession& session, WoodenSupportType supportType, OpenRCT2::TrackElemType trackType, uint8_t sequence,
    Direction direction, int32_t height, ImageId imageTemplate);
