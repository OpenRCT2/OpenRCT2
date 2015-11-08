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

#include "../config.h"
#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../network/network.h"
#include "../sprites.h"
#include "../util/util.h"
#include "error.h"

char _port[7];
char _name[65];
char _password[33];

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PORT_INPUT,
	WIDX_NAME_INPUT,
	WIDX_PASSWORD_INPUT,
	WIDX_MAXPLAYERS,
	WIDX_MAXPLAYERS_INCREASE,
	WIDX_MAXPLAYERS_DECREASE,
	WIDX_ADVERTISE_CHECKBOX,
	WIDX_START_SERVER
};

#define WW 300
#define WH 120

static rct_widget window_server_start_widgets[] = {
	{ WWT_FRAME,			0,	0,		WW-1,	0,			WH-1,	0xFFFFFFFF,				STR_NONE },					// panel / background
	{ WWT_CAPTION,			0,	1,		WW-2,	1,			14,		STR_START_SERVER,		STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0,	WW-13,	WW-3,	2,			13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },		// close x button
	{ WWT_TEXT_BOX,			1,	120,	WW-8,	20,			32,		(uint32)_port,			STR_NONE },					// port text box
	{ WWT_TEXT_BOX,			1,	120,	WW-8,	36,			48,		(uint32)_name,			STR_NONE },					// name text box
	{ WWT_TEXT_BOX,			1,	120,	WW-8,	52,			64,		(uint32)_password,		STR_NONE },					// password text box
	{ WWT_SPINNER,			1,	120,	WW-8,	68,			77,		1871,					STR_NONE },					// max players
	{ WWT_DROPDOWN_BUTTON,	1,	WW-18,	WW-8,	68,			72,		STR_NUMERIC_UP,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	WW-18,	WW-8,	72,			76,		STR_NUMERIC_DOWN,		STR_NONE },
	{ WWT_CHECKBOX,			1,	6,		WW-8,	85,			91,		STR_ADVERTISE,			STR_NONE },					// advertise checkbox
	{ WWT_DROPDOWN_BUTTON,	1,	6,		106,	WH-6-11,	WH-6,	STR_START_SERVER,		STR_NONE },					// start server button
	{ WIDGETS_END },
};

static void window_server_start_close(rct_window *w);
static void window_server_start_mouseup(rct_window *w, int widgetIndex);
static void window_server_start_update(rct_window *w);
static void window_server_start_textinput(rct_window *w, int widgetIndex, char *text);
static void window_server_start_invalidate(rct_window *w);
static void window_server_start_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_server_start_events = {
	window_server_start_close,
	window_server_start_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_server_start_update,
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
	window_server_start_textinput,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_server_start_invalidate,
	window_server_start_paint,
	NULL
};

void window_server_start_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_SERVER_START);
	if (window != NULL)
		return;

	window = window_create_centred(WW, WH, &window_server_start_events, WC_SERVER_START, WF_10);

	window->widgets = window_server_start_widgets;
	window->enabled_widgets = (
		(1 << WIDX_CLOSE) |
		(1 << WIDX_PORT_INPUT) |
		(1 << WIDX_NAME_INPUT) |
		(1 << WIDX_PASSWORD_INPUT) |
		(1 << WIDX_MAXPLAYERS) |
		(1 << WIDX_MAXPLAYERS_INCREASE) |
		(1 << WIDX_MAXPLAYERS_DECREASE) |
		(1 << WIDX_ADVERTISE_CHECKBOX) |
		(1 << WIDX_START_SERVER)
	);
	window_init_scroll_widgets(window);
	window->no_list_items = 0;
	window->selected_list_item = -1;
	window->frame_no = 0;
	window->min_width = window->width;
	window->min_height = window->height;
	window->max_width = window->min_width;
	window->max_height = window->min_height;

	window->page = 0;
	window->list_information_type = 0;
	window->colours[0] = 1;
	window->colours[1] = 26;
	window->colours[2] = 26;

	sprintf(_port, "%lu", gConfigNetwork.default_port);
	safe_strncpy(_name, gConfigNetwork.server_name, sizeof(_name));
}

static void window_server_start_close(rct_window *w)
{

}

static void window_server_start_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_PORT_INPUT:
		window_start_textbox(w, widgetIndex, 1170, (uint32)_port, 6);
		break;
	case WIDX_NAME_INPUT:
		window_start_textbox(w, widgetIndex, 1170, (uint32)_name, 64);
		break;
	case WIDX_PASSWORD_INPUT:
		window_start_textbox(w, widgetIndex, 1170, (uint32)_password, 32);
		break;
	case WIDX_MAXPLAYERS_INCREASE:
		if (gConfigNetwork.maxplayers < 255) {
			gConfigNetwork.maxplayers++;
		}
		config_save_default();
		window_invalidate(w);
		break;
	case WIDX_MAXPLAYERS_DECREASE:
		if (gConfigNetwork.maxplayers > 1) {
			gConfigNetwork.maxplayers--;
		}
		config_save_default();
		window_invalidate(w);
		break;
	case WIDX_ADVERTISE_CHECKBOX:
		gConfigNetwork.advertise = !gConfigNetwork.advertise;
		config_save_default();
		window_invalidate(w);
		break;
	case WIDX_START_SERVER:
		network_set_password(_password);
		window_loadsave_open(LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME | LOADSAVETYPE_NETWORK, NULL);
		break;
	}
}

static void window_server_start_update(rct_window *w)
{
	if (gCurrentTextBox.window.classification == w->classification && gCurrentTextBox.window.number == w->number) {
		window_update_textbox_caret();
		widget_invalidate(w, WIDX_NAME_INPUT);
		widget_invalidate(w, WIDX_PASSWORD_INPUT);
	}
}

static void window_server_start_textinput(rct_window *w, int widgetIndex, char *text)
{
	if (text == NULL) return;

	switch (widgetIndex) {
	case WIDX_PORT_INPUT:
		if (strcmp(_port, text) == 0)
			return;

		memset(_port, 0, sizeof(_port));
		if (strlen(text) > 0) {
			safe_strncpy(_port, text, sizeof(_port));
		}

		gConfigNetwork.default_port = atoi(_port);
		config_save_default();

		widget_invalidate(w, WIDX_NAME_INPUT);
		break;
	case WIDX_NAME_INPUT:
		if (strcmp(_name, text) == 0)
			return;

		memset(_name, 0, sizeof(_name));
		if (strlen(text) > 0) {
			safe_strncpy(_name, text, sizeof(_name));
		}

		if (strlen(_name) > 0) {
			SafeFree(gConfigNetwork.server_name);
			gConfigNetwork.server_name = _strdup(_name);
			config_save_default();
		}

		widget_invalidate(w, WIDX_NAME_INPUT);
		break;
	case WIDX_PASSWORD_INPUT:
		if (strcmp(_password, text) == 0)
			return;

		memset(_password, 0, sizeof(_password));
		if (strlen(text) > 0) {
			safe_strncpy(_password, text, sizeof(_password));
		}

		widget_invalidate(w, WIDX_PASSWORD_INPUT);
		break;
	}
}

static void window_server_start_invalidate(rct_window *w)
{
	widget_set_checkbox_value(w, WIDX_ADVERTISE_CHECKBOX, gConfigNetwork.advertise);
	RCT2_GLOBAL(0x013CE964, uint16) = gConfigNetwork.maxplayers;
}

static void window_server_start_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	gfx_draw_string_left(dpi, STR_PORT, NULL, w->colours[1], w->x + 6, w->y + w->widgets[WIDX_PORT_INPUT].top);
	gfx_draw_string_left(dpi, STR_SERVER_NAME, NULL, w->colours[1], w->x + 6, w->y + w->widgets[WIDX_NAME_INPUT].top);
	gfx_draw_string_left(dpi, STR_PASSWORD, NULL, w->colours[1], w->x + 6, w->y + w->widgets[WIDX_PASSWORD_INPUT].top);
	gfx_draw_string_left(dpi, STR_MAX_PLAYERS, NULL, w->colours[1], w->x + 6, w->y + w->widgets[WIDX_MAXPLAYERS].top);
}
