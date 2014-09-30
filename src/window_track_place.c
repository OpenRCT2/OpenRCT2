/*****************************************************************************
 * Copyright (c) 2014 Ted John
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
#include "string_ids.h"
#include "viewport.h"
#include "widget.h"
#include "window.h"

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_ROTATE,
	WIDX_MIRROR,
	WIDX_SELECT_DIFFERENT_DESIGN,
	WIDX_PRICE
};

static rct_widget window_track_place_widgets[] = {
	{ WWT_FRAME,			0,	0,		199,	0,		123,	0xFFFFFFFF,						STR_NONE									},
	{ WWT_CAPTION,			0,	1,		198,	1,		14,		3155,							STR_WINDOW_TITLE_TIP						},
	{ WWT_CLOSEBOX,			0,	187,	197,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP						},
	{ WWT_FLATBTN,			0,	173,	196,	83,		106,	SPR_ROTATE_ARROW,				STR_ROTATE_90_TIP							},
	{ WWT_FLATBTN,			0,	173,	196,	59,		82,		5170,							STR_MIRROR_IMAGE_TIP						},
	{ WWT_DROPDOWN_BUTTON,	0,	4,		195,	109,	120,	STR_SELECT_A_DIFFERENT_DESIGN,	STR_GO_BACK_TO_DESIGN_SELECTION_WINDOW_TIP	},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,						STR_NONE									},
	{ WIDGETS_END },
};

static void window_track_place_emptysub() { }
static void window_track_place_close();
static void window_track_place_mouseup();
static void window_track_place_update(rct_window *w);
static void window_track_place_toolupdate();
static void window_track_place_tooldown();
static void window_track_place_toolabort();
static void window_track_place_unknown14();
static void window_track_place_paint();

static void* window_track_place_events[] = {
	window_track_place_close,
	window_track_place_mouseup,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_update,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_toolupdate,
	window_track_place_tooldown,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_toolabort,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_unknown14,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_emptysub,
	window_track_place_paint,
	window_track_place_emptysub
};

/**
 *
 *  rct2: 0x006CFCA0
 */
void window_track_place_open()
{
	rct_window *w;

	window_close_construction_windows();
	RCT2_GLOBAL(0x00F44168, void*) = rct2_malloc(13104);
	RCT2_CALLPROC_EBPSAFE(0x006D182E);

	w = window_create(0, 29, 200, 124, (uint32*)window_track_place_events, WC_TRACK_DESIGN_PLACE, 0);
	w->widgets = window_track_place_widgets;
	w->enabled_widgets = 4 | 8 | 0x10 | 0x20;
	window_init_scroll_widgets(w);
	w->colours[0] = 24;
	w->colours[1] = 24;
	w->colours[2] = 24;
	tool_set(w, 6, 12);
	RCT2_GLOBAL(0x009DE518, uint32) |= 6;
	window_push_others_right(w);
	show_gridlines();
	RCT2_GLOBAL(0x00F440D9, uint32) |= 0x80000000;
	RCT2_GLOBAL(0x00F440DD, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x00F440AE, uint8) = (-RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)) & 3;
	RCT2_CALLPROC_EBPSAFE(0x006D1845);
}

/**
 *
 *  rct2: 0x006D0119
 */
static void window_track_place_close()
{
	RCT2_CALLPROC_EBPSAFE(0x006D017F);
	viewport_set_visibility(0);
	RCT2_CALLPROC_EBPSAFE(0x0068AB1B);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~6;
	hide_gridlines();
	rct2_free(RCT2_GLOBAL(0x00F44168, void*));
}

/**
 *
 *  rct2: 0x006CFEAC
 */
static void window_track_place_mouseup()
{
	rct_window *w;
	short widgetIndex;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_ROTATE:
		RCT2_CALLPROC_EBPSAFE(0x006D017F);
		RCT2_GLOBAL(0x00F440AE, uint16) = (RCT2_GLOBAL(0x00F440AE, uint16) + 1) & 3;
		window_invalidate(w);
		RCT2_GLOBAL(0x00F440DD, uint16) = 0xFFFF;
		RCT2_CALLPROC_EBPSAFE(0x006D1845);
		break;
	case WIDX_MIRROR:
		RCT2_CALLPROC_EBPSAFE(0x006D2436);
		RCT2_GLOBAL(0x00F440AE, uint16) = (-RCT2_GLOBAL(0x00F440AE, uint16)) & 3;
		window_invalidate(w);
		RCT2_GLOBAL(0x00F440DD, uint16) = 0xFFFF;
		RCT2_CALLPROC_EBPSAFE(0x006D1845);
		break;
	case WIDX_SELECT_DIFFERENT_DESIGN:
		window_close(w);
		window_track_list_open(_window_track_list_item);
		break;
	}
}

/**
 *
 *  rct2: 0x006CFCA0
 */
static void window_track_place_update(rct_window *w)
{
	if (!(RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)))
		if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) != WC_TRACK_DESIGN_PLACE)
			window_close(w);
}

/**
 *
 *  rct2: 0x006CFF2D
 */
static void window_track_place_toolupdate()
{
	rct_window *w;
	short widgetIndex, x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	RCT2_CALLPROC_X(0x006CFF2D, x, y, 0, widgetIndex, (int)w, 0, 0);
}

/**
 *
 *  rct2: 0x006CFF34
 */
static void window_track_place_tooldown()
{
	rct_window *w;
	short widgetIndex, x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	RCT2_CALLPROC_X(0x006CFF34, x, y, 0, widgetIndex, (int)w, 0, 0);
}

/**
 *
 *  rct2: 0x006D015C
 */
static void window_track_place_toolabort()
{
	RCT2_CALLPROC_EBPSAFE(0x006D017F);
}

/**
 *
 *  rct2: 0x006CFF01
 */
static void window_track_place_unknown14()
{
	RCT2_CALLPROC_EBPSAFE(0x006D1845);
}

/**
 *
 *  rct2: 0x006CFD9D
 */
static void window_track_place_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi, *clippedDpi;
	rct_g1_element tmpElement, *subsituteElement, *g1Elements = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element);
	
	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	// Draw mini tile preview
	clippedDpi = clip_drawpixelinfo(dpi, w->x + 4, 168, w->y + 18, 78);
	if (clippedDpi != NULL) {
		subsituteElement = &g1Elements[0];
		tmpElement = *subsituteElement;
		subsituteElement->offset = RCT2_GLOBAL(0x00F44168, uint8*);
		subsituteElement->width = 168;
		subsituteElement->height = 78;
		subsituteElement->x_offset = 0;
		subsituteElement->y_offset = 0;
		subsituteElement->flags = 0;
		gfx_draw_sprite(clippedDpi, 0, 0, 0, 0);
		*subsituteElement = tmpElement;
	}

	if (RCT2_GLOBAL(0x00F440D9, money32) == 0x80000000)
		return;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)
		return;

	gfx_draw_string_centred(dpi, STR_COST_LABEL, w->x + 88, w->y + 94, 0, (money32*)0x00F440D9);
}