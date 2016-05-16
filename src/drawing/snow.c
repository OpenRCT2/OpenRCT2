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

#include "../addresses.h"
#include "../interface/window.h"
#include "../world/climate.h"
#include "drawing.h"

typedef void(*draw_snow_func)(int left, int top, int width, int height);

static void draw_snow(int left, int top, int width, int height);

const draw_snow_func draw_snow_function[] = {
	NULL,
	&draw_snow
};

static void call_draw_snow_func(rct_window* w, short left, short right, short top, short bottom, uint32 draw_snow_func)
{
	rct_viewport* vp = w->viewport;
	if (vp == NULL) {
		return;
	}

	left = max(left, vp->x);
	right = min(right, vp->width);

	top = max(top, vp->y);
	bottom = min(bottom, vp->height);

	if (left >= right || top >= bottom) {
		return;
	}

	int width = right - left;
	int height = bottom - top;

	draw_snow_function[draw_snow_func](left, top, width, height);
}

static void draw_snow_window(rct_window* original_w, short left, short right, short top, short bottom, uint32 draw_snow_func)
{
	rct_window* newWindow = gWindowNextSlot;

	rct_window* w = original_w + 1; // Start from second window
	for (; ; w++) {
		if (w >= newWindow) {
			// Loop ended, draw snow for original_w
			call_draw_snow_func(original_w, left, right, top, bottom, draw_snow_func);
			return;
		}

		if (right <= w->x || bottom <= w->y) {
			continue;
		}

		if (RCT_WINDOW_RIGHT(w) <= left || RCT_WINDOW_BOTTOM(w) <= top) {
			continue;
		}

		if (left >= w->x) {
			break;
		}

		draw_snow_window(original_w, left, w->x, top, bottom, draw_snow_func);

		left = w->x;
		draw_snow_window(original_w, left, right, top, bottom, draw_snow_func);
		return;
	}

	sint16 w_right = RCT_WINDOW_RIGHT(w);
	if (right > w_right) {
		draw_snow_window(original_w, left, w_right, top, bottom, draw_snow_func);

		left = w_right;
		draw_snow_window(original_w, left, right, top, bottom, draw_snow_func);
		return;
	}

	if (top < w->y) {
		draw_snow_window(original_w, left, right, top, w->y, draw_snow_func);

		top = w->y;
		draw_snow_window(original_w, left, right, top, bottom, draw_snow_func);
		return;
	}

	sint16 w_bottom = RCT_WINDOW_BOTTOM(w);
	if (bottom > w_bottom) {
		draw_snow_window(original_w, left, right, top, w_bottom, draw_snow_func);

		top = w_bottom;
		draw_snow_window(original_w, left, right, top, bottom, draw_snow_func);
		return;
	}
}

static void draw_snow_animation(uint32 draw_snow_func)
{
	rct_drawpixelinfo *screenDPI = &gScreenDPI;
	short left = screenDPI->x;
	short right = left + screenDPI->width;
	short top = screenDPI->y;
	short bottom = top + screenDPI->height;

	rct_window* newWindow = gWindowNextSlot;

	for (rct_window* w = g_window_list; w < newWindow; w++) {
		draw_snow_window(w, left, right, top, bottom, draw_snow_func);
	}
}

void update_snow_animation()
{
	// Get snow draw function and draw snow
	uint32 draw_snow_func = gClimateCurrentSnowLevel;
	if (draw_snow_func > 0 && !(RCT2_GLOBAL(0x009DEA6F, uint8) & 1))
		draw_snow_animation(draw_snow_func);
}

static void draw_snow(int left, int top, int width, int height)
{
	int x_start = -(int)gScenarioTicks + 8;
	x_start = -(int)gScenarioTicks + 16;
	int y_start = gScenarioTicks * 3 + 7;
	y_start = gScenarioTicks * 1 + 7;
	y_start = -y_start;

	x_start += left;
	y_start += top;

	gfx_draw_snow(left, top, width, height, x_start, y_start);

	x_start = -(int)gScenarioTicks + 1;
	y_start = gScenarioTicks * 1 - 7;
	y_start = -y_start;

	x_start += left;
	y_start += top;
	gfx_draw_snow(left, top, width, height, x_start, y_start);
};
