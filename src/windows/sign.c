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
#include "../game.h"
#include "../config.h"
#include "../localisation/localisation.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../ride/ride.h"
#include "../world/map.h"
#include "../world/banner.h"
#include "../world/scenery.h"
#include "error.h"
#include "dropdown.h"
#include "../drawing/drawing.h"

#define WW 113
#define WH 96

enum WINDOW_SIGN_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_VIEWPORT,
	WIDX_SIGN_TEXT,
	WIDX_SIGN_DEMOLISH,
	WIDX_MAIN_COLOR,
	WIDX_TEXT_COLOR
};

// 0x9AEE00
rct_widget window_sign_widgets[] = {
		{ WWT_FRAME,	0, 0,		WW - 1,		0,			WH - 1,		0x0FFFFFFFF,	65535 },					// panel / background
		{ WWT_CAPTION,	0, 1,		WW - 2,		1,			14,			STR_SIGN,		STR_WINDOW_TITLE_TIP },		// title bar
		{ WWT_CLOSEBOX, 0, WW - 13, WW - 3,		2,			13,			STR_CLOSE_X,	STR_CLOSE_WINDOW_TIP },		// close x button
		{ WWT_VIEWPORT, 1, 3,		WW - 26,	17,			WH - 20,	0x0FFFFFFFE,	65535 },					// Viewport
		{ WWT_FLATBTN,	1, WW - 25, WW - 2,		19,			42,			5168,			STR_CHANGE_SIGN_TEXT_TIP },	// change sign button
		{ WWT_FLATBTN,	1, WW - 25, WW - 2,		67,			90,			5165,			STR_DEMOLISH_SIGN_TIP },	// demolish button
		{ WWT_COLORBTN, 1, 5,		16,			WH - 16,	WH - 5,		0x0FFFFFFFF,	STR_SELECT_MAIN_COLOR_TIP },// Main colour
		{ WWT_COLORBTN, 1, 17,		28,			WH - 16,	WH - 5,		0x0FFFFFFFF,	STR_SELECT_TEXT_COLOR_TIP },// Text colour
		{ WIDGETS_END },
};

static void window_sign_emptysub() { }
static void window_sign_mouseup();
static void window_sign_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_sign_dropdown();
static void window_sign_textinput();
static void window_sign_invalidate();
static void window_sign_paint();
static void window_sign_unknown_14();

// 0x98E44C
static void* window_sign_events[] = {
	window_sign_emptysub,
	window_sign_mouseup,
	window_sign_emptysub,
	window_sign_mousedown,
	window_sign_dropdown,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_textinput,
	window_sign_unknown_14,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_invalidate,
	window_sign_paint,
	window_sign_emptysub
};

static void window_sign_small_mouseup();
static void window_sign_small_dropdown();
static void window_sign_small_invalidate();

// 0x9A410C
static void* window_sign_small_events[] = {
	window_sign_emptysub,
	window_sign_small_mouseup,
	window_sign_emptysub,
	window_sign_mousedown,
	window_sign_small_dropdown,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_textinput,
	window_sign_unknown_14,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_emptysub,
	window_sign_small_invalidate,
	window_sign_paint,
	window_sign_emptysub
};

/**
*
*  rct2: 0x006BA305
*/
void window_sign_open(rct_windownumber number)
{
	rct_window* w;
	rct_widget *viewportWidget;


	// Check if window is already open
	w = window_bring_to_front_by_number(WC_BANNER, number);
	if (w != NULL)
		return;

	w = window_create_auto_pos(WW, WH, (uint32*)window_sign_events, WC_BANNER, 0);
	w->widgets = window_sign_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_SIGN_TEXT) |
		(1 << WIDX_SIGN_DEMOLISH) |
		(1 << WIDX_MAIN_COLOR) |
		(1 << WIDX_TEXT_COLOR);

	w->number = number;
	window_init_scroll_widgets(w);
	w->colours[0] = 24;
	w->colours[1] = 24;
	w->colours[2] = 24;

	int view_x = gBanners[w->number].x << 5;
	int view_y = gBanners[w->number].y << 5;

	rct_map_element* map_element = map_get_first_element_at(view_x / 32, view_y / 32);

	while (1){
		if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE) {
			int ebx = map_element->properties.scenerymultiple.type;
			ebx |= (map_element->properties.scenerymultiple.index & 0x3) << 8;
			rct_scenery_entry* scenery_entry = g_largeSceneryEntries[ebx];
			if (scenery_entry->large_scenery.var_11 != 0xFF){
				int id = (map_element->type & 0xC0) |
					((map_element->properties.scenerymultiple.colour[0] & 0xE0) >> 2) |
					((map_element->properties.scenerymultiple.colour[1] & 0xE0) >> 5);
				if (id == w->number)
					break;
			}
		}
		map_element++;
	}

	int view_z = map_element->base_height << 3;
	w->frame_no = view_z;

	w->list_information_type = map_element->properties.scenerymultiple.colour[0] & 0x1F;
	w->var_492 = map_element->properties.scenerymultiple.colour[1] & 0x1F;
	w->var_48C = map_element->properties.scenerymultiple.type;

	view_x += 16;
	view_y += 16;

	// Create viewport
	viewportWidget = &window_sign_widgets[WIDX_VIEWPORT];
	viewport_create(
		w,
		w->x + viewportWidget->left + 1,
		w->y + viewportWidget->top + 1,
		(viewportWidget->right - viewportWidget->left) - 1,
		(viewportWidget->bottom - viewportWidget->top) - 1,
		0,
		view_x,
		view_y,
		view_z,
		0,
		-1
	);

	w->viewport->flags = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES) ? VIEWPORT_FLAG_GRIDLINES : 0;
	w->flags |= WF_2;
	window_invalidate(w);
}

/* rct2: 0x6B9765*/
static void window_sign_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	rct_banner* banner = &gBanners[w->number];
	int x = banner->x << 5;
	int y = banner->y << 5;

	rct_string_id string_id;

	rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_SIGN_DEMOLISH:
		while (1){
			if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE) {
				int ebx = map_element->properties.scenerymultiple.type;
				ebx |= (map_element->properties.scenerymultiple.index & 0x3) << 8;
				rct_scenery_entry* scenery_entry = g_largeSceneryEntries[ebx];
				if (scenery_entry->large_scenery.var_11 != 0xFF){
					int id = (map_element->type & 0xC0) |
						((map_element->properties.scenerymultiple.colour[0] & 0xE0) >> 2) |
						((map_element->properties.scenerymultiple.colour[1] & 0xE0) >> 5);
					if (id == w->number)
						break;
				}
			}
			map_element++;
		}
		game_do_command(
			x,
			1 | ((map_element->type&0x3) << 8),
			y,
			map_element->base_height | ((map_element->properties.scenerymultiple.index >> 2) << 8),
			GAME_COMMAND_44, 
			0, 
			0);
		break;
	case WIDX_SIGN_TEXT:
		if (banner->flags&BANNER_FLAG_2){
			rct_ride* ride = GET_RIDE(banner->colour);
			RCT2_GLOBAL(0x13CE962, uint32) = ride->name_arguments;
			string_id = ride->name;
		}
		else
		{
			string_id = gBanners[w->number].string_idx;
		}
		window_text_input_open(w, WIDX_SIGN_TEXT, 2992, 2993, string_id, 0, 32);
		break;
	}
}

/* rct2: 0x6B9784 & 0x6E6164 */
static void window_sign_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	rct_banner* banner = &gBanners[w->number];

	switch (widgetIndex) {
	case WIDX_MAIN_COLOR:
		window_dropdown_show_colour(w, widget, w->colours[1] | 0x80, (uint8)w->list_information_type);
		break;
	case WIDX_TEXT_COLOR:
		window_dropdown_show_colour(w, widget, w->colours[1] | 0x80, (uint8)w->var_492);
		break;
	}
}

/* rct2: 0x6B979C */
static void window_sign_dropdown()
{
	short widgetIndex, dropdownIndex;
	rct_window* w;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	switch (widgetIndex){
	case WIDX_MAIN_COLOR:
		if (dropdownIndex == -1) return;
		w->list_information_type = dropdownIndex;
		break;
	case WIDX_TEXT_COLOR:
		if (dropdownIndex == -1) return;
		w->var_492 = dropdownIndex;
		break;
	default:
		return;
	}

	rct_banner* banner = &gBanners[w->number];
	int x = banner->x << 5;
	int y = banner->y << 5;

	rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

	while (1){
		if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE) {
			int ebx = map_element->properties.scenerymultiple.type;
			ebx |= (map_element->properties.scenerymultiple.index & 0x3) << 8;
			rct_scenery_entry* scenery_entry = g_largeSceneryEntries[ebx];
			if (scenery_entry->large_scenery.var_11 != 0xFF){
				int id = (map_element->type & 0xC0) |
					((map_element->properties.scenerymultiple.colour[0] & 0xE0) >> 2) |
					((map_element->properties.scenerymultiple.colour[1] & 0xE0) >> 5);
				if (id == w->number)
					break;
			}
		}
		map_element++;
	}

	int edx = map_element->base_height | ((map_element->properties.scenerymultiple.index >> 2) << 8);
	int ebp = w->list_information_type | (w->var_492 << 8);
	int ebx = (map_element->type & 0x3) << 8;
	RCT2_CALLPROC_X(0x6B9B05, x, ebx, y, edx, 0, w->number, ebp);
	window_invalidate(w);
}

/* rct2: 0x6B9791 & 0x6E6171*/
static void window_sign_textinput()
{
	short widgetIndex;
	rct_window *w;
	uint8 result;
	uint8* text;

	window_text_input_get_registers(w, widgetIndex, result, text);
	rct_banner* banner = &gBanners[w->number];
	int x = banner->x << 5;
	int y = banner->y << 5;

	if (widgetIndex == WIDX_SIGN_TEXT && result) {

		if (*text != 0){
			rct_string_id string_id = user_string_allocate(128, text);
			if (string_id != 0) {
				rct_string_id prev_string_id = banner->string_idx;
				banner->string_idx = string_id;
				user_string_free(prev_string_id);

				banner->flags &= ~(BANNER_FLAG_2);
				gfx_invalidate_screen();
			} else {
				window_error_open(2984, RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id));
			}
		}
		else{
			int eax = x, ebx = 0, ecx = y, edx = 16, ebp = 0, edi = 0, esi = 0;
			RCT2_CALLFUNC_X(0x6B7D86, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
			if ((eax & 0xFF) == 0xFF)return;
			banner->colour = eax & 0xFF;
			banner->flags |= BANNER_FLAG_2;

			rct_string_id prev_string_id = banner->string_idx;
			banner->string_idx = 778;
			user_string_free(prev_string_id);
			gfx_invalidate_screen();
		}
	}
}

/* rct2: 0x006B96F5 */
static void window_sign_invalidate()
{
	rct_window* w;

	window_get_register(w);

	rct_widget* main_colour_btn = &window_sign_widgets[WIDX_MAIN_COLOR];
	rct_widget* text_colour_btn = &window_sign_widgets[WIDX_TEXT_COLOR];

	rct_scenery_entry* scenery_entry = g_largeSceneryEntries[w->var_48C];

	main_colour_btn->type = WWT_EMPTY;
	text_colour_btn->type = WWT_EMPTY;

	if (scenery_entry->large_scenery.flags&(1 << 0)){
		main_colour_btn->type = WWT_COLORBTN;
	}
	if (scenery_entry->large_scenery.flags&(1 << 1)) {
		text_colour_btn->type = WWT_COLORBTN;
	}

	main_colour_btn->image = (w->list_information_type << 19) | 0x600013C3;
	text_colour_btn->image = (w->var_492 << 19) | 0x600013C3;
}

/* rct2: 0x006B9754 & 0x006E6134 */
static void window_sign_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	// Draw viewport
	if (w->viewport != NULL) {
		window_draw_viewport(dpi, w);
	}
}

/* rct2: 0x6B9A6C & 0x6E6424 */
static void window_sign_unknown_14()
{
	rct_window* w;
	window_get_register(w);

	rct_viewport* view = w->viewport;
	w->viewport = 0;

	view->width = 0;
	viewport_update_pointers();

	rct_banner* banner = &gBanners[w->number];

	int view_x = (banner->x << 5) + 16;
	int view_y = (banner->y << 5) + 16;
	int view_z = w->frame_no;

	// Create viewport
	rct_widget* viewportWidget = &window_sign_widgets[WIDX_VIEWPORT];
	viewport_create(
		w,
		w->x + viewportWidget->left + 1,
		w->y + viewportWidget->top + 1,
		(viewportWidget->right - viewportWidget->left) - 1,
		(viewportWidget->bottom - viewportWidget->top) - 1,
		0,
		view_x,
		view_y,
		view_z,
		0,
		-1
		);

	w->viewport->flags = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES) ? VIEWPORT_FLAG_GRIDLINES : 0;
	window_invalidate(w);
}


/* rct2: 0x6E5F52 */
void window_sign_small_open(rct_windownumber number){
	rct_window* w;
	rct_widget *viewportWidget;


	// Check if window is already open
	w = window_bring_to_front_by_number(WC_BANNER, number);
	if (w != NULL)
		return;

	w = window_create_auto_pos(WW, WH, (uint32*)window_sign_small_events, WC_BANNER, 0);
	w->widgets = window_sign_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_SIGN_TEXT) |
		(1 << WIDX_SIGN_DEMOLISH) |
		(1 << WIDX_MAIN_COLOR) |
		(1 << WIDX_TEXT_COLOR);

	w->number = number;
	window_init_scroll_widgets(w);
	w->colours[0] = 24;
	w->colours[1] = 24;
	w->colours[2] = 24;

	int view_x = gBanners[w->number].x << 5;
	int view_y = gBanners[w->number].y << 5;
	
	rct_map_element* map_element = map_get_first_element_at(view_x / 32, view_y / 32);

	while (1){
		if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_FENCE) {
			rct_scenery_entry* scenery_entry = g_wallSceneryEntries[map_element->properties.fence.slope];
			if (scenery_entry->wall.var_0D != 0xFF){
				if (map_element->properties.fence.item[0] == w->number)
					break;
			}
		}
		map_element++;
	}

	int view_z = map_element->base_height << 3;
	w->frame_no = view_z;

	w->list_information_type = map_element->properties.fence.item[1] & 0x1F;
	w->var_492 = (map_element->properties.fence.item[1] >> 5) | ((map_element->flags&0x60) >> 2);
	w->var_48C = map_element->properties.fence.slope;

	view_x += 16;
	view_y += 16;

	// Create viewport
	viewportWidget = &window_sign_widgets[WIDX_VIEWPORT];
	viewport_create(
		w,
		w->x + viewportWidget->left + 1,
		w->y + viewportWidget->top + 1,
		(viewportWidget->right - viewportWidget->left) - 1,
		(viewportWidget->bottom - viewportWidget->top) - 1,
		0,
		view_x,
		view_y,
		view_z,
		0,
		-1
	);

	w->viewport->flags = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES) ? VIEWPORT_FLAG_GRIDLINES : 0;
	w->flags |= WF_2;
	window_invalidate(w);
}

/* rct2: 0x6E6145 */
static void window_sign_small_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	rct_banner* banner = &gBanners[w->number];
	int x = banner->x << 5;
	int y = banner->y << 5;

	rct_string_id string_id;

	rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_SIGN_DEMOLISH:
		while (1){
			if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_FENCE) {
				rct_scenery_entry* scenery_entry = g_wallSceneryEntries[map_element->properties.fence.slope];
				if (scenery_entry->wall.var_0D != 0xFF){
					if (map_element->properties.fence.item[0] == w->number)
						break;
				}
			}
			map_element++;
		}
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1158;
		game_do_command(
			x,
			1 | ((map_element->type & 0x3) << 8),
			y,
			(map_element->base_height << 8) | (map_element->type & 0x3),
			GAME_COMMAND_REMOVE_FENCE,
			0,
			0);
		break;
	case WIDX_SIGN_TEXT:
		if (banner->flags&BANNER_FLAG_2){
			rct_ride* ride = GET_RIDE(banner->colour);
			RCT2_GLOBAL(0x13CE962, uint32) = ride->name_arguments;
			string_id = ride->name;
		}
		else
		{
			string_id = gBanners[w->number].string_idx;
		}
		window_text_input_open(w, WIDX_SIGN_TEXT, 2992, 2993, string_id, 0, 32);
		break;
	}
}

/* rct2: 0x6E617C */
static void window_sign_small_dropdown()
{
	short widgetIndex, dropdownIndex;
	rct_window* w;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	switch (widgetIndex){
	case WIDX_MAIN_COLOR:
		if (dropdownIndex == -1) return;
		w->list_information_type = dropdownIndex;
		break;
	case WIDX_TEXT_COLOR:
		if (dropdownIndex == -1) return;
		w->var_492 = dropdownIndex;
		break;
	default:
		return;
	}

	rct_banner* banner = &gBanners[w->number];
	int x = banner->x << 5;
	int y = banner->y << 5;

	rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

	while (1){
		if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_FENCE) {
			rct_scenery_entry* scenery_entry = g_wallSceneryEntries[map_element->properties.fence.slope];
			if (scenery_entry->wall.var_0D != 0xFF){
				if (map_element->properties.fence.item[0] == w->number)
					break;
			}
		}
		map_element++;
	}

	map_element->flags &= 0x9F;
	map_element->properties.fence.item[1] =
		w->list_information_type |
		((w->var_492 & 0x7) << 5);
	map_element->flags |= ((w->var_492 & 0x18) << 2);

	RCT2_CALLPROC_X(0x6EC847, x, 0, y, 0, map_element->clearance_height << 3, map_element->base_height << 3, 0);
	window_invalidate(w);
}

/* rct2: 0x006E60D5 */
static void window_sign_small_invalidate()
{
	rct_window* w;

	window_get_register(w);

	rct_widget* main_colour_btn = &window_sign_widgets[WIDX_MAIN_COLOR];
	rct_widget* text_colour_btn = &window_sign_widgets[WIDX_TEXT_COLOR];

	rct_scenery_entry* scenery_entry = g_wallSceneryEntries[w->var_48C];

	main_colour_btn->type = WWT_EMPTY;
	text_colour_btn->type = WWT_EMPTY;

	if (scenery_entry->wall.flags&(1 << 0)){
		main_colour_btn->type = WWT_COLORBTN;
	}
	if (scenery_entry->wall.flags&(1 << 6)) {
		text_colour_btn->type = WWT_COLORBTN;
	}

	main_colour_btn->image = (w->list_information_type << 19) | 0x600013C3;
	text_colour_btn->image = (w->var_492 << 19) | 0x600013C3;
}