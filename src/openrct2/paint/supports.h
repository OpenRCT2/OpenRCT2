#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _DRAWING_SUPPORTS_H_
#define _DRAWING_SUPPORTS_H_

#include "../common.h"
#include "../world/footpath.h"

#ifdef NO_RCT2
extern paint_struct * gWoodenSupportsPrependTo;
#else
#define gWoodenSupportsPrependTo		RCT2_GLOBAL(0x009DEA58, paint_struct *)
#endif

bool wooden_a_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, bool* underground);
bool wooden_b_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, bool* underground);
bool metal_a_supports_paint_setup(uint8 supportType, uint8 segment, int special, int height, uint32 imageColourFlags);
bool metal_b_supports_paint_setup(uint8 supportType, uint8 segment, int special, int height, uint32 imageColourFlags);
bool path_a_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, rct_footpath_entry * pathEntry, bool * underground);
bool path_b_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, rct_footpath_entry * pathEntry);

// There are 13 types of metal supports. A graphic showing all of them is available here: https://cloud.githubusercontent.com/assets/737603/19420485/7eaba28e-93ec-11e6-83cb-03190accc094.png
enum {
    METAL_SUPPORTS_TUBES,               // Used by the steel twister and other rides
    METAL_SUPPORTS_FORK,                // Used by the junior RC and other rides
    METAL_SUPPORTS_FORK_ALT,            // Rotated version of METAL_SUPPORTS_FORK
    METAL_SUPPORTS_BOXED,               // Used by the vertical roller coasters, the log flume and other rides
    METAL_SUPPORTS_STICK,               // Used by the Steeplechase (or perhaps the _ALT version is used instead)
    METAL_SUPPORTS_STICK_ALT,           // No visible difference from METAL_SUPPORTS_STICK
    METAL_SUPPORTS_THICK_CENTRED,       // Every THICK type seems to be used for the Looping Roller Coaster's
    METAL_SUPPORTS_THICK,               // loop, and only for that specific part.
    METAL_SUPPORTS_THICK_ALT,
    METAL_SUPPORTS_THICK_ALT_CENTRED,
    METAL_SUPPORTS_TRUSS,               // Used by the chairlift
    METAL_SUPPORTS_TUBES_ALT,           // Mostly the same as METAL_SUPPORTS_TUBES, but with a thinner crossbeam
    METAL_SUPPORTS_BOXED_COATED
};

#endif
