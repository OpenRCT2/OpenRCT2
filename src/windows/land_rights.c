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

#include "../addresses.h"
#include "../input.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/viewport.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../world/map.h"
#include "../game.h"
#include "../interface/themes.h"

const int MAX_LAND_RIGHTS_SIZE = 64;

enum WINDOW_WATER_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PREVIEW,
	WIDX_DECREMENT,
	WIDX_INCREMENT,
	WIDX_BUY_LAND_RIGHTS,
	WIDX_BUY_CONSTRUCTION_RIGHTS
};

static rct_widget window_land_rights_widgets[] = {
	{ WWT_FRAME,	0,	0,	97,	0,	93,	-1,											STR_NONE },							// panel / background
	{ WWT_CAPTION,	0,	1,	96,	1,	14,	5136,										STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,	0,	85,	95,	2,	13,	824,										STR_CLOSE_WINDOW_TIP },				// close x button
	{ WWT_IMGBTN,	0,	27,	70,	17,	48,	SPR_LAND_TOOL_SIZE_0,						STR_NONE },							// preview box
	{ WWT_TRNBTN,	2,	28,	43,	18,	33,	0x20000000 | SPR_LAND_TOOL_DECREASE,		5133 },								// decrement size
	{ WWT_TRNBTN,	2,	54,	69,	32,	47,	0x20000000 | SPR_LAND_TOOL_INCREASE,		5134 },								// increment size
	{ WWT_FLATBTN,	2,	22, 45, 53, 76, 0x20000000 | SPR_BUY_LAND_RIGHTS,			SPR_BUY_LAND_RIGHTS_TIP },			// land rights
	{ WWT_FLATBTN,	2,	52, 75, 53, 76, 0x20000000 | SPR_BUY_CONSTRUCTION_RIGHTS,	SPR_BUY_CONSTRUCTION_RIGHTS_TIP },	// construction rights
	{ WIDGETS_END },
};

static int window_land_rights_should_close();

static void window_land_rights_emptysub() { }
static void window_land_rights_close();
static void window_land_rights_mouseup();
static void window_land_rights_update();
static void window_land_rights_invalidate();
static void window_land_rights_paint();
static void window_land_rights_textinput();
static void window_land_rights_inputsize(rct_window *w);

static void* window_land_rights_events[] = {
	window_land_rights_close,
	window_land_rights_mouseup,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_update,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_textinput,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_emptysub,
	window_land_rights_invalidate,
	window_land_rights_paint,
	window_land_rights_emptysub
};

void window_land_rights_open()
{
	rct_window* window;

	// Check if window is already open
	if (window_find_by_class(WC_LAND_RIGHTS) != NULL)
		return;

	window = window_create(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - 98, 29, 98, 94, (uint32*)window_land_rights_events, WC_LAND_RIGHTS, 0);
	window->widgets = window_land_rights_widgets;
	window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_DECREMENT) | (1 << WIDX_INCREMENT) | (1 << WIDX_PREVIEW) |
		(1 << WIDX_BUY_LAND_RIGHTS) | (1 << WIDX_BUY_CONSTRUCTION_RIGHTS);
	window_init_scroll_widgets(window);
	window_push_others_below(window);

	LandRightsMode = true;
	window->pressed_widgets = (1 << WIDX_BUY_LAND_RIGHTS);

	RCT2_GLOBAL(RCT2_ADDRESS_WATER_RAISE_COST, uint32) = MONEY32_UNDEFINED;
	RCT2_GLOBAL(RCT2_ADDRESS_WATER_LOWER_COST, uint32) = MONEY32_UNDEFINED;

	show_land_rights();
}

static void window_land_rights_close()
{
	//if (LandRightsMode)
	//	hide_land_rights();
	//else
	//	hide_construction_rights();
	// If the tool wasn't changed, turn tool off
	if (!window_land_rights_should_close())
		tool_cancel();
}

static void window_land_rights_mouseup()
{
	rct_window *w;
	int limit;
	short widgetIndex;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_DECREMENT:
		// Decrement land tool size
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16)--;
		limit = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) < limit)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = limit;

		// Invalidate the window
		window_invalidate(w);
		break;
	case WIDX_INCREMENT:
		// Increment land tool size
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16)++;

		limit = MAX_LAND_RIGHTS_SIZE;
		
		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) > limit)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = limit;

		// Invalidate the window
		window_invalidate(w);
		break;
	case WIDX_PREVIEW:
		window_land_rights_inputsize(w);
		break;
	case WIDX_BUY_LAND_RIGHTS:
		if (!LandRightsMode) {
			LandRightsMode = true;
			hide_construction_rights();
			show_land_rights();
			window_invalidate(w);
		}
		break;
	case WIDX_BUY_CONSTRUCTION_RIGHTS:
		if (LandRightsMode) {
			LandRightsMode = false;
			hide_land_rights();
			show_construction_rights();
			window_invalidate(w);
		}
		break;
	}
}

static void window_land_rights_textinput()
{
	uint8 result;
	short widgetIndex;
	rct_window *w;
	char *text;
	int size;
	char* end;

	window_textinput_get_registers(w, widgetIndex, result, text);

	if (widgetIndex != WIDX_PREVIEW || !result)
		return;

	size = strtol(text, &end, 10);
	if (*end == '\0') {
		if (size < 1) size = 1;
		if (size > MAX_LAND_RIGHTS_SIZE) size = MAX_LAND_RIGHTS_SIZE;
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = size;
		window_invalidate(w);
	}
}

static void window_land_rights_inputsize(rct_window *w)
{
	((uint16*)TextInputDescriptionArgs)[0] = 1;
	((uint16*)TextInputDescriptionArgs)[1] = MAX_LAND_RIGHTS_SIZE;
	window_text_input_open(w, WIDX_PREVIEW, 5128, 5129, STR_NONE, STR_NONE, 3);
}

static void window_land_rights_update(rct_window *w)
{
	// Close window if another tool is open
	if (window_land_rights_should_close())
		window_close(w);
}

static void window_land_rights_invalidate()
{
	rct_window *w;

	window_get_register(w);
	colour_scheme_update(w);

	// Set the preview image button to be pressed down
	w->pressed_widgets |= (1 << WIDX_PREVIEW) | (1 << (LandRightsMode ? WIDX_BUY_LAND_RIGHTS : WIDX_BUY_CONSTRUCTION_RIGHTS));
	w->pressed_widgets &= ~(1 << (!LandRightsMode ? WIDX_BUY_LAND_RIGHTS : WIDX_BUY_CONSTRUCTION_RIGHTS));

	// Update the preview image
	window_land_rights_widgets[WIDX_PREVIEW].image = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) <= 7 ?
		SPR_LAND_TOOL_SIZE_0 + RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) :
		0xFFFFFFFF;
}

static void window_land_rights_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	int x, y;

	window_paint_get_registers(w, dpi);

	x = w->x + (window_land_rights_widgets[WIDX_PREVIEW].left + window_land_rights_widgets[WIDX_PREVIEW].right) / 2;
	y = w->y + (window_land_rights_widgets[WIDX_PREVIEW].top + window_land_rights_widgets[WIDX_PREVIEW].bottom) / 2;

	window_draw_widgets(w, dpi);
	// FEATURE larger land tool size support
	if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) > 7) {
		RCT2_GLOBAL(0x009BC677, char) = FORMAT_BLACK;
		RCT2_GLOBAL(0x009BC678, char) = FORMAT_COMMA16;
		RCT2_GLOBAL(0x009BC679, char) = 0;
		RCT2_GLOBAL(0x013CE952, sint16) = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16);
		gfx_draw_string_centred(dpi, 3165, x, y - 2, 0, (void*)0x013CE952);
	}
	y = w->y + window_land_rights_widgets[WIDX_PREVIEW].bottom + 5;

	// Draw cost amount
	x = (window_land_rights_widgets[WIDX_PREVIEW].left + window_land_rights_widgets[WIDX_PREVIEW].right) / 2 + w->x;
	y = window_land_rights_widgets[WIDX_PREVIEW].bottom + w->y + 32;
	if (RCT2_GLOBAL(0x00F1AD62, uint32) != MONEY32_UNDEFINED && RCT2_GLOBAL(0x00F1AD62, uint32) != 0)
		gfx_draw_string_centred(dpi, 986, x, y, 0, (void*)0x00F1AD62);
}

static int window_land_rights_should_close()
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE))
		return 1;
	if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) != WC_PARK_INFORMATION)
		return 1;
	if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) != 14)
		return 1;
	return 0;
}
