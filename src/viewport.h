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

#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include "window.h"

enum {
	VIEWPORT_FLAG_UNDERGROUND_INSIDE = (1 << 0),
	VIEWPORT_FLAG_SEETHROUGH_RIDES = (1 << 1),
	VIEWPORT_FLAG_SEETHROUGH_SCENERY = (1 << 2),
	VIEWPORT_FLAG_INVISIBLE_SUPPORTS = (1 << 3),
	VIEWPORT_FLAG_LAND_HEIGHTS = (1 << 4),
	VIEWPORT_FLAG_TRACK_HEIGHTS = (1 << 5),
	VIEWPORT_FLAG_PATH_HEIGHTS = (1 << 6),
	VIEWPORT_FLAG_GRIDLINES = (1 << 7),
	VIEWPORT_FLAG_LAND_OWNERSHIP = (1 << 8),
	VIEWPORT_FLAG_CONSTRUCTION_RIGHTS = (1 << 9),
	VIEWPORT_FLAG_SOUND_ON = (1 << 10),
	VIEWPORT_FLAG_INVISIBLE_PEEPS = (1 << 11),
	VIEWPORT_FLAG_HIDE_BASE = (1 << 12),
	VIEWPORT_FLAG_HIDE_VERTICAL = (1 << 13),
	VIEWPORT_FLAG_INVISIBLE_SPRITES = (1 << 14),
	VIEWPORT_FLAG_15 = (1 << 15)
};

// rct2: 0x014234BC
extern rct_viewport* g_viewport_list;

void viewport_init_all();
void viewport_create(rct_window *w, int x, int y, int width, int height, int zoom, int ecx, int edx);
void viewport_update_pointers();
void viewport_update_position(rct_window *window);
void viewport_render(rct_drawpixelinfo *dpi, rct_viewport *viewport, int left, int top, int right, int bottom);

void screen_pos_to_map_pos(int *x, int *y);

void show_gridlines();
void hide_gridlines();
void show_land_rights();
void hide_land_rights();
void show_construction_rights();
void hide_construction_rights();

#endif
