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
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../ride/track.h"
#include "error.h"

#pragma region Widgets

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_RENAME,
	WIDX_DELETE,

	WIDX_PROMPT_DELETE = 3,
	WIDX_PROMPT_CANCEL = 4,
};

static rct_widget window_track_manage_widgets[] = {
	{ WWT_FRAME,			0,	0,		249,	0,		43,		STR_NONE,					STR_NONE				},
	{ WWT_CAPTION,			0,	1,		248,	1,		14,		3155,						STR_WINDOW_TITLE_TIP	},
	{ WWT_CLOSEBOX,			0,	237,	247,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP	},
	{ WWT_DROPDOWN_BUTTON,	0,	10,		119,	24,		35,		STR_TRACK_MANAGE_RENAME,	STR_NONE				},
	{ WWT_DROPDOWN_BUTTON,	0,	130,	239,	24,		35,		STR_TRACK_MANAGE_DELETE,	STR_NONE				},
	{ WIDGETS_END }
};

static rct_widget window_track_delete_prompt_widgets[] = {
	{ WWT_FRAME,			0,	0,		249,	0,		73,		STR_NONE,					STR_NONE				},
	{ WWT_CAPTION,			0,	1,		248,	1,		14,		3356,						STR_WINDOW_TITLE_TIP	},
	{ WWT_CLOSEBOX,			0,	237,	247,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP	},
	{ WWT_DROPDOWN_BUTTON,	0,	10,		119,	54,		65,		3349,						STR_NONE				},
	{ WWT_DROPDOWN_BUTTON,	0,	130,	239,	54,		65,		972,						STR_NONE				},
	{ WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_track_manage_emptysub() { }

static void window_track_manage_close();
static void window_track_manage_mouseup();
static void window_track_manage_textinput();
static void window_track_manage_paint();

static void window_track_delete_prompt_emptysub() { }

static void window_track_delete_prompt_mouseup();
static void window_track_delete_prompt_paint();

// 0x009940EC
static void* window_track_manage_events[] = {
	window_track_manage_close,
	window_track_manage_mouseup,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_textinput,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_emptysub,
	window_track_manage_paint,
	window_track_manage_emptysub
};

// 0x0099415C
static void* window_track_delete_prompt_events[] = {
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_mouseup,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_emptysub,
	window_track_delete_prompt_paint,
	window_track_delete_prompt_emptysub
};

#pragma endregion

static void window_track_delete_prompt_open();

/**
 *
 *  rct2: 0x006D348F
 */
void window_track_manage_open()
{
	rct_window *w, *trackDesignListWindow;

	window_close_by_class(WC_MANAGE_TRACK_DESIGN);

	w = window_create_centred(
		250,
		44,
		(uint32*)window_track_manage_events,
		WC_MANAGE_TRACK_DESIGN,
		WF_STICK_TO_FRONT
	);
	w->widgets = window_track_manage_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_RENAME) |
		(1 << WIDX_DELETE);
	window_init_scroll_widgets(w);
	w->flags |= WF_TRANSPARENT;
	w->colours[0] = 1;
	w->colours[1] = 1;
	w->colours[2] = 1;

	trackDesignListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
	if (trackDesignListWindow != NULL)
		trackDesignListWindow->track_list.var_484 |= 1;
}

/**
 *
 *  rct2: 0x006D364C
 */
static void window_track_manage_close()
{
	rct_window *w;

	w = window_find_by_class(WC_TRACK_DESIGN_LIST);
	if (w != NULL)
		w->track_list.var_484 &= ~1;
}

/**
 *
 *  rct2: 0x006D3523
 */
static void window_track_manage_mouseup()
{
	uint8 *trackDesignList = (uint8*)0x00F441EC;
	rct_window *w, *trackDesignListWindow;
	short widgetIndex;
	char *dst, *src;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_RENAME:
		trackDesignListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
		if (trackDesignListWindow != NULL) {
			src = &trackDesignList[trackDesignListWindow->track_list.var_482 * 128];
			dst = (char*)0x009BC677;
			while (*src != 0 && *src != '.')
				*dst++ = *src++;
			*dst = 0;
			window_text_input_open(w, widgetIndex, 3350, 3351, 3165, 0, 127);
		}
		break;
	case WIDX_DELETE:
		window_track_delete_prompt_open();
		break;
	}
}

/**
 *
 *  rct2: 0x006D3523
 */
static void window_track_manage_textinput()
{
	rct_window *w;
	short widgetIndex;
	uint8 result;
	char *text;

	window_textinput_get_registers(w, widgetIndex, result, text);

	if (widgetIndex != WIDX_RENAME || !result)
		return;

	if (track_rename(text)) {
		window_close_by_class(WC_TRACK_DELETE_PROMPT);
		window_close(w);
	} else {
		window_error_open(STR_CANT_RENAME_TRACK_DESIGN, RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16));
	}
}

/**
 *
 *  rct2: 0x006D3523
 */
static void window_track_manage_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
}

/**
 *
 *  rct2: 0x006D35CD
 */
static void window_track_delete_prompt_open()
{
	rct_window *w;

	window_close_by_class(WC_TRACK_DELETE_PROMPT);

	w = window_create(
		max(28, (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) - 250) / 2),
		(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) - 44) / 2,
		250,
		74,
		(uint32*)window_track_delete_prompt_events,
		WC_TRACK_DELETE_PROMPT,
		WF_STICK_TO_FRONT
	);
	w->widgets = window_track_delete_prompt_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_RENAME) |
		(1 << WIDX_DELETE);
	window_init_scroll_widgets(w);
	w->flags |= WF_TRANSPARENT;
	w->colours[0] = 26;
	w->colours[1] = 26;
	w->colours[2] = 26;
}

/**
 *
 *  rct2: 0x006D3823
 */
static void window_track_delete_prompt_mouseup()
{
	rct_window *w;
	short widgetIndex;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
	case WIDX_PROMPT_CANCEL:
		window_close(w);
		break;
	case WIDX_PROMPT_DELETE:
		window_close(w);
		if (track_delete())
			window_close_by_class(WC_MANAGE_TRACK_DESIGN);
		else
			window_error_open(STR_CANT_DELETE_TRACK_DESIGN, RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16));
		break;
	}
}

/**
 *
 *  rct2: 0x006D37EE
 */
static void window_track_delete_prompt_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_string_id stringId;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	stringId = 3155;
	gfx_draw_string_centred_wrapped(
		dpi, &stringId, w->x + 125, w->y + 28, 246, STR_ARE_YOU_SURE_YOU_WANT_TO_PERMANENTLY_DELETE_TRACK, 0
	);
}