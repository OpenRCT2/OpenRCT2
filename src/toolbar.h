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

#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

#include "interface/window.h"
#include "interface/widget.h"

typedef enum {
	DDIDX_UNDERGROUND_INSIDE = 0,
	DDIDX_HIDE_BASE = 1,
	DDIDX_HIDE_VERTICAL = 2,
	DDIDX_SEETHROUGH_RIDES = 4,
	DDIDX_SEETHROUGH_SCENARY = 5,
	DDIDX_INVISIBLE_SUPPORTS = 6,
	DDIDX_INVISIBLE_PEEPS = 7,
	DDIDX_LAND_HEIGHTS = 9,
	DDIDX_TRACK_HEIGHTS = 10,
	DDIDX_PATH_HEIGHTS = 11,
} TOP_TOOLBAR_VIEW_MENU_DDIDX;

void top_toolbar_init_view_menu(rct_window* window, rct_widget* widget);
void top_toolbar_view_menu_dropdown(short dropdownIndex);

void toggle_footpath_window();
void toggle_land_window(rct_window* topToolbar, int widgetIndex);
void toggle_clear_scenery_window(rct_window* topToolbar, int widgetIndex);
void toggle_water_window(rct_window* topToolbar, int widgetIndex);
#endif
