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

static void* window_editor_object_selection_events[] = {
	(void*)0x006AB199,
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
	(void*)0x006AB0B6,
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
			object_free_scenario_text();
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
	object_free_scenario_text();
	window_invalidate(w);
}