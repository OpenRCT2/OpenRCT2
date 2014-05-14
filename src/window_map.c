/*****************************************************************************
 * Copyright (c) 2014 Ted John, Dennis Devriendt
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "addresses.h"
#include "strings.h"
#include "widget.h"
#include "window.h"

static enum WINDOW_MAP_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
};

static rct_widget window_map_widgets[] = {
	{ WWT_FRAME,			0,	0,		309,	0,		371,	STR_NONE,		STR_NONE },
	{ WWT_CAPTION,			0,	1,		308,	1,		14,		STR_OPTIONS,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	297,	307,	2,		13,		STR_CLOSE_X,	STR_CLOSE_WINDOW_TIP },
	{ WIDGETS_END },
};

static void window_map_emptysub() { }
static void window_map_close();
static void window_map_mouseup();
static void window_map_mousedown();
static void window_map_update();
static void window_map_scrollgetsize();
static void window_map_scrollmousedown();
static void window_map_invalidate();
static void window_map_paint();
static void window_map_scrollpaint();

static void* window_map_events[] = {
	window_map_close,
	window_map_mouseup,
	window_map_emptysub,
	window_map_mousedown,
	window_map_emptysub,
	window_map_emptysub,
	window_map_update,
	window_map_emptysub,
	window_map_emptysub,
	(void*)0x0068D093,
	(void*)0x0068D074,
	(void*)0x0068D088,
	window_map_emptysub,
	(void*)0x0068D055,
	window_map_emptysub,
	window_map_scrollgetsize,
	window_map_scrollmousedown,
	window_map_scrollmousedown,
	window_map_emptysub,
	window_map_emptysub,
	window_map_emptysub,
	window_map_emptysub,
	(void*)0x0068D140,
	window_map_emptysub,
	window_map_emptysub,
	window_map_invalidate,
	window_map_paint,
	window_map_scrollpaint
};

/**
*
*  rct2: 0x0068C88A
*/
void window_map_open()
{
	RCT2_CALLPROC_EBPSAFE(0x0068C88A);
	//rct_window* w;

	// Check if window is already open
	//w = window_bring_to_front_by_id(WC_OPTIONS, 0);
	//if (w != NULL)
	//	return;
}

/**
*
*  rct2: 0x0068D0F1
*/
static void window_map_close()
{
	RCT2_CALLPROC_EBPSAFE(0x0068D0F1);
}

/**
*
*  rct2: 0x006BAFCA
*/
static void window_map_mouseup()
{
	RCT2_CALLPROC_EBPSAFE(0x006BAFCA);
	/*short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	}*/
}

/**
*
*  rct2: 0x0068D040
*/
static void window_map_mousedown()
{
	RCT2_CALLPROC_EBPSAFE(0x0068D040);
}

/**
*
*  rct2: 0x0068D7FB
*/
static void window_map_update()
{
	RCT2_CALLPROC_EBPSAFE(0x0068D7FB);
}

/**
*
*  rct2: 0x0068D7CC
*/
static void window_map_scrollgetsize()
{
	RCT2_CALLPROC_EBPSAFE(0x0068D7CC);
}

/**
*
*  rct2: 0x0068D726
*/
static void window_map_scrollmousedown()
{
	RCT2_CALLPROC_EBPSAFE(0x0068D726);
}

/**
*
*  rct2: 0x0068CA8F
*/
static void window_map_invalidate()
{
	RCT2_CALLPROC_EBPSAFE(0x0068CA8F);
}

/**
*
*  rct2: 0x0068CDA9
*/
static void window_map_paint()
{
	RCT2_CALLPROC_EBPSAFE(0x0068CDA9);
	/*rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);*/
}

/**
*
*  rct2: 0x0068CF23
*/
static void window_map_scrollpaint()
{
	RCT2_CALLPROC_EBPSAFE(0x0068CF23);
}