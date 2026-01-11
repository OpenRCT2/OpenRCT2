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

enum class MetalSupportType
{
    /**
     * Used by the Steel Twister, Looping RC, and other rides.
     */
    tubes = 0,

    /**
     * Used by the Junior RC and other rides.
     */
    fork = 1,

    /**
     * Used by the vertical roller coasters, the Log Flume and other rides.
     */
    boxed = 2,

    /**
     * Used by the Steeplechase.
     */
    stick = 3,

    /**
     * Used for the Looping Roller Coaster’s loop, and only for that specific part.
     */
    thick = 4,

    /**
     * Used by the chairlift.
     */
    truss = 5,

    /**
     * Used by inverted rcs like the flying, lay-down, compact inverted.
     * Mostly the same as `Tubes`, but with a thinner crossbeam.
     */
    tubesInverted = 6,

    /**
     * Does not seem to be used in RCT2, but it was used in RCT1 for one of the path support types.
     */
    boxedCoated = 7,
};
constexpr uint8_t kMetalSupportTypeCount = 8;

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
    topCorner = 0,
    leftCorner = 1,
    rightCorner = 2,
    bottomCorner = 3,
    centre = 4,
    topLeftSide = 5,
    topRightSide = 6,
    bottomLeftSide = 7,
    bottomRightSide = 8,

    none = 255,
};

struct PaintSession;
struct PathRailingsDescriptor;

/** @deprecated */
bool MetalASupportsPaintSetup(
    PaintSession& session, MetalSupportType supportType, MetalSupportPlace placement, int32_t heightExtra, int32_t height,
    ImageId imageTemplate);
bool MetalASupportsPaintSetupRotated(
    PaintSession& session, MetalSupportType supportType, MetalSupportPlace placement, Direction direction, int32_t heightExtra,
    int32_t height, ImageId imageTemplate);
/** @deprecated */
bool MetalBSupportsPaintSetup(
    PaintSession& session, MetalSupportType supportType, MetalSupportPlace placement, int32_t heightExtra, int32_t height,
    ImageId imageTemplate);
bool MetalBSupportsPaintSetupRotated(
    PaintSession& session, MetalSupportType supportType, MetalSupportPlace placement, Direction direction, int32_t heightExtra,
    int32_t height, ImageId imageTemplate);
void DrawSupportsSideBySide(
    PaintSession& session, Direction direction, uint16_t height, ImageId colour, MetalSupportType type,
    int32_t heightExtra = 0);
bool PathPoleSupportsPaintSetup(
    PaintSession& session, MetalSupportPlace supportPlace, bool isSloped, int32_t height, ImageId imageTemplate,
    const PathRailingsDescriptor& railings);
