#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../audio/audio.h"
#include "../cheats.h"
#include "../game.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../localisation/localisation.h"
#include "../rct2.h"
#include "../sprites.h"
#include "../world/footpath.h"
#include "../world/scenery.h"
#include "dropdown.h"

#pragma region MEASUREMENTS

#define WW						300
#define WH						240
#define PADDING					5
#define IMGBUTTON_SIZE			23
#define WIDGET_WIDTH			(WW - PADDING)
#define LIST_WIDTH				(WW - 2 * PADDING - IMGBUTTON_SIZE)
#define LIST_BUTTON_X			(LIST_WIDTH + PADDING)
#define BUTTON_HEIGHT			16
#define APPEND_BUTTON_LEFT		50
#define APPEND_BUTTON_RIGHT		(WW - APPEND_BUTTON_LEFT)
#define BUTTON_RIGHT			(PADDING + 250)
#define WIDGET_HEIGHT			10

#define DROPDOWN_RIGHT			(PADDING + 150)

#define YPL(ROW)	((sint16)(PADDING + (BUTTON_HEIGHT * ROW)))
#define HPL(ROW)	((sint16)(YPL(ROW) + BUTTON_HEIGHT))
#define OHPL(ROW)	((sint16)(YPL(ROW) + WIDGET_HEIGHT))
#define FHPL(ROW)	((sint16)(YPL(ROW) + IMGBUTTON_SIZE))

#pragma endregion

enum WINDOW_FOOTPATH_SCENERY_WIDGET_IDX
{
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,

	WIDX_RULE_LIST,

	WIDX_CLEAR_RULES,

	WIDX_OBJECT,
	WIDX_OBJECT_DROPDOWN,

	WIDX_ADD,
};

static rct_widget window_footpath_scenery_widgets[] = {
	{ WWT_FRAME,			0,	0,								WW - 1,					0,				WH - 1,			0xFFFFFFFF, 						STR_NONE },
	{ WWT_CAPTION,			0,	1,								WW - 2,					1,				14,				STR_FOOTPATH_SCENERY_TITLE,			STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	WW - 13,						WW - 3,					2,				13,				STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP },

	{ WWT_SCROLL,			1,	PADDING,						LIST_WIDTH,				YPL(1),			OHPL(9),		SCROLL_VERTICAL,					STR_NONE },
	{ WWT_FLATBTN,			1,	LIST_BUTTON_X,					WIDGET_WIDTH,			YPL(1),			FHPL(1),		SPR_DEMOLISH,						STR_FOOTPATH_SCENERY_DELETE_RULES },

	{ WWT_DROPDOWN, 		2,	PADDING,						DROPDOWN_RIGHT,			YPL(11),		OHPL(11),		STR_STRINGID,						STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	2,	DROPDOWN_RIGHT - 11,			DROPDOWN_RIGHT - 1,		YPL(11) + 1,	OHPL(11) - 1,	STR_DROPDOWN_GLYPH,					STR_NONE },

	{ WWT_CLOSEBOX,			1,	APPEND_BUTTON_LEFT,				APPEND_BUTTON_RIGHT,	YPL(13),		HPL(13),		STR_FOOTPATH_SCENERY_APPEND,		STR_NONE },

	{ WIDGETS_END },
};

static void window_footpath_scenery_close(rct_window * w);
static void window_footpath_scenery_mousedown(int widgetIndex, rct_window * w, rct_widget * widget);
static void window_footpath_scenery_mouseup(rct_window * w, int widgetIndex);
static void window_footpath_scenery_update(rct_window * w);
static void window_footpath_scenery_invalidate(rct_window * w);
static void window_footpath_scenery_list_scrollgetsize(rct_window * w, int scrollIndex, int * width, int * height);
static void window_footpath_scenery_paint(rct_window * w, rct_drawpixelinfo * dpi);
static void window_footpath_scenery_scrollpaint(rct_window * w, rct_drawpixelinfo * dpi, int scrollIndex);
static void window_footpath_scenery_list_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_footpath_scenery_dropdown(rct_window * w, int widgetIndex, int dropdownIndex);

static void window_footpath_scenery_path_scenery_dialog(rct_window * w, rct_widget * widget, int widgetIndex);
static void window_footpath_scenery_object_dropdown(rct_window * w, int widgetIndex, int dropdownIndex);
static void window_footpath_scenery_clear_rules(rct_window * w);
static void window_footpath_scenery_update_listselection(int index);

static void window_footpath_scenery_add(rct_window * w);

static rct_window_event_list window_footpath_scenery_events = {
	window_footpath_scenery_close,
	window_footpath_scenery_mouseup,
	NULL,
	window_footpath_scenery_mousedown,
	window_footpath_scenery_dropdown,
	NULL,
	window_footpath_scenery_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_footpath_scenery_list_scrollgetsize,
	window_footpath_scenery_list_scrollmousedown,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_footpath_scenery_invalidate,
	window_footpath_scenery_paint,
	window_footpath_scenery_scrollpaint
};

static uint64 window_footpath_scenery_enabled_widgets = {
  (1ULL << WIDX_CLOSE) | (1ULL << WIDX_RULE_LIST) |
  (1ULL << WIDX_CLEAR_RULES) | (1ULL << WIDX_OBJECT) |
  (1ULL << WIDX_OBJECT_DROPDOWN) | (1ULL << WIDX_ADD)
};

typedef struct path_rule_entry
{
	rct_scenery_entry *		 sceneEntry;
	uint16					 sceneryId;
	struct path_rule_entry * next;
} path_rule_entry_t;

static path_rule_entry_t * _footpath_scenery_list				   = NULL;
static path_rule_entry_t * _footpath_scenery_selected_listEntry	   = NULL;
static rct_scenery_entry * _footpath_scenery_selected_sceneryEntry = NULL;
static uint16			   _footpath_scenery_selected_sceneryId	   = 0;

void window_footpath_scenery_open()
{
	rct_window * window = window_bring_to_front_by_class(WC_FOOTPATH_SCENERY);
	if (window != NULL)
		return;

	window = window_create_auto_pos(WW, WH, &window_footpath_scenery_events, WC_FOOTPATH_SCENERY, 0);

	window->widgets			   = window_footpath_scenery_widgets;
	window->enabled_widgets	   = window_footpath_scenery_enabled_widgets;
	window->no_list_items	   = 0;
	window->selected_list_item = -1;

	window_init_scroll_widgets(window);
}

static void window_footpath_scenery_close(rct_window * w)
{
	window_footpath_scenery_clear_rules(w);
}

static void window_footpath_scenery_mouseup(rct_window * w, int widgetIndex)
{
	switch (widgetIndex)
	{
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_CLEAR_RULES:
		window_footpath_scenery_clear_rules(w);
		break;
	case WIDX_ADD:
		window_footpath_scenery_add(w);
		break;
	}
}

static void window_footpath_scenery_mousedown(int widgetIndex, rct_window * w, rct_widget * widget)
{
	switch (widgetIndex)
	{
	case WIDX_OBJECT_DROPDOWN:
		window_footpath_scenery_path_scenery_dialog(w, widget - 1, widgetIndex);
		break;
	}
}

static void window_footpath_scenery_dropdown(rct_window * w, int widgetIndex, int dropdownIndex)
{
	if (widgetIndex == WIDX_OBJECT_DROPDOWN)
	{
		window_footpath_scenery_object_dropdown(w, widgetIndex, dropdownIndex);
	}
}

static void window_footpath_scenery_update(rct_window * w)
{
	window_invalidate(w);
}

static void window_footpath_scenery_invalidate(rct_window * w)
{
	colour_scheme_update(w);

	if (_footpath_scenery_selected_sceneryEntry != NULL)
	{
		set_format_arg(0, rct_string_id, _footpath_scenery_selected_sceneryEntry->name);
	}
	else
	{
		set_format_arg(0, rct_string_id, STR_FOOTPATH_SCENERY_NOTHING);
	}
}

static void window_footpath_scenery_paint(rct_window * w, rct_drawpixelinfo * dpi)
{
	window_draw_widgets(w, dpi);

	gfx_draw_string_left(dpi, STR_FOOTPATH_SCENERY_ON_PATH, NULL, COLOUR_BLACK, w->x + PADDING, w->y + YPL(10) + 3);
}

static void window_footpath_scenery_path_scenery_dialog(rct_window * w, rct_widget * widget, int widgetIndex)
{
	int numSceneryTypes		= 1;
	gDropdownItemsFormat[0] = -1;
	gDropdownItemsArgs[0]	= SPR_NONE;

	for (uint16 sceneryId = SCENERY_PATHSCENERY_ID_MIN; sceneryId < SCENERY_PATHSCENERY_ID_MAX; sceneryId++)
	{
		int pathBitIndex = sceneryId - SCENERY_PATHSCENERY_ID_MIN;

		if (get_footpath_item_entry(pathBitIndex) == (rct_scenery_entry *)-1)
			continue;

		if (!scenery_is_invented(sceneryId))
			continue;

		rct_scenery_entry * sceneryEntry = get_footpath_item_entry(pathBitIndex);

		gDropdownItemsFormat[numSceneryTypes] = -1;
		gDropdownItemsArgs[numSceneryTypes]	  = sceneryEntry->image;
		numSceneryTypes++;
	}

	window_dropdown_show_image(w->x + widget->left, w->y + widget->top, widget->bottom - widget->top + 1, w->colours[1], 0,
							   numSceneryTypes, 50, 50, gAppropriateImageDropdownItemsPerRow[numSceneryTypes]);
	return;
}

static void window_footpath_scenery_object_dropdown(rct_window * w, int widgetIndex, int dropdownIndex)
{
	int numSceneryTypes = 0;
	if (dropdownIndex == 0)
	{
		_footpath_scenery_selected_sceneryEntry = NULL;
		_footpath_scenery_selected_sceneryId	= 0;
	}
	else if (dropdownIndex != -1)
	{
		dropdownIndex--;
		uint16 sceneryId;

		for (sceneryId = SCENERY_PATHSCENERY_ID_MIN; sceneryId < SCENERY_PATHSCENERY_ID_MAX; sceneryId++)
		{
			int pathBitIndex = sceneryId - SCENERY_PATHSCENERY_ID_MIN;

			if (get_footpath_item_entry(pathBitIndex) == (rct_scenery_entry *)-1)
				continue;

			if (!scenery_is_invented(sceneryId))
				continue;

			_footpath_scenery_selected_sceneryEntry = get_footpath_item_entry(pathBitIndex);
			_footpath_scenery_selected_sceneryId	= sceneryId;

			if (dropdownIndex == numSceneryTypes)
				break;
			numSceneryTypes++;
		}
	}

	window_invalidate(w);
}

static void window_footpath_scenery_scrollpaint(rct_window * w, rct_drawpixelinfo * dpi, int scrollIndex)
{
	int y = 0, i = 0;
	rct_string_id format = STR_BLACK_STRING;

	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height, ColourMapA[w->colours[1]].mid_light);

	path_rule_entry_t * c = _footpath_scenery_list;
	while (c != NULL)
	{
		if (i == w->selected_list_item)
		{
			gfx_filter_rect(dpi, 0, y, LIST_WIDTH, y + 9, PALETTE_DARKEN_1);
		}

		if (c->sceneEntry != NULL)
		{
			gfx_draw_string_left_clipped(dpi, format, &c->sceneEntry->name, COLOUR_BLACK, 0, y - 1, LIST_WIDTH);
		}
		else
		{
			rct_string_id nothing = STR_FOOTPATH_SCENERY_NOTHING;
			gfx_draw_string_left_clipped(dpi, format, &nothing, COLOUR_BLACK, 0, y - 1, LIST_WIDTH);
		}
		y += 10;
		i++;
		c = c->next;
	}
}

static void window_footpath_scenery_list_scrollgetsize(rct_window * w, int scrollIndex, int * width, int * height)
{
	int top;

	*height = w->no_list_items * 10;

	top = *height - window_footpath_scenery_widgets[WIDX_RULE_LIST].bottom +
		  window_footpath_scenery_widgets[WIDX_RULE_LIST].top + 21;
	if (top < 0)
		top = 0;
	if (top < w->scrolls[0].v_top)
	{
		w->scrolls[0].v_top = top;
		window_invalidate(w);
	}
}

static void window_footpath_scenery_list_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	int index;

	index = y / 10;
	if (index >= w->no_list_items)
		return;

	w->selected_list_item = index;
	window_footpath_scenery_update_listselection(index);
}

static void window_footpath_scenery_update_listselection(int index)
{
	path_rule_entry_t * selected = _footpath_scenery_list;
	int i;

	for (i = 0; i < index; i++)
	{
		if (selected == NULL) {
			_footpath_scenery_selected_listEntry = _footpath_scenery_list;
			return;
		}
		selected = selected->next;
	}
	_footpath_scenery_selected_listEntry = selected;
}

static void window_footpath_scenery_add(rct_window * w)
{
	int i = 0;

	if (w->selected_list_item == -1)
	{
		// Append
		path_rule_entry_t * add_to = malloc(sizeof(path_rule_entry_t));
		add_to->sceneEntry		   = _footpath_scenery_selected_sceneryEntry;
		add_to->sceneryId		   = _footpath_scenery_selected_sceneryId;
		add_to->next			   = NULL;

		if (_footpath_scenery_list == NULL)
		{
			_footpath_scenery_list				 = add_to;
			w->selected_list_item				 = 0;
			_footpath_scenery_selected_listEntry = _footpath_scenery_list;
		}
		else
		{
			path_rule_entry_t * last = _footpath_scenery_list;
			while (last->next != NULL)
				last   = last->next;
			last->next = add_to;
		}
	}
	else
	{
		// Append after selected
		path_rule_entry_t * add_to = malloc(sizeof(path_rule_entry_t));
		add_to->sceneEntry		   = _footpath_scenery_selected_sceneryEntry;
		add_to->sceneryId		   = _footpath_scenery_selected_sceneryId;

		path_rule_entry_t * after = _footpath_scenery_list;
		while (i != w->selected_list_item)
		{
			after = after->next;
			i++;
			if (after == NULL)
				break;
		}

		if (after != NULL)
		{
			add_to->next = after->next;
			after->next	 = add_to;
			w->selected_list_item++;
			_footpath_scenery_selected_listEntry = add_to;
		}
		else
		{
			free(add_to);
		}
	}

	// _footpath_scenery_selected_sceneryEntry = NULL;
	w->no_list_items++;
	window_invalidate(w);
}

static void window_footpath_scenery_clear_rules(rct_window * w)
{
	path_rule_entry_t * c = _footpath_scenery_list;
	while (c != NULL)
	{
		path_rule_entry_t * next = c->next;
		free(c);
		c = next;
	}

	_footpath_scenery_list					= NULL;
	_footpath_scenery_selected_sceneryEntry = NULL;
	_footpath_scenery_selected_listEntry	= NULL;
	_footpath_scenery_selected_sceneryId	= 0;

	w->no_list_items	  = 0;
	w->selected_list_item = -1;
	window_invalidate(w);
}

uint16 window_footpath_scenery_get_next_scenery()
{
	if (_footpath_scenery_selected_listEntry == NULL)
		return 0;

	return _footpath_scenery_selected_listEntry->sceneryId;
}

void window_footpath_scenery_advance()
{
	if (_footpath_scenery_selected_listEntry == NULL)
		return;

	rct_window * w = window_find_by_class(WC_FOOTPATH_SCENERY);
	if (w == NULL)
		return;

	if (_footpath_scenery_selected_listEntry->next != NULL)
	{
		_footpath_scenery_selected_listEntry = _footpath_scenery_selected_listEntry->next;
		w->selected_list_item++;
	}
	else
	{
		_footpath_scenery_selected_listEntry = _footpath_scenery_list;
		w->selected_list_item = 0;
	}

	window_invalidate(w);
}
