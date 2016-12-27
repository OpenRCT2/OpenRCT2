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

#endif
