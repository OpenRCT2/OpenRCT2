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
#include "../interface/widget.h"
#include "../config.h"
#include "dropdown.h"

enum WINDOW_CUSTOM_CURRENCY_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_RATE,
	WIDX_RATE_UP,
	WIDX_RATE_DOWN,
	WIDX_SYMBOL_TEXT,
	WIDX_AFFIX_DROPDOWN,
	WIDX_AFFIX_DROPDOWN_BUTTON,
};

rct_widget window_custom_currency_widgets[] = {
//	  TYPE				COLOUR	LEFT		RIGHT	TOP		BOTTOM	IMAGE								TOOLTIP
	{ WWT_FRAME,			0,	0,			399,	0,		99,		0x0FFFFFFFF,						STR_NONE },				// panel / background
	{ WWT_CAPTION,			0,	1,			398,	1,		14,		STR_CUSTOM_CURRENCY_WINDOW_TITLE,	STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,			0,	387,		397,	2,		13,		STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_SPINNER,			1,	100,		200,	30,		40,		5462,								STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	189,		199,	31,		35,		STR_NUMERIC_UP,						STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	189,		199,	36,		40,		STR_NUMERIC_DOWN,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1, 	120,		200,	50,		60,		0,									STR_NONE },
	{ WWT_DROPDOWN,			1,	220,		350,	50,		60,		865,								STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	339,		349,	51,		59,		STR_DROPDOWN_GLYPH,					STR_NONE },
	{ WIDGETS_END },
};

static void window_custom_currency_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_custom_currency_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_custom_currency_text_input(struct rct_window *w, int windgetIndex, char *text);
static void window_custom_currency_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void invalidate_money_widgets();

static rct_window_event_list window_custom_currency_events = {
	NULL,
	NULL,
	NULL,
	window_custom_currency_mousedown,
	window_custom_currency_dropdown,
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
	window_custom_currency_text_input,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_custom_currency_paint,
	NULL
};

/**
 *
 *  rct2: 0x0066D2AC
 */
void window_custom_currency_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_CUSTOM_CURRENCY_CONFIG);
	if (window != NULL)
		return;

	window = window_create_centred(
		400,
		100,
		&window_custom_currency_events,
		WC_CUSTOM_CURRENCY_CONFIG,
		0
	);
	window->widgets = window_custom_currency_widgets;
	window->enabled_widgets =	(1 << WIDX_CLOSE) |
								(1 << WIDX_RATE) |
								(1 << WIDX_RATE_UP) |
								(1 << WIDX_RATE_DOWN) |
								(1 << WIDX_SYMBOL_TEXT) |
								(1 << WIDX_AFFIX_DROPDOWN) |
								(1 << WIDX_AFFIX_DROPDOWN_BUTTON);

	window_init_scroll_widgets(window);
	window->colours[0] = 22;
	window->colours[1] = 22;
	window->colours[2] = 22;
}



/**
*
*  rct2: 0x006BB01B
*/
static void window_custom_currency_mousedown(int widgetIndex, rct_window*w, rct_widget* widget) {
	widget = &w->widgets[widgetIndex - 1];

	switch (widgetIndex) {

	case WIDX_CLOSE:
		window_close(w);
		break;

	case WIDX_RATE_UP:
		CurrencyDescriptors[CURRENCY_CUSTOM].rate += 1;
		gConfigGeneral.custom_currency_rate = CurrencyDescriptors[CURRENCY_CUSTOM].rate;
		config_save_default();
		invalidate_money_widgets();
		break;

	case WIDX_RATE_DOWN:
		if(CurrencyDescriptors[CURRENCY_CUSTOM].rate > 1) {
			CurrencyDescriptors[CURRENCY_CUSTOM].rate -= 1;
			gConfigGeneral.custom_currency_rate = CurrencyDescriptors[CURRENCY_CUSTOM].rate;
			config_save_default();
			invalidate_money_widgets();
		}
		break;

	case WIDX_AFFIX_DROPDOWN_BUTTON:
		gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[0] = STR_PREFIX;

		gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[1] = STR_SUFFIX;

		window_dropdown_show_text_custom_width(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			2,
			widget->right - widget->left - 3
		);

		if(CurrencyDescriptors[CURRENCY_CUSTOM].affix_unicode == CURRENCY_PREFIX) {
			dropdown_set_checked(0, true);
		} else {
			dropdown_set_checked(1, true);
		}

		break;

	case WIDX_SYMBOL_TEXT:
		window_text_input_raw_open(	w,
									WIDX_SYMBOL_TEXT,
									STR_CUSTOM_CURRENCY_SYMBOL_INPUT_TITLE,
									STR_CUSTOM_CURRENCY_SYMBOL_INPUT_DESC,
									CurrencyDescriptors[CURRENCY_CUSTOM].symbol_unicode,
									CURRENCY_SYMBOL_MAX_SIZE);
		break;

	}
}

/**
*
*  rct2: 0x006BB076
*/
static void window_custom_currency_dropdown(rct_window *w, int widgetIndex, int dropdownIndex) {
	if (dropdownIndex == -1)
			return;

	if(widgetIndex == WIDX_AFFIX_DROPDOWN_BUTTON) {

		if(dropdownIndex == 0) {
			CurrencyDescriptors[CURRENCY_CUSTOM].affix_ascii = CurrencyDescriptors[CURRENCY_CUSTOM].affix_unicode = CURRENCY_PREFIX;
		} else if(dropdownIndex == 1) {
			CurrencyDescriptors[CURRENCY_CUSTOM].affix_ascii = CurrencyDescriptors[CURRENCY_CUSTOM].affix_unicode = CURRENCY_SUFFIX;
		}


		gConfigGeneral.custom_currency_affix = CurrencyDescriptors[CURRENCY_CUSTOM].affix_unicode;
		config_save_default();

		invalidate_money_widgets();

	}
}

static void window_custom_currency_text_input(struct rct_window *w, int windgetIndex, char *text) {
	if(text != NULL) {
		strncpy(CurrencyDescriptors[CURRENCY_CUSTOM].symbol_unicode, text, CURRENCY_SYMBOL_MAX_SIZE);
		strncpy(gConfigGeneral.custom_currency_symbol, CurrencyDescriptors[CURRENCY_CUSTOM].symbol_unicode, CURRENCY_SYMBOL_MAX_SIZE);
		config_save_default();
		invalidate_money_widgets();
	}
}

static void invalidate_money_widgets() {
	widget_invalidate_by_class(WC_BOTTOM_TOOLBAR, 2 /*WIDX_MONEY (not accesible)*/);
	widget_invalidate_by_class(WC_CUSTOM_CURRENCY_CONFIG, WIDX_RATE);
}

/**
 *
 *  rct2: 0x0066D321
 */
static void window_custom_currency_paint(rct_window *w, rct_drawpixelinfo *dpi) {
	int x, y;

	set_format_arg(0, sint32, 100);

	window_draw_widgets(w, dpi);

	x = w->x + 10;
	y = w->y + 30;

	gfx_draw_string_left(dpi, STR_RATE, NULL, w->colours[1], x, y);

	sint32 baseExchange = 100/CurrencyDescriptors[CURRENCY_POUNDS].rate;
	set_format_arg(0, sint32, baseExchange);
	gfx_draw_string_left(dpi, STR_CUSTOM_CURRENCY_EQUIVALENCY, gCommonFormatArgs, w->colours[1], x+200, y);

	y += 20;

	gfx_draw_string_left(dpi, STR_CURRENCY_SYMBOL_TEXT, NULL, w->colours[1], x, y);

	gfx_draw_string(dpi, CurrencyDescriptors[CURRENCY_CUSTOM].symbol_unicode, w->colours[1], w->x + window_custom_currency_widgets[WIDX_SYMBOL_TEXT].left + 1, w->y + window_custom_currency_widgets[WIDX_SYMBOL_TEXT].top);

	if(CurrencyDescriptors[CURRENCY_CUSTOM].affix_unicode == CURRENCY_PREFIX){
		gfx_draw_string_left(dpi, STR_PREFIX, w, w->colours[1], w->x + window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].left + 1, w->y + window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].top);
	} else {
		gfx_draw_string_left(dpi, STR_SUFFIX, w, w->colours[1], w->x + window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].left + 1, w->y + window_custom_currency_widgets[WIDX_AFFIX_DROPDOWN].top);
	}

}
