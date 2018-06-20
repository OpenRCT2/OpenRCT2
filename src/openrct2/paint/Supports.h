/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _DRAWING_SUPPORTS_H_
#define _DRAWING_SUPPORTS_H_

#include "../common.h"
#include "../world/Footpath.h"

bool wooden_a_supports_paint_setup(paint_session * session, int32_t supportType, int32_t special, int32_t height, uint32_t imageColourFlags, bool* underground);
bool wooden_b_supports_paint_setup(paint_session * session, int32_t supportType, int32_t special, int32_t height, uint32_t imageColourFlags, bool* underground);
bool metal_a_supports_paint_setup(paint_session * session, uint8_t supportType, uint8_t segment, int32_t special, int32_t height, uint32_t imageColourFlags);
bool metal_b_supports_paint_setup(paint_session * session, uint8_t supportType, uint8_t segment, int32_t special, int32_t height, uint32_t imageColourFlags);
bool path_a_supports_paint_setup(paint_session * session, int32_t supportType, int32_t special, int32_t height, uint32_t imageColourFlags, rct_footpath_entry * pathEntry, bool * underground);
bool path_b_supports_paint_setup(paint_session * session, int32_t supportType, int32_t special, int32_t height, uint32_t imageColourFlags, rct_footpath_entry * pathEntry);

// There are 13 types of metal supports. A graphic showing all of them is available here: https://cloud.githubusercontent.com/assets/737603/19420485/7eaba28e-93ec-11e6-83cb-03190accc094.png
enum {
    METAL_SUPPORTS_TUBES,               // Used by the steel twister, looping rc, and other rides
    METAL_SUPPORTS_FORK,                // Used by the junior RC and other rides
    METAL_SUPPORTS_FORK_ALT,            // Rotated version of METAL_SUPPORTS_FORK
    METAL_SUPPORTS_BOXED,               // Used by the vertical roller coasters, the log flume and other rides
    METAL_SUPPORTS_STICK,               // Used by the Steeplechase
    METAL_SUPPORTS_STICK_ALT,           // No visible difference from METAL_SUPPORTS_STICK, also used by the Steeplechase
    METAL_SUPPORTS_THICK_CENTRED,       // Every THICK type seems to be used for the Looping Roller Coaster's
    METAL_SUPPORTS_THICK,               // loop, and only for that specific part.
    METAL_SUPPORTS_THICK_ALT,
    METAL_SUPPORTS_THICK_ALT_CENTRED,
    METAL_SUPPORTS_TRUSS,               // Used by the chairlift
    METAL_SUPPORTS_TUBES_INVERTED,      // Used by inverted rcs like the flying, lay-down, compact inverted. Mostly the same as METAL_SUPPORTS_TUBES, but with a thinner crossbeam.
    METAL_SUPPORTS_BOXED_COATED         // Does not seem to be used in RCT2, but it was used in RCT1 for one of the path support types.
};

#endif
