/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../drawing/ImageId.hpp"
#include "../world/Footpath.h"

struct FootpathPaintInfo;

constexpr uint8_t NumVanillaWoodenSupportTypes = 49;

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
};

// There are 13 types of metal supports. A graphic showing all of them is available here:
// https://cloud.githubusercontent.com/assets/737603/19420485/7eaba28e-93ec-11e6-83cb-03190accc094.png
enum class MetalSupportType : uint8_t
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
};

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
bool MetalASupportsPaintSetup(
    PaintSession& session, MetalSupportType supportTypeMember, MetalSupportPlace placement, int32_t special, int32_t height,
    ImageId imageTemplate);
bool MetalBSupportsPaintSetup(
    PaintSession& session, MetalSupportType supportTypeMember, MetalSupportPlace placement, int32_t special, int32_t height,
    ImageId imageTemplate);
bool PathBoxSupportsPaintSetup(
    PaintSession& session, WoodenSupportSubType supportType, bool isSloped, Direction slopeRotation, int32_t height,
    ImageId imageTemplate, const FootpathPaintInfo& pathPaintInfo);
bool PathPoleSupportsPaintSetup(
    PaintSession& session, MetalSupportPlace supportPlace, bool isSloped, int32_t height, ImageId imageTemplate,
    const FootpathPaintInfo& pathPaintInfo);
void DrawSupportsSideBySide(
    PaintSession& session, Direction direction, uint16_t height, ImageId colour, MetalSupportType type, int32_t special = 0);

enum
{
    SUPPORTS_SLOPE_5 = 1 << 5
};
