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
#include "sprites.h"
#include "strings.h"
#include "widget.h"
#include "window.h"

static enum WINDOW_MAP_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
};

static rct_widget window_map_widgets[] = {
	{ WWT_FRAME,			0,	0,		244,	0,		258,	STR_NONE,		STR_NONE },
	{ WWT_CAPTION,			0,	1,		243,	1,		14,		STR_MAP,		STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	232,	242,	2,		13,		STR_CLOSE_X,	STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		244,	43,		257,	STR_NONE,		STR_NONE },
	{ WWT_COLORBTN,			1,	3,		33,		17,		43,		0x02000144E,	STR_SHOW_PEOPLE_ON_MAP_TIP },
	{ WWT_COLORBTN,			1,	34,		64,		17,		43,		0x02000144E,	STR_SHOW_RIDES_STALLS_ON_MAP_TIP },
	{ WWT_SCROLL,			1,	3,		241,	46,		225,	0x3,			STR_NONE },
	{ WWT_SPINNER,			1,	104,	198,	229,	240,	0xC8C,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	230,	234,	STR_NUMERIC_UP,	STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	235,	239,	STR_NUMERIC_DOWN, STR_NONE },
	{ WWT_FLATBTN,			1,	4,		27,		1,		24,		SPR_BUY_LAND_RIGHTS, STR_SELECT_PARK_OWNED_LAND_TIP },
	{ WWT_FLATBTN,			1,	4,		27,		1,		24,		SPR_BUY_CONSTRUCTION_RIGHTS, STR_BUILD_PARK_ENTRANCE_TIP },
	{ WWT_FLATBTN,			1,	28,		51,		1,		24,		STR_NONE,		STR_SET_STARTING_POSITIONS_TIP },
	{ WWT_IMGBTN,			1,	4,		47,		17,		48,		SPR_LAND_TOOL_SIZE_0, STR_NONE },
	{ WWT_TRNBTN,			1,	5,		20,		18,		33,		0x02000157B,	STR_ADJUST_SMALLER_LAND_TIP },
	{ WWT_TRNBTN,			1,	31,		46,		32,		47,		0x02000157D,	STR_ADJUST_LARGER_LAND_TIP },
	{ WWT_CHECKBOX,			1,	58,		241,	197,	208,	STR_LAND_OWNED,	STR_SET_LAND_TO_BE_OWNED_TIP },
	{ WWT_CHECKBOX,			1,	58,		241,	197,	208,	STR_CONSTRUCTION_RIGHTS_OWNED, STR_SET_CONSTRUCTION_RIGHTS_TO_BE_OWNED_TIP },
	{ WWT_CHECKBOX,			1,	58,		241,	197,	208,	STR_LAND_SALE,	STR_SET_LAND_TO_BE_AVAILABLE_TIP },
	{ WWT_CHECKBOX,			1,	58,		231,	197,	208,	STR_CONSTRUCTION_RIGHTS_SALE, STR_SET_CONSTRUCTION_RIGHTS_TO_BE_AVAILABLE_TIP },
	{ WWT_FLATBTN,			1,	218,	241,	45,		68,		SPR_ROTATE_ARROW, STR_ROTATE_OBJECTS_90 },
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