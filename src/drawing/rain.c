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

typedef void(*draw_rain_func)(int left, int top, int width, int height);

static void draw_light_rain(int left, int top, int width, int height);
static void draw_heavy_rain(int left, int top, int width, int height);

/**
 *
 *  rct2: 0x009AC058
 */
const draw_rain_func draw_rain_function[] = {
	NULL,
	&draw_light_rain,
	&draw_heavy_rain
};

/**
 *
 *  rct2: 0x00684383
 */
static void call_draw_rain_func(rct_window* w, short left, short right, short top, short bottom, uint32 draw_rain_func)
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

	draw_rain_function[draw_rain_func](left, top, width, height);
}

/**
 *
 *  rct2: 0x006842AF
 *  From 0x00684383 on: split into call_draw_rain_func
 */
static void draw_rain_window(rct_window* original_w, short left, short right, short top, short bottom, uint32 draw_rain_func)
{
	rct_window* newWindow = gWindowNextSlot;

	rct_window* w = original_w + 1; // Start from second window
	for (; ; w++) {
		if (w >= newWindow) {
			// Loop ended, draw rain for original_w
			call_draw_rain_func(original_w, left, right, top, bottom, draw_rain_func);
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

		draw_rain_window(original_w, left, w->x, top, bottom, draw_rain_func);

		left = w->x;
		draw_rain_window(original_w, left, right, top, bottom, draw_rain_func);
		return;
	}

	sint16 w_right = RCT_WINDOW_RIGHT(w);
	if (right > w_right) {
		draw_rain_window(original_w, left, w_right, top, bottom, draw_rain_func);

		left = w_right;
		draw_rain_window(original_w, left, right, top, bottom, draw_rain_func);
		return;
	}

	if (top < w->y) {
		draw_rain_window(original_w, left, right, top, w->y, draw_rain_func);

		top = w->y;
		draw_rain_window(original_w, left, right, top, bottom, draw_rain_func);
		return;
	}

	sint16 w_bottom = RCT_WINDOW_BOTTOM(w);
	if (bottom > w_bottom) {
		draw_rain_window(original_w, left, right, top, w_bottom, draw_rain_func);

		top = w_bottom;
		draw_rain_window(original_w, left, right, top, bottom, draw_rain_func);
		return;
	}
}

/**
 *
 *  rct2: 0x00684266
 */
static void draw_rain_animation(uint32 draw_rain_func)
{
	rct_drawpixelinfo *screenDPI = &gScreenDPI;
	short left = screenDPI->x;
	short right = left + screenDPI->width;
	short top = screenDPI->y;
	short bottom = top + screenDPI->height;

	rct_window* newWindow = gWindowNextSlot;

	for (rct_window* w = g_window_list; w < newWindow; w++) {
		draw_rain_window(w, left, right, top, bottom, draw_rain_func);
	}
}

/**
 *
 *  rct2: 0x00684218
 */
void update_rain_animation()
{
	// Get rain draw function and draw rain
	uint32 draw_rain_func = gClimateCurrentRainLevel;
	if (draw_rain_func > 0 && !(RCT2_GLOBAL(0x009DEA6F, uint8) & 1))
		draw_rain_animation(draw_rain_func);
}

/**
 *
 *  rct2: 0x00684114
 */
static void draw_light_rain(int left, int top, int width, int height)
{
	int x_start = -(int)gScenarioTicks + 8;
	int y_start = (gScenarioTicks * 3) + 7;
	y_start = -y_start;

	x_start += left;
	y_start += top;

	gfx_draw_rain(left, top, width, height, x_start, y_start);

	x_start = -(int)gScenarioTicks + 0x18;
	y_start = (gScenarioTicks * 4) + 0x0D;
	y_start = -y_start;

	x_start += left;
	y_start += top;
	gfx_draw_rain(left, top, width, height, x_start, y_start);
}

/**
 *
 *  rct2: 0x0068416D
 */
static void draw_heavy_rain(int left, int top, int width, int height)
{
	int x_start = -(int)gScenarioTicks;
	int y_start = gScenarioTicks * 5;
	y_start = -y_start;

	x_start += left;
	y_start += top;

	gfx_draw_rain(left, top, width, height, x_start, y_start);

	x_start = -(int)gScenarioTicks + 0x10;
	y_start = (gScenarioTicks * 6) + 5;
	y_start = -y_start;

	x_start += left;
	y_start += top;

	gfx_draw_rain(left, top, width, height, x_start, y_start);

	x_start = -(int)gScenarioTicks + 8;
	y_start = (gScenarioTicks * 3) + 7;
	y_start = -y_start;

	x_start += left;
	y_start += top;

	gfx_draw_rain(left, top, width, height, x_start, y_start);

	x_start = -(int)gScenarioTicks + 0x18;
	y_start = (gScenarioTicks * 4) + 0x0D;
	y_start = -y_start;

	x_start += left;
	y_start += top;

	gfx_draw_rain(left, top, width, height, x_start, y_start);
}
