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
#include "../interface/themes.h"
#include "dropdown.h"
#include "../rct1.h"

enum {
	FILTER_RCT2 = (1 << 0),
	FILTER_WW = (1 << 1),
	FILTER_TT = (1 << 2),
	FILTER_CUSTOM = (1 << 3),

	FILTER_RIDE_TRANSPORT = (1 << 5),
	FILTER_RIDE_GENTLE = (1 << 6),
	FILTER_RIDE_COASTER = (1 << 7),
	FILTER_RIDE_THRILL = (1 << 8),
	FILTER_RIDE_WATER = (1 << 9),
	FILTER_RIDE_STALL = (1 << 10),

	
	FILTER_ALL = 0x7EF,
} FILTER_FLAGS;

uint32 _filter_flags;
uint16 _filter_object_counts[11];
uint8 _filter_ride_tab;

char _filter_string[41];

#define _FILTER_ALL ((_filter_flags & FILTER_ALL) == FILTER_ALL)
#define _FILTER_RCT2 (_filter_flags & FILTER_RCT2)
#define _FILTER_WW (_filter_flags & FILTER_WW)
#define _FILTER_TT (_filter_flags & FILTER_TT)
#define _FILTER_CUSTOM (_filter_flags & FILTER_CUSTOM)

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
	WIDX_ADVANCED,				// 15, 8000
	WIDX_LIST,					// 16, 10000
	WIDX_PREVIEW,				// 17, 20000
	WIDX_INSTALL_TRACK,			// 18, 40000
	WIDX_FILTER_DROPDOWN,		// 19, 80000
	WIDX_FILTER_STRING_BUTTON,	// 20, 100000
	WIDX_FILTER_CLEAR_BUTTON,	// 21, 200000
	WIDX_FILTER_RIDE_TAB_FRAME,
	WIDX_FILTER_RIDE_TAB_ALL,
	WIDX_FILTER_RIDE_TAB_TRANSPORT,
	WIDX_FILTER_RIDE_TAB_GENTLE,
	WIDX_FILTER_RIDE_TAB_COASTER,
	WIDX_FILTER_RIDE_TAB_THRILL,
	WIDX_FILTER_RIDE_TAB_WATER,
	WIDX_FILTER_RIDE_TAB_STALL,
	WIDX_LIST_SORT_TYPE,
	WIDX_LIST_SORT_RIDE,
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
	{ WWT_SCROLL,			1,	4,		291,	60,		386,	2,								STR_NONE },
	{ WWT_FLATBTN,			1,	391,	504,	46,		159,	0xFFFFFFFF,						STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	384,	595,	24,		35,		STR_INSTALL_NEW_TRACK_DESIGN,	STR_INSTALL_NEW_TRACK_DESIGN_TIP },
	{ WWT_DROPDOWN_BUTTON,	0,	350,	463,	23,		34,		5261,							5265 },
	{ WWT_TEXT_BOX,			1,	4,		214,	46,		57,		(uint32)_filter_string,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	218,	287,	46,		57,		5277,							STR_NONE },
	{ WWT_IMGBTN,			1,	3,		287,	73,		76,		0xFFFFFFFF,						STR_NONE },
	{ WWT_TAB,				1,	3,		33,		47,		73,		0x2000144E,						5349 },
	{ WWT_TAB,				1,	34,		64,		47,		73,		0x2000144E,						1223 },
	{ WWT_TAB,				1,	65,		95,		47,		73,		0x2000144E,						1224 },
	{ WWT_TAB,				1,	96,		126,	47,		73,		0x2000144E,						1225 },
	{ WWT_TAB,				1,	127,	157,	47,		73,		0x2000144E,						1226 },
	{ WWT_TAB,				1,	158,	188,	47,		73,		0x2000144E,						1227 },
	{ WWT_TAB,				1,	189,	219,	47,		73,		0x2000144E,						1228 },
	{ WWT_13,				1,	4,		204,	80,		93,		STR_NONE,						STR_NONE },
	{ WWT_13,				1,	205,	291,	80,		93,		STR_NONE,						STR_NONE },
	{ WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_editor_object_selection_close(rct_window *w);
static void window_editor_object_selection_mouseup(rct_window *w, int widgetIndex);
static void window_editor_object_selection_resize(rct_window *w);
static void window_editor_object_selection_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_editor_object_selection_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_editor_object_selection_update(rct_window *w);
static void window_editor_object_selection_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_editor_object_selection_scroll_mousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_editor_object_selection_scroll_mouseover(rct_window *w, int scrollIndex, int x, int y);
static void window_editor_object_selection_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
static void window_editor_object_selection_invalidate(rct_window *w);
static void window_editor_object_selection_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_editor_object_selection_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);
static void window_editor_object_selection_textinput(rct_window *w, int widgetIndex, char *text);

static rct_window_event_list window_editor_object_selection_events = {
	window_editor_object_selection_close,
	window_editor_object_selection_mouseup,
	window_editor_object_selection_resize,
	window_editor_object_selection_mousedown,
	window_editor_object_selection_dropdown,
	NULL,
	window_editor_object_selection_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_editor_object_selection_scrollgetsize,
	window_editor_object_selection_scroll_mousedown,
	NULL,
	window_editor_object_selection_scroll_mouseover,
	window_editor_object_selection_textinput,
	NULL,
	NULL,
	window_editor_object_selection_tooltip,
	NULL,
	NULL,
	window_editor_object_selection_invalidate,
	window_editor_object_selection_paint,
	window_editor_object_selection_scrollpaint
};

#pragma endregion

const int window_editor_object_selection_animation_loops[] = { 20, 32, 10, 72, 24, 28, 16 };
const int window_editor_object_selection_animation_divisor[] = { 4, 8, 2, 4, 4, 4, 2 };

static void window_editor_object_set_page(rct_window *w, int page);
static void window_editor_object_selection_set_pressed_tab(rct_window *w);
static void window_editor_object_selection_select_default_objects();
static void window_editor_object_selection_select_required_objects();
static int window_editor_object_selection_select_object(uint8 bh, int flags, rct_object_entry *entry);
static int get_object_from_object_selection(uint8 object_type, int y, uint8 *object_selection_flags, rct_object_entry **installed_entry);
static void window_editor_object_selection_manage_tracks();
static void editor_load_selected_objects();
static bool filter_string(rct_object_entry *entry);
static bool filter_source(rct_object_entry *entry);
static bool filter_chunks(rct_object_entry *entry, rct_object_filters *filter);
static void filter_update_counts();

void reset_selected_object_count_and_size();
void reset_required_object_flags();
static int sub_6AB211();

static rct_object_entry RequiredSelectedObjects[] = {
	// Objects that are always required
	{ 0x00000087, { "SCGTREES" }, 0 },		// Scenery: Trees
	{ 0x00000087, { "SCGSHRUB" }, 0 },		// Scenery: Shrubs and Ornaments
	{ 0x00000087, { "SCGGARDN" }, 0 },		// Scenery: Gardens
	{ 0x00000087, { "SCGFENCE" }, 0 },		// Scenery: Fences and Walls
	{ 0x00000087, { "SCGWALLS" }, 0 },		// Scenery: Walls and Roofs
	{ 0x00000087, { "SCGPATHX" }, 0 },		// Scenery: Signs and Items for Footpaths
	{ 0x00000085, { "TARMAC  " }, 0 },		// Footpath: Tarmac
};

static rct_object_entry DefaultSelectedObjects[] = {
	// An initial default selection
	{ 0x00000080, { "TWIST1  " }, 0 },		// Ride: Twist
	{ 0x00000080, { "PTCT1   " }, 0 },		// Ride: Wooden Roller Coaster (Wooden Roller Coaster Trains)
	{ 0x00000080, { "ZLDB    " }, 0 },		// Ride: Junior Roller Coaster (Ladybird Trains)
	{ 0x00000080, { "LFB1    " }, 0 },		// Ride: Log Flume
	{ 0x00000080, { "VCR     " }, 0 },		// Ride: Vintage Cars
	{ 0x00000080, { "MGR1    " }, 0 },		// Ride: Merry-Go-Round
	{ 0x00000080, { "TLT1    " }, 0 },		// Ride: Restroom
	{ 0x00000080, { "ATM1    " }, 0 },		// Ride: Cash Machine
	{ 0x00000080, { "FAID1   " }, 0 },		// Ride: First Aid Room
	{ 0x00000080, { "INFOK   " }, 0 },		// Ride: Information Kiosk
	{ 0x00000080, { "DRNKS   " }, 0 },		// Ride: Drinks Stall
	{ 0x00000080, { "CNDYF   " }, 0 },		// Ride: Cotten Candy Stall
	{ 0x00000080, { "BURGB   " }, 0 },		// Ride: Burger Bar
	{ 0x00000080, { "BALLN   " }, 0 },		// Ride: Balloon Stall
	{ 0x00000080, { "ARRT1   " }, 0 },		// Ride: Corkscrew Roller Coaster
	{ 0x00000080, { "RBOAT   " }, 0 },		// Ride: Rowing Boats
	{ 0x00000088, { "PKENT1  " }, 0 },		// Park Entrace: Traditional Park Entrance
	{ 0x00000089, { "WTRCYAN " }, 0 },		// Water: Natural Water
	{ 0x00000085, { "TARMACB " }, 0 },		// Footpath: Brown Tarmac Footpath
	{ 0x00000085, { "PATHSPCE" }, 0 },		// Footpath: Space Style Footpath
	{ 0x00000085, { "PATHDIRT" }, 0 },		// Footpath: Dirt Footpath
	{ 0x00000085, { "PATHCRZY" }, 0 },		// Footpath: Crazy Paving Footpath
	{ 0x00000085, { "PATHASH " }, 0 },		// Footpath: Ash Footpath

	// The following are for all random map generation features to work out the box
	{ 0x00000087, { "SCGJUNGL" }, 0 },		// Jungle Themeing
	{ 0x00000087, { "SCGSNOW " }, 0 },		// Snow and Ice Themeing
	{ 0x00000087, { "SCGWATER" }, 0 }		// Water Feature Themeing
};

enum {
	RIDE_SORT_TYPE,
	RIDE_SORT_RIDE
};

typedef struct {
	rct_object_entry *entry;
	rct_object_filters *filter;
	uint8 *flags;
} list_item;

typedef int (*sortFunc)(const void *, const void *);

static int _numListItems = 0;
static list_item *_listItems = NULL;
static int _listSortType = RIDE_SORT_TYPE;
static bool _listSortDescending = false;

static void visible_list_dispose()
{
	SafeFree(_listItems);
	_numListItems = 0;
}

static int visible_list_sort_ride_name(const void *rawA, const void *rawB)
{
	list_item *a = (list_item*)rawA;
	list_item *b = (list_item*)rawB;

	const char *nameA = object_get_name(a->entry);
	const char *nameB = object_get_name(b->entry);
	return strcmp(nameA, nameB);
}

static int visible_list_sort_ride_type(const void *rawA, const void *rawB)
{
	list_item *a = (list_item*)rawA;
	list_item *b = (list_item*)rawB;

	const char *rideTypeA = language_get_string(2 + a->filter->ride.ride_type);
	const char *rideTypeB = language_get_string(2 + b->filter->ride.ride_type);
	int result = strcmp(rideTypeA, rideTypeB);
	if (result != 0)
		return result;

	return visible_list_sort_ride_name(rawA, rawB);
}

static void visible_list_refresh(rct_window *w)
{
	int numObjects = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, sint32);

	visible_list_dispose();
	_listItems = malloc(numObjects * sizeof(list_item));
	_numListItems = 0;

	list_item *currentListItem = &_listItems[0];
	rct_object_entry *entry = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
	uint8 *itemFlags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	for (int i = 0; i < numObjects; i++) {
		rct_object_filters *filter = get_object_filter(i);
		int type = entry->flags & 0x0F;
		int source = (entry->flags & 0xF0) >> 4;
		if (type == w->selected_tab && !(*itemFlags & OBJECT_SELECTION_FLAG_6) && filter_source(entry) && filter_string(entry) && filter_chunks(entry, filter)) {
			currentListItem->entry = entry;
			currentListItem->filter = filter;
			currentListItem->flags = itemFlags;
			currentListItem++;
			_numListItems++;
		}

		entry = object_get_next(entry);
		itemFlags++;
	}

	_listItems = realloc(_listItems, _numListItems * sizeof(list_item));

	sortFunc sortFunc = NULL;
	switch (_listSortType) {
	case RIDE_SORT_TYPE:
		sortFunc = visible_list_sort_ride_type;
		break;
	case RIDE_SORT_RIDE:
		sortFunc = visible_list_sort_ride_name;
		break;
	}
	qsort(_listItems, _numListItems, sizeof(list_item), sortFunc);

	if (_listSortDescending) {
		for (int i = 0; i < _numListItems / 2; i++) {
			int ri = _numListItems - i - 1;
			list_item temp = _listItems[i];
			_listItems[i] = _listItems[ri];
			_listItems[ri] = temp;
		}
	}

	window_invalidate(w);
}

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

	if (!sub_6AB211())
		return;
	reset_selected_object_count_and_size();

	window = window_create_centred(
		600,
		400,
		&window_editor_object_selection_events,
		WC_EDITOR_OBJECT_SELECTION,
		WF_10 | WF_RESIZABLE
	);
	window->widgets = window_editor_object_selection_widgets;

	window->enabled_widgets =
		(1 << WIDX_ADVANCED) |
		(1 << WIDX_INSTALL_TRACK) |
		(1 << WIDX_FILTER_DROPDOWN) |
		(1 << WIDX_FILTER_STRING_BUTTON) |
		(1 << WIDX_FILTER_CLEAR_BUTTON) |
		(1 << WIDX_CLOSE) |
		(1 << WIDX_LIST_SORT_TYPE) |
		(1 << WIDX_LIST_SORT_RIDE);

	_filter_flags = gConfigInterface.object_selection_filter_flags;
	memset(_filter_string, 0, sizeof(_filter_string));

	for (int i = WIDX_TAB_1; i <= WIDX_TAB_11; i++)
		window->enabled_widgets |= (1LL << i);
	window_init_scroll_widgets(window);

	window->var_4AE = 0;
	window->selected_tab = 0;
	window->selected_list_item = -1;
	window->var_494 = 0xFFFFFFFF;
	window->min_width = 600;
	window->min_height = 400;
	window->max_width = 1200;
	window->max_height = 1000;

	visible_list_refresh(window);
}

/* rct2: 0x006ABCD1 */
static void setup_track_manager_objects(){
	uint8 ride_list[128] = { 0 };
	uint8* selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	rct_object_entry* installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
	uint16 num_objects = 0;

	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i){
		uint8 object_type = installedObject->flags & 0xF;
		if (object_type == OBJECT_TYPE_RIDE){
			*selection_flags |= OBJECT_SELECTION_FLAG_6;

			uint8* pos = (uint8*)installedObject;
			// Skip sizeof(rct_object_entry)
			pos += 16;

			// Skip filename
			while (*pos++);

			// Skip no of images
			pos += 4;

			// Skip name
			while (*pos++);

			// Skip size of chunk
			pos += 4;

			// Skip required objects
			pos += *pos * 16 + 1;

			// Skip theme objects
			pos += *pos * 16 + 1;

			for (uint8 j = 0; j < 3; j++){
				uint8 ride_type = pos[j];
				if (ride_type == 0xFF)
					continue;

				if (!(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride_type * 2] &
					RIDE_TYPE_FLAG_HAS_TRACK))
					continue;

				if (pos[3] & (1 << 0)){
					*selection_flags &= ~OBJECT_SELECTION_FLAG_6;
				}
				else if (ride_list[ride_type] & (1 << 0)){
					continue;
				}
				else{
					ride_list[ride_type] |= (1 << 0);
					*selection_flags &= ~OBJECT_SELECTION_FLAG_6;
				}
				num_objects++;
				break;
			}
		}

		installedObject = object_get_next(installedObject);
		selection_flags++;
	}

	RCT2_GLOBAL(0x00F43412, uint16) = num_objects;
}

/* rct2: 0x006ABC1E */
static void setup_track_designer_objects(){
	uint8* selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	rct_object_entry* installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
	uint16 num_objects = 0;

	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i){
		uint8 object_type = installedObject->flags & 0xF;
		if (object_type == OBJECT_TYPE_RIDE){
			*selection_flags |= OBJECT_SELECTION_FLAG_6;

			uint8* pos = (uint8*)installedObject;
			// Skip sizeof(rct_object_entry)
			pos += 16;

			// Skip filename
			while (*pos++);

			// Skip no of images
			pos += 4;

			// Skip name
			while (*pos++);

			// Skip size of chunk
			pos += 4;

			// Skip required objects
			pos += *pos * 16 + 1;

			// Skip theme objects
			pos += *pos * 16 + 1;

			for (uint8 j = 0; j < 3; j++){
				uint8 ride_type = pos[j];
				if (ride_type == 0xFF)
					continue;

				if (!(RCT2_ADDRESS(0x0097D4F2, uint16)[ride_type * 4] &
					(1 << 11)))
					continue;

				*selection_flags &= ~OBJECT_SELECTION_FLAG_6;
				num_objects++;
				break;
			}
		}

		installedObject = object_get_next(installedObject);
		selection_flags++;
	}

	RCT2_GLOBAL(0x00F43412, uint16) = num_objects;
}

/* rct2: 0x006AA82B */
static void setup_in_use_selection_flags(){

	for (uint8 object_type = 0; object_type < 11; object_type++){
		for (uint16 i = 0; i < object_entry_group_counts[object_type]; i++){
			RCT2_ADDRESS(0x0098DA38, uint8*)[object_type][i] = 0;
		}
	}

	for (uint8 object_type = 0; object_type < 11; object_type++){
		for (uint16 i = 0; i < object_entry_group_counts[object_type]; i++){
			if (object_entry_groups[object_type].chunks[i] != (uint8*)0xFFFFFFFF){
				RCT2_ADDRESS(0x0098DA38, uint8*)[object_type][i] |= (1 << 1);
			}
		}
	}

	map_element_iterator iter;
	map_element_iterator_begin(&iter);
	do {
		uint16 type;
		uint8 path_additions;
		rct_banner* banner;

		switch (map_element_get_type(iter.element)) {
		default:
		case MAP_ELEMENT_TYPE_SURFACE:
		case MAP_ELEMENT_TYPE_TRACK:
			break;
		case MAP_ELEMENT_TYPE_PATH:
			type = iter.element->properties.path.type;
			RCT2_ADDRESS(0x0098DA38, uint8*)[OBJECT_TYPE_PATHS][type] |= (1 << 0);

			path_additions = iter.element->properties.path.additions & 0xF;
			if (path_additions){
				path_additions--;
				RCT2_ADDRESS(0x0098DA38, uint8*)[OBJECT_TYPE_PATH_BITS][path_additions] |= (1 << 0);
			}
			break;
		case MAP_ELEMENT_TYPE_SCENERY:
			type = iter.element->properties.scenery.type;
			RCT2_ADDRESS(0x0098DA38, uint8*)[OBJECT_TYPE_SMALL_SCENERY][type] |= (1 << 0);
			break;
		case MAP_ELEMENT_TYPE_ENTRANCE:
			if (iter.element->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE)
				break;

			RCT2_ADDRESS(0x0098DA38, uint8*)[OBJECT_TYPE_PARK_ENTRANCE][0] |= (1 << 0);

			type = iter.element->properties.entrance.path_type;
			RCT2_ADDRESS(0x0098DA38, uint8*)[OBJECT_TYPE_PATHS][type] |= (1 << 0);
			break;
		case MAP_ELEMENT_TYPE_FENCE:
			type = iter.element->properties.fence.type;
			RCT2_ADDRESS(0x0098DA38, uint8*)[OBJECT_TYPE_WALLS][type] |= (1 << 0);
			break;
		case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
			type = iter.element->properties.scenerymultiple.type & 0x3FF;
			RCT2_ADDRESS(0x0098DA38, uint8*)[OBJECT_TYPE_LARGE_SCENERY][type] |= (1 << 0);
			break;
		case MAP_ELEMENT_TYPE_BANNER:
			banner = &gBanners[iter.element->properties.banner.index];
			type = banner->type;
			RCT2_ADDRESS(0x0098DA38, uint8*)[OBJECT_TYPE_BANNERS][type] |= (1 << 0);
			break;
		}
	} while (map_element_iterator_next(&iter));

	for (uint8 ride_index = 0; ride_index < 0xFF; ride_index++){
		rct_ride* ride = GET_RIDE(ride_index);
		if (ride->type == RIDE_TYPE_NULL)
			continue;

		uint8 type = ride->subtype;
		RCT2_ADDRESS(0x0098DA38, uint8*)[OBJECT_TYPE_RIDE][type] |= (1 << 0);
	}

	uint8* selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	rct_object_entry* installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i){
		*selection_flags &= ~OBJECT_SELECTION_FLAG_IN_USE;

		uint8 entry_type, entry_index;
		if (find_object_in_entry_group(installedObject, &entry_type, &entry_index)){
			if (RCT2_ADDRESS(0x0098DA38, uint8*)[entry_type][entry_index] & (1 << 0)){
				*selection_flags |= 
					OBJECT_SELECTION_FLAG_IN_USE | 
					OBJECT_SELECTION_FLAG_SELECTED;
			}
			if (RCT2_ADDRESS(0x0098DA38, uint8*)[entry_type][entry_index] & (1 << 1)){
				*selection_flags |= OBJECT_SELECTION_FLAG_SELECTED;
			}
		}
		installedObject = object_get_next(installedObject);
		selection_flags++;
	}
}

/* rct2: 0x006AB211 */
static int sub_6AB211(){
	uint32 total_objects = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32);

	RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*) = rct2_malloc(total_objects);

	if (RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*) == NULL){
		log_error("Failed to allocate memory for object flag list.");
		return 0;
	}

	memset(RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*), 0, total_objects);
	for (uint8 object_type = 0; object_type < 11; object_type++){
		RCT2_ADDRESS(0x00F433F7, uint16)[object_type] = 0;
		RCT2_ADDRESS(0x00F433E1, uint16)[object_type] = 0;
	}

	rct_object_entry* installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i){
		uint8 object_type = installedObject->flags & 0xF;
		RCT2_ADDRESS(0x00F433E1, uint16)[object_type]++;

		installedObject = object_get_next(installedObject);
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER){
		setup_track_designer_objects();
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER){
		setup_track_manager_objects();
	}

	setup_in_use_selection_flags();
	reset_selected_object_count_and_size();

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))){
		window_editor_object_selection_select_required_objects();

		// To prevent it breaking in scenario mode.
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR)
			window_editor_object_selection_select_default_objects();
	}

	reset_required_object_flags();
	reset_selected_object_count_and_size();
	return 1;
}

/* rct2: 0x006AB316 */
static void editor_object_flags_free(){
	if (RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*) == NULL){
		return;
	}
	rct2_free(RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*));
	RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*) = NULL;
}

/* rct2: 0x00685791 */
void remove_selected_objects_from_research(rct_object_entry* installedObject){
	uint8 entry_type, entry_index;
	if (!find_object_in_entry_group(installedObject, &entry_type, &entry_index))
		return;

	if (entry_type == OBJECT_TYPE_RIDE){
		rct_ride_type* rideEntry = (rct_ride_type*)object_entry_groups[entry_type].chunks[entry_index];
		research_remove(entry_index | rideEntry->ride_type[0] << 8 | 0x10000);
		research_remove(entry_index | rideEntry->ride_type[1] << 8 | 0x10000);
		research_remove(entry_index | rideEntry->ride_type[2] << 8 | 0x10000);
	}
	else if (entry_type == OBJECT_TYPE_SCENERY_SETS){
		research_remove(entry_index);
	}
}

/* rct2: 0x006ABB66 */
void unload_unselected_objects(){
	uint8* selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	rct_object_entry* installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i){
		if (!(*selection_flags & OBJECT_SELECTION_FLAG_SELECTED)){
			remove_selected_objects_from_research(installedObject);
			object_unload(installedObject);
		}
		selection_flags++;
		installedObject = object_get_next(installedObject);
	}
}

/**
 * 
 *  rct2: 0x006AB199
 */
static void window_editor_object_selection_close(rct_window *w)
{
	//if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_EDITOR))
	//	return;

	unload_unselected_objects();
	editor_load_selected_objects();
	reset_loaded_objects();
	object_free_scenario_text();
	editor_object_flags_free();

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_EDITOR) {
		research_populate_list_random();
		research_remove_non_separate_vehicle_types();
	}
	else {
		// Used for in-game object selection cheat
		research_reset_items();
		research_populate_list_researched();
		gSilentResearch = true;
		sub_684AC3();
		gSilentResearch = false;
	}
	window_new_ride_init_vars();

	visible_list_dispose();
}

/**
 * 
 *  rct2: 0x006AAFAB
 */
static void window_editor_object_selection_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_EDITOR) {
			game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 1, 0);
		}
		else {
			// Used for in-game object selection cheat
			window_close(w);
		}
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
	case WIDX_FILTER_RIDE_TAB_ALL:
		_filter_flags |= 0x7E0;
		gConfigInterface.object_selection_filter_flags = _filter_flags;
		config_save_default();

		filter_update_counts();
		visible_list_refresh(w);

		w->selected_list_item = -1;
		w->var_494 = 0xFFFFFFFF;
		w->scrolls[0].v_top = 0;
		object_free_scenario_text();
		window_invalidate(w);
		break;
	case WIDX_FILTER_RIDE_TAB_TRANSPORT:
	case WIDX_FILTER_RIDE_TAB_GENTLE:
	case WIDX_FILTER_RIDE_TAB_COASTER:
	case WIDX_FILTER_RIDE_TAB_THRILL:
	case WIDX_FILTER_RIDE_TAB_WATER:
	case WIDX_FILTER_RIDE_TAB_STALL:
		_filter_flags &= ~0x7E0;
		_filter_flags |= (1 << (widgetIndex - WIDX_FILTER_RIDE_TAB_TRANSPORT + 5));
		gConfigInterface.object_selection_filter_flags = _filter_flags;
		config_save_default();

		filter_update_counts();
		visible_list_refresh(w);

		w->selected_list_item = -1;
		w->var_494 = 0xFFFFFFFF;
		w->scrolls[0].v_top = 0;
		object_free_scenario_text();
		window_invalidate(w);
		break;

	case WIDX_ADVANCED:
		w->list_information_type ^= 1;
		window_invalidate(w);
		break;

	case WIDX_INSTALL_TRACK:
		if (w->selected_list_item != -1) {
			w->selected_list_item = -1;
			object_free_scenario_text();
		}
		window_invalidate(w);

		window_loadsave_open(LOADSAVETYPE_LOAD | LOADSAVETYPE_TRACK, NULL);
		break;
	case WIDX_FILTER_STRING_BUTTON:
		//window_text_input_open(w, widgetIndex, 5275, 5276, 1170, (uint32)_filter_string, 40);
		window_start_textbox(w, widgetIndex, 1170, (uint32)_filter_string, 40);
		break;
	case WIDX_FILTER_CLEAR_BUTTON:
		memset(_filter_string, 0, sizeof(_filter_string));

		w->scrolls->v_top = 0;

		window_invalidate(w);
		break;
	case WIDX_LIST_SORT_TYPE:
		if (_listSortType == RIDE_SORT_TYPE) {
			_listSortDescending = !_listSortDescending;
		} else {
			_listSortType = RIDE_SORT_TYPE;
			_listSortDescending = false;
		}
		visible_list_refresh(w);
		break;
	case WIDX_LIST_SORT_RIDE:
		if (_listSortType == RIDE_SORT_RIDE) {
			_listSortDescending = !_listSortDescending;
		} else {
			_listSortType = RIDE_SORT_RIDE;
			_listSortDescending = false;
		}
		visible_list_refresh(w);
		break;
	}
}

static void window_editor_object_selection_resize(rct_window *w)
{
	window_set_resize(w, 600, 400, 1200, 1000);
}

void window_editor_object_selection_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	int num_items;

	//widget = &w->widgets[widgetIndex - 1];

	switch (widgetIndex) {
	case WIDX_FILTER_DROPDOWN:

		num_items = 4;
		gDropdownItemsFormat[0] = 1156;
		gDropdownItemsFormat[1] = 1156;
		gDropdownItemsFormat[2] = 1156;
		gDropdownItemsFormat[3] = 1156;
		gDropdownItemsArgs[0] = 2741;
		gDropdownItemsArgs[1] = 5262;
		gDropdownItemsArgs[2] = 5263;
		gDropdownItemsArgs[3] = 5264;

		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[widget->colour],
			DROPDOWN_FLAG_STAY_OPEN,
			num_items
			);

		gDropdownItemsChecked = _filter_flags & 0xF;
		break;
		
	}
}

static void window_editor_object_selection_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	if (dropdownIndex == -1)
		return;

	switch (widgetIndex) {
	case WIDX_FILTER_DROPDOWN:
		_filter_flags ^= (1 << dropdownIndex);
		gConfigInterface.object_selection_filter_flags = _filter_flags;
		config_save_default();

		filter_update_counts();
		w->scrolls->v_top = 0;

		visible_list_refresh(w);
		window_invalidate(w);
		break;
	}
}

/**
 * 
 *  rct2: 0x006AB031
 */
static void window_editor_object_selection_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	*height = _numListItems * 12;
}

/**
 * 
 *  rct2: 0x006AB0B6
 */
static void window_editor_object_selection_scroll_mousedown(rct_window *w, int scrollIndex, int x, int y)
{
	// Used for in-game object selection cheat to prevent crashing the game
	// when windows attempt to draw objects that don't exist any more
	window_close_all_except_class(WC_EDITOR_OBJECT_SELECTION);

	uint8 object_selection_flags;
	rct_object_entry* installed_entry;
	int selected_object = get_object_from_object_selection((w->selected_tab & 0xFF), y, &object_selection_flags, &installed_entry);
	if (selected_object == -1 || (object_selection_flags & OBJECT_SELECTION_FLAG_6))
		return;

	window_invalidate(w);

	sound_play_panned(SOUND_CLICK_1, RCT2_GLOBAL(0x142406C,uint32), 0, 0, 0);


	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
		if (!window_editor_object_selection_select_object(0, 1, installed_entry))
			return;

		// Close any other open windows such as options/colour schemes to prevent a crash.
		window_close_all();
		//window_close(w);

		//This function calls window_track_list_open
		window_editor_object_selection_manage_tracks();
		return;
	}

	int ebx = 6;
	// If already selected
	if (!(object_selection_flags & OBJECT_SELECTION_FLAG_SELECTED))
		ebx = 7;

	RCT2_GLOBAL(0xF43411, uint8) = 0;
	if (!window_editor_object_selection_select_object(0, ebx, installed_entry)) {
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
static void window_editor_object_selection_scroll_mouseover(rct_window *w, int scrollIndex, int x, int y)
{
	rct_object_entry *installedEntry;
	int selectedObject;
	uint8 objectSelectionFlags;

	selectedObject = get_object_from_object_selection(
		w->selected_tab & 0xFF, y, &objectSelectionFlags, &installedEntry
	);
	if (objectSelectionFlags & OBJECT_SELECTION_FLAG_6)
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
static void window_editor_object_selection_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
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
static void window_editor_object_selection_invalidate(rct_window *w)
{
	int i, x;
	rct_widget *widget;

	colour_scheme_update(w);

	// Resize widgets
	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;
	w->widgets[WIDX_TAB_CONTENT_PANEL].right = w->width - 1;
	w->widgets[WIDX_TAB_CONTENT_PANEL].bottom = w->height - 1;
	w->widgets[WIDX_ADVANCED].left = w->width - 130;
	w->widgets[WIDX_ADVANCED].right = w->width - 9;
	w->widgets[WIDX_LIST].right = w->width - 309;
	w->widgets[WIDX_LIST].bottom = w->height - 14;
	w->widgets[WIDX_PREVIEW].left = w->width - 209;
	w->widgets[WIDX_PREVIEW].right = w->width - 96;
	w->widgets[WIDX_INSTALL_TRACK].left = w->width - 216;
	w->widgets[WIDX_INSTALL_TRACK].right = w->width - 130;
	w->widgets[WIDX_FILTER_DROPDOWN].left = w->width - 250;
	w->widgets[WIDX_FILTER_DROPDOWN].right = w->width - 137;

	// Set pressed widgets
	w->pressed_widgets |= 1 << WIDX_PREVIEW;
	window_editor_object_selection_set_pressed_tab(w);
	if (w->list_information_type & 1)
		w->pressed_widgets |= (1 << WIDX_ADVANCED);
	else
		w->pressed_widgets &= ~(1 << WIDX_ADVANCED);

	// Set window title and buttons
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, rct_string_id) = STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS + w->selected_tab;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
		w->widgets[WIDX_TITLE].image = STR_TRACK_DESIGNS_MANAGER_SELECT_RIDE_TYPE;
		w->widgets[WIDX_CLOSE].type = WWT_EMPTY;
		w->widgets[WIDX_INSTALL_TRACK].type = WWT_DROPDOWN_BUTTON;
	} else if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER) {
		w->widgets[WIDX_TITLE].image = STR_ROLLER_COASTER_DESIGNER_SELECT_RIDE_TYPES_VEHICLES;
		w->widgets[WIDX_CLOSE].type = WWT_CLOSEBOX;
		w->widgets[WIDX_INSTALL_TRACK].type = WWT_EMPTY;
	} else {
		w->widgets[WIDX_TITLE].image = STR_OBJECT_SELECTION;
		w->widgets[WIDX_CLOSE].type = WWT_CLOSEBOX;
		w->widgets[WIDX_INSTALL_TRACK].type = WWT_EMPTY;
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
		w->widgets[WIDX_ADVANCED].type = WWT_EMPTY;
		w->widgets[WIDX_FILTER_DROPDOWN].type = WWT_EMPTY;
		for (i = 1; i < WINDOW_OBJECT_SELECTION_PAGE_COUNT; i++)
			w->widgets[WIDX_TAB_1 + i].type = WWT_EMPTY;
		x = 150;
	} else {
		w->widgets[WIDX_ADVANCED].type = WWT_DROPDOWN_BUTTON;
		w->widgets[WIDX_FILTER_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		x = 300;
	}

	w->widgets[WIDX_LIST].right = w->width - (600 - 587) - x;
	w->widgets[WIDX_PREVIEW].left = w->width - (600 - 537) - (x / 2);
	w->widgets[WIDX_PREVIEW].right = w->widgets[WIDX_PREVIEW].left + 113;
	w->widgets[WIDX_FILTER_RIDE_TAB_FRAME].right = w->widgets[WIDX_LIST].right;

	bool ridePage = (w->selected_tab == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS);
	w->widgets[WIDX_LIST].top = (ridePage ? 118 : 60);
	w->widgets[WIDX_FILTER_STRING_BUTTON].right = w->widgets[WIDX_LIST].right - 77;
	w->widgets[WIDX_FILTER_STRING_BUTTON].top = (ridePage ? 80 : 46);
	w->widgets[WIDX_FILTER_STRING_BUTTON].bottom = (ridePage ? 91 : 57);
	w->widgets[WIDX_FILTER_CLEAR_BUTTON].left = w->widgets[WIDX_LIST].right - 73;
	w->widgets[WIDX_FILTER_CLEAR_BUTTON].right = w->widgets[WIDX_LIST].right;
	w->widgets[WIDX_FILTER_CLEAR_BUTTON].top = (ridePage ? 80 : 46);
	w->widgets[WIDX_FILTER_CLEAR_BUTTON].bottom = (ridePage ? 91 : 57);

	if (ridePage) {
		w->enabled_widgets |= (1 << WIDX_FILTER_RIDE_TAB_ALL) | (1 << WIDX_FILTER_RIDE_TAB_TRANSPORT) |
			(1 << WIDX_FILTER_RIDE_TAB_GENTLE) | (1 << WIDX_FILTER_RIDE_TAB_COASTER) | (1 << WIDX_FILTER_RIDE_TAB_THRILL) |
			(1 << WIDX_FILTER_RIDE_TAB_WATER) | (1 << WIDX_FILTER_RIDE_TAB_STALL);
		for (i = 0; i < 7; i++)
			w->pressed_widgets &= ~(1 << (WIDX_FILTER_RIDE_TAB_ALL + i));
		if ((_filter_flags & 0x7E0) == 0x7E0)
			w->pressed_widgets |= (1 << WIDX_FILTER_RIDE_TAB_ALL);
		else {
			for (int i = 0; i < 6; i++) {
				if (_filter_flags & (1 << (5 + i)))
					w->pressed_widgets |= (uint64)(1 << (WIDX_FILTER_RIDE_TAB_TRANSPORT + i));
			}
		}
		w->widgets[WIDX_FILTER_RIDE_TAB_FRAME].type = WWT_IMGBTN;
		for (int i = WIDX_FILTER_RIDE_TAB_ALL; i <= WIDX_FILTER_RIDE_TAB_STALL; i++)
			w->widgets[i].type = WWT_TAB;

		w->widgets[WIDX_LIST_SORT_TYPE].type = WWT_13;
		w->widgets[WIDX_LIST_SORT_TYPE].top = w->widgets[WIDX_FILTER_STRING_BUTTON].bottom + 3;
		w->widgets[WIDX_LIST_SORT_TYPE].bottom = w->widgets[WIDX_LIST_SORT_TYPE].top + 13;
		w->widgets[WIDX_LIST_SORT_RIDE].type = WWT_13;
		w->widgets[WIDX_LIST_SORT_RIDE].top = w->widgets[WIDX_LIST_SORT_TYPE].top;
		w->widgets[WIDX_LIST_SORT_RIDE].bottom = w->widgets[WIDX_LIST_SORT_TYPE].bottom;
		w->widgets[WIDX_LIST_SORT_RIDE].right = w->widgets[WIDX_LIST].right;
		w->widgets[WIDX_LIST].top = w->widgets[WIDX_LIST_SORT_TYPE].bottom + 2;
	} else {
		w->enabled_widgets &= ~((1 << WIDX_FILTER_RIDE_TAB_ALL) | (1 << WIDX_FILTER_RIDE_TAB_TRANSPORT) |
			(1 << WIDX_FILTER_RIDE_TAB_GENTLE) | (1 << WIDX_FILTER_RIDE_TAB_COASTER) | (1 << WIDX_FILTER_RIDE_TAB_THRILL) |
			(1 << WIDX_FILTER_RIDE_TAB_WATER) | (1 << WIDX_FILTER_RIDE_TAB_STALL));
		for (int i = WIDX_FILTER_RIDE_TAB_FRAME; i <= WIDX_FILTER_RIDE_TAB_STALL; i++)
			w->widgets[i].type = WWT_EMPTY;
		
		w->widgets[WIDX_LIST_SORT_TYPE].type = WWT_EMPTY;
		w->widgets[WIDX_LIST_SORT_RIDE].type = WWT_EMPTY;
	}
}

/**
 * 
 *  rct2: 0x006AAB56
 */
static void window_editor_object_selection_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	int i, x, y, width, numSelected, totalSelectable, type;
	rct_widget *widget;
	rct_object_entry *highlightedEntry;
	rct_string_id stringId;
	uint8 *text, source;
	char *datName, *name, *stringBuffer;

	/*if (w->selected_tab == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS) {
		gfx_fill_rect_inset(dpi,
			w->x + w->widgets[WIDX_FILTER_RIDE_TAB_ALL].left - 1,
			w->y + w->widgets[WIDX_FILTER_RIDE_TAB_ALL].bottom,
			w->x + w->widgets[WIDX_FILTER_RIDE_TAB_STALL].right + 1,
			w->y + w->widgets[WIDX_FILTER_RIDE_TAB_ALL].bottom + 2,
			w->colours[1],
			0x10
			);
	}*/

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

	const int ride_tabs[] = { 5458, 0x200015A1, 5542, 0x200015AA, 5557, 5551, 5530, 5327 };
	const int ThrillRidesTabAnimationSequence[] = {
		5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0
	};

	// Draw ride tabs
	if (w->selected_tab == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS) {
		for (i = 0; i < 7; i++) {
			widget = &w->widgets[WIDX_FILTER_RIDE_TAB_ALL + i];
			if (widget->type == WWT_EMPTY)
				continue;

			int spriteIndex = ride_tabs[i];
			int frame = 0;
			if (i != 0 && w->pressed_widgets & (1ULL << (WIDX_FILTER_RIDE_TAB_ALL + i))) {
				frame = w->frame_no / window_editor_object_selection_animation_divisor[i - 1];
			}
			spriteIndex += (i == 4 ? ThrillRidesTabAnimationSequence[frame] : frame);

			x = w->x + widget->left;
			y = w->y + widget->top;
			gfx_draw_sprite(dpi, spriteIndex | (w->colours[1] << 19), x, y, 0);
		}

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
		y = w->y + w->height - 13;

		numSelected = RCT2_ADDRESS(0x00F433F7, uint16)[w->selected_tab];
		totalSelectable = object_entry_group_counts[w->selected_tab];
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)
			totalSelectable = 4;

		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = numSelected;
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint16) = totalSelectable;
		gfx_draw_string_left(dpi, 3164, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0, x, y);
	}

	rct_stex_entry* stex_entry = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TEXT_TEMP_CHUNK, rct_stex_entry*);

	/*gfx_fill_rect_inset(dpi,
		w->x + window_editor_object_selection_widgets[WIDX_FILTER_STRING_BUTTON].left,
		w->y + window_editor_object_selection_widgets[WIDX_FILTER_STRING_BUTTON].top,
		w->x + window_editor_object_selection_widgets[WIDX_FILTER_STRING_BUTTON].right,
		w->y + window_editor_object_selection_widgets[WIDX_FILTER_STRING_BUTTON].bottom,
		w->colours[1],
		0x30
		);

	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = (uint32)&_filter_string;
	gfx_draw_string_left_clipped(
		dpi,
		1170,
		(void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS,
		w->colours[1],
		w->x + window_editor_object_selection_widgets[WIDX_FILTER_STRING_BUTTON].left + 1,
		w->y + window_editor_object_selection_widgets[WIDX_FILTER_STRING_BUTTON].top,
		w->x + window_editor_object_selection_widgets[WIDX_FILTER_STRING_BUTTON].right
		);*/

	// Draw sort button text
	widget = &w->widgets[WIDX_LIST_SORT_TYPE];
	if (widget->type != WWT_EMPTY) {
		stringId = _listSortType == RIDE_SORT_TYPE ? (_listSortDescending ? STR_DOWN : STR_UP) : STR_NONE;
		gfx_draw_string_left_clipped(dpi, STR_OBJECTS_SORT_TYPE, &stringId, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}
	widget = &w->widgets[WIDX_LIST_SORT_RIDE];
	if (widget->type != WWT_EMPTY) {
		stringId = _listSortType == RIDE_SORT_RIDE ? (_listSortDescending ? STR_DOWN : STR_UP) : STR_NONE;
		gfx_draw_string_left_clipped(dpi, STR_OBJECTS_SORT_RIDE, &stringId, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}

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
		text += *text * 16 + 1;
		text += *text * 16 + 1;

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

	// Draw object source
	source = (highlightedEntry->flags & 0xF0) >> 4;
	switch (source) {
	case 8: stringId = 2741; break;
	case 1: stringId = 5262; break;
	case 2: stringId = 5263; break;
	default: stringId = 5264; break;
	}
	gfx_draw_string_right(dpi, stringId, NULL, 2, w->x + w->width - 5, w->y + w->height - 3 - 12 - 14);

	// 
	if (w->selected_tab == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS) {
		y = w->y + w->height - 3 - 12 - 14 - 14;

		rct_ride_type *rideType = (rct_ride_type*)stex_entry;
		for (int i = 0; i < 3; i++) {
			if (rideType->ride_type[i] == 255)
				continue;

			stringId = 2 + rideType->ride_type[i];
			gfx_draw_string_right(dpi, stringId, NULL, 2, w->x + w->width - 5, y);
			y -= 11;
		}
	}

	//stringId = highlightedEntry->checksum
	// gfx_draw_string_right(dpi, stringId, NULL, 2, w->x + w->width - 5, w->y + w->height - 3 - 12 - 14);

	// Draw object dat name
	stringId = 3165;
	strcpy(stringBuffer, datName);
	gfx_draw_string_right(dpi, stringId, NULL, 0, w->x + w->width - 5, w->y + w->height - 3 - 12);
}

/**
 * 
 *  rct2: 0x006AADA3
 */
static void window_editor_object_selection_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	int x, y, i, colour, colour2;

	bool ridePage = (w->selected_tab == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS);

	colour = RCT2_ADDRESS(0x0141FC48, uint8)[w->colours[1] * 8];
	colour = (colour << 24) | (colour << 16) | (colour << 8) | colour;
	gfx_clear(dpi, colour);

	y = 0;
	for (i = 0; i < _numListItems; i++) {
		list_item *listItem = &_listItems[i];

		if (y + 12 >= dpi->y && y <= dpi->y + dpi->height) {
			// Draw checkbox
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) && !(*listItem->flags & 0x20))
				gfx_fill_rect_inset(dpi, 2, y, 11, y + 10, w->colours[1], 0xE0);

			// Highlight background
			colour = 142;
			if (listItem->entry == (rct_object_entry*)w->var_494 && !(*listItem->flags & OBJECT_SELECTION_FLAG_6)) {
				gfx_fill_rect(dpi, 0, y, w->width, y + 11, 0x2000031);
				colour = 14;
			}

			// Draw checkmark
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) && (*listItem->flags & OBJECT_SELECTION_FLAG_SELECTED)) {
				x = 2;
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = colour == 14 ? -2 : -1;
				colour2 = w->colours[1] & 0x7F;
				if (*listItem->flags & (OBJECT_SELECTION_FLAG_IN_USE | OBJECT_SELECTION_FLAG_REQUIRED | OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED))
					colour2 |= 0x40;

				gfx_draw_string(dpi, (char*)CheckBoxMarkString, colour2, x, y);
			}

			x = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER ? 0 : 15;

			char *bufferWithColour = (char*)0x0141ED68;
			char *buffer = utf8_write_codepoint(bufferWithColour, colour);
			if (*listItem->flags & OBJECT_SELECTION_FLAG_6) {
				colour = w->colours[1] & 0x7F;
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = -1;
			}
			else {
				colour = 0;
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = 224;
			}

			if (ridePage) {
				// Draw ride type
				strcpy(buffer, language_get_string(2 + listItem->filter->ride.ride_type));
				gfx_draw_string(dpi, bufferWithColour, colour, x, y);
				x = w->widgets[WIDX_LIST_SORT_RIDE].left - w->widgets[WIDX_LIST].left;
			}

			// Draw text
			strcpy(buffer, object_get_name(listItem->entry));
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER) {
				while (*buffer != 0 && *buffer != 9)
					buffer++;

				*buffer = 0;
			}
			gfx_draw_string(dpi, bufferWithColour, colour, x, y);
		}
		y += 12;
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

	if (page == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS) {
		_listSortType = RIDE_SORT_TYPE;
		_listSortDescending = false;
	} else {
		_listSortType = RIDE_SORT_RIDE;
		_listSortDescending = false;
	}

	visible_list_refresh(w);
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
*  rct2: 0x006AA805
*/
static void window_editor_object_selection_select_default_objects()
{
	int i;

	if (RCT2_GLOBAL(0x00F433F7, uint16) != 0)
		return;

	for (i = 0; i < countof(DefaultSelectedObjects); i++)
		window_editor_object_selection_select_object(0, 7, &DefaultSelectedObjects[i]);
}

/**
 *
 *  rct2: 0x006AA7E9
 */
static void window_editor_object_selection_select_required_objects()
{
	int i;

	for (i = 0; i < countof(RequiredSelectedObjects); i++)
		window_editor_object_selection_select_object(0, 0xF, &RequiredSelectedObjects[i]);
}

/* rct2: 0x006AA770 */
void reset_selected_object_count_and_size(){
	for (uint8 object_type = 0; object_type < 11; object_type++){
		RCT2_ADDRESS(0x00F433F7, uint16)[object_type] = 0;
	}

	uint32 total_object_size = 0;

	uint8* selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	rct_object_entry* installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i){
		uint8 object_type = installedObject->flags & 0xF;

		if (*selection_flags & OBJECT_SELECTION_FLAG_SELECTED){
			uint8* pos = (uint8*)installedObject;
			// Skip sizeof(rct_object_entry)
			pos += 16;

			// Skip filename
			while (*pos++);

			// Skip no of images
			pos += 4;

			// Skip name
			while (*pos++);

			uint32 size_of_chunk = *((uint32*)pos);
			RCT2_ADDRESS(0x00F433F7, uint16)[object_type]++;
			total_object_size += size_of_chunk;
		}
		selection_flags++;
		installedObject = object_get_next(installedObject);
	}

	RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_OBJECTS_FILE_SIZE, uint32) = total_object_size;
}

/* rct2: 0x006AB863 */
void set_required_object_flags(rct_object_entry* required_object){
	uint8* selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	rct_object_entry* installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i){
		if (object_entry_compare(required_object, installedObject)){
			*selection_flags |= OBJECT_SELECTION_FLAG_REQUIRED;

			uint8* pos = (uint8*)installedObject;
			// Skip sizeof(rct_object_entry)
			pos += 16;

			// Skip filename
			while (*pos++);

			// Skip no of images
			pos += 4;

			// Skip name
			while (*pos++);

			// Skip size of chunk
			pos += 4;

			uint8 no_required_objects = *pos++;

			required_object = (rct_object_entry*)pos;
			for (; no_required_objects > 0; no_required_objects--){
				set_required_object_flags(required_object);
				required_object++;
			}
			return;
		}

		selection_flags++;
		installedObject = object_get_next(installedObject);
	}
}

/* rct2: 0x006AB923 */
void reset_required_object_flags(){
	uint8* selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i){
		*selection_flags &= ~OBJECT_SELECTION_FLAG_REQUIRED;
		selection_flags++;
	}

	selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	rct_object_entry* installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i){
		if (*selection_flags & OBJECT_SELECTION_FLAG_SELECTED){
			uint8* pos = (uint8*)installedObject;
			// Skip sizeof(rct_object_entry)
			pos += 16;

			// Skip filename
			while (*pos++);

			// Skip no of images
			pos += 4;

			// Skip name
			while (*pos++);

			// Skip size of chunk
			pos += 4;

			uint8 no_required_objects = *pos++;

			rct_object_entry* required_object = (rct_object_entry*)pos;
			for (; no_required_objects > 0; no_required_objects--){
				set_required_object_flags(required_object);
				required_object++;
			}
			
		}

		selection_flags++;
		installedObject = object_get_next(installedObject);
	}
}

/* 
 * Master objects are objects that are not
 * optional / required dependants of an 
 * object.
 */
void set_object_selection_error(uint8 is_master_object, rct_string_id error_msg){
	RCT2_GLOBAL(0x141E9AC, rct_string_id) = error_msg;
	if (!is_master_object){
		reset_selected_object_count_and_size();
	}
}

/**
 *
 *  rct2: 0x006AB54F
 */
static int window_editor_object_selection_select_object(uint8 bh, int flags, rct_object_entry *entry)
{
	uint8* selection_flags;
	//if (bh == 0){
	//	// Unsure what this does??
	//	uint16 total_objects = 0;
	//	for (uint8 i = 0; i < 11; ++i){
	//		total_objects += RCT2_ADDRESS(0x00F433E1, uint16)[i];
	//	}

	//	selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	//	for (; total_objects != 0; total_objects--, selection_flags++){
	//		uint8 select_flag = *selection_flags & ~OBJECT_SELECTION_FLAG_2;
	//		if (select_flag & OBJECT_SELECTION_FLAG_SELECTED){
	//			select_flag |= OBJECT_SELECTION_FLAG_2;
	//		}
	//	}
	//}

	selection_flags = RCT2_GLOBAL(RCT2_ADDRESS_EDITOR_OBJECT_FLAGS_LIST, uint8*);
	// There was previously a check to make sure the object list had an item
	rct_object_entry* installedObject = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);

	uint8 not_found = 1;
	for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i){
		if (object_entry_compare(entry, installedObject)){
			not_found = 0;
			break;
		}

		installedObject = object_get_next(installedObject);
		selection_flags++;
	}
	if (not_found){
		set_object_selection_error(bh, 3169);
		return 0;
	}

	if (!(flags & 1)){
		if (!(*selection_flags & OBJECT_SELECTION_FLAG_SELECTED))
		{
			if (bh == 0){
				reset_required_object_flags();
			}
			return 1;
		}
		else if (*selection_flags & OBJECT_SELECTION_FLAG_IN_USE){
			set_object_selection_error(bh, 3173);
			return 0;
		}
		else if (*selection_flags & OBJECT_SELECTION_FLAG_REQUIRED){
			set_object_selection_error(bh, 3174);
			return 0;
		}
		else if (*selection_flags & OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED){
			set_object_selection_error(bh, 3175);
			return 0;
		}

		uint8* pos = (uint8*)installedObject;
		// Skip sizeof(rct_object_entry)
		pos += 16;

		// Skip filename
		while (*pos++);

		// Skip no of images
		pos += 4;

		// Skip name
		while (*pos++);

		uint32 size_of_chunk = *((uint32*)pos);
		// Skip size of chunk
		pos += 4;

		// Skip required objects
		pos += *pos * 16 + 1;

		uint8 no_theme_objects = *pos++;

		if (no_theme_objects != 0 && flags&(1 << 2)){
			rct_object_entry* theme_object = (rct_object_entry*)pos;
			for (; no_theme_objects > 0; no_theme_objects--){
				window_editor_object_selection_select_object(++bh, flags, theme_object);
				theme_object++;
			}
		}

		RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_OBJECTS_FILE_SIZE, uint32) -= size_of_chunk;
		uint8 object_type = installedObject->flags & 0xF;
		RCT2_ADDRESS(0x00F433F7, uint16)[object_type]--;
		*selection_flags &= ~OBJECT_SELECTION_FLAG_SELECTED;
		if (bh == 0){
			reset_required_object_flags();
		}
		return 1;
	}
	else{
		if (bh == 0){
			if (flags & (1 << 3)){
				*selection_flags |= OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED;
			}
		}
		if (*selection_flags & OBJECT_SELECTION_FLAG_SELECTED){
			if (bh == 0){
				reset_required_object_flags();
			}
			return 1;
		}

		uint8 object_type = installedObject->flags & 0xF;
		uint16 no_objects = object_entry_group_counts[object_type];
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER){
			no_objects = 4;
		}

		if (no_objects <= RCT2_ADDRESS(0x00F433F7, uint16)[object_type]){
			set_object_selection_error(bh, 3171);
			return 0;
		}

		uint8* pos = (uint8*)installedObject;
		// Skip sizeof(rct_object_entry)
		pos += 16;

		// Skip filename
		while (*pos++);

		// Skip no of images
		pos += 4;

		// Skip name
		while (*pos++);

		uint32 size_of_chunk = *((uint32*)pos);
		// Skip size of chunk
		pos += 4;

		uint8 num_required_objects = *pos++;	
		rct_object_entry* required_objects = (rct_object_entry*)pos;
		for (; num_required_objects != 0; num_required_objects--){
			if (!window_editor_object_selection_select_object(++bh, flags, required_objects)){
				if (bh != 0){
					reset_selected_object_count_and_size();
				}
				return 0;
			}
			required_objects++;
		}
		pos = (uint8*)required_objects;

		uint8 num_theme_objects = *pos++;
		rct_object_entry* theme_object = (rct_object_entry*)pos;
		for (; num_theme_objects != 0; num_theme_objects--){
			if (flags & (1 << 2)){
				if (!window_editor_object_selection_select_object(++bh, flags, theme_object)){
					RCT2_GLOBAL(0x00F43411, uint8) |= 1;
				}
			}
			theme_object++;
		}
		
		if (bh != 0 && !(flags&(1 << 1))){
			uint32* arguments = RCT2_ADDRESS(0x0013CE952, uint32);
			object_create_identifier_name((char*)0x009BC95A, installedObject);
			*arguments = (uint32)0x009BC95A;
			set_object_selection_error(bh, 3172);
			return 0;
		}

		if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_OBJECTS_FILE_SIZE, uint32) + size_of_chunk > 0x40000){
			set_object_selection_error(bh, 3170);
			return 0;
		}

		if (no_objects <= RCT2_ADDRESS(0x00F433F7, uint16)[object_type]){
			set_object_selection_error(bh, 3171);
			return 0;
		}

		RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_OBJECTS_FILE_SIZE, uint32) += size_of_chunk;
		RCT2_ADDRESS(0x00F433F7, uint16)[object_type]++;

		*selection_flags |= OBJECT_SELECTION_FLAG_SELECTED;
		if (bh == 0){
			reset_required_object_flags();
		}
		return 1;
	}
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
	int listItemIndex = y / 12;
	if (listItemIndex < 0 || listItemIndex >= _numListItems)
		return -1;

	list_item *listItem = &_listItems[listItemIndex];
	*object_selection_flags = *listItem->flags;
	*installed_entry = listItem->entry;
	return listItemIndex;
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
		if (*selection_flags & OBJECT_SELECTION_FLAG_SELECTED) {
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

static void window_editor_object_selection_update(rct_window *w)
{
	if (gCurrentTextBox.window.classification == w->classification &&
		gCurrentTextBox.window.number == w->number) {
		window_update_textbox_caret();
		widget_invalidate(w, WIDX_FILTER_STRING_BUTTON);
	}

	for (int i = WIDX_FILTER_RIDE_TAB_TRANSPORT; i <= WIDX_FILTER_RIDE_TAB_STALL; i++) {
		if (!(w->pressed_widgets & (1ULL << i)))
			continue;

		w->frame_no++;
		if (w->frame_no >= window_editor_object_selection_animation_loops[i - WIDX_FILTER_RIDE_TAB_TRANSPORT])
			w->frame_no = 0;

		widget_invalidate(w, i);
		break;
	}
}

static void window_editor_object_selection_textinput(rct_window *w, int widgetIndex, char *text)
{
	if (widgetIndex != WIDX_FILTER_STRING_BUTTON || text == NULL)
		return;

	if (strcmp(_filter_string, text) == 0)
		return;

	if (strlen(text) == 0) {
		memset(_filter_string, 0, sizeof(_filter_string));
	}
	else {
		memset(_filter_string, 0, sizeof(_filter_string));
		strcpy(_filter_string, text);
	}

	filter_update_counts();

	w->scrolls->v_top = 0;

	visible_list_refresh(w);
	window_invalidate(w);
}

static bool filter_string(rct_object_entry *entry)
{
	if (_filter_string[0] == 0)
		return true;

	char *name = object_get_name(entry);
	if (name[0] == 0)
		return false;

	char name_lower[MAX_PATH];
	char filter_lower[sizeof(_filter_string)];
	strcpy(name_lower, name);
	strcpy(filter_lower, _filter_string);

	for (int i = 0; i < (int)strlen(name_lower); i++)
		name_lower[i] = (char)tolower(name_lower[i]);
	for (int i = 0; i < (int)strlen(filter_lower); i++)
		filter_lower[i] = (char)tolower(filter_lower[i]);

	return strstr(name_lower, filter_lower) != NULL;
}

static bool filter_source(rct_object_entry *entry)
{
	if (_FILTER_ALL)
		return true;

	uint8 source = (entry->flags & 0xF0) >> 4;
	return (_FILTER_RCT2 && source == 8) || (_FILTER_WW && source == 1) || (_FILTER_TT && source == 2) || (_FILTER_CUSTOM && source != 8 && source != 1 && source != 2);
}

static bool filter_chunks(rct_object_entry *entry, rct_object_filters *filter)
{
	switch (entry->flags & 0x0F) {
	case OBJECT_TYPE_RIDE:
		if(!gConfigInterface.select_by_track_type) {
			if (_filter_flags & (1 << (filter->ride.category[0] + 5)))
				return true;
			if (_filter_flags & (1 << (filter->ride.category[1] + 5)))
				return true;
		}
		else {
			if (_filter_flags & (1 << (gRideCategories[filter->ride.ride_type] + 5)))
				return true;
		}
		return false;
	}
	return true;
}

static void filter_update_counts()
{
	if (!_FILTER_ALL || strlen(_filter_string) > 0) {
		rct_object_entry *installed_entry = RCT2_GLOBAL(RCT2_ADDRESS_INSTALLED_OBJECT_LIST, rct_object_entry*);
		rct_object_filters *filter;
		uint8 type;
		for (int i = 0; i < 11; i++) {
			_filter_object_counts[i] = 0;
		}
		for (int i = RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32); i > 0; --i) {
			filter = get_object_filter(RCT2_GLOBAL(RCT2_ADDRESS_OBJECT_LIST_NO_ITEMS, uint32) - i);
			type = installed_entry->flags & 0xF;
			if (filter_source(installed_entry) && filter_string(installed_entry) && filter_chunks(installed_entry, filter)) {
				_filter_object_counts[type]++;
			}
			installed_entry = object_get_next(installed_entry);
		}
	}
}
