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
#include "../cursors.h"
#include "../input.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../management/research.h"
#include "../object.h"
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
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,				STR_NONE				},
	{ WWT_SCROLL,			1,	4,		371,	56,		175,	2,						STR_NONE				},
	{ WWT_SCROLL,			1,	4,		371,	189,	396,	2,						STR_NONE				},
	{ WWT_FLATBTN,			1,	431,	544,	106,	219,	0xFFFFFFFF,				STR_NONE				},
	{ WWT_DROPDOWN_BUTTON,	1,	375,	594,	385,	396,	STR_RANDOM_SHUFFLE,		STR_RANDOM_SHUFFLE_TIP	},
	{ WWT_DROPDOWN_BUTTON,	1,	375,	594,	372,	383,	2751,					STR_NONE				},
	{ WWT_DROPDOWN_BUTTON,	1,	375,	594,	359,	370,	2750,					STR_NONE				},
	{ WIDGETS_END }
};

static rct_widget window_editor_inventions_list_drag_widgets[] = {
	{ WWT_IMGBTN,			0,	0,		149,	0,		13,		STR_NONE,				STR_NONE				},
	{ WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_editor_inventions_list_emptysub() { }

static void window_editor_inventions_list_close();
static void window_editor_inventions_list_mouseup();
static void window_editor_inventions_list_update(rct_window *w);
static void window_editor_inventions_list_scrollgetheight();
static void window_editor_inventions_list_scrollmousedown();
static void window_editor_inventions_list_scrollmouseover();
static void window_editor_inventions_list_tooltip();
static void window_editor_inventions_list_cursor();
static void window_editor_inventions_list_invalidate();
static void window_editor_inventions_list_paint();
static void window_editor_inventions_list_scrollpaint();

static void window_editor_inventions_list_drag_cursor();
static void window_editor_inventions_list_drag_moved();
static void window_editor_inventions_list_drag_paint();

// 0x0098177C
static void* window_editor_inventions_list_events[] = {
	window_editor_inventions_list_close,
	window_editor_inventions_list_mouseup,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_update,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_scrollgetheight,
	window_editor_inventions_list_scrollmousedown,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_scrollmouseover,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_tooltip,
	window_editor_inventions_list_cursor,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_invalidate,
	window_editor_inventions_list_paint,
	window_editor_inventions_list_scrollpaint
};

// 0x0098177C
static void* window_editor_inventions_list_drag_events[] = {
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_drag_cursor,
	window_editor_inventions_list_drag_moved,
	window_editor_inventions_list_emptysub,
	window_editor_inventions_list_drag_paint,
	window_editor_inventions_list_emptysub
};

#pragma endregion

rct_research_item *_editorInventionsListDraggedItem;

#define WindowHighlightedItem(w) *((rct_research_item**)&(w->var_494))

static void window_editor_inventions_list_drag_open(rct_research_item *researchItem);

static int research_item_is_always_researched(rct_research_item *researchItem)
{
	return (researchItem->entryIndex & 0x60000000) != 0;
}

/**
 *
 *  rct2: 0x00685901
 */
static void sub_685901()
{
	RCT2_CALLPROC_EBPSAFE(0x00685901);
}

/**
 *
 *  rct2: 0x00685A79
 */
static void sub_685A79()
{
	RCT2_CALLPROC_EBPSAFE(0x00685A79);
}

/**
 *
 *  rct2: 0x0068563D
 */
static rct_string_id research_item_get_name(uint32 researchItem)
{
	rct_ride_type *rideEntry;
	rct_scenery_set_entry *sceneryEntry;
	
	if (researchItem < 0x10000) {
		sceneryEntry = g_scenerySetEntries[researchItem & 0xFF];
		if (sceneryEntry == NULL || sceneryEntry == (rct_scenery_set_entry*)0xFFFFFFFF)
			return 0;

		return sceneryEntry->name;
	}

	rideEntry = GET_RIDE_ENTRY(researchItem & 0xFF);
	if (rideEntry == NULL || rideEntry == (rct_ride_type*)0xFFFFFFFF)
		return 0;

	if (rideEntry->var_008 & 0x1000)
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
	int i, ri, numNonResearchedItems;

	// Skip pre-researched items
	for (researchItem = gResearchItems; researchItem->entryIndex != RESEARCHED_ITEMS_SEPERATOR; researchItem++) {}
	researchItem++;
	researchOrderBase = researchItem;

	// Count non pre-researched items
	numNonResearchedItems = 0;
	for (; researchItem->entryIndex != RESEARCHED_ITEMS_END; researchItem++)
		numNonResearchedItems++;

	// Shuffle list
	for (i = 0; i < numNonResearchedItems; i++) {
		ri = rand() % numNonResearchedItems;
		if (ri == i)
			continue;

		researchItemTemp = researchOrderBase[i];
		researchOrderBase[i] = researchOrderBase[ri];
		researchOrderBase[ri] = researchItemTemp;
	}

	// RCT2_CALLPROC_EBPSAFE(0x00685A93);
}

static void research_items_make_all_unresearched()
{
	rct_research_item *researchItem, *nextResearchItem, researchItemTemp;

	int sorted;
	do {
		sorted = 1;
		for (researchItem = gResearchItems; researchItem->entryIndex != RESEARCHED_ITEMS_SEPERATOR; researchItem++) {
			if (research_item_is_always_researched(researchItem))
				continue;

			nextResearchItem = researchItem + 1;
			if (nextResearchItem->entryIndex == RESEARCHED_ITEMS_SEPERATOR || research_item_is_always_researched(nextResearchItem)) {
				// Bubble up always researched item or seperator
				researchItemTemp = *researchItem;
				*researchItem = *nextResearchItem;
				*nextResearchItem = researchItemTemp;
				sorted = 0;

				if (researchItem->entryIndex == RESEARCHED_ITEMS_SEPERATOR)
					break;
			}
		}
	} while (!sorted);
}

static void research_items_make_all_researched()
{
	rct_research_item *researchItem, researchItemTemp;

	// Find seperator
	for (researchItem = gResearchItems; researchItem->entryIndex != RESEARCHED_ITEMS_SEPERATOR; researchItem++) { }

	// Move seperator below all items
	for (; (researchItem + 1)->entryIndex != RESEARCHED_ITEMS_END; researchItem++) {
		// Swap seperator with research item
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

	if (_editorInventionsListDraggedItem == beforeItem - 1)
		return;

	// Back up the dragged item
	draggedItem = *_editorInventionsListDraggedItem;

	// Remove dragged item from list
	researchItem = _editorInventionsListDraggedItem;
	do {
		*researchItem = *(researchItem + 1);
		researchItem++;
	} while ((researchItem - 1)->entryIndex != RESEARCHED_ITEMS_END);

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
		WindowHighlightedItem(w) = NULL;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x0068558E
 */
static rct_research_item *window_editor_inventions_list_get_item_from_scroll_y(int scrollIndex, int y)
{
	rct_research_item *researchItem;

	researchItem = gResearchItems;

	if (scrollIndex != 0) {
		// Skip pre-researched items
		for (; researchItem->entryIndex != RESEARCHED_ITEMS_SEPERATOR; researchItem++) { }
		researchItem++;
	}

	for (; researchItem->entryIndex != RESEARCHED_ITEMS_SEPERATOR && researchItem->entryIndex != RESEARCHED_ITEMS_END; researchItem++) {
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
static rct_research_item *window_editor_inventions_list_get_item_from_scroll_y_include_seps(int scrollIndex, int y)
{
	rct_research_item *researchItem;

	researchItem = gResearchItems;

	if (scrollIndex != 0) {
		// Skip pre-researched items
		for (; researchItem->entryIndex != RESEARCHED_ITEMS_SEPERATOR; researchItem++) { }
		researchItem++;
	}

	for (; researchItem->entryIndex != RESEARCHED_ITEMS_END_2; researchItem++) {
		y -= 10;
		if (y < 0)
			return researchItem;
	}

	return researchItem - 1;
}

static rct_research_item *get_research_item_at(int x, int y)
{
	rct_window *w;
	rct_widget *widget;
	int scrollY, outX, outY, outScrollArea, outScrollId;
	short widgetIndex;

	w = window_find_by_class(WC_EDITOR_INVENTION_LIST);
	if (w != NULL && w->x <= x && w->y < y && w->x + w->width > x && w->y + w->height > y) {
		widgetIndex = window_find_widget_from_point(w, x, y);
		widget = &w->widgets[widgetIndex];
		if (widgetIndex == WIDX_PRE_RESEARCHED_SCROLL || widgetIndex == WIDX_RESEARCH_ORDER_SCROLL) {
			RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, uint32) = widgetIndex;
			widget_scroll_get_part(w, widget, x, y, &outX, &outY, &outScrollArea, &outScrollId);
			if (outScrollArea == SCROLL_PART_VIEW) {
				outScrollId = outScrollId == 0 ? 0 : 1;

				scrollY = y - (w->y + widget->top) + w->scrolls[outScrollId].v_top + 5;
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
	// RCT2_CALLPROC_EBPSAFE(0x00684E04); return;

	rct_window *w;

	w = window_bring_to_front_by_class(WC_EDITOR_INVENTION_LIST);
	if (w != NULL)
		return;

	sub_685901();

	w = window_create_centred(
		600,
		400,
		(uint32*)window_editor_inventions_list_events,
		WC_EDITOR_INVENTION_LIST,
		WF_2
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
	WindowHighlightedItem(w) = NULL;
	_editorInventionsListDraggedItem = NULL;
	w->colours[0] = 4;
	w->colours[1] = 1;
	w->colours[2] = 1;
}

/**
 *
 *  rct2: 0x006853D2
 */
static void window_editor_inventions_list_close()
{
	sub_685A79();
}

/**
 *
 *  rct2: 0x0068521B
 */
static void window_editor_inventions_list_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

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
		window_invalidate(w);
		break;
	case WIDX_MOVE_ITEMS_TO_BOTTOM:
		research_items_make_all_unresearched();
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
static void window_editor_inventions_list_scrollgetheight()
{
	rct_window *w;
	short scrollIndex;
	rct_research_item *researchItem;
	int width, height;

	window_scroll_get_registers(w, scrollIndex);

	width = 0;
	height = 0;

	// Count / skip pre-researched items
	for (researchItem = gResearchItems; researchItem->entryIndex != RESEARCHED_ITEMS_SEPERATOR; researchItem++)
		height += 10;
	
	if (scrollIndex == 1) {
		researchItem++;

		// Count non pre-researched items
		height = 0;
		for (; researchItem->entryIndex != RESEARCHED_ITEMS_END; researchItem++)
			height += 10;
	}

	window_scrollsize_set_registers(width, height);
}

/**
 *
 *  rct2: 0x006852D4
 */
static void window_editor_inventions_list_scrollmousedown()
{
	rct_window *w;
	rct_research_item *researchItem;
	short scrollIndex, x, y;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	researchItem = window_editor_inventions_list_get_item_from_scroll_y(scrollIndex, y);
	if (researchItem == NULL)
		return;

	if (researchItem->entryIndex < (uint32)-3 && research_item_is_always_researched(researchItem))
		return;

	window_invalidate(w);
	window_editor_inventions_list_drag_open(researchItem);
}

/**
 *
 *  rct2: 0x00685275
 */
static void window_editor_inventions_list_scrollmouseover()
{
	rct_window *w;
	short scrollIndex, x, y;
	rct_research_item *researchItem;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	researchItem = window_editor_inventions_list_get_item_from_scroll_y(scrollIndex, y);
	if (researchItem != WindowHighlightedItem(w)) {
		WindowHighlightedItem(w) = researchItem;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x0068526B
 */
static void window_editor_inventions_list_tooltip()
{
	RCT2_GLOBAL(0x013CE952, uint16) = 3159;
}

/**
 *
 *  rct2: 0x00685291
 */
static void window_editor_inventions_list_cursor()
{
	rct_window *w;
	rct_research_item *researchItem;
	short widgetIndex, x, y;
	int scrollIndex, cursorId;

	window_cursor_get_registers(w, widgetIndex, x, y);

	if (widgetIndex == WIDX_PRE_RESEARCHED_SCROLL) {
		scrollIndex = 0;
	} else if (widgetIndex == WIDX_RESEARCH_ORDER_SCROLL) {
		scrollIndex = 1;
	} else {
		cursorId = -1;
		window_cursor_set_registers(cursorId);
		return;
	}

	researchItem = window_editor_inventions_list_get_item_from_scroll_y(scrollIndex, y);
	if (researchItem == NULL)
		return;

	if (researchItem->entryIndex < (uint32)-3 && research_item_is_always_researched(researchItem)) {
		cursorId = -1;
		window_cursor_set_registers(cursorId);
		return;
	}

	cursorId = CURSOR_HAND_OPEN;
	window_cursor_set_registers(cursorId);
}

/**
 *
 *  rct2: 0x00685392
 */
static void window_editor_inventions_list_invalidate()
{
	rct_window *w;

	window_get_register(w);

	w->pressed_widgets |= 1 << WIDX_PREVIEW;
	w->pressed_widgets |= 1 << WIDX_TAB_1;

	w->widgets[WIDX_CLOSE].type =
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR ? WWT_EMPTY : WWT_CLOSEBOX;
}

/**
 *
 *  rct2: 0x00684EE0
 */
static void window_editor_inventions_list_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_widget *widget;
	rct_research_item *researchItem;
	rct_string_id stringId;
	int x, y, width;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	
	// Tab image
	x = w->x + w->widgets[WIDX_TAB_1].left;
	y = w->y + w->widgets[WIDX_TAB_1].top;
	gfx_draw_sprite(dpi, 5327 + (w->frame_no / 2) % 8, x, y, 0);

	// Pre-researched items label
	x = w->x + w->widgets[WIDX_PRE_RESEARCHED_SCROLL].left;
	y = w->y + w->widgets[WIDX_PRE_RESEARCHED_SCROLL].top - 11;
	gfx_draw_string_left(dpi, 3197, NULL, 0, x, y);

	// Research order label
	x = w->x + w->widgets[WIDX_RESEARCH_ORDER_SCROLL].left;
	y = w->y + w->widgets[WIDX_RESEARCH_ORDER_SCROLL].top - 11;
	gfx_draw_string_left(dpi, 3198, NULL, 0, x, y);

	// Preview background
	widget = &w->widgets[WIDX_PREVIEW];
	gfx_fill_rect(
		dpi,
		w->x + widget->left + 1,
		w->y + widget->top + 1,
		w->x + widget->right - 1,
		w->y + widget->bottom - 1,
		RCT2_GLOBAL(0x0141FC44 + (w->colours[1] * 8), uint8)
	);

	researchItem = _editorInventionsListDraggedItem;
	if (researchItem == NULL)
		researchItem = WindowHighlightedItem(w);
	if (researchItem == NULL)
		return;

	// Preview image
	x = w->x + ((widget->left + widget->right) / 2) + 1;
	y = w->y + ((widget->top + widget->bottom) / 2) + 1;

	int objectEntryType = 7;
	int eax = researchItem->entryIndex & 0xFFFFFF;
	if (eax >= 0x10000)
		objectEntryType = 0;

	void **entries = RCT2_GLOBAL(0x0098D97C + (objectEntryType * 8), void*);
	void *entry = entries[researchItem->entryIndex & 0xFF];
	if (entry == NULL || entry == (void*)0xFFFFFFFF)
		return;

	object_paint(objectEntryType, 3, objectEntryType, x, y, 0, (int)dpi, (int)entry);

	// Item name
	x = w->x + ((widget->left + widget->right) / 2) + 1;
	y = w->y + widget->bottom + 3;
	width = w->width - w->widgets[WIDX_RESEARCH_ORDER_SCROLL].right - 6;
	stringId = research_item_get_name(eax);
	gfx_draw_string_centred_clipped(dpi, 1193, &stringId, 0, x, y, width);
	y += 15;

	// Item category
	x = w->x + w->widgets[WIDX_RESEARCH_ORDER_SCROLL].right + 4;
	stringId = 2253 + researchItem->category;
	gfx_draw_string_left(dpi, 3196, &stringId, 0, x, y);
}

/**
 *
 *  rct2: 0x006850BD
 */
static void window_editor_inventions_list_scrollpaint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	short scrollIndex;
	uint32 colour;
	rct_research_item *researchItem;
	int left, top, bottom, itemY, disableItemMovement;
	sint32 researchItemEndMarker;
	rct_string_id stringId;
	char buffer[256], *ptr;

	window_scrollpaint_get_registers(w, dpi, scrollIndex);

	// Draw background
	colour = RCT2_GLOBAL(0x0141FC48 + (w->colours[1] * 8), uint8);
	colour = (colour << 24) | (colour << 16) | (colour << 8) | colour;
	gfx_clear(dpi, colour);

	researchItem = gResearchItems;

	if (scrollIndex == 1) {
		// Skip pre-researched items
		for (; researchItem->entryIndex != RESEARCHED_ITEMS_SEPERATOR; researchItem++) { }
		researchItem++;
		researchItemEndMarker = RESEARCHED_ITEMS_END;
	} else {
		researchItemEndMarker = RESEARCHED_ITEMS_SEPERATOR;
	}

	itemY = 0;
	for (; researchItem->entryIndex != researchItemEndMarker; researchItem++, itemY += 10) {
		if (itemY + 10 < dpi->y || itemY >= dpi->y + dpi->height)
			continue;

		colour = 142;
		if (WindowHighlightedItem(w) == researchItem) {
			if (_editorInventionsListDraggedItem == NULL) {
				// Highlight
				top = itemY;
				bottom = itemY + 9;
			} else {
				// Drop horizontal rule
				top = itemY - 1;
				bottom = itemY;
			}
			gfx_fill_rect(dpi, 0, top, w->width, bottom, 0x2000031);

			if (_editorInventionsListDraggedItem == NULL)
				colour = 14;
		}

		if (researchItem->entryIndex == RESEARCHED_ITEMS_SEPERATOR || researchItem->entryIndex == RESEARCHED_ITEMS_END)
			continue;

		if (researchItem == _editorInventionsListDraggedItem)
			continue;

		disableItemMovement = research_item_is_always_researched(researchItem);
		stringId = research_item_get_name(researchItem->entryIndex & 0xFFFFFF);

		ptr = buffer;
		if (!disableItemMovement)
			*ptr++ = colour & 0xFF;

		format_string(ptr, stringId, NULL);

		if (disableItemMovement) {
			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = -1;
			if ((colour & 0xFF) == 14 && _editorInventionsListDraggedItem == NULL)
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = -2;
			colour = 64 | w->colours[1];
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = 224;
			colour = 0;
		}

		left = 1;
		top = itemY - 1;
		gfx_draw_string(dpi, buffer, colour, left, top);
	}
}

#pragma region Drag item

/**
 *
 *  rct2: 0x006852F4
 */
static void window_editor_inventions_list_drag_open(rct_research_item *researchItem)
{
	char buffer[256];
	int stringWidth;
	rct_string_id stringId;
	rct_window *w;

	window_close_by_class(WC_EDITOR_INVENTION_LIST_DRAG);
	_editorInventionsListDraggedItem = researchItem;

	stringId = research_item_get_name(researchItem->entryIndex & 0xFFFFFF);
	format_string(buffer, stringId, NULL);
	stringWidth = gfx_get_string_width(buffer);
	window_editor_inventions_list_drag_widgets[0].right = stringWidth;

	w = window_create(
		RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_X, uint16) - (stringWidth / 2),
		RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_Y, uint16) - 7,
		stringWidth,
		14,
		(uint32*)window_editor_inventions_list_drag_events,
		WC_EDITOR_INVENTION_LIST_DRAG,
		WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_SNAPPING
	);
	w->widgets = window_editor_inventions_list_drag_widgets;
	w->colours[1] = 2;
	input_window_position_begin(
		w, 0, RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_X, uint16), RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_Y, uint16)
	);
}

/**
 *
 *  rct2: 0x0068549C
 */
static void window_editor_inventions_list_drag_cursor()
{
	rct_window *w, *inventionListWindow;
	rct_research_item *researchItem;
	short widgetIndex, x, y;
	int cursorId;

	window_cursor_get_registers(w, widgetIndex, x, y);

	inventionListWindow = window_find_by_class(WC_EDITOR_INVENTION_LIST);
	if (inventionListWindow != NULL) {
		researchItem = get_research_item_at(x, y);
		if (researchItem != WindowHighlightedItem(inventionListWindow)) {
			WindowHighlightedItem(inventionListWindow) = researchItem;
			window_invalidate(inventionListWindow);
		}
	}

	cursorId = CURSOR_HAND_CLOSED;
	window_cursor_set_registers(cursorId);
}

/**
 *
 *  rct2: 0x00685412
 */
static void window_editor_inventions_list_drag_moved()
{
	rct_window *w;
	rct_research_item *researchItem;
	short x, y, widgetIndex;

	window_cursor_get_registers(w, widgetIndex, x, y);

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
static void window_editor_inventions_list_drag_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_string_id stringId;
	int x, y;

	window_paint_get_registers(w, dpi);

	x = w->x;
	y = w->y + 2;
	stringId = research_item_get_name(_editorInventionsListDraggedItem->entryIndex & 0xFFFFFF);
	gfx_draw_string_left(dpi, 1193, &stringId, 32, x, y);
}

#pragma endregion