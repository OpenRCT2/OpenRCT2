/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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

