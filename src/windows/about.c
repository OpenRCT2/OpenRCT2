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

#include <windows.h>
#include <string.h>
#include "../addresses.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../interface/widget.h"
#include "../interface/window.h"

enum WINDOW_ABOUT_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_MUSIC_CREDITS,
	WIDX_PUBLISHER_CREDITS
};

rct_widget window_about_widgets[] = {
	{ WWT_FRAME,			0,	0,			399,	0,		329,	0x0FFFFFFFF,							STR_NONE },				// panel / background
	{ WWT_CAPTION,			0,	1,			398,	1,		14,		STR_ROLLERCOASTER_TYCOON_2,				STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,			0,	387,		397,	2,		13,		STR_CLOSE_X,							STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_DROPDOWN_BUTTON,	1,	100,		299,	230,	241,	STR_MUSIC_ACKNOWLEDGEMENTS_ELLIPSIS,	STR_NONE },				// music credits button
	{ WWT_DROPDOWN_BUTTON,	1,	157,		356,	307,	318,	STR_INFOGRAMES_INTERACTIVE_CREDITS,		STR_NONE },				// infogrames credits button
	{ WIDGETS_END },
};

static void window_about_emptysub() { }
static void window_about_mouseup();
static void window_about_paint();

static void* window_about_events[] = {
	window_about_emptysub,
	window_about_mouseup,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_emptysub,
	window_about_paint,
	window_about_emptysub
};

/**
 * 
 *  rct2: 0x0066D2AC
 */
void window_about_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_id(WC_ABOUT, 0);
	if (window != NULL)
		return;

	window = window_create(
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) / 2 - 200,
		max(28, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) / 2 - 165),
		400,
		330,
		(uint32*)window_about_events,
		WC_ABOUT,
		0
	);
	window->widgets = window_about_widgets;
	window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_MUSIC_CREDITS) | (1 << WIDX_PUBLISHER_CREDITS);

	window_init_scroll_widgets(window);
	window->colours[0] = 7;
	window->colours[1] = 7;
	window->colours[2] = 7;
}

/**
 * 
 *  rct2: 0x0066D4D5
 */
static void window_about_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
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
static void window_about_paint()
{
	int x, y;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	x = w->x + 200;
	y = w->y + 17;

	// Version
	RCT2_GLOBAL(0x009C383C, uint8) = 49;
	gfx_draw_string_centred(dpi, STR_VERSION_X, x, y, 0, (void*)0x009E2D28);
	
	// Credits
	RCT2_GLOBAL(0x009C383C, uint8) = 48;
	y += 10;
	gfx_draw_string_centred(dpi, STR_COPYRIGHT_CS, x, y, 0, (void*)0x009E2D28);
	y += 79;
	gfx_draw_string_centred(dpi, STR_DESIGNED_AND_PROGRAMMED_BY_CS, x, y, 0, (void*)0x009E2D28);
	y += 10;
	gfx_draw_string_centred(dpi, STR_GRAPHICS_BY_SF, x, y, 0, (void*)0x009E2D28);
	y += 10;
	gfx_draw_string_centred(dpi, STR_SOUND_AND_MUSIC_BY_AB, x, y, 0, (void*)0x009E2D28);
	y += 10;
	gfx_draw_string_centred(dpi, STR_ADDITIONAL_SOUNDS_RECORDED_BY_DE, x, y, 0, (void*)0x009E2D28);
	y += 13;
	gfx_draw_string_centred(dpi, STR_REPRESENTATION_BY_JL, x, y, 0, (void*)0x009E2D28);
	y += 25;
	gfx_draw_string_centred(dpi, STR_THANKS_TO, x, y, 0, (void*)0x009E2D28);
	y += 10;
	gfx_draw_string_centred(dpi, STR_THANKS_TO_PEOPLE, x, y, 0, (void*)0x009E2D28);
	y += 10;
	gfx_draw_string_centred(dpi, STR_CREDIT_SPARE_1, x, y, 0, (void*)0x009E2D28);
	y += 10;
	gfx_draw_string_centred(dpi, STR_CREDIT_SPARE_2, x, y, 0, (void*)0x009E2D28);
	y += 10;
	gfx_draw_string_centred(dpi, STR_CREDIT_SPARE_3, x, y, 0, (void*)0x009E2D28);
	y += 10;
	gfx_draw_string_centred(dpi, STR_CREDIT_SPARE_4, x, y, 0, (void*)0x009E2D28);
	y += 10;
	gfx_draw_string_centred(dpi, STR_CREDIT_SPARE_5, x, y, 0, (void*)0x009E2D28);
	y += 10;
	gfx_draw_string_centred(dpi, STR_CREDIT_SPARE_6, x, y, 0, (void*)0x009E2D28);
	y += 10;
	gfx_draw_string_centred(dpi, STR_CREDIT_SPARE_7, x, y, 0, (void*)0x009E2D28);

	// Images
	gfx_draw_sprite(dpi, SPR_CREDITS_CHRIS_SAWYER_SMALL, w->x + 92, w->y + 40, 0);
	gfx_draw_sprite(dpi, SPR_CREDITS_INFOGRAMES, w->x + 50, w->y + 247, 0);

	// Licence
	gfx_draw_string_left(dpi, STR_LICENSED_TO_INFOGRAMES_INTERACTIVE_INC, 0, 0, w->x + 157, w->y + 257);
}
