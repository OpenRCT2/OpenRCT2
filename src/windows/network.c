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

#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../network/network.h"

static void window_chat_host_emptysub() { }

static void window_chat_host_textinput()
{
	rct_window *w;
	short widgetIndex;
	uint8 result;
	char *text;

	window_textinput_get_registers(w, widgetIndex, result, text);

	if (!result)
		return;

	network_send_chat(text);

	window_close(w);
}

static void* window_chat_host_events[] = {
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_textinput,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub
};

void window_chat_open()
{
	rct_window *w;
	w = window_create(0, 0, 0, 0, (uint32*)window_chat_host_events, WC_CHAT_HOST, 0);
	w->colours[0] = 1;
	w->colours[1] = 1;
	w->colours[2] = 0;
	w->number = 0;
	window_text_input_open(w, 0, 6000, 6001, STR_NONE, 0, 64);
}