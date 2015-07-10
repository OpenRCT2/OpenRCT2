/*****************************************************************************
 * Copyright (c) 2014 Ted John, Timmy Weerwag
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
#include "../config.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/themes.h"

static rct_widget window_title_options_widgets[] = {
	{ WWT_DROPDOWN_BUTTON, 2, 0, 79, 0, 11, STR_OPTIONS, STR_NONE },
	{ WIDGETS_END },
};

static void window_title_options_mouseup(rct_window *w, int widgetIndex);
static void window_title_options_invalidate(rct_window *w);
static void window_title_options_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_options_events = {
	NULL,
	window_title_options_mouseup,
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
	window_title_options_invalidate,
	window_title_options_paint,
	NULL
};

/**
 * Creates the window containing the options button on the title screen.
 */
void window_title_options_open()
{
	rct_window* window;

	window = window_create(
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) - 80, 0,
		80, 12,
		&window_title_options_events,
		WC_TITLE_OPTIONS,
		WF_STICK_TO_BACK | WF_TRANSPARENT
	);
	window->widgets = window_title_options_widgets;
	window->enabled_widgets |= 1;
	window_init_scroll_widgets(window);
}

static void window_title_options_mouseup(rct_window *w, int widgetIndex)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) != 0)
		return;

	if (widgetIndex == 0)
		window_options_open();
}

static void window_title_options_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
}

static void window_title_options_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}
