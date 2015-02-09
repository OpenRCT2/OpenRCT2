/*****************************************************************************
* Copyright (c) 2014 Dániel Tar
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
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../object.h"
#include "../audio/audio.h"
#include "../ride/track.h"
#include "error.h"

#pragma region Widgets

enum WINDOW_STAFF_LIST_WIDGET_IDX {
	WIDX_EDITOR_OBJECT_SELECTION_BACKGROUND,			// 0, 1
	WIDX_EDITOR_OBJECT_SELECTION_TITLE,					// 1, 2
	WIDX_EDITOR_OBJECT_SELECTION_CLOSE,					// 2, 4
	WIDX_EDITOR_OBJECT_SELECTION_TAB_CONTENT_PANEL,		// 3, 8
	WIDX_EDITOR_OBJECT_SELECTION_TAB_1,					// 4, 10
	WIDX_EDITOR_OBJECT_SELECTION_TAB_2,					// 5, 20
	WIDX_EDITOR_OBJECT_SELECTION_TAB_3,					// 6, 40
	WIDX_EDITOR_OBJECT_SELECTION_TAB_4,					// 7, 80
	WIDX_EDITOR_OBJECT_SELECTION_TAB_5,					// 8, 100
	WIDX_EDITOR_OBJECT_SELECTION_TAB_6,					// 9, 200
	WIDX_EDITOR_OBJECT_SELECTION_TAB_7,					// 10, 400
	WIDX_EDITOR_OBJECT_SELECTION_TAB_8,					// 11, 800
	WIDX_EDITOR_OBJECT_SELECTION_TAB_9,					// 12, 1000
	WIDX_EDITOR_OBJECT_SELECTION_TAB_10,				// 13, 2000
	WIDX_EDITOR_OBJECT_SELECTION_TAB_11,				// 14, 4000
	WIDX_EDITOR_OBJECT_SELECTION_DROPDOWN1,				// 15, 8000
	WIDX_EDITOR_OBJECT_SELECTION_LIST,					// 16, 10000
	WIDX_EDITOR_OBJECT_SELECTION_FLATBTN,				// 17, 20000
	WIDX_EDITOR_OBJECT_SELECTION_DROPDOWN2,				// 18, 40000
};

#pragma endregion

#pragma region Events

static void window_editor_object_selection_emptysub() { }

static void window_editor_object_selection_mouseup();
static void window_editor_object_selection_scroll_mousedown();
static void window_editor_object_selection_close();

static void* window_editor_object_selection_events[] = {
	window_editor_object_selection_close,
	(void*)window_editor_object_selection_mouseup,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)0x006AB031,
	window_editor_object_selection_scroll_mousedown,
	(void*)window_editor_object_selection_emptysub,
	(void*)0x006AB079,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)0x006AB058,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)0x006AA9FD,
	(void*)0x006AAB56,
	(void*)0x006AADA3
};

#pragma endregion

static void window_editor_object_set_page(rct_window *w, int page);

/**
 *
 *  rct2: 0x006AA64E
 */
void window_editor_object_selection_open()
{
	rct_window* window;

	window = window_bring_to_front_by_class(WC_EDITOR_OBJECT_SELECTION);
	if (window != NULL)
		return;

	RCT2_CALLPROC_EBPSAFE(0x006AB211);
	RCT2_CALLPROC_EBPSAFE(0x006AA770);

	window = window_create(
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) / 2 - 300,
		max(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) / 2 - 200, 28),
		600,
		400,
		(uint32*)window_editor_object_selection_events,
		WC_EDITOR_OBJECT_SELECTION,
		WF_STICK_TO_FRONT
	);
	window->widgets = (rct_widget*)0x009ADB00;

	window->enabled_widgets =
		(1 << WIDX_EDITOR_OBJECT_SELECTION_DROPDOWN1) |
		(1 << WIDX_EDITOR_OBJECT_SELECTION_DROPDOWN2) |
		(1 << WIDX_EDITOR_OBJECT_SELECTION_CLOSE);

	for (int i = WIDX_EDITOR_OBJECT_SELECTION_TAB_1; i <= WIDX_EDITOR_OBJECT_SELECTION_TAB_11; i++)
		window->enabled_widgets |= (1LL << i);
	window_init_scroll_widgets(window);

	window->var_4AE = 0;
	window->selected_tab = 0;
	window->selected_list_item = -1;
	window->var_494 = 0xFFFFFFFF;
	window->colours[0] = 4;
	window->colours[1] = 1;
	window->colours[2] = 1;
}

static void window_editor_object_selection_mouseup()
{
	rct_window *w;
	short widgetIndex;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_EDITOR_OBJECT_SELECTION_CLOSE:
		game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 1, 0);
		break;

	case WIDX_EDITOR_OBJECT_SELECTION_TAB_1:
	case WIDX_EDITOR_OBJECT_SELECTION_TAB_2:
	case WIDX_EDITOR_OBJECT_SELECTION_TAB_3:
	case WIDX_EDITOR_OBJECT_SELECTION_TAB_4:
	case WIDX_EDITOR_OBJECT_SELECTION_TAB_5:
	case WIDX_EDITOR_OBJECT_SELECTION_TAB_6:
	case WIDX_EDITOR_OBJECT_SELECTION_TAB_7:
	case WIDX_EDITOR_OBJECT_SELECTION_TAB_8:
	case WIDX_EDITOR_OBJECT_SELECTION_TAB_9:
	case WIDX_EDITOR_OBJECT_SELECTION_TAB_10:
	case WIDX_EDITOR_OBJECT_SELECTION_TAB_11:
		window_editor_object_set_page(w, widgetIndex - WIDX_EDITOR_OBJECT_SELECTION_TAB_1);
		break;

	case WIDX_EDITOR_OBJECT_SELECTION_DROPDOWN1:
		w->list_information_type ^= 1;
		window_invalidate(w);
		break;

	case WIDX_EDITOR_OBJECT_SELECTION_DROPDOWN2:
		if (w->selected_list_item != -1) {
			w->selected_list_item = -1;
			RCT2_CALLPROC_EBPSAFE(0x006A982D); // object_free_scenario_text();
		}
		window_invalidate(w);

		int eax, ebx, ecx, edx, esi, edi, ebp;
		RCT2_CALLFUNC_X(0x00674FCE, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
		if (eax == 1) {
			window_close(w);
			RCT2_CALLPROC_EBPSAFE(0x006D386D);
		}
		break;
	}
}

static void window_editor_object_set_page(rct_window *w, int page)
{
	if (w->selected_tab == page)
		return;

	w->selected_tab = page;
	w->selected_list_item = -1;
	w->var_494 = 0xFFFFFFFF;
	w->scrolls[0].v_top = 0;
	RCT2_CALLPROC_EBPSAFE(0x006A982D); // object_free_scenario_text();
	window_invalidate(w);
}

/* rct2: 0x006AA703 
 * Takes the y coordinate of the clicked on scroll list
 * and converts this into an object selection.
 * Returns the position in the list.
 * Object_selection_flags, installed_entry also populated
 */
int get_object_from_object_selection(uint8 object_type, int y, uint8* object_selection_flags, rct_object_entry** installed_entry){
	*installed_entry = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
	uint8* selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	int object_count = 0;
	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i){
		if (((*installed_entry)->flags & 0xF) == object_type){
			if (!(*selection_flags & 0x20)){
				y -= 12;
				*object_selection_flags = *selection_flags;
				if (y < 0)return object_count;
				object_count++;
			}
		}

		*installed_entry = object_get_next(*installed_entry);
		selection_flags++;
	}
	return -1;
}

/* rct2: 0x006D33E2 */
void sub_6d33e2(){
	RCT2_GLOBAL(0x1357404, sint32) = -1;
	RCT2_GLOBAL(0x1357408, sint32) = -1;
	RCT2_GLOBAL(0x135740C, sint32) = -1;
	RCT2_GLOBAL(0x1357410, sint32) = -1;

	for (int i = 0; i < 128; ++i){
		RCT2_ADDRESS(0x1357444, uint32)[i] = RCT2_ADDRESS(0x97C468, uint32)[i];
		RCT2_ADDRESS(0x1357644, uint32)[i] = RCT2_ADDRESS(0x97C5D4, uint32)[i];
	}

	for (int i = 0; i < 8; ++i){
		RCT2_ADDRESS(0x1357424, sint32)[i] = -1;
	}

	RCT2_GLOBAL(0x141F570, uint8) = 7;

	int entry_index = 0;
	for (; ((int)object_entry_groups[0].chunks[entry_index]) == -1; ++entry_index);

	RCT2_GLOBAL(0xF44157, uint8) = entry_index;

	rct_ride_type* ride_entry = GET_RIDE_ENTRY(entry_index);
	uint8* ride_type_array = &ride_entry->var_00C;

	int ride_type;
	for (int i = 0; (ride_type = ride_type_array[i]) == 0xFF; i++);
	RCT2_GLOBAL(0xF44158, uint8) = ride_type;

	ride_list_item item = { ride_type, entry_index };
	track_load_list(item);
	window_track_list_open(item);
}

/* rct2: 0x006AB0B6 */
static void window_editor_object_selection_scroll_mousedown(){
	short x, y, scrollIndex;
	rct_window *w;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	uint8 object_selection_flags;
	rct_object_entry* installed_entry;
	int selected_object = get_object_from_object_selection((w->selected_tab & 0xFF), y, &object_selection_flags, &installed_entry);
	if (selected_object == -1) return;

	if (object_selection_flags & 0x20)return;

	window_invalidate(w);

	sound_play_panned(SOUND_CLICK_1, RCT2_GLOBAL(0x142406C,uint32), 0, 0, 0);

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER){
		if (RCT2_CALLPROC_X(0x6AB54F, 0, 1, 0, 0, 0, 0, (int)installed_entry) & 0x100)return;

		window_close(w);

		//This function calls window_track_list_open
		sub_6d33e2();
		return;
	}

	int ebx = 6;
	// If already selected
	if (!(object_selection_flags & 1))
		ebx = 7;

	RCT2_GLOBAL(0xF43411, uint8) = 0;
	if (0x100 & RCT2_CALLPROC_X(0x6AB54F, 0, ebx, 0, 0, 0, 0, (int)installed_entry)){

		rct_string_id error_title;
		if (ebx & 1)
			error_title = 3176;
		else
			error_title = 3177;

		window_error_open(error_title, RCT2_GLOBAL(0x141E9AC, uint16));
		return;
	}

	if (!RCT2_GLOBAL(0xF43411, uint8) & 1)return;

	window_error_open(3374, 3375);
}

/* rct2:0x006ABBBE */
static void editor_load_selected_objects(){
	uint8* selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	rct_object_entry* installed_entry = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32) == 0)return;

	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i != 0; --i, selection_flags++){

		if (*selection_flags & 1){

			uint8 entry_index, entry_type;
			if (!find_object_in_entry_group(installed_entry, &entry_type, &entry_index)){
				int chunk_size;
				if (!object_load(-1, installed_entry, &chunk_size)){
					log_error("Failed to load entry %.8s", installed_entry->name);
				}
			}
		}

		installed_entry = object_get_next(installed_entry);
	}
}

/* rct2: 0x006AB199 */
static void window_editor_object_selection_close(){
	rct_window* w;
	window_get_register(w);

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 
		(SCREEN_FLAGS_SCENARIO_EDITOR | 
		SCREEN_FLAGS_TRACK_DESIGNER | 
		SCREEN_FLAGS_TRACK_MANAGER))
		)return;

	RCT2_CALLPROC_EBPSAFE(0x6ABB66);
	editor_load_selected_objects();
	sub_6A9FC0();
	RCT2_CALLPROC_EBPSAFE(0x6A982D);
	RCT2_CALLPROC_EBPSAFE(0x6AB316);
	RCT2_CALLPROC_EBPSAFE(0x685675);
	RCT2_CALLPROC_EBPSAFE(0x68585B);
	window_new_ride_init_vars();
}