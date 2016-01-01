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

#include "../addresses.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/viewport.h"
#include "../world/scenery.h"
#include "../world/map.h"
#include "../world/footpath.h"
#include "../sprites.h"

enum WINDOW_TILE_INSPECTOR_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_LIST,
	WIDX_CORRUPT,
	WIDX_REMOVE,
	WIDX_MOVE_DOWN,
	WIDX_MOVE_UP,
	WIDX_COLUMN_TYPE,
	WIDX_COLUMN_BASEHEIGHT,
	WIDX_COLUMN_CLEARANCEHEIGHT,
	WIDX_COLUMN_GHOSTFLAG,
	WIDX_COLUMN_BROKENFLAG,
	WIDX_COLUMN_LASTFLAG,
};

#define WW 400
#define WH 200
#define MIN_WW WW
#define MAX_WW WW
#define MIN_WH 150
#define MAX_WH 800

#define BW (WW - 5) // Button's right side
#define BX (BW - 23) // Button's left side
#define BY 17 // Button's Top
#define BH (BY + 23) // Button's Bottom
#define BS 24

#define SCROLL_BOTTOM_OFFSET 15
#define LIST_ITEM_HEIGHT 11

// Column offsets
#define COL_X_TYPE 3 // Type
#define COL_X_BH   (COL_X_TYPE + 300) // Base height
#define COL_X_CH   (COL_X_BH + 20) // Clearance height
#define COL_X_GF   (COL_X_CH + 20) // Ghost flag
#define COL_X_BF   (COL_X_GF + 12) // Broken flag
#define COL_X_LF   (COL_X_BF + 12) // Last for tile flag

rct_widget window_tile_inspector_widgets[] = {
	{ WWT_FRAME,		0,	0,				WW - 1,				0,				WH - 1,		0x0FFFFFFFF,				STR_NONE },				// panel / background
	{ WWT_CAPTION,		0,	1,				WW - 2,				1,				14,			STR_TILE_INSPECTOR_TITLE,	STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,		0,	WW - 13,		WW - 3,				2,				13,			STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },	// close x button

	// Map element list
	{ WWT_SCROLL,		1,	3,				WW - 4,				57,				WH - SCROLL_BOTTOM_OFFSET,	2,			STR_NONE },				// scroll area

	// Buttons
	{ WWT_FLATBTN,		1,	BX,				BW,					BY,				BH,			SPR_MAP,		STR_INSERT_CORRUPT_TIP }, // Insert corrupt button
	{ WWT_FLATBTN,		1,  BX - BS * 1,	BW - BS * 1,		BY,				BH,			SPR_DEMOLISH,	5607 }, // Remove button
	{ WWT_CLOSEBOX, 	1,	BX - BS * 2,	BW - BS * 2,		BY,				BY + 11, 	5375,			5617 }, // Move down
	{ WWT_CLOSEBOX, 	1,	BX - BS * 2, 	BW - BS * 2,		BH - 11,		BH, 		5376,			5618 }, // Move up

	// Column headers
	{ WWT_13,			1, COL_X_TYPE,	COL_X_BH - 1, 	42,		42 + 13,	STR_NONE,	STR_NONE }, // Type
	{ WWT_13,			1, COL_X_BH,	COL_X_CH - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_BASE_HEIGHT }, // Base height
	{ WWT_13,			1, COL_X_CH,	COL_X_GF - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_CLEARANCE_HEIGHT }, // Clearance height
	{ WWT_13,			1, COL_X_GF,	COL_X_BF - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_FLAG_GHOST }, // Ghost flag
	{ WWT_13,			1, COL_X_BF,	COL_X_LF - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_FLAG_BROKEN }, // Broken flag
	{ WWT_13,			1, COL_X_LF,	WW - 3,			42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_FLAG_LAST }, // Last of tile flag

	{ WIDGETS_END },
};

static sint16 window_tile_inspector_highlighted_index = -1;

static int window_tile_inspector_tile_x;
static int window_tile_inspector_tile_y;
static int window_tile_inspector_item_count;

static void window_tile_inspector_close(rct_window *w);
static void window_tile_inspector_mouseup(rct_window *w, int widgetIndex);
static void window_tile_inspector_resize(rct_window *w);
static void window_title_editor_update(rct_window *w);
static void window_tile_inspector_tool_update(rct_window* w, int widgetIndex, int x, int y);
static void window_tile_inspector_tool_down(rct_window* w, int widgetIndex, int x, int y);
static void window_tile_inspector_tool_abort(rct_window *w, int widgetIndex);
static void window_tile_inspector_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_tile_inspector_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_tile_inspector_scrollmouseover(rct_window *w, int scrollIndex, int x, int y);
static void window_tile_inspector_invalidate(rct_window *w);
static void window_tile_inspector_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_tile_inspector_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static void window_tile_inspector_auto_set_buttons(rct_window *w);

static rct_window_event_list window_tile_inspector_events = {
	window_tile_inspector_close,
	window_tile_inspector_mouseup,
	window_tile_inspector_resize,
	NULL,
	NULL,
	NULL,
	window_title_editor_update,
	NULL,
	NULL,
	window_tile_inspector_tool_update,
	window_tile_inspector_tool_down,
	NULL,
	NULL,
	window_tile_inspector_tool_abort,
	NULL,
	window_tile_inspector_scrollgetsize,
	window_tile_inspector_scrollmousedown,
	NULL,
	window_tile_inspector_scrollmouseover,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_tile_inspector_invalidate,
	window_tile_inspector_paint,
	window_tile_inspector_scrollpaint
};

void window_tile_inspector_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_TILE_INSPECTOR);
	if (window != NULL)
		return;

	window = window_create(
		0,
		29,
		WW,
		WH,
		&window_tile_inspector_events,
		WC_TILE_INSPECTOR,
		WF_RESIZABLE
	);
	window->widgets = window_tile_inspector_widgets;
	window->enabled_widgets = (1 << WIDX_CLOSE);
	window->disabled_widgets = (1 << WIDX_CORRUPT) | (1 << WIDX_MOVE_UP) | (1 << WIDX_MOVE_DOWN) | (1 << WIDX_REMOVE);

	window_init_scroll_widgets(window);
	window->colours[0] = 7;
	window->colours[1] = 7;
	window->colours[2] = 7;
	window->min_width = MIN_WW;
	window->min_height = MIN_WH;
	window->max_width = MAX_WW;
	window->max_height = MAX_WH;
	window->selected_list_item = -1;

	window_tile_inspector_tile_x = -1;
	window_tile_inspector_tile_y = -1;

	tool_set(window, WIDX_BACKGROUND, 12);
}

static void window_tile_inspector_close(rct_window *w)
{
	tool_cancel();
}

void corrupt_element(int x, int y) {
	rct_map_element* mapElement;
	mapElement = map_get_first_element_at(x, y);

	while (!map_element_is_last_for_tile(mapElement++));
	mapElement--;

	mapElement = map_element_insert(x, y, mapElement->base_height, 0);
	mapElement->type = (8 << 2);
}

void remove_element(int index)
{
	assert(index < window_tile_inspector_item_count);
	rct_map_element *mapElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);
	mapElement += index;
	map_element_remove(mapElement);
	window_tile_inspector_item_count--;
}

// Swap element with its parent
void swap_elements(sint16 first, sint16 second)
{
	rct_map_element *mapElement;
	rct_map_element *firstElement = NULL;
	rct_map_element *secondElement = NULL;
	mapElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);

	// swap_elements shouldn't be called when there is only one element on the tile
	assert(!map_element_is_last_for_tile(mapElement));

	// Search for the elements
	sint16 i = 0;
	do {
		if (i == first) firstElement = mapElement;
		if (i == second) secondElement = mapElement;
		i++;

		// Check if both elements have been found
		if (firstElement != NULL && secondElement != NULL)
			break;
	} while (!map_element_is_last_for_tile(mapElement++));

	// Swap their memory
	rct_map_element temp = *firstElement;
	*firstElement = *secondElement;
	*secondElement = temp;

	// Swap the 'last map element for tile' flag if either one of them was last
	if (map_element_is_last_for_tile(firstElement) || map_element_is_last_for_tile(secondElement)) {
		firstElement->flags ^= MAP_ELEMENT_FLAG_LAST_TILE;
		secondElement->flags ^= MAP_ELEMENT_FLAG_LAST_TILE;
	}
}

static void window_tile_inspector_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_CORRUPT:
		corrupt_element(window_tile_inspector_tile_x, window_tile_inspector_tile_y);
		w->scrolls[0].v_top = 0;
		w->selected_list_item = window_tile_inspector_item_count++;
		window_tile_inspector_auto_set_buttons(w);
		widget_invalidate(w, WIDX_LIST);
		break;
	case WIDX_REMOVE:
		remove_element(w->selected_list_item);
		w->selected_list_item = -1;
		window_tile_inspector_auto_set_buttons(w);
		widget_invalidate(w, WIDX_LIST);
		break;
	case WIDX_MOVE_DOWN:
		swap_elements(w->selected_list_item, w->selected_list_item + 1);
		w->selected_list_item++;
		window_tile_inspector_auto_set_buttons(w);
		widget_invalidate(w, WIDX_LIST);
		break;
	case WIDX_MOVE_UP:
		swap_elements(w->selected_list_item - 1, w->selected_list_item);
		w->selected_list_item--;
		window_tile_inspector_auto_set_buttons(w);
		widget_invalidate(w, WIDX_LIST);
		break;
	}
}

static void window_tile_inspector_resize(rct_window *w)
{
	w->min_width = WW;
	w->min_height = MIN_WH;
	if (w->width < w->min_width) {
		window_invalidate(w);
		w->width = w->min_width;
	}
	if (w->height < w->min_height) {
		window_invalidate(w);
		w->height = w->min_height;
	}
}

static void window_title_editor_update(rct_window *w)
{
	// Check if the mouse is hovering over the list
	if (!widget_is_highlighted(w, WIDX_LIST))
	{
		window_tile_inspector_highlighted_index = -1;
		widget_invalidate(w, WIDX_LIST);
	}
}

static void window_tile_inspector_tool_update(rct_window* w, int widgetIndex, int x, int y)
{
	int direction;
	short mapX, mapY;

	map_invalidate_selection_rect();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);

	mapX = x;
	mapY = y;
	screen_pos_to_map_pos(&mapX, &mapY, &direction);
	if (mapX == (short)0x8000) {
		return;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = mapX;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = mapY;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = mapX;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = mapY;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;

	map_invalidate_selection_rect();

}

static void window_tile_inspector_tool_down(rct_window* w, int widgetIndex, int x, int y)
{
	int direction;
	short mapX, mapY;

	mapX = x;
	mapY = y;
	screen_pos_to_map_pos(&mapX, &mapY, &direction);
	if (mapX == (short)0x8000) {
		return;
	}

	window_tile_inspector_tile_x = mapX >> 5;
	window_tile_inspector_tile_y = mapY >> 5;

	rct_map_element *element = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);
	int numItems = 0;
	do {
		numItems++;
	} while (!map_element_is_last_for_tile(element++));

	window_tile_inspector_item_count = numItems;

	// Enable 'insert corrupt element' button
	w->enabled_widgets |= (1 << WIDX_CORRUPT);
	w->disabled_widgets &= ~(1ULL << WIDX_CORRUPT);
	// undo selection and buttons affecting it
	w->selected_list_item = -1;
	w->disabled_widgets |= (1ULL << WIDX_MOVE_UP) | (1ULL << WIDX_MOVE_DOWN) | (1ULL << WIDX_REMOVE);
	w->enabled_widgets &= ~((1ULL << WIDX_MOVE_UP) | (1ULL << WIDX_MOVE_DOWN) | (1ULL << WIDX_REMOVE));

	w->scrolls[0].v_top = 0;
	window_invalidate(w);
}

static void window_tile_inspector_tool_abort(rct_window *w, int widgetIndex)
{
	window_close(w);
}

static void window_tile_inspector_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	*width = WW - 30;
	*height = window_tile_inspector_item_count * LIST_ITEM_HEIGHT;
}

static void window_tile_inspector_auto_set_buttons(rct_window *w)
{
	// Remove button
	if (w->selected_list_item == -1) { // Check if anything is selected
		w->disabled_widgets |= (1ULL << WIDX_REMOVE); 
		w->enabled_widgets &= ~(1ULL << WIDX_REMOVE);
	} else { // Nothing is selected
		w->disabled_widgets &= ~(1ULL << WIDX_REMOVE);
		w->enabled_widgets |= (1ULL << WIDX_REMOVE);
	}
	widget_invalidate(w, WIDX_REMOVE);

	// Move Up button
	if (w->selected_list_item <= 0) { // Top element in list, or -1
		w->disabled_widgets |= (1ULL << WIDX_MOVE_UP);
		w->enabled_widgets &= ~(1ULL << WIDX_MOVE_UP);
	} else { // Not the top element in the list
		w->enabled_widgets |= (1ULL << WIDX_MOVE_UP);
		w->disabled_widgets &= ~(1ULL << WIDX_MOVE_UP);
	}
	widget_invalidate(w, WIDX_MOVE_UP);

	// Move Down button
	if (w->selected_list_item == window_tile_inspector_item_count - 1 || w->selected_list_item == -1) { // Bottom element in list, or -1
		w->disabled_widgets |= (1ULL << WIDX_MOVE_DOWN);
		w->enabled_widgets &= ~(1ULL << WIDX_MOVE_DOWN);
	} else { // Not the bottom element in the list
		w->enabled_widgets |= (1ULL << WIDX_MOVE_DOWN);
		w->disabled_widgets &= ~(1ULL << WIDX_MOVE_DOWN);
	}
	widget_invalidate(w, WIDX_MOVE_DOWN);
}

static void window_tile_inspector_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	// Because the list items are displayed in reverse order, subtract the number from the amount of elements
	sint16 index = window_tile_inspector_item_count - (y - 1) / LIST_ITEM_HEIGHT - 1;
	if (index < 0 || index >= window_tile_inspector_item_count)
		return;
	w->selected_list_item = index;

	// Enable/disable buttons
	window_tile_inspector_auto_set_buttons(w);
}

static void window_tile_inspector_scrollmouseover(rct_window *w, int scrollIndex, int x, int y)
{
	sint16 index = window_tile_inspector_item_count - (y - 1) / LIST_ITEM_HEIGHT - 1;
	if (index < 0 || index >= window_tile_inspector_item_count)
		window_tile_inspector_highlighted_index = -1;
	else
		window_tile_inspector_highlighted_index = index;

	widget_invalidate(w, WIDX_LIST);
}

static void window_tile_inspector_invalidate(rct_window *w)
{
	window_tile_inspector_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_tile_inspector_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	window_tile_inspector_widgets[WIDX_CLOSE].left = w->width - 13;
	window_tile_inspector_widgets[WIDX_CLOSE].right = w->width - 3;
	window_tile_inspector_widgets[WIDX_TITLE].right = w->width - 2;
	window_tile_inspector_widgets[WIDX_LIST].right = w->width - 4;
	window_tile_inspector_widgets[WIDX_LIST].bottom = w->height - SCROLL_BOTTOM_OFFSET;
	window_tile_inspector_widgets[WIDX_COLUMN_LASTFLAG].right = w->width - 3;
}

static void window_tile_inspector_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	char buffer[256];
	int x = w->x /*+ w->widgets[WIDX_LIST].left*/ + 3;
	int y = w->y + w->height - 13;

	// Set medium font size
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = FONT_SPRITE_BASE_MEDIUM;

	// Draw column headers
	rct_widget *widget;
	if ((widget= &w->widgets[WIDX_COLUMN_TYPE])->type != WWT_EMPTY) {
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_ELEMENT_TYPE, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}
	if ((widget = &w->widgets[WIDX_COLUMN_BASEHEIGHT])->type != WWT_EMPTY)
	{
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_BASE_HEIGHT_SHORT, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}
	if ((widget = &w->widgets[WIDX_COLUMN_CLEARANCEHEIGHT])->type != WWT_EMPTY)
	{
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_CLEARANGE_HEIGHT_SHORT, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}
	if ((widget = &w->widgets[WIDX_COLUMN_GHOSTFLAG])->type != WWT_EMPTY)
	{
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_FLAG_GHOST_SHORT, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}
	if ((widget = &w->widgets[WIDX_COLUMN_BROKENFLAG])->type != WWT_EMPTY)
	{
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_FLAG_BROKEN_SHORT, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}
	if ((widget = &w->widgets[WIDX_COLUMN_LASTFLAG])->type != WWT_EMPTY)
	{
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_FLAG_LAST_SHORT, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}

	// Draw coordinates
	if (window_tile_inspector_tile_x == -1) { // No tile selected
		gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_CHOOSE_MSG, NULL, 12, x, y);
	} else {
		sprintf(
			buffer,
			"X: %d, Y: %d",
			window_tile_inspector_tile_x,
			window_tile_inspector_tile_y
		);

		gfx_draw_string(dpi, buffer, 12, x, y);
	}
}

static void window_tile_inspector_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	int x = 3;
	int y = LIST_ITEM_HEIGHT * (window_tile_inspector_item_count - 1);
	int i = 0;
	char buffer[256];

	if (window_tile_inspector_tile_x == -1)
		return;

	rct_map_element *element = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = FONT_SPRITE_BASE_MEDIUM;
	do {
		int type = map_element_get_type(element);
		char *type_name;
		int base_height = element->base_height;
		int clearance_height = element->clearance_height;

		// Fill colour for current list element
		const int list_width = w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left;
		if (i == w->selected_list_item) // Currently selected element
			gfx_fill_rect(dpi, 0, y, list_width, y + LIST_ITEM_HEIGHT - 1, ColourMapA[w->colours[1]].darker | 0x1000000);
		else if (i == window_tile_inspector_highlighted_index) // Hovering
			gfx_fill_rect(dpi, 0, y, list_width, y + LIST_ITEM_HEIGHT - 1, ColourMapA[w->colours[1]].mid_dark | 0x1000000);
		else if ((i & 1) != 0) // odd / even check
			gfx_fill_rect(dpi, 0, y, list_width, y + LIST_ITEM_HEIGHT - 1, ColourMapA[w->colours[1]].lighter | 0x1000000);

		switch (type) {
			case MAP_ELEMENT_TYPE_SURFACE:
				sprintf(
					buffer,
					"Surface (%s, %s)",
					language_get_string(STR_TILE_INSPECTOR_TERRAIN_START + map_element_get_terrain(element)),
					language_get_string(STR_TILE_INSPECTOR_TERRAIN_EDGE_START + map_element_get_terrain_edge(element))
				);
				type_name = buffer;
				break;
			case MAP_ELEMENT_TYPE_PATH:
			{
				const uint8 pathType = footpath_element_get_type(element);
				const uint8 pathHasScenery = footpath_element_has_path_scenery(element);
				const uint8 pathAdditionType = footpath_element_get_path_scenery_index(element);
				if (footpath_element_is_queue(element)) {
					sprintf(
						buffer, "Queue (%s)%s%s for (%d)",
						language_get_string(g_pathSceneryEntries[pathType]->name), // Path name
						pathHasScenery ? " with " : "", // Adds " with " when there is something on the path
						pathHasScenery ? language_get_string(g_pathBitSceneryEntries[pathAdditionType]->name) : "", // Path addition name
						element->properties.path.ride_index // Ride index for queue
					);
				} else {
					sprintf(
						buffer, "Path (%s)%s%s",
						language_get_string(g_pathSceneryEntries[pathType]->name), // Path name
						pathHasScenery ? " with " : "", // Adds " with " when there is something on the path
						pathHasScenery ? language_get_string(g_pathBitSceneryEntries[pathAdditionType]->name) : "" // Path addition name
					);
				}
			}
			type_name = buffer;
			break;
			case MAP_ELEMENT_TYPE_TRACK:
				sprintf(
					buffer,
					"Track (%s)",
					language_get_string(2 + GET_RIDE(element->properties.track.ride_index)->type)
				);
				type_name = buffer;
				break;
			case MAP_ELEMENT_TYPE_SCENERY:
				sprintf(
					buffer,
					"Scenery (%s)",
					language_get_string(g_smallSceneryEntries[element->properties.scenery.type]->name)
				);
				type_name = buffer;
				break;
			case MAP_ELEMENT_TYPE_ENTRANCE:
				sprintf(
					buffer,
					"Entrance (%s)",
					language_get_string(STR_TILE_INSPECTOR_ENTRANCE_START + element->properties.entrance.type)
					);
				type_name = buffer;
				break;
			case MAP_ELEMENT_TYPE_FENCE:
				sprintf(
					buffer,
					"Fence (%s)",
					language_get_string(g_wallSceneryEntries[element->properties.scenery.type]->name)
					);
				type_name = buffer;
				break;
			case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
				type_name = "Scenery multiple";
				break;
			case MAP_ELEMENT_TYPE_BANNER:
				sprintf(
					buffer,
					"Banner (%d)",
					element->properties.banner.index
				);
				type_name = buffer;
				break;
			case MAP_ELEMENT_TYPE_CORRUPT:
				// fall-through
			default:
				sprintf(buffer, "Unknown (type %d)", type);
				type_name = buffer;
		}

		// Undo relative scroll offset, but keep the 3 pixel padding
		x = -w->widgets[WIDX_LIST].left;
		const bool ghost = (element->flags & MAP_ELEMENT_FLAG_GHOST) != 0;
		const bool broken = (element->flags & MAP_ELEMENT_FLAG_BROKEN) != 0;
		const bool last = (element->flags & MAP_ELEMENT_FLAG_LAST_TILE) != 0;
		gfx_draw_string(dpi, type_name, 12, x + COL_X_TYPE + 3, y); // 3px padding
		gfx_draw_string_left(dpi, 5182, &base_height, 12, x + COL_X_BH, y);
		gfx_draw_string_left(dpi, 5182, &clearance_height, 12, x + COL_X_CH, y);
		if (ghost) gfx_draw_string(dpi, (char*)CheckBoxMarkString, w->colours[1], x + COL_X_GF, y);
		if (broken) gfx_draw_string(dpi, (char*)CheckBoxMarkString, w->colours[1], x + COL_X_BF, y);
		if (last) gfx_draw_string(dpi, (char*)CheckBoxMarkString, w->colours[1], x + COL_X_LF, y);

		y -= LIST_ITEM_HEIGHT;
		i++;
	} while (!map_element_is_last_for_tile(element++));
}
