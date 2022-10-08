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
    paint_session& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate);
bool WoodenBSupportsPaintSetup(
    paint_session& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate);
bool MetalASupportsPaintSetup(
    paint_session& session, uint8_t supportType, uint8_t segment, int32_t special, int32_t height, ImageId imageColourFlags);
bool MetalBSupportsPaintSetup(
    paint_session& session, uint8_t supportType, uint8_t segment, int32_t special, int32_t height, ImageId imageColourFlags);
bool path_a_supports_paint_setup(
    paint_session& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate,
    const FootpathPaintInfo& pathPaintInfo, bool* underground);
bool path_b_supports_paint_setup(
    paint_session& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate,
    const FootpathPaintInfo& pathPaintInfo);

// There are 13 types of metal supports. A graphic showing all of them is available here:
// https://cloud.githubusercontent.com/assets/737603/19420485/7eaba28e-93ec-11e6-83cb-03190accc094.png
enum : uint8_t
{
    METAL_SUPPORTS_TUBES,         // Used by the steel twister, looping rc, and other rides
    METAL_SUPPORTS_FORK,          // Used by the junior RC and other rides
    METAL_SUPPORTS_FORK_ALT,      // Rotated version of METAL_SUPPORTS_FORK
    METAL_SUPPORTS_BOXED,         // Used by the vertical roller coasters, the log flume and other rides
    METAL_SUPPORTS_STICK,         // Used by the Steeplechase
    METAL_SUPPORTS_STICK_ALT,     // No visible difference from METAL_SUPPORTS_STICK, also used by the Steeplechase
    METAL_SUPPORTS_THICK_CENTRED, // Every THICK type seems to be used for the Looping Roller Coaster's
    METAL_SUPPORTS_THICK,         // loop, and only for that specific part.
    METAL_SUPPORTS_THICK_ALT,
    METAL_SUPPORTS_THICK_ALT_CENTRED,
    METAL_SUPPORTS_TRUSS,          // Used by the chairlift
    METAL_SUPPORTS_TUBES_INVERTED, // Used by inverted rcs like the flying, lay-down, compact inverted. Mostly the same as
                                   // METAL_SUPPORTS_TUBES, but with a thinner crossbeam.
    METAL_SUPPORTS_BOXED_COATED // Does not seem to be used in RCT2, but it was used in RCT1 for one of the path support types.
};

enum
{
    SUPPORTS_SLOPE_5 = 1 << 5
};
