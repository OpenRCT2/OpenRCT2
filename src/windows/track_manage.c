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
#include "../game.h"
#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../ride/track.h"
#include "../ride/track_design.h"
#include "../util/util.h"
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
	{ WWT_CAPTION,			0,	1,		248,	1,		14,		STR_DELETE_FILE,			STR_WINDOW_TITLE_TIP	},
	{ WWT_CLOSEBOX,			0,	237,	247,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP	},
	{ WWT_DROPDOWN_BUTTON,	0,	10,		119,	54,		65,		STR_TRACK_MANAGE_DELETE,	STR_NONE				},
	{ WWT_DROPDOWN_BUTTON,	0,	130,	239,	54,		65,		STR_CANCEL,					STR_NONE				},
	{ WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_track_manage_close(rct_window *w);
static void window_track_manage_mouseup(rct_window *w, int widgetIndex);
static void window_track_manage_textinput(rct_window *w, int widgetIndex, char *text);
static void window_track_manage_invalidate(rct_window *w);
static void window_track_manage_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_track_delete_prompt_mouseup(rct_window *w, int widgetIndex);
static void window_track_delete_prompt_invalidate(rct_window *w);
static void window_track_delete_prompt_paint(rct_window *w, rct_drawpixelinfo *dpi);

// 0x009940EC
static rct_window_event_list window_track_manage_events = {
	window_track_manage_close,
	window_track_manage_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_track_manage_textinput,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_track_manage_invalidate,
	window_track_manage_paint,
	NULL
};

// 0x0099415C
static rct_window_event_list window_track_delete_prompt_events = {
	NULL,
	window_track_delete_prompt_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_track_delete_prompt_invalidate,
	window_track_delete_prompt_paint,
	NULL
};

#pragma endregion

static track_design_file_ref *_trackDesignFileReference;

static void window_track_delete_prompt_open();

/**
 *
 *  rct2: 0x006D348F
 */
void window_track_manage_open(track_design_file_ref *tdFileRef)
{
	window_close_by_class(WC_MANAGE_TRACK_DESIGN);

	rct_window *w = window_create_centred(
		250,
		44,
		&window_track_manage_events,
		WC_MANAGE_TRACK_DESIGN,
		WF_STICK_TO_FRONT | WF_TRANSPARENT
	);
	w->widgets = window_track_manage_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_RENAME) |
		(1 << WIDX_DELETE);
	window_init_scroll_widgets(w);

	rct_window *trackDesignListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
	if (trackDesignListWindow != NULL) {
		trackDesignListWindow->track_list.var_484 |= 1;
	}

	utf8 *title = (utf8*)language_get_string(3155);
	format_string(title, STR_TRACK_LIST_NAME_FORMAT, &tdFileRef->name);

	_trackDesignFileReference = tdFileRef;
}

/**
 *
 *  rct2: 0x006D364C
 */
static void window_track_manage_close(rct_window *w)
{
	rct_window *trackDesignListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
	if (trackDesignListWindow != NULL) {
		trackDesignListWindow->track_list.var_484 &= ~1;
	}
}

/**
 *
 *  rct2: 0x006D3523
 */
static void window_track_manage_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_RENAME:
		window_text_input_raw_open(
			w,
			widgetIndex,
			STR_TRACK_DESIGN_RENAME_TITLE,
			STR_TRACK_DESIGN_RENAME_DESC,
			_trackDesignFileReference->name,
			127
		);
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
static void window_track_manage_textinput(rct_window *w, int widgetIndex, char *text)
{
	if (widgetIndex != WIDX_RENAME || text == NULL) {
		return;
	}

	// if (track_rename(text)) {
	// 	window_close_by_class(WC_TRACK_DELETE_PROMPT);
	// 	window_close(w);
	// } else {
	// 	window_error_open(STR_CANT_RENAME_TRACK_DESIGN, gGameCommandErrorText);
	// }
}

static void window_track_manage_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}

/**
 *
 *  rct2: 0x006D3523
 */
static void window_track_manage_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
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
		max(28, (gScreenWidth - 250) / 2),
		(gScreenHeight - 44) / 2,
		250,
		74,
		&window_track_delete_prompt_events,
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
}

/**
 *
 *  rct2: 0x006D3823
 */
static void window_track_delete_prompt_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
	case WIDX_PROMPT_CANCEL:
		window_close(w);
		break;
	case WIDX_PROMPT_DELETE:
		window_close(w);
		if (track_delete()) {
			window_close_by_class(WC_MANAGE_TRACK_DESIGN);
		} else {
			window_error_open(STR_CANT_DELETE_TRACK_DESIGN, gGameCommandErrorText);
		}
		break;
	}
}

static void window_track_delete_prompt_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}

/**
 *
 *  rct2: 0x006D37EE
 */
static void window_track_delete_prompt_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	gfx_draw_string_centred_wrapped(
		dpi,
		&_trackDesignFileReference->name,
		w->x + 125,
		w->y + 28,
		246,
		STR_ARE_YOU_SURE_YOU_WANT_TO_PERMANENTLY_DELETE_TRACK,
		0
	);
}
