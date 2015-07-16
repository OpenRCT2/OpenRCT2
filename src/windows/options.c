/*****************************************************************************
 * Copyright (c) 2014 Ted John, Dennis Devriendt
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

/**
 * To better group the options together and allow the window to be scalable with additional OpenRCT2 options, the window has
 * been changed to a tab interface similar to the options window seen in Locomotion.
 *
 * TODO Some parts, particularly the string handling and order of widgets needs reorganising.
 *      Padding between the widgets and the window needs reducing, an artifact from originally being inside group boxes.
 */

#include <math.h>
#include "../addresses.h"
#include "../audio/audio.h"
#include "../audio/mixer.h"
#include "../config.h"
#include "../drawing/drawing.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../platform/platform.h"
#include "../sprites.h"
#include "dropdown.h"
#include "error.h"
#include "../interface/themes.h"
#include "../interface/title_sequences.h"
#include "../title.h"

enum WINDOW_OPTIONS_PAGE {
	WINDOW_OPTIONS_PAGE_DISPLAY,
	WINDOW_OPTIONS_PAGE_CULTURE,
	WINDOW_OPTIONS_PAGE_AUDIO,
	WINDOW_OPTIONS_PAGE_CONTROLS,
	WINDOW_OPTIONS_PAGE_MISC,
	WINDOW_OPTIONS_PAGE_TWITCH,
	WINDOW_OPTIONS_PAGE_COUNT
};

#pragma region Widgets

enum WINDOW_OPTIONS_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,
	WIDX_TAB_4,
	WIDX_TAB_5,
	WIDX_TAB_6,

	WIDX_PAGE_START,

	// Display
	WIDX_RESOLUTION = WIDX_PAGE_START,
	WIDX_RESOLUTION_DROPDOWN,
	WIDX_FULLSCREEN,
	WIDX_FULLSCREEN_DROPDOWN,
	WIDX_TILE_SMOOTHING_CHECKBOX,
	WIDX_GRIDLINES_CHECKBOX,
	WIDX_HARDWARE_DISPLAY_CHECKBOX,
	WIDX_UNCAP_FPS_CHECKBOX,
	WIDX_MINIMIZE_FOCUS_LOSS,
	WIDX_CONSTRUCTION_MARKER,
	WIDX_CONSTRUCTION_MARKER_DROPDOWN,
	WIDX_THEMES,
	WIDX_THEMES_DROPDOWN,
	WIDX_THEMES_BUTTON,
	
	// Culture / Units
	WIDX_LANGUAGE = WIDX_PAGE_START,
	WIDX_LANGUAGE_DROPDOWN,
	WIDX_CURRENCY,
	WIDX_CURRENCY_DROPDOWN,
	WIDX_DISTANCE,
	WIDX_DISTANCE_DROPDOWN,
	WIDX_TEMPERATURE,
	WIDX_TEMPERATURE_DROPDOWN,
	WIDX_HEIGHT_LABELS,
	WIDX_HEIGHT_LABELS_DROPDOWN,
	WIDX_DATE_FORMAT,
	WIDX_DATE_FORMAT_DROPDOWN,

	// Audio
	WIDX_SOUND = WIDX_PAGE_START,
	WIDX_SOUND_DROPDOWN,
	WIDX_SOUND_CHECKBOX,
	WIDX_MUSIC_CHECKBOX,
	WIDX_TITLE_MUSIC,
	WIDX_TITLE_MUSIC_DROPDOWN,
	WIDX_MASTER_VOLUME,
	WIDX_MUSIC_VOLUME,

	// Controls
	WIDX_SCREEN_EDGE_SCROLLING = WIDX_PAGE_START,
	WIDX_INVERT_DRAG,
	WIDX_HOTKEY_DROPDOWN,
	WIDX_TOOLBAR_SHOW_FINANCES,
	WIDX_TOOLBAR_SHOW_RESEARCH,
	WIDX_TOOLBAR_SHOW_CHEATS,

	// Misc
	WIDX_REAL_NAME_CHECKBOX = WIDX_PAGE_START,
	WIDX_SAVE_PLUGIN_DATA_CHECKBOX,
	WIDX_AUTOSAVE,
	WIDX_AUTOSAVE_DROPDOWN,
	WIDX_SELECT_BY_TRACK_TYPE,
	WIDX_TEST_UNFINISHED_TRACKS,
	WIDX_AUTO_STAFF_PLACEMENT,
	WIDX_DEBUGGING_TOOLS,
	WIDX_TITLE_SEQUENCE,
	WIDX_TITLE_SEQUENCE_DROPDOWN,
	WIDX_TITLE_SEQUENCE_BUTTON,

	// Twitch
	WIDX_CHANNEL_BUTTON = WIDX_PAGE_START,
	WIDX_FOLLOWER_PEEP_NAMES_CHECKBOX,
	WIDX_FOLLOWER_PEEP_TRACKING_CHECKBOX,
	WIDX_CHAT_PEEP_NAMES_CHECKBOX,
	WIDX_CHAT_PEEP_TRACKING_CHECKBOX,
	WIDX_NEWS_CHECKBOX
};

#define WW 310
#define WH 194

#define MAIN_OPTIONS_WIDGETS \
	{ WWT_FRAME,			0,	0,		WW-1,	0,		WH-1,	STR_NONE,			STR_NONE }, \
	{ WWT_CAPTION,			0,	1,		WW-2,	1,		14,		STR_OPTIONS_TITLE,	STR_WINDOW_TITLE_TIP }, \
	{ WWT_CLOSEBOX,			0,	WW-13,	WW-3,	2,		13,		STR_CLOSE_X,		STR_CLOSE_WINDOW_TIP }, \
	{ WWT_RESIZE,			1,	0,		WW-1,	43,		WH-1,	0xFFFFFFFF,			STR_NONE }, \
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,			5266 }, \
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,			5267 }, \
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,			5268 }, \
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,			5269 }, \
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,			5270 }, \
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,			5271 }

static rct_widget window_options_display_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_DROPDOWN,			1,	155,	299,	53,		64,		840,				STR_NONE },					// resolution
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	54,		63,		876,				STR_NONE },
	{ WWT_DROPDOWN,			1,	155,	299,	68,		79,		871,				STR_NONE },					// fullscreen
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	69,		78,		876,				STR_NONE },
	{ WWT_CHECKBOX,			1,	10,		290,	84,		95,		STR_TILE_SMOOTHING, STR_TILE_SMOOTHING_TIP },	// landscape smoothing
	{ WWT_CHECKBOX,			1,	10,		290,	99,		110,	STR_GRIDLINES,		STR_GRIDLINES_TIP },		// gridlines
	{ WWT_CHECKBOX,			1,	10,		290,	114,	125,	5154,				STR_NONE },					// hardware display
	{ WWT_CHECKBOX,			1,	155,	290,	114,	125,	5454,				STR_NONE },					// uncap fps
	{ WWT_CHECKBOX,			1,	10,		290,	129,	140,	5440,				STR_NONE },					// minimize fullscreen focus loss
	{ WWT_DROPDOWN,			1,	155,	299,	143,	154,	STR_NONE,			STR_NONE },					// construction marker
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	144,	153,	876,				STR_NONE },
	{ WWT_DROPDOWN,			1,	155,	299,	158,	169,	STR_NONE,			STR_NONE },					// colour schemes
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	159,	168,	876,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	26,		185,	174,	185,	5153,				STR_NONE },					// colour schemes button
	{ WIDGETS_END },
};

static rct_widget window_options_culture_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_DROPDOWN,			1,	155,	299,	53,		64,		STR_NONE,		STR_NONE },	// language
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	54,		63,		876,			STR_NONE },
	{ WWT_DROPDOWN,			1,	155,	299,	68,		79,		871,			STR_NONE },	// currency
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	69,		78,		876,			STR_NONE }, //
	{ WWT_DROPDOWN,			1,	155,	299,	83,		94,		872,			STR_NONE },	// distance
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	84,		93,		876,			STR_NONE },
	{ WWT_DROPDOWN,			1,	155,	299,	98,		110,	875,			STR_NONE },	// temperature
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	99,		108,	876,			STR_NONE }, 
	{ WWT_DROPDOWN,			1,	155,	299,	113,	124,	868,			STR_NONE },	// height labels
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	114,	123,	876,			STR_NONE },
	{ WWT_DROPDOWN,			1,	155,	299,	128,	139,	STR_NONE,		STR_NONE },	// date format
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	129,	138,	876,			STR_NONE },
	{ WIDGETS_END },
};

static rct_widget window_options_audio_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_DROPDOWN,			1,	10,		299,	53,		64,		865,			STR_NONE },	// audio device
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	54,		63,		876,			STR_NONE },
	{ WWT_CHECKBOX,			1,	10,		229,	69,		80,		STR_SOUND,		STR_NONE },	// enable / disable sound
	{ WWT_CHECKBOX,			1,	10,		229,	84,		95,		STR_MUSIC,		STR_NONE },	// enable / disable music
	{ WWT_DROPDOWN,			1,	155,	299,	98,		109,	STR_NONE,		STR_NONE },	// title music
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	99,		108,	876,			STR_NONE },
	{ WWT_SCROLL,			1,	155,	299,	68,		80,		1,				STR_NONE }, // master volume
	{ WWT_SCROLL,			1,	155,	299,	83,		95,		1,				STR_NONE }, // music volume
	{ WIDGETS_END },
};

static rct_widget window_options_controls_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_CHECKBOX,			2,	10,		299,	54,		65,		STR_SCREEN_EDGE_SCROLLING,	STR_SCREEN_EDGE_SCROLLING_TIP },
	{ WWT_CHECKBOX,			2,	10,		299,	69,		80,		STR_INVERT_RIGHT_MOUSE_DRAG,STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	26,		185,	84,		95,		STR_HOTKEY,					STR_HOTKEY_TIP },
	{ WWT_CHECKBOX,			2,	10,		299,	99,		110,	5120,						STR_NONE },
	{ WWT_CHECKBOX,			2,	10,		299,	114,	125,	5121,						STR_NONE },
	{ WWT_CHECKBOX,			2,	10,		299,	129,	140,	5147,						STR_NONE },
	{ WIDGETS_END },
};

static rct_widget window_options_misc_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_CHECKBOX,			2,	10,		299,	54,		65,		STR_REAL_NAME,			STR_REAL_NAME_TIP },
	{ WWT_CHECKBOX,			2,	10,		299,	69,		80,		STR_SAVE_PLUGIN_DATA,	STR_SAVE_PLUGIN_DATA_TIP },
	{ WWT_DROPDOWN,			1,	155,	299,	83,		94,		STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	84,		93,		876,					STR_NONE },
	{ WWT_CHECKBOX,			2,	10,		299,	99,		110,	5122,					5441 },		// select by track type
	{ WWT_CHECKBOX,			2,	10,		299,	114,	125,	5155,					5156 },		// test unfinished tracks
	{ WWT_CHECKBOX,			2,	10,		299,	129,	140,	5343,					STR_NONE }, // auto staff placement
	{ WWT_CHECKBOX,			2,	10,		299,	144,	155,	5150,					STR_NONE },	// enabled debugging tools
	{ WWT_DROPDOWN,			1,	155,	299,	158,	169,	STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	159,	168,	876,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	26,		185,	174,	185,	5436,					STR_NONE },	// Title sequences button
	{ WIDGETS_END },
};

static rct_widget window_options_twitch_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_DROPDOWN_BUTTON,	2,	10,		299,	54,		65,		STR_TWITCH_NAME,			STR_NONE },							// Twitch channel name 
	{ WWT_CHECKBOX,			2,	10,		299,	69,		80,		STR_TWITCH_PEEP_FOLLOWERS,	STR_TWITCH_PEEP_FOLLOWERS_TIP },	// Twitch name peeps by follows
	{ WWT_CHECKBOX,			2,	10,		299,	84,		95,		STR_TWITCH_FOLLOWERS_TRACK,	STR_TWITCH_FOLLOWERS_TRACK_TIP },	// Twitch information on for follows
	{ WWT_CHECKBOX,			2,	10,		299,	99,		110,	STR_TWITCH_PEEP_CHAT,		STR_TWITCH_PEEP_CHAT_TIP },			// Twitch name peeps by chat
	{ WWT_CHECKBOX,			2,	10,		299,	114,	125,	STR_TWITCH_CHAT_TRACK,		STR_TWITCH_CHAT_TRACK_TIP  },		// Twitch information on for chat
	{ WWT_CHECKBOX,			2,	10,		299,	129,	140,	STR_TWITCH_CHAT_NEWS,		STR_TWITCH_CHAT_NEWS_TIP },			// Twitch chat !news as notifications in game
	{ WIDGETS_END },
};

rct_widget *window_options_page_widgets[] = {
	window_options_display_widgets,
	window_options_culture_widgets,
	window_options_audio_widgets,
	window_options_controls_widgets,
	window_options_misc_widgets,
	window_options_twitch_widgets
};

#pragma endregion

const int window_options_tab_animation_divisor[] = { 4, 8, 2, 2, 2, 1 };
const int window_options_tab_animation_frames[] = { 16, 8, 16, 4, 16, 1 };

static void window_options_set_page(rct_window *w, int page);
static void window_options_set_pressed_tab(rct_window *w);
static void window_options_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int page, int spriteIndex);
static void window_options_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);
static void window_options_show_dropdown(rct_window *w, rct_widget *widget, int num_items);
static void window_options_update_height_markers();

#pragma region Events

static void window_options_mouseup(rct_window *w, int widgetIndex);
static void window_options_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_options_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_options_update(rct_window *w);
static void window_options_invalidate(rct_window *w);
static void window_options_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_options_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_options_text_input(rct_window *w, int widgetIndex, char *text);

static rct_window_event_list window_options_events = {
	NULL,
	window_options_mouseup,
	NULL,
	window_options_mousedown,
	window_options_dropdown,
	NULL,
	window_options_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_options_scrollgetsize,
	NULL,
	NULL,
	NULL,
	window_options_text_input,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_options_invalidate,
	window_options_paint,
	NULL
};

#pragma endregion

#pragma region Enabled Widgets

#define MAIN_OPTIONS_ENABLED_WIDGETS \
	(1 << WIDX_CLOSE) | \
	(1 << WIDX_TAB_1) | \
	(1 << WIDX_TAB_2) | \
	(1 << WIDX_TAB_3) | \
	(1 << WIDX_TAB_4) | \
	(1 << WIDX_TAB_5) | \
	(1 << WIDX_TAB_6)

static uint32 window_options_page_enabled_widgets[] = {
	MAIN_OPTIONS_ENABLED_WIDGETS |
	(1 << WIDX_RESOLUTION) |
	(1 << WIDX_RESOLUTION_DROPDOWN) |
	(1 << WIDX_FULLSCREEN) |
	(1 << WIDX_FULLSCREEN_DROPDOWN) |
	(1 << WIDX_TILE_SMOOTHING_CHECKBOX) |
	(1 << WIDX_GRIDLINES_CHECKBOX) |
	(1 << WIDX_HARDWARE_DISPLAY_CHECKBOX) |
	(1 << WIDX_UNCAP_FPS_CHECKBOX) |
	(1 << WIDX_MINIMIZE_FOCUS_LOSS) |
	(1 << WIDX_CONSTRUCTION_MARKER) |
	(1 << WIDX_CONSTRUCTION_MARKER_DROPDOWN) |
	(1 << WIDX_THEMES) |
	(1 << WIDX_THEMES_DROPDOWN) |
	(1 << WIDX_THEMES_BUTTON),

	MAIN_OPTIONS_ENABLED_WIDGETS |
	(1 << WIDX_LANGUAGE) |
	(1 << WIDX_LANGUAGE_DROPDOWN) |
	(1 << WIDX_CURRENCY) |
	(1 << WIDX_CURRENCY_DROPDOWN) |
	(1 << WIDX_DISTANCE) |
	(1 << WIDX_DISTANCE_DROPDOWN) |
	(1 << WIDX_TEMPERATURE) |
	(1 << WIDX_TEMPERATURE_DROPDOWN) |
	(1 << WIDX_HEIGHT_LABELS) |
	(1 << WIDX_HEIGHT_LABELS_DROPDOWN) |
	(1 << WIDX_DATE_FORMAT) |
	(1 << WIDX_DATE_FORMAT_DROPDOWN),

	MAIN_OPTIONS_ENABLED_WIDGETS |
	(1 << WIDX_SOUND) |
	(1 << WIDX_SOUND_DROPDOWN) |
	(1 << WIDX_SOUND_CHECKBOX) |
	(1 << WIDX_MUSIC_CHECKBOX) |
	(1 << WIDX_TITLE_MUSIC) |
	(1 << WIDX_TITLE_MUSIC_DROPDOWN),

	MAIN_OPTIONS_ENABLED_WIDGETS |
	(1 << WIDX_SCREEN_EDGE_SCROLLING) |
	(1 << WIDX_INVERT_DRAG) |
	(1 << WIDX_HOTKEY_DROPDOWN) |
	(1 << WIDX_TOOLBAR_SHOW_FINANCES) |
	(1 << WIDX_TOOLBAR_SHOW_RESEARCH) |
	(1 << WIDX_TOOLBAR_SHOW_CHEATS),

	MAIN_OPTIONS_ENABLED_WIDGETS |
	(1 << WIDX_REAL_NAME_CHECKBOX) |
	(1 << WIDX_SAVE_PLUGIN_DATA_CHECKBOX) |
	(1 << WIDX_AUTOSAVE) |
	(1 << WIDX_AUTOSAVE_DROPDOWN) |
	(1 << WIDX_SELECT_BY_TRACK_TYPE) |
	(1 << WIDX_TEST_UNFINISHED_TRACKS) |
	(1 << WIDX_AUTO_STAFF_PLACEMENT) |
	(1 << WIDX_DEBUGGING_TOOLS) |
	(1 << WIDX_TITLE_SEQUENCE) |
	(1 << WIDX_TITLE_SEQUENCE_DROPDOWN) |
	(1 << WIDX_TITLE_SEQUENCE_BUTTON),

	MAIN_OPTIONS_ENABLED_WIDGETS |
	(1 << WIDX_CHANNEL_BUTTON) |
	(1 << WIDX_FOLLOWER_PEEP_NAMES_CHECKBOX) |
	(1 << WIDX_FOLLOWER_PEEP_TRACKING_CHECKBOX) |
	(1 << WIDX_CHAT_PEEP_NAMES_CHECKBOX) |
	(1 << WIDX_CHAT_PEEP_TRACKING_CHECKBOX) |
	(1 << WIDX_NEWS_CHECKBOX)
};

#pragma endregion

/**
*
*  rct2: 0x006BAC5B
*/
void window_options_open()
{
	rct_window* w;

	// Check if window is already open
	w = window_bring_to_front_by_class(WC_OPTIONS);
	if (w != NULL)
		return;

	w = window_create_centred(WW, WH, &window_options_events, WC_OPTIONS, WF_RESIZABLE);
	w->widgets = window_options_display_widgets;
	w->enabled_widgets = window_options_page_enabled_widgets[WINDOW_OPTIONS_PAGE_DISPLAY];
	w->page = WINDOW_OPTIONS_PAGE_DISPLAY;
	w->frame_no = 0;
	window_init_scroll_widgets(w);
	//window_invalidate(w);

	colour_scheme_update(w);
}


/**
*
*  rct2: 0x006BAFCA
*/
static void window_options_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
		window_options_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	}

	switch (w->page) {
	case WINDOW_OPTIONS_PAGE_DISPLAY:
		switch (widgetIndex) {
		case WIDX_TILE_SMOOTHING_CHECKBOX:
			gConfigGeneral.landscape_smoothing ^= 1;
			config_save_default();
			gfx_invalidate_screen();
			break;
		case WIDX_GRIDLINES_CHECKBOX:
			gConfigGeneral.always_show_gridlines ^= 1;
			config_save_default();
			gfx_invalidate_screen();
			if ((w = window_get_main()) != NULL) {
				if (gConfigGeneral.always_show_gridlines)
					w->viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
				else
					w->viewport->flags &= ~VIEWPORT_FLAG_GRIDLINES;
			}
			break;
		case WIDX_HARDWARE_DISPLAY_CHECKBOX:
			gConfigGeneral.hardware_display ^= 1;
			platform_refresh_video();
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_UNCAP_FPS_CHECKBOX:
			gConfigGeneral.uncap_fps ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_MINIMIZE_FOCUS_LOSS:
			gConfigGeneral.minimize_fullscreen_focus_loss ^= 1;
			platform_refresh_video();
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_THEMES_BUTTON:
			window_themes_open();
			window_invalidate(w);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CULTURE:
		break;

	case WINDOW_OPTIONS_PAGE_AUDIO:
		switch (widgetIndex) {
		case WIDX_SOUND_CHECKBOX:
			toggle_all_sounds();
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_MUSIC_CHECKBOX:
			gConfigSound.ride_music = !gConfigSound.ride_music;
			if (!gConfigSound.ride_music) {
				stop_ride_music();
			}
			config_save_default();
			window_invalidate(w);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CONTROLS:
		switch (widgetIndex) {
		case WIDX_HOTKEY_DROPDOWN:
			window_shortcut_keys_open();
			break;
		case WIDX_SCREEN_EDGE_SCROLLING:
			gConfigGeneral.edge_scrolling ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_TOOLBAR_SHOW_FINANCES:
			gConfigInterface.toolbar_show_finances ^= 1;
			config_save_default();
			window_invalidate(w);
			window_invalidate_by_class(WC_TOP_TOOLBAR);
			break;
		case WIDX_TOOLBAR_SHOW_RESEARCH:
			gConfigInterface.toolbar_show_research ^= 1;
			config_save_default();
			window_invalidate(w);
			window_invalidate_by_class(WC_TOP_TOOLBAR);
			break;
		case WIDX_TOOLBAR_SHOW_CHEATS:
			gConfigInterface.toolbar_show_cheats ^= 1;
			config_save_default();
			window_invalidate(w);
			window_invalidate_by_class(WC_TOP_TOOLBAR);
			break;
		case WIDX_INVERT_DRAG:
			gConfigGeneral.invert_viewport_drag ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_MISC:
		switch (widgetIndex) {
		case WIDX_SELECT_BY_TRACK_TYPE:
			gConfigInterface.select_by_track_type ^= 1;
			config_save_default();
			window_invalidate(w);
			window_invalidate_by_class(WC_RIDE);
			window_invalidate_by_class(WC_CONSTRUCT_RIDE);
			break;
		case WIDX_DEBUGGING_TOOLS:
			gConfigGeneral.debugging_tools ^= 1;
			config_save_default();
			window_invalidate(w);
			window_invalidate_by_class(WC_TOP_TOOLBAR);
			break;
		case WIDX_TEST_UNFINISHED_TRACKS:
			gConfigGeneral.test_unfinished_tracks ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_REAL_NAME_CHECKBOX:
			peep_update_names(
				!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_SHOW_REAL_GUEST_NAMES)
			);
			break;
		case WIDX_SAVE_PLUGIN_DATA_CHECKBOX:
			gConfigGeneral.save_plugin_data ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_AUTO_STAFF_PLACEMENT:
			gConfigGeneral.auto_staff_placement ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_TITLE_SEQUENCE_BUTTON:
			window_title_editor_open(0);
		}
		break;

	case WINDOW_OPTIONS_PAGE_TWITCH:
		switch (widgetIndex) {
		case WIDX_CHANNEL_BUTTON:
			window_text_input_raw_open(w, widgetIndex, STR_TWITCH_NAME, STR_TWITCH_NAME_DESC, gConfigTwitch.channel, 32);
			break;
		case WIDX_FOLLOWER_PEEP_NAMES_CHECKBOX:
			gConfigTwitch.enable_follower_peep_names ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_FOLLOWER_PEEP_TRACKING_CHECKBOX:
			gConfigTwitch.enable_follower_peep_tracking ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_CHAT_PEEP_NAMES_CHECKBOX:
			gConfigTwitch.enable_chat_peep_names ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_CHAT_PEEP_TRACKING_CHECKBOX:
			gConfigTwitch.enable_chat_peep_tracking ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_NEWS_CHECKBOX:
			gConfigTwitch.enable_news ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		}
		break;
	}
}

/**
*
*  rct2: 0x006BB01B
*/
static void window_options_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	int num_items, i;
	
	widget = &w->widgets[widgetIndex - 1];

	switch (w->page) {
	case WINDOW_OPTIONS_PAGE_DISPLAY:
		switch (widgetIndex) {
		case WIDX_RESOLUTION_DROPDOWN:
		{
			platform_update_fullscreen_resolutions();

			int selectedResolution = -1;
			for (i = 0; i < gNumResolutions; i++) {
				resolution *resolution = &gResolutions[i];

				gDropdownItemsFormat[i] = 1142;

				uint16 *args = (uint16*)&gDropdownItemsArgs[i];
				args[0] = 839;
				args[1] = resolution->width;
				args[2] = resolution->height;

				if (resolution->width == gConfigGeneral.fullscreen_width && resolution->height == gConfigGeneral.fullscreen_height)
					selectedResolution = i;
			}

			window_options_show_dropdown(w, widget, gNumResolutions);

			if (selectedResolution != -1 && selectedResolution < 32)
				gDropdownItemsChecked = 1 << selectedResolution;
		}

		break;
		case WIDX_FULLSCREEN_DROPDOWN:
			gDropdownItemsFormat[0] = 1142;
			gDropdownItemsFormat[1] = 1142;
			gDropdownItemsFormat[2] = 1142;
			gDropdownItemsArgs[0] = 2773;
			gDropdownItemsArgs[1] = 2774;
			gDropdownItemsArgs[2] = 2775;

			window_options_show_dropdown(w, widget, 3);

			gDropdownItemsChecked = 1 << gConfigGeneral.fullscreen_mode;
			break;
		case WIDX_CONSTRUCTION_MARKER_DROPDOWN:
			gDropdownItemsFormat[0] = 1142;
			gDropdownItemsFormat[1] = 1142;
			gDropdownItemsArgs[0] = STR_WHITE;
			gDropdownItemsArgs[1] = STR_TRANSLUCENT;

			window_options_show_dropdown(w, widget, 2);

			gDropdownItemsChecked = 1 << gConfigGeneral.construction_marker_colour;
			break;
		case WIDX_THEMES_DROPDOWN:
			num_items = gConfigThemes.num_presets;

			gDropdownItemsFormat[0] = 2777;
			gDropdownItemsArgs[0] = (uint32)&gConfigThemes.presets[1].name;
			gDropdownItemsFormat[1] = 2777;
			gDropdownItemsArgs[1] = (uint32)&gConfigThemes.presets[0].name;

			for (i = 2; i < num_items; i++) {
				gDropdownItemsFormat[i] = 2777;
				gDropdownItemsArgs[i] = (uint32)&gConfigThemes.presets[i].name;
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

			if (gCurrentTheme == 0 || gCurrentTheme == 1)
				gDropdownItemsChecked = 1 << (gCurrentTheme ^ 1);
			else
				gDropdownItemsChecked = 1 << (gCurrentTheme);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CULTURE:
		switch (widgetIndex) {
		case WIDX_HEIGHT_LABELS_DROPDOWN:
			gDropdownItemsFormat[0] = 1142;
			gDropdownItemsFormat[1] = 1142;
			gDropdownItemsArgs[0] = STR_UNITS;
			gDropdownItemsArgs[1] = STR_REAL_VALUES;

			window_options_show_dropdown(w, widget, 2);

			gDropdownItemsChecked = gConfigGeneral.show_height_as_units ? 1 : 2;
			break;
		case WIDX_CURRENCY_DROPDOWN:
			num_items = 10;

			for (i = 0; i < num_items; i++) {
				gDropdownItemsFormat[i] = 1142;
				gDropdownItemsArgs[i] = STR_POUNDS + i;
			}

			window_options_show_dropdown(w, widget, num_items);

			gDropdownItemsChecked = 1 << gConfigGeneral.currency_format;
			break;
		case WIDX_DISTANCE_DROPDOWN:
			gDropdownItemsFormat[0] = 1142;
			gDropdownItemsFormat[1] = 1142;
			gDropdownItemsArgs[0] = STR_IMPERIAL;
			gDropdownItemsArgs[1] = STR_METRIC;

			window_options_show_dropdown(w, widget, 2);

			gDropdownItemsChecked = 1 << gConfigGeneral.measurement_format;
			break;
		case WIDX_TEMPERATURE_DROPDOWN:
			gDropdownItemsFormat[0] = 1142;
			gDropdownItemsFormat[1] = 1142;
			gDropdownItemsArgs[0] = STR_CELSIUS;
			gDropdownItemsArgs[1] = STR_FAHRENHEIT;

			window_options_show_dropdown(w, widget, 2);

			gDropdownItemsChecked = 1 << gConfigGeneral.temperature_format;
			break;
		case WIDX_LANGUAGE_DROPDOWN:
			for (i = 1; i < LANGUAGE_COUNT; i++) {
				gDropdownItemsFormat[i - 1] = 2777;
				gDropdownItemsArgs[i - 1] = (sint32)language_names[i];
			}
			window_options_show_dropdown(w, widget, LANGUAGE_COUNT - 1);
			gDropdownItemsChecked = 1 << (gCurrentLanguage - 1);
			break;
		case WIDX_DATE_FORMAT_DROPDOWN:
			for (i = 0; i < 2; i++) {
				gDropdownItemsFormat[i] = 1142;
				gDropdownItemsArgs[i] = 5162 + i;
			}
			window_options_show_dropdown(w, widget, 2);
			gDropdownItemsChecked = 1 << (gConfigGeneral.date_format);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_AUDIO:
		switch (widgetIndex) {
		case WIDX_SOUND_DROPDOWN:
			audio_get_devices();

			// populate the list with the sound devices
			for (i = 0; i < gAudioDeviceCount; i++) {
				gDropdownItemsFormat[i] = 1142;
				gDropdownItemsArgs[i] = 1170 | ((uint64)(intptr_t)gAudioDevices[i].name << 16);
			}

			window_options_show_dropdown(w, widget, gAudioDeviceCount);

			gDropdownItemsChecked |= (1 << RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32));
			break;
		case WIDX_TITLE_MUSIC_DROPDOWN:
			num_items = 4;

			for (i = 0; i < num_items - 1; i++) {
				gDropdownItemsFormat[i] = 1142;
				gDropdownItemsArgs[i] = 2739 + i;
			}
			// Random title music
			gDropdownItemsFormat[3] = 1142;
			gDropdownItemsArgs[3] = 5126;

			window_options_show_dropdown(w, widget, num_items);

			gDropdownItemsChecked = 1 << gConfigSound.title_music;
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CONTROLS:
		break;

	case WINDOW_OPTIONS_PAGE_MISC:
		switch (widgetIndex) {
		case WIDX_AUTOSAVE_DROPDOWN:
			for (i = AUTOSAVE_EVERY_WEEK; i <= AUTOSAVE_NEVER; i++) {
				gDropdownItemsFormat[i] = 1142;
				gDropdownItemsArgs[i] = 2701 + i;
			}
			window_options_show_dropdown(w, widget, AUTOSAVE_NEVER + 1);
			gDropdownItemsChecked = 1 << gConfigGeneral.autosave_frequency;
			break;
		case WIDX_TITLE_SEQUENCE_DROPDOWN:
		/*	gDropdownItemsFormat[0] = 1142;
			gDropdownItemsArgs[0] = STR_TITLE_SEQUENCE_RCT2;
			gDropdownItemsFormat[1] = 1142;
			gDropdownItemsArgs[1] = STR_TITLE_SEQUENCE_OPENRCT2;
			window_options_show_dropdown(w, widget, 2);
			if (gConfigGeneral.title_sequence == TITLE_SEQUENCE_RCT2)
				gDropdownItemsChecked = 1 << 0;
			else if (gConfigGeneral.title_sequence == TITLE_SEQUENCE_OPENRCT2)
				gDropdownItemsChecked = 1 << 1;
			break;
		case WIDX_THEMES_DROPDOWN:*/
			num_items = gConfigTitleSequences.num_presets;

			for (i = 0; i < num_items; i++) {
				gDropdownItemsFormat[i] = 2777;
				gDropdownItemsArgs[i] = (uint32)&gConfigTitleSequences.presets[i].name;
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

			gDropdownItemsChecked = 1 << (gCurrentPreviewTitleSequence);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_TWITCH:
		break;
	}
}

/**
*
*  rct2: 0x006BB076
*/
static void window_options_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	if (dropdownIndex == -1)
		return;

	switch (w->page) {
	case WINDOW_OPTIONS_PAGE_DISPLAY:
		switch (widgetIndex) {
		case WIDX_RESOLUTION_DROPDOWN:
			{
				resolution *resolution = &gResolutions[dropdownIndex];
				if (resolution->width != gConfigGeneral.fullscreen_width || resolution->height != gConfigGeneral.fullscreen_height) {
					gConfigGeneral.fullscreen_width = resolution->width;
					gConfigGeneral.fullscreen_height = resolution->height;

					if (gConfigGeneral.fullscreen_mode == SDL_WINDOW_FULLSCREEN)
						platform_set_fullscreen_mode(SDL_WINDOW_FULLSCREEN);

					config_save_default();
					gfx_invalidate_screen();
				}
			}
			break;
		case WIDX_FULLSCREEN_DROPDOWN:
			if (dropdownIndex != gConfigGeneral.fullscreen_mode){
				platform_set_fullscreen_mode(dropdownIndex);

				gConfigGeneral.fullscreen_mode = (uint8)dropdownIndex;
				config_save_default();
				gfx_invalidate_screen();
			}
			break;
		case WIDX_CONSTRUCTION_MARKER_DROPDOWN:
			if (dropdownIndex != gConfigGeneral.construction_marker_colour) {
				gConfigGeneral.construction_marker_colour = (uint8)dropdownIndex;
				config_save_default();
				gfx_invalidate_screen();
			}
			break;
		case WIDX_THEMES_DROPDOWN:
			if (dropdownIndex != -1) {
				if (dropdownIndex == 0 || dropdownIndex == 1)
					dropdownIndex ^= 1;
				theme_change_preset(dropdownIndex);
			}
			config_save_default();
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CULTURE:
		switch (widgetIndex) {
		case WIDX_HEIGHT_LABELS_DROPDOWN:
			// reset flag and set it to 1 if height as units is selected
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) &= ~CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS;
			gConfigGeneral.show_height_as_units = 0;

			if (dropdownIndex == 0) {
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) |= CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS;
				gConfigGeneral.show_height_as_units = 1;
			}

			window_options_update_height_markers();
			break;
		case WIDX_CURRENCY_DROPDOWN:
			gConfigGeneral.currency_format = (sint8)dropdownIndex;
			config_save_default();
			gfx_invalidate_screen();
			break;
		case WIDX_DISTANCE_DROPDOWN:
			gConfigGeneral.measurement_format = (sint8)dropdownIndex;
			config_save_default();
			window_options_update_height_markers();
			break;
		case WIDX_TEMPERATURE_DROPDOWN:
			if (dropdownIndex != gConfigGeneral.temperature_format) {
				gConfigGeneral.temperature_format = (sint8)dropdownIndex;
				config_save_default();
				gfx_invalidate_screen();
			}
			break;
		case WIDX_LANGUAGE_DROPDOWN:
			if (dropdownIndex != gCurrentLanguage - 1) {
				language_open(dropdownIndex + 1);
				gConfigGeneral.language = dropdownIndex + 1;
				config_save_default();
				gfx_invalidate_screen();
			}
			break;
		case WIDX_DATE_FORMAT_DROPDOWN:
			if (dropdownIndex != gConfigGeneral.date_format) {
				gConfigGeneral.date_format = (uint8)dropdownIndex;
				config_save_default();
				gfx_invalidate_screen();
			}
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_AUDIO:
		switch (widgetIndex) {
		case WIDX_SOUND_DROPDOWN:
			audio_init2(dropdownIndex);
			if (dropdownIndex < gAudioDeviceCount) {
#ifdef USE_MIXER
				if (dropdownIndex == 0) {
					Mixer_Init(NULL);
					gConfigSound.device = NULL;
				}
				else {
					char* devicename = gAudioDevices[dropdownIndex].name;
					Mixer_Init(devicename);
					SafeFree(gConfigSound.device);
					gConfigSound.device = malloc(strlen(devicename) + 1);
					strcpy(gConfigSound.device, devicename);
				}
				config_save_default();
#endif
			}
			/*#ifdef _MSC_VER
			__asm movzx ax, dropdownIndex
			#else
			__asm__ ( "movzx ax, %[dropdownIndex]		 " : : [dropdownIndex] "g" ((char)dropdownIndex) );
			#endif
			// the switch replaces ax value
			RCT2_CALLPROC_EBPSAFE(0x006BA9B5);	// part of init audio*/

			window_invalidate(w);
			break;
		case WIDX_TITLE_MUSIC_DROPDOWN:
			if ((dropdownIndex == 1 || dropdownIndex == 3) && !platform_file_exists(get_file_path(PATH_ID_CSS50))) {
				window_error_open(2742, 2743);
			}
			else {
				gConfigSound.title_music = (sint8)dropdownIndex;
				config_save_default();
				window_invalidate(w);
			}

			stop_title_music();
			if (dropdownIndex != 0)
				start_title_music();
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CONTROLS:
		break;

	case WINDOW_OPTIONS_PAGE_MISC:
		switch (widgetIndex) {
		case WIDX_AUTOSAVE_DROPDOWN:
			if (dropdownIndex != gConfigGeneral.autosave_frequency) {
				gConfigGeneral.autosave_frequency = (uint8)dropdownIndex;
				config_save_default();
				window_invalidate(w);
			}
			break;
		case WIDX_TITLE_SEQUENCE_DROPDOWN:
			if (dropdownIndex != gCurrentPreviewTitleSequence) {
				title_sequence_change_preset(dropdownIndex);
				title_refresh_sequence();
				config_save_default();
				window_invalidate(w);
			}
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_TWITCH:
		break;
	}
}

/**
*  
*  rct2: 0x006BAD48
*/
static void window_options_invalidate(rct_window *w)
{
	rct_widget* widget;
	sint32 currentSoundDevice;

	colour_scheme_update(w);

	if (window_options_page_widgets[w->page] != w->widgets) {
		w->widgets = window_options_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}
	window_options_set_pressed_tab(w);
	w->disabled_widgets = 0;

	switch (w->page) {
	case WINDOW_OPTIONS_PAGE_DISPLAY:
		RCT2_GLOBAL(0x013CE952 + 16, uint16) = (uint16)gConfigGeneral.fullscreen_width;
		RCT2_GLOBAL(0x013CE952 + 18, uint16) = (uint16)gConfigGeneral.fullscreen_height;
		RCT2_GLOBAL(0x013CE952 + 12, uint16) = 2773 + gConfigGeneral.fullscreen_mode;

		// disable resolution dropdown on "Fullscreen (desktop)"
		if (gConfigGeneral.fullscreen_mode == 2){
			w->disabled_widgets |= (1 << WIDX_RESOLUTION_DROPDOWN);
			w->disabled_widgets |= (1 << WIDX_RESOLUTION);
		}
		else {
			w->disabled_widgets &= ~(1 << WIDX_RESOLUTION_DROPDOWN);
			w->disabled_widgets &= ~(1 << WIDX_RESOLUTION);
		}

		widget_set_checkbox_value(w, WIDX_TILE_SMOOTHING_CHECKBOX, (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_DISABLE_SMOOTH_LANDSCAPE) == 0);
		widget_set_checkbox_value(w, WIDX_GRIDLINES_CHECKBOX, RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES);
		widget_set_checkbox_value(w, WIDX_HARDWARE_DISPLAY_CHECKBOX, gConfigGeneral.hardware_display);
		widget_set_checkbox_value(w, WIDX_UNCAP_FPS_CHECKBOX, gConfigGeneral.uncap_fps);
		widget_set_checkbox_value(w, WIDX_MINIMIZE_FOCUS_LOSS, gConfigGeneral.minimize_fullscreen_focus_loss);

		// construction marker: celsius/fahrenheit
		window_options_display_widgets[WIDX_CONSTRUCTION_MARKER].image = STR_WHITE + RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CONSTRUCTION_MARKER, uint8);

		window_options_display_widgets[WIDX_RESOLUTION].type = WWT_DROPDOWN;
		window_options_display_widgets[WIDX_RESOLUTION_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_display_widgets[WIDX_FULLSCREEN].type = WWT_DROPDOWN;
		window_options_display_widgets[WIDX_FULLSCREEN_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_display_widgets[WIDX_TILE_SMOOTHING_CHECKBOX].type = WWT_CHECKBOX;
		window_options_display_widgets[WIDX_GRIDLINES_CHECKBOX].type = WWT_CHECKBOX;
		window_options_display_widgets[WIDX_CONSTRUCTION_MARKER].type = WWT_DROPDOWN;
		window_options_display_widgets[WIDX_CONSTRUCTION_MARKER_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_display_widgets[WIDX_HARDWARE_DISPLAY_CHECKBOX].type = WWT_CHECKBOX;
		window_options_display_widgets[WIDX_UNCAP_FPS_CHECKBOX].type = WWT_CHECKBOX;
		window_options_display_widgets[WIDX_MINIMIZE_FOCUS_LOSS].type = WWT_CHECKBOX;
		window_options_display_widgets[WIDX_THEMES].type = WWT_DROPDOWN;
		window_options_display_widgets[WIDX_THEMES_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_display_widgets[WIDX_THEMES_BUTTON].type = WWT_DROPDOWN_BUTTON;
		break;

	case WINDOW_OPTIONS_PAGE_CULTURE:
		// currency: pounds, dollars, etc. (10 total)
		RCT2_GLOBAL(0x013CE952 + 12, uint16) = STR_POUNDS + gConfigGeneral.currency_format;

		// distance: metric/imperial
		RCT2_GLOBAL(0x013CE952 + 14, uint16) = STR_IMPERIAL + gConfigGeneral.measurement_format;

		// temperature: celsius/fahrenheit
		RCT2_GLOBAL(0x013CE952 + 20, uint16) = STR_CELSIUS + gConfigGeneral.temperature_format;

		// height: units/real values
		RCT2_GLOBAL(0x013CE952 + 6, uint16) = gConfigGeneral.show_height_as_units ? STR_UNITS : STR_REAL_VALUES;

		window_options_culture_widgets[WIDX_LANGUAGE].type = WWT_DROPDOWN;
		window_options_culture_widgets[WIDX_LANGUAGE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_culture_widgets[WIDX_CURRENCY].type = WWT_DROPDOWN;
		window_options_culture_widgets[WIDX_CURRENCY_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_culture_widgets[WIDX_DISTANCE].type = WWT_DROPDOWN;
		window_options_culture_widgets[WIDX_DISTANCE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_culture_widgets[WIDX_TEMPERATURE].type = WWT_DROPDOWN;
		window_options_culture_widgets[WIDX_TEMPERATURE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_culture_widgets[WIDX_HEIGHT_LABELS].type = WWT_DROPDOWN;
		window_options_culture_widgets[WIDX_HEIGHT_LABELS_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_culture_widgets[WIDX_DATE_FORMAT].type = WWT_DROPDOWN;
		window_options_culture_widgets[WIDX_DATE_FORMAT_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		break;

	case WINDOW_OPTIONS_PAGE_AUDIO:
		currentSoundDevice = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, sint32);

		// sound devices
		if (currentSoundDevice == -1 || gAudioDeviceCount == 0) {
			RCT2_GLOBAL(0x013CE952, uint16) = STR_SOUND_NONE;
		}
		else {
			RCT2_GLOBAL(0x013CE952, uint16) = 1170;
			RCT2_GLOBAL(0x013CE952 + 2, uint32) = (uint32)gAudioDevices[currentSoundDevice].name;
		}

		// music: on/off
		RCT2_GLOBAL(0x013CE952 + 8, uint16) = STR_OFF + gConfigSound.ride_music;

		widget_set_checkbox_value(w, WIDX_SOUND_CHECKBOX, gConfigSound.sound);
		widget_set_checkbox_value(w, WIDX_MUSIC_CHECKBOX, gConfigSound.ride_music);

		if(w->frame_no == 0){ // initialize only on first frame, otherwise the scrollbars wont be able to be modified
			widget = &window_options_audio_widgets[WIDX_MASTER_VOLUME];
			w->scrolls[0].h_left = (sint16)ceil((gConfigSound.master_volume / 100.0f) * (w->scrolls[0].h_right - ((widget->right - widget->left) - 1)));
			widget = &window_options_audio_widgets[WIDX_MUSIC_VOLUME];
			w->scrolls[1].h_left = (sint16)ceil((gConfigSound.music_volume / 100.0f) * (w->scrolls[1].h_right - ((widget->right - widget->left) - 1)));
		}

		widget_scroll_update_thumbs(w, WIDX_MASTER_VOLUME);
		widget_scroll_update_thumbs(w, WIDX_MUSIC_VOLUME);

		window_options_audio_widgets[WIDX_SOUND].type = WWT_DROPDOWN;
		window_options_audio_widgets[WIDX_SOUND_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_audio_widgets[WIDX_SOUND_CHECKBOX].type = WWT_CHECKBOX;
		window_options_audio_widgets[WIDX_MUSIC_CHECKBOX].type = WWT_CHECKBOX;
		window_options_audio_widgets[WIDX_TITLE_MUSIC].type = WWT_DROPDOWN;
		window_options_audio_widgets[WIDX_TITLE_MUSIC_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_audio_widgets[WIDX_MASTER_VOLUME].type = WWT_SCROLL;
		window_options_audio_widgets[WIDX_MUSIC_VOLUME].type = WWT_SCROLL;
		break;

	case WINDOW_OPTIONS_PAGE_CONTROLS:
		widget_set_checkbox_value(w, WIDX_SCREEN_EDGE_SCROLLING, gConfigGeneral.edge_scrolling);
		widget_set_checkbox_value(w, WIDX_INVERT_DRAG, gConfigGeneral.invert_viewport_drag);
		widget_set_checkbox_value(w, WIDX_TOOLBAR_SHOW_FINANCES, gConfigInterface.toolbar_show_finances);
		widget_set_checkbox_value(w, WIDX_TOOLBAR_SHOW_RESEARCH, gConfigInterface.toolbar_show_research);
		widget_set_checkbox_value(w, WIDX_TOOLBAR_SHOW_CHEATS, gConfigInterface.toolbar_show_cheats);

		window_options_controls_widgets[WIDX_SCREEN_EDGE_SCROLLING].type = WWT_CHECKBOX;
		window_options_controls_widgets[WIDX_HOTKEY_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_controls_widgets[WIDX_TOOLBAR_SHOW_FINANCES].type = WWT_CHECKBOX;
		window_options_controls_widgets[WIDX_TOOLBAR_SHOW_RESEARCH].type = WWT_CHECKBOX;
		window_options_controls_widgets[WIDX_TOOLBAR_SHOW_CHEATS].type = WWT_CHECKBOX;
		break;

	case WINDOW_OPTIONS_PAGE_MISC:
		// unknown park flag can disable real name checkbox
		if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & 0x8000)
			w->disabled_widgets |= (1ULL << WIDX_REAL_NAME_CHECKBOX);

		// save plugin data checkbox: visible or not
		if (RCT2_GLOBAL(RCT2_ADDRESS_CUSTOM_OBJECTS_INSTALLED, uint8) == 1)
			window_options_misc_widgets[WIDX_SAVE_PLUGIN_DATA_CHECKBOX].type = WWT_EMPTY;
		else
			window_options_misc_widgets[WIDX_SAVE_PLUGIN_DATA_CHECKBOX].type = WWT_CHECKBOX;

		widget_set_checkbox_value(w, WIDX_SELECT_BY_TRACK_TYPE, gConfigInterface.select_by_track_type);
		widget_set_checkbox_value(w, WIDX_REAL_NAME_CHECKBOX, RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_SHOW_REAL_GUEST_NAMES);
		widget_set_checkbox_value(w, WIDX_SAVE_PLUGIN_DATA_CHECKBOX, gConfigGeneral.save_plugin_data);
		widget_set_checkbox_value(w, WIDX_TEST_UNFINISHED_TRACKS, gConfigGeneral.test_unfinished_tracks);
		widget_set_checkbox_value(w, WIDX_AUTO_STAFF_PLACEMENT, gConfigGeneral.auto_staff_placement);
		widget_set_checkbox_value(w, WIDX_DEBUGGING_TOOLS, gConfigGeneral.debugging_tools);

		window_options_misc_widgets[WIDX_REAL_NAME_CHECKBOX].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_SAVE_PLUGIN_DATA_CHECKBOX].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_AUTOSAVE].type = WWT_DROPDOWN;
		window_options_misc_widgets[WIDX_AUTOSAVE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_misc_widgets[WIDX_SELECT_BY_TRACK_TYPE].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_TEST_UNFINISHED_TRACKS].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_AUTO_STAFF_PLACEMENT].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_DEBUGGING_TOOLS].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_TITLE_SEQUENCE].type = WWT_DROPDOWN;
		window_options_misc_widgets[WIDX_TITLE_SEQUENCE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_misc_widgets[WIDX_TITLE_SEQUENCE_BUTTON].type = WWT_DROPDOWN_BUTTON;
		break;

	case WINDOW_OPTIONS_PAGE_TWITCH:
		widget_set_checkbox_value(w, WIDX_FOLLOWER_PEEP_NAMES_CHECKBOX, gConfigTwitch.enable_follower_peep_names);
		widget_set_checkbox_value(w, WIDX_FOLLOWER_PEEP_TRACKING_CHECKBOX, gConfigTwitch.enable_follower_peep_tracking);
		widget_set_checkbox_value(w, WIDX_CHAT_PEEP_NAMES_CHECKBOX, gConfigTwitch.enable_chat_peep_names);
		widget_set_checkbox_value(w, WIDX_CHAT_PEEP_TRACKING_CHECKBOX, gConfigTwitch.enable_chat_peep_tracking);
		widget_set_checkbox_value(w, WIDX_NEWS_CHECKBOX, gConfigTwitch.enable_news);

		window_options_twitch_widgets[WIDX_CHANNEL_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_options_twitch_widgets[WIDX_FOLLOWER_PEEP_NAMES_CHECKBOX].type = WWT_CHECKBOX;
		window_options_twitch_widgets[WIDX_FOLLOWER_PEEP_TRACKING_CHECKBOX].type = WWT_CHECKBOX;
		window_options_twitch_widgets[WIDX_CHAT_PEEP_NAMES_CHECKBOX].type = WWT_CHECKBOX;
		window_options_twitch_widgets[WIDX_CHAT_PEEP_TRACKING_CHECKBOX].type = WWT_CHECKBOX;
		window_options_twitch_widgets[WIDX_NEWS_CHECKBOX].type = WWT_CHECKBOX;
		break;
	}
}

static void window_options_update(rct_window *w)
{
	// Tab animation
	w->frame_no++;
	widget_invalidate(w, WIDX_TAB_1 + w->page);

	rct_widget *widget;
	if (w->page == WINDOW_OPTIONS_PAGE_AUDIO) {
		widget = &window_options_audio_widgets[WIDX_MASTER_VOLUME];
		uint8 master_volume = (uint8)(((float)w->scrolls[0].h_left / (w->scrolls[0].h_right - ((widget->right - widget->left) - 1))) * 100);
		widget = &window_options_audio_widgets[WIDX_MUSIC_VOLUME];
		uint8 music_volume = (uint8)(((float)w->scrolls[1].h_left / (w->scrolls[1].h_right - ((widget->right - widget->left) - 1))) * 100);
		if (master_volume != gConfigSound.master_volume) {
			gConfigSound.master_volume = master_volume;
			config_save_default();
		}
		if (music_volume != gConfigSound.music_volume) {
			gConfigSound.music_volume = music_volume;
			config_save_default();
		}
		widget_invalidate(w, WIDX_MASTER_VOLUME);
		widget_invalidate(w, WIDX_MUSIC_VOLUME);
	}
}

/**
*
*  rct2: 0x006BAEB4
*/
static void window_options_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_options_draw_tab_images(dpi, w);

	switch (w->page) {
	case WINDOW_OPTIONS_PAGE_DISPLAY:
		gfx_draw_string_left(dpi, STR_DISPLAY_RESOLUTION, w, w->colours[1], w->x + 10, w->y + window_options_display_widgets[WIDX_RESOLUTION].top + 1);
		gfx_draw_string_left(dpi, STR_FULLSCREEN_MODE, w, w->colours[1], w->x + 10, w->y + window_options_display_widgets[WIDX_FULLSCREEN].top + 1);
		gfx_draw_string_left(dpi, STR_CONSTRUCTION_MARKER, w, w->colours[1], w->x + 10, w->y + window_options_display_widgets[WIDX_CONSTRUCTION_MARKER].top + 1);

		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = (uint32)&gConfigThemes.presets[gCurrentTheme].name;
		gfx_draw_string_left(dpi, 5238, NULL, w->colours[1], w->x + 10, w->y + window_options_display_widgets[WIDX_THEMES].top + 1);
		gfx_draw_string_left_clipped(
			dpi,
			1170,
			(void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS,
			w->colours[1],
			w->x + window_options_display_widgets[WIDX_THEMES].left + 1,
			w->y + window_options_display_widgets[WIDX_THEMES].top,
			window_options_display_widgets[WIDX_THEMES_DROPDOWN].left - window_options_display_widgets[WIDX_THEMES].left - 4
			);
		break;
	case WINDOW_OPTIONS_PAGE_CULTURE:
		gfx_draw_string_left(dpi, 2776, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_LANGUAGE].top + 1);

		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
		gfx_draw_string(
			dpi,
			(char*)language_names[gCurrentLanguage],
			w->colours[1],
			w->x + window_options_culture_widgets[WIDX_LANGUAGE].left + 1,
			w->y + window_options_culture_widgets[WIDX_LANGUAGE].top
		);

		gfx_draw_string_left(dpi, STR_CURRENCY, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_CURRENCY].top + 1);
		gfx_draw_string_left(dpi, STR_DISTANCE_AND_SPEED, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_DISTANCE].top + 1);
		gfx_draw_string_left(dpi, STR_TEMPERATURE, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_TEMPERATURE].top + 1);
		gfx_draw_string_left(dpi, STR_HEIGHT_LABELS, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_HEIGHT_LABELS].top + 1);
		gfx_draw_string_left(dpi, 5161, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_DATE_FORMAT].top + 1);
		gfx_draw_string_left(
			dpi,
			5162 + gConfigGeneral.date_format,
			NULL,
			w->colours[1],
			w->x + window_options_culture_widgets[WIDX_DATE_FORMAT].left + 1,
			w->y + window_options_culture_widgets[WIDX_DATE_FORMAT].top
			);
		break;
	case WINDOW_OPTIONS_PAGE_AUDIO:
		gfx_draw_string_left(dpi, 2738, w, w->colours[1], w->x + 10, w->y + window_options_audio_widgets[WIDX_TITLE_MUSIC].top + 1);
		gfx_draw_string_left(
			dpi,
			(gConfigSound.title_music == 3 ? 5126 : 2739 + gConfigSound.title_music),
			NULL,
			w->colours[1],
			w->x + window_options_audio_widgets[WIDX_TITLE_MUSIC].left + 1,
			w->y + window_options_audio_widgets[WIDX_TITLE_MUSIC].top
		);
		break;
	case WINDOW_OPTIONS_PAGE_MISC:
		gfx_draw_string_left(dpi, 2700, w, w->colours[1], w->x + 10, w->y + window_options_misc_widgets[WIDX_AUTOSAVE].top + 1);
		gfx_draw_string_left(
			dpi,
			2701 + gConfigGeneral.autosave_frequency,
			NULL,
			w->colours[1],
			w->x + window_options_misc_widgets[WIDX_AUTOSAVE].left + 1,
			w->y + window_options_misc_widgets[WIDX_AUTOSAVE].top
		);
		
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32) = (uint32)&gConfigTitleSequences.presets[gCurrentPreviewTitleSequence].name;
		gfx_draw_string_left(dpi, STR_TITLE_SEQUENCE, w, w->colours[1], w->x + 10, w->y + window_options_misc_widgets[WIDX_TITLE_SEQUENCE].top + 1);
		gfx_draw_string_left_clipped(
			dpi,
			1170,
			(void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS,
			w->colours[1],
			w->x + window_options_misc_widgets[WIDX_TITLE_SEQUENCE].left + 1,
			w->y + window_options_misc_widgets[WIDX_TITLE_SEQUENCE].top,
			window_options_misc_widgets[WIDX_TITLE_SEQUENCE_DROPDOWN].left - window_options_misc_widgets[WIDX_TITLE_SEQUENCE].left - 4
		);
		break;
	}
}

// helper function, all dropdown boxes have similar properties
static void window_options_show_dropdown(rct_window *w, rct_widget *widget, int num_items)
{
	window_dropdown_show_text_custom_width(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		w->colours[1],
		DROPDOWN_FLAG_STAY_OPEN,
		num_items,
		widget->right - widget->left - 3
	);
}

static void window_options_update_height_markers() 
{
	RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, uint16) = gConfigGeneral.show_height_as_units ?
		0 : (gConfigGeneral.measurement_format + 1) * 256;
	config_save_default();
	gfx_invalidate_screen();
}

static void window_options_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	if (w->page == WINDOW_OPTIONS_PAGE_AUDIO) {
		*width = 1000;
	}
}

static void window_options_text_input(rct_window *w, int widgetIndex, char *text)
{
	if (text == NULL)
		return;

	if (widgetIndex == WIDX_CHANNEL_BUTTON) {
		if (gConfigTwitch.channel != NULL)
			free(gConfigTwitch.channel);
		gConfigTwitch.channel = _strdup(text);
		config_save_default();
	}
}

#pragma region Common

static void window_options_set_page(rct_window *w, int page)
{
	w->page = page;
	w->frame_no = 0;
	w->enabled_widgets = window_options_page_enabled_widgets[page];
	w->pressed_widgets = 0;
	w->widgets = window_options_page_widgets[page];

	window_invalidate(w);
	window_event_resize_call(w);
	window_event_invalidate_call(w);
	window_init_scroll_widgets(w);
	window_invalidate(w);
}

static void window_options_set_pressed_tab(rct_window *w)
{
	int i;
	for (i = 0; i < WINDOW_OPTIONS_PAGE_COUNT; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_options_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int page, int spriteIndex)
{
	int widgetIndex = WIDX_TAB_1 + page;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		if (w->page == page) {
			int frame = w->frame_no / window_options_tab_animation_divisor[w->page];
			spriteIndex += (frame % window_options_tab_animation_frames[w->page]);
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

static void window_options_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_DISPLAY, 5442);
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_CULTURE, 5229);
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_AUDIO, 5335);
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_CONTROLS, 5201);
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_MISC, 5205);
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_TWITCH, SPR_G2_TAB_TWITCH);
}

#pragma endregion
