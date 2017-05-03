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

#include "../config/Config.h"
#include "../drawing/drawing.h"
#include "../game.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../ParkImporter.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../scenario/scenario.h"
#include "../scenario/ScenarioSources.h"
#include "../sprites.h"
#include "../title/TitleScreen.h"
#include "../title/TitleSequence.h"
#include "../title/TitleSequenceManager.h"
#include "../title/TitleSequencePlayer.h"
#include "../util/util.h"
#include "../world/sprite.h"
#include "dropdown.h"
#include "error.h"

enum WINDOW_TITLE_EDITOR_TAB {
	WINDOW_TITLE_EDITOR_TAB_PRESETS,
	WINDOW_TITLE_EDITOR_TAB_SAVES,
	WINDOW_TITLE_EDITOR_TAB_SCRIPT,
	WINDOW_TITLE_EDITOR_TAB_COUNT
};

static void window_title_editor_close(rct_window *w);
static void window_title_editor_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_title_editor_resize(rct_window *w);
static void window_title_editor_mousedown(rct_widgetindex widgetIndex, rct_window*w, rct_widget* widget);
static void window_title_editor_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_title_editor_update(rct_window *w);
static void window_title_editor_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_title_editor_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_title_editor_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_title_editor_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_title_editor_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_title_editor_invalidate(rct_window *w);
static void window_title_editor_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_title_editor_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);
static void window_title_editor_scrollpaint_saves(rct_window *w, rct_drawpixelinfo *dpi);
static void window_title_editor_scrollpaint_commands(rct_window *w, rct_drawpixelinfo *dpi);
static void window_title_editor_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);
static void window_title_editor_load_sequence(size_t index);
static ITitleSequencePlayer * window_title_editor_get_player();
static bool window_title_editor_check_can_edit();
static void window_title_editor_add_park_callback(sint32 result, const utf8 * path);
static void window_title_editor_rename_park(size_t index, const utf8 * name);

static rct_window_event_list window_title_editor_events = {
	window_title_editor_close,
	window_title_editor_mouseup,
	window_title_editor_resize,
	window_title_editor_mousedown,
	window_title_editor_dropdown,
	NULL,
	window_title_editor_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_title_editor_scrollgetsize,
	window_title_editor_scrollmousedown,
	NULL,
	window_title_editor_scrollmouseover,
	window_title_editor_textinput,
	NULL,
	NULL,
	window_title_editor_tooltip,
	NULL,
	NULL,
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
	WIDX_TITLE_EDITOR_NEW_BUTTON,
	WIDX_TITLE_EDITOR_DUPLICATE_BUTTON,
	WIDX_TITLE_EDITOR_DELETE_BUTTON,
	WIDX_TITLE_EDITOR_RENAME_SAVE_BUTTON,

	// Saves Tab
	WIDX_TITLE_EDITOR_ADD_SAVE,
	WIDX_TITLE_EDITOR_REMOVE_SAVE,
	WIDX_TITLE_EDITOR_RENAME_SAVE,
	WIDX_TITLE_EDITOR_LOAD_SAVE,

	// Script Tab
	WIDX_TITLE_EDITOR_INSERT,
	WIDX_TITLE_EDITOR_EDIT,
	WIDX_TITLE_EDITOR_DELETE,
	//WIDX_TITLE_EDITOR_RELOAD,
	WIDX_TITLE_EDITOR_SKIP_TO,

	WIDX_TITLE_EDITOR_MOVE_DOWN,
	WIDX_TITLE_EDITOR_MOVE_UP,

	WIDX_TITLE_EDITOR_REPLAY,
	WIDX_TITLE_EDITOR_STOP,
	WIDX_TITLE_EDITOR_PLAY,
	WIDX_TITLE_EDITOR_SKIP,
};

// Increase BW if certain languages do not fit
// BW should be a multiple of 4
#define WW 320
#define WH 270
#define BX 8
#define BW 72
#define BY 52
#define BH 63
#define BS 18
#define ROW_HEIGHT 11
#define SCROLL_WIDTH 350
#define WH2 127

static rct_widget window_title_editor_widgets[] = {
	{ WWT_FRAME,			0,	0,		WW-1,	0,		WH2-1,	0xFFFFFFFF,				STR_NONE },								// panel / background
	{ WWT_CAPTION,			0,	1,		WW-2,	1,		14,		STR_TITLE_EDITOR_TITLE,	STR_WINDOW_TITLE_TIP },					// title bar
	{ WWT_CLOSEBOX,			0,	WW-13,	WW-3,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },					// close button
	{ WWT_RESIZE,			1,	0,		WW-1,	43,		WH2-1,	0xFFFFFFFF,				STR_NONE },								// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x20000000 | SPR_TAB,	STR_THEMES_TAB_SETTINGS_TIP },	// presets tab
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x20000000 | SPR_TAB,	STR_TITLE_EDITOR_SAVES_TAB_TIP },		// saves tab
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x20000000 | SPR_TAB,	STR_TITLE_EDITOR_SCRIPT_TAB_TIP },		// script tab
	{ WWT_SCROLL,			1,	BX+BW+9,WW-4,	48,		WH-4,	SCROLL_BOTH,			STR_NONE },								// command/save list

	// Presets Tab
	{ WWT_DROPDOWN,			1,	125,	299,	60,		71,		STR_NONE,							STR_NONE },						// Preset title sequences
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	61,		70,		STR_DROPDOWN_GLYPH,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	10,		100,	82,		93,		STR_TITLE_EDITOR_ACTION_CREATE,		STR_TITLE_EDITOR_ACTION_CREATE_SEQUENCE_TIP },						// Create button
	{ WWT_DROPDOWN_BUTTON,	1,	10,		100,	82+20,	93+20,	STR_TITLE_EDITOR_ACTION_DUPLICATE,	STR_TITLE_EDITOR_ACTION_DUPLICATE_SEQUENCE_TIP },						// Duplicate button
	{ WWT_DROPDOWN_BUTTON,	1,	110,	200,	82,		93,		STR_TRACK_MANAGE_DELETE,			STR_TITLE_EDITOR_ACTION_DELETE_SEQUENCE_TIP },						// Delete button
	{ WWT_DROPDOWN_BUTTON,	1,	210,	300,	82,		93,		STR_TRACK_MANAGE_RENAME,			STR_TITLE_EDITOR_ACTION_RENAME_SEQUENCE_TIP },						// Rename button

	// Saves Tab
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY,			BH,			STR_TITLE_EDITOR_ACTION_ADD,		STR_TITLE_EDITOR_ACTION_ADD_TIP }, // Add
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*1),	BH+(BS*1),	STR_TITLE_EDITOR_ACTION_REMOVE,		STR_TITLE_EDITOR_ACTION_REMOVE_TIP }, // Remove
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*2),	BH+(BS*2),	STR_TRACK_MANAGE_RENAME,			STR_TITLE_EDITOR_ACTION_RENAME_TIP }, // Rename
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*3),	BH+(BS*3),	STR_TITLE_EDITOR_ACTION_LOAD,		STR_TITLE_EDITOR_ACTION_LOAD_TIP }, // Load

	// Script Tab
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY,			BH,			STR_TITLE_EDITOR_ACTION_INSERT,		STR_TITLE_EDITOR_ACTION_INSERT_TIP }, // Insert
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*1),	BH+(BS*1),	STR_TITLE_EDITOR_ACTION_EDIT,		STR_TITLE_EDITOR_ACTION_EDIT_TIP }, // Edit
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*2),	BH+(BS*2),	STR_TRACK_MANAGE_DELETE,			STR_TITLE_EDITOR_ACTION_DELETE_TIP }, // Delete
	//{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*3),	BH+(BS*3),	STR_TITLE_EDITOR_ACTION_RELOAD,		STR_TITLE_EDITOR_ACTION_RELOAD_TIP }, // Reload
	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW-1,BY+(BS*3),	BH+(BS*3),	STR_TITLE_EDITOR_ACTION_SKIP_TO,	STR_TITLE_EDITOR_ACTION_SKIP_TO_TIP }, // Skip to

	{ WWT_DROPDOWN_BUTTON,	1,	BX,		BX+BW/2-1,BY+(BS*5),BH+(BS*5),	STR_DOWN,	STR_TITLE_EDITOR_ACTION_MOVE_DOWN_TIP }, // Move down
	{ WWT_DROPDOWN_BUTTON,	1,	BX+BW/2,BX+BW-1,BY+(BS*5),	BH+(BS*5),	STR_UP,	STR_TITLE_EDITOR_ACTION_MOVE_UP_TIP }, // Move up

	{ WWT_IMGBTN,			1,	BX,		BX+BW/4-1,	WH-32,	WH-16,		SPR_G2_TITLE_RESTART,	STR_TITLE_EDITOR_ACTION_REPLAY_TIP }, // Replay
	{ WWT_IMGBTN,			1,	BX+BW/4,BX+BW/2-1,	WH-32,	WH-16,		SPR_G2_TITLE_STOP,		STR_TITLE_EDITOR_ACTION_STOP_TIP }, // Stop
	{ WWT_IMGBTN,			1,	BX+BW/2,BX+BW*3/4-1,WH-32,	WH-16,		SPR_G2_TITLE_PLAY,		STR_TITLE_EDITOR_ACTION_PLAY_TIP }, // Play
	{ WWT_IMGBTN,			1,	BX+BW*3/4,BX+BW,	WH-32,	WH-16,		SPR_G2_TITLE_SKIP,		STR_TITLE_EDITOR_ACTION_SKIP_TIP }, // Skip

	{ WIDGETS_END },
};

static size_t _selectedTitleSequence = 0;
static bool _isSequenceReadOnly;
static TitleSequence * _editingTitleSequence = NULL;
static bool _isSequencePlaying = false;
static const utf8 * _sequenceName;

static utf8 * _renameSavePath = NULL;

static sint16 _window_title_editor_highlighted_index;

static sint32 window_title_editor_tab_animation_loops[] = {
	64,
	1,
	28
};
static sint32 window_title_editor_tab_animation_divisor[] = {
	4,
	1,
	4
};
static sint32 window_title_editor_tab_sprites[] = {
	SPR_TAB_RIDE_0,
	SPR_FLOPPY,
	SPR_TAB_STATS_0
};

void window_title_editor_open(sint32 tab)
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_TITLE_EDITOR);
	if (window != NULL)
		return;

	window = window_create_auto_pos(WW, WH2, &window_title_editor_events, WC_TITLE_EDITOR, WF_10 | WF_RESIZABLE);
	window->widgets = window_title_editor_widgets;
	window->enabled_widgets =
		(1 << WIDX_TITLE_EDITOR_CLOSE) |
		(1 << WIDX_TITLE_EDITOR_PRESETS_TAB) |
		(1 << WIDX_TITLE_EDITOR_SAVES_TAB) |
		(1 << WIDX_TITLE_EDITOR_SCRIPT_TAB) |

		(1 << WIDX_TITLE_EDITOR_PRESETS) |
		(1 << WIDX_TITLE_EDITOR_PRESETS_DROPDOWN) |
		(1 << WIDX_TITLE_EDITOR_NEW_BUTTON) |
		(1 << WIDX_TITLE_EDITOR_DUPLICATE_BUTTON) |
		(1 << WIDX_TITLE_EDITOR_DELETE_BUTTON) |
		(1 << WIDX_TITLE_EDITOR_RENAME_SAVE_BUTTON) |

		(1 << WIDX_TITLE_EDITOR_ADD_SAVE) |
		(1 << WIDX_TITLE_EDITOR_REMOVE_SAVE) |
		(1 << WIDX_TITLE_EDITOR_RENAME_SAVE) |
		(1 << WIDX_TITLE_EDITOR_LOAD_SAVE) |

		(1 << WIDX_TITLE_EDITOR_INSERT) |
		(1 << WIDX_TITLE_EDITOR_EDIT) |
		(1 << WIDX_TITLE_EDITOR_DELETE) |
		//(1 << WIDX_TITLE_EDITOR_RELOAD) |
		(1 << WIDX_TITLE_EDITOR_SKIP_TO) |
		(1 << WIDX_TITLE_EDITOR_MOVE_DOWN) |
		(1 << WIDX_TITLE_EDITOR_MOVE_UP) |

		(1 << WIDX_TITLE_EDITOR_PLAY) |
		(1 << WIDX_TITLE_EDITOR_STOP) |
		(1 << WIDX_TITLE_EDITOR_REPLAY) |
		(1 << WIDX_TITLE_EDITOR_SKIP);

	window_init_scroll_widgets(window);
	window->list_information_type = 0;

	window->selected_tab = tab;
	window->selected_list_item = -1;
	_window_title_editor_highlighted_index = -1;
	window->scrolls[0].v_top = 0;
	window->scrolls[0].h_left = 0;

	window->min_width = WW;
	window->min_height = WH;
	window->max_width = 500;
	window->max_height = 450;

	if (_selectedTitleSequence >= title_sequence_manager_get_count()) {
		_selectedTitleSequence = 0;
	}
	window_title_editor_load_sequence(_selectedTitleSequence);
}

static void window_title_editor_close(rct_window *w)
{
	// Close the related windows
	window_close_by_class(WC_TITLE_COMMAND_EDITOR);

	FreeTitleSequence(_editingTitleSequence);
	_editingTitleSequence = NULL;
	_isSequencePlaying = false;
	_sequenceName = NULL;

	free(_renameSavePath);
	_renameSavePath = NULL;
}

static void window_title_editor_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	bool commandEditorOpen = (window_find_by_class(WC_TITLE_COMMAND_EDITOR) != NULL);
	switch (widgetIndex) {
	case WIDX_TITLE_EDITOR_CLOSE:
		window_close(w);
		break;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Presets tab
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	case WIDX_TITLE_EDITOR_NEW_BUTTON:
		if (!commandEditorOpen) {
			// TODO: This should probably be 'NEW'
			window_text_input_open(w, widgetIndex, STR_TITLE_EDITOR_ACTION_DUPLICATE, STR_TITLE_EDITOR_ENTER_NAME_FOR_SEQUENCE, STR_NONE, 0, 64);
		}
		break;
	case WIDX_TITLE_EDITOR_DUPLICATE_BUTTON:
		if (!commandEditorOpen && _editingTitleSequence != NULL) {
			window_text_input_open(w, widgetIndex, STR_TITLE_EDITOR_ACTION_DUPLICATE, STR_TITLE_EDITOR_ENTER_NAME_FOR_SEQUENCE, STR_STRING, (uintptr_t)_editingTitleSequence->Name, 64);
		}
		break;
	case WIDX_TITLE_EDITOR_DELETE_BUTTON:
		if (window_title_editor_check_can_edit()) {
			title_sequence_manager_delete(_selectedTitleSequence);
			window_title_editor_load_sequence(0);
		}
		break;
	case WIDX_TITLE_EDITOR_RENAME_SAVE_BUTTON:
		if (window_title_editor_check_can_edit() && _editingTitleSequence != NULL) {
			window_text_input_open(w, widgetIndex, STR_TRACK_MANAGE_RENAME, STR_TITLE_EDITOR_ENTER_NAME_FOR_SEQUENCE, STR_STRING, (uintptr_t)_editingTitleSequence->Name, 64);
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Editor tab
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	case WIDX_TITLE_EDITOR_ADD_SAVE:
		if (!_isSequenceReadOnly && !_isSequencePlaying && !commandEditorOpen) {
			window_loadsave_open(LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME, NULL);
			window_loadsave_set_loadsave_callback(window_title_editor_add_park_callback);
		}
		break;
	case WIDX_TITLE_EDITOR_REMOVE_SAVE:
		if (window_title_editor_check_can_edit()) {
			if (w->selected_list_item != -1) {
				TitleSequenceRemovePark(_editingTitleSequence, w->selected_list_item);
				if (w->selected_list_item >= (sint16)_editingTitleSequence->NumSaves) {
					w->selected_list_item--;
				}
			}
		}
		break;
	case WIDX_TITLE_EDITOR_RENAME_SAVE:
		if (window_title_editor_check_can_edit()) {
			if (w->selected_list_item != -1) {
				window_text_input_open(w, widgetIndex, STR_FILEBROWSER_RENAME_SAVE_TITLE, STR_TITLE_EDITOR_ENTER_NAME_FOR_SAVE, STR_STRING, (uintptr_t)_editingTitleSequence->Saves[w->selected_list_item], 52 - 1);
			}
		}
		break;
	case WIDX_TITLE_EDITOR_LOAD_SAVE:
		if (w->selected_list_item >= 0 && w->selected_list_item < (sint16)_editingTitleSequence->NumSaves) {
			TitleSequenceParkHandle * handle = TitleSequenceGetParkHandle(_editingTitleSequence, w->selected_list_item);
			const utf8 * extension = path_get_extension(handle->HintPath);
			bool isScenario = park_importer_extension_is_scenario(extension);
			park_importer_load_from_stream(handle->Stream, handle->HintPath);
			if (isScenario) {
				scenario_begin();
			} else {
				game_load_init();
			}
			TitleSequenceCloseParkHandle(handle);
			window_title_editor_open(WINDOW_TITLE_EDITOR_TAB_SAVES);
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Commands tab
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	case WIDX_TITLE_EDITOR_INSERT:
		if (window_title_editor_check_can_edit()) {
			if (w->selected_list_item != -1) {
				window_title_command_editor_open(_editingTitleSequence, w->selected_list_item + 1, true);
			} else {
				window_title_command_editor_open(_editingTitleSequence, (sint32)_editingTitleSequence->NumCommands, true);
			}
		}
		break;
	case WIDX_TITLE_EDITOR_EDIT:
		if (window_title_editor_check_can_edit()) {
			if (w->selected_list_item != -1 && w->selected_list_item < (sint16)_editingTitleSequence->NumCommands) {
				window_title_command_editor_open(_editingTitleSequence, w->selected_list_item, false);
			}
		}
		break;
	case WIDX_TITLE_EDITOR_DELETE:
		if (window_title_editor_check_can_edit()) {
			if (w->selected_list_item != -1 && w->selected_list_item < (sint16)_editingTitleSequence->NumCommands) {
				for (sint32 i = w->selected_list_item; i < (sint16)_editingTitleSequence->NumCommands - 1; i++) {
					_editingTitleSequence->Commands[i] = _editingTitleSequence->Commands[i + 1];
				}
				_editingTitleSequence->NumCommands--;
				if (w->selected_list_item >= (sint16)_editingTitleSequence->NumCommands) {
					w->selected_list_item--;
				}
				TileSequenceSave(_editingTitleSequence);
			}
		}
		break;
	case WIDX_TITLE_EDITOR_SKIP_TO:
	{
		sint32 position = w->selected_list_item;
		if (_isSequencePlaying && position != -1 && position < (sint32)_editingTitleSequence->NumCommands) {
			ITitleSequencePlayer * player = window_title_editor_get_player();
			title_sequence_player_seek(player, position);
			title_sequence_player_update(player);
		}
		break;
	}
	case WIDX_TITLE_EDITOR_MOVE_DOWN:
		if (window_title_editor_check_can_edit()) {
			if (w->selected_list_item != -1 && w->selected_list_item < (sint16)_editingTitleSequence->NumCommands - 1) {
				TitleCommand * a = &_editingTitleSequence->Commands[w->selected_list_item];
				TitleCommand * b = &_editingTitleSequence->Commands[w->selected_list_item + 1];
				TitleCommand tmp = *a;
				*a = *b;
				*b = tmp;
				w->selected_list_item++;
				TileSequenceSave(_editingTitleSequence);
			}
		}
		break;
	case WIDX_TITLE_EDITOR_MOVE_UP:
		if (window_title_editor_check_can_edit()) {
			if (w->selected_list_item > 0 && w->selected_list_item < (sint16)_editingTitleSequence->NumCommands) {
				TitleCommand * a = &_editingTitleSequence->Commands[w->selected_list_item - 1];
				TitleCommand * b = &_editingTitleSequence->Commands[w->selected_list_item];
				TitleCommand tmp = *b;
				*b = *a;
				*a = tmp;
				w->selected_list_item--;
				TileSequenceSave(_editingTitleSequence);
			}
		}
		break;
	case WIDX_TITLE_EDITOR_REPLAY:
		if (_isSequencePlaying) {
			ITitleSequencePlayer * player = window_title_editor_get_player();
			title_sequence_player_reset(player);
			title_sequence_player_update(player);
		}
		break;
	case WIDX_TITLE_EDITOR_STOP:
		if (_isSequencePlaying) {
			gTitleCurrentSequence = 0;
			_isSequencePlaying = false;
		}
		break;
	case WIDX_TITLE_EDITOR_PLAY:
		if (!_isSequencePlaying && (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) {
			gTitleCurrentSequence = (uint16)_selectedTitleSequence;
			_isSequencePlaying = true;
		}
		break;
	case WIDX_TITLE_EDITOR_SKIP:
		if (_isSequencePlaying) {
			ITitleSequencePlayer * player = window_title_editor_get_player();
			sint32 position = title_sequence_player_get_current_position(player) + 1;
			if (position >= (sint32)_editingTitleSequence->NumCommands) {
				position = 0;
			}
			title_sequence_player_seek(player, position);
			title_sequence_player_update(player);
		}
		break;
	}
}

static void window_title_editor_resize(rct_window *w)
{
	if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_PRESETS) {
		window_set_resize(w, WW, WH, 500, WH2);
	} else {
		window_set_resize(w, WW, WH, 500, 580);
	}
}

static void window_title_editor_mousedown(rct_widgetindex widgetIndex, rct_window* w, rct_widget* widget)
{
	switch (widgetIndex) {
	case WIDX_TITLE_EDITOR_PRESETS_TAB:
	case WIDX_TITLE_EDITOR_SAVES_TAB:
	case WIDX_TITLE_EDITOR_SCRIPT_TAB:
	{
		sint32 newSelectedTab = widgetIndex - WIDX_TITLE_EDITOR_PRESETS_TAB;
		if (w->selected_tab != newSelectedTab) {
			w->selected_tab = newSelectedTab;
			w->selected_list_item = -1;
			_window_title_editor_highlighted_index = -1;
			w->scrolls[0].v_top = 0;
			w->frame_no = 0;
			window_event_resize_call(w);
			window_invalidate(w);
		}
		break;
	}
	case WIDX_TITLE_EDITOR_PRESETS_DROPDOWN:
		if (window_find_by_class(WC_TITLE_COMMAND_EDITOR) != NULL) {
			window_error_open(STR_TITLE_EDITOR_ERR_CANT_CHANGE_WHILE_EDITOR_IS_OPEN, STR_NONE);
		} else {
			sint32 numItems = (sint32)title_sequence_manager_get_count();
			for (sint32 i = 0; i < numItems; i++) {
				gDropdownItemsFormat[i] = STR_OPTIONS_DROPDOWN_ITEM;
				gDropdownItemsArgs[i] = (uintptr_t)title_sequence_manager_get_name(i);
			}

			widget--;
			window_dropdown_show_text_custom_width(
				w->x + widget->left,
				w->y + widget->top,
				widget->bottom - widget->top + 1,
				w->colours[1],
				0,
				DROPDOWN_FLAG_STAY_OPEN,
				numItems,
				widget->right - widget->left - 3);
			dropdown_set_checked((sint32)_selectedTitleSequence, true);
		}
		break;
	}
}

static void window_title_editor_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	if (dropdownIndex == -1)
		return;

	if (widgetIndex == WIDX_TITLE_EDITOR_PRESETS_DROPDOWN) {
		window_title_editor_load_sequence(dropdownIndex);
		window_invalidate(w);
	}
}

static void window_title_editor_update(rct_window *w)
{
	w->frame_no++;
	if (w->frame_no >= window_title_editor_tab_animation_loops[w->selected_tab]) {
		w->frame_no = 0;
	}

	if (!widget_is_highlighted(w, WIDX_TITLE_EDITOR_LIST)) {
		_window_title_editor_highlighted_index = -1;
		widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
	}

	widget_invalidate(w, WIDX_TITLE_EDITOR_PRESETS_TAB + w->selected_tab);
}

static void window_title_editor_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
	size_t lineCount = 1;
	if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_SAVES) {
		lineCount = _editingTitleSequence->NumSaves;
	} else if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_SCRIPT) {
		lineCount = _editingTitleSequence->NumCommands;
	}

	*height = (sint32)(lineCount * ROW_HEIGHT);
	sint32 i = *height - window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].bottom + window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].top + 21;
	if (i < 0) {
		i = 0;
	}
	if (i < w->scrolls[0].v_top) {
		w->scrolls[0].v_top = i;
		window_invalidate(w);
	}

	*width = SCROLL_WIDTH;
}

static void window_title_editor_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	sint32 index = y / ROW_HEIGHT;
	w->selected_list_item = -1;
	switch (w->selected_tab) {
	case WINDOW_TITLE_EDITOR_TAB_SAVES:
		if (index < (sint32)_editingTitleSequence->NumSaves) {
			w->selected_list_item = index;
			widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
		}
		break;
	case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
		if (index < (sint32)_editingTitleSequence->NumCommands) {
			w->selected_list_item = index;
			widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
		}
		break;
	}
}

static void window_title_editor_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	sint32 index = y / ROW_HEIGHT;
	switch (w->selected_tab) {
	case WINDOW_TITLE_EDITOR_TAB_SAVES:
		if (index < (sint32)_editingTitleSequence->NumSaves)
			_window_title_editor_highlighted_index = (sint16)index;
		break;
	case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
		if (index < (sint32)_editingTitleSequence->NumCommands)
			_window_title_editor_highlighted_index = (sint16)index;
		break;
	}
	widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
}

static void window_title_editor_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
	if (str_is_null_or_empty(text)) {
		return;
	}

	switch (widgetIndex) {
	case WIDX_TITLE_EDITOR_NEW_BUTTON:
	case WIDX_TITLE_EDITOR_DUPLICATE_BUTTON:
	case WIDX_TITLE_EDITOR_RENAME_SAVE_BUTTON:
		if (filename_valid_characters(text)) {
			if (title_sequence_manager_get_index_for_name(text) == SIZE_MAX) {
				if (widgetIndex == WIDX_TITLE_EDITOR_NEW_BUTTON) {
					size_t newIndex = title_sequence_manager_create(text);
					window_title_editor_load_sequence(newIndex);
				} else if (widgetIndex == WIDX_TITLE_EDITOR_DUPLICATE_BUTTON) {
					size_t newIndex = title_sequence_manager_duplicate(_selectedTitleSequence, text);
					window_title_editor_load_sequence(newIndex);
				} else {
					size_t newIndex = title_sequence_manager_rename(_selectedTitleSequence, text);
					window_title_editor_load_sequence(newIndex);
				}
				config_save_default();
				window_invalidate(w);
			} else {
				window_error_open(STR_ERROR_EXISTING_NAME, STR_NONE);
			}
		} else {
			window_error_open(STR_ERROR_INVALID_CHARACTERS, STR_NONE);
		}
		break;
	case WIDX_TITLE_EDITOR_RENAME_SAVE:
		window_title_editor_rename_park(w->selected_list_item, text);
		break;
	}
}

static void window_title_editor_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
	set_format_arg(0, rct_string_id, STR_LIST);
}

static void window_title_editor_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	sint32 pressed_widgets = w->pressed_widgets & ~(
			(1LL << WIDX_TITLE_EDITOR_PRESETS_TAB) |
			(1LL << WIDX_TITLE_EDITOR_SAVES_TAB) |
			(1LL << WIDX_TITLE_EDITOR_SCRIPT_TAB)
	);
	rct_widgetindex widgetIndex = w->selected_tab + WIDX_TITLE_EDITOR_PRESETS_TAB;

	w->pressed_widgets = pressed_widgets | (1 << widgetIndex);

	window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WWT_EMPTY;

	window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_NEW_BUTTON].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_DUPLICATE_BUTTON].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE_BUTTON].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_SAVE_BUTTON].type = WWT_EMPTY;

	window_title_editor_widgets[WIDX_TITLE_EDITOR_ADD_SAVE].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_REMOVE_SAVE].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_SAVE].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_LOAD_SAVE].type = WWT_EMPTY;

	window_title_editor_widgets[WIDX_TITLE_EDITOR_INSERT].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_EDIT].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE].type = WWT_EMPTY;
	//window_title_editor_widgets[WIDX_TITLE_EDITOR_RELOAD].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP_TO].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_UP].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_DOWN].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].type = WWT_EMPTY;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].type = WWT_EMPTY;

	switch (w->selected_tab) {
	case WINDOW_TITLE_EDITOR_TAB_PRESETS:
		window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].type = WWT_DROPDOWN;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_NEW_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_DUPLICATE_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_SAVE_BUTTON].type = WWT_DROPDOWN_BUTTON;
		break;
	case WINDOW_TITLE_EDITOR_TAB_SAVES:
		window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WWT_SCROLL;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_ADD_SAVE].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_REMOVE_SAVE].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_SAVE].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_LOAD_SAVE].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].type = WWT_IMGBTN;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].type = WWT_IMGBTN;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].type = WWT_IMGBTN;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].type = WWT_IMGBTN;
		break;
	case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
		window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WWT_SCROLL;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_INSERT].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_EDIT].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE].type = WWT_DROPDOWN_BUTTON;
		//window_title_editor_widgets[WIDX_TITLE_EDITOR_RELOAD].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP_TO].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_UP].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_DOWN].type = WWT_DROPDOWN_BUTTON;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].type = WWT_IMGBTN;
		window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].type = WWT_IMGBTN;
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

	window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].top = w->height - 32;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].bottom = w->height - 16;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].top = w->height - 32;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].bottom = w->height - 16;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].top = w->height - 32;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].bottom = w->height - 16;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].top = w->height - 32;
	window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].bottom = w->height - 16;

	if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) {
		w->disabled_widgets |= (1 << WIDX_TITLE_EDITOR_PLAY);
	} else {
		w->disabled_widgets &= ~(1 << WIDX_TITLE_EDITOR_PLAY);
	}
	if (!_isSequencePlaying) {
		w->disabled_widgets |= (1 << WIDX_TITLE_EDITOR_REPLAY) | (1 << WIDX_TITLE_EDITOR_STOP) | (1 << WIDX_TITLE_EDITOR_SKIP) | (1 << WIDX_TITLE_EDITOR_SKIP_TO);
	} else {
		w->disabled_widgets &= ~((1 << WIDX_TITLE_EDITOR_REPLAY) | (1 << WIDX_TITLE_EDITOR_STOP) | (1 << WIDX_TITLE_EDITOR_SKIP) | (1 << WIDX_TITLE_EDITOR_SKIP_TO));
	}
}

static void window_title_editor_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	// Widgets
	window_draw_widgets(w, dpi);
	window_title_editor_draw_tab_images(dpi, w);

	// Draw strings
	switch (w->selected_tab) {
	case WINDOW_TITLE_EDITOR_TAB_PRESETS:
		set_format_arg(0, uintptr_t, _sequenceName);
		gfx_draw_string_left(dpi, STR_TITLE_SEQUENCE, NULL, w->colours[1], w->x + 10, w->y + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].top + 1);
		gfx_draw_string_left_clipped(
			dpi,
			STR_STRING,
			gCommonFormatArgs,
			w->colours[1],
			w->x + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].left + 1,
			w->y + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].top,
			w->x + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].left - window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].left - 4);
		break;
	case WINDOW_TITLE_EDITOR_TAB_SAVES:
		break;
	case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
		break;
	}
}

static void window_title_editor_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, ColourMapA[w->colours[1]].mid_light);
	switch (w->selected_tab) {
	case WINDOW_TITLE_EDITOR_TAB_SAVES:
		window_title_editor_scrollpaint_saves(w, dpi);
		break;
	case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
		window_title_editor_scrollpaint_commands(w, dpi);
		break;
	}
}

static void window_title_editor_scrollpaint_saves(rct_window *w, rct_drawpixelinfo *dpi)
{
	sint32 currentSaveIndex = -1;
	// ITitleSequencePlayer * player = window_title_editor_get_player();
	// sint32 position = title_sequence_player_get_current_position(player);
	// const TitleCommand * command = &_editingTitleSequence->Commands[position];

	sint32 x = 0;
	sint32 y = 0;
	if (_editingTitleSequence == NULL) {
		return;
	}
	for (sint32 i = 0; i < (sint32)_editingTitleSequence->NumSaves; i++, y += ROW_HEIGHT) {
		bool selected = false;
		bool hover = false;
		if (i == w->selected_list_item) {
			selected = true;
			gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + ROW_HEIGHT - 1, ColourMapA[w->colours[1]].dark);
		} else if (i == _window_title_editor_highlighted_index || i == currentSaveIndex) {
			hover = true;
			gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + ROW_HEIGHT - 1, ColourMapA[w->colours[1]].mid_dark);
		} else if (i & 1) {
			gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + ROW_HEIGHT - 1, ColourMapA[w->colours[1]].lighter | 0x1000000);
		}

		char buffer[256];
		set_format_arg(0, uintptr_t, _editingTitleSequence->Saves[i]);
		if (selected || hover) {
			format_string(buffer, 256, STR_STRING, gCommonFormatArgs);
		} else {
			format_string(buffer + 1, 255, STR_STRING, gCommonFormatArgs);
			buffer[0] = (utf8)(uint8)FORMAT_BLACK;
		}
		set_format_arg(0, uintptr_t, &buffer);
		gfx_draw_string_left(dpi, STR_STRING, gCommonFormatArgs, w->colours[1], x + 5, y);
	}
}

static void window_title_editor_scrollpaint_commands(rct_window *w, rct_drawpixelinfo *dpi)
{
	sint32 position = -1;
	if (_isSequencePlaying) {
		ITitleSequencePlayer * player = window_title_editor_get_player();
		position = title_sequence_player_get_current_position(player);
	}

	sint32 x = 0;
	sint32 y = 0;
	for (sint32 i = 0; i < (sint32)_editingTitleSequence->NumCommands; i++, y += ROW_HEIGHT) {
		TitleCommand * command = &_editingTitleSequence->Commands[i];
		bool selected = false;
		bool hover = false;
		bool error = false;
		if (i == w->selected_list_item) {
			selected = true;
			gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + ROW_HEIGHT - 1, ColourMapA[w->colours[1]].dark);
		} else if (i == (sint32)_window_title_editor_highlighted_index || i == position) {
			hover = true;
			gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + ROW_HEIGHT - 1, ColourMapA[w->colours[1]].mid_dark);
		} else if (i & 1) {
			gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + ROW_HEIGHT - 1, ColourMapA[w->colours[1]].lighter | 0x1000000);
		}

		rct_string_id commandName = STR_NONE;
		switch (command->Type) {
		case TITLE_SCRIPT_LOAD:
			commandName = STR_TITLE_EDITOR_COMMAND_LOAD_FILE;
			if (command->SaveIndex == SAVE_INDEX_INVALID) {
				commandName = STR_TITLE_EDITOR_COMMAND_LOAD_NO_SAVE;
				error = true;
			}
			set_format_arg(0, uintptr_t, _editingTitleSequence->Saves[command->SaveIndex]);
			break;
		case TITLE_SCRIPT_LOADMM:
			commandName = STR_TITLE_EDITOR_COMMAND_LOAD_SFMM;
			break;
		case TITLE_SCRIPT_LOCATION:
			commandName = STR_TITLE_EDITOR_COMMAND_LOCATION;
			set_format_arg(0, uint16, command->X);
			set_format_arg(2, uint16, command->Y);
			break;
		case TITLE_SCRIPT_ROTATE:
			commandName = STR_TITLE_EDITOR_COMMAND_ROTATE;
			set_format_arg(0, uint16, command->Rotations);
			break;
		case TITLE_SCRIPT_ZOOM:
			commandName = STR_TITLE_EDITOR_COMMAND_ZOOM;
			set_format_arg(0, uint16, command->Zoom);
			break;
		case TITLE_SCRIPT_SPEED:
			commandName = STR_TITLE_EDITOR_COMMAND_SPEED;
			set_format_arg(0, rct_string_id, SpeedNames[command->Speed - 1]);
			break;
		case TITLE_SCRIPT_WAIT:
			commandName = STR_TITLE_EDITOR_COMMAND_WAIT;
			set_format_arg(0, uint16, command->Seconds);
			break;
		case TITLE_SCRIPT_RESTART:
			commandName = STR_TITLE_EDITOR_RESTART;
			// TODO: Why the format arg?
			set_format_arg(0, uint16, command->Zoom);
			break;
		case TITLE_SCRIPT_END:
			commandName = STR_TITLE_EDITOR_END;
			break;
		case TITLE_SCRIPT_LOADRCT1:
			commandName = STR_TITLE_EDITOR_COMMAND_LOAD_FILE;
			const char * name = "";
			source_desc desc;
			if (scenario_get_source_desc_by_id(command->SaveIndex, &desc)) {
				name = desc.title;
			}
			set_format_arg(0, uintptr_t, name);
			break;
		default:
			log_warning("Unknown command %d", command->Type);
		}

		char buffer[256];
		if ((selected || hover) && !error) {
			format_string(buffer, 256, commandName, gCommonFormatArgs);
		} else {
			format_string(buffer + 1, 255, commandName, gCommonFormatArgs);
			buffer[0] = (utf8)(error ? ((selected || hover) ? FORMAT_LIGHTPINK : FORMAT_RED) : FORMAT_BLACK);
		}
		set_format_arg(0, uintptr_t, &buffer);
		gfx_draw_string_left(dpi, STR_STRING, gCommonFormatArgs, w->colours[1], x + 5, y);
	}
}

static void window_title_editor_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	for (sint32 i = 0; i < WINDOW_TITLE_EDITOR_TAB_COUNT; i++) {
		sint32 x = 0;
		sint32 y = 0;
		sint32 spriteId = window_title_editor_tab_sprites[i];
		if (w->selected_tab == i) {
			spriteId += w->frame_no / window_title_editor_tab_animation_divisor[w->selected_tab];
		}
		if (i == 1) {
			x = 4;
			y = 1;
		}
		gfx_draw_sprite(dpi, spriteId, w->x + w->widgets[WIDX_TITLE_EDITOR_PRESETS_TAB + i].left + x, w->y + w->widgets[WIDX_TITLE_EDITOR_PRESETS_TAB + i].top + y, 0);
	}
}

static void window_title_editor_load_sequence(size_t index)
{
	if (index >= title_sequence_manager_get_count()) {
		return;
	}

	const char * path = title_sequence_manager_get_path(index);
	TitleSequence * titleSequence = LoadTitleSequence(path);
	if (titleSequence == NULL) {
		window_error_open(STR_FAILED_TO_LOAD_FILE_CONTAINS_INVALID_DATA, STR_NONE);
		return;
	}

	_selectedTitleSequence = index;
	uint16 predefinedIndex = title_sequence_manager_get_predefined_index(index);
	_isSequenceReadOnly = (predefinedIndex != UINT16_MAX);
	_sequenceName = title_sequence_manager_get_name(index);
	FreeTitleSequence(_editingTitleSequence);
	_editingTitleSequence = titleSequence;

	window_close_by_class(WC_TITLE_COMMAND_EDITOR);
}

static ITitleSequencePlayer * window_title_editor_get_player()
{
	return (ITitleSequencePlayer *)title_get_sequence_player();
}

static bool window_title_editor_check_can_edit()
{
	bool commandEditorOpen = (window_find_by_class(WC_TITLE_COMMAND_EDITOR) != NULL);
	if (_isSequenceReadOnly) {
		window_error_open(STR_ERROR_CANT_CHANGE_TITLE_SEQUENCE, STR_NONE);
	} else if (_isSequencePlaying) {
		window_error_open(STR_TITLE_EDITOR_ERR_CANT_EDIT_WHILE_PLAYING, STR_TITLE_EDITOR_PRESS_STOP_TO_CONTINUE_EDITING);
	} else if (commandEditorOpen) {
		window_error_open(STR_TITLE_EDITOR_ERR_CANT_CHANGE_WHILE_EDITOR_IS_OPEN, STR_NONE);
	} else {
		return true;
	}
	return false;
}

static bool save_filename_exists(const utf8 * filename)
{
	TitleSequence * seq = _editingTitleSequence;
	for (size_t i = 0; i < seq->NumSaves; i++) {
		const utf8 * savePath = seq->Saves[i];
		if (_stricmp(savePath, filename) == 0) {
			return true;
		}
	}
	return false;
}

static void window_title_editor_add_park_callback(sint32 result, const utf8 * path)
{
	uint32 extension = get_file_extension_type(path);
	if (extension != FILE_EXTENSION_SV4 && extension != FILE_EXTENSION_SV6) {
		return;
	}

	const utf8 * filename = path_get_filename(path);
	if (save_filename_exists(filename)) {
		free(_renameSavePath);
		_renameSavePath = _strdup(filename);
		// set_format_arg(0, intptr_t, (intptr_t)&_renameSavePath);
		rct_window * w = window_find_by_class(WC_TITLE_EDITOR);
		window_text_input_open(w, WIDX_TITLE_EDITOR_RENAME_SAVE, STR_FILEBROWSER_RENAME_SAVE_TITLE, STR_ERROR_EXISTING_NAME, STR_STRING, (uintptr_t)_renameSavePath, 52 - 1);
		return;
	}

	TileSequenceAddPark(_editingTitleSequence, path, filename);
}

static void window_title_editor_rename_park(size_t index, const utf8 * name)
{
	if (!filename_valid_characters(name)) {
		window_error_open(STR_ERROR_INVALID_CHARACTERS, STR_NONE);
		return;
	}

	for (size_t i = 0; i < _editingTitleSequence->NumSaves; i++) {
		if (i != index) {
			const utf8 * savePath = _editingTitleSequence->Saves[i];
			if (_strcmpi(savePath, name) == 0) {
				window_error_open(STR_ERROR_EXISTING_NAME, STR_NONE);
				return;
			}
		}
	}

	if (TileSequenceRenamePark(_editingTitleSequence, index, name)) {
		TileSequenceSave(_editingTitleSequence);
	}
}
