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

#include "addresses.h"
#include "sprites.h"
#include "strings.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"
#include "viewport.h"

enum {
	WIDX_PAUSE,
	WIDX_FILE_MENU,
	WIDX_ZOOM_OUT,
	WIDX_ZOOM_IN,
	WIDX_ROTATE,
	WIDX_VIEW_MENU,
	WIDX_MAP,

	WIDX_LAND,
	WIDX_WATER,
	WIDX_SCENERY,
	WIDX_PATH,
	WIDX_CONSTRUCT_RIDE,
	WIDX_RIDES,
	WIDX_PARK,
	WIDX_STAFF,
	WIDX_GUESTS,
	WIDX_CLEAR_SCENERY,

	WIDX_FASTFORWARD
};

static rct_widget window_game_top_toolbar_widgets[] = {
	{ WWT_TRNBTN,	0,	0x0000,			0x001D,			0,		27,		0x20000000 | SPR_TOOLBAR_PAUSE,				STR_PAUSE_GAME_TIP },				// Pause
	{ WWT_TRNBTN,	0,	0x001E + 30,	0x003B + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_FILE,				STR_DISC_AND_GAME_OPTIONS_TIP },	// File menu
	{ WWT_TRNBTN,	1,	0x0046 + 30,	0x0063 + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_ZOOM_OUT,			STR_ZOOM_OUT_TIP },					// Zoom out
	{ WWT_TRNBTN,	1,	0x0064 + 30,	0x0081 + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_ZOOM_IN,			STR_ZOOM_IN_TIP },					// Zoom in
	{ WWT_TRNBTN,	1,	0x0082 + 30,	0x009F + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_ROTATE,			STR_ROTATE_TIP },					// Rotate camera
	{ WWT_TRNBTN,	1,	0x00A0 + 30,	0x00BD + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_VIEW,				STR_VIEW_OPTIONS_TIP },				// Transparancy menu
	{ WWT_TRNBTN,	1,	0x00BE + 30,	0x00DB + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_MAP,				STR_SHOW_MAP_TIP },					// Map

	{ WWT_TRNBTN,	2,	0x010B,	0x0128,	0,						27,		0x20000000 | SPR_TOOLBAR_LAND,				STR_ADJUST_LAND_TIP },				// Land
	{ WWT_TRNBTN,	2,	0x0129,	0x0146,	0,						27,		0x20000000 | SPR_TOOLBAR_WATER,				STR_ADJUST_WATER_TIP },				// Water
	{ WWT_TRNBTN,	2,	0x0147,	0x0164,	0,						27,		0x20000000 | SPR_TOOLBAR_SCENERY,			STR_PLACE_SCENERY_TIP },			// Scenery
	{ WWT_TRNBTN,	2,	0x0165,	0x0182,	0,						27,		0x20000000 | SPR_TOOLBAR_FOOTPATH,			STR_BUILD_FOOTPATH_TIP },			// Path
	{ WWT_TRNBTN,	2,	0x0183,	0x01A0,	0,						27,		0x20000000 | SPR_TOOLBAR_CONSTRUCT_RIDE,	STR_BUILD_RIDE_TIP },				// Construct ride
	{ WWT_TRNBTN,	3,	0x01EA,	0x0207,	0,						27,		0x20000000 | SPR_TOOLBAR_RIDES,				STR_RIDES_IN_PARK_TIP },			// Rides
	{ WWT_TRNBTN,	3,	0x0208,	0x0225,	0,						27,		0x20000000 | SPR_TOOLBAR_PARK,				STR_PARK_INFORMATION_TIP },			// Park
	{ WWT_TRNBTN,	3,	0x0226,	0x0243,	0,						27,		0x20000000 | 0x15F9,						STR_STAFF_TIP },					// Staff
	{ WWT_TRNBTN,	3,	0x0230,	0x024D,	0,						27,		0x20000000 | SPR_TOOLBAR_GUESTS,			STR_GUESTS_TIP },					// Guests
	{ WWT_TRNBTN,	2,	0x0230,	0x024D,	0,						27,		0x20000000 | SPR_TOOLBAR_CLEAR_SCENERY,		STR_CLEAR_SCENERY_TIP },			// Clear scenery

	{ WWT_TRNBTN,	0,	0x001E,	0x003B,	0,						27,		0x20000000 | 0x15F9,	STR_NONE },							// Fast forward

	{ WIDGETS_END },
};

static void window_game_top_toolbar_emptysub() { }
static void window_game_top_toolbar_mouseup();
static void window_game_top_toolbar_mousedown();
static void window_game_top_toolbar_dropdown();
static void window_game_top_toolbar_invalidate();
static void window_game_top_toolbar_paint();

static uint32 window_game_top_toolbar_events[] = {
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_mouseup,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_mousedown,
	window_game_top_toolbar_dropdown,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	0x0066CB25,
	0x0066CB73,
	0x0066CB4E,
	0x0066CC5B,
	0x0066CA58,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_invalidate,
	window_game_top_toolbar_paint,
	window_game_top_toolbar_emptysub
};

/**
 * Creates the main game top toolbar window.
 *  rct2: 0x0066B485 (part of 0x0066B3E8)
 */
void window_game_top_toolbar_open()
{
	rct_window* window;

	window = window_create(
		0, 0,
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16), 28,
		window_game_top_toolbar_events,
		WC_TOP_TOOLBAR,
		WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_5
	);
	window->widgets = window_game_top_toolbar_widgets;
	window->enabled_widgets |= (1 | 2 | 4 | 8 | 0x10 | 0x20 | 0x40 | 0x80 | 0x100 | 0x200 | 0x400 | 0x800 |
		0x1000 | 0x2000 | 0x4000 | 0x8000 | 0x10000 | 0x20000);
	window_init_scroll_widgets(window);
	window->colours[0] = 7;
	window->colours[1] = 12;
	window->colours[2] = 24;
	window->colours[3] = 1;
}

/**
 * 
 *  rct2: 0x0066C957
 */
static void window_game_top_toolbar_mouseup()
{
	short widgetIndex;
	rct_window *w, *mainWindow;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	switch (widgetIndex) {
	case WIDX_PAUSE:
		RCT2_CALLPROC_X(0x006677F2, 0, 1, 0, 0, 2, 0, 0);
		break;
	case WIDX_FASTFORWARD:
		window_cheats_open();
		break;

	case WIDX_ZOOM_OUT:
		if ((mainWindow = window_get_main()) != NULL)
			window_zoom_out(mainWindow);
		break;
	case WIDX_ZOOM_IN:
		if ((mainWindow = window_get_main()) != NULL)
			window_zoom_in(mainWindow);
		break;
	case WIDX_ROTATE:
		if ((mainWindow = window_get_main()) != NULL)
			window_rotate_camera(mainWindow);
		break;
	case WIDX_MAP:
		RCT2_CALLPROC_EBPSAFE(0x0068C88A);
		break;

	case WIDX_CLEAR_SCENERY:
		if ((RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)) && RCT2_GLOBAL(0x009DE544, uint8) == 1 && RCT2_GLOBAL(0x009DE546, uint16) == 16) {
			RCT2_CALLPROC_EBPSAFE(0x006EE281);
		} else {
			show_gridlines();
			RCT2_CALLPROC_X(0x006EE212, 12, 0, 0, WIDX_CLEAR_SCENERY, w, 0, 0);
			RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 6);
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 2;
			window_clear_scenery_open();
		}
		break;
	case WIDX_LAND:
		if ((RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)) && RCT2_GLOBAL(0x009DE544, uint8) == 1 && RCT2_GLOBAL(0x009DE546, uint16) == 7) {
			RCT2_CALLPROC_EBPSAFE(0x006EE281);
		} else {
			show_gridlines();
			RCT2_CALLPROC_X(0x006EE212, 18, 0, 0, WIDX_LAND, w, 0, 0);
			RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 6);
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;
			window_land_open();
		}
		break;
	case WIDX_WATER:
		if ((RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)) && RCT2_GLOBAL(0x009DE544, uint8) == 1 && RCT2_GLOBAL(0x009DE546, uint16) == 8) {
			RCT2_CALLPROC_EBPSAFE(0x006EE281);
		} else {
			show_gridlines();
			RCT2_CALLPROC_X(0x006EE212, 19, 0, 0, WIDX_WATER, w, 0, 0);
			RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 6);
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;
			window_water_open();
		}
		break;
	case WIDX_SCENERY:
		RCT2_CALLPROC_X(0x006EE212, 0, 0, 0, WIDX_SCENERY, w, 0, 0);
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 6);
		RCT2_CALLPROC_EBPSAFE(0x006E0FEF);
		break;
	case WIDX_PATH:
		if (window_find_by_id(WC_FOOTPATH, 0) == NULL) {
			window_footpath_open();
		} else {
			RCT2_CALLPROC_EBPSAFE(0x006EE281);
			window_close_by_id(0x80 | WC_FOOTPATH, 0);
		}
		break;
	case WIDX_CONSTRUCT_RIDE:
		RCT2_CALLPROC_EBPSAFE(0x006B3CFF);
		break;
	case WIDX_RIDES:
		window_ride_list_open();
		break;
	case WIDX_PARK:
		window_park_entrance_open();
		break;
	case WIDX_STAFF:
		RCT2_CALLPROC_EBPSAFE(0x006BD3CC);
		break;
	case WIDX_GUESTS:
		RCT2_CALLPROC_EBPSAFE(0x006992E3);
		break;
	}
}

/**
 * 
 *  rct2: 0x0066CA3B
 */
static void window_game_top_toolbar_mousedown()
{
	short widgetIndex;
	rct_window *w;
	rct_widget *widget;
	rct_viewport *mainViewport;

	__asm mov widgetIndex, dx
	__asm mov w, esi
	__asm mov widget, edi

	if (widgetIndex == WIDX_FILE_MENU) {
		gDropdownItemsFormat[0] = 882;
		gDropdownItemsFormat[1] = 883;
		gDropdownItemsFormat[2] = 0;
		gDropdownItemsFormat[3] = 847;
		gDropdownItemsFormat[4] = 2327;
		gDropdownItemsFormat[5] = 891;
		gDropdownItemsFormat[6] = 0;
		gDropdownItemsFormat[7] = 886;
		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[0] | 0x80,
			0x80,
			8
		);
	} else if (widgetIndex == WIDX_VIEW_MENU) {
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

		gDropdownItemsArgs[0] = 939;
		gDropdownItemsArgs[1] = 940;
		gDropdownItemsArgs[2] = 941;
		gDropdownItemsArgs[4] = 942;
		gDropdownItemsArgs[5] = 943;
		gDropdownItemsArgs[6] = 1051;
		gDropdownItemsArgs[7] = 1052;
		gDropdownItemsArgs[9] = 1154;
		gDropdownItemsArgs[10] = 1153;
		gDropdownItemsArgs[11] = 1155;

		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[1] | 0x80,
			0,
			12
		);

		// Set checkmarks
		mainViewport = window_get_main()->viewport;
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
	}
}

/**
 * 
 *  rct2: 0x0066C9EA
 */
static void window_game_top_toolbar_dropdown()
{
	short widgetIndex, dropdownIndex;

	__asm mov widgetIndex, dx
	__asm mov dropdownIndex, ax

	if (widgetIndex == WIDX_FILE_MENU) {
		switch (dropdownIndex) {
		case 0:		// load game
			RCT2_CALLPROC_X(0x006677F2, 0, 1, 0, 0, 5, 0, 0);
			break;
		case 1:		// save game
			RCT2_CALLPROC_EBPSAFE(0x006EE281);
			{
				int eax, ebx, ecx, edx, esi, edi, ebp;
				RCT2_CALLFUNC_X(0x006750E9, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
				if (eax == 0) {
					gfx_invalidate_screen();
					break;
				}

				char *src = 0x0141EF67;
				do {
					src++;
				} while (*src != '.' && *src != '\0');
				strcpy(src, ".SV6");
				strcpy(0x009ABB37, 0x0141EF68);

				eax = 0;
				if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & 8)
					eax |= 1;
				RCT2_CALLPROC_X(0x006754F5, eax, 0, 0, 0, 0, 0, 0);
				// check success?

				RCT2_CALLPROC_X(0x006677F2, 0, 1047, 0, -1, 0, 0, 0);
				gfx_invalidate_screen();
			}
			break;
		case 3:		// about
			window_about_open();
			break;
		case 4:		// options
			RCT2_CALLPROC_EBPSAFE(0x006BAC5B);
			break;
		case 5:		// screenshot
			RCT2_GLOBAL(0x009DEA6D, sint8) = 10;
			break;
		case 7:		// quit game
			RCT2_CALLPROC_X(0x006677F2, 0, 1, 0, 0, 5, 1, 0);
			break;
		}
	} else if (widgetIndex == WIDX_VIEW_MENU) {
		RCT2_CALLPROC_X(0x0066CF8A, dropdownIndex, 0, 0, 0, 0, 0, 0);
	}
}

/**
 * 
 *  rct2: 0x0066C810
 */
static void window_game_top_toolbar_invalidate()
{
	int x;
	rct_window *w;

	__asm mov w, esi

	// Anchor the right half of the buttons to the right
	x = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16);
	if (x < 640)
		x = 640;
	x--;
	window_game_top_toolbar_widgets[WIDX_GUESTS].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_GUESTS].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_STAFF].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_STAFF].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_PARK].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_PARK].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_RIDES].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_RIDES].left = x;
	x -= 11;
	window_game_top_toolbar_widgets[WIDX_CONSTRUCT_RIDE].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_CONSTRUCT_RIDE].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_PATH].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_PATH].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_SCENERY].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_SCENERY].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_WATER].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_WATER].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_LAND].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_LAND].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_CLEAR_SCENERY].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_CLEAR_SCENERY].left = x;

	// Footpath button pressed down
	if (window_find_by_id(0x80 | 0x20, 0) == NULL)
		w->pressed_widgets &= ~(1 << WIDX_PATH);
	else
		w->pressed_widgets |= (1 << WIDX_PATH);

	// Fast forward button pressed down
	if (0)
		w->pressed_widgets |= (1 << WIDX_FASTFORWARD);
	else
		w->pressed_widgets &= ~(1 << WIDX_FASTFORWARD);
}

/**
 * 
 *  rct2: 0x0066C8EC
 */
static void window_game_top_toolbar_paint()
{
	int x, y, imgId;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);

	// Draw staff button image (setting masks to the staff colours)
	x = w->x + window_game_top_toolbar_widgets[WIDX_STAFF].left;
	y = w->y + window_game_top_toolbar_widgets[WIDX_STAFF].top;
	imgId = 5627;
	if (widget_is_pressed(w, WIDX_STAFF))
		imgId++;
	imgId |= (RCT2_GLOBAL(0x01357BCD, uint8) << 19) | 0xA0000000 | (RCT2_GLOBAL(0x01357BCE, uint8) << 24);
	gfx_draw_sprite(dpi, imgId, x, y);
}
