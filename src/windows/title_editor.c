/*****************************************************************************
* Copyright (c) 2014 Maciek Baron, Dániel Tar
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
#include "../config.h"
#include "../game.h"
#include "../drawing/drawing.h"
#include "../input.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../world/sprite.h"
#include "dropdown.h"
#include "../interface/themes.h"
#include "../sprites.h"
#include "../title.h"
#include "../interface/title_sequences.h"
#include "error.h"
#include "../scenario.h"
#include "../util/util.h"

enum {
	WINDOW_TITLE_EDITOR_TAB_PRESETS,
	WINDOW_TITLE_EDITOR_TAB_SAVES,
	WINDOW_TITLE_EDITOR_TAB_SCRIPT,
	WINDOW_TITLE_EDITOR_TAB_COUNT
} WINDOW_TITLE_EDITOR_TAB;

static void window_title_editor_emptysub() { }
static void window_title_editor_close();
static void window_title_editor_mouseup();
static void window_title_editor_resize();
static void window_title_editor_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_title_editor_dropdown();
static void window_title_editor_update(rct_window *w);
static void window_title_editor_scrollgetsize();
static void window_title_editor_scrollmousedown();
static void window_title_editor_scrollmouseover();
static void window_title_editor_textinput();
static void window_title_editor_tooltip();
static void window_title_editor_invalidate();
static void window_title_editor_paint();
static void window_title_editor_scrollpaint();
static void window_title_editor_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

static void* window_title_editor_events[] = {
	window_title_editor_close,
	window_title_editor_mouseup,
	window_title_editor_resize,
	window_title_editor_mousedown,
	window_title_editor_dropdown,
	window_title_editor_emptysub,
	window_title_editor_update,
	window_title_editor_emptysub,
	window_title_editor_emptysub,
	window_title_editor_emptysub,
	window_title_editor_emptysub,
	window_title_editor_emptysub,
	window_title_editor_emptysub,
	window_title_editor_emptysub,
	window_title_editor_emptysub,
	window_title_editor_scrollgetsize,
	window_title_editor_scrollmousedown,
	window_title_editor_emptysub,
	window_title_editor_scrollmouseover,
	window_title_editor_textinput,
	window_title_editor_emptysub,
	window_title_editor_emptysub,
	window_title_editor_tooltip,
	window_title_editor_emptysub,
	window_title_editor_emptysub,
	window_title_editor_invalidate,
	window_title_editor_paint,
	window_title_editor_scrollpaint,
};

enum WINDOW_TITLE_EDITOR_WIDGET_IDX {
	WIDX_TITLE_EDITOR_BACKGROUND,
	WIDX_TITLE_EDITOR_TITLE,
	WIDX_TITLE_EDITOR_CLOSE,
	WIDX_TITLE_EDITOR_TAB_CONTENT_PANEL,
	WIDX_TITLE_EDITOR_PRESETS_TAB,
	WIDX_TITLE_EDITOR_SAVES_TAB,
	WIDX_TITLE_EDITOR_SCRIPT_TAB,
	WIDX_TITLE_EDITOR_LIST,
	
	// Presets Tab
	WIDX_TITLE_EDITOR_PRESETS,
	WIDX_TITLE_EDITOR_PRESETS_DROPDOWN,
	WIDX_TITLE_EDITOR_DUPLICATE_BUTTON,
	WIDX_TITLE_EDITOR_DELETE_BUTTON,
	WIDX_TITLE_EDITOR_RENAME_BUTTON,

	// Saves Tab
	WIDX_TITLE_EDITOR_ADD,
	WIDX_TITLE_EDITOR_REMOVE,
	WIDX_TITLE_EDITOR_RENAME,
	WIDX_TITLE_EDITOR_LOAD,

	// Script Tab
	WIDX_TITLE_EDITOR_INSERT,
	WIDX_TITLE_EDITOR_EDIT,
	WIDX_TITLE_EDITOR_DELETE,
	WIDX_TITLE_EDITOR_DISABLE,
	WIDX_TITLE_EDITOR_SKIP_TO,
	
	WIDX_TITLE_EDITOR_MOVE_DOWN,
	WIDX_TITLE_EDITOR_MOVE_UP,

	WIDX_TITLE_EDITOR_REPLAY,
	WIDX_TITLE_EDITOR_PLAY,
	WIDX_TITLE_EDITOR_PAUSE,
	WIDX_TITLE_EDITOR_SKIP,
};

// Increase BW if certain launguages do not fit
// BW should be a multiple of 4
#define WW 320
#define WH 270
#define BX 8
#define BW 72
#define BY 52
#define BH 63
#define BS 18
#define ROW_HEIGHT 11
#define SCROLL_WIDTH 300

static rct_widget window_title_editor_widgets[] = {
	{ WWT_FRAME,			0,	0,		WW-1,	0,		WH-1,	0x0FFFFFFFF,					STR_NONE },						// panel / background
	{ WWT_CAPTION,			0,	1,		WW-2,	1,		14,		STR_STAFF,						STR_WINDOW_TITLE_TIP },			// title bar
	{ WWT_CLOSEBOX,			0,	WW-13,	WW-3,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },			// close button
	{ WWT_RESIZE,			1,	0,		WW-1,	43,		WH-1,	0x0FFFFFFFF,					STR_NONE },						// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x02000144E,					5235 },							// presets tab
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x02000144E,					5371 },							// saves tab
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x02000144E,					5372 },							// script tab
	{ WWT_SCROLL,			1,	BX+BW+9,WW-4,	48,		WH-4,	3,								STR_NONE },						// command/save list
	
	// Presets Tab
	{ WWT_DROPDOWN,			1,	125,	299,	60,		71,		STR_NONE,						STR_NONE },						// Preset title sequences
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	61,		70,		876,							STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	10,		100,	82,		93,		5239,							STR_NONE },						// Duplicate button
	{ WWT_DROPDOWN_BUTTON,	1,	110,	200,	82,		93,		3349,							STR_NONE },						// Delete button
	{ WWT_DROPDOWN_BUTTON,	1,	210,	300,	82,		93,		3348,							STR_NONE },						// Rename button
	
	// Saves Tab
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY,			BH,			5373,					STR_NONE }, // Add
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*1),	BH+(BS*1),	5374,					STR_NONE }, // Remove
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*2),	BH+(BS*2),	3348,					STR_NONE }, // Rename
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*3),	BH+(BS*3),	5375,					STR_NONE }, // Load

	// Script Tab
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY,			BH,			5376,					STR_NONE }, // Insert
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*1),	BH+(BS*1),	5377,					STR_NONE }, // Edit
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*2),	BH+(BS*2),	3349,					STR_NONE }, // Delete
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*3),	BH+(BS*3),	5378,					STR_NONE }, // Disable
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*4),	BH+(BS*4),	5380,					STR_NONE }, // Skip to

	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW/2-1,BY+(BS*6),BH+(BS*6),	5382,					STR_NONE }, // Move down
	{ WWT_DROPDOWN_BUTTON,	1,	BX+BW/2,BX+BW-1,BY+(BS*6),	BH+(BS*6),	5381,					STR_NONE }, // Move up

	{ WWT_IMGBTN,			1,	BX,		BX+BW/4-1,	WH-32,	WH-16,		STR_NONE,				STR_NONE }, // Replay
	{ WWT_IMGBTN,			1,	BX+BW/4,BX+BW/2-1,	WH-32,	WH-16,		STR_NONE,				STR_NONE }, // Pause
	{ WWT_IMGBTN,			1,	BX+BW/2,BX+BW*3/4-1,WH-32,	WH-16,		STR_NONE,				STR_NONE }, // Play
	{ WWT_IMGBTN,			1,	BX+BW*3/4,BX+BW,	WH-32,	WH-16,		STR_NONE,				STR_NONE }, // Skip

	{ WIDGETS_END },
};

static sint16 _window_title_editor_highlighted_index;


static int window_title_editor_tab_animation_loops[] = {
	64,
	1,
	14
};
static int window_title_editor_tab_animation_divisor[] = {
	4,
	1,
	2
};
static int window_title_editor_tab_sprites[] = {
	SPR_TAB_RIDE_0,
	5183,
	SPR_TAB_STATS_0
};

static void window_title_editor_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	int sprite_idx;
	int x, y;

	for (int i = 0; i < WINDOW_TITLE_EDITOR_TAB_COUNT; i++) {
		x = 0;
		y = 0;
		sprite_idx = window_title_editor_tab_sprites[i];
		if (w->selected_tab == i)
			sprite_idx += w->frame_no / window_title_editor_tab_animation_divisor[w->selected_tab];
		if (i == 1) {
			x = 4;
			y = 1;
		}
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TITLE_EDITOR_PRESETS_TAB + i].left + x, w->y + w->widgets[WIDX_TITLE_EDITOR_PRESETS_TAB + i].top + y, 0);
	}
}

void window_title_editor_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_TITLE_EDITOR);
	if (window != NULL)
		return;

	window = window_create_auto_pos(WW, WH, (uint32*)window_title_editor_events, WC_TITLE_EDITOR, WF_10 | WF_RESIZABLE);
	window->widgets = window_title_editor_widgets;
	window->enabled_widgets =
		(1 << WIDX_TITLE_EDITOR_CLOSE) |
		(1 << WIDX_TITLE_EDITOR_PRESETS_TAB) |
		(1 << WIDX_TITLE_EDITOR_SAVES_TAB) |
		(1 << WIDX_TITLE_EDITOR_SCRIPT_TAB) |

		(1 << WIDX_TITLE_EDITOR_PRESETS) |
		(1 << WIDX_TITLE_EDITOR_PRESETS_DROPDOWN) |
		(1 << WIDX_TITLE_EDITOR_DUPLICATE_BUTTON) |
		(1 << WIDX_TITLE_EDITOR_DELETE_BUTTON) |
		(1 << WIDX_TITLE_EDITOR_RENAME_BUTTON) |

		(1 << WIDX_TITLE_EDITOR_ADD) |
		(1 << WIDX_TITLE_EDITOR_REMOVE) |
		(1 << WIDX_TITLE_EDITOR_RENAME) |
		(1 << WIDX_TITLE_EDITOR_LOAD) |

		(1 << WIDX_TITLE_EDITOR_INSERT) |
		(1 << WIDX_TITLE_EDITOR_EDIT) |
		(1 << WIDX_TITLE_EDITOR_DELETE) |
		(1 << WIDX_TITLE_EDITOR_DISABLE) |
		(1 << WIDX_TITLE_EDITOR_SKIP_TO) |
		(1 << WIDX_TITLE_EDITOR_MOVE_DOWN) |
		(1 << WIDX_TITLE_EDITOR_MOVE_UP) |

		(1 << WIDX_TITLE_EDITOR_PLAY) |
		(1 << WIDX_TITLE_EDITOR_PAUSE) |
		(1 << WIDX_TITLE_EDITOR_REPLAY) |
		(1 << WIDX_TITLE_EDITOR_SKIP);

	window_init_scroll_widgets(window);
	window->list_information_type = 0;

	window->selected_tab = WINDOW_TITLE_EDITOR_TAB_PRESETS;
	window->selected_list_item = -1;
	_window_title_editor_highlighted_index = -1;

	window->min_width = WW;
	window->min_height = WH;
	window->max_width = 500;
	window->max_height = 450;

}

void window_title_editor_close() {
	rct_window *w;

	window_get_register(w);
}

static void window_title_editor_mouseup()
{
	short widgetIndex;
	rct_window *w;
	uint16 newStaffId;
	char path[MAX_PATH];
	char separator = platform_get_path_separator();

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_TITLE_EDITOR_CLOSE:
		window_close(w);
		break;
	case WIDX_TITLE_EDITOR_DUPLICATE_BUTTON:
		window_text_input_open(w, widgetIndex, 5239, 5240, 1170, (uint32)&gConfigTitleSequences.presets[gCurrentTitleSequence].name, 64);
		break;
	case WIDX_TITLE_EDITOR_DELETE_BUTTON:
		if (gCurrentTitleSequence >= TITLE_SEQUENCE_DEFAULT_PRESETS) {
			title_sequence_delete_preset(gCurrentTitleSequence);
		}
		else {
			window_error_open(5241, STR_NONE);
		}
		break;
	case WIDX_TITLE_EDITOR_RENAME_BUTTON:
		if (gCurrentTitleSequence >= TITLE_SEQUENCE_DEFAULT_PRESETS) {
			window_text_input_open(w, widgetIndex, 3348, 5240, 1170, (uint32)&gConfigTitleSequences.presets[gCurrentTitleSequence].name, 64);
		}
		else {
			window_error_open(5241, STR_NONE);
		}
		break;
	case WIDX_TITLE_EDITOR_ADD:
		window_loadsave_open(LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME, NULL);
		gLoadSaveTitleSequenceSave = true;
		break;
	case WIDX_TITLE_EDITOR_REMOVE:
		if (w->selected_list_item != -1) {
			title_sequence_remove_save(gCurrentTitleSequence, w->selected_list_item);
		}
		break;
	case WIDX_TITLE_EDITOR_RENAME:
		if (w->selected_list_item != -1) {
			
		}
		break;
	case WIDX_TITLE_EDITOR_LOAD:
		if (w->selected_list_item != -1) {
			if (gConfigTitleSequences.presets[gCurrentTitleSequence].path[0]) {
				strcpy(path, gConfigTitleSequences.presets[gCurrentTitleSequence].path);
			}
			else {
				platform_get_user_directory(path, "title sequences");
				strcat(path, gConfigTitleSequences.presets[gCurrentTitleSequence].name);
				strncat(path, &separator, 1);
			}

			strcat(path, gConfigTitleSequences.presets[gCurrentTitleSequence].saves[w->selected_list_item]);
			game_load_save(path);
		}
		break;
	}
}

static void window_title_editor_resize()
{
	rct_window *w;

	window_get_register(w);
	
	if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_PRESETS) {
		w->min_width = WW;
		w->min_height = 107;
		w->max_width = WW;
		w->max_height = 107;

		if (w->width < w->min_width) {
			w->width = w->min_width;
			gfx_invalidate_screen();
		}
		if (w->height < w->min_height) {
			w->height = w->min_height;
			gfx_invalidate_screen();
		}
		if (w->width > w->max_width) {
			w->width = w->max_width;
			gfx_invalidate_screen();
		}
		if (w->height > w->max_height) {
			w->height = w->max_height;
			gfx_invalidate_screen();
		}
	}
	else {
		w->min_width = WW;
		w->min_height = WH;
		w->max_width = 320;
		w->max_height = 450;

		if (w->width < w->min_width) {
			w->width = w->min_width;
			window_invalidate(w);
		}
		if (w->height < w->min_height) {
			w->height = w->min_height;
			window_invalidate(w);
		}
		if (w->width > w->max_width) {
			w->width = w->max_width;
			window_invalidate(w);
		}
		if (w->height > w->max_height) {
			w->height = w->max_height;
			window_invalidate(w);
		}
	}
}

static void window_title_editor_mousedown(int widgetIndex, rct_window* w, rct_widget* widget)
{
	short newSelectedTab;
	int num_items, i;

	switch (widgetIndex) {
	case WIDX_TITLE_EDITOR_PRESETS_TAB:
	case WIDX_TITLE_EDITOR_SAVES_TAB:
	case WIDX_TITLE_EDITOR_SCRIPT_TAB:
		newSelectedTab = widgetIndex - WIDX_TITLE_EDITOR_PRESETS_TAB;
		if (w->selected_tab == newSelectedTab)
			break;
		w->selected_tab = newSelectedTab;
		w->selected_list_item = -1;
		_window_title_editor_highlighted_index = -1;
		window_invalidate(w);
		w->scrolls[0].v_top = 0;
		break;
	case WIDX_TITLE_EDITOR_PRESETS_DROPDOWN:
		num_items = gConfigTitleSequences.num_presets;

		widget--;
		for (i = 0; i < num_items; i++) {
			gDropdownItemsFormat[i] = 2777;
			gDropdownItemsArgs[i] = (uint64)&gConfigTitleSequences.presets[i].name;
		}

		window_dropdown_show_text_custom_width(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			num_items,
			widget->right - widget->left - 3
			);

		gDropdownItemsChecked = 1 << (gCurrentTitleSequence);
		break;
	}
}

static void window_title_editor_dropdown()
{
	rct_window* w;
	short widgetIndex, dropdownIndex;
	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);
	
	switch (widgetIndex) {
	case WIDX_TITLE_EDITOR_PRESETS_DROPDOWN:
		gCurrentTitleSequence = dropdownIndex;
		window_invalidate(w);
		break;
	}
}

void window_title_editor_update(rct_window *w)
{
	w->frame_no++;
	if (w->frame_no >= window_title_editor_tab_animation_loops[w->selected_tab])
		w->frame_no = 0;

	widget_invalidate(w, WIDX_TITLE_EDITOR_PRESETS_TAB + w->selected_tab);
}

void window_title_editor_scrollgetsize()
{
	int i, width, height, spriteIndex;
	uint16 lineCount;
	rct_window *w;

	window_get_register(w);

	lineCount = 1;
	if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_SAVES)
		lineCount = gConfigTitleSequences.presets[gCurrentTitleSequence].num_saves;
	else if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_SCRIPT)
		lineCount = gConfigTitleSequences.presets[gCurrentTitleSequence].num_commands;
	
	height = lineCount * ROW_HEIGHT;
	i = height - window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].bottom + window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].top + 21;
	if (i < 0)
		i = 0;
	if (i < w->scrolls[0].v_top) {
		w->scrolls[0].v_top = i;
		window_invalidate(w);
	}

	width = SCROLL_WIDTH;
	window_scrollsize_set_registers(width, height);
}

void window_title_editor_scrollmousedown() {
	int i, index;
	short x, y, scrollIndex;
	rct_window *w;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	index = y / ROW_HEIGHT;
	w->selected_list_item = -1;
	switch (w->selected_tab) {
	case WINDOW_TITLE_EDITOR_TAB_SAVES:
		if (index < gConfigTitleSequences.presets[gCurrentTitleSequence].num_saves) {
			w->selected_list_item = index;
			widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
		}
		break;
	case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
		if (index < gConfigTitleSequences.presets[gCurrentTitleSequence].num_commands) {
			w->selected_list_item = index;
			widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
		}
		break;
	}
}

void window_title_editor_scrollmouseover() {
	int i, index;
	short x, y, scrollIndex;
	rct_window *w;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);
	
	index = y / ROW_HEIGHT;
	_window_title_editor_highlighted_index = -1;
	switch (w->selected_tab) {
	case WINDOW_TITLE_EDITOR_TAB_SAVES:
		if (index < gConfigTitleSequences.presets[gCurrentTitleSequence].num_saves) {
			_window_title_editor_highlighted_index = index;
			widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
		}
		break;
	case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
		if (index < gConfigTitleSequences.presets[gCurrentTitleSequence].num_commands) {
			_window_title_editor_highlighted_index = index;
			widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
		}
		break;
	}
}

static void window_title_editor_textinput()
{
	
	rct_window *w;
	short widgetIndex;
	uint8 result;
	char *text;

	window_textinput_get_registers(w, widgetIndex, result, text);

	if (!result || text[0] == 0)
		return;

	switch (widgetIndex) {
	case WIDX_TITLE_EDITOR_DUPLICATE_BUTTON:
	case WIDX_TITLE_EDITOR_RENAME_BUTTON:
		if (filename_valid_characters(text)) {
			if (!title_sequence_name_exists(text)) {
				if (widgetIndex == WIDX_TITLE_EDITOR_DUPLICATE_BUTTON) {
					title_sequence_create_preset(gCurrentTitleSequence, text);
				}
				else {
					title_sequence_rename_preset(gCurrentTitleSequence, text);
				}
				config_save_default();
				window_invalidate(w);
			}
			else {
				window_error_open(5243, STR_NONE);
			}
		}
		else {
			window_error_open(5243, STR_NONE);
		}
		break;
	case WIDX_TITLE_EDITOR_RENAME:
		if (filename_valid_characters(text)) {
			if (!title_sequence_save_exists(gCurrentTitleSequence, text)) {
				title_sequence_rename_save(gCurrentTitleSequence, w->selected_list_item, text);
				title_sequence_save_preset_script(gCurrentTitleSequence);
				window_invalidate(w);
			}
			else {
				window_error_open(5243, STR_NONE);
			}
		}
		else {
			window_error_open(5243, STR_NONE);
		}
		break;
	}
}

void window_title_editor_tooltip()
{
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

void window_title_editor_invalidate()
{
	rct_window *w;

	window_get_register(w);
	colour_scheme_update(w);

	int pressed_widgets = w->pressed_widgets & 0xFFFFFF8F;
	uint8 widgetIndex = w->selected_tab + 4;

	w->pressed_widgets = pressed_widgets | (1 << widgetIndex);
	
	window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WWT_EMPTY;

	window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_DUPLICATE_BUTTON].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE_BUTTON].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_BUTTON].type = WWT_EMPTY;

	window_title_editor_widgets[WIDX_TITLE_EDITOR_ADD].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_REMOVE].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_LOAD].type = WWT_EMPTY;

	window_title_editor_widgets[WIDX_TITLE_EDITOR_INSERT].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_EDIT].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_DISABLE].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP_TO].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_UP].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_DOWN].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_PAUSE].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].type = WWT_EMPTY;
	
	switch (w->selected_tab) {
	case WINDOW_TITLE_EDITOR_TAB_PRESETS:
		window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].type = WWT_DROPDOWN;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_DUPLICATE_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_BUTTON].type = WWT_DROPDOWN_BUTTON;
		break;
	case WINDOW_TITLE_EDITOR_TAB_SAVES:
		window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WWT_SCROLL;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_ADD].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_REMOVE].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_LOAD].type = WWT_DROPDOWN_BUTTON;
		break;
	case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
		window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WWT_SCROLL;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_INSERT].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_EDIT].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_DISABLE].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP_TO].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_UP].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_DOWN].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].type = WWT_IMGBTN;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_PAUSE].type = WWT_IMGBTN;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].type = WWT_IMGBTN;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].type = WWT_IMGBTN;
		break;
	}
	
	window_title_editor_widgets[WIDX_TITLE_EDITOR_BACKGROUND].right = w->width - 1;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_BACKGROUND].bottom = w->height - 1;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_TAB_CONTENT_PANEL].right = w->width - 1;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_TAB_CONTENT_PANEL].bottom = w->height - 1;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_TITLE].right = w->width - 2;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_CLOSE].left = w->width - 2 - 0x0B;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_CLOSE].right = w->width - 2 - 0x0B + 0x0A;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].right = w->width - 4;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].bottom = w->height - 16;
}

void window_title_editor_paint() {
	int i;
	uint8 selectedTab;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	// Widgets
	window_draw_widgets(w, dpi);
	window_title_editor_draw_tab_images(dpi, w);

	// Draw strings
	switch (w->selected_tab) {
	case WINDOW_TITLE_EDITOR_TAB_PRESETS:

		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = (uint32)&gConfigTitleSequences.presets[gCurrentTitleSequence].name;
		gfx_draw_string_left(dpi, 5238, NULL, w->colours[1], w->x + 10, w->y + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].top + 1);
		gfx_draw_string_left_clipped(
			dpi,
			1170,
			(void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS,
			w->colours[1],
			w->x + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].left + 1,
			w->y + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].top,
			w->x + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].left - window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].left - 4
			);

		break;
	case WINDOW_TITLE_EDITOR_TAB_SAVES:

		break;
	case WINDOW_TITLE_EDITOR_TAB_SCRIPT:

		break;
	}
}

void window_title_editor_scrollpaint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	char buffer[256];
	bool selected, hover, error;
	int y, x, x2, width;

	window_paint_get_registers(w, dpi);

	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, ((char*)0x0141FC48)[w->colours[1] * 8]);

	title_sequence *title = &gConfigTitleSequences.presets[gCurrentTitleSequence];

	y = 0; x = 0; x2 = 0; width = 0;
	width = w->widgets[WIDX_TITLE_EDITOR_LIST].right - w->widgets[WIDX_TITLE_EDITOR_LIST].left;
	if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_SAVES) {

		for (int i = 0; i < title->num_saves; i++, y += ROW_HEIGHT) {
			selected = false;
			hover = false;
			if (i == w->selected_list_item) {
				selected = true;
				gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH, y + ROW_HEIGHT - 1, RCT2_GLOBAL(0x0141FC46 + (w->colours[1] * 8), uint8));
			}
			else if (i == _window_title_editor_highlighted_index) {
				hover = true;
				gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH, y + ROW_HEIGHT - 1, RCT2_GLOBAL(0x0141FC47 + (w->colours[1] * 8), uint8));
			}
			else if (i & 1) {
				gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH, y + ROW_HEIGHT - 1, RCT2_GLOBAL(0x0141FC4A + (w->colours[1] * 8), uint8) | 0x1000000);
			}
			
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = (uint32)&title->saves[i];
			if (selected || hover) {
				format_string(buffer, 1170, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);
			}
			else {
				format_string(buffer + 1, 1170, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);
				buffer[0] = FORMAT_BLACK;
			}
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = &buffer;
			gfx_draw_string_left(dpi, 1170, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, w->colours[1], x + 5, y);
		}
	}
	else if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_SCRIPT) {

		x2 = 92;
		for (int i = 0; i < title->num_commands; i++, y += ROW_HEIGHT) {
			title_command *command = &title->commands[i];
			selected = false;
			hover = false;
			error = false;
			if (i == w->selected_list_item) {
				selected = true;
				gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH, y + ROW_HEIGHT - 1, RCT2_GLOBAL(0x0141FC46 + (w->colours[1] * 8), uint8));
			}
			else if (i == _window_title_editor_highlighted_index) {
				hover = true;
				gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH, y + ROW_HEIGHT - 1, RCT2_GLOBAL(0x0141FC47 + (w->colours[1] * 8), uint8));
			}
			else if (i & 1) {
				gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH, y + ROW_HEIGHT - 1, RCT2_GLOBAL(0x0141FC4A + (w->colours[1] * 8), uint8) | 0x1000000);
			}

			rct_string_id commandName;
			switch (command->command) {
			case TITLE_SCRIPT_WAIT:
				commandName = 5385;
				RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = command->seconds;
				break;
			case TITLE_SCRIPT_LOADMM:
				commandName = 5386;
				break;
			case TITLE_SCRIPT_LOCATION:
				commandName = 5387;
				RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = command->x;
				RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint16) = command->y;
				break;
			case TITLE_SCRIPT_ROTATE:
				commandName = 5388;
				RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = command->rotations;
				break;
			case TITLE_SCRIPT_ZOOM:
				commandName = 5389;
				RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = command->zoom;
				break;
			case TITLE_SCRIPT_RESTART:
				commandName = 5390;
				RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = command->zoom;
				break;
			case TITLE_SCRIPT_LOAD:
				commandName = 5391;
				if (command->saveIndex == 0xFF) {
					commandName = 5384;
					error = true;
				}
				RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = (uint32)&title->saves[command->saveIndex];
				break;
			case TITLE_SCRIPT_END:
				commandName = 5392;
				break;
			}
			
			if ((selected || hover) && !error) {
				format_string(buffer, commandName, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);
			}
			else {
				format_string(buffer + 1, commandName, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);
				buffer[0] = (error ? ((selected || hover) ? FORMAT_LIGHTPINK : FORMAT_RED) : FORMAT_BLACK);
			}
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = &buffer;
			gfx_draw_string_left(dpi, 1170, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, w->colours[1], x + 5, y);
		}
	}
}