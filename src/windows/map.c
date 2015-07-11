/*****************************************************************************
 * Copyright (c) 2014 Ted John, Dennis Devriendt
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

#include "../addresses.h"
#include "../audio/audio.h"
#include "../cheats.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/viewport.h"
#include "../interface/window.h"
#include "../sprites.h"
#include "../world/footpath.h"
#include "../world/scenery.h"
#include "error.h"

enum {
	PAGE_PEEPS,
	PAGE_RIDES
};

enum WINDOW_MAP_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_RESIZE = 3,
	WIDX_PEOPLE_TAB = 4,
	WIDX_RIDES_TAB = 5,
	WIDX_MAP = 6,
	WIDX_MAP_SIZE_SPINNER = 7,
	WIDX_MAP_SIZE_SPINNER_UP = 8,
	WIDX_MAP_SIZE_SPINNER_DOWN = 9,
	WIDX_SET_LAND_RIGHTS = 10,
	WIDX_BUILD_PARK_ENTRANCE = 11,
	WIDX_PEOPLE_STARTING_POSITION = 12,
	WIDX_LAND_TOOL = 13,
	WIDX_LAND_TOOL_SMALLER = 14,
	WIDX_LAND_TOOL_LARGER = 15,
	WIDX_LAND_OWNED_CHECKBOX = 16,
	WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX = 17,
	WIDX_LAND_SALE_CHECKBOX = 18,
	WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX = 19,
	WIDX_ROTATE_90 = 20
};

static rct_widget window_map_widgets[] = {
	{ WWT_FRAME,			0,	0,		244,	0,		258,	STR_NONE,							STR_NONE },
	{ WWT_CAPTION,			0,	1,		243,	1,		14,		STR_MAP,							STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	232,	242,	2,		13,		STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		244,	43,		257,	STR_NONE,							STR_NONE },
	{ WWT_COLORBTN,			1,	3,		33,		17,		43,		0x02000144E,						STR_SHOW_PEOPLE_ON_MAP_TIP },
	{ WWT_COLORBTN,			1,	34,		64,		17,		43,		0x02000144E,						STR_SHOW_RIDES_STALLS_ON_MAP_TIP },
	{ WWT_SCROLL,			1,	3,		241,	46,		225,	0x3,								STR_NONE },
	{ WWT_SPINNER,			1,	104,	198,	229,	240,	0xC8C,								STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	230,	234,	STR_NUMERIC_UP,						STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	235,	239,	STR_NUMERIC_DOWN,					STR_NONE },
	{ WWT_FLATBTN,			1,	4,		27,		1,		24,		SPR_BUY_LAND_RIGHTS,				STR_SELECT_PARK_OWNED_LAND_TIP },
	{ WWT_FLATBTN,			1,	4,		27,		1,		24,		SPR_PARK_ENTRANCE,					STR_BUILD_PARK_ENTRANCE_TIP },
	{ WWT_FLATBTN,			1,	28,		51,		1,		24,		STR_NONE,							STR_SET_STARTING_POSITIONS_TIP },
	{ WWT_IMGBTN,			1,	4,		47,		17,		48,		SPR_LAND_TOOL_SIZE_0,				STR_NONE },
	{ WWT_TRNBTN,			1,	5,		20,		18,		33,		0x02000157B,						STR_ADJUST_SMALLER_LAND_TIP },
	{ WWT_TRNBTN,			1,	31,		46,		32,		47,		0x02000157D,						STR_ADJUST_LARGER_LAND_TIP },
	{ WWT_CHECKBOX,			1,	58,		241,	197,	208,	STR_LAND_OWNED,						STR_SET_LAND_TO_BE_OWNED_TIP },
	{ WWT_CHECKBOX,			1,	58,		241,	197,	208,	STR_CONSTRUCTION_RIGHTS_OWNED,		STR_SET_CONSTRUCTION_RIGHTS_TO_BE_OWNED_TIP },
	{ WWT_CHECKBOX,			1,	58,		241,	197,	208,	STR_LAND_SALE,						STR_SET_LAND_TO_BE_AVAILABLE_TIP },
	{ WWT_CHECKBOX,			1,	58,		231,	197,	208,	STR_CONSTRUCTION_RIGHTS_SALE,		STR_SET_CONSTRUCTION_RIGHTS_TO_BE_AVAILABLE_TIP },
	{ WWT_FLATBTN,			1,	218,	241,	45,		68,		SPR_ROTATE_ARROW,					STR_ROTATE_OBJECTS_90 },
	{ WIDGETS_END },
};

// used in transforming viewport view coordinates to minimap coordinates
// rct2: 0x00981BBC
const rct_xy16 MiniMapOffsets[] = {
	{ 256 - 8,   0 },
	{ 512 - 8, 256 },
	{ 256 - 8, 512 },
	{   0 - 8, 256 }
};

static void window_map_close(rct_window *w);
static void window_map_resize(rct_window *w);
static void window_map_mouseup(rct_window *w, int widgetIndex);
static void window_map_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_map_update(rct_window *w);
static void window_map_toolupdate(rct_window* w, int widgetIndex, int x, int y);
static void window_map_tooldown(rct_window* w, int widgetIndex, int x, int y);
static void window_map_tooldrag(rct_window* w, int widgetIndex, int x, int y);
static void window_map_toolabort(rct_window *w, int widgetIndex);
static void window_map_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_map_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_map_textinput(rct_window *w, int widgetIndex, char *text);
static void window_map_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
static void window_map_invalidate(rct_window *w);
static void window_map_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_map_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_map_events = {
	window_map_close,
	window_map_mouseup,
	window_map_resize,
	window_map_mousedown,
	NULL,
	NULL,
	window_map_update,
	NULL,
	NULL,
	window_map_toolupdate,
	window_map_tooldown,
	window_map_tooldrag,
	NULL,
	window_map_toolabort,
	NULL,
	window_map_scrollgetsize,
	window_map_scrollmousedown,
	window_map_scrollmousedown,
	NULL,
	window_map_textinput,
	NULL,
	NULL,
	window_map_tooltip,
	NULL,
	NULL,
	window_map_invalidate,
	window_map_paint,
	window_map_scrollpaint
};


static void window_map_init_map();
static void window_map_center_on_view_point();
static void window_map_show_default_scenario_editor_buttons(rct_window *w);
static void window_map_draw_tab_images(rct_window *w, rct_drawpixelinfo *dpi);
static void window_map_paint_peep_overlay(rct_drawpixelinfo *dpi);
static void window_map_paint_train_overlay(rct_drawpixelinfo *dpi);
static void window_map_paint_hud_rectangle(rct_drawpixelinfo *dpi);
static void window_map_inputsize_land(rct_window *w);
static void window_map_inputsize_map(rct_window *w);
static void window_map_set_bounds(rct_window* w);

static void window_map_set_land_rights_tool_update(int x, int y);
static void window_map_place_park_entrance_tool_update(int x, int y);
static void window_map_set_peep_spawn_tool_update(int x, int y);
static void window_map_place_park_entrance_tool_down(int x, int y);
static void window_map_set_peep_spawn_tool_down(int x, int y);
static void map_window_increase_map_size();
static void map_window_decrease_map_size();
static void map_window_set_pixels(rct_window *w);

static void map_window_screen_to_map(int screenX, int screenY, int *mapX, int *mapY);

/**
*
*  rct2: 0x0068C88A
*/
void window_map_open()
{
	rct_window *w;
	uint32 *map_image_data;

	// Check if window is already open
	w = window_bring_to_front_by_class(WC_MAP);
	if (w != NULL) {
		w->selected_tab = 0;
		w->list_information_type = 0;
		return;
	}

	map_image_data = malloc(256 * 256 * sizeof(uint32));
	if (map_image_data == NULL)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_IMAGE_DATA, uint32*) = map_image_data;
	w = window_create_auto_pos(245, 259, &window_map_events, WC_MAP, WF_10);
	w->widgets = window_map_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_PEOPLE_TAB) |
		(1 << WIDX_RIDES_TAB) |
		(1 << WIDX_MAP_SIZE_SPINNER) |
		(1 << WIDX_MAP_SIZE_SPINNER_UP) |
		(1 << WIDX_MAP_SIZE_SPINNER_DOWN) |
		(1 << WIDX_LAND_TOOL) |
		(1 << WIDX_LAND_TOOL_SMALLER) |
		(1 << WIDX_LAND_TOOL_LARGER) |
		(1 << WIDX_SET_LAND_RIGHTS) |
		(1 << WIDX_LAND_OWNED_CHECKBOX) |
		(1 << WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX) |
		(1 << WIDX_LAND_SALE_CHECKBOX) |
		(1 << WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX) |
		(1 << WIDX_BUILD_PARK_ENTRANCE) |
		(1 << WIDX_ROTATE_90) |
		(1 << WIDX_PEOPLE_STARTING_POSITION);

	w->hold_down_widgets =
		(1 << WIDX_MAP_SIZE_SPINNER_UP) |
		(1 << WIDX_MAP_SIZE_SPINNER_DOWN);

	window_init_scroll_widgets(w);

	w->map.rotation = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint16);

	window_map_init_map();
	RCT2_GLOBAL(0x00F64F05, uint8) = 0;
	window_map_center_on_view_point();
}

/**
*
*  rct2: 0x0068D0F1
*/
static void window_map_close(rct_window *w)
{
	free(RCT2_GLOBAL(RCT2_ADDRESS_MAP_IMAGE_DATA, uint32*));
	if ((RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) &&
		RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == w->classification &&
		RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, uint16) == w->number) {
		tool_cancel();
	}

	// Reset land tool size
	RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 0;
}

/**
 *
 *  rct2: 0x0068CFC1
 */
static void window_map_mouseup(rct_window *w, int widgetIndex)
{
	// Maximum land ownership tool size
	int landToolSizeLimit;

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_SET_LAND_RIGHTS:
		window_invalidate(w);
		if (tool_set(w, widgetIndex, 2))
			break;

		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;
		RCT2_GLOBAL(0xF1AD61, sint8) = 2;
		show_gridlines();
		show_land_rights();
		show_construction_rights();
		break;
	case WIDX_LAND_OWNED_CHECKBOX:
		RCT2_GLOBAL(0xF1AD61, sint8) ^= 2;

		if (RCT2_GLOBAL(0xF1AD61, sint8) & 2)
			RCT2_GLOBAL(0xF1AD61, sint8) &= 0xF2;

		window_invalidate(w);
		break;
	case WIDX_LAND_SALE_CHECKBOX:
		RCT2_GLOBAL(0xF1AD61, sint8) ^= 8;

		if (RCT2_GLOBAL(0xF1AD61, sint8) & 8)
			RCT2_GLOBAL(0xF1AD61, sint8) &= 0xF8;

		window_invalidate(w);
		break;
	case WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX:
		RCT2_GLOBAL(0xF1AD61, sint8) ^= 1;

		if (RCT2_GLOBAL(0xF1AD61, sint8) & 1)
			RCT2_GLOBAL(0xF1AD61, sint8) &= 0xF1;

		window_invalidate(w);
		break;
	case WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX:
		RCT2_GLOBAL(0xF1AD61, sint8) ^= 4;

		if (RCT2_GLOBAL(0xF1AD61, sint8) & 4)
			RCT2_GLOBAL(0xF1AD61, sint8) &= 0xF4;

		window_invalidate(w);
		break;
	case WIDX_LAND_TOOL_SMALLER:
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16)--;
		landToolSizeLimit = 1;

		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) < landToolSizeLimit)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = landToolSizeLimit;

		window_invalidate(w);
		break;
	case WIDX_LAND_TOOL_LARGER:
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16)++;
		landToolSizeLimit = 64;

		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) > landToolSizeLimit)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = landToolSizeLimit;

		window_invalidate(w);
		break;
	case WIDX_BUILD_PARK_ENTRANCE:
		window_invalidate(w);
		if (tool_set(w, widgetIndex, 2))
			break;

		RCT2_GLOBAL(0x9E32D2, sint8) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;

		show_gridlines();
		show_land_rights();
		show_construction_rights();
		break;
	case WIDX_ROTATE_90:
		window_scenery_rotation = (window_scenery_rotation + 1) & 3;
		break;
	case WIDX_PEOPLE_STARTING_POSITION:
		if (tool_set(w, widgetIndex, 2))
			break;

		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_PEEP_SPAWNS, sint16) != -1 && RCT2_GLOBAL(0x13573F8, sint16) != -1)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;

		show_gridlines();
		show_land_rights();
		show_construction_rights();
		break;
	case WIDX_LAND_TOOL:
		window_map_inputsize_land(w);
		break;
	case WIDX_MAP_SIZE_SPINNER:
		window_map_inputsize_map(w);
		break;
	default:
		if (widgetIndex >= WIDX_PEOPLE_TAB && widgetIndex <= WIDX_RIDES_TAB) {
			widgetIndex -= WIDX_PEOPLE_TAB;
			if (widgetIndex == w->selected_tab)
				break;

			w->selected_tab = widgetIndex;
			w->list_information_type = 0;
		}
		break;
	}
}

/**
*
*  rct2: 0x0068D7DC
*/
static void window_map_resize(rct_window *w)
{
	w->flags |= WF_RESIZABLE;
	w->min_width = 245;
	w->max_width = 800;
	w->min_height = 259;
	w->max_height = 560;
}

/**
 *
 *  rct2: 0x0068D040
 */
static void window_map_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	switch (widgetIndex) {
	case WIDX_MAP_SIZE_SPINNER_UP:
		map_window_increase_map_size();
		break;
	case WIDX_MAP_SIZE_SPINNER_DOWN:
		map_window_decrease_map_size();
		break;
	case WIDX_SET_LAND_RIGHTS:
		// When unselecting the land rights tool, reset the size so the number doesn't
		// stay in the map window.
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;
		break;
	}
}

/**
 *
 *  rct2: 0x0068D7FB
 */
static void window_map_update(rct_window *w)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) != w->map.rotation) {
		w->map.rotation = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8);
		window_map_init_map();
		window_map_center_on_view_point();
	}

	for (int i = 0; i < 16; i++)
		map_window_set_pixels(w);

	window_invalidate(w);

	// Update tab animations
	w->list_information_type++;
	switch (w->selected_tab) {
	case PAGE_PEEPS:
		if (w->list_information_type >= 32) {
			w->list_information_type = 0;
		}
		break;
	case PAGE_RIDES:
		if (w->list_information_type >= 64) {
			w->list_information_type = 0;
		}
		break;
	}
}

/**
 *
 *  rct2: 0x0068D093
 */
static void window_map_toolupdate(rct_window* w, int widgetIndex, int x, int y)
{
	switch (widgetIndex) {
	case WIDX_SET_LAND_RIGHTS:
		window_map_set_land_rights_tool_update(x, y);
		break;
	case WIDX_BUILD_PARK_ENTRANCE:
		window_map_place_park_entrance_tool_update(x, y);
		break;
	case WIDX_PEOPLE_STARTING_POSITION:
		window_map_set_peep_spawn_tool_update(x, y);
		break;
	}
}

/**
 *
 *  rct2: 0x0068D074
 */
static void window_map_tooldown(rct_window* w, int widgetIndex, int x, int y)
{
	switch (widgetIndex) {
	case WIDX_BUILD_PARK_ENTRANCE:
		window_map_place_park_entrance_tool_down(x, y);
		break;
	case WIDX_PEOPLE_STARTING_POSITION:
		window_map_set_peep_spawn_tool_down(x, y);
		break;
	}
}

/**
 *
 *  rct2: 0x0068D088
 */
static void window_map_tooldrag(rct_window* w, int widgetIndex, int x, int y)
{
	switch (widgetIndex) {
	case WIDX_SET_LAND_RIGHTS:
		if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0)) {
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = 0;
			game_do_command(
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16),
				GAME_COMMAND_FLAG_APPLY,
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16),
				RCT2_GLOBAL(0x00F1AD61, uint8),
				GAME_COMMAND_SET_LAND_OWNERSHIP,
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16),
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16)
			);
		}
		break;
	}
}

/**
 *
 *  rct2: 0x0068D055
 */
static void window_map_toolabort(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_SET_LAND_RIGHTS:
		window_invalidate(w);
		hide_gridlines();
		hide_land_rights();
		hide_construction_rights();
		break;
	case WIDX_BUILD_PARK_ENTRANCE:
		park_remove_ghost_entrance();
		window_invalidate(w);
		hide_gridlines();
		hide_land_rights();
		hide_construction_rights();
		break;
	case WIDX_PEOPLE_STARTING_POSITION:
		window_invalidate(w);
		hide_gridlines();
		hide_land_rights();
		hide_construction_rights();
		break;
	}
}

/**
 *
 *  rct2: 0x0068D7CC
 */
static void window_map_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	window_map_invalidate(w);

	*width = 512;
	*height = 512;
}

/**
 *
 *  rct2: 0x0068D726
 */
static void window_map_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	int mapX, mapY, mapZ;
	rct_window *mainWindow;

	map_window_screen_to_map(x, y, &mapX, &mapY);
	mapX = clamp(0, mapX, 8191);
	mapY = clamp(0, mapY, 8191);
	mapZ = map_element_height(x, y);

	mainWindow = window_get_main();
	if (mainWindow != NULL) {
		window_scroll_to_location(mainWindow, mapX, mapY, mapZ);
	}

	if (land_tool_is_active()) {
		// Set land terrain
		int landToolSize = max(1, RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, uint16));
		int size = (landToolSize * 32) - 32;
		int radius = (landToolSize * 16) - 16;
		mapX = (mapX - radius) & 0xFFE0;
		mapY = (mapY - radius) & 0xFFE0;

		map_invalidate_selection_rect();
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16) = mapX;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16) = mapY;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16) = mapX + size;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16) = mapY + size;
		map_invalidate_selection_rect();

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1387;
		game_do_command(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
			GAME_COMMAND_FLAG_APPLY,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) | (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) << 8),
			GAME_COMMAND_CHANGE_SURFACE_STYLE,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16)
		);
	} else if (widget_is_active_tool(w, WIDX_SET_LAND_RIGHTS)) {
		// Set land rights
		int landToolSize = max(1, RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, uint16));
		int size = (landToolSize * 32) - 32;
		int radius = (landToolSize * 16) - 16;
		mapX = (mapX - radius) & 0xFFE0;
		mapY = (mapY - radius) & 0xFFE0;

		map_invalidate_selection_rect();
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16) = mapX;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16) = mapY;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16) = mapX + size;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16) = mapY + size;
		map_invalidate_selection_rect();

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = 0;
		game_do_command(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16),
			GAME_COMMAND_FLAG_APPLY,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16),
			RCT2_GLOBAL(0x00F1AD61, uint8),
			GAME_COMMAND_SET_LAND_OWNERSHIP,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16)
		);
	}
}

static void window_map_textinput(rct_window *w, int widgetIndex, char *text)
{
	int size;
	char* end;

	if (text == NULL)
		return;

	switch (widgetIndex) {
	case WIDX_LAND_TOOL:
		size = strtol(text, &end, 10);
		if (*end == '\0') {
			if (size < 1) size = 1;
			if (size > 64) size = 64;
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = size;
			window_invalidate(w);
		}
		break;
	case WIDX_MAP_SIZE_SPINNER:
		size = strtol(text, &end, 10);
		if (*end == '\0') {
			if (size < 50) size = 50;
			if (size > 256) size = 256;
			int currentSize = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16);
			while (size < currentSize) {
				map_window_decrease_map_size();
				currentSize--;
			}
			while (size > currentSize) {
				map_window_increase_map_size();
				currentSize++;
			}
			window_invalidate(w);
		}
		break;
	}
}

/*
 *
 *  rct2: 0x0068D140
 */
static void window_map_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, short) = 3157;
}

/**
 *
 *  rct2: 0x0068CA8F
 */
static void window_map_invalidate(rct_window *w)
{
	uint64 pressedWidgets;
	int i, height;

	colour_scheme_update(w);

	// Set the pressed widgets
	pressedWidgets = w->pressed_widgets;
	pressedWidgets &= (1ULL << WIDX_PEOPLE_TAB);
	pressedWidgets &= (1ULL << WIDX_RIDES_TAB);
	pressedWidgets &= (1ULL << WIDX_MAP);
	pressedWidgets &= (1ULL << WIDX_LAND_OWNED_CHECKBOX);
	pressedWidgets &= (1ULL << WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX);
	pressedWidgets &= (1ULL << WIDX_LAND_SALE_CHECKBOX);
	pressedWidgets &= (1ULL << WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX);

	pressedWidgets |= (1ULL << (WIDX_PEOPLE_TAB + w->selected_tab));
	pressedWidgets |= (1ULL << WIDX_LAND_TOOL);

	if (RCT2_GLOBAL(0x00F1AD61, uint8) & (1 << 3))
		pressedWidgets |= (1 << WIDX_LAND_SALE_CHECKBOX);

	if (RCT2_GLOBAL(0x00F1AD61, uint8) & (1 << 2))
		pressedWidgets |= (1 << WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX);

	if (RCT2_GLOBAL(0x00F1AD61, uint8) & (1 << 1))
		pressedWidgets |= (1 << WIDX_LAND_OWNED_CHECKBOX);

	if (RCT2_GLOBAL(0x00F1AD61, uint8) & (1 << 0))
		pressedWidgets |= (1 << WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX);

	w->pressed_widgets = pressedWidgets;

	// Resize widgets to window size
	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_RESIZE].right = w->width - 1;
	w->widgets[WIDX_RESIZE].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 2 - 11;
	w->widgets[WIDX_CLOSE].right = w->width - 2 - 11 + 10;
	w->widgets[WIDX_MAP].right = w->width - 4;

	if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode)
		w->widgets[WIDX_MAP].bottom = w->height - 1 - 72;
	else if (w->selected_tab == PAGE_RIDES)
		w->widgets[WIDX_MAP].bottom = w->height - 1 - 44;
	else
		w->widgets[WIDX_MAP].bottom = w->height - 1 - 14;

	w->widgets[WIDX_MAP_SIZE_SPINNER].top = w->height - 15;
	w->widgets[WIDX_MAP_SIZE_SPINNER].bottom = w->height - 4;
	w->widgets[WIDX_MAP_SIZE_SPINNER_UP].top = w->height - 14;
	w->widgets[WIDX_MAP_SIZE_SPINNER_UP].bottom = w->height - 10;
	w->widgets[WIDX_MAP_SIZE_SPINNER_DOWN].top = w->height - 9;
	w->widgets[WIDX_MAP_SIZE_SPINNER_DOWN].bottom = w->height - 5;

	w->widgets[WIDX_SET_LAND_RIGHTS].top = w->height - 70;
	w->widgets[WIDX_SET_LAND_RIGHTS].bottom = w->height - 70 + 23;
	w->widgets[WIDX_BUILD_PARK_ENTRANCE].top = w->height - 46;
	w->widgets[WIDX_BUILD_PARK_ENTRANCE].bottom = w->height - 46 + 23;
	w->widgets[WIDX_ROTATE_90].top = w->height - 46;
	w->widgets[WIDX_ROTATE_90].bottom = w->height - 46 + 23;
	w->widgets[WIDX_PEOPLE_STARTING_POSITION].top = w->height - 46;
	w->widgets[WIDX_PEOPLE_STARTING_POSITION].bottom = w->height - 46 + 23;

	w->widgets[WIDX_LAND_TOOL].top = w->height - 42;
	w->widgets[WIDX_LAND_TOOL].bottom = w->height - 42 + 30;
	w->widgets[WIDX_LAND_TOOL_SMALLER].top = w->height - 41;
	w->widgets[WIDX_LAND_TOOL_SMALLER].bottom = w->height - 41 + 15;
	w->widgets[WIDX_LAND_TOOL_LARGER].top = w->height - 27;
	w->widgets[WIDX_LAND_TOOL_LARGER].bottom = w->height - 27 + 15;

	// Land tool mode (4 checkboxes)
	height = w->height - 55;
	for (i = 0; i < 4; i++) {
		w->widgets[WIDX_LAND_OWNED_CHECKBOX + i].top = height;
		height += 11;
		w->widgets[WIDX_LAND_OWNED_CHECKBOX + i].bottom = height;
		height += 2;
	}

	// Disable all scenario editor related widgets
	for (i = WIDX_MAP_SIZE_SPINNER; i <= WIDX_ROTATE_90; i++) {
		w->widgets[i].type = WWT_EMPTY;
	}

	if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) {
		// scenario editor: build park entrance selected, show rotate button
		if (
			(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) &&
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == WC_MAP &&
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint8) == WIDX_BUILD_PARK_ENTRANCE
		) {
			w->widgets[WIDX_ROTATE_90].type = WWT_FLATBTN;
		}

		// Always show set land rights button
		w->widgets[WIDX_SET_LAND_RIGHTS].type = WWT_FLATBTN;

		// If any tool is active
		if (
			(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) &&
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == WC_MAP
		) {
			// if not in set land rights mode: show the default scenario editor buttons
			if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint8) != WIDX_SET_LAND_RIGHTS) {
				window_map_show_default_scenario_editor_buttons(w);
			} else { // if in set land rights mode: show land tool buttons + modes
				w->widgets[WIDX_LAND_TOOL].type = WWT_IMGBTN;
				w->widgets[WIDX_LAND_TOOL_SMALLER].type = WWT_TRNBTN;
				w->widgets[WIDX_LAND_TOOL_LARGER].type = WWT_TRNBTN;

				for (i = 0; i < 4; i++)
					w->widgets[WIDX_LAND_OWNED_CHECKBOX + i].type = WWT_CHECKBOX;

				w->widgets[WIDX_LAND_TOOL].image = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, uint16) <= 7 ?
					SPR_LAND_TOOL_SIZE_0 + RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, uint16) :
					0xFFFFFFFF;
			}
		} else {
			// if no tool is active: show the default scenario editor buttons
			window_map_show_default_scenario_editor_buttons(w);
		}
	}
}

/**
 *
 *  rct2: 0x0068CDA9
 */
static void window_map_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	int i, x, y;

	window_draw_widgets(w, dpi);
	window_map_draw_tab_images(w, dpi);

	x = w->x + (window_map_widgets[WIDX_LAND_TOOL].left + window_map_widgets[WIDX_LAND_TOOL].right) / 2;
	y = w->y + (window_map_widgets[WIDX_LAND_TOOL].top + window_map_widgets[WIDX_LAND_TOOL].bottom) / 2;

	// Draw land tool size
	if (widget_is_active_tool(w, WIDX_SET_LAND_RIGHTS) && RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) > 7) {
		RCT2_GLOBAL(0x009BC677, char) = FORMAT_BLACK;
		RCT2_GLOBAL(0x009BC678, char) = FORMAT_COMMA16;
		RCT2_GLOBAL(0x009BC679, char) = 0;
		RCT2_GLOBAL(0x013CE952, sint16) = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16);
		gfx_draw_string_centred(dpi, 3165, x, y - 2, 0, (void*)0x013CE952);
	}
	y = w->y + window_map_widgets[WIDX_LAND_TOOL].bottom + 5;

	// People starting position (scenario editor only)
	if (w->widgets[WIDX_PEOPLE_STARTING_POSITION].type != 0) {
		x = w->x + w->widgets[WIDX_PEOPLE_STARTING_POSITION].left + 12;
		y = w->y + w->widgets[WIDX_PEOPLE_STARTING_POSITION].top + 18;
		gfx_draw_sprite(dpi, 0x0B6E0190A, x, y, 0);
	}

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode) {
		// Render the map legend
		if (w->selected_tab == PAGE_RIDES) {
			x = w->x + 4;
			y = w->y + w->widgets[WIDX_MAP].bottom + 2;

			for (i = 0; i < 8; i++) {
				gfx_fill_rect(dpi, x, y + 2, x + 6, y + 8, RCT2_GLOBAL(0x00981BCC + (i * 2), uint8));
				gfx_draw_string_left(dpi, STR_MAP_RIDE + i, w, 0, x + 10, y);
				y += 10;
				if (i == 3) {
					x += 118;
					y -= 40;
				}
			}
		}
	} else if (!widget_is_active_tool(w, WIDX_SET_LAND_RIGHTS)) {
		gfx_draw_string_left(dpi, STR_MAP_SIZE, 0, 0, w->x + 4, w->y + w->widgets[WIDX_MAP_SIZE_SPINNER].top + 1);
	}
}

/**
 *
 *  rct2: 0x0068CF23
 */
static void window_map_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	rct_g1_element *g1_element, pushed_g1_element;

	gfx_clear(dpi, 0x0A0A0A0A);

	g1_element = &g1Elements[0];
	pushed_g1_element = *g1_element;

	g1_element->offset = RCT2_GLOBAL(RCT2_ADDRESS_MAP_IMAGE_DATA, uint8*);
	g1_element->width = 0x200;
	g1_element->height = 0x200;
	g1_element->x_offset = 0xFFF8;
	g1_element->y_offset = 0xFFF8;
	g1_element->flags = 0;

	gfx_draw_sprite(dpi, 0, 0, 0, 0);

	*g1_element = pushed_g1_element;

	if (w->selected_tab == PAGE_PEEPS)
		window_map_paint_peep_overlay(dpi);
	else
		window_map_paint_train_overlay(dpi);
	
	window_map_paint_hud_rectangle(dpi);
}

/**
 *
 *  rct2: 0x0068CA6C
 */
static void window_map_init_map() 
{
	memset(RCT2_GLOBAL(RCT2_ADDRESS_MAP_IMAGE_DATA, void*), 0x0A0A0A0A, 256 * 256 * sizeof(uint32));
	RCT2_GLOBAL(0x00F1AD6C, uint32) = 0;
}

/**
 *
 *  rct2: 0x0068C990
 */
static void window_map_center_on_view_point()
{
	rct_window *w = window_get_main();
	rct_window *w_map;
	sint16 ax, bx, cx, dx;
	sint16 bp, di;

	if (w == NULL || w->viewport == NULL)
		return;

	w_map = window_find_by_class(WC_MAP);
	if (w_map == NULL)
		return;

	rct_xy16 offset = MiniMapOffsets[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) & 3];

	// calculate center view point of viewport and transform it to minimap coordinates

	cx = ((w->viewport->view_width >> 1) + w->viewport->view_x) >> 5;
	dx = ((w->viewport->view_height >> 1) + w->viewport->view_y) >> 4;
	cx += offset.x;
	dx += offset.y;
	
	// calculate width and height of minimap

	ax = w_map->widgets[WIDX_MAP].right - w_map->widgets[WIDX_MAP].left - 11;
	bx = w_map->widgets[WIDX_MAP].bottom - w_map->widgets[WIDX_MAP].top - 11;
	bp = ax;
	di = bx;

	ax >>= 1;
	bx >>= 1;
	cx = max(cx - ax, 0);
	dx = max(dx - bx, 0);

	bp = w_map->scrolls[0].h_right - bp;
	di = w_map->scrolls[0].v_bottom - di;

	if (bp < 0 && (bp - cx) < 0)
		cx = 0;

	if (di < 0 && (di - dx) < 0)
		dx = 0;

	w_map->scrolls[0].h_left = cx;
	w_map->scrolls[0].v_top = dx;
	widget_scroll_update_thumbs(w_map, WIDX_MAP);
}

/**
 *
 *  rct2: 0x0068CD35 (part of 0x0068CA8F)
 */
static void window_map_show_default_scenario_editor_buttons(rct_window *w) {
	w->widgets[WIDX_BUILD_PARK_ENTRANCE].type = WWT_FLATBTN;
	w->widgets[WIDX_PEOPLE_STARTING_POSITION].type = WWT_FLATBTN;
	w->widgets[WIDX_MAP_SIZE_SPINNER].type = WWT_SPINNER;
	w->widgets[WIDX_MAP_SIZE_SPINNER_UP].type = WWT_DROPDOWN_BUTTON;
	w->widgets[WIDX_MAP_SIZE_SPINNER_DOWN].type = WWT_DROPDOWN_BUTTON;
	RCT2_GLOBAL(0x013CE952 + 2, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16);
}

static void window_map_inputsize_land(rct_window *w)
{
	((uint16*)TextInputDescriptionArgs)[0] = 1;
	((uint16*)TextInputDescriptionArgs)[1] = 64;
	window_text_input_open(w, WIDX_LAND_TOOL, 5128, 5129, STR_NONE, STR_NONE, 3);
}

static void window_map_inputsize_map(rct_window *w)
{
	((uint16*)TextInputDescriptionArgs)[0] = 50;
	((uint16*)TextInputDescriptionArgs)[1] = 256;
	window_text_input_open(w, WIDX_MAP_SIZE_SPINNER, 5130, 5131, STR_NONE, STR_NONE, 4);
}

static void window_map_draw_tab_images(rct_window *w, rct_drawpixelinfo *dpi)
{
	uint32 image;

	// Guest tab image (animated)
	image = SPR_TAB_GUESTS_0;
	if (w->selected_tab == PAGE_PEEPS)
		image += w->list_information_type / 4;

	gfx_draw_sprite(dpi, image, w->x + w->widgets[WIDX_PEOPLE_TAB].left, w->y + w->widgets[WIDX_PEOPLE_TAB].top, 0);

	// Ride/stall tab image (animated)
	image = SPR_TAB_RIDE_0;
	if (w->selected_tab == PAGE_RIDES)
		image += w->list_information_type / 4;

	gfx_draw_sprite(dpi, image, w->x + w->widgets[WIDX_RIDES_TAB].left, w->y + w->widgets[WIDX_RIDES_TAB].top, 0);
}

/**
 *
 *  part of window_map_paint_peep_overlay and window_map_paint_train_overlay
 */
static void window_map_transform_to_map_coords(sint16 *left, sint16 *top)
{
	sint16 x = *left, y = *top;
	sint16 temp;

	switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)) {
	case 3:
		temp = x;
		x = y;
		y = temp;
		x = 0x1FFF - x;
		break;
	case 2:
		x = 0x1FFF - x;
		y = 0x1FFF - y;
		break;
	case 1:
		temp = x;
		x = y;
		y = temp;
		y = 0x1FFF - y;
		break;
	case 0:
		break;
	}
	x >>= 5;
	y >>= 5;

	*left = -x + y + 0xF8;
	*top = x + y - 8;
}

/**
 *
 *  rct2: 0x0068DADA
 */
static void window_map_paint_peep_overlay(rct_drawpixelinfo *dpi)
{
	rct_peep *peep;
	uint16 spriteIndex;

	sint16 left, right, bottom, top;
	sint16 color;

	FOR_ALL_PEEPS(spriteIndex, peep) {
		left = peep->x;
		top = peep->y;

		if (left == SPRITE_LOCATION_NULL)
			continue;

		window_map_transform_to_map_coords(&left, &top);

		right = left;
		bottom = top;

		color = 0x14;

		if ((peep->var_0C & 0x200) != 0) {
			if (peep->type == PEEP_TYPE_STAFF) {
				if ((RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_MAP_FLASHING_FLAGS, uint16) & (1 << 3)) != 0) {
					color = 0x8A;
					left--;
					if ((RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_MAP_FLASHING_FLAGS, uint16) & (1 << 15)) == 0)
						color = 0xA;
				}
			} else {
				if ((RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_MAP_FLASHING_FLAGS, uint16) & (1 << 1)) != 0) {
					color = 0xAC;
					left--;
					if ((RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_MAP_FLASHING_FLAGS, uint16) & (1 << 15)) == 0)
						color = 0x15;
				}
			}
		}
		gfx_fill_rect(dpi, left, top, right, bottom, color);
	}
}

/**
 *
 *  rct2: 0x0068DBC1
 */
static void window_map_paint_train_overlay(rct_drawpixelinfo *dpi)
{
	rct_vehicle *train, *vehicle;
	uint16 train_index, vehicle_index;

	sint16 left, top, right, bottom;

	for (train_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_VEHICLE, uint16); train_index != SPRITE_INDEX_NULL; train_index = train->next) {
		train = GET_VEHICLE(train_index);
		for (vehicle_index = train_index; vehicle_index != SPRITE_INDEX_NULL; vehicle_index = vehicle->next_vehicle_on_train) {
			vehicle = GET_VEHICLE(vehicle_index);

			left = vehicle->x;
			top = vehicle->y;

			if (left == SPRITE_LOCATION_NULL)
				continue;

			window_map_transform_to_map_coords(&left, &top);

			right = left;
			bottom = top;

			gfx_fill_rect(dpi, left, top, right, bottom, 0xAB);
		}
	}
}

/**
 *  The call to gfx_fill_rect was originally wrapped in sub_68DABD which made sure that arguments were ordered correctly,
 *  but it doesn't look like it's ever necessary here so the call was removed.
 * 
 *  rct2: 0x0068D8CE
 */
static void window_map_paint_hud_rectangle(rct_drawpixelinfo *dpi)
{
	rct_window *main_window = window_get_main();
	if (main_window == NULL)
		return;

	rct_viewport *viewport = main_window->viewport;
	if (viewport == NULL)
		return;

	rct_xy16 offset = MiniMapOffsets[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32) & 3];
	sint16 left = (viewport->view_x >> 5) + offset.x;
	sint16 right = ((viewport->view_x + viewport->view_width) >> 5) + offset.x;
	sint16 top = (viewport->view_y >> 4) + offset.y;
	sint16 bottom = ((viewport->view_y + viewport->view_height) >> 4) + offset.y;

	// top horizontal lines
	gfx_fill_rect(dpi, left, top, left + 3, top, 0x38);
	gfx_fill_rect(dpi, right - 3, top, right, top, 0x38);

	// left vertical lines
	gfx_fill_rect(dpi, left, top, left, top + 3, 0x38);
	gfx_fill_rect(dpi, left, bottom - 3, left, bottom, 0x38);

	// bottom horizontal lines
	gfx_fill_rect(dpi, left, bottom, left + 3, bottom, 0x38);
	gfx_fill_rect(dpi, right - 3, bottom, right, bottom, 0x38);

	// right vertical lines
	gfx_fill_rect(dpi, right, top, right, top + 3, 0x38);
	gfx_fill_rect(dpi, right, bottom - 3, right, bottom, 0x38);
}

/**
 * 
 *  rct2: 0x0068D24E
 */
static void window_map_set_land_rights_tool_update(int x, int y)
{
	sint16 mapX, mapY;
	rct_viewport *viewport;

	map_invalidate_selection_rect();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);
	screen_get_map_xy(x, y, &mapX, &mapY, &viewport);
	if (mapX == (sint16)0x8000)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;

	int landToolSize = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, uint16);
	if (landToolSize == 0)
		landToolSize = 1;

	int size = (landToolSize * 32) - 32;
	int radius = (landToolSize * 16) - 16;
	mapX = (mapX - radius) & 0xFFE0;
	mapY = (mapY - radius) & 0xFFE0;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16) = mapX;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16) = mapY;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16) = mapX + size;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16) = mapY + size;
	map_invalidate_selection_rect();
}

/**
 * 
 *  rct2: 0x00666EEF
 */
void sub_666EEF(int x, int y, sint16 *mapX, sint16 *mapY, sint16 *mapZ, int *direction)
{
	rct_map_element *mapElement;

	sub_68A15E(x, y, mapX, mapY, direction, &mapElement);
	if (*mapX == (sint16)0x8000)
		return;

	mapElement = map_get_surface_element_at(*mapX >> 5, *mapY >> 5);
	*mapZ = mapElement->properties.surface.slope & 0x1F;
	if (*mapZ == 0) {
		*mapZ = mapElement->base_height / 2;
		if ((mapElement->properties.surface.slope & 0x0F) != 0) {
			(*mapZ)++;
			if (mapElement->properties.surface.slope & 0x10) {
				(*mapZ)++;
			}
		}
	}
	*direction = (window_scenery_rotation - RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)) & 3;
}

/**
 * 
 *  rct2: 0x00666FD0
 */
static void window_map_place_park_entrance_tool_update(int x, int y)
{
	sint16 mapX, mapY, mapZ;
	int direction, sideDirection;

	map_invalidate_selection_rect();
	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 1);
	sub_666EEF(x, y, &mapX, &mapY, &mapZ, &direction);
	if (mapX == (sint16)-1) {
		park_remove_ghost_entrance();
		return;
	}

	sideDirection = (direction + 1) & 3;
	gMapSelectionTiles[0].x = mapX;
	gMapSelectionTiles[0].y = mapY;
	gMapSelectionTiles[1].x = mapX + TileDirectionDelta[sideDirection].x;
	gMapSelectionTiles[1].y = mapY + TileDirectionDelta[sideDirection].y;
	gMapSelectionTiles[2].x = mapX - TileDirectionDelta[sideDirection].x;
	gMapSelectionTiles[2].y = mapY - TileDirectionDelta[sideDirection].y;
	gMapSelectionTiles[3].x = -1;
	gMapSelectionTiles[3].y = -1;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 1);
	map_invalidate_map_selection_tiles();
	if (
		(RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_GHOST_EXISTS, uint8) & (1 << 0)) &&
		mapX == RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_GHOST_X, uint16) &&
		mapY == RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_GHOST_Y, uint16) &&
		direction == RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_GHOST_DIRECTION, uint8)
	) {
		return;
	}

	park_remove_ghost_entrance();
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_GHOST_PRICE, uint32) = park_place_ghost_entrance(mapX, mapY, mapZ, direction);
}

/**
 * 
 *  rct2: 0x0068D4E9
 */
static void window_map_set_peep_spawn_tool_update(int x, int y)
{
	int mapX, mapY, mapZ, direction;
	rct_map_element *mapElement;

	map_invalidate_selection_rect();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 2);
	footpath_bridge_get_info_from_pos(x, y, &mapX, &mapY, &direction, &mapElement);
	if ((mapX & 0xFFFF) == 0x8000)
		return;

	mapZ = mapElement->base_height * 8;
	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SURFACE) {
		if ((mapElement->properties.surface.slope & 0x0F) != 0)
			mapZ += 16;
		if (mapElement->properties.surface.slope & 0x10)
			mapZ += 16;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 2);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16) = mapX;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16) = mapY;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16) = mapX;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16) = mapY;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = direction ^ 2;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_X, uint16) = mapX;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Y, uint16) = mapY;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, uint16) = mapZ;
	map_invalidate_selection_rect();
}

/**
 * 
 *  rct2: 0x006670A4
 */
static void window_map_place_park_entrance_tool_down(int x, int y)
{
	sint16 mapX, mapY, mapZ;
	int direction;
	money32 price;

	park_remove_ghost_entrance();
	sub_666EEF(x, y, &mapX, &mapY, &mapZ, &direction);
	if (mapX == (sint16)0x8000)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = STR_CANT_BUILD_PARK_ENTRANCE_HERE;
	price = game_do_command(
		mapX,
		GAME_COMMAND_FLAG_APPLY | (direction << 8),
		mapY,
		mapZ,
		GAME_COMMAND_PLACE_PARK_ENTRANCE,
		0,
		0
	);
	if (price == MONEY32_UNDEFINED)
		return;

	sound_play_panned(
		SOUND_PLACE_ITEM,
		0x8001,
		RCT2_GLOBAL(0x009DEA62, uint16),
		RCT2_GLOBAL(0x009DEA64, uint16),
		RCT2_GLOBAL(0x009DEA66, uint16)
	);
}

/**
 * 
 *  rct2: 0x0068D573
 */
static void window_map_set_peep_spawn_tool_down(int x, int y)
{
	rct_map_element *mapElement, *surfaceMapElement;
	int mapX, mapY, mapZ, direction;

	footpath_get_coordinates_from_pos(x, y, &mapX, &mapY, &direction, &mapElement);
	if (mapX == 0x8000)
		return;

	surfaceMapElement = map_get_surface_element_at(mapX >> 5, mapY >> 5);
	if (surfaceMapElement->properties.surface.ownership & 0xF0) {
		return;
	}

	mapX = mapX + 16 + (word_981D6C[direction].x * 15);
	mapY = mapY + 16 + (word_981D6C[direction].y * 15);
	mapZ = mapElement->base_height / 2;

	int peepSpawnIndex = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, uint16) != 1 && gPeepSpawns[0].x != 0xFFFF)
		peepSpawnIndex = 1;

	gPeepSpawns[peepSpawnIndex].x = mapX;
	gPeepSpawns[peepSpawnIndex].y = mapY;
	gPeepSpawns[peepSpawnIndex].z = mapZ;
	gPeepSpawns[peepSpawnIndex].direction = direction;
	gfx_invalidate_screen();
	RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, uint16) = peepSpawnIndex;
}

/**
 * 
 *  rct2: 0x0068D641
 */
static void map_window_increase_map_size()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) >= 256) {
		window_error_open(STR_CANT_INCREASE_MAP_SIZE_ANY_FURTHER, STR_NONE);
		return;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16)++;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) - 1) * 32;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_MINUS_2, uint16) = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) * 32) + 254;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_MAX_XY, uint16) = ((RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) - 1) * 32) - 1;
	map_extend_boundary_surface();
	window_map_init_map();
	window_map_center_on_view_point();
	gfx_invalidate_screen();
}

/**
 * 
 *  rct2: 0x0068D6B4
 */
static void map_window_decrease_map_size()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) < 16) {
		window_error_open(STR_CANT_DECREASE_MAP_SIZE_ANY_FURTHER, STR_NONE);
		return;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16)--;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) - 1) * 32;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_MINUS_2, uint16) = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) * 32) + 254;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_MAX_XY, uint16) = ((RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) - 1) * 32) - 1;
	map_remove_out_of_range_elements();
	window_map_init_map();
	window_map_center_on_view_point();
	gfx_invalidate_screen();
}

static const uint16 WaterColour = 0xC3C3;
static const uint16 TerrainColour[] = {
	0x4949,			// TERRAIN_GRASS
	0x2828,			// TERRAIN_SAND
	0x6C6C,			// TERRAIN_DIRT
	0x0C0C,			// TERRAIN_ROCK
	0x3E3E,			// TERRAIN_MARTIAN
	0x0A10,			// TERRAIN_CHECKERBOARD
	0x496C,			// TERRAIN_GRASS_CLUMPS
	0x8D8D,			// TERRAIN_ICE
	0xAC0A,			// TERRAIN_GRID_RED
	0x360A,			// TERRAIN_GRID_YELLOW
	0xA20A,			// TERRAIN_GRID_BLUE
	0x660A,			// TERRAIN_GRID_GREEN
	0x6F6F,			// TERRAIN_SAND_DARK
	0xDEDE,			// TERRAIN_SAND_LIGHT
};

static const uint16 ElementTypeMaskColour[] = {
	0xFFFF,			// MAP_ELEMENT_TYPE_SURFACE
	0x0000,			// MAP_ELEMENT_TYPE_PATH
	0x00FF,			// MAP_ELEMENT_TYPE_TRACK
	0xFF00,			// MAP_ELEMENT_TYPE_SCENERY
	0x0000,			// MAP_ELEMENT_TYPE_ENTRANCE
	0xFFFF,			// MAP_ELEMENT_TYPE_FENCE
	0x0000,			// MAP_ELEMENT_TYPE_SCENERY_MULTIPLE
	0xFFFF			// MAP_ELEMENT_TYPE_BANNER
};

static const uint16 ElementTypeAddColour[] = {
	0x0000,			// MAP_ELEMENT_TYPE_SURFACE
	0x1111,			// MAP_ELEMENT_TYPE_PATH
	0xB700,			// MAP_ELEMENT_TYPE_TRACK
	0x0063,			// MAP_ELEMENT_TYPE_SCENERY
	0xBABA,			// MAP_ELEMENT_TYPE_ENTRANCE
	0x0000,			// MAP_ELEMENT_TYPE_FENCE
	0x6363,			// MAP_ELEMENT_TYPE_SCENERY_MULTIPLE
	0x0000			// MAP_ELEMENT_TYPE_BANNER
};

enum {
	COLOUR_KEY_RIDE,
	COLOUR_KEY_FOOD,
	COLOUR_KEY_DRINK,
	COLOUR_KEY_SOUVENIR,
	COLOUR_KEY_KIOSK,
	COLOUR_KEY_FIRST_AID,
	COLOUR_KEY_CASH_MACHINE,
	COLOUR_KEY_TOILETS
};

static const uint8 RideColourKey[] = {
	COLOUR_KEY_RIDE,				// RIDE_TYPE_SPIRAL_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_STAND_UP_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_SUSPENDED_SWINGING_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_INVERTED_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_JUNIOR_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MINIATURE_RAILWAY
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MONORAIL
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MINI_SUSPENDED_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_BOAT_RIDE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_WOODEN_WILD_MOUSE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_STEEPLECHASE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_CAR_RIDE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_LAUNCHED_FREEFALL
	COLOUR_KEY_RIDE,				// RIDE_TYPE_BOBSLEIGH_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_OBSERVATION_TOWER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_LOOPING_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_DINGHY_SLIDE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MINE_TRAIN_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_CHAIRLIFT
	COLOUR_KEY_RIDE,				// RIDE_TYPE_CORKSCREW_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MAZE = 20
	COLOUR_KEY_RIDE,				// RIDE_TYPE_SPIRAL_SLIDE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_GO_KARTS
	COLOUR_KEY_RIDE,				// RIDE_TYPE_LOG_FLUME
	COLOUR_KEY_RIDE,				// RIDE_TYPE_RIVER_RAPIDS
	COLOUR_KEY_RIDE,				// RIDE_TYPE_DODGEMS
	COLOUR_KEY_RIDE,				// RIDE_TYPE_PIRATE_SHIP
	COLOUR_KEY_RIDE,				// RIDE_TYPE_SWINGING_INVERTER_SHIP
	COLOUR_KEY_FOOD,				// RIDE_TYPE_FOOD_STALL
	COLOUR_KEY_FOOD,				// RIDE_TYPE_1D
	COLOUR_KEY_DRINK,				// RIDE_TYPE_DRINK_STALL
	COLOUR_KEY_DRINK,				// RIDE_TYPE_1F
	COLOUR_KEY_SOUVENIR,			// RIDE_TYPE_SHOP
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MERRY_GO_ROUND
	COLOUR_KEY_SOUVENIR,			// RIDE_TYPE_22
	COLOUR_KEY_KIOSK,				// RIDE_TYPE_INFORMATION_KIOSK
	COLOUR_KEY_TOILETS,				// RIDE_TYPE_TOILETS
	COLOUR_KEY_RIDE,				// RIDE_TYPE_FERRIS_WHEEL
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MOTION_SIMULATOR
	COLOUR_KEY_RIDE,				// RIDE_TYPE_3D_CINEMA
	COLOUR_KEY_RIDE,				// RIDE_TYPE_TOP_SPIN
	COLOUR_KEY_RIDE,				// RIDE_TYPE_SPACE_RINGS
	COLOUR_KEY_RIDE,				// RIDE_TYPE_REVERSE_FREEFALL_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_LIFT
	COLOUR_KEY_RIDE,				// RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
	COLOUR_KEY_CASH_MACHINE,		// RIDE_TYPE_CASH_MACHINE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_TWIST
	COLOUR_KEY_RIDE,				// RIDE_TYPE_HAUNTED_HOUSE
	COLOUR_KEY_FIRST_AID,			// RIDE_TYPE_FIRST_AID
	COLOUR_KEY_RIDE,				// RIDE_TYPE_CIRCUS_SHOW
	COLOUR_KEY_RIDE,				// RIDE_TYPE_GHOST_TRAIN
	COLOUR_KEY_RIDE,				// RIDE_TYPE_TWISTER_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_WOODEN_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_WILD_MOUSE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_38
	COLOUR_KEY_RIDE,				// RIDE_TYPE_FLYING_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_3A
	COLOUR_KEY_RIDE,				// RIDE_TYPE_VIRGINIA_REEL
	COLOUR_KEY_RIDE,				// RIDE_TYPE_SPLASH_BOATS
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MINI_HELICOPTERS
	COLOUR_KEY_RIDE,				// RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_SUSPENDED_MONORAIL
	COLOUR_KEY_RIDE,				// RIDE_TYPE_40
	COLOUR_KEY_RIDE,				// RIDE_TYPE_REVERSER_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_HEARTLINE_TWISTER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MINI_GOLF
	COLOUR_KEY_RIDE,				// RIDE_TYPE_GIGA_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_ROTO_DROP
	COLOUR_KEY_RIDE,				// RIDE_TYPE_FLYING_SAUCERS
	COLOUR_KEY_RIDE,				// RIDE_TYPE_CROOKED_HOUSE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MONORAIL_CYCLES
	COLOUR_KEY_RIDE,				// RIDE_TYPE_COMPACT_INVERTED_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_WATER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_INVERTED_HAIRPIN_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MAGIC_CARPET
	COLOUR_KEY_RIDE,				// RIDE_TYPE_SUBMARINE_RIDE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_RIVER_RAFTS
	COLOUR_KEY_RIDE,				// RIDE_TYPE_50
	COLOUR_KEY_RIDE,				// RIDE_TYPE_ENTERPRISE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_52
	COLOUR_KEY_RIDE,				// RIDE_TYPE_53
	COLOUR_KEY_RIDE,				// RIDE_TYPE_54
	COLOUR_KEY_RIDE,				// RIDE_TYPE_55
	COLOUR_KEY_RIDE,				// RIDE_TYPE_INVERTED_IMPULSE_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MINI_ROLLER_COASTER
	COLOUR_KEY_RIDE,				// RIDE_TYPE_MINE_RIDE
	COLOUR_KEY_RIDE,				// RIDE_TYPE_59
	COLOUR_KEY_RIDE,				// RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
	COLOUR_KEY_RIDE,				//
	COLOUR_KEY_RIDE,				//
	COLOUR_KEY_RIDE,				//
};

static const uint16 RideKeyColours[] = {
	0x3D3D,			// COLOUR_KEY_RIDE
	0x2A2A,			// COLOUR_KEY_FOOD
	0x1414,			// COLOUR_KEY_DRINK
	0x0D1D,			// COLOUR_KEY_SOUVENIR
	0x8888,			// COLOUR_KEY_KIOSK
	0x6666,			// COLOUR_KEY_FIRST_AID
	0x3737,			// COLOUR_KEY_CASH_MACHINE
	0xA1A1,			// COLOUR_KEY_TOILETS
};

static uint16 map_window_get_pixel_colour_peep(int x, int y)
{
	rct_map_element *mapElement;
	uint16 colour;

	mapElement = map_get_surface_element_at(x >> 5, y >> 5);
	colour = TerrainColour[map_element_get_terrain(mapElement)];
	if (mapElement->properties.surface.terrain & 0x1F)
		colour = WaterColour;

	if (!(mapElement->properties.surface.ownership & OWNERSHIP_OWNED))
		colour = 10 | (colour & 0xFF00);

	if (!(mapElement->flags & (1 << 5)))
		colour = 10 | (colour & 0xFF00);

	while (!map_element_is_last_for_tile(mapElement++)) {
		int mapElementType = map_element_get_type(mapElement);
		colour &= ElementTypeMaskColour[mapElementType >> 2];
		colour |= ElementTypeAddColour[mapElementType >> 2];
	}

	return colour;
}

static uint16 map_window_get_pixel_colour_ride(int x, int y)
{
	rct_map_element *mapElement;
	rct_ride *ride;
	uint32 colour;

	colour = 0x0D0D0000;
	mapElement = map_get_surface_element_at(x >> 5, y >> 5);
	do {
		switch (map_element_get_type(mapElement)) {
		case MAP_ELEMENT_TYPE_SURFACE:
			if (mapElement->properties.surface.terrain & 0x1F) {
				colour &= 0xFFFF;
				colour |= 0xC2C20000;
			}
			if (!(mapElement->properties.surface.ownership & OWNERSHIP_OWNED)) {
				colour &= 0xFF00FFFF;
				colour |= 0x000A0000;
			}
			break;
		case MAP_ELEMENT_TYPE_PATH:
			colour = 0x0E0E;
			break;
		case MAP_ELEMENT_TYPE_ENTRANCE:
			if (mapElement->properties.entrance.type == ENTRANCE_TYPE_PARK_ENTRANCE)
				break;
			// fall-through
		case MAP_ELEMENT_TYPE_TRACK:
			ride = GET_RIDE(mapElement->properties.track.ride_index);
			colour = RideKeyColours[RideColourKey[ride->type]];
			break;
		}
	} while (!map_element_is_last_for_tile(mapElement++));

	if ((colour & 0xFFFF) == 0)
		colour >>= 16;

	return colour & 0xFFFF;
}

static void map_window_set_pixels(rct_window *w)
{
	uint16 colour, *destination;
	int x, y, dx, dy;
	
	destination = (uint16*)((RCT2_GLOBAL(0x00F1AD6C, uint32) * 511) + RCT2_GLOBAL(0x00F1AD68, uint32) + 255);
	switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)) {
	case 0:
		x = RCT2_GLOBAL(0x00F1AD6C, uint32) * 32;
		y = 0;
		dx = 0;
		dy = 32;
		break;
	case 1:
		x = 8192 - 32;
		y = RCT2_GLOBAL(0x00F1AD6C, uint32) * 32;
		dx = -32;
		dy = 0;
		break;
	case 2:
		x = (255 - RCT2_GLOBAL(0x00F1AD6C, uint32)) * 32;
		y = 8192 - 32;
		dx = 0;
		dy = -32;
		break;
	case 3:
		x = 0;
		y = (255 - RCT2_GLOBAL(0x00F1AD6C, uint32)) * 32;
		dx = 32;
		dy = 0;
		break;
	}

	for (int i = 0; i < 256; i++) {
		if (
			x > 0 &&
			y > 0 &&
			x < RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) &&
			y < RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16)
		) {
			switch (w->selected_tab) {
			case PAGE_PEEPS:
				colour = map_window_get_pixel_colour_peep(x, y);
				break;
			case PAGE_RIDES:
				colour = map_window_get_pixel_colour_ride(x, y);
				break;
			}
			*destination = colour;
		}
		x += dx;
		y += dy;
		destination = (uint16*)((int)destination + 513);
	}
	RCT2_GLOBAL(0x00F1AD6C, uint32)++;
	if (RCT2_GLOBAL(0x00F1AD6C, uint32) >= 256)
		RCT2_GLOBAL(0x00F1AD6C, uint32) = 0;
}

static void map_window_screen_to_map(int screenX, int screenY, int *mapX, int *mapY)
{
	int x, y;

	screenX = ((screenX + 8) - 256) / 2;
	screenY = ((screenY + 8)      ) / 2;
	x = (screenY - screenX) * 32;
	y = (screenX + screenY) * 32;
	switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) & 3) {
	case 0:
		*mapX = x;
		*mapY = y;
		break;
	case 1:
		*mapX = 8191 - y;
		*mapY = x;
		break;
	case 2:
		*mapX = 8191 - x;
		*mapY = 8191 - y;
		break;
	case 3:
		*mapX = y;
		*mapY = 8191 - x;
		break;
	}
}
