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

#include "../localisation/localisation.h"
#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/viewport.h"
#include "../OpenRCT2.h"
#include "../platform/platform.h"
#include "../util/util.h"
#include "../world/footpath.h"
#include "../world/map.h"
#include "../world/scenery.h"

enum
{
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_CONTENT_PANEL,
	WIDX_SCROLL,
};

#define WW 500
#define WH 400
#define MIN_WW 300
#define MIN_WH 200

rct_widget window_textreader_widgets[] = {
	{ WWT_FRAME,			0,	0,			WW - 1,	0,			WH - 1,		0xFFFFFFFF,					STR_NONE },				// panel / background
	{ WWT_CAPTION,			0,	1,			WW - 2,	1,			14,			STR_TEXTREADER_TITLE,		STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,			0,	WW - 13,	WW - 3,	2,			13,			STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_RESIZE,			1,	0,			WW - 1,	14,			WH - 1,		0xFFFFFFFF,					STR_NONE },				// content panel
	{ WWT_SCROLL,			1,	3,			WW - 3,	16,			WH - 15,	SCROLL_BOTH,				STR_NONE },				// scroll area
	{ WIDGETS_END },
};

static void window_textreader_close(rct_window *w);
static void window_textreader_mouseup(rct_window *w, sint32 widgetIndex);
static void window_textreader_resize(rct_window *w);
static void window_textreader_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_textreader_invalidate(rct_window *w);
static void window_textreader_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_textreader_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static rct_window_event_list window_textreader_events = {
	window_textreader_close,
	window_textreader_mouseup,
	window_textreader_resize,
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
	window_textreader_scrollgetsize,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_textreader_invalidate,
	window_textreader_paint,
	window_textreader_scrollpaint
};

static bool window_textreader_read_file();
static void window_textreader_dispose_file();

static char *_textreaderFilePath = NULL;
static char *_textreaderText = NULL;
static size_t _textreaderTextSize = 0;
static char **_textreaderLines = NULL;
static sint32 _textreaderNumLines = 0;
static sint32 _textreaderLongestLineWidth = 0;

rct_window *window_textreader_open(const char * file_path)
{
	rct_window* window;

	window_close_by_class(WC_TEXTREADER);
	
	SafeFree(_textreaderFilePath);
	_textreaderFilePath = _strdup(file_path);
	
	if (!window_textreader_read_file())
		return NULL;

	window = window_create_centred(
		(gScreenWidth * 4) / 5,
		(gScreenHeight * 4) / 5,
		&window_textreader_events,
		WC_TEXTREADER,
		WF_RESIZABLE
	);
	window->widgets = window_textreader_widgets;
	window->enabled_widgets = (1 << WIDX_CLOSE);

	window_init_scroll_widgets(window);
	window->min_width = MIN_WW;
	window->min_height = MIN_WH;
	window->max_width = MIN_WW;
	window->max_height = MIN_WH;
	return window;
}

static void window_textreader_close(rct_window *w)
{
	window_textreader_dispose_file();
}

static void window_textreader_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
		case WIDX_CLOSE:
			window_close(w);
			break;
	}
}

static void window_textreader_resize(rct_window *w)
{
	window_set_resize(w, MIN_WW, MIN_WH, (gScreenWidth * 4) / 5, (gScreenHeight * 4) / 5);
}

static void window_textreader_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
	*width = _textreaderLongestLineWidth + 4;
	*height = _textreaderNumLines * 11;
}

static void window_textreader_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	set_format_arg(0, const char *, _textreaderFilePath); // set title caption to file path name
	
	window_textreader_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_textreader_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	window_textreader_widgets[WIDX_TITLE].right = w->width - 2;
	window_textreader_widgets[WIDX_CLOSE].left = w->width - 13;
	window_textreader_widgets[WIDX_CLOSE].right = w->width - 3;
	window_textreader_widgets[WIDX_CONTENT_PANEL].right = w->width - 1;
	window_textreader_widgets[WIDX_CONTENT_PANEL].bottom = w->height - 1;
	window_textreader_widgets[WIDX_SCROLL].right = w->width - 3;
	window_textreader_widgets[WIDX_SCROLL].bottom = w->height - 15;
}

static void window_textreader_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
}

static void window_textreader_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
	gCurrentFontFlags = 0;
	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	
	sint32 x = 3, y = 3;
	for (sint32 i = 0; i < _textreaderNumLines; i++) {
		gfx_draw_string(dpi, _textreaderLines[i], w->colours[0], x, y);
		y += 11;
	}
}

static bool window_textreader_read_file()
{
	window_textreader_dispose_file();
	utf8 path[MAX_PATH];
	safe_strcpy(path, gExePath, MAX_PATH);
	safe_strcat_path(path, _textreaderFilePath, MAX_PATH);
	if (!readentirefile(path, (void**)&_textreaderText, &_textreaderTextSize)) {
		log_error("Unable to read file path: %s", path);
		return false;
	}
	_textreaderText = realloc(_textreaderText, _textreaderTextSize + 1);
	_textreaderText[_textreaderTextSize++] = 0;

	char *start = _textreaderText;
	if (_textreaderTextSize >= 3 && utf8_is_bom(_textreaderText))
		start += 3;

	sint32 textreaderLinesCapacity = 8;
	_textreaderLines = malloc(textreaderLinesCapacity * sizeof(char*));
	_textreaderLines[0] = start;
	_textreaderNumLines = 1;

	char *ch = start;
	while (*ch != 0) {
		unsigned char c = *ch;
		if (c == '\n') {
			*ch++ = 0;
			_textreaderNumLines++;
			if (_textreaderNumLines > textreaderLinesCapacity) {
				textreaderLinesCapacity *= 2;
				_textreaderLines = realloc(_textreaderLines, textreaderLinesCapacity * sizeof(char*));
			}
			_textreaderLines[_textreaderNumLines - 1] = ch;
		}
		else if (c < 32 || c > 122) {
			// A character that won't be drawn or change state.
			*ch++ = FORMAT_OUTLINE_OFF;
		}
		else {
			ch++;
		}
	}

	_textreaderLines = realloc(_textreaderLines, _textreaderNumLines * sizeof(char*));
	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	_textreaderLongestLineWidth = 0;
	for (sint32 i = 0; i < _textreaderNumLines; i++) {
		int width = gfx_get_string_width(_textreaderLines[i]);
		_textreaderLongestLineWidth = max(width, _textreaderLongestLineWidth);
	}
	return true;
}

static void window_textreader_dispose_file()
{
	SafeFree(_textreaderText);
	SafeFree(_textreaderLines);
	_textreaderTextSize = 0;
	_textreaderNumLines = 0;
}
