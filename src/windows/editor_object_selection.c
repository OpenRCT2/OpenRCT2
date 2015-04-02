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
#include "../audio/audio.h"
#include "../game.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../management/research.h"
#include "../object.h"
#include "../ride/track.h"
#include "../scenario.h"
#include "error.h"

enum {
	WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS,
	WINDOW_OBJECT_SELECTION_PAGE_SMALL_SCENERY,
	WINDOW_OBJECT_SELECTION_PAGE_LARGE_SCENERY,
	WINDOW_OBJECT_SELECTION_PAGE_WALLS_FENCES,
	WINDOW_OBJECT_SELECTION_PAGE_PATH_SIGNS,
	WINDOW_OBJECT_SELECTION_PAGE_FOOTPATHS,
	WINDOW_OBJECT_SELECTION_PAGE_PATH_EXTRAS,
	WINDOW_OBJECT_SELECTION_PAGE_SCENERY_GROUPS,
	WINDOW_OBJECT_SELECTION_PAGE_PARK_ENTRANCE,
	WINDOW_OBJECT_SELECTION_PAGE_WATER,
	WINDOW_OBJECT_SELECTION_PAGE_SCENARIO_DESCRIPTION,
	WINDOW_OBJECT_SELECTION_PAGE_COUNT
};

#pragma region Widgets

enum WINDOW_STAFF_LIST_WIDGET_IDX {
	WIDX_BACKGROUND,			// 0, 1
	WIDX_TITLE,					// 1, 2
	WIDX_CLOSE,					// 2, 4
	WIDX_TAB_CONTENT_PANEL,		// 3, 8
	WIDX_TAB_1,					// 4, 10
	WIDX_TAB_2,					// 5, 20
	WIDX_TAB_3,					// 6, 40
	WIDX_TAB_4,					// 7, 80
	WIDX_TAB_5,					// 8, 100
	WIDX_TAB_6,					// 9, 200
	WIDX_TAB_7,					// 10, 400
	WIDX_TAB_8,					// 11, 800
	WIDX_TAB_9,					// 12, 1000
	WIDX_TAB_10,				// 13, 2000
	WIDX_TAB_11,				// 14, 4000
	WIDX_DROPDOWN1,				// 15, 8000
	WIDX_LIST,					// 16, 10000
	WIDX_PREVIEW,				// 17, 20000
	WIDX_DROPDOWN2,				// 18, 40000
};

static rct_widget window_editor_object_selection_widgets[] = {
	{ WWT_FRAME,			0,	0,		599,	0,		399,	0xFFFFFFFF,						STR_NONE },
	{ WWT_CAPTION,			0,	1,		598,	1,		14,		3181,							STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	587,	597,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		599,	43,		399,	0xFFFFFFFF,						STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						1812 },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,						1812 },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						1812 },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						1812 },
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						1812 },
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						1812 },
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						1812 },
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,						1812 },
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,						1812 },
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,						1812 },
	{ WWT_TAB,				1,	313,	343,	17,		43,		0x2000144E,						1812 },
	{ WWT_DROPDOWN_BUTTON,	0,	470,	591,	23,		34,		STR_OBJECT_SELECTION_ADVANCED,	STR_OBJECT_SELECTION_ADVANCED_TIP },
	{ WWT_SCROLL,			1,	4,		291,	46,		386,	2,								STR_NONE },
	{ WWT_FLATBTN,			1,	391,	504,	46,		159,	0xFFFFFFFF,						STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	384,	595,	24,		35,		STR_INSTALL_NEW_TRACK_DESIGN,	STR_INSTALL_NEW_TRACK_DESIGN_TIP },
	{ WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_editor_object_selection_emptysub() { }

static void window_editor_object_selection_close();
static void window_editor_object_selection_mouseup();
static void window_editor_object_selection_scrollgetsize();
static void window_editor_object_selection_scroll_mousedown();
static void window_editor_object_selection_scroll_mouseover();
static void window_editor_object_selection_tooltip();
static void window_editor_object_selection_invalidate();
static void window_editor_object_selection_paint();
static void window_editor_object_selection_scrollpaint();

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
	(void*)window_editor_object_selection_scrollgetsize,
	(void*)window_editor_object_selection_scroll_mousedown,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_scroll_mouseover,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_tooltip,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_emptysub,
	(void*)window_editor_object_selection_invalidate,
	(void*)window_editor_object_selection_paint,
	(void*)window_editor_object_selection_scrollpaint
};

#pragma endregion

static void window_editor_object_set_page(rct_window *w, int page);
static void window_editor_object_selection_set_pressed_tab(rct_window *w);
static void window_editor_object_selection_select_default_objects();
static int window_editor_object_selection_select_object(int flags, rct_object_entry *entry);
static int get_object_from_object_selection(uint8 object_type, int y, uint8 *object_selection_flags, rct_object_entry **installed_entry);
static void window_editor_object_selection_manage_tracks();
static void editor_load_selected_objects();

static rct_object_entry DefaultSelectedObjects[] = {
	// Objects that are always required
	{ 0x00000087, { "SCGTREES" }, 0 },		// Scenery: Trees
	{ 0x00000087, { "SCGSHRUB" }, 0 },		// Scenery: Shrubs and Ornaments
	{ 0x00000087, { "SCGGARDN" }, 0 },		// Scenery: Gardens
	{ 0x00000087, { "SCGFENCE" }, 0 },		// Scenery: Fences and Walls
	{ 0x00000087, { "SCGWALLS" }, 0 },		// Scenery: Walls and Roofs
	{ 0x00000087, { "SCGPATHX" }, 0 },		// Scenery: Signs and Items for Footpaths
	{ 0x00000085, { "TARMAC  " }, 0 },		// Footpath: Tarmac

	// An initial default selection
	{ 0x000080FF, { "TWIST1  " }, 0 },		// Ride: Twist
	{ 0x00008000, { "PTCT1	 " }, 0 },		// Ride: Wooden Roller Coaster (Wooden Roller Coaster Trains)
	{ 0x00008000, { "ZLDB	 " }, 0 },		// Ride: Junior Roller Coaster (Ladybird Trains)
	{ 0x00008000, { "LFB1	 " }, 0 },		// Ride: Log Flume
	{ 0x00008000, { "VCR	 " }, 0 },		// Ride: Vintage Cars
	{ 0x00008000, { "MGR1	 " }, 0 },		// Ride: Merry-Go-Round
	{ 0x00008000, { "TLT1	 " }, 0 },		// Ride: Restroom
	{ 0x00008000, { "ATM1	 " }, 0 },		// Ride: Cash Machine
	{ 0x00008000, { "FAID1	 " }, 0 },		// Ride: First Aid Room
	{ 0x00008000, { "INFOK	 " }, 0 },		// Ride: Information Kiosk
	{ 0x00008000, { "DRNKS	 " }, 0 },		// Ride: Drinks Stall
	{ 0x00008000, { "CNDYF	 " }, 0 },		// Ride: Cotten Candy Stall
	{ 0x00008000, { "BURGB	 " }, 0 },		// Ride: Burger Bar
	{ 0x00008000, { "BALLN	 " }, 0 },		// Ride: Balloon Stall
	{ 0x00008000, { "ARRT1	 " }, 0 },		// Ride: Corkscrew Roller Coaster
	{ 0x00008000, { "RBOAT	 " }, 0 },		// Ride: Rowing Boats
	{ 0x00008800, { "PKENT1	 " }, 0 },		// Park Entrace: Traditional Park Entrance
	{ 0x00008900, { "WTRCYAN " }, 0 },		// Water: Natural Water
	{ 0x00008500, { "TARMACB " }, 0 },		// Footpath: Brown Tarmac Footpath
	{ 0x00008500, { "PATHSPC " }, 0 },		// Footpath: Space Style Footpath
	{ 0x00008500, { "PATHDIR " }, 0 },		// Footpath: Dirt Footpath
	{ 0x00008500, { "PATHCRZ " }, 0 },		// Footpath: Crazy Paving Footpath
	{ 0x00008500, { "PATHASH " }, 0 },		// Footpath: Ash Footpath

	// The following are for all random map generation features to work out the box
	{ 0x00000087, { "SCGJUNGL" }, 0 },		// Jungle Themeing
	{ 0x00000087, { "SCGSNOW " }, 0 },		// Snow and Ice Themeing
	{ 0x00000087, { "SCGWATER" }, 0 }		// Water Feature Themeing
};

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

	// Not really where its called, but easy way to change default objects for now
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR)
		window_editor_object_selection_select_default_objects();

	window = window_create_centred(
		600,
		400,
		(uint32*)window_editor_object_selection_events,
		WC_EDITOR_OBJECT_SELECTION,
		WF_STICK_TO_FRONT
	);
	window->widgets = window_editor_object_selection_widgets;

	window->enabled_widgets =
		(1 << WIDX_DROPDOWN1) |
		(1 << WIDX_DROPDOWN2) |
		(1 << WIDX_CLOSE);

	for (int i = WIDX_TAB_1; i <= WIDX_TAB_11; i++)
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

/**
 * 
 *  rct2: 0x006AB199
 */
static void window_editor_object_selection_close()
{
	rct_window* w;
	window_get_register(w);

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 
		(SCREEN_FLAGS_SCENARIO_EDITOR | 
		SCREEN_FLAGS_TRACK_DESIGNER | 
		SCREEN_FLAGS_TRACK_MANAGER))
		)return;

	RCT2_CALLPROC_EBPSAFE(0x6ABB66);
	editor_load_selected_objects();
	reset_loaded_objects();
	object_free_scenario_text();
	RCT2_CALLPROC_EBPSAFE(0x6AB316);
	research_populate_list_random();
	research_remove_non_separate_vehicle_types();
	window_new_ride_init_vars();
}

/**
 * 
 *  rct2: 0x006AAFAB
 */
static void window_editor_object_selection_mouseup()
{
	rct_window *w;
	short widgetIndex;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 1, 0);
		break;

	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
	case WIDX_TAB_11:
		window_editor_object_set_page(w, widgetIndex - WIDX_TAB_1);
		break;

	case WIDX_DROPDOWN1:
		w->list_information_type ^= 1;
		window_invalidate(w);
		break;

	case WIDX_DROPDOWN2:
		if (w->selected_list_item != -1) {
			w->selected_list_item = -1;
			object_free_scenario_text();
		}
		window_invalidate(w);

		window_loadsave_open(LOADSAVETYPE_LOAD | LOADSAVETYPE_TRACK);
		break;
	}
}

/**
 * 
 *  rct2: 0x006AB031
 */
static void window_editor_object_selection_scrollgetsize()
{
	rct_window *w;
	short scrollIndex;
	int numItems, width, height;

	window_scroll_get_registers(w, scrollIndex);

	numItems = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER) ?
		RCT2_GLOBAL(0x00F43412, uint16) :
		RCT2_ADDRESS(0x00F433E1, uint16)[w->selected_tab];

	width = 0;
	height = numItems * 12;
	window_scrollsize_set_registers(width, height);
}

/**
 * 
 *  rct2: 0x006AB0B6
 */
static void window_editor_object_selection_scroll_mousedown()
{
	short x, y, scrollIndex;
	rct_window *w;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	uint8 object_selection_flags;
	rct_object_entry* installed_entry;
	int selected_object = get_object_from_object_selection((w->selected_tab & 0xFF), y, &object_selection_flags, &installed_entry);
	if (selected_object == -1 || (object_selection_flags & 0x20))
		return;

	window_invalidate(w);

	sound_play_panned(SOUND_CLICK_1, RCT2_GLOBAL(0x142406C,uint32), 0, 0, 0);

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
		if (!window_editor_object_selection_select_object(1, installed_entry))
			return;

		window_close(w);

		//This function calls window_track_list_open
		window_editor_object_selection_manage_tracks();
		return;
	}

	int ebx = 6;
	// If already selected
	if (!(object_selection_flags & 1))
		ebx = 7;

	RCT2_GLOBAL(0xF43411, uint8) = 0;
	if (!window_editor_object_selection_select_object(ebx, installed_entry)) {
		rct_string_id error_title = ebx & 1 ?
			STR_UNABLE_TO_SELECT_THIS_OBJECT :
			STR_UNABLE_TO_DE_SELECT_THIS_OBJECT;

		window_error_open(error_title, RCT2_GLOBAL(0x141E9AC, uint16));
		return;
	}

	if (!RCT2_GLOBAL(0xF43411, uint8) & 1)
		return;

	window_error_open(STR_WARNING_TOO_MANY_OBJECTS_SELECTED, STR_NOT_ALL_OBJECTS_IN_THIS_SCENERY_GROUP_COULD_BE_SELECTED);
}

/**
 * 
 *  rct2: 0x006AB079
 */
static void window_editor_object_selection_scroll_mouseover()
{
	rct_window *w;
	rct_object_entry *installedEntry;
	int selectedObject;
	short x, y, scrollIndex;
	uint8 objectSelectionFlags;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	selectedObject = get_object_from_object_selection(
		w->selected_tab & 0xFF, y, &objectSelectionFlags, &installedEntry
	);
	if (objectSelectionFlags & 0x20)
		selectedObject = -1;

	if (selectedObject == w->selected_list_item)
		return;

	w->selected_list_item = selectedObject;
	w->var_494 = (uint32)installedEntry;
	object_free_scenario_text();
	if (selectedObject != -1)
		object_get_scenario_text(installedEntry);

	window_invalidate(w);
}

/**
 * 
 *  rct2: 0x006AB058
 */
static void window_editor_object_selection_tooltip()
{
	rct_window *w;
	short widgetIndex;

	window_scroll_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
	case WIDX_TAB_11:
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, rct_string_id) =
			STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS + (widgetIndex - WIDX_TAB_1);
		break;
	default:
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, rct_string_id) = STR_LIST;
		break;
	}
}

/**
 * 
 *  rct2: 0x006AA9FD
 */
static void window_editor_object_selection_invalidate()
{
	int i, x;
	rct_window *w;
	rct_widget *widget;

	window_get_register(w);

	// Set pressed widgets
	w->pressed_widgets |= 1 << WIDX_PREVIEW;
	window_editor_object_selection_set_pressed_tab(w);
	if (w->list_information_type & 1)
		w->pressed_widgets |= (1 << WIDX_DROPDOWN1);
	else
		w->pressed_widgets &= ~(1 << WIDX_DROPDOWN1);

	// Set window title and buttons
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, rct_string_id) = STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS + w->selected_tab;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
		w->widgets[WIDX_TITLE].image = STR_TRACK_DESIGNS_MANAGER_SELECT_RIDE_TYPE;
		w->widgets[WIDX_CLOSE].type = WWT_EMPTY;
		w->widgets[WIDX_DROPDOWN2].type = WWT_DROPDOWN_BUTTON;
	} else if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER) {
		w->widgets[WIDX_TITLE].image = STR_ROLLER_COASTER_DESIGNER_SELECT_RIDE_TYPES_VEHICLES;
		w->widgets[WIDX_CLOSE].type = WWT_CLOSEBOX;
		w->widgets[WIDX_DROPDOWN2].type = WWT_EMPTY;
	} else {
		w->widgets[WIDX_TITLE].image = STR_OBJECT_SELECTION;
		w->widgets[WIDX_CLOSE].type = WWT_CLOSEBOX;
		w->widgets[WIDX_DROPDOWN2].type = WWT_EMPTY;
	}

	// Align tabs, hide advanced ones
	x = 3;
	for (i = 0; i < WINDOW_OBJECT_SELECTION_PAGE_COUNT; i++) {
		widget = &w->widgets[WIDX_TAB_1 + i];

		if (!(w->list_information_type & 1) && ((1 << i) & 0x5E)) {
			widget->type = WWT_EMPTY;
		} else {
			widget->type = WWT_TAB;
			widget->left = x;
			widget->right = x + 30;
			x += 31;
		}
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TRACK_DESIGNER)) {
		w->widgets[WIDX_DROPDOWN1].type = WWT_EMPTY;
		for (i = 1; i < WINDOW_OBJECT_SELECTION_PAGE_COUNT; i++)
			w->widgets[WIDX_TAB_1 + i].type = WWT_EMPTY;
		x = 150;
	} else {
		w->widgets[WIDX_DROPDOWN1].type = WWT_DROPDOWN_BUTTON;
		x = 300;
	}

	w->widgets[WIDX_LIST].right = 587 - x;
	w->widgets[WIDX_PREVIEW].left = 537 - (x >> 1);
	w->widgets[WIDX_PREVIEW].right = w->widgets[WIDX_PREVIEW].left + 113;
}

/**
 * 
 *  rct2: 0x006AAB56
 */
static void window_editor_object_selection_paint()
{
	int i, x, y, width, numSelected, totalSelectable, type;
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_widget *widget;
	rct_object_entry *highlightedEntry;
	rct_string_id stringId;
	uint8 *text;
	char *datName, *name, *stringBuffer;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	// Draw tabs
	for (i = 0; i < WINDOW_OBJECT_SELECTION_PAGE_COUNT; i++) {
		widget = &w->widgets[WIDX_TAB_1 + i];
		if (widget->type == WWT_EMPTY)
			continue;

		x = w->x + widget->left;
		y = w->y + widget->top;
		gfx_draw_sprite(dpi, 5458 + i, x, y, 0);
	}

	// Preview background
	widget = &w->widgets[WIDX_PREVIEW];
	gfx_fill_rect(
		dpi,
		w->x + widget->left + 1,
		w->y + widget->top + 1,
		w->x + widget->right - 1,
		w->y + widget->bottom - 1,
		RCT2_ADDRESS(0x0141FC44, uint8)[w->colours[1] * 8]
	);

	// Draw number of selected items
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER)) {
		x = w->x + 3;
		y = w->y + w->height - 12;

		numSelected = RCT2_ADDRESS(0x00F433F7, uint16)[w->selected_tab];
		totalSelectable = object_entry_group_counts[w->selected_tab];
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)
			totalSelectable = 4;

		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = numSelected;
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint16) = totalSelectable;
		gfx_draw_string_left(dpi, 3164, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0, x, y);
	}

	rct_stex_entry* stex_entry = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TEXT_TEMP_CHUNK, rct_stex_entry*);

	if (w->selected_list_item == -1 || stex_entry == NULL)
		return;

	highlightedEntry = (rct_object_entry*)w->var_494;
	type = highlightedEntry->flags & 0x0F;

	// Draw preview
	widget = &w->widgets[WIDX_PREVIEW];
	x = w->x + (widget->left + widget->right) / 2 + 1;
	y = w->y + (widget->top + widget->bottom) / 2 + 1;
	object_paint(type, 3, type, x, y, 0, (int)dpi, (int)stex_entry);

	// Draw name of object
	x = w->x + (widget->left + widget->right) / 2 + 1;
	y = w->y + widget->bottom + 3;
	width = w->width - w->widgets[WIDX_LIST].right - 6;

	// Skip object dat name
	text = (char*)(highlightedEntry + 1);
	datName = text;
	do {
		text++;
	} while (*(text - 1) != 0);
	text += 4;
	name = text;

	RCT2_GLOBAL(0x009BC677, uint8) = 14;

	stringId = 3165;
	stringBuffer = (char*)language_get_string(3165) + 1;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)) {
		// Skip name
		do {
			text++;
		} while (*(text - 1) != 0);
		text += 4;
		text += *text++ * 16;
		text += *text++ * 16;

		if (RCT2_GLOBAL(text, uint32) & 0x1000000) {
			strcpy(stringBuffer, name);
		} else {
			int eax = *text;
			if (*text == 0xFF) {
				eax = *(text + 1);
				if (*(text + 1) == 0xFF)
					eax = *(text + 2);
			}
			format_string(stringBuffer, eax + 2, NULL);
		}
	} else {
		strcpy(stringBuffer, name);
	}
	gfx_draw_string_centred_clipped(dpi, stringId, NULL, 0, x, y, width);
	
	// Draw description of object
	x = w->x + w->widgets[WIDX_LIST].right + 4;
	y += 15;
	object_paint(type, 259, type, x, y, (int)w, (int)dpi, (int)stex_entry);

	// Draw object dat name
	strcpy(stringBuffer, datName);
	gfx_draw_string_right(dpi, stringId, NULL, 0, w->x + w->width - 5, w->y + w->height - 3 - 12);
}

/**
 * 
 *  rct2: 0x006AADA3
 */
static void window_editor_object_selection_scrollpaint()
{
	int x, y, i, colour, colour2, numObjects, type;
	short scrollIndex;
	rct_object_entry *entry;
	rct_window *w;
	rct_drawpixelinfo *dpi;
	uint8 *itemFlags;

	window_scrollpaint_get_registers(w, dpi, scrollIndex);

	colour = RCT2_ADDRESS(0x0141FC48, uint8)[w->colours[1] * 8];
	colour = (colour << 24) | (colour << 16) | (colour << 8) | colour;
	gfx_clear(dpi, colour);

	numObjects = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, sint32);
	entry = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
	itemFlags = RCT2_GLOBAL(0x009ADAEC, uint8*);
	y = 0;
	for (i = 0; i < numObjects; i++) {
		type = entry->flags & 0x0F;
		if (type == w->selected_tab && !(*itemFlags & 0x20)) {
			if (y + 12 >= dpi->y && y <= dpi->y + dpi->height) {
				// Draw checkbox
				if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) && !(*itemFlags & 0x20))
					gfx_fill_rect_inset(dpi, 2, y, 11, y + 10, w->colours[1], 0xE0);

				// Highlight background
				colour = 142;
				if (entry == (rct_object_entry*)w->var_494 && !(*itemFlags & 0x20)) {
					gfx_fill_rect(dpi, 0, y, w->width, y + 11, 0x2000031);
					colour = 14;
				}

				// Draw checkmark
				if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) && (*itemFlags & 1)) {
					x = 2;
					RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = colour == 14 ? -2 : -1;
					colour2 = w->colours[1] & 0x7F;
					if (*itemFlags & 0x1C)
						colour2 |= 0x40;

					gfx_draw_string(dpi, (char*)0x009DED72, colour2, x, y);
				}

				// Draw text
				char *buffer = (char*)0x0141ED68;
				*buffer = colour;
				strcpy(buffer + 1, object_get_name(entry));
				if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
					while (*buffer != 0 && *buffer != 9)
						buffer++;

					*buffer = 0;
				}

				if (*itemFlags & 0x20) {
					colour = w->colours[1] & 0x7F;
					RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = -1;
				} else {
					colour = 0;
					RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = 224;
				}
				x = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER ? 0 : 15;
				gfx_draw_string(dpi, (char*)0x0141ED68, colour, x, y);
			}
			y += 12;
		}

		entry = object_get_next(entry);
		itemFlags++;
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

static void window_editor_object_selection_set_pressed_tab(rct_window *w)
{
	int i;
	for (i = 0; i < WINDOW_OBJECT_SELECTION_PAGE_COUNT; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->selected_tab);
}

/**
 *
 *  rct2: 0x006AA7E9
 */
static void window_editor_object_selection_select_default_objects()
{
	int i;

	if (RCT2_GLOBAL(0x00F433F7, uint16) == 0)
		return;

	for (i = 0; i < countof(DefaultSelectedObjects); i++)
		window_editor_object_selection_select_object(7, &DefaultSelectedObjects[i]);
}

/**
 *
 *  rct2: 0x006AB54F
 */
static int window_editor_object_selection_select_object(int flags, rct_object_entry *entry)
{
	return (RCT2_CALLPROC_X(0x006AB54F, 0, flags, 0, 0, 0, 0, (int)entry) & 0x100) == 0;
}

/**
 * Takes the y coordinate of the clicked on scroll list
 * and converts this into an object selection.
 * Returns the position in the list.
 * Object_selection_flags, installed_entry also populated
 *
 * rct2: 0x006AA703
 */
static int get_object_from_object_selection(uint8 object_type, int y, uint8 *object_selection_flags, rct_object_entry **installed_entry)
{
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

/**
 * 
 *  rct2: 0x006D33E2
 */
static void window_editor_object_selection_manage_tracks()
{
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
	uint8* ride_type_array = &ride_entry->ride_type[0];

	int ride_type;
	for (int i = 0; (ride_type = ride_type_array[i]) == 0xFF; i++);
	RCT2_GLOBAL(0xF44158, uint8) = ride_type;

	ride_list_item item = { ride_type, entry_index };
	track_load_list(item);
	window_track_list_open(item);
}

/**
 * 
 *  rct2: 0x006ABBBE
 */
static void editor_load_selected_objects()
{
	uint8 *selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	rct_object_entry *installed_entry = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32) == 0)
		return;

	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i != 0; i--, selection_flags++) {
		if (*selection_flags & 1) {
			uint8 entry_index, entry_type;
			if (!find_object_in_entry_group(installed_entry, &entry_type, &entry_index)){
				int chunk_size;
				if (!object_load(-1, installed_entry, &chunk_size)) {
					log_error("Failed to load entry %.8s", installed_entry->name);
				}
			}
		}

		installed_entry = object_get_next(installed_entry);
	}
}