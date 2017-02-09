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

#include "../game.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../interface/themes.h"
#include "../util/util.h"

#define WW 400
#define WH 100

enum WINDOW_OPEN_LINK_PROMPT_WIDGET_IDX
{
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_OPEN,
	WIDX_CANCEL
};

char open_link_prompt_url[1024];

static rct_widget window_open_link_prompt_widgets[] = {
	{ WWT_FRAME,			0, 0,				WW - 1,			0,			WH - 1, STR_NONE,					STR_NONE },
	{ WWT_CAPTION,			0, 1,				WW - 2,			1,			14,		STR_OPEN_LINK_PROMPT_TITLE,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0, WW - 13,			WW - 3,			2,			13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },
	{ WWT_DROPDOWN_BUTTON,	0, 10,				(WW / 2) - 5,	WH - 20,	WH - 9, STR_OPEN,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0, (WW / 2) + 5,	WW - 11,		WH - 20,	WH - 9, STR_SAVE_PROMPT_CANCEL,		STR_NONE },
	{ WIDGETS_END }
};

static void window_open_link_prompt_mouseup(rct_window *w, sint32 widgetIndex);
static void window_open_link_prompt_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_open_link_prompt_events = {
	NULL,
	window_open_link_prompt_mouseup,
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
	NULL,
	window_open_link_prompt_paint,
	NULL
};

void window_open_link_prompt_open(const char * url) 
{
	safe_strcpy(open_link_prompt_url, url, sizeof(open_link_prompt_url));

	rct_window *w;
	w = window_bring_to_front_by_class(WC_OPEN_LINK_PROMPT);
	if (w != NULL)
		return;

	w = window_create_centred(WW, WH, &window_open_link_prompt_events, WC_OPEN_LINK_PROMPT, 0);
	w->widgets = window_open_link_prompt_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_CANCEL) | (1 << WIDX_OPEN);
	window_init_scroll_widgets(w);

	w->colours[0] = COLOUR_LIGHT_BLUE;
	w->colours[1] = COLOUR_LIGHT_BLUE;
	w->colours[2] = COLOUR_LIGHT_BLUE;
}

static void window_open_link_prompt_mouseup(rct_window *w, sint32 widgetIndex) 
{
	switch (widgetIndex)
	{
	case WIDX_OPEN:
		platform_open_browser(open_link_prompt_url);
	case WIDX_CANCEL:
	case WIDX_CLOSE:
		window_close(w);
		break;
	}
}

static void window_open_link_prompt_paint(rct_window *w, rct_drawpixelinfo *dpi) 
{
	window_draw_widgets(w, dpi);

	sint32 x = w->x + WW / 2;
	sint32 y = w->y + (WH / 2) - 8;

	set_format_arg(0, const char *, open_link_prompt_url);
	gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, WW - 4, STR_OPEN_LINK_PROMPT_DESCRIPTION, w->colours[2]);
}
