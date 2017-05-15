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
#include "../config/Config.h"
#include "../localisation/localisation.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../world/map.h"
#include "../world/banner.h"
#include "../world/scenery.h"
#include "error.h"
#include "dropdown.h"
#include "../drawing/drawing.h"
#include "../interface/themes.h"
#include "../sprites.h"

#define WW 113
#define WH 96

enum WINDOW_BANNER_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_VIEWPORT,
	WIDX_BANNER_TEXT,
	WIDX_BANNER_NO_ENTRY,
	WIDX_BANNER_DEMOLISH,
	WIDX_MAIN_COLOUR,
	WIDX_TEXT_COLOUR_DROPDOWN,
	WIDX_TEXT_COLOUR_DROPDOWN_BUTTON
};

static const rct_string_id BannerColouredTextFormats[] = {
	STR_TEXT_COLOR_BLACK,
	STR_TEXT_COLOR_GREY,
	STR_TEXT_COLOR_WHITE,
	STR_TEXT_COLOR_RED,
	STR_TEXT_COLOR_GREEN,
	STR_TEXT_COLOR_YELLOW,
	STR_TEXT_COLOR_TOPAZ,
	STR_TEXT_COLOR_CELADON,
	STR_TEXT_COLOR_BABYBLUE,
	STR_TEXT_COLOR_PALELAVENDER,
	STR_TEXT_COLOR_PALEGOLD,
	STR_TEXT_COLOR_LIGHTPINK,
	STR_TEXT_COLOR_PEARLAQUA,
	STR_TEXT_COLOR_PALESILVER,
};

rct_widget window_banner_widgets[] = {
	{ WWT_FRAME,			0,	0,			WW - 1,	0,		WH - 1,		0xFFFFFFFF,					STR_NONE},							// panel / background
	{ WWT_CAPTION,			0,	1,			WW - 2,	1,		14,			STR_BANNER_WINDOW_TITLE,	STR_WINDOW_TITLE_TIP},				// title bar
	{ WWT_CLOSEBOX,			0,	WW - 13,	WW - 3,	2,		13,			STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP},				// close x button
	{ WWT_VIEWPORT,			1,	3,			WW - 26,17,		WH - 20,	0x0FFFFFFFE,				STR_NONE},							// tab content panel
	{ WWT_FLATBTN,			1,	WW - 25,	WW - 2,	19,		42,			SPR_RENAME,					STR_CHANGE_BANNER_TEXT_TIP},		// change banner button
	{ WWT_FLATBTN,			1,	WW - 25,	WW - 2,	43,		66,			SPR_NO_ENTRY,				STR_SET_AS_NO_ENTRY_BANNER_TIP},	// no entry button
	{ WWT_FLATBTN,			1,	WW - 25,	WW - 2,	67,		90,			SPR_DEMOLISH,				STR_DEMOLISH_BANNER_TIP},			// demolish button
	{ WWT_COLOURBTN,		1,	5,			16,		WH - 16,WH - 5,		0xFFFFFFFF,					STR_SELECT_MAIN_SIGN_COLOUR_TIP},	// high money
	{ WWT_DROPDOWN,			1,	43,			81,		WH - 16,WH - 5,		0xFFFFFFFF,					STR_NONE},							// high money
	{ WWT_DROPDOWN_BUTTON,	1,	70,			80,		WH - 15,WH - 6,		STR_DROPDOWN_GLYPH,			STR_SELECT_TEXT_COLOUR_TIP},		// high money
	{ WIDGETS_END },
};

static void window_banner_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_banner_mousedown(rct_widgetindex widgetIndex, rct_window*w, rct_widget* widget);
static void window_banner_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_banner_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_banner_unknown_14(rct_window *w);
static void window_banner_invalidate(rct_window *w);
static void window_banner_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_banner_events = {
	NULL,
	window_banner_mouseup,
	NULL,
	window_banner_mousedown,
	window_banner_dropdown,
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
	window_banner_textinput,
	window_banner_unknown_14,
	NULL,
	NULL,
	NULL,
	NULL,
	window_banner_invalidate,
	window_banner_paint,
	NULL
};

/**
*
*  rct2: 0x006BA305
*/
void window_banner_open(rct_windownumber number)
{
	rct_window* w;
	rct_widget *viewportWidget;


	// Check if window is already open
	w = window_bring_to_front_by_number(WC_BANNER, number);
	if (w != NULL)
		return;

	w = window_create_auto_pos(WW, WH, &window_banner_events, WC_BANNER, WF_NO_SCROLLING);
	w->widgets = window_banner_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_BANNER_TEXT) |
		(1 << WIDX_BANNER_NO_ENTRY) |
		(1 << WIDX_BANNER_DEMOLISH) |
		(1 << WIDX_MAIN_COLOUR) |
		(1 << WIDX_TEXT_COLOUR_DROPDOWN) |
		(1 << WIDX_TEXT_COLOUR_DROPDOWN_BUTTON);

	w->number = number;
	window_init_scroll_widgets(w);

	sint32 view_x = gBanners[w->number].x << 5;
	sint32 view_y = gBanners[w->number].y << 5;

	rct_map_element* map_element = map_get_first_element_at(view_x / 32, view_y / 32);
	while(1) {
		if (
			(map_element_get_type(map_element) == MAP_ELEMENT_TYPE_BANNER) &&
			(map_element->properties.banner.index == w->number)
		) {
			break;
		}

		map_element++;
	}

	sint32 view_z = map_element->base_height<<3;
	w->frame_no = view_z;

	view_x += 16;
	view_y += 16;

	// Create viewport
	viewportWidget = &window_banner_widgets[WIDX_VIEWPORT];
	viewport_create(
		w,
		w->x + viewportWidget->left + 1,
		w->y + viewportWidget->top + 1,
		(viewportWidget->right - viewportWidget->left) - 2,
		(viewportWidget->bottom - viewportWidget->top) - 2,
		0,
		view_x,
		view_y,
		view_z,
		0,
		-1
	);

	w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
	window_invalidate(w);
}

/**
 *
 *  rct2: 0x6ba4d6
 */
static void window_banner_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	rct_banner* banner = &gBanners[w->number];
	sint32 x = banner->x << 5;
	sint32 y = banner->y << 5;

	rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

	while (1){
		if ((map_element_get_type(map_element) == MAP_ELEMENT_TYPE_BANNER) &&
			(map_element->properties.banner.index == w->number)) break;
		map_element++;
	}

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_BANNER_DEMOLISH:
		game_do_command(x, 1, y, map_element->base_height | (map_element->properties.banner.position << 8), GAME_COMMAND_REMOVE_BANNER, 0, 0);
		break;
	case WIDX_BANNER_TEXT:
		window_text_input_open(w, WIDX_BANNER_TEXT, STR_BANNER_TEXT, STR_ENTER_BANNER_TEXT, gBanners[w->number].string_idx, 0, 32);
		break;
	case WIDX_BANNER_NO_ENTRY:
		textinput_cancel();
		game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, banner->colour, GAME_COMMAND_SET_BANNER_STYLE, banner->text_colour, banner->flags ^ BANNER_FLAG_NO_ENTRY);
		break;
	}
}

/**
 *
 *  rct2: 0x6ba4ff
 */
static void window_banner_mousedown(rct_widgetindex widgetIndex, rct_window*w, rct_widget* widget)
{
	rct_banner* banner = &gBanners[w->number];

	switch (widgetIndex) {
	case WIDX_MAIN_COLOUR:
		window_dropdown_show_colour(w, widget, TRANSLUCENT(w->colours[1]), banner->colour);
		break;
	case WIDX_TEXT_COLOUR_DROPDOWN_BUTTON:

		for( sint32 i = 0; i < 13; ++i){
			gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[i] = BannerColouredTextFormats[i + 1];
		}

		// Switch to the dropdown box widget.
		widget--;

		window_dropdown_show_text_custom_width(
			widget->left + w->x,
			widget->top + w->y,
			widget->bottom - widget->top + 1,
			w->colours[1],
			0,
			DROPDOWN_FLAG_STAY_OPEN,
			13,
			widget->right - widget->left - 3);

		dropdown_set_checked(banner->text_colour - 1, true);
		break;
	}
}

/**
 *
 *  rct2: 0x6ba517
 */
static void window_banner_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	rct_banner* banner = &gBanners[w->number];

	switch(widgetIndex){
	case WIDX_MAIN_COLOUR:
		if (dropdownIndex == -1)
			break;

		game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, dropdownIndex, GAME_COMMAND_SET_BANNER_STYLE, banner->text_colour, banner->flags);
		break;
	case WIDX_TEXT_COLOUR_DROPDOWN_BUTTON:
		if (dropdownIndex == -1)
			break;

		game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, banner->colour, GAME_COMMAND_SET_BANNER_STYLE, dropdownIndex + 1, banner->flags);
		break;
	}
}

/**
 *
 *  rct2: 0x6ba50c
 */
static void window_banner_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
	if (widgetIndex == WIDX_BANNER_TEXT && text != NULL) {
		game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 0)), GAME_COMMAND_SET_BANNER_NAME, *((sint32*)(text + 8)), *((sint32*)(text + 4)));
		game_do_command(2, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 12)), GAME_COMMAND_SET_BANNER_NAME, *((sint32*)(text + 20)), *((sint32*)(text + 16)));
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, w->number, *((sint32*)(text + 24)), GAME_COMMAND_SET_BANNER_NAME, *((sint32*)(text + 32)), *((sint32*)(text + 28)));
	}
}

/**
 *
 *  rct2: 0x006BA44D
 */
static void window_banner_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	rct_banner* banner = &gBanners[w->number];
	rct_widget* colour_btn = &window_banner_widgets[WIDX_MAIN_COLOUR];
	colour_btn->type = WWT_EMPTY;

	// Scenery item not sure why we use this instead of banner?
	rct_scenery_entry* sceneryEntry = get_banner_entry(banner->type);

	if (sceneryEntry->banner.flags & 1) colour_btn->type = WWT_COLOURBTN;

	w->pressed_widgets &= ~(1ULL<<WIDX_BANNER_NO_ENTRY);
	w->disabled_widgets &= ~(
		(1ULL<<WIDX_BANNER_TEXT)|
		(1ULL<<WIDX_TEXT_COLOUR_DROPDOWN)|
		(1ULL<<WIDX_TEXT_COLOUR_DROPDOWN_BUTTON));

	if (banner->flags & BANNER_FLAG_NO_ENTRY){
		w->pressed_widgets |= (1ULL<<WIDX_BANNER_NO_ENTRY);
		w->disabled_widgets |=
			(1ULL<<WIDX_BANNER_TEXT)|
			(1ULL<<WIDX_TEXT_COLOUR_DROPDOWN)|
			(1ULL<<WIDX_TEXT_COLOUR_DROPDOWN_BUTTON);
	}

	colour_btn->image = (banner->colour << 19) | 0x60000000 | SPR_PALETTE_BTN;

	rct_widget* drop_down_widget = &window_banner_widgets[WIDX_TEXT_COLOUR_DROPDOWN];
	drop_down_widget->text = BannerColouredTextFormats[banner->text_colour];
}

/* rct2: 0x006BA4C5 */
static void window_banner_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	// Draw viewport
	if (w->viewport != NULL) {
		window_draw_viewport(dpi, w);
	}
}

/**
 *
 *  rct2: 0x6BA7B5
 */
static void window_banner_unknown_14(rct_window *w)
{
	rct_viewport* view = w->viewport;
	w->viewport = 0;

	view->width = 0;
	viewport_update_pointers();

	rct_banner* banner = &gBanners[w->number];

	sint32 view_x = (banner->x << 5) + 16;
	sint32 view_y = (banner->y << 5) + 16;
	sint32 view_z = w->frame_no;

	// Create viewport
	rct_widget* viewportWidget = &window_banner_widgets[WIDX_VIEWPORT];
	viewport_create(
		w,
		w->x + viewportWidget->left + 1,
		w->y + viewportWidget->top + 1,
		(viewportWidget->right - viewportWidget->left) - 1,
		(viewportWidget->bottom - viewportWidget->top) - 1,
		0,
		view_x,
		view_y,
		view_z,
		0,
		-1
	);

	w->viewport->flags = gConfigGeneral.always_show_gridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
	window_invalidate(w);
}
