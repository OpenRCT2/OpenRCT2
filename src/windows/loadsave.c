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

#include <time.h>  
#include "../addresses.h"
#include "../config.h"
#include "../game.h"
#include "../editor.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../network/network.h"
#include "../scenario.h"
#include "../title.h"
#include "../windows/error.h"
#include "../interface/themes.h"
#include "../interface/title_sequences.h"
#include "../util/util.h"

#pragma region Widgets

#define WW 340
#define WH 400

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_SORT_NAME,
	WIDX_SORT_DATE,
	WIDX_SCROLL,
	WIDX_BROWSE,
};

// 0x9DE48C
static rct_widget window_loadsave_widgets[] = {
	{ WWT_FRAME,		0,		0,					WW - 1,			0,			WH - 1,		STR_NONE,			STR_NONE },
	{ WWT_CAPTION,		0,		1,					WW - 2,			1,			14,			STR_NONE,			STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,		0,		WW - 13,			WW - 3,			2,			13,			STR_CLOSE_X,		STR_CLOSE_WINDOW_TIP },
	{ WWT_CLOSEBOX,		0,		4,					(WW - 5) / 2,	36,			47,			STR_NONE,			STR_NONE },
	{ WWT_CLOSEBOX,		0,		(WW - 5) / 2 + 1,	WW - 5 - 1,		36,			47,			STR_NONE,			STR_NONE },
	{ WWT_SCROLL,		0,		4,					WW - 5,			47,			WH - 40,	2,					STR_NONE },
	{ WWT_CLOSEBOX,		0,		4,					200,			WH - 36,	WH - 18,	2707,				STR_NONE }, // Use native browser
	{ WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_loadsave_close(rct_window *w);
static void window_loadsave_mouseup(rct_window *w, int widgetIndex);
static void window_loadsave_update(rct_window *w);
static void window_loadsave_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_loadsave_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_loadsave_scrollmouseover(rct_window *w, int scrollIndex, int x, int y);
static void window_loadsave_textinput(rct_window *w, int widgetIndex, char *text);
static void window_loadsave_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
static void window_loadsave_invalidate(rct_window *w);
static void window_loadsave_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_loadsave_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

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
	TYPE_UP,
	TYPE_NEW_FILE,
	TYPE_DIRECTORY,
	TYPE_FILE,
};

typedef struct {
	char name[256];
	char path[MAX_PATH];
	time_t date_modified;
	uint8 type;
} loadsave_list_item;

int _listItemsCount = 0;
loadsave_list_item *_listItems = NULL;
char _directory[MAX_PATH];
char _shortenedDirectory[MAX_PATH];
char _extension[32];
char *_defaultName = NULL;
int _loadsaveType;
int _type;

static void window_loadsave_populate_list(int includeNewItem, bool browsable, const char *directory, const char *extension);
static void window_loadsave_select(rct_window *w, const char *path);
static void window_loadsave_sort_list(int index, int endIndex);

static int has_extension(char *path, char *extension);

static void shorten_path(char* path, char* buffer, int available_width);

static rct_window *window_overwrite_prompt_open(const char *name, const char *path);

rct_window *window_loadsave_open(int type, char *defaultName)
{
	gLoadSaveTitleSequenceSave = false;
	char path[MAX_PATH], *ch;
	int includeNewItem;
	rct_window* w;
	_type = type;
	_defaultName = defaultName;

	w = window_bring_to_front_by_class(WC_LOADSAVE);
	if (w == NULL) {
		w = window_create_centred(WW, WH, &window_loadsave_events, WC_LOADSAVE, WF_STICK_TO_FRONT);
		w->widgets = window_loadsave_widgets;
		w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_SORT_NAME) | (1 << WIDX_SORT_DATE) | (1 << WIDX_BROWSE);
		w->colours[0] = 7;
		w->colours[1] = 7;
		w->colours[2] = 7;
	}

	_loadsaveType = type;
	switch (type & 0x0F) {
	case (LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME):
		w->widgets[WIDX_TITLE].image = STR_LOAD_GAME;
		break;
	case (LOADSAVETYPE_SAVE | LOADSAVETYPE_GAME) :
		w->widgets[WIDX_TITLE].image = STR_SAVE_GAME;
		break;
	case (LOADSAVETYPE_LOAD | LOADSAVETYPE_LANDSCAPE) :
		w->widgets[WIDX_TITLE].image = STR_LOAD_LANDSCAPE;
		break;
	case (LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE) :
		w->widgets[WIDX_TITLE].image = STR_SAVE_LANDSCAPE;
		break;
	case (LOADSAVETYPE_SAVE | LOADSAVETYPE_SCENARIO) :
		w->widgets[WIDX_TITLE].image = STR_SAVE_SCENARIO;
		break;
	case (LOADSAVETYPE_LOAD | LOADSAVETYPE_TRACK) :
		w->widgets[WIDX_TITLE].image = 1039;
		break;
	}

	w->no_list_items = 0;
	w->selected_list_item = -1;

	includeNewItem = (type & 0x01) == LOADSAVETYPE_SAVE;
	switch (type & 0x0E) {
	case LOADSAVETYPE_GAME:
		platform_get_user_directory(path, "save");
		if (!platform_ensure_directory_exists(path)) {
			log_error("Unable to create save directory.");
			window_close(w);
			return NULL;
		}

		window_loadsave_populate_list(includeNewItem, true, path, ".sv6");
		break;
	case LOADSAVETYPE_LANDSCAPE:
		platform_get_user_directory(path, "landscape");
		if (!platform_ensure_directory_exists(path)) {
			log_error("Unable to create landscapes directory.");
			window_close(w);
			return NULL;
		}

		window_loadsave_populate_list(includeNewItem, true, path, ".sc6");
		break;
	case LOADSAVETYPE_SCENARIO:
		/*
		Uncomment when user scenarios are separated

		platform_get_user_directory(path, "scenario");
		if (!platform_ensure_directory_exists(path)) {
		log_error("Unable to create scenarios directory.");
		window_close(w);
		return NULL;
		}
		*/

		strcpy(path, RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char));
		ch = strchr(path, '*');
		if (ch != NULL)
			*ch = 0;

		window_loadsave_populate_list(includeNewItem, true, path, ".sc6");
		break;
	case LOADSAVETYPE_TRACK:
		/*
		Uncomment when user tracks are separated

		platform_get_user_directory(path, "tracks");
		if (!platform_ensure_directory_exists(path)) {
		log_error("Unable to create tracks directory.");
		window_close(w);
		return NULL;
		}
		*/

		strcpy(path, RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char));
		ch = strchr(path, '*');
		if (ch != NULL)
			*ch = 0;

		window_loadsave_populate_list(includeNewItem, true, path, ".td?");
		break;
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

static void window_loadsave_mouseup(rct_window *w, int widgetIndex)
{
	int result;
	char filename[MAX_PATH], filter[MAX_PATH];

	switch (widgetIndex){
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_BROWSE:
		strcpy(filename, _directory);
		if (_type & LOADSAVETYPE_SAVE)
			strcat(filename, (char*)RCT2_ADDRESS_SCENARIO_NAME);

		memset(filter, '\0', MAX_PATH);
		strncpy(filter, "*", MAX_PATH);
		strncat(filter, _extension, MAX_PATH);

		switch (_type) {
		case (LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME) :
			result = platform_open_common_file_dialog(1, (char*)language_get_string(STR_LOAD_GAME), filename, filter, _extension);
			break;
		case (LOADSAVETYPE_SAVE | LOADSAVETYPE_GAME) :
			result = platform_open_common_file_dialog(0, (char*)language_get_string(STR_SAVE_GAME), filename, filter, _extension);
			break;
		case (LOADSAVETYPE_LOAD | LOADSAVETYPE_LANDSCAPE) :
			result = platform_open_common_file_dialog(1, (char*)language_get_string(STR_LOAD_LANDSCAPE), filename, filter, _extension);
			break;
		case (LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE) :
			result = platform_open_common_file_dialog(0, (char*)language_get_string(STR_SAVE_LANDSCAPE), filename, filter, _extension);
			break;
		case (LOADSAVETYPE_SAVE | LOADSAVETYPE_SCENARIO) :
			result = platform_open_common_file_dialog(0, (char*)language_get_string(STR_SAVE_SCENARIO), filename, filter, _extension);
			break;
		case (LOADSAVETYPE_LOAD | LOADSAVETYPE_TRACK) :
			result = platform_open_common_file_dialog(1, (char*)language_get_string(1039), filename, filter, _extension);
			break;
		}

		if (result) {
			if (!has_extension(filename, _extension)) {
				strncat(filename, _extension, MAX_PATH);
			}
			window_loadsave_select(w, filename);
		}
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
	}
}

static int has_extension(char *path, char *extension)
{
	int extensionLength = strlen(extension);
	int pathLength = strlen(path);
	for (int u = 0; u < extensionLength; u++) {
		if (tolower(path[pathLength - extensionLength + u]) != tolower(extension[u]))
			return 0;
	}
	return 1;
}

static void window_loadsave_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	*height = w->no_list_items * 10;
}

static void window_loadsave_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	int selectedItem;

	selectedItem = y / 10;
	if (selectedItem >= w->no_list_items)
		return;

	if (_listItems[selectedItem].type == TYPE_NEW_FILE) {
		rct_string_id templateStringId = 3165;
		char *templateString;

		templateString = (char*)language_get_string(templateStringId);
		strcpy(templateString, _defaultName);

		window_text_input_open(w, WIDX_SCROLL, STR_NONE, 2710, templateStringId, 0, 64);
	} else {
		if (_listItems[selectedItem].type == TYPE_DIRECTORY || _listItems[selectedItem].type == TYPE_UP){
			// The selected item is a folder
			int includeNewItem;

			w->no_list_items = 0;
			w->selected_list_item = -1;

			includeNewItem = (_type & 1) == LOADSAVETYPE_SAVE;

			char directory[MAX_PATH];
			strncpy(directory, _listItems[selectedItem].path, sizeof(directory));

			window_loadsave_populate_list(includeNewItem, true, directory, _extension);
			window_init_scroll_widgets(w);

			w->no_list_items = _listItemsCount;
		} else {
			// TYPE_FILE
			// Load or overwrite
			if ((_loadsaveType & 0x01) == LOADSAVETYPE_SAVE)
				window_overwrite_prompt_open(_listItems[selectedItem].name, _listItems[selectedItem].path);
			else
				window_loadsave_select(w, _listItems[selectedItem].path);
		}
	}
}

static void window_loadsave_scrollmouseover(rct_window *w, int scrollIndex, int x, int y)
{
	int selectedItem;

	selectedItem = y / 10;
	if (selectedItem >= w->no_list_items)
		return;
	
	w->selected_list_item = selectedItem;

	window_invalidate(w);
}

static void window_loadsave_textinput(rct_window *w, int widgetIndex, char *text)
{
	char path[MAX_PATH];
	int i, overwrite;

	if (text == NULL || text[0] == 0)
		return;

	if (gLoadSaveTitleSequenceSave) {
		if (filename_valid_characters(text)) {
			if (!title_sequence_save_exists(gCurrentTitleSequence, text)) {
				title_sequence_add_save(gCurrentTitleSequence, path, text);
			}
			else {
				window_error_open(5404, STR_NONE);
			}
		}
		else {
			window_error_open(5243, STR_NONE);
		}
		return;
	}

	strncpy(path, _directory, sizeof(path));
	strncat(path, text, sizeof(path));
	strncat(path, _extension, sizeof(path));

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
}

static void window_loadsave_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

static void window_loadsave_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}

static void window_loadsave_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	if (_shortenedDirectory[0] == '\0')
		shorten_path(_directory, _shortenedDirectory, w->width - 8);

	utf8 buffer[256];

	// Format text
	utf8 *ch = buffer;
	ch = utf8_write_codepoint(ch, FORMAT_MEDIUMFONT);
	ch = utf8_write_codepoint(ch, FORMAT_BLACK);
	strcpy(ch, _shortenedDirectory);

	// Draw shadow
	gfx_draw_string(dpi, buffer, 0, w->x + 4, w->y + 20);
	rct_string_id id = STR_NONE;
	if (gConfigGeneral.load_save_sort == SORT_NAME_ASCENDING)
		id = STR_UP;
	else if (gConfigGeneral.load_save_sort == SORT_NAME_DESCENDING)
		id = STR_DOWN;
	gfx_draw_string_centred_clipped(dpi, STR_NAME, &id, 1, w->x + 4 + (w->width - 8) / 4, w->y + 36, (w->width - 8) / 2);
	if (gConfigGeneral.load_save_sort == SORT_DATE_ASCENDING)
		id = STR_UP;
	else if (gConfigGeneral.load_save_sort == SORT_DATE_DESCENDING)
		id = STR_DOWN;
	else
		id = STR_NONE;
	gfx_draw_string_centred_clipped(dpi, STR_DATE, &id, 1, w->x + 4 + (w->width - 8) * 3 / 4, w->y + 36, (w->width - 8) / 2);
}

static void shorten_path(char* path, char* buffer, int available_width){
	int length = strlen(path);

	// Return full string if it fits
	if (gfx_get_string_width(path) <= available_width){
		strcpy(buffer, path);
		return;
	}

	// Count path separators
	int path_separators = 0;
	for (int x = 0; x < length; x++)
		if (path[x] == platform_get_path_separator())
			path_separators++;

	// TODO: Replace with unicode ellipsis when supported
	strcpy(buffer, "...");

	// Abreviate beginning with xth separator

	int begin = -1;
	for (int x = 0; x < path_separators; x++){
		do {
			begin++;
		} while (path[begin] != platform_get_path_separator());

		strcpy(buffer + 3, path + begin);
		if (gfx_get_string_width(buffer) <= available_width)
			return;
	}

	strcpy(buffer, path);
	return;
}

static void window_loadsave_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	int i, y;
	rct_string_id stringId, templateStringId = 3165;
	char *templateString;

	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, RCT2_ADDRESS(0x0141FC48,uint8)[w->colours[1] * 8]);
	
	templateString = (char*)language_get_string(templateStringId);
	for (i = 0; i < w->no_list_items; i++) {
		y = i * 10;
		if (y > dpi->y + dpi->height)
			break;
		
		if (y + 10 < dpi->y)
			continue;

		stringId = STR_BLACK_STRING;
		if (i == w->selected_list_item) {
			stringId = STR_WINDOW_COLOUR_2_STRING;
			gfx_fill_rect(dpi, 0, y, 800, y + 9, 0x2000031);
		}

		strcpy(templateString, _listItems[i].name);
		gfx_draw_string_left(dpi, stringId, &templateStringId, 0, 0, y - 1);
	}
}

static int compare_string_case_insensitive(char const *a, char const *b)
{
	for (;; a++, b++) {
		int d = tolower(*a) - tolower(*b);
		if (d != 0 || !*a)
			return d;
	}
}

static int list_item_sort(const void *a, const void *b)
{
	const loadsave_list_item *itemA = (loadsave_list_item*)a;
	const loadsave_list_item *itemB = (loadsave_list_item*)b;

	if (itemA->type != itemB->type)
		return itemA->type - itemB->type;

	switch (gConfigGeneral.load_save_sort){
	case SORT_NAME_ASCENDING:
		return compare_string_case_insensitive(itemA->name, itemB->name);
	case SORT_NAME_DESCENDING:
		return -compare_string_case_insensitive(itemA->name, itemB->name);
	case SORT_DATE_DESCENDING:
		return (int) -difftime(itemA->date_modified, itemB->date_modified);
	case SORT_DATE_ASCENDING:
		return (int) difftime(itemA->date_modified, itemB->date_modified);
	default:
		return compare_string_case_insensitive(itemA->name, itemB->name);
	}
}

static void window_loadsave_sort_list(int index, int endIndex)
{
	int count = endIndex - index + 1;
	if (count < 0)
		return;

	qsort(_listItems + index, count, sizeof(loadsave_list_item), list_item_sort);
}

static void window_loadsave_populate_list(int includeNewItem, bool browsable, const char *directory, const char *extension)
{
	int i, listItemCapacity, fileEnumHandle;
	file_info fileInfo;
	loadsave_list_item *listItem;
	const char *src;
	char *dst, filter[MAX_PATH], subDir[MAX_PATH];

	strncpy(_directory, directory, sizeof(_directory));
	strncpy(_extension, extension, sizeof(_extension));
	_shortenedDirectory[0] = '\0';

	strncpy(filter, directory, sizeof(filter));
	strncat(filter, "*", sizeof(filter));
	strncat(filter, extension, sizeof(filter));

	if (_listItems != NULL)
		free(_listItems);

	listItemCapacity = 8;
	_listItems = (loadsave_list_item*)malloc(listItemCapacity * sizeof(loadsave_list_item));
	_listItemsCount = 0;

	bool drivesList = false;

	if (browsable) {
		int directoryLength = strlen(directory);
		int topLevel = 1;
		int lastSlash = directoryLength;
		for (int index = directoryLength; index >= 0; index--) {
			if (directory[index] == platform_get_path_separator()) {
				if (lastSlash != directoryLength){
					// The last slash has been changed before, we're now one up
					lastSlash = index;
					topLevel = 0;
					break;
				} else {
					// The last slash, after the whole path
					lastSlash = index;
				}
			}
		}
		if (!topLevel){
			listItem = &_listItems[_listItemsCount];
			strncpy(listItem->name, language_get_string(2718), sizeof(listItem->name));
			memset(listItem->path, '\0', MAX_PATH);
			strncpy(listItem->path, directory, lastSlash + 1);
			listItem->type = TYPE_UP;
			_listItemsCount++;
		} else if (platform_get_drives() != 0 && directory[0] != '\0'){
			includeNewItem = false;
			listItem = &_listItems[_listItemsCount];
			strncpy(listItem->name, language_get_string(2718), sizeof(listItem->name));
			memset(listItem->path, '\0', MAX_PATH);
			listItem->type = TYPE_UP;
			_listItemsCount++;
		}
	}

	if (includeNewItem) {
		listItem = &_listItems[_listItemsCount];
		strncpy(listItem->name, language_get_string(2719), sizeof(listItem->name));
		listItem->path[0] = '\0';
		listItem->type = TYPE_NEW_FILE;
		_listItemsCount++;
	}

	int sortStartIndex = _listItemsCount;

	if (directory[0] != '\0'){
		fileEnumHandle = platform_enumerate_directories_begin(directory);
		while (platform_enumerate_directories_next(fileEnumHandle, subDir)){
			if (listItemCapacity <= _listItemsCount) {
				listItemCapacity *= 2;
				_listItems = realloc(_listItems, listItemCapacity * sizeof(loadsave_list_item));
			}

			listItem = &_listItems[_listItemsCount];
			memset(listItem->path, '\0', MAX_PATH);
			strncpy(listItem->path, directory, MAX_PATH);
			strncat(listItem->path, subDir, MAX_PATH);
			strncpy(listItem->name, subDir, sizeof(listItem->name));
			listItem->type = TYPE_DIRECTORY;
			_listItemsCount++;
		}
		platform_enumerate_files_end(fileEnumHandle);
		window_loadsave_sort_list(sortStartIndex, _listItemsCount - 1);

		sortStartIndex = _listItemsCount;
		fileEnumHandle = platform_enumerate_files_begin(filter);
		while (platform_enumerate_files_next(fileEnumHandle, &fileInfo)) {
			if (listItemCapacity <= _listItemsCount) {
				listItemCapacity *= 2;
				_listItems = realloc(_listItems, listItemCapacity * sizeof(loadsave_list_item));
			}

			listItem = &_listItems[_listItemsCount];
			strncpy(listItem->path, directory, sizeof(listItem->path));
			strncat(listItem->path, fileInfo.path, sizeof(listItem->path));
			listItem->type = TYPE_FILE;
			listItem->date_modified = platform_file_get_modified_time(listItem->path);

			src = fileInfo.path;
			dst = listItem->name;
			i = 0;
			while (*src != 0 && *src != '.' && i < sizeof(listItem->name) - 1) {
				*dst++ = *src++;
				i++;
			}
			*dst = 0;

			_listItemsCount++;
		}
		platform_enumerate_files_end(fileEnumHandle);
	} else {
		for (int x = 0; x < 32; x++){
			if (listItemCapacity <= _listItemsCount) {
				listItemCapacity *= 2;
				_listItems = realloc(_listItems, listItemCapacity * sizeof(loadsave_list_item));
			}

			if (platform_get_drives() & (1 << (x))){
				listItem = &_listItems[_listItemsCount];
				memset(listItem->path, '\0', MAX_PATH);
				listItem->path[0] = 'A' + x;
				listItem->path[1] = ':';
				listItem->path[2] = platform_get_path_separator();
				strcpy(listItem->name, listItem->path);
				listItem->type = TYPE_DIRECTORY;
				_listItemsCount++;
			}
		}
	}

	window_loadsave_sort_list(sortStartIndex, _listItemsCount - 1);
}

static void window_loadsave_select(rct_window *w, const char *path)
{
	SDL_RWops* rw;
	switch (_loadsaveType & 0x0F) {
	case (LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME) :
		if (gLoadSaveTitleSequenceSave) {
			utf8 newName[MAX_PATH];
			char *extension = (char*)path_get_extension(path_get_filename(path));
			strcpy(newName, path_get_filename(path));
			if (_stricmp(extension, ".sv6") != 0 && _stricmp(extension, ".sc6") != 0)
				strcat(newName, ".sv6");
			if (title_sequence_save_exists(gCurrentTitleSequence, newName)) {
				RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = (uint32)&_listItems[w->selected_list_item].name;
				window_text_input_open(w, WIDX_SCROLL, 5435, 5404, 1170, (uint32)_listItems[w->selected_list_item].name, TITLE_SEQUENCE_MAX_SAVE_LENGTH - 1);
			}
			else {
				title_sequence_add_save(gCurrentTitleSequence, path, newName);
				window_close(w);
			}
		}
		else if (game_load_save(path)) {
			if (_loadsaveType & LOADSAVETYPE_NETWORK) {
				network_begin_server(gConfigNetwork.default_port);
			}

			strcpy(gScenarioSaveName, path_get_filename(path));
			path_remove_extension(gScenarioSaveName);
			gFirstTimeSave = 0;

			window_close(w);
			gfx_invalidate_screen();
			rct2_endupdate();
		}
		else {
			// 1050, not the best message...
			window_error_open(STR_LOAD_GAME, 1050);
		}
		break;
	case (LOADSAVETYPE_SAVE | LOADSAVETYPE_GAME) :
		rw = platform_sdl_rwfromfile(path, "wb+");
		if (rw != NULL) {
			int success = scenario_save(rw, gConfigGeneral.save_plugin_data ? 1 : 0);
			SDL_RWclose(rw);
			if (success) {

				strcpy(gScenarioSaveName, path_get_filename(path));
				path_remove_extension(gScenarioSaveName);
				gFirstTimeSave = 0;

				window_close_by_class(WC_LOADSAVE);
				game_do_command(0, 1047, 0, -1, GAME_COMMAND_SET_RIDE_APPEARANCE, 0, 0);
				gfx_invalidate_screen();
			} else {
				window_error_open(STR_SAVE_GAME, 1047);
			}
		} else {
			window_error_open(STR_SAVE_GAME, 1047);
		}
		break;
	case (LOADSAVETYPE_LOAD | LOADSAVETYPE_LANDSCAPE) :
		editor_load_landscape(path);
		if (1) {
			gfx_invalidate_screen();
			rct2_endupdate();
		}
		else {
			// 1050, not the best message...
			window_error_open(STR_LOAD_LANDSCAPE, 1050);
		}
		break;
	case (LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE) :
		rw = platform_sdl_rwfromfile(path, "wb+");
		if (rw != NULL) {
			int success = scenario_save(rw, gConfigGeneral.save_plugin_data ? 3 : 2);
			SDL_RWclose(rw);
			if (success) {
				window_close_by_class(WC_LOADSAVE);
				gfx_invalidate_screen();
			} else {
				window_error_open(STR_SAVE_LANDSCAPE, 1049);
			}
		} else {
			window_error_open(STR_SAVE_LANDSCAPE, 1049);
		}
		break;
	case (LOADSAVETYPE_SAVE | LOADSAVETYPE_SCENARIO) :
	{
		rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;
		int parkFlagsBackup = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32);
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) &= ~PARK_FLAGS_18;
		s6Info->var_000 = 255;
		rw = platform_sdl_rwfromfile(path, "wb+");
		int success = 0;
		if (rw != NULL) {
			success = scenario_save(rw, gConfigGeneral.save_plugin_data ? 3 : 2);
			SDL_RWclose(rw);
		}
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) = parkFlagsBackup;

		if (success) {
			window_close_by_class(WC_LOADSAVE);
			title_load();
		} else {
			window_error_open(STR_SAVE_SCENARIO, STR_SCENARIO_SAVE_FAILED);
			s6Info->var_000 = 4;
		}
		break;
	}
	case (LOADSAVETYPE_LOAD | LOADSAVETYPE_TRACK) :
		window_install_track_open(path);
		window_close_by_class(WC_LOADSAVE);
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
	{ WWT_FRAME,			0, 0,					OVERWRITE_WW - 1,	0,					OVERWRITE_WH - 1,	STR_NONE,				STR_NONE },
	{ WWT_CAPTION,			0, 1,					OVERWRITE_WW - 2,	1,					14,					2709,					STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0, OVERWRITE_WW - 13,	OVERWRITE_WW - 3,	2,					13,					STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
	{ WWT_DROPDOWN_BUTTON,	0, 10,					94,					OVERWRITE_WH - 20,	OVERWRITE_WH - 9,	2709,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0, OVERWRITE_WW - 95,	OVERWRITE_WW - 11,	OVERWRITE_WH - 20,	OVERWRITE_WH - 9,	STR_SAVE_PROMPT_CANCEL, STR_NONE },
	{ WIDGETS_END }
};

static void window_overwrite_prompt_mouseup(rct_window *w, int widgetIndex);
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
	w->colours[0] = 154;

	strncpy(_window_overwrite_prompt_name, name, sizeof(_window_overwrite_prompt_name));
	strncpy(_window_overwrite_prompt_path, path, sizeof(_window_overwrite_prompt_path));

	return w;
}

static void window_overwrite_prompt_mouseup(rct_window *w, int widgetIndex)
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

	rct_string_id templateStringId = 3165;
	char *templateString;

	templateString = (char*)language_get_string(templateStringId);
	strcpy(templateString, _window_overwrite_prompt_name);

	int x = w->x + w->width / 2;
	int y = w->y + (w->height / 2) - 3;
	gfx_draw_string_centred_wrapped(dpi, &templateStringId, x, y, w->width - 4, 2708, 0);
}


#pragma endregion
