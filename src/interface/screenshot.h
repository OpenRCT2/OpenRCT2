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

#ifndef _SCREENSHOT_H_
#define _SCREENSHOT_H_

#include "../drawing/drawing.h"

extern uint8 gScreenshotCountdown;

void screenshot_check();
int screenshot_dump();
int screenshot_dump_png(rct_drawpixelinfo *dpi);
int screenshot_dump_png_32bpp(sint32 width, sint32 height, const void *pixels);

void screenshot_giant();
int cmdline_for_screenshot(const char **argv, int argc);

#endif
