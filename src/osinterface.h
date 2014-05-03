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
 
#ifndef _SDL_INTERFACE_H_
#define _SDL_INTERFACE_H_

enum {
	CURSOR_UP = 0,
	CURSOR_DOWN = 1,
	CURSOR_CHANGED = 2,
	CURSOR_RELEASED = CURSOR_UP | CURSOR_CHANGED,
	CURSOR_PRESSED = CURSOR_DOWN | CURSOR_CHANGED,
};

typedef struct {
	int x, y;
	unsigned char left, middle, right, any;
	int wheel;
	int old;
} openrct2_cursor;

extern openrct2_cursor gCursorState;
extern unsigned char* gKeysState;
extern unsigned char* gKeysPressed;
extern unsigned int gLastKeyPressed;

void osinterface_init();
void osinterface_process_messages();
void osinterface_draw();
void osinterface_free();

int osinterface_open_common_file_dialog(int type, char *title, char *filename, char *filterPattern, char *filterName);

#endif
