/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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

using track_type_t = uint16_t;

enum class MetalSupportType
{
    /**
     * Used by the Steel Twister, Looping RC, and other rides.
     */
    Tubes = 0,

    /**
     * Used by the Junior RC and other rides.
     */
    Fork = 1,

    /**
     * Used by the vertical roller coasters, the Log Flume and other rides.
     */
    Boxed = 2,

    /**
     * Used by the Steeplechase.
     */
    Stick = 3,

    /**
     * Used for the Looping Roller Coaster’s loop, and only for that specific part.
     */
    Thick = 4,

    /**
     * Used by the chairlift.
     */
    Truss = 5,

    /**
     * Used by inverted rcs like the flying, lay-down, compact inverted.
     * Mostly the same as `Tubes`, but with a thinner crossbeam.
     */
    TubesInverted = 6,

    /**
     * Does not seem to be used in RCT2, but it was used in RCT1 for one of the path support types.
     */
    BoxedCoated = 7,
};
constexpr uint8_t kMetalSupportTypeCount = 8;

// There are 13 types of metal support graphics, including rotated versions. A graphic showing all of them is available here:
// https://cloud.githubusercontent.com/assets/737603/19420485/7eaba28e-93ec-11e6-83cb-03190accc094.png
// TODO Make this internal to MetalSupports.cpp
enum class MetalSupportGraphic : uint8_t
{
    /**
     * Used by the Steel Twister, Looping RC, and other rides.
     */
    Tubes = 0,
    /**
     * Used by the Junior RC and other rides.
     */
    Fork = 1,
    /**
     * Rotated version of `Fork`.
     */
    ForkAlt = 2,
    /**
     * Used by the vertical roller coasters, the Log Flume and other rides.
     */
    Boxed = 3,
    /**
     * Used by the Steeplechase.
     */
    Stick = 4,
    /**
     * No visible difference from `Stick`, also used by the Steeplechase
     */
    StickAlt = 5,
    /**
     * Every “Thick” type seems to be used for the Looping Roller Coaster’s loop, and only for that specific part.
     */
    ThickCentred = 6,
    Thick = 7,
    ThickAlt = 8,
    ThickAltCentred = 9,
    /**
     * Used by the chairlift.
     */
    Truss = 10,
    /**
     * Used by inverted rcs like the flying, lay-down, compact inverted.
     * Mostly the same as `Tubes`, but with a thinner crossbeam.
     */
    TubesInverted = 11,
    /**
     * Does not seem to be used in RCT2, but it was used in RCT1 for one of the path support types.
     */
    BoxedCoated,
};

/**
 * Tiles are rendered at a 45 degree angle, with the corners on the top, bottom, left and right.
 * This enum controls where the supports are rendered on the screen.
 *
 * The tile is divided into nine parts:
 *
 *        0
 *    5       6
 * 1      4      2
 *    7       8
 *        3
 */
enum class MetalSupportPlace : uint8_t
{
    TopCorner = 0,
    LeftCorner = 1,
    RightCorner = 2,
    BottomCorner = 3,
    Centre = 4,
    TopLeftSide = 5,
    TopRightSide = 6,
    BottomLeftSide = 7,
    BottomRightSide = 8,

    None = 255,
};

struct FootpathPaintInfo;
struct PaintSession;

bool MetalASupportsPaintSetup(
    PaintSession& session, MetalSupportGraphic supportTypeMember, MetalSupportPlace placement, int32_t special, int32_t height,
    ImageId imageTemplate);
/** @deprecated */
bool MetalASupportsPaintSetup(
    PaintSession& session, MetalSupportType supportType, MetalSupportPlace placement, int32_t special, int32_t height,
    ImageId imageTemplate);
bool MetalASupportsPaintSetupRotated(
    PaintSession& session, MetalSupportType supportType, MetalSupportPlace placement, Direction direction, int32_t special,
    int32_t height, ImageId imageTemplate);
bool MetalBSupportsPaintSetup(
    PaintSession& session, MetalSupportGraphic supportTypeMember, MetalSupportPlace placement, int32_t special, int32_t height,
    ImageId imageTemplate);
/** @deprecated */
bool MetalBSupportsPaintSetup(
    PaintSession& session, MetalSupportType supportType, MetalSupportPlace placement, int32_t special, int32_t height,
    ImageId imageTemplate);
bool MetalBSupportsPaintSetupRotated(
    PaintSession& session, MetalSupportType supportType, MetalSupportPlace placement, Direction direction, int32_t special,
    int32_t height, ImageId imageTemplate);
MetalSupportGraphic RotateMetalSupportGraphic(MetalSupportType supportType, Direction direction);
void DrawSupportsSideBySide(
    PaintSession& session, Direction direction, uint16_t height, ImageId colour, MetalSupportType type, int32_t special = 0);
bool PathPoleSupportsPaintSetup(
    PaintSession& session, MetalSupportPlace supportPlace, bool isSloped, int32_t height, ImageId imageTemplate,
    const FootpathPaintInfo& pathPaintInfo);
