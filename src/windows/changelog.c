#include "../addresses.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/viewport.h"
#include "../world/scenery.h"
#include "../world/map.h"
#include "../world/footpath.h"
#include "../util/util.h"
#include "../openrct2.h"
#include "../platform/platform.h"

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_CONTENT_PANEL,
	WIDX_SCROLL
};

#define WW 500
#define WH 400
#define MIN_WW 300
#define MIN_WH 200

rct_widget window_changelog_widgets[] = {
	{ WWT_FRAME,			0,	0,			WW - 1,	0,		WH - 1,		0x0FFFFFFFF,						STR_NONE },				// panel / background
	{ WWT_CAPTION,			0,	1,			WW - 2,	1,		14,			STR_CHANGELOG_TITLE,				STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,			0,	WW - 13,	WW - 3,	2,		13,			STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_RESIZE,			1,	0,			WW - 1,	14,		WH - 1,		0x0FFFFFFFF,						STR_NONE },				// content panel
	{ WWT_SCROLL,			1,	3,			WW - 3,	16,		WH - 15,	3,									STR_NONE },				// scroll area
	{ WIDGETS_END },
};

static void window_changelog_close(rct_window *w);
static void window_changelog_mouseup(rct_window *w, int widgetIndex);
static void window_changelog_resize(rct_window *w);
static void window_changelog_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_changelog_invalidate(rct_window *w);
static void window_changelog_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_changelog_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_changelog_events = {
	window_changelog_close,
	window_changelog_mouseup,
	window_changelog_resize,
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
	window_changelog_scrollgetsize,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_changelog_invalidate,
	window_changelog_paint,
	window_changelog_scrollpaint
};

static bool window_changelog_read_file();
static void window_changelog_dispose_file();

static char *_changelogText = NULL;
static long _changelogTextSize = 0;
static char **_changelogLines = NULL;
static int _changelogNumLines = 0;
static int _changelogLongestLineWidth = 0;

rct_window *window_changelog_open()
{
	rct_window* window;

	window = window_bring_to_front_by_class(WC_CHANGELOG);
	if (window != NULL)
		return window;

	if (!window_changelog_read_file())
		return NULL;

	int screenWidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
	int screenHeight = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);

	window = window_create_centred(
		screenWidth * 4 / 5,
		screenHeight * 4 / 5,
		&window_changelog_events,
		WC_CHANGELOG,
		WF_RESIZABLE
	);
	window->widgets = window_changelog_widgets;
	window->enabled_widgets = (1 << WIDX_CLOSE);

	window_init_scroll_widgets(window);
	window->colours[0] = 7;
	window->colours[1] = 7;
	window->colours[2] = 7;
	window->min_width = MIN_WW;
	window->min_height = MIN_WH;
	window->max_width = MIN_WW;
	window->max_height = MIN_WH;
	return window;
}

static void window_changelog_close(rct_window *w)
{
	window_changelog_dispose_file();
}

static void window_changelog_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	}
}

static void window_changelog_resize(rct_window *w)
{
	int screenWidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
	int screenHeight = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);

	w->max_width = (screenWidth * 4) / 5;
	w->max_height = (screenHeight * 4) / 5;

	w->min_width = MIN_WW;
	w->min_height = MIN_WH;
	if (w->width < w->min_width) {
		window_invalidate(w);
		w->width = w->min_width;
	}
	if (w->height < w->min_height) {
		window_invalidate(w);
		w->height = w->min_height;
	}
}

static void window_changelog_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	*width = _changelogLongestLineWidth + 4;
	*height = _changelogNumLines * 11;
}

static void window_changelog_invalidate(rct_window *w)
{
	window_changelog_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_changelog_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	window_changelog_widgets[WIDX_TITLE].right = w->width - 2;
	window_changelog_widgets[WIDX_CLOSE].left = w->width - 13;
	window_changelog_widgets[WIDX_CLOSE].right = w->width - 3;
	window_changelog_widgets[WIDX_CONTENT_PANEL].right = w->width - 1;
	window_changelog_widgets[WIDX_CONTENT_PANEL].bottom = w->height - 1;
	window_changelog_widgets[WIDX_SCROLL].right = w->width - 3;
	window_changelog_widgets[WIDX_SCROLL].bottom = w->height - 15;
}

static void window_changelog_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
}

static void window_changelog_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	uint16 *currentFontFlags = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16);
	sint16 *currentFontSpriteBase = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16);
	*currentFontFlags = 0;
	*currentFontSpriteBase = 224;
	gfx_draw_string(dpi, (char*)0x009C383D, 1, dpi->x, dpi->y);

	int x = 3;
	int y = 3;
	for (int i = 0; i < _changelogNumLines; i++) {
		gfx_draw_string(dpi, _changelogLines[i], 254, x, y);
		y += 11;
	}
}

static bool window_changelog_read_file()
{
	window_changelog_dispose_file();
	utf8 path[MAX_PATH];
	sprintf(path, "%s%cchangelog.txt", gExePath, platform_get_path_separator());
	if (!readentirefile(path, &_changelogText, &_changelogTextSize)) {
		log_error("Unable to read changelog.txt");
		return false;
	}
	_changelogText = realloc(_changelogText, _changelogTextSize + 1);
	_changelogText[_changelogTextSize++] = 0;

	char *start = _changelogText;
	if (_changelogTextSize >= 3 && utf8_is_bom(_changelogText))
		start += 3;

	int changelogLinesCapacity = 8;
	_changelogLines = malloc(changelogLinesCapacity * sizeof(char*));
	_changelogLines[0] = start;
	_changelogNumLines = 1;

	char *ch = start;
	while (*ch != 0) {
		unsigned char c = *ch;
		if (c == '\n') {
			*ch++ = 0;
			_changelogNumLines++;
			if (_changelogNumLines > changelogLinesCapacity) {
				changelogLinesCapacity *= 2;
				_changelogLines = realloc(_changelogLines, changelogLinesCapacity * sizeof(char*));
			}
			_changelogLines[_changelogNumLines - 1] = ch;
		} else if (c < 32 || c > 122) {
			// A character that won't be drawn or change state.
			*ch++ = FORMAT_OUTLINE_OFF;
		} else {
			ch++;
		}
	}

	_changelogLines = realloc(_changelogLines, _changelogNumLines * sizeof(char*));

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	_changelogLongestLineWidth = 0;
	for (int i = 0; i < _changelogNumLines; i++) {
		int width = gfx_get_string_width(_changelogLines[i]);
		_changelogLongestLineWidth = max(width, _changelogLongestLineWidth);
	}
	return true;
}

static void window_changelog_dispose_file()
{
	SafeFree(_changelogText);
	SafeFree(_changelogLines);
	_changelogTextSize = 0;
	_changelogNumLines = 0;
}