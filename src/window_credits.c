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
#include "addresses.h"
#include "string_ids.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"

static void window_music_credits_emptysub() { }

static void* window_music_credits_events[] = {
	window_music_credits_emptysub,
	(void*)0x0066DB2C, // onmouseup
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	(void*)0x0066DB37, //onscrollgetsize
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	window_music_credits_emptysub,
	(void*)0x0066D7B9, // paint
	(void*)0x0066D7BF //on scroll paint
};

/**
*
*  rct2: 0x0066D55B
*/
void window_music_credits_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_id(WC_PUBLISHER_CREDITS, 0);
	if (window != NULL)
		return;

	window = window_create(
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) / 2 - 255,
		max(28, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) / 2 - 157),
		510,
		314,
		(uint32*)window_music_credits_events,
		WC_PUBLISHER_CREDITS,
		0
		);

	window->widgets = (rct_widget*) 0x009A9704;
	window->enabled_widgets = 0x4;

	window_init_scroll_widgets(window);
	window->colours[0] = 7;
	window->colours[1] = 7;
	window->colours[2] = 7;
	
}