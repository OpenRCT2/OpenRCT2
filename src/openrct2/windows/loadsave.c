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

#include <time.h>
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../editor.h"
#include "../game.h"
#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../network/network.h"
#include "../scenario/scenario.h"
#include "../title/TitleScreen.h"
#include "../util/util.h"
#include "../windows/error.h"

#pragma region Widgets

#define WW 340
#define WH 400

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_DEFAULT,
	WIDX_UP,
	WIDX_NEW_FOLDER,
	WIDX_NEW_FILE,
	WIDX_SORT_NAME,
	WIDX_SORT_DATE,
	WIDX_SCROLL,
	WIDX_BROWSE,
};

// 0x9DE48C
static rct_widget window_loadsave_widgets[] = {
	{ WWT_FRAME,		0,		0,					WW - 1,			0,			WH - 1,		STR_NONE,							STR_NONE },
	{ WWT_CAPTION,		0,		1,					WW - 2,			1,			14,			STR_NONE,							STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,		0,		WW - 13,			WW - 3,			2,			13,			STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP },		//Window close button
	{ WWT_CLOSEBOX,		0,		4,					84,				36,			47,			STR_LOADSAVE_DEFAULT,				STR_LOADSAVE_DEFAULT_TIP },	// Go to default directory
	{ WWT_CLOSEBOX,		0,		85,					165,			36,			47,			STR_FILEBROWSER_ACTION_UP,			STR_NONE},					// Up
	{ WWT_CLOSEBOX,		0,		166,				246,			36,			47,			STR_FILEBROWSER_ACTION_NEW_FOLDER,	STR_NONE },					// New
	{ WWT_CLOSEBOX,		0,		247,				328,			36,			47,			STR_FILEBROWSER_ACTION_NEW_FILE,	STR_NONE },					// New
	{ WWT_CLOSEBOX,		0,		4,					(WW - 5) / 2,	50,			61,			STR_NONE,							STR_NONE },					// Name
	{ WWT_CLOSEBOX,		0,		(WW - 5) / 2 + 1,	WW - 5 - 1,		50,			61,			STR_NONE,							STR_NONE },					// Date
	{ WWT_SCROLL,		0,		4,					WW - 5,			61,			WH - 40,	SCROLL_VERTICAL,					STR_NONE },					// File list
	{ WWT_CLOSEBOX,		0,		4,					200,			WH - 36,	WH - 18,	STR_FILEBROWSER_USE_SYSTEM_WINDOW,	STR_NONE },					// Use native browser
	{ WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_loadsave_close(rct_window *w);
static void window_loadsave_mouseup(rct_window *w, sint32 widgetIndex);
static void window_loadsave_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_loadsave_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_loadsave_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_loadsave_textinput(rct_window *w, sint32 widgetIndex, char *text);
static void window_loadsave_tooltip(rct_window* w, sint32 widgetIndex, rct_string_id *stringId);
static void window_loadsave_invalidate(rct_window *w);
static void window_loadsave_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_loadsave_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static rct_window_event_list window_loadsave_events = {
	window_loadsave_close,
	window_loadsave_mouseup,
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
	window_loadsave_scrollgetsize,
	window_loadsave_scrollmousedown,
	NULL,
	window_loadsave_scrollmouseover,
	window_loadsave_textinput,
	NULL,
	NULL,
	window_loadsave_tooltip,
	NULL,
	NULL,
	window_loadsave_invalidate,
	window_loadsave_paint,
	window_loadsave_scrollpaint
};

#pragma endregion

enum {
	TYPE_DIRECTORY,
	TYPE_FILE,
};

typedef struct loadsave_list_item {
	char name[256];
	char path[MAX_PATH];
	time_t date_modified;
	uint8 type;
} loadsave_list_item;

static loadsave_callback _loadSaveCallback;

sint32 _listItemsCount = 0;
loadsave_list_item *_listItems = NULL;
char _directory[MAX_PATH];
char _shortenedDirectory[MAX_PATH];
static char _parentDirectory[MAX_PATH];
char _extension[32];
char _defaultName[MAX_PATH];
sint32 _type;

static void window_loadsave_populate_list(rct_window *w, sint32 includeNewItem, const char *directory, const char *extension);
static void window_loadsave_select(rct_window *w, const char *path);
static void window_loadsave_sort_list(sint32 index, sint32 endIndex);

static rct_window *window_overwrite_prompt_open(const char *name, const char *path);

void window_loadsave_set_loadsave_callback(loadsave_callback cb)
{
	_loadSaveCallback = cb;
}

static sint32 window_loadsave_get_dir(utf8 *last_save, char *path, const char *subdir, size_t pathSize)
{
	if (last_save && platform_ensure_directory_exists(last_save))
		safe_strcpy(path, last_save, pathSize);
	else
		platform_get_user_directory(path, subdir, pathSize);

	if (!platform_ensure_directory_exists(path)) {
		log_error("Unable to create save directory.");
		return 0;
	}
	return 1;
}

rct_window *window_loadsave_open(sint32 type, char *defaultName)
{
	_loadSaveCallback = NULL;
	_type = type;
	_defaultName[0] = '\0';

	if (!str_is_null_or_empty(defaultName)) {
		safe_strcpy(_defaultName, defaultName, sizeof(_defaultName));
	}

	rct_window *w = window_bring_to_front_by_class(WC_LOADSAVE);
	if (w == NULL) {
		w = window_create_centred(WW, WH, &window_loadsave_events, WC_LOADSAVE, WF_STICK_TO_FRONT);
		w->widgets = window_loadsave_widgets;
		w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_UP) | (1 << WIDX_NEW_FOLDER) | (1 << WIDX_NEW_FILE) | (1 << WIDX_SORT_NAME) | (1 << WIDX_SORT_DATE) | (1 << WIDX_BROWSE) | (1 << WIDX_DEFAULT);
		w->colours[0] = COLOUR_LIGHT_BLUE;
		w->colours[1] = COLOUR_LIGHT_BLUE;
		w->colours[2] = COLOUR_LIGHT_BLUE;
	}

	w->no_list_items = 0;
	w->selected_list_item = -1;

	bool isSave = (type & 0x01) == LOADSAVETYPE_SAVE;
	bool success = false;
	char path[MAX_PATH];

	switch (type & 0x0E) {
	case LOADSAVETYPE_GAME:
		w->widgets[WIDX_TITLE].text = isSave ? STR_FILE_DIALOG_TITLE_SAVE_GAME : STR_FILE_DIALOG_TITLE_LOAD_GAME;
		if (window_loadsave_get_dir(gConfigGeneral.last_save_game_directory, path, "save", sizeof(path))) {
			window_loadsave_populate_list(w, isSave, path, isSave ? ".sv6" : ".sv6;.sc6;.sv4;.sc4");
			success = true;
		}
		break;
	case LOADSAVETYPE_LANDSCAPE:
		w->widgets[WIDX_TITLE].text = isSave ? STR_FILE_DIALOG_TITLE_SAVE_LANDSCAPE : STR_FILE_DIALOG_TITLE_LOAD_LANDSCAPE;
		if (window_loadsave_get_dir(gConfigGeneral.last_save_landscape_directory, path, "landscape", sizeof(path))) {
			window_loadsave_populate_list(w, isSave, path, isSave ? ".sc6" : ".sc6;.sv6;.sc4;.sv4");
			success = true;
		}
		break;
	case LOADSAVETYPE_SCENARIO:
		w->widgets[WIDX_TITLE].text = STR_FILE_DIALOG_TITLE_SAVE_SCENARIO;
		if (window_loadsave_get_dir(gConfigGeneral.last_save_scenario_directory, path, "scenario", sizeof(path))) {
			window_loadsave_populate_list(w, isSave, path, ".sc6");
			success = true;
		}
		break;
	case LOADSAVETYPE_TRACK:
		w->widgets[WIDX_TITLE].text = isSave ? STR_FILE_DIALOG_TITLE_SAVE_TRACK : STR_FILE_DIALOG_TITLE_INSTALL_NEW_TRACK_DESIGN;
		if (window_loadsave_get_dir(gConfigGeneral.last_save_track_directory, path, "track", sizeof(path))) {
			window_loadsave_populate_list(w, isSave, path, isSave ? ".td6" : ".td6;.td4");
			success = true;
		}
		break;
	case LOADSAVETYPE_IMAGE:
		openrct2_assert(isSave == false, "Cannot save images through loadsave window");
		w->widgets[WIDX_TITLE].text = STR_FILE_DIALOG_TITLE_LOAD_HEIGHTMAP;
		if (window_loadsave_get_dir(gConfigGeneral.last_save_track_directory, path, "", sizeof(path))) {
			window_loadsave_populate_list(w, false, path, ".bmp;.png");
			success = true;
		}
		break;
	default:
		log_error("Unsupported load/save type: %d", type & 0x0F);
	}

	if (!success) {
		window_close(w);
		return NULL;
	}

	w->no_list_items = _listItemsCount;
	window_init_scroll_widgets(w);
	return w;
}

static void window_loadsave_close(rct_window *w)
{
	if (_listItems != NULL) {
		free(_listItems);
		_listItems = NULL;
	}

	window_close_by_class(WC_LOADSAVE_OVERWRITE_PROMPT);
}

static bool browse(bool isSave, char *path, size_t pathSize)
{
	safe_strcpy(path, _directory, pathSize);
	if (isSave)
		safe_strcat_path(path, _defaultName, pathSize);

	file_dialog_desc desc = { 0 };
	desc.initial_directory = _directory;
	desc.type = isSave ? FD_SAVE : FD_OPEN;
	desc.default_filename = isSave ? path : NULL;

	rct_string_id title = STR_NONE;
	switch (_type & 0x0E) {
	case LOADSAVETYPE_GAME:
		title = isSave ? STR_FILE_DIALOG_TITLE_SAVE_GAME : STR_FILE_DIALOG_TITLE_LOAD_GAME;
		desc.filters[0].name = language_get_string(STR_OPENRCT2_SAVED_GAME);
		desc.filters[0].pattern = isSave ? "*.sv6" : "*.sv6;*.sc6;*.sv4;*.sc4";
		break;
	case LOADSAVETYPE_LANDSCAPE:
		title = isSave ? STR_FILE_DIALOG_TITLE_SAVE_LANDSCAPE : STR_FILE_DIALOG_TITLE_LOAD_LANDSCAPE;
		desc.filters[0].name = language_get_string(STR_OPENRCT2_LANDSCAPE_FILE);
		desc.filters[0].pattern = isSave ? "*.sc6" : "*.sc6;*.sv6;*.sc4;*.sv4";
		break;
	case LOADSAVETYPE_SCENARIO:
		title = STR_FILE_DIALOG_TITLE_SAVE_SCENARIO;
		desc.filters[0].name = language_get_string(STR_OPENRCT2_SCENARIO_FILE);
		desc.filters[0].pattern = "*.sc6";
		break;
	case LOADSAVETYPE_TRACK:
		title = isSave ? STR_FILE_DIALOG_TITLE_SAVE_TRACK : STR_FILE_DIALOG_TITLE_INSTALL_NEW_TRACK_DESIGN;
		desc.filters[0].name = language_get_string(STR_OPENRCT2_TRACK_DESIGN_FILE);
		desc.filters[0].pattern = isSave ? "*.td6" : "*.td6;*.td4";
		break;
	}

	desc.title = language_get_string(title);
	return platform_open_common_file_dialog(path, &desc, pathSize);
}

static void window_loadsave_mouseup(rct_window *w, sint32 widgetIndex)
{
	char path[MAX_PATH];

	bool isSave = (_type & 0x01) == LOADSAVETYPE_SAVE;
	switch (widgetIndex){
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_UP:
		safe_strcpy(path, _parentDirectory, sizeof(path));
		window_loadsave_populate_list(w, isSave, path, _extension);
		window_init_scroll_widgets(w);
		w->no_list_items = _listItemsCount;
		break;
	case WIDX_NEW_FILE:
		window_text_input_open(w, WIDX_NEW_FILE, STR_NONE, STR_FILEBROWSER_FILE_NAME_PROMPT, STR_STRING, (uintptr_t)&_defaultName, 64);
		break;
	case WIDX_NEW_FOLDER:
		window_text_input_raw_open(w, WIDX_NEW_FOLDER, STR_NONE, STR_FILEBROWSER_FOLDER_NAME_PROMPT, NULL, 64);
		break;
	case WIDX_BROWSE:
		if (browse(isSave, path, sizeof(path)))
			window_loadsave_select(w, path);
		break;
	case WIDX_SORT_NAME:
		if (gConfigGeneral.load_save_sort == SORT_NAME_ASCENDING){
			gConfigGeneral.load_save_sort = SORT_NAME_DESCENDING;
		} else {
			gConfigGeneral.load_save_sort = SORT_NAME_ASCENDING;
		}
		config_save_default();
		window_loadsave_sort_list(0, _listItemsCount - 1);
		window_invalidate(w);
		break;
	case WIDX_SORT_DATE:
		if (gConfigGeneral.load_save_sort == SORT_DATE_DESCENDING){
			gConfigGeneral.load_save_sort = SORT_DATE_ASCENDING;
		} else {
			gConfigGeneral.load_save_sort = SORT_DATE_DESCENDING;
		}
		config_save_default();
		window_loadsave_sort_list(0, _listItemsCount - 1);
		window_invalidate(w);
		break;
	case WIDX_DEFAULT:
		switch (_type & 0x0E) {
		case LOADSAVETYPE_GAME:
			platform_get_user_directory(path, "save", sizeof(path));
			break;
		case LOADSAVETYPE_LANDSCAPE:
			platform_get_user_directory(path, "landscape", sizeof(path));
			break;
		case LOADSAVETYPE_SCENARIO:
			platform_get_user_directory(path, "scenario", sizeof(path));
			break;
		case LOADSAVETYPE_TRACK:
			platform_get_user_directory(path, "track", sizeof(path));
			break;
		}

		window_loadsave_populate_list(w, isSave, path, _extension);
		window_init_scroll_widgets(w);
		w->no_list_items = _listItemsCount;
		break;
	}
}

static void window_loadsave_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
	*height = w->no_list_items * 10;
}

static void window_loadsave_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	sint32 selectedItem;

	selectedItem = y / 10;
	if (selectedItem >= w->no_list_items)
		return;
	if (_listItems[selectedItem].type == TYPE_DIRECTORY){
		// The selected item is a folder
		sint32 includeNewItem;

		w->no_list_items = 0;
		w->selected_list_item = -1;
		includeNewItem = (_type & 1) == LOADSAVETYPE_SAVE;

		char directory[MAX_PATH];
		safe_strcpy(directory, _listItems[selectedItem].path, sizeof(directory));

		window_loadsave_populate_list(w, includeNewItem, directory, _extension);
		window_init_scroll_widgets(w);

		w->no_list_items = _listItemsCount;
	} else {
		// TYPE_FILE
		// Load or overwrite
		if ((_type & 0x01) == LOADSAVETYPE_SAVE)
			window_overwrite_prompt_open(_listItems[selectedItem].name, _listItems[selectedItem].path);
		else
			window_loadsave_select(w, _listItems[selectedItem].path);
	}
}

static void window_loadsave_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	sint32 selectedItem;

	selectedItem = y / 10;
	if (selectedItem >= w->no_list_items)
		return;

	w->selected_list_item = selectedItem;

	window_invalidate(w);
}

static void window_loadsave_textinput(rct_window *w, sint32 widgetIndex, char *text)
{
	char path[MAX_PATH];
	sint32 i, overwrite;

	if (text == NULL || text[0] == 0)
		return;

	switch (widgetIndex) {
		case WIDX_NEW_FOLDER:
			if (!filename_valid_characters(text)) {
				window_error_open(STR_ERROR_INVALID_CHARACTERS, STR_NONE);
				return;
			}

			safe_strcpy(path, _directory, sizeof(path));
			safe_strcat_path(path, text, sizeof(path));

			if (!platform_ensure_directory_exists(path)) {
				window_error_open(STR_UNABLE_TO_CREATE_FOLDER, STR_NONE);
				return;
			}
			w->no_list_items = 0;
			w->selected_list_item = -1;

			window_loadsave_populate_list(w, (_type & 1) == LOADSAVETYPE_SAVE, path, _extension);
			window_init_scroll_widgets(w);

			w->no_list_items = _listItemsCount;
			window_invalidate(w);
			break;
		case WIDX_NEW_FILE:
			safe_strcpy(path, _directory, sizeof(path));
			safe_strcat_path(path, text, sizeof(path));
			path_append_extension(path, _extension, sizeof(path));

			overwrite = 0;
			for (i = 0; i < _listItemsCount; i++) {
				if (_stricmp(_listItems[i].path, path) == 0) {
					overwrite = 1;
					break;
				}
			}

			if (overwrite)
				window_overwrite_prompt_open(text, path);
			else
				window_loadsave_select(w, path);
			break;
	}


}

static void window_loadsave_tooltip(rct_window* w, sint32 widgetIndex, rct_string_id *stringId)
{
	set_format_arg(0, rct_string_id, STR_LIST);
}

static void window_loadsave_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}

static void window_loadsave_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	if (_shortenedDirectory[0] == '\0') {
		shorten_path(_shortenedDirectory, sizeof(_shortenedDirectory), _directory, w->width - 8);
	}

	utf8 buffer[256];

	// Format text
	utf8 *ch = buffer;
	ch = utf8_write_codepoint(ch, FORMAT_MEDIUMFONT);
	ch = utf8_write_codepoint(ch, FORMAT_BLACK);
	safe_strcpy(ch, _shortenedDirectory, sizeof(buffer) - (ch - buffer));

	// Draw shadow
	gfx_draw_string(dpi, buffer, COLOUR_BLACK, w->x + 4, w->y + 20);
	rct_string_id id = STR_NONE;
	// Name button text
	if (gConfigGeneral.load_save_sort == SORT_NAME_ASCENDING)
		id = STR_UP;
	else if (gConfigGeneral.load_save_sort == SORT_NAME_DESCENDING)
		id = STR_DOWN;
	gfx_draw_string_centred_clipped(dpi, STR_NAME, &id, COLOUR_GREY, w->x + 4 + (w->width - 8) / 4, w->y + 50, (w->width - 8) / 2);
	// Date button text
	if (gConfigGeneral.load_save_sort == SORT_DATE_ASCENDING)
		id = STR_UP;
	else if (gConfigGeneral.load_save_sort == SORT_DATE_DESCENDING)
		id = STR_DOWN;
	else
		id = STR_NONE;
	gfx_draw_string_centred_clipped(dpi, STR_DATE, &id, COLOUR_GREY, w->x + 4 + (w->width - 8) * 3 / 4, w->y + 50, (w->width - 8) / 2);
}

static void window_loadsave_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, ColourMapA[w->colours[1]].mid_light);

	for (sint32 i = 0; i < w->no_list_items; i++) {
		sint32 y = i * 10;
		if (y > dpi->y + dpi->height)
			break;

		if (y + 10 < dpi->y)
			continue;

		rct_string_id stringId = STR_BLACK_STRING;
		if (i == w->selected_list_item) {
			stringId = STR_WINDOW_COLOUR_2_STRINGID;
			gfx_filter_rect(dpi, 0, y, 800, y + 9, PALETTE_DARKEN_1);
		}

		set_format_arg(0, rct_string_id, STR_STRING);
		set_format_arg(2, char*, _listItems[i].name);
		gfx_draw_string_left(dpi, stringId, gCommonFormatArgs, COLOUR_BLACK, 0, y - 1);
	}
}

static sint32 list_item_sort(const void *a, const void *b)
{
	const loadsave_list_item *itemA = (loadsave_list_item*)a;
	const loadsave_list_item *itemB = (loadsave_list_item*)b;

	if (itemA->type != itemB->type)
		return itemA->type - itemB->type;

	switch (gConfigGeneral.load_save_sort){
	case SORT_NAME_ASCENDING:
		return strcicmp(itemA->name, itemB->name);
	case SORT_NAME_DESCENDING:
		return -strcicmp(itemA->name, itemB->name);
	case SORT_DATE_DESCENDING:
		return (sint32) -difftime(itemA->date_modified, itemB->date_modified);
	case SORT_DATE_ASCENDING:
		return (sint32) difftime(itemA->date_modified, itemB->date_modified);
	default:
		return strcicmp(itemA->name, itemB->name);
	}
}

static void window_loadsave_sort_list(sint32 index, sint32 endIndex)
{
	sint32 count = endIndex - index + 1;
	if (count < 0)
		return;

	qsort(_listItems + index, count, sizeof(loadsave_list_item), list_item_sort);
}

static void window_loadsave_populate_list(rct_window *w, sint32 includeNewItem, const char *directory, const char *extension)
{
	safe_strcpy(_directory, directory, sizeof(_directory));
	if (_extension != extension) {
		safe_strcpy(_extension, extension, sizeof(_extension));
	}
	_shortenedDirectory[0] = '\0';

	if (_listItems != NULL)
		free(_listItems);

	sint32 listItemCapacity = 8;
	_listItems = malloc(listItemCapacity * sizeof(loadsave_list_item));
	_listItemsCount = 0;

	// Show "new" buttons when saving
	window_loadsave_widgets[WIDX_NEW_FILE].type = includeNewItem ? WWT_CLOSEBOX : WWT_EMPTY;
	window_loadsave_widgets[WIDX_NEW_FOLDER].type = includeNewItem ? WWT_CLOSEBOX : WWT_EMPTY;

	sint32 drives = platform_get_drives();
	if (str_is_null_or_empty(directory) && drives) {
		// List Windows drives
		w->disabled_widgets |= (1 << WIDX_NEW_FILE) | (1 << WIDX_NEW_FOLDER) | (1 << WIDX_UP);
		for (sint32 x = 0; x < 26; x++){
			if (listItemCapacity <= _listItemsCount) {
				listItemCapacity *= 2;
				void *new_memory = realloc(_listItems, listItemCapacity * sizeof(loadsave_list_item));
				if (new_memory == NULL) {
					log_error("Failed to reallocate memory for loadsave list");
					return;
				}
				_listItems = (loadsave_list_item*)new_memory;
			}

			if (drives & (1 << x)){
				// If the drive exists, list it
				loadsave_list_item *listItem = &_listItems[_listItemsCount];

				snprintf(listItem->path, sizeof(listItem->path), "%c:" PATH_SEPARATOR, 'A' + x);
				safe_strcpy(listItem->name, listItem->path, sizeof(listItem->name));
				listItem->type = TYPE_DIRECTORY;

				_listItemsCount++;
			}
		}
	}
	else {

		// Remove the separator at the end of the path, if present
		safe_strcpy(_parentDirectory, directory, sizeof(_parentDirectory));
		if (_parentDirectory[strlen(_parentDirectory) - 1] == *PATH_SEPARATOR)
			_parentDirectory[strlen(_parentDirectory) - 1] = '\0';

		// Remove everything past the now last separator
		char *ch = strrchr(_parentDirectory, *PATH_SEPARATOR);
		if (ch != NULL) {
			*(ch + 1) = '\0';
		} else if (drives) {
			// If on Windows, clear the entire path to show the drives
			_parentDirectory[0] = '\0';
		} else {
			// Else, go to the root directory
			snprintf(_parentDirectory, MAX_PATH, "%c", *PATH_SEPARATOR);
		}

		// Disable the Up button if the current directory is the root directory
		if (str_is_null_or_empty(_parentDirectory) && !drives)
			w->disabled_widgets |= (1 << WIDX_UP);
		else
			w->disabled_widgets &= ~(1 << WIDX_UP);

		// Re-enable the "new" buttons if these were disabled
		w->disabled_widgets &= ~(1 << WIDX_NEW_FILE);
		w->disabled_widgets &= ~(1 << WIDX_NEW_FOLDER);

		// List all directories
		char subDir[MAX_PATH];
		sint32 fileEnumHandle = platform_enumerate_directories_begin(directory);
		while (platform_enumerate_directories_next(fileEnumHandle, subDir)) {
			if (listItemCapacity <= _listItemsCount) {
				listItemCapacity *= 2;
				_listItems = realloc(_listItems, listItemCapacity * sizeof(loadsave_list_item));
			}

			loadsave_list_item *listItem = &_listItems[_listItemsCount];
			safe_strcpy(listItem->path, directory, sizeof(listItem->path));
			safe_strcat_path(listItem->path, subDir, sizeof(listItem->path));
			safe_strcpy(listItem->name, subDir, sizeof(listItem->name));
			listItem->type = TYPE_DIRECTORY;

			_listItemsCount++;
		}
		platform_enumerate_files_end(fileEnumHandle);

		// List all files with the wanted extensions
		char filter[MAX_PATH];
		char extCopy[64];
		safe_strcpy(extCopy, extension, sizeof(extCopy));
		char * extToken;
		bool showExtension = false;
		extToken = strtok(extCopy, ";");
		while (extToken != NULL) {
			safe_strcpy(filter, directory, sizeof(filter));
			safe_strcat_path(filter, "*", sizeof(filter));
			path_append_extension(filter, extToken, sizeof(filter));

			file_info fileInfo;
			fileEnumHandle = platform_enumerate_files_begin(filter);
			while (platform_enumerate_files_next(fileEnumHandle, &fileInfo)) {
				if (listItemCapacity <= _listItemsCount) {
					listItemCapacity *= 2;
					_listItems = realloc(_listItems, listItemCapacity * sizeof(loadsave_list_item));
				}

				loadsave_list_item *listItem = &_listItems[_listItemsCount];

				safe_strcpy(listItem->path, directory, sizeof(listItem->path));
				safe_strcat_path(listItem->path, fileInfo.path, sizeof(listItem->path));
				listItem->type = TYPE_FILE;
				listItem->date_modified = platform_file_get_modified_time(listItem->path);

				// Remove the extension (but only the first extension token)
				safe_strcpy(listItem->name, fileInfo.path, sizeof(listItem->name));
				if (!showExtension)
					path_remove_extension(listItem->name);

				_listItemsCount++;
			}
			platform_enumerate_files_end(fileEnumHandle);

			extToken = strtok(NULL, ";");
			showExtension = true; //Show any extension after the first iteration
		}

		window_loadsave_sort_list(0, _listItemsCount - 1);
	}
}

static void window_loadsave_invoke_callback(sint32 result, const utf8 * path)
{
	if (_loadSaveCallback != NULL) {
		_loadSaveCallback(result, path);
	}
}

static void save_path(utf8 **config_str, const char *path)
{
	if (*config_str != NULL)
		free(*config_str);
	*config_str = path_get_directory(path);
	config_save_default();
}

static bool is_valid_path(const char * path)
{
	char filename[MAX_PATH];
	safe_strcpy(filename, path_get_filename(path), sizeof(filename));

	// HACK This is needed because tracks get passed through with td?
	//      I am sure this will change eventually to use the new FileScanner
	//      which handles multiple patterns
	path_remove_extension(filename);

	return filename_valid_characters(filename);
}

static void window_loadsave_select(rct_window *w, const char *path)
{
	if (!is_valid_path(path)) {
		window_error_open(STR_ERROR_INVALID_CHARACTERS, STR_NONE);
		return;
	}

	switch (_type & 0x0F) {
	case (LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME) :
		save_path(&gConfigGeneral.last_save_game_directory, path);
		safe_strcpy(gScenarioSavePath, path, MAX_PATH);
		window_loadsave_invoke_callback(MODAL_RESULT_OK, path);
		window_close(w);
		gfx_invalidate_screen();
		break;
	case (LOADSAVETYPE_SAVE | LOADSAVETYPE_GAME) :
		save_path(&gConfigGeneral.last_save_game_directory, path);
		if (scenario_save(path, gConfigGeneral.save_plugin_data ? 1 : 0)) {
			safe_strcpy(gScenarioSavePath, path, MAX_PATH);
			gFirstTimeSave = 0;

			window_close_by_class(WC_LOADSAVE);
			gfx_invalidate_screen();

			window_loadsave_invoke_callback(MODAL_RESULT_OK, path);
		} else {
			window_error_open(STR_SAVE_GAME, STR_GAME_SAVE_FAILED);
			window_loadsave_invoke_callback(MODAL_RESULT_FAIL, path);
		}
		break;
	case (LOADSAVETYPE_LOAD | LOADSAVETYPE_LANDSCAPE) :
		save_path(&gConfigGeneral.last_save_landscape_directory, path);
		if (editor_load_landscape(path)) {
			gfx_invalidate_screen();
			window_loadsave_invoke_callback(MODAL_RESULT_OK, path);
		} else {
			// Not the best message...
			window_error_open(STR_LOAD_LANDSCAPE, STR_FAILED_TO_LOAD_FILE_CONTAINS_INVALID_DATA);
			window_loadsave_invoke_callback(MODAL_RESULT_FAIL, path);
		}
		break;
	case (LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE) :
		save_path(&gConfigGeneral.last_save_landscape_directory, path);
		scenario_set_filename(path);
		if (scenario_save(path, gConfigGeneral.save_plugin_data ? 3 : 2)) {
			window_close_by_class(WC_LOADSAVE);
			gfx_invalidate_screen();
			window_loadsave_invoke_callback(MODAL_RESULT_OK, path);
		} else {
			window_error_open(STR_SAVE_LANDSCAPE, STR_LANDSCAPE_SAVE_FAILED);
			window_loadsave_invoke_callback(MODAL_RESULT_FAIL, path);
		}
		break;
	case (LOADSAVETYPE_SAVE | LOADSAVETYPE_SCENARIO) :
	{
		save_path(&gConfigGeneral.last_save_scenario_directory, path);
		sint32 parkFlagsBackup = gParkFlags;
		gParkFlags &= ~PARK_FLAGS_SPRITES_INITIALISED;
		gS6Info.editor_step = 255;
		scenario_set_filename(path);
		sint32 success = scenario_save(path, gConfigGeneral.save_plugin_data ? 3 : 2);
		gParkFlags = parkFlagsBackup;

		if (success) {
			window_close_by_class(WC_LOADSAVE);
			window_loadsave_invoke_callback(MODAL_RESULT_OK, path);
			title_load();
		} else {
			window_error_open(STR_FILE_DIALOG_TITLE_SAVE_SCENARIO, STR_SCENARIO_SAVE_FAILED);
			gS6Info.editor_step = EDITOR_STEP_OBJECTIVE_SELECTION;
			window_loadsave_invoke_callback(MODAL_RESULT_FAIL, path);
		}
		break;
	}
	case (LOADSAVETYPE_LOAD | LOADSAVETYPE_TRACK) :
		save_path(&gConfigGeneral.last_save_track_directory, path);
		window_install_track_open(path);
		window_close_by_class(WC_LOADSAVE);
		window_loadsave_invoke_callback(MODAL_RESULT_OK, path);
		break;
	case (LOADSAVETYPE_SAVE | LOADSAVETYPE_TRACK) :
	{
		char p[MAX_PATH];
		safe_strcpy(p, path, sizeof(p));
		path_set_extension(p, "td6", sizeof(p));
		sint32 success = track_design_save_to_file(p);

		if (success) {
			window_close_by_class(WC_LOADSAVE);
			window_ride_measurements_design_cancel();
			window_loadsave_invoke_callback(MODAL_RESULT_OK, path);
		} else {
			window_error_open(STR_FILE_DIALOG_TITLE_SAVE_TRACK, STR_TRACK_SAVE_FAILED);
			window_loadsave_invoke_callback(MODAL_RESULT_FAIL, path);
		}
	}
	case (LOADSAVETYPE_LOAD | LOADSAVETYPE_IMAGE):
		window_close_by_class(WC_LOADSAVE);
		window_loadsave_invoke_callback(MODAL_RESULT_OK, path);
		break;
	}
}

#pragma region Overwrite prompt

#define OVERWRITE_WW 200
#define OVERWRITE_WH 100

enum {
	WIDX_OVERWRITE_BACKGROUND,
	WIDX_OVERWRITE_TITLE,
	WIDX_OVERWRITE_CLOSE,
	WIDX_OVERWRITE_OVERWRITE,
	WIDX_OVERWRITE_CANCEL
};

static rct_widget window_overwrite_prompt_widgets[] = {
	{ WWT_FRAME,			0, 0,					OVERWRITE_WW - 1,	0,					OVERWRITE_WH - 1,	STR_NONE,							STR_NONE },
	{ WWT_CAPTION,			0, 1,					OVERWRITE_WW - 2,	1,					14,					STR_FILEBROWSER_OVERWRITE_TITLE,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0, OVERWRITE_WW - 13,	OVERWRITE_WW - 3,	2,					13,					STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP },
	{ WWT_DROPDOWN_BUTTON,	0, 10,					94,					OVERWRITE_WH - 20,	OVERWRITE_WH - 9,	STR_FILEBROWSER_OVERWRITE_TITLE,	STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0, OVERWRITE_WW - 95,	OVERWRITE_WW - 11,	OVERWRITE_WH - 20,	OVERWRITE_WH - 9,	STR_SAVE_PROMPT_CANCEL, 			STR_NONE },
	{ WIDGETS_END }
};

static void window_overwrite_prompt_mouseup(rct_window *w, sint32 widgetIndex);
static void window_overwrite_prompt_invalidate(rct_window *w);
static void window_overwrite_prompt_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_overwrite_prompt_events = {
	NULL,
	window_overwrite_prompt_mouseup,
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
	window_overwrite_prompt_invalidate,
	window_overwrite_prompt_paint,
	NULL
};

static char _window_overwrite_prompt_name[256];
static char _window_overwrite_prompt_path[MAX_PATH];

static rct_window *window_overwrite_prompt_open(const char *name, const char *path)
{
	rct_window *w;

	window_close_by_class(WC_LOADSAVE_OVERWRITE_PROMPT);

	w = window_create_centred(OVERWRITE_WW, OVERWRITE_WH, &window_overwrite_prompt_events, WC_LOADSAVE_OVERWRITE_PROMPT, WF_STICK_TO_FRONT);
	w->widgets = window_overwrite_prompt_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_OVERWRITE_CANCEL) | (1 << WIDX_OVERWRITE_OVERWRITE);
	window_init_scroll_widgets(w);
	w->flags |= WF_TRANSPARENT;
	w->colours[0] = TRANSLUCENT(COLOUR_BORDEAUX_RED);

	safe_strcpy(_window_overwrite_prompt_name, name, sizeof(_window_overwrite_prompt_name));
	safe_strcpy(_window_overwrite_prompt_path, path, sizeof(_window_overwrite_prompt_path));

	return w;
}

static void window_overwrite_prompt_mouseup(rct_window *w, sint32 widgetIndex)
{
	rct_window *loadsaveWindow;

	switch (widgetIndex) {
	case WIDX_OVERWRITE_OVERWRITE:
		loadsaveWindow = window_find_by_class(WC_LOADSAVE);
		if (loadsaveWindow != NULL)
			window_loadsave_select(loadsaveWindow, _window_overwrite_prompt_path);
		// As the window_loadsave_select function can change the order of the
		// windows we can't use window_close(w).
		window_close_by_class(WC_LOADSAVE_OVERWRITE_PROMPT);
		break;
	case WIDX_OVERWRITE_CANCEL:
	case WIDX_OVERWRITE_CLOSE:
		window_close(w);
		break;
	}
}

static void window_overwrite_prompt_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}

static void window_overwrite_prompt_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	set_format_arg(0, rct_string_id, STR_STRING);
	set_format_arg(2, char *, _window_overwrite_prompt_name);

	sint32 x = w->x + w->width / 2;
	sint32 y = w->y + (w->height / 2) - 3;
	gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, w->width - 4, STR_FILEBROWSER_OVERWRITE_PROMPT, COLOUR_BLACK);
}


#pragma endregion
