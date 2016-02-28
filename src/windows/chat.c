/*****************************************************************************
 * Copyright (c) 2014 Chad Ian Anderson
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
#include "../localisation/localisation.h"
#include "../world/sprite.h"
#include "../sprites.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/themes.h"
#include "../interface/chat.h"
#include "../network/network.h"
#include "../util/util.h"

char _textbox[256 + 1];

enum WINDOW_CHAT_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_SCROLL,
	WIDX_TEXT
};

static rct_widget window_chat_widgets[] = {
	{ WWT_FRAME,			0,	0,			399,	0,		299,	0x0FFFFFFFF,			STR_NONE },				// panel / background
	{ WWT_CAPTION,			0,	1,			398,	1,		14,		STR_CHAT,	STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,			0,	387,		397,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_SCROLL,			0,	4,			395,	18,		280,	2,						STR_NONE },				// scroll
	{ WWT_TEXT_BOX,			0,	4,			395,	284,	295,	(uint32)_textbox,		STR_NONE },					// chat text box

	{ WIDGETS_END },
};

static void window_chat_mouseup(rct_window *w, int widgetIndex);
static void window_chat_update(rct_window *w);
static void window_chat_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_chat_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_chat_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
static void window_chat_invalidate(rct_window *w);
static void window_chat_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_chat_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_chat_events = {
	NULL,
	window_chat_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_chat_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_chat_scrollgetsize,
	window_chat_scrollmousedown,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_chat_tooltip,
	NULL,
	NULL,
	window_chat_invalidate,
	window_chat_paint,
	window_chat_scrollpaint
};

void window_chat_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_CHAT);
	if (window == NULL) {
		window = window_create_auto_pos(
			400,
			300,
			&window_chat_events,
			WC_CHAT,
			0
		);
		window->widgets = window_chat_widgets;
		window->enabled_widgets = (
			(1 << WIDX_CLOSE) |
			(1 << WIDX_TEXT)
		);
		window_init_scroll_widgets(window);
	}

// sub_66E4BA:
	rct_widget *widget;

	int width = 0;
	int height = 0;
	window_get_scroll_size(window, 0, &width, &height);
	widget = &window_chat_widgets[WIDX_SCROLL];
	window->scrolls[0].v_top = max(0, height - (widget->bottom - widget->top - 1));
	widget_scroll_update_thumbs(window, WIDX_SCROLL);

}

static void window_chat_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TEXT:
		window_start_textbox(w, widgetIndex, 1170, (uint32)_textbox, 63);
		break;
	}
}

static void window_chat_update(rct_window *w)
{
	int i, j, x, y, z;

	window_invalidate(w);
	audio_play_sound_panned(SOUND_CLICK_2, w->x + (w->width / 2), 0, 0, 0);

}

static void window_chat_textinput(rct_window *w, int widgetIndex, char *text)
{
	if (widgetIndex != WIDX_TEXT || text == NULL)
		return;
	
	if (strlen(text) > 0) {
		network_send_chat(text);
	}
}

static void window_chat_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	int i;

	*height = 0;
	
	if (network_get_mode() == NETWORK_MODE_NONE || network_get_status() != NETWORK_STATUS_CONNECTED || network_get_authstatus() != NETWORK_AUTH_OK) {
		gChatOpen = false;
		return;
	}
	rct_drawpixelinfo *dpi = (rct_drawpixelinfo*)RCT2_ADDRESS_SCREEN_DPI;
	//_chatLeft = 10;
	//_chatTop = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 40 - ((CHAT_HISTORY_SIZE + 1) * 10);
	//_chatRight = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) - 10;
	//_chatBottom = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 40;
	char lineBuffer[256 + 10];
	char* lineCh = lineBuffer;
	int x = 10;
	int y = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 40 - (10 * 2);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;
	for (int i = 0; i < 10; i++, y -= 10) {
		//safe_strcpy(lineBuffer, chat_history_get(i), CHAT_INPUT_SIZE + 10);
		gfx_set_dirty_blocks(x, y, x + gfx_get_string_width(lineBuffer), y + 12);
		gfx_draw_string(dpi, lineBuffer, 255, x, y);
	}
	if (gChatOpen) {
		lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
		lineCh = utf8_write_codepoint(lineCh, FORMAT_CELADON);
		//safe_strcpy(lineCh, _chatCurrentLine, 256);
		y = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 40 - 10;
		gfx_set_dirty_blocks(x, y, x + gfx_get_string_width(lineBuffer) + 7, y + 12);
		gfx_draw_string(dpi, lineBuffer, 255, x, y);
		//if (_chatCaretTicks < 15) {
		//	memcpy(lineBuffer, _chatCurrentLine, gTextInputCursorPosition);
		//	lineBuffer[gTextInputCursorPosition] = 0;
			int caretX = x + gfx_get_string_width(lineBuffer);
			int caretY = y + 10;
		//
			gfx_fill_rect(dpi, caretX, caretY, caretX + 6, caretY + 1, 0x38);
		//}
	}
}

static void window_chat_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	int i, buttonIndex;

	buttonIndex = 0;

	if (buttonIndex != 0) {
		window_invalidate(w);
		audio_play_sound_panned(SOUND_CLICK_1, w->x + (w->width / 2), 0, 0, 0);
	}
}

static void window_chat_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = 3159;
}

static void window_chat_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	gfx_draw_string_left(dpi, STR_PLAYER_NAME, NULL, COLOUR_WHITE, w->x + 6, w->y + w->widgets[WIDX_TEXT].top);
	window_draw_widgets(w, dpi);
}

static void window_chat_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}

static void window_chat_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	int i, x, y, yy, press;

	y = 0;
}
