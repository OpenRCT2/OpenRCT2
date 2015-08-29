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

#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../network/network.h"
#include "../sprites.h"
#include "error.h"

#define WWIDTH_MIN 500
#define WHEIGHT_MIN 300
#define WWIDTH_MAX 1200
#define WHEIGHT_MAX 800
#define ITEM_HEIGHT (3 + 11 + 1 + 11 + 3)

typedef struct {
	char *address;
	utf8 *name;
	utf8 *description;
} saved_server;

char _playerName[64];
saved_server *_savedServers = NULL;
int _numSavedServers = 0;

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PLAYER_NAME_INPUT,
	WIDX_LIST,
	WIDX_ADD_SERVER,
	WIDX_START_SERVER
};

enum {
	WIDX_LIST_REMOVE,
	WIDX_LIST_SPECTATE
};

static rct_widget window_server_list_widgets[] = {
	{ WWT_FRAME,			0,	0,		340,	0,		90,		0xFFFFFFFF,					STR_NONE },					// panel / background
	{ WWT_CAPTION,			0,	1,		338,	1,		14,		STR_SERVER_LIST,			STR_WINDOW_TITLE_TIP },		// title bar
	{ WWT_CLOSEBOX,			0,	327,	337,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },		// close x button
	{ WWT_TEXT_BOX,			1,	100,	344,	20,		31,		(uint32)_playerName,		STR_NONE },					// player name text box
	{ WWT_SCROLL,			1,	6,		337,	37,		50,		STR_NONE,					STR_NONE },					// server list
	{ WWT_DROPDOWN_BUTTON,	1,	6,		106,	53,		64,		STR_ADD_SERVER,				STR_NONE },					// add server button
	{ WWT_DROPDOWN_BUTTON,	1,	112,	212,	53,		64,		STR_START_SERVER,			STR_NONE },					// start server button
	{ WIDGETS_END },
};

static void window_server_list_close(rct_window *w);
static void window_server_list_mouseup(rct_window *w, int widgetIndex);
static void window_server_list_resize(rct_window *w);
static void window_server_list_update(rct_window *w);
static void window_server_list_scroll_getsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_server_list_scroll_mousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_server_list_scroll_mouseover(rct_window *w, int scrollIndex, int x, int y);
static void window_server_list_textinput(rct_window *w, int widgetIndex, char *text);
static void window_server_list_invalidate(rct_window *w);
static void window_server_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_server_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_server_list_events = {
	window_server_list_close,
	window_server_list_mouseup,
	window_server_list_resize,
	NULL,
	NULL,
	NULL,
	window_server_list_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_server_list_scroll_getsize,
	window_server_list_scroll_mousedown,
	NULL,
	window_server_list_scroll_mouseover,
	window_server_list_textinput,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_server_list_invalidate,
	window_server_list_paint,
	window_server_list_scrollpaint
};

static int _hoverButtonIndex = -1;

static void server_list_get_item_button(int buttonIndex, int x, int y, int width, int *outX, int *outY);
static void server_list_update_player_name();
static void server_list_load_saved_servers();
static void server_list_save_saved_servers();
static void dispose_saved_server_list();
static void dispose_saved_server(saved_server *serverInfo);
static void add_saved_server(char *address);
static void remove_saved_server(int index);
static void join_server(char *address, bool spectate);

void window_server_list_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_SERVER_LIST);
	if (window != NULL)
		return;

	window = window_create_centred(WWIDTH_MIN, WHEIGHT_MIN, &window_server_list_events, WC_SERVER_LIST, WF_10 | WF_RESIZABLE);

	window->widgets = window_server_list_widgets;
	window->enabled_widgets = (
		(1 << WIDX_CLOSE) |
		(1 << WIDX_PLAYER_NAME_INPUT) |
		(1 << WIDX_ADD_SERVER) |
		(1 << WIDX_START_SERVER)
	);
	window_init_scroll_widgets(window);
	window->no_list_items = 0;
	window->selected_list_item = -1;
	window->frame_no = 0;
	window->min_width = 320;
	window->min_height = 90;
	window->max_width = window->min_width;
	window->max_height = window->min_height;

	window->page = 0;
	window->list_information_type = 0;
	window->colours[0] = 1;
	window->colours[1] = 26;
	window->colours[2] = 26;

	window_set_resize(window, WWIDTH_MIN, WHEIGHT_MIN, WWIDTH_MAX, WHEIGHT_MAX);

	strncpy(_playerName, gConfigNetwork.player_name, sizeof(_playerName));

	server_list_load_saved_servers();
	window->no_list_items = _numSavedServers;
}

static void window_server_list_close(rct_window *w)
{
	server_list_update_player_name();
	dispose_saved_server_list();
}

static void window_server_list_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_PLAYER_NAME_INPUT:
		window_start_textbox(w, widgetIndex, 1170, (uint32)_playerName, 63);
		break;
	case WIDX_ADD_SERVER:
		window_text_input_open(w, widgetIndex, STR_ADD_SERVER, STR_ENTER_HOSTNAME_OR_IP_ADDRESS, STR_NONE, 0, 128);
		break;
	case WIDX_START_SERVER:
		server_list_update_player_name();
		window_loadsave_open(LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME | LOADSAVETYPE_NETWORK, NULL);
		break;
	}
}

static void window_server_list_resize(rct_window *w)
{
	window_set_resize(w, WWIDTH_MIN, WHEIGHT_MIN, WWIDTH_MAX, WHEIGHT_MAX);
}

static void window_server_list_update(rct_window *w)
{
	if (gCurrentTextBox.window.classification == w->classification && gCurrentTextBox.window.number == w->number) {
		window_update_textbox_caret();
		widget_invalidate(w, WIDX_PLAYER_NAME_INPUT);
	}
}

static void window_server_list_scroll_getsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	*width = 0;
	*height = w->no_list_items * ITEM_HEIGHT;
}

static void window_server_list_scroll_mousedown(rct_window *w, int scrollIndex, int x, int y)
{
	int serverIndex = w->selected_list_item;
	if (serverIndex < 0) return;
	if (serverIndex >= _numSavedServers) return;

	char *serverAddress = _savedServers[serverIndex].address;
	
	switch (_hoverButtonIndex) {
	case WIDX_LIST_REMOVE:
		remove_saved_server(serverIndex);
		server_list_save_saved_servers();
		w->no_list_items = _numSavedServers;
		window_invalidate(w);
		break;
	case WIDX_LIST_SPECTATE:
		server_list_update_player_name();
		join_server(serverAddress, true);
		break;
	default:
		server_list_update_player_name();
		join_server(serverAddress, false);
		break;
	}
}

static void window_server_list_scroll_mouseover(rct_window *w, int scrollIndex, int x, int y)
{
	// Item
	int index = y / ITEM_HEIGHT;
	if (index < 0 || index >= w->no_list_items) {
		index = -1;
	}

	int hoverButtonIndex = -1;
	if (index != -1) {
		int width = w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left;
		int sy = index * ITEM_HEIGHT;
		for (int i = 0; i < 2; i++) {
			int bx, by;

			server_list_get_item_button(i, 0, sy, width, &bx, &by);
			if (x >= bx && y >= by && x < bx + 24 && y < by + 24) {
				hoverButtonIndex = i;
				break;
			}
		}
	}

	if (w->selected_list_item != index || _hoverButtonIndex != hoverButtonIndex) {
		w->selected_list_item = index;
		_hoverButtonIndex = hoverButtonIndex;
		window_invalidate(w);
	}
}

static void window_server_list_textinput(rct_window *w, int widgetIndex, char *text)
{
	if (text == NULL || text[0] == 0) return;

	switch (widgetIndex) {
	case WIDX_PLAYER_NAME_INPUT:
		if (strcmp(_playerName, text) == 0)
			return;

		if (strlen(text) == 0) {
			memset(_playerName, 0, sizeof(_playerName));
		} else {
			memset(_playerName, 0, sizeof(_playerName));
			strcpy(_playerName, text);
		}

		widget_invalidate(w, WIDX_PLAYER_NAME_INPUT);
		break;

	case WIDX_ADD_SERVER:
		add_saved_server(text);
		server_list_save_saved_servers();
		w->no_list_items = _numSavedServers;
		window_invalidate(w);
		break;
	}
}

static void window_server_list_invalidate(rct_window *w)
{
	window_server_list_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_server_list_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	window_server_list_widgets[WIDX_TITLE].right = w->width - 2;
	window_server_list_widgets[WIDX_CLOSE].left = w->width - 2 - 11;
	window_server_list_widgets[WIDX_CLOSE].right = w->width - 2 - 11 + 10;

	window_server_list_widgets[WIDX_PLAYER_NAME_INPUT].right = w->width - 6;
	window_server_list_widgets[WIDX_LIST].left = 6;
	window_server_list_widgets[WIDX_LIST].right = w->width - 6;
	window_server_list_widgets[WIDX_LIST].bottom = w->height - 6 - 11 - 6;
	window_server_list_widgets[WIDX_ADD_SERVER].top = w->height - 6 - 11;
	window_server_list_widgets[WIDX_ADD_SERVER].bottom = w->height - 6;
	window_server_list_widgets[WIDX_START_SERVER].top = w->height - 6 - 11;
	window_server_list_widgets[WIDX_START_SERVER].bottom = w->height - 6;
}

static void window_server_list_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	gfx_draw_string_left(dpi, STR_PLAYER_NAME, NULL, w->colours[1], w->x + 6, w->y + w->widgets[WIDX_PLAYER_NAME_INPUT].top);
}

static void window_server_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	uint32 colour;
	int bx, by;

	colour = ((char*)0x0141FC48)[w->colours[1] * 8];
	colour = (colour << 24) | (colour << 16) | (colour << 8) | colour;
	gfx_clear(dpi, colour);

	int width = w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left;

	int y = 0;
	for (int i = 0; i < w->no_list_items; i++) {
		if (y >= dpi->y + dpi->height) continue;
		// if (y + ITEM_HEIGHT < dpi->y) continue;

		saved_server *serverDetails = &_savedServers[i];
		bool highlighted = i == w->selected_list_item;

		// Draw hover highlight
		if (highlighted) {
			gfx_fill_rect(dpi, 0, y, width, y + ITEM_HEIGHT, 0x02000031);
		}

		// Draw server information
		if (highlighted) {
			gfx_draw_string(dpi, serverDetails->address, w->colours[1], 3, y + 3);
		} else {
			gfx_draw_string(dpi, serverDetails->name, w->colours[1], 3, y + 3);
		}
		gfx_draw_string(dpi, serverDetails->description, w->colours[1], 3, y + 14);

		// Draw delete server button
		server_list_get_item_button(0, 0, y, width, &bx, &by);
		if (highlighted && _hoverButtonIndex == WIDX_LIST_REMOVE) {
			gfx_fill_rect_inset(dpi, bx, by, bx + 24, by + 24, w->colours[1], 0);
		}
		gfx_draw_sprite(dpi, SPR_DEMOLISH, bx, by, 0);

		// Draw spectate server button
		server_list_get_item_button(1, 0, y, width, &bx, &by);
		if (highlighted && _hoverButtonIndex == WIDX_LIST_SPECTATE) {
			gfx_fill_rect_inset(dpi, bx, by, bx + 24, by + 24, w->colours[1], 0);
		}
		gfx_draw_sprite(dpi, SPR_LOCATE, bx, by, 0);

		y += ITEM_HEIGHT;
	}
}

static void server_list_get_item_button(int buttonIndex, int x, int y, int width, int *outX, int *outY)
{
	*outX = width - 3 - 36 - (30 * buttonIndex);
	*outY = y + 2;
}

static void server_list_update_player_name()
{
	if (strlen(_playerName) > 0) {
		SafeFree(gConfigNetwork.player_name);
		gConfigNetwork.player_name = _strdup(_playerName);
		config_save_default();
	}
}

static char *freadstralloc(SDL_RWops *file)
{
	int capacity = 64;
	char *buffer = malloc(capacity);

	int length = 0;
	int c;
	for (;;) {
		c = 0;
		if (SDL_RWread(file, &c, 1, 1) != 1) break;
		if (c == 0) break;

		if (length > capacity) {
			capacity *= 2;
			buffer = realloc(buffer, capacity);
		}
		buffer[length] = c;
		length++;
	}

	buffer = realloc(buffer, length + 1);
	buffer[length] = 0;
	return buffer;
}

static void server_list_load_saved_servers()
{
	utf8 path[MAX_PATH];
	SDL_RWops *file;

	platform_get_user_directory(path, NULL);
	strcat(path, "servers.cfg");

	file = SDL_RWFromFile(path, "rb");
	if (file == NULL) {
		return;
	}

	dispose_saved_server_list();

	// Read number of saved servers
	SDL_RWread(file, &_numSavedServers, sizeof(uint32), 1);
	_savedServers = malloc(_numSavedServers * sizeof(saved_server));
	
	// Load each saved server
	for (int i = 0; i < _numSavedServers; i++) {
		saved_server *serverInfo = &_savedServers[i];

		serverInfo->address = freadstralloc(file);
		serverInfo->name = freadstralloc(file);
		serverInfo->description = freadstralloc(file);
	}

	SDL_RWclose(file);
}

static void server_list_save_saved_servers()
{
	utf8 path[MAX_PATH];
	SDL_RWops *file;

	platform_get_user_directory(path, NULL);
	strcat(path, "servers.cfg");

	file = SDL_RWFromFile(path, "wb");
	if (file == NULL) {
		log_error("Unable to save servers.");
		return;
	}

	// Write number of saved servers
	SDL_RWwrite(file, &_numSavedServers, sizeof(uint32), 1);

	// Write each saved server
	for (int i = 0; i < _numSavedServers; i++) {
		saved_server *serverInfo = &_savedServers[i];

		SDL_RWwrite(file, serverInfo->address, strlen(serverInfo->address) + 1, 1);
		SDL_RWwrite(file, serverInfo->name, strlen(serverInfo->name) + 1, 1);
		SDL_RWwrite(file, serverInfo->description, strlen(serverInfo->description) + 1, 1);
	}

	SDL_RWclose(file);
}

static void dispose_saved_server_list()
{
	if (_savedServers != NULL) {
		for (int i = 0; i < _numSavedServers; i++) {
			dispose_saved_server(&_savedServers[i]);
		}
		free(_savedServers);
		_savedServers = NULL;
	}
	_numSavedServers = 0;
}

static void dispose_saved_server(saved_server *serverInfo)
{
	SafeFree(serverInfo->address);
	SafeFree(serverInfo->name);
	SafeFree(serverInfo->description);
}

static void add_saved_server(char *address)
{
	_numSavedServers++;
	if (_savedServers == NULL) {
		_savedServers = malloc(_numSavedServers * sizeof(saved_server));
	} else {
		_savedServers = realloc(_savedServers, _numSavedServers * sizeof(saved_server));
	}

	int index = _numSavedServers - 1;
	_savedServers[index].address = _strdup(address);
	_savedServers[index].name = _strdup(address);
	_savedServers[index].description = _strdup("");
}

static void remove_saved_server(int index)
{
	if (_numSavedServers <= index) return;

	int serversToMove = _numSavedServers - index - 1;
	memmove(&_savedServers[index], &_savedServers[index + 1], serversToMove * sizeof(saved_server));

	_numSavedServers--;
	_savedServers = realloc(_savedServers, _numSavedServers * sizeof(saved_server));
}

static char *substr(char *start, int length)
{
	char *result = malloc(length + 1);
	memcpy(result, start, length);
	result[length] = 0;
	return result;
}

static void join_server(char *address, bool spectate)
{
	int port = gConfigNetwork.default_port;

	char *colon = strchr(address, ':');
	if (colon != NULL) {
		address = substr(address, colon - address);
		sscanf(colon + 1, "%d", &port);
	}

	if (!network_begin_client(address, port)) {
		window_error_open(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_NONE);
	}

	if (colon != NULL) {
		free(address);
	}
}
