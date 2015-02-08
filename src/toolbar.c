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

#include "rct2.h"
#include "addresses.h"
#include "input.h"
#include "toolbar.h"
#include "localisation/string_ids.h"
#include "interface/viewport.h"
#include "interface/window.h"
#include "windows/dropdown.h"

/**
*
*  rct2: 0x0066CDE4
*/
void top_toolbar_init_view_menu(rct_window* w, rct_widget* widget) {
	gDropdownItemsFormat[0] = 1156;
	gDropdownItemsFormat[1] = 1156;
	gDropdownItemsFormat[2] = 1156;
	gDropdownItemsFormat[3] = 0;
	gDropdownItemsFormat[4] = 1156;
	gDropdownItemsFormat[5] = 1156;
	gDropdownItemsFormat[6] = 1156;
	gDropdownItemsFormat[7] = 1156;
	gDropdownItemsFormat[8] = 0;
	gDropdownItemsFormat[9] = 1156;
	gDropdownItemsFormat[10] = 1156;
	gDropdownItemsFormat[11] = 1156;

	gDropdownItemsArgs[0] = STR_UNDERGROUND_VIEW;
	gDropdownItemsArgs[1] = STR_REMOVE_BASE_LAND;
	gDropdownItemsArgs[2] = STR_REMOVE_VERTICAL_FACES;
	gDropdownItemsArgs[4] = STR_SEE_THROUGH_RIDES;
	gDropdownItemsArgs[5] = STR_SEE_THROUGH_SCENERY;
	gDropdownItemsArgs[6] = STR_INVISIBLE_SUPPORTS;
	gDropdownItemsArgs[7] = STR_INVISIBLE_PEOPLE;
	gDropdownItemsArgs[9] = STR_HEIGHT_MARKS_ON_LAND;
	gDropdownItemsArgs[10] = STR_HEIGHT_MARKS_ON_RIDE_TRACKS;
	gDropdownItemsArgs[11] = STR_HEIGHT_MARKS_ON_PATHS;

	window_dropdown_show_text(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		w->colours[1] | 0x80,
		0,
		12
		);

	// Set checkmarks
	rct_viewport* mainViewport = window_get_main()->viewport;
	if (mainViewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
		gDropdownItemsChecked |= (1 << 0);
	if (mainViewport->flags & VIEWPORT_FLAG_HIDE_BASE)
		gDropdownItemsChecked |= (1 << 1);
	if (mainViewport->flags & VIEWPORT_FLAG_HIDE_VERTICAL)
		gDropdownItemsChecked |= (1 << 2);
	if (mainViewport->flags & VIEWPORT_FLAG_SEETHROUGH_RIDES)
		gDropdownItemsChecked |= (1 << 4);
	if (mainViewport->flags & VIEWPORT_FLAG_SEETHROUGH_SCENERY)
		gDropdownItemsChecked |= (1 << 5);
	if (mainViewport->flags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
		gDropdownItemsChecked |= (1 << 6);
	if (mainViewport->flags & VIEWPORT_FLAG_INVISIBLE_PEEPS)
		gDropdownItemsChecked |= (1 << 7);
	if (mainViewport->flags & VIEWPORT_FLAG_LAND_HEIGHTS)
		gDropdownItemsChecked |= (1 << 9);
	if (mainViewport->flags & VIEWPORT_FLAG_TRACK_HEIGHTS)
		gDropdownItemsChecked |= (1 << 10);
	if (mainViewport->flags & VIEWPORT_FLAG_PATH_HEIGHTS)
		gDropdownItemsChecked |= (1 << 11);

	RCT2_GLOBAL(0x9DEBA2, uint16) = 0;
}

/**
*
*  rct2: 0x0066CF8A
*/
void top_toolbar_view_menu_dropdown(short dropdownIndex) {
	if (dropdownIndex == -1) dropdownIndex = RCT2_GLOBAL(0x9DEBA2, uint16);
	rct_window* w = window_get_main();
	if (w) {
		switch (dropdownIndex) {
		case DDIDX_UNDERGROUND_INSIDE:
			w->viewport->flags ^= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
			break;
		case DDIDX_HIDE_BASE:
			w->viewport->flags ^= VIEWPORT_FLAG_HIDE_BASE;
			break;
		case DDIDX_HIDE_VERTICAL:
			w->viewport->flags ^= VIEWPORT_FLAG_HIDE_VERTICAL;
			break;
		case DDIDX_SEETHROUGH_RIDES:
			w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_RIDES;
			break;
		case DDIDX_SEETHROUGH_SCENARY:
			w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_SCENERY;
			break;
		case DDIDX_INVISIBLE_SUPPORTS:
			w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_SUPPORTS;
			break;
		case DDIDX_INVISIBLE_PEEPS:
			w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_PEEPS;
			break;
		case DDIDX_LAND_HEIGHTS:
			w->viewport->flags ^= VIEWPORT_FLAG_LAND_HEIGHTS;
			break;
		case DDIDX_TRACK_HEIGHTS:
			w->viewport->flags ^= VIEWPORT_FLAG_TRACK_HEIGHTS;
			break;
		case DDIDX_PATH_HEIGHTS:
			w->viewport->flags ^= VIEWPORT_FLAG_PATH_HEIGHTS;
			break;
		default:
			return;
		}
		window_invalidate(w);
	}
}


/**
*
*  rct2: 0x0066CCE7
*/
void toggle_footpath_window() {
	if (window_find_by_class(WC_FOOTPATH) == NULL) {
		window_footpath_open();
	} else {
		tool_cancel();
		window_close_by_class(WC_FOOTPATH);
	}
}

/*
*
* rct2: 0x0066CD54
*/
void toggle_land_window(rct_window* topToolbar, int widgetIndex) {
	if ((RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == 1 && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) == 7) {
		tool_cancel();
	} else {
		show_gridlines();
		tool_set(topToolbar, widgetIndex, 18);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;
		window_land_open();
	}
}

/*
*
* rct2: 0x0066CD0C
*/
void toggle_clear_scenery_window(rct_window* topToolbar, int widgetIndex) {
	if ((RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == 1 && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) == 16) {
		tool_cancel();
	} else {
		show_gridlines();
		tool_set(topToolbar, widgetIndex, 12);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 2;
		window_clear_scenery_open();
	}
}

/*
*
* rct2: 0x0066CD9C
*/
void toggle_water_window(rct_window* topToolbar, int widgetIndex) {
	if ((RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == 1 && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) == 8) {
		tool_cancel();
	} else {
		show_gridlines();
		tool_set(topToolbar, widgetIndex, 19);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;
		window_water_open();
	}
}