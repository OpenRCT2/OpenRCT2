/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

constexpr const uint8_t NumVanillaWoodenSupportTypes = 49;

bool WoodenASupportsPaintSetup(
    PaintSession& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate);
bool WoodenBSupportsPaintSetup(
    PaintSession& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate);
bool WoodenASupportsPaintSetup(
    PaintSession& session, int32_t supportType, int32_t special, int32_t height, uint32_t imageColourFlags);
bool WoodenBSupportsPaintSetup(
    PaintSession& session, int32_t supportType, int32_t special, int32_t height, uint32_t imageColourFlags);
bool MetalASupportsPaintSetup(
    PaintSession& session, uint8_t supportType, uint8_t segment, int32_t special, int32_t height, ImageId imageColourFlags);
bool MetalBSupportsPaintSetup(
    PaintSession& session, uint8_t supportType, uint8_t segment, int32_t special, int32_t height, ImageId imageColourFlags);
bool PathASupportsPaintSetup(
    PaintSession& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate,
    const FootpathPaintInfo& pathPaintInfo, bool* underground);
bool PathBSupportsPaintSetup(
    PaintSession& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate,
    const FootpathPaintInfo& pathPaintInfo);

// There are 13 types of metal supports. A graphic showing all of them is available here:
// https://cloud.githubusercontent.com/assets/737603/19420485/7eaba28e-93ec-11e6-83cb-03190accc094.png
enum : uint8_t
{
    MetalSupportsTubes,         // Used by the steel twister, looping rc, and other rides
    MetalSupportsFork,          // Used by the junior RC and other rides
    MetalSupportsForkAlt,       // Rotated version of MetalSupportsFork
    MetalSupportsBoxed,         // Used by the vertical roller coasters, the log flume and other rides
    MetalSupportsStick,         // Used by the Steeplechase
    MetalSupportsStickAlt,      // No visible difference from MetalSupportsStick, also used by the Steeplechase
    MetalSupportsThickCentered, // Every THICK type seems to be used for the Looping Roller Coaster's
    MetalSupportsThick,         // loop, and only for that specific part.
    MetalSupportsThickAlt,
    MetalSupportsThickAltCentered,
    MetalSupportsTruss,         // Used by the chairlift
    MetalSupportsTubesInverted, // Used by inverted rcs like the flying, lay-down, compact inverted. Mostly the same as
                                // MetalSupportsTubes, but with a thinner crossbeam.
    MetalSupportsBoxedCoated    // Does not seem to be used in RCT2, but it was used in RCT1 for one of the path support types.
};

enum
{
    SupportsSlope5 = 1 << 5
};
