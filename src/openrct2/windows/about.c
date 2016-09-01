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

#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/themes.h"
#include "../util/util.h"
#include "../openrct2.h"

#define WW 400
#define WH 350

enum
{
	WINDOW_ABOUT_PAGE_OPENRCT2,
	WINDOW_ABOUT_PAGE_RCT2,
};

enum WINDOW_ABOUT_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_ABOUT_OPENRCT2,
	WIDX_ABOUT_RCT2,
	WIDX_PAGE_BACKGROUND,

	WIDX_PAGE_START,

	//About OpenRCT2
	WIDX_CHANGELOG = WIDX_PAGE_START,
	WIDX_CONTRIBUTORS,
	WIDX_GITHUB,

	//About RCT2
	WIDX_MUSIC_CREDITS = WIDX_PAGE_START,
	WIDX_PUBLISHER_CREDITS
};


#define WIDGETS_MAIN \
	  WWT_FRAME,			0,	0,				WW - 1,			0,		WW - 71,	0xFFFFFFFF,							STR_NONE },				/* panel / background */ \
	{ WWT_CAPTION,			0,	1,				WW - 2,			1,		14,			STR_ABOUT,							STR_WINDOW_TITLE_TIP },	/* title bar */ \
	{ WWT_CLOSEBOX,			0,	WW - 13,		WW - 3,			2,		13,			STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP },	/* close x button */ \
	{ WWT_DROPDOWN_BUTTON,	0,	10,				(WW / 2) - 1,	15,		30,			STR_TITLE_SEQUENCE_OPENRCT2,		STR_NONE }, \
	{ WWT_DROPDOWN_BUTTON,	0,	(WW / 2) + 1,	WW - 10,		15,		30,			STR_TITLE_SEQUENCE_RCT2,			STR_NONE },	\
	{ WWT_IMGBTN,			1,	0,				WW - 1,			31,		WH - 1,		0xFFFFFFFF,							STR_NONE


#define BTNY(row) ((sint16)(280 + (row * 15)))
static rct_widget window_about_openrct2_widgets[] = {
	{ WIDGETS_MAIN },
	{ WWT_DROPDOWN_BUTTON,	1,	10,				(WW / 2) - 2,	280,	293,	STR_ABOUT_CHANGELOG,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	(WW / 2) + 2,	WW - 10,		280,	293,	STR_ABOUT_CONTRIBUTORS,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	10,				WW - 10,		295,	308,	STR_ABOUT_OPENRCT2_AT_GITHUB,	STR_NONE },
	{ WIDGETS_END },
};

static rct_widget window_about_rct2_widgets[] = {
	{ WIDGETS_MAIN },
	{ WWT_DROPDOWN_BUTTON,	1,	100,		299,	233,	244,	STR_MUSIC_ACKNOWLEDGEMENTS_ELLIPSIS,	STR_NONE },				// music credits button
	{ WWT_DROPDOWN_BUTTON,	1,	157,		356,	288,	299,	STR_INFOGRAMES_INTERACTIVE_CREDITS,		STR_NONE },				// infogrames credits button
	{ WIDGETS_END },
};

#define DEFAULT_ENABLED_WIDGETS \
	1ULL << WIDX_CLOSE) | (1ULL << WIDX_ABOUT_OPENRCT2) | (1ULL << WIDX_ABOUT_RCT2

static uint64 window_about_page_enabled_widgets[] = {
	(DEFAULT_ENABLED_WIDGETS) | (1ULL << WIDX_CHANGELOG) | (1ULL << WIDX_CONTRIBUTORS) | (1ULL << WIDX_GITHUB),
	(DEFAULT_ENABLED_WIDGETS) | (1ULL << WIDX_MUSIC_CREDITS) | (1ULL << WIDX_PUBLISHER_CREDITS)
};

static rct_widget *window_about_page_widgets[] = {
	window_about_openrct2_widgets,
	window_about_rct2_widgets,
};

static void window_about_openrct2_mouseup(rct_window *w, int widgetIndex);
static void window_about_openrct2_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_about_rct2_mouseup(rct_window *w, int widgetIndex);
static void window_about_rct2_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_about_set_page(rct_window *w, int page);

static rct_window_event_list window_about_openrct2_events = {
	NULL,
	window_about_openrct2_mouseup,
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
	window_about_openrct2_paint,
	NULL
};

static rct_window_event_list window_about_rct2_events = {
	NULL,
	window_about_rct2_mouseup,
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
	window_about_rct2_paint,
	NULL
};

static rct_window_event_list *window_about_page_events[] = {
	&window_about_openrct2_events,
	&window_about_rct2_events,
};

/**
 *
 *  rct2: 0x0066D2AC
 */
void window_about_open()
{
	rct_window* w;

	// Check if window is already open
	w = window_bring_to_front_by_class(WC_ABOUT);
	if (w != NULL)
		return;

	w = window_create_centred(WW, WH, window_about_page_events[WINDOW_ABOUT_PAGE_OPENRCT2], WC_ABOUT, 0);
	w->widgets = window_about_page_widgets[WINDOW_ABOUT_PAGE_OPENRCT2];
	w->enabled_widgets = window_about_page_enabled_widgets[WINDOW_ABOUT_PAGE_OPENRCT2];
	window_init_scroll_widgets(w);

	w->colours[0] = COLOUR_LIGHT_BLUE;
	w->colours[1] = COLOUR_LIGHT_BLUE;
	w->colours[2] = COLOUR_LIGHT_BLUE;
}

#pragma region OPENRCT2

static void window_about_openrct2_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_ABOUT_OPENRCT2:
	case WIDX_ABOUT_RCT2:
		window_about_set_page(w, widgetIndex - WIDX_ABOUT_OPENRCT2);
		break;
	case WIDX_CHANGELOG:
		window_textreader_open("changelog.txt");
		break;
	case WIDX_CONTRIBUTORS:
		window_textreader_open("contributors.md");
		break;
	case WIDX_GITHUB:
		window_open_link_prompt_open("https://www.github.com/OpenRCT2/OpenRCT2");
		break;
	}
}

static void window_about_openrct2_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	int x, y, width, _width, _height;
	x = w->x + (w->width / 2);
	y = w->y + w->widgets[WIDX_PAGE_BACKGROUND].top + 10;
	width = w->width - 20;
	utf8 buffer[256];
	utf8 *ch = buffer;
	openrct2_write_full_version_info(ch, sizeof(buffer) - (ch - buffer));
	y += gfx_draw_string_centred_wrapped(dpi, &ch, x, y, width, STR_STRING, w->colours[2]) + 11;
	y += gfx_draw_string_centred_wrapped(dpi, NULL, x, y, width, STR_ABOUT_OPENRCT2_DESCRIPTION, w->colours[2]) + 2;
	gfx_get_sprite_size(SPR_G2_LOGO, &_width, &_height);
	gfx_draw_sprite(dpi, SPR_G2_LOGO, x - (_width / 2), y, 0);
	y += _height + 20;
	y += gfx_draw_string_centred_wrapped(dpi, NULL, x, y, width, STR_ABOUT_OPENRCT2_DESCRIPTION_2, w->colours[2]) + 11;
	gfx_draw_string_centred_wrapped(dpi, NULL, x, y, width, STR_ABOUT_OPENRCT2_DESCRIPTION_3, w->colours[2]);
}

#pragma endregion OPENRCT2

#pragma region RCT2

static void window_about_rct2_mouseup(rct_window *w, int widgetIndex) 
{
	switch (widgetIndex) {
		case WIDX_CLOSE:
			window_close(w);
			break;
		case WIDX_ABOUT_OPENRCT2:
		case WIDX_ABOUT_RCT2:
			window_about_set_page(w, widgetIndex - WIDX_ABOUT_OPENRCT2);
			break;
		case WIDX_MUSIC_CREDITS:
			window_music_credits_open();
			break;
		case WIDX_PUBLISHER_CREDITS:
			window_publisher_credits_open();
			break;
	}
}

/**
*
*  rct2: 0x0066D321
*/
static void window_about_rct2_paint(rct_window *w, rct_drawpixelinfo *dpi) 
{
	sint32 x, y;

	window_draw_widgets(w, dpi);

	x = w->x + 200;
	y = w->y + w->widgets[WIDX_PAGE_BACKGROUND].top + 10;
	
	int col = 0; //my fork is 0, but upstream is COLOUR_BLACK.. see if COLOUR_BLACK works enough..
	// Version
	gfx_draw_string_centred(dpi, STR_VERSION_X, x, y, 0, NULL);
	y += 10;

	// Credits
	gfx_draw_string_centred(dpi, STR_COPYRIGHT_CS, x, y, 0, NULL);
	y += 14; //mine is 14, but target is 10
	gfx_draw_sprite(dpi, SPR_CREDITS_CHRIS_SAWYER_SMALL, w->x + 92, y, 0);
	y += 65; //mine is 65 but target is 79
	gfx_draw_string_centred(dpi, STR_DESIGNED_AND_PROGRAMMED_BY_CS, x, y, 0, NULL);
	y += 10;
	gfx_draw_string_centred(dpi, STR_GRAPHICS_BY_SF, x, y, COLOUR_BLACK, NULL);
	y += 10;
	gfx_draw_string_centred(dpi, STR_SOUND_AND_MUSIC_BY_AB, x, y, COLOUR_BLACK, NULL);
	y += 10;
	gfx_draw_string_centred(dpi, STR_ADDITIONAL_SOUNDS_RECORDED_BY_DE, x, y, COLOUR_BLACK, NULL);
	y += 13;
	gfx_draw_string_centred(dpi, STR_REPRESENTATION_BY_JL, x, y, COLOUR_BLACK, NULL);
	y += 25;
	gfx_draw_string_centred(dpi, STR_THANKS_TO, x, y, COLOUR_BLACK, NULL);
	y += 10;
	gfx_draw_string_centred(dpi, STR_THANKS_TO_PEOPLE, x, y, 0, NULL);
	y += 50;
	gfx_draw_sprite(dpi, SPR_CREDITS_INFOGRAMES, w->x + 50, y, 0);
	y += 10;
	gfx_draw_string_left(dpi, STR_LICENSED_TO_INFOGRAMES_INTERACTIVE_INC, NULL, 0, w->x + 157, y);
}

#pragma endregion RCT2

static void window_about_set_page(rct_window *w, int page) 
{
	w->page = page;
	w->frame_no = 0;
	w->enabled_widgets = window_about_page_enabled_widgets[page];
	w->pressed_widgets = 0;
	w->widgets = window_about_page_widgets[page];
	w->event_handlers = window_about_page_events[page];

	window_invalidate(w);
	window_event_resize_call(w);
	window_event_invalidate_call(w);
	window_init_scroll_widgets(w);
	window_invalidate(w);
}
