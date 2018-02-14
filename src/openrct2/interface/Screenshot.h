#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#pragma once

#include "../common.h"

struct rct_drawpixelinfo;

extern uint8 gScreenshotCountdown;

struct ScreenshotOptions
{
    sint32 weather     = 0;
    bool hide_guests   = false;
    bool hide_sprites  = false;
    bool clear_grass   = false;
    bool mowed_grass   = false;
    bool water_plants  = false;
    bool fix_vandalism = false;
    bool remove_litter = false;
    bool tidy_up_park  = false;
};

void screenshot_check();
sint32 screenshot_dump();
sint32 screenshot_dump_png(rct_drawpixelinfo *dpi);
sint32 screenshot_dump_png_32bpp(sint32 width, sint32 height, const void *pixels);

void screenshot_giant();
sint32 cmdline_for_screenshot(const char * * argv, sint32 argc, ScreenshotOptions * options);
sint32 cmdline_for_gfxbench(const char **argv, sint32 argc);

