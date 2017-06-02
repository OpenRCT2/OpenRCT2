#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../editor.h"
#include "../input.h"
#include "../interface/Cursors.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../management/research.h"
#include "../object.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../rct1.h"
#include "../sprites.h"
#include "../util/util.h"
#include "../world/scenery.h"

#pragma region Widgets

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_PRE_RESEARCHED_SCROLL,
	WIDX_RESEARCH_ORDER_SCROLL,
	WIDX_PREVIEW,
	WIDX_RANDOM_SHUFFLE,
	WIDX_MOVE_ITEMS_TO_BOTTOM,
	WIDX_MOVE_ITEMS_TO_TOP
};

static rct_widget window_editor_inventions_list_widgets[] = {
	{ WWT_FRAME,			0,	0,		599,	0,		399,	STR_NONE,				STR_NONE				},
	{ WWT_CAPTION,			0,	1,		598,	1,		14,		STR_INVENTION_LIST,		STR_WINDOW_TITLE_TIP	},
	{ WWT_CLOSEBOX,			0,	587,	597,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP	},
	{ WWT_RESIZE,			1,	0,		599,	43,		399,	STR_NONE,				STR_NONE				},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x20000000 | SPR_TAB,	STR_NONE				},
	{ WWT_SCROLL,			1,	4,		371,	56,		175,	SCROLL_VERTICAL,		STR_NONE				},
	{ WWT_SCROLL,			1,	4,		371,	189,	396,	SCROLL_VERTICAL,		STR_NONE				},
	{ WWT_FLATBTN,			1,	431,	544,	106,	219,	0xFFFFFFFF,				STR_NONE				},
	{ WWT_DROPDOWN_BUTTON,	1,	375,	594,	385,	396,	STR_RANDOM_SHUFFLE,		STR_RANDOM_SHUFFLE_TIP	},
	{ WWT_DROPDOWN_BUTTON,	1,	375,	594,	372,	383,	STR_MOVE_ALL_BOTTOM,	STR_NONE				},
	{ WWT_DROPDOWN_BUTTON,	1,	375,	594,	359,	370,	STR_MOVE_ALL_TOP,		STR_NONE				},
	{ WIDGETS_END }
};

static rct_widget window_editor_inventions_list_drag_widgets[] = {
	{ WWT_IMGBTN,			0,	0,		149,	0,		13,		STR_NONE,				STR_NONE				},
	{ WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_editor_inventions_list_close(rct_window *w);
static void window_editor_inventions_list_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_editor_inventions_list_update(rct_window *w);
static void window_editor_inventions_list_scrollgetheight(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_editor_inventions_list_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_editor_inventions_list_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_editor_inventions_list_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_editor_inventions_list_cursor(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y, sint32 *cursorId);
static void window_editor_inventions_list_invalidate(rct_window *w);
static void window_editor_inventions_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_editor_inventions_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static void window_editor_inventions_list_drag_cursor(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y, sint32 *cursorId);
static void window_editor_inventions_list_drag_moved(rct_window* w, sint32 x, sint32 y);
static void window_editor_inventions_list_drag_paint(rct_window *w, rct_drawpixelinfo *dpi);

// 0x0098177C
static rct_window_event_list window_editor_inventions_list_events = {
	window_editor_inventions_list_close,
	window_editor_inventions_list_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_editor_inventions_list_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_editor_inventions_list_scrollgetheight,
	window_editor_inventions_list_scrollmousedown,
	NULL,
	window_editor_inventions_list_scrollmouseover,
	NULL,
	NULL,
	NULL,
	window_editor_inventions_list_tooltip,
	window_editor_inventions_list_cursor,
	NULL,
	window_editor_inventions_list_invalidate,
	window_editor_inventions_list_paint,
	window_editor_inventions_list_scrollpaint
};

// 0x009817EC
static rct_window_event_list window_editor_inventions_list_drag_events = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_editor_inventions_list_drag_cursor,
	window_editor_inventions_list_drag_moved,
	NULL,
	window_editor_inventions_list_drag_paint,
	NULL
};

#pragma endregion

rct_research_item *_editorInventionsListDraggedItem;

static const rct_string_id EditorInventionsResearchCategories[] = {
	STR_RESEARCH_NEW_TRANSPORT_RIDES,
	STR_RESEARCH_NEW_GENTLE_RIDES,
	STR_RESEARCH_NEW_ROLLER_COASTERS,
	STR_RESEARCH_NEW_THRILL_RIDES,
	STR_RESEARCH_NEW_WATER_RIDES,
	STR_RESEARCH_NEW_SHOPS_AND_STALLS,
	STR_RESEARCH_NEW_SCENERY_AND_THEMING,
};

static void window_editor_inventions_list_drag_open(rct_research_item *researchItem);
static void move_research_item(rct_research_item *beforeItem);

static sint32 research_item_is_always_researched(rct_research_item *researchItem)
{
	return (researchItem->entryIndex & (RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED | RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED)) != 0;
}

/**
 *
 *  rct2: 0x0068596F
 * Sets rides that are in use to be always researched
 */
static void research_rides_setup(){
	// Reset all objects to not required
	for (uint8 object_type = OBJECT_TYPE_RIDE; object_type < 11; object_type++){
		uint8* in_use = gEditorSelectedObjects[object_type];
		for (uint8 num_objects = object_entry_group_counts[object_type]; num_objects != 0; num_objects--){
			*in_use++ = 0;
		}
	}

	// Set research required for rides in use
	for (uint16 rideIndex = 0; rideIndex < 255; rideIndex++){
		rct_ride* ride = get_ride(rideIndex);
		if (ride->type == RIDE_TYPE_NULL)continue;
		gEditorSelectedObjects[OBJECT_TYPE_RIDE][ride->subtype] |= 1;
	}

	for (rct_research_item* research = gResearchItems; research->entryIndex != RESEARCHED_ITEMS_END; research++){
		if (research->entryIndex & RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED)
			continue;

		// If not a ride
		if ((research->entryIndex & 0xFFFFFF) < 0x10000)
			continue;

		uint8 ride_base_type = (research->entryIndex >> 8) & 0xFF;

		uint8 object_index = research->entryIndex & 0xFF;
		rct_ride_entry* ride_entry = get_ride_entry(object_index);

		bool master_found = false;
		if (!(ride_entry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE)){

			for (uint8 rideType = 0; rideType < object_entry_group_counts[OBJECT_TYPE_RIDE]; rideType++){
				rct_ride_entry* master_ride = get_ride_entry(rideType);
				if (master_ride == NULL || (intptr_t)master_ride == -1)
					continue;

				if (master_ride->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE)
					continue;

				// If master ride not in use
				if (!(gEditorSelectedObjects[OBJECT_TYPE_RIDE][rideType] & (1 << 0)))
					continue;

				for (uint8 j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++) {
					if (master_ride->ride_type[j] == ride_base_type) {
						master_found = true;
						break;
					}
				}

				if (master_found) {
					break;
				}
			}
		}

		if (!master_found){
			// If not in use
			if (!(gEditorSelectedObjects[OBJECT_TYPE_RIDE][object_index] & (1 << 0))) {
				continue;
			}

			bool foundBaseType = false;
			for (uint8 j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++) {
				if (ride_entry->ride_type[j] == ride_base_type) {
					foundBaseType = true;
				}
			}

			if (!foundBaseType) {
				continue;
			}
		}

		research->entryIndex |= RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED;
		_editorInventionsListDraggedItem = research;
		move_research_item(gResearchItems);
		_editorInventionsListDraggedItem = NULL;
		research--;
	}
}

extern rct_object_entry RequiredSelectedObjects[7];

/**
 *
 *  rct2: 0x0068590C
 * Sets the critical scenery sets to always researched
 */
static void research_scenery_sets_setup(){

	for (sint32 i = 0; i < countof(RequiredSelectedObjects); i++) {
		rct_object_entry * object = &RequiredSelectedObjects[i];

		uint8 entry_type, entry_index;
		if (!find_object_in_entry_group(object, &entry_type, &entry_index))
			continue;

		if (entry_type != OBJECT_TYPE_SCENERY_SETS)
			continue;

		rct_research_item* research = gResearchItems;
		for (; research->entryIndex != RESEARCHED_ITEMS_END; research++){

			if ((research->entryIndex & 0xFFFFFF) != entry_index)
				continue;

			research->entryIndex |= RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED;
			_editorInventionsListDraggedItem = research;
			move_research_item(gResearchItems);
			_editorInventionsListDraggedItem = NULL;
		}
	}
}

/**
 *
 *  rct2: 0x00685901
 */
static void research_always_researched_setup()
{
	research_rides_setup();
	research_scenery_sets_setup();
}

/**
 *
 *  rct2: 0x00685A79
 *  Do not use the research list outside of the inventions list window with the flags
 */
static void research_remove_flags()
{
	for (rct_research_item* research = gResearchItems;
		research->entryIndex != RESEARCHED_ITEMS_END_2;
		research++){

		// Clear the always researched flags.
		if (research->entryIndex > RESEARCHED_ITEMS_SEPARATOR){
			research->entryIndex &= 0x00FFFFFF;
		}
	}
}

/**
 *
 *  rct2: 0x0068563D
 */
static rct_string_id research_item_get_name(uint32 researchItem)
{
	if (researchItem < 0x10000) {
		rct_scenery_set_entry *sceneryEntry = get_scenery_group_entry(researchItem & 0xFF);
		if (sceneryEntry == NULL || sceneryEntry == (rct_scenery_set_entry*)-1)
			return 0;

		return sceneryEntry->name;
	}

	rct_ride_entry *rideEntry = get_ride_entry(researchItem & 0xFF);
	if (rideEntry == NULL || rideEntry == (rct_ride_entry*)-1)
		return 0;

	if (rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME)
		return rideEntry->name;

	return ((researchItem >> 8) & 0xFF) + 2;
}

/**
 *
 *  rct2: 0x00685A93
 */
static void research_items_shuffle()
{
	rct_research_item *researchItem, *researchOrderBase, researchItemTemp;
	sint32 i, numNonResearchedItems;

	// Skip pre-researched items
	for (researchItem = gResearchItems; researchItem->entryIndex != RESEARCHED_ITEMS_SEPARATOR; researchItem++) {}
	researchItem++;
	researchOrderBase = researchItem;

	// Count non pre-researched items
	numNonResearchedItems = 0;
	for (; researchItem->entryIndex != RESEARCHED_ITEMS_END; researchItem++)
		numNonResearchedItems++;

	// Shuffle list
	for (i = 0; i < numNonResearchedItems; i++) {
		sint32 ri = util_rand() % numNonResearchedItems;
		if (ri == i)
			continue;

		researchItemTemp = researchOrderBase[i];
		researchOrderBase[i] = researchOrderBase[ri];
		researchOrderBase[ri] = researchItemTemp;
	}
}

static void research_items_make_all_unresearched()
{
	rct_research_item *researchItem, *nextResearchItem, researchItemTemp;

	sint32 sorted;
	do {
		sorted = 1;
		for (researchItem = gResearchItems; researchItem->entryIndex != RESEARCHED_ITEMS_SEPARATOR; researchItem++) {
			if (research_item_is_always_researched(researchItem))
				continue;

			nextResearchItem = researchItem + 1;
			if (nextResearchItem->entryIndex == RESEARCHED_ITEMS_SEPARATOR || research_item_is_always_researched(nextResearchItem)) {
				// Bubble up always researched item or separator
				researchItemTemp = *researchItem;
				*researchItem = *nextResearchItem;
				*nextResearchItem = researchItemTemp;
				sorted = 0;

				if (researchItem->entryIndex == RESEARCHED_ITEMS_SEPARATOR)
					break;
			}
		}
	} while (!sorted);
}

static void research_items_make_all_researched()
{
	rct_research_item *researchItem, researchItemTemp;

	// Find separator
	for (researchItem = gResearchItems; researchItem->entryIndex != RESEARCHED_ITEMS_SEPARATOR; researchItem++) { }

	// Move separator below all items
	for (; (researchItem + 1)->entryIndex != RESEARCHED_ITEMS_END; researchItem++) {
		// Swap separator with research item
		researchItemTemp = *researchItem;
		*researchItem = *(researchItem + 1);
		*(researchItem + 1) = researchItemTemp;
	}
}

/**
 *
 *  rct2: 0x006855E7
 */
static void move_research_item(rct_research_item *beforeItem)
{
	rct_window *w;
	rct_research_item *researchItem, draggedItem;

	if (_editorInventionsListDraggedItem + 1 == beforeItem)
		return;

	// Back up the dragged item
	draggedItem = *_editorInventionsListDraggedItem;

	// Remove dragged item from list
	researchItem = _editorInventionsListDraggedItem;
	do {
		*researchItem = *(researchItem + 1);
		researchItem++;
	} while (researchItem->entryIndex != RESEARCHED_ITEMS_END_2);
	// At end of this researchItem points to the end of the list

	if (beforeItem > _editorInventionsListDraggedItem)
		beforeItem--;

	// Add dragged item to list
	do {
		*researchItem = *(researchItem - 1);
		researchItem--;
	} while (researchItem != beforeItem);

	*researchItem = draggedItem;

	w = window_find_by_class(WC_EDITOR_INVENTION_LIST);
	if (w != NULL) {
		w->research_item = NULL;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x0068558E
 */
static rct_research_item *window_editor_inventions_list_get_item_from_scroll_y(sint32 scrollIndex, sint32 y)
{
	rct_research_item *researchItem;

	researchItem = gResearchItems;

	if (scrollIndex != 0) {
		// Skip pre-researched items
		for (; researchItem->entryIndex != RESEARCHED_ITEMS_SEPARATOR; researchItem++) { }
		researchItem++;
	}

	for (; researchItem->entryIndex != RESEARCHED_ITEMS_SEPARATOR && researchItem->entryIndex != RESEARCHED_ITEMS_END; researchItem++) {
		y -= 10;
		if (y < 0)
			return researchItem;
	}

	return NULL;
}

/**
 *
 *  rct2: 0x006855BB
 */
static rct_research_item *window_editor_inventions_list_get_item_from_scroll_y_include_seps(sint32 scrollIndex, sint32 y)
{
	rct_research_item *researchItem;

	researchItem = gResearchItems;

	if (scrollIndex != 0) {
		// Skip pre-researched items
		for (; researchItem->entryIndex != RESEARCHED_ITEMS_SEPARATOR; researchItem++) { }
		researchItem++;
	}

	for (; researchItem->entryIndex != RESEARCHED_ITEMS_SEPARATOR && researchItem->entryIndex != RESEARCHED_ITEMS_END; researchItem++) {
		y -= 10;
		if (y < 0)
			return researchItem;
	}

	return researchItem;
}

static rct_research_item *get_research_item_at(sint32 x, sint32 y)
{
	rct_window *w = window_find_by_class(WC_EDITOR_INVENTION_LIST);
	if (w != NULL && w->x <= x && w->y < y && w->x + w->width > x && w->y + w->height > y) {
		rct_widgetindex widgetIndex = window_find_widget_from_point(w, x, y);
		rct_widget *widget = &w->widgets[widgetIndex];
		if (widgetIndex == WIDX_PRE_RESEARCHED_SCROLL || widgetIndex == WIDX_RESEARCH_ORDER_SCROLL) {
			gPressedWidget.widget_index = widgetIndex;
			sint32 outX, outY, outScrollArea, outScrollId;
			widget_scroll_get_part(w, widget, x, y, &outX, &outY, &outScrollArea, &outScrollId);
			if (outScrollArea == SCROLL_PART_VIEW) {
				outScrollId = outScrollId == 0 ? 0 : 1;

				sint32 scrollY = y - (w->y + widget->top) + w->scrolls[outScrollId].v_top + 5;
				return window_editor_inventions_list_get_item_from_scroll_y_include_seps(outScrollId, scrollY);
			}
		}
	}

	return NULL;
}

/**
 *
 *  rct2: 0x00684E04
 */
void window_editor_inventions_list_open()
{
	rct_window *w;

	w = window_bring_to_front_by_class(WC_EDITOR_INVENTION_LIST);
	if (w != NULL)
		return;

	research_always_researched_setup();

	w = window_create_centred(
		600,
		400,
		&window_editor_inventions_list_events,
		WC_EDITOR_INVENTION_LIST,
		WF_NO_SCROLLING
	);
	w->widgets = window_editor_inventions_list_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_TAB_1) |
		(1 << WIDX_RANDOM_SHUFFLE) |
		(1 << WIDX_MOVE_ITEMS_TO_BOTTOM) |
		(1 << WIDX_MOVE_ITEMS_TO_TOP);
	window_init_scroll_widgets(w);
	w->var_4AE = 0;
	w->selected_tab = 0;
	w->research_item = NULL;
	_editorInventionsListDraggedItem = NULL;
}

/**
 *
 *  rct2: 0x006853D2
 */
static void window_editor_inventions_list_close(rct_window *w)
{
	research_remove_flags();

	// When used in-game (as a cheat)
	if (!(gScreenFlags & SCREEN_FLAGS_EDITOR)) {
		gSilentResearch = true;
		research_reset_current_item();
		gSilentResearch = false;
	}
}

/**
 *
 *  rct2: 0x0068521B
 */
static void window_editor_inventions_list_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_RANDOM_SHUFFLE:
		research_items_shuffle();
		window_invalidate(w);
		break;
	case WIDX_MOVE_ITEMS_TO_TOP:
		research_items_make_all_researched();
		window_init_scroll_widgets(w);
		window_invalidate(w);
		break;
	case WIDX_MOVE_ITEMS_TO_BOTTOM:
		research_items_make_all_unresearched();
		window_init_scroll_widgets(w);
		window_invalidate(w);
		break;
	}
}

/**
 *
 *  rct2: 0x00685392
 */
static void window_editor_inventions_list_update(rct_window *w)
{
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_1);

	if (_editorInventionsListDraggedItem == NULL)
		return;

	if (window_find_by_class(WC_EDITOR_INVENTION_LIST_DRAG) != NULL)
		return;

	_editorInventionsListDraggedItem = NULL;
	window_invalidate(w);
}

/**
 *
 *  rct2: 0x00685239
 */
static void window_editor_inventions_list_scrollgetheight(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
	rct_research_item *researchItem;

	*height = 0;

	// Count / skip pre-researched items
	for (researchItem = gResearchItems; researchItem->entryIndex != RESEARCHED_ITEMS_SEPARATOR; researchItem++)
		*height += 10;

	if (scrollIndex == 1) {
		researchItem++;

		// Count non pre-researched items
		*height = 0;
		for (; researchItem->entryIndex != RESEARCHED_ITEMS_END; researchItem++)
			*height += 10;
	}
}

/**
 *
 *  rct2: 0x006852D4
 */
static void window_editor_inventions_list_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	rct_research_item *researchItem;

	researchItem = window_editor_inventions_list_get_item_from_scroll_y(scrollIndex, y);
	if (researchItem == NULL)
		return;

	if (researchItem->entryIndex < (uint32)RESEARCHED_ITEMS_END_2 && research_item_is_always_researched(researchItem))
		return;

	window_invalidate(w);
	window_editor_inventions_list_drag_open(researchItem);
}

/**
 *
 *  rct2: 0x00685275
 */
static void window_editor_inventions_list_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	rct_research_item *researchItem;

	researchItem = window_editor_inventions_list_get_item_from_scroll_y(scrollIndex, y);
	if (researchItem != w->research_item) {
		w->research_item = researchItem;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x0068526B
 */
static void window_editor_inventions_list_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
	set_format_arg(0, rct_string_id, STR_LIST);
}

/**
 *
 *  rct2: 0x00685291
 */
static void window_editor_inventions_list_cursor(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y, sint32 *cursorId)
{
	rct_research_item *researchItem;
	sint32 scrollIndex;

	switch (widgetIndex) {
	case WIDX_PRE_RESEARCHED_SCROLL:
		scrollIndex = 0;
		break;
	case WIDX_RESEARCH_ORDER_SCROLL:
		scrollIndex = 1;
		break;
	default:
		return;
	}

	researchItem = window_editor_inventions_list_get_item_from_scroll_y(scrollIndex, y);
	if (researchItem == NULL)
		return;

	if (researchItem->entryIndex < (uint32)RESEARCHED_ITEMS_END_2 && research_item_is_always_researched(researchItem)) {
		return;
	}

	*cursorId = CURSOR_HAND_OPEN;
}

/**
 *
 *  rct2: 0x00685392
 */
static void window_editor_inventions_list_invalidate(rct_window *w)
{
	w->pressed_widgets |= 1 << WIDX_PREVIEW;
	w->pressed_widgets |= 1 << WIDX_TAB_1;

	w->widgets[WIDX_CLOSE].type =
		gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR ? WWT_EMPTY : WWT_CLOSEBOX;
}

/**
 *
 *  rct2: 0x00684EE0
 */
static void window_editor_inventions_list_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_widget *widget;
	rct_research_item *researchItem;
	rct_string_id stringId;
	sint32 x, y, width;

	window_draw_widgets(w, dpi);

	// Tab image
	x = w->x + w->widgets[WIDX_TAB_1].left;
	y = w->y + w->widgets[WIDX_TAB_1].top;
	gfx_draw_sprite(dpi, SPR_TAB_FINANCES_RESEARCH_0 + (w->frame_no / 2) % 8, x, y, 0);

	// Pre-researched items label
	x = w->x + w->widgets[WIDX_PRE_RESEARCHED_SCROLL].left;
	y = w->y + w->widgets[WIDX_PRE_RESEARCHED_SCROLL].top - 11;
	gfx_draw_string_left(dpi, STR_INVENTION_PREINVENTED_ITEMS, NULL, COLOUR_BLACK, x, y - 1);

	// Research order label
	x = w->x + w->widgets[WIDX_RESEARCH_ORDER_SCROLL].left;
	y = w->y + w->widgets[WIDX_RESEARCH_ORDER_SCROLL].top - 11;
	gfx_draw_string_left(dpi, STR_INVENTION_TO_BE_INVENTED_ITEMS, NULL, COLOUR_BLACK, x, y - 1);

	// Preview background
	widget = &w->widgets[WIDX_PREVIEW];
	gfx_fill_rect(
		dpi,
		w->x + widget->left + 1,
		w->y + widget->top + 1,
		w->x + widget->right - 1,
		w->y + widget->bottom - 1,
		ColourMapA[w->colours[1]].darkest
	);

	researchItem = _editorInventionsListDraggedItem;
	if (researchItem == NULL)
		researchItem = w->research_item;
	// If the research item is null or a list separator.
	if (researchItem == NULL || researchItem->entryIndex < 0)
		return;

	// Preview image
	sint32 objectEntryType = 7;
	sint32 eax = researchItem->entryIndex & 0xFFFFFF;
	if (eax >= 0x10000)
		objectEntryType = 0;

	void *chunk = object_entry_groups[objectEntryType].chunks[researchItem->entryIndex & 0xFF];

	if (chunk == NULL || chunk == (void*)-1)
		return;

	rct_object_entry * entry = &object_entry_groups[objectEntryType].entries[researchItem->entryIndex & 0xFF].entry;

	// Draw preview
	widget = &w->widgets[WIDX_PREVIEW];

	void * object = object_manager_get_loaded_object(entry);
	if (object != NULL) {
		rct_drawpixelinfo clipDPI;
		x = w->x + widget->left + 1;
		y = w->y + widget->top + 1;
		width = widget->right - widget->left - 1;
		sint32 height = widget->bottom - widget->top - 1;
		if (clip_drawpixelinfo(&clipDPI, dpi, x, y, width, height)) {
			object_draw_preview(object, &clipDPI, width, height);
		}
	}

	// Item name
	x = w->x + ((widget->left + widget->right) / 2) + 1;
	y = w->y + widget->bottom + 3;
	width = w->width - w->widgets[WIDX_RESEARCH_ORDER_SCROLL].right - 6;
	stringId = research_item_get_name(eax);
	gfx_draw_string_centred_clipped(dpi, STR_WINDOW_COLOUR_2_STRINGID, &stringId, COLOUR_BLACK, x, y, width);
	y += 15;

	// Item category
	x = w->x + w->widgets[WIDX_RESEARCH_ORDER_SCROLL].right + 4;
	stringId = EditorInventionsResearchCategories[researchItem->category];
	gfx_draw_string_left(dpi, STR_INVENTION_RESEARCH_GROUP, &stringId, COLOUR_BLACK, x, y);
}

/**
 *
 *  rct2: 0x006850BD
 */
static void window_editor_inventions_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
	rct_research_item *researchItem;
	sint32 left, top, bottom, itemY, disableItemMovement;
	sint32 researchItemEndMarker;
	rct_string_id stringId;
	utf8 buffer[256], *ptr;

	// Draw background
	uint8 paletteIndex = ColourMapA[w->colours[1]].mid_light;
	gfx_clear(dpi, paletteIndex);

	researchItem = gResearchItems;

	if (scrollIndex == 1) {
		// Skip pre-researched items
		for (; researchItem->entryIndex != RESEARCHED_ITEMS_SEPARATOR; researchItem++) { }
		researchItem++;
		researchItemEndMarker = RESEARCHED_ITEMS_END;
	} else {
		researchItemEndMarker = RESEARCHED_ITEMS_SEPARATOR;
	}

	// Since this is now a do while need to counteract the +10
	itemY = -10;
	do{
		itemY += 10;
		if (itemY + 10 < dpi->y || itemY >= dpi->y + dpi->height)
			continue;

		uint8 colour = COLOUR_BRIGHT_GREEN | COLOUR_FLAG_TRANSLUCENT;
		if (w->research_item == researchItem) {
			if (_editorInventionsListDraggedItem == NULL) {
				// Highlight
				top = itemY;
				bottom = itemY + 9;
			} else {
				// Drop horizontal rule
				top = itemY - 1;
				bottom = itemY;
			}
			gfx_filter_rect(dpi, 0, top, w->width, bottom, PALETTE_DARKEN_1);

			if (_editorInventionsListDraggedItem == NULL)
				colour = COLOUR_BRIGHT_GREEN;
		}

		if (researchItem->entryIndex == RESEARCHED_ITEMS_SEPARATOR || researchItem->entryIndex == RESEARCHED_ITEMS_END)
			continue;

		if (researchItem == _editorInventionsListDraggedItem)
			continue;

		disableItemMovement = research_item_is_always_researched(researchItem);
		stringId = research_item_get_name(researchItem->entryIndex & 0xFFFFFF);

		ptr = buffer;
		if (!disableItemMovement) {
			ptr = utf8_write_codepoint(ptr, colour);
		}

		format_string(ptr, 256, stringId, NULL);

		if (disableItemMovement) {
			gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM_DARK;
			if (colour == COLOUR_BRIGHT_GREEN && _editorInventionsListDraggedItem == NULL) {
				gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM_EXTRA_DARK;
			}
			colour = COLOUR_FLAG_INSET | w->colours[1];
		} else {
			gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
			colour = COLOUR_BLACK;
		}

		left = 1;
		top = itemY - 1;
		gfx_draw_string(dpi, buffer, colour, left, top);
	}while(researchItem++->entryIndex != researchItemEndMarker);
}

#pragma region Drag item

/**
 *
 *  rct2: 0x006852F4
 */
static void window_editor_inventions_list_drag_open(rct_research_item *researchItem)
{
	char buffer[256];
	sint32 stringWidth;
	rct_string_id stringId;
	rct_window *w;

	window_close_by_class(WC_EDITOR_INVENTION_LIST_DRAG);
	_editorInventionsListDraggedItem = researchItem;

	stringId = research_item_get_name(researchItem->entryIndex & 0xFFFFFF);
	format_string(buffer, 256, stringId, NULL);
	stringWidth = gfx_get_string_width(buffer);
	window_editor_inventions_list_drag_widgets[0].right = stringWidth;

	w = window_create(
		gTooltipCursorX - (stringWidth / 2),
		gTooltipCursorY - 7,
		stringWidth,
		14,
		&window_editor_inventions_list_drag_events,
		WC_EDITOR_INVENTION_LIST_DRAG,
		WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_SNAPPING
	);
	w->widgets = window_editor_inventions_list_drag_widgets;
	w->colours[1] = COLOUR_WHITE;
	input_window_position_begin(w, 0, gTooltipCursorX, gTooltipCursorY);
}

/**
 *
 *  rct2: 0x0068549C
 */
static void window_editor_inventions_list_drag_cursor(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y, sint32 *cursorId)
{
	rct_window *inventionListWindow = window_find_by_class(WC_EDITOR_INVENTION_LIST);
	if (inventionListWindow != NULL) {
		rct_research_item *researchItem = get_research_item_at(x, y);
		if (researchItem != inventionListWindow->research_item) {
			inventionListWindow = (rct_window *)researchItem;
			window_invalidate(inventionListWindow);
		}
	}

	*cursorId = CURSOR_HAND_CLOSED;
}

/**
 *
 *  rct2: 0x00685412
 */
static void window_editor_inventions_list_drag_moved(rct_window* w, sint32 x, sint32 y)
{
	rct_research_item *researchItem;

	researchItem = get_research_item_at(x, y);
	if (researchItem != NULL)
		move_research_item(researchItem);

	window_close(w);
	_editorInventionsListDraggedItem = NULL;
	window_invalidate_by_class(WC_EDITOR_INVENTION_LIST);
}

/**
 *
 *  rct2: 0x006853D9
 */
static void window_editor_inventions_list_drag_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_string_id stringId;
	sint32 x, y;

	x = w->x;
	y = w->y + 2;
	stringId = research_item_get_name(_editorInventionsListDraggedItem->entryIndex & 0xFFFFFF);
	gfx_draw_string_left(dpi, STR_WINDOW_COLOUR_2_STRINGID, &stringId, COLOUR_BLACK | COLOUR_FLAG_OUTLINE, x, y);
}

#pragma endregion
