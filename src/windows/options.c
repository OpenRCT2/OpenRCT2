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

/**
 * To better group the options together and allow the window to be scalable with additional OpenRCT2 options, the window has
 * been changed to a tab interface similar to the options window seen in Locomotion.
 *
 * TODO Some parts, particularly the string handling and order of widgets needs reorganising.
 *      Padding between the widgets and the window needs reducing, an artifact from originally being inside group boxes.
 */

#include "../addresses.h"
#include "../audio/audio.h"
#include "../audio/mixer.h"
#include "../config.h"
#include "../drawing/drawing.h"
#include "../interface/themes.h"
#include "../interface/title_sequences.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/currency.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../platform/platform.h"
#include "../sprites.h"
#include "../title.h"
#include "dropdown.h"
#include "error.h"
#include "../util/util.h"

enum WINDOW_OPTIONS_PAGE {
	WINDOW_OPTIONS_PAGE_DISPLAY,
	WINDOW_OPTIONS_PAGE_RENDERING,
	WINDOW_OPTIONS_PAGE_CULTURE,
	WINDOW_OPTIONS_PAGE_AUDIO,
	WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE,
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
	WIDX_TAB_7,

	WIDX_PAGE_START,

	// Display
	WIDX_HARDWARE_GROUP = WIDX_PAGE_START,
	WIDX_FULLSCREEN,
	WIDX_FULLSCREEN_DROPDOWN,
	WIDX_RESOLUTION,
	WIDX_RESOLUTION_DROPDOWN,
	WIDX_SCALE,
	WIDX_SCALE_UP,
	WIDX_SCALE_DOWN,
	WIDX_DRAWING_ENGINE,
	WIDX_DRAWING_ENGINE_DROPDOWN,
	WIDX_SCALE_QUALITY,
	WIDX_SCALE_QUALITY_DROPDOWN,
	WIDX_SCALE_USE_NN_AT_INTEGER_SCALES_CHECKBOX,
	WIDX_STEAM_OVERLAY_PAUSE,
	WIDX_UNCAP_FPS_CHECKBOX,
	WIDX_SHOW_FPS_CHECKBOX,
	WIDX_MINIMIZE_FOCUS_LOSS,

	// Rendering
	WIDX_RENDERING_GROUP = WIDX_PAGE_START,
	WIDX_TILE_SMOOTHING_CHECKBOX,
	WIDX_GRIDLINES_CHECKBOX,
	WIDX_CONSTRUCTION_MARKER,
	WIDX_CONSTRUCTION_MARKER_DROPDOWN,
	WIDX_DAY_NIGHT_CHECKBOX,
	WIDX_UPPER_CASE_BANNERS_CHECKBOX,
	WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX,

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
	WIDX_AUDIO_FOCUS_CHECKBOX,
	WIDX_TITLE_MUSIC,
	WIDX_TITLE_MUSIC_DROPDOWN,
	WIDX_SOUND_VOLUME,
	WIDX_MUSIC_VOLUME,

	// Controls and interface
	WIDX_CONTROLS_GROUP = WIDX_PAGE_START,
	WIDX_SCREEN_EDGE_SCROLLING,
	WIDX_TRAP_CURSOR,
	WIDX_INVERT_DRAG,
	WIDX_HOTKEY_DROPDOWN,
	WIDX_THEMES_GROUP,
	WIDX_THEMES,
	WIDX_THEMES_DROPDOWN,
	WIDX_THEMES_BUTTON,
	WIDX_TOOLBAR_BUTTONS_GROUP,
	WIDX_TOOLBAR_SHOW_FINANCES,
	WIDX_TOOLBAR_SHOW_RESEARCH,
	WIDX_TOOLBAR_SHOW_CHEATS,
	WIDX_TOOLBAR_SHOW_NEWS,
	WIDX_SELECT_BY_TRACK_TYPE,
	WIDX_SCENARIO_GROUPING,
	WIDX_SCENARIO_GROUPING_DROPDOWN,
	WIDX_SCENARIO_UNLOCKING,

	// Misc
	WIDX_REAL_NAME_CHECKBOX = WIDX_PAGE_START,
	WIDX_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM,
	WIDX_SAVE_PLUGIN_DATA_CHECKBOX,
	WIDX_TEST_UNFINISHED_TRACKS,
	WIDX_DEBUGGING_TOOLS,
	WIDX_STAY_CONNECTED_AFTER_DESYNC,
	WIDX_AUTOSAVE,
	WIDX_AUTOSAVE_DROPDOWN,
	WIDX_TITLE_SEQUENCE,
	WIDX_TITLE_SEQUENCE_DROPDOWN,
	WIDX_TITLE_SEQUENCE_BUTTON,
	WIDX_AUTO_STAFF_PLACEMENT,
	WIDX_HANDYMEN_MOW_DEFAULT,
	WIDX_AUTO_OPEN_SHOPS,
	WIDX_DEFAULT_INSPECTION_INTERVAL,
	WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN,

	// Twitch
	WIDX_CHANNEL_BUTTON = WIDX_PAGE_START,
	WIDX_FOLLOWER_PEEP_NAMES_CHECKBOX,
	WIDX_FOLLOWER_PEEP_TRACKING_CHECKBOX,
	WIDX_CHAT_PEEP_NAMES_CHECKBOX,
	WIDX_CHAT_PEEP_TRACKING_CHECKBOX,
	WIDX_NEWS_CHECKBOX
};

#define WW 			310
#define WH 			332

#define MAIN_OPTIONS_WIDGETS \
	{ WWT_FRAME,			0,	0,		WW-1,	0,		WH-1,	STR_NONE,			STR_NONE }, \
	{ WWT_CAPTION,			0,	1,		WW-2,	1,		14,		STR_OPTIONS_TITLE,	STR_WINDOW_TITLE_TIP }, \
	{ WWT_CLOSEBOX,			0,	WW-13,	WW-3,	2,		13,		STR_CLOSE_X,		STR_CLOSE_WINDOW_TIP }, \
	{ WWT_RESIZE,			1,	0,		WW-1,	43,		WH-1,	0xFFFFFFFF,			STR_NONE }, \
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,			STR_OPTIONS_DISPLAY_TIP }, \
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,			STR_OPTIONS_RENDERING_TIP }, \
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,			STR_OPTIONS_CULTURE_TIP }, \
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,			STR_OPTIONS_AUDIO_TIP }, \
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,			STR_OPTIONS_CONTROLS_AND_INTERFACE_TIP }, \
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,			STR_OPTIONS_MISCELLANEOUS_TIP }, \
	{ WWT_TAB,				1,	189,	209,	17,		43,		0x2000144E,			STR_OPTIONS_TWITCH_TIP }

static rct_widget window_options_display_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_GROUPBOX,			1,	5,		304,	53,		205,	STR_HARDWARE_GROUP,		STR_NONE },					// Hardware group

	{ WWT_DROPDOWN,			1,	155,	299,	68,		79,		871,					STR_NONE },					// Fullscreen
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	69,		78,		STR_DROPDOWN_GLYPH,		STR_FULLSCREEN_MODE_TIP },

		{ WWT_DROPDOWN,			1,	155,	299,	83,		94,		STR_RESOLUTION_X_BY_Y,	STR_NONE },				// Resolution
		{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	84,		93,		STR_DROPDOWN_GLYPH,		STR_DISPLAY_RESOLUTION_TIP },

	{ WWT_SPINNER,			1,	155,	299,	98,		109,	STR_NONE,				STR_WINDOW_SCALE_TIP },		// Scale spinner
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	99,		103,	STR_NUMERIC_UP,			STR_NONE },					// Scale spinner up
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	104,	108,	STR_NUMERIC_DOWN,		STR_NONE },					// Scale spinner down

	{ WWT_DROPDOWN,			1,	155,	299,	113,	124,	STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	114,	123,	STR_DROPDOWN_GLYPH,		STR_DRAWING_ENGINE_TIP },

		{ WWT_DROPDOWN,			1,	155,	299,	128,	139,	STR_NONE,						STR_NONE },							// Scaling quality hint
		{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	129,	138,	STR_DROPDOWN_GLYPH,				STR_SCALE_QUALITY_TIP },
		{ WWT_CHECKBOX,			1,	25,		290,	143,	154,	STR_USE_NN_AT_INTEGER_SCALE,	STR_USE_NN_AT_INTEGER_SCALE_TIP },	// Use nn scaling at integer scales

		{ WWT_CHECKBOX,			1,	25,		290,	158,	169,	STR_STEAM_OVERLAY_PAUSE,		STR_STEAM_OVERLAY_PAUSE_TIP },		// Pause on steam overlay

	{ WWT_CHECKBOX,			1,	10,		290,	174,	185,	STR_UNCAP_FPS,			STR_UNCAP_FPS_TIP },		// Uncap fps
	{ WWT_CHECKBOX,			1,	155,	299,	174,	185,	STR_SHOW_FPS,			STR_SHOW_FPS_TIP },			// Show fps
	{ WWT_CHECKBOX,			1,	10,		290,	189,	200,	STR_MINIMISE_FULLSCREEN_ON_FOCUS_LOSS,	STR_MINIMISE_FULLSCREEN_ON_FOCUS_LOSS_TIP },	// Minimise fullscreen focus loss


	{ WIDGETS_END },
};

static rct_widget window_options_rendering_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
#define FRAME_RENDERING_START 53
	{ WWT_GROUPBOX,			1,	5,      304,	FRAME_RENDERING_START + 0,	FRAME_RENDERING_START + 106,	STR_RENDERING_GROUP,			STR_NONE },								// Rendering group
	{ WWT_CHECKBOX,			1,	10,		290,	FRAME_RENDERING_START + 15,	FRAME_RENDERING_START + 26,		STR_TILE_SMOOTHING, 			STR_TILE_SMOOTHING_TIP },				// Landscape smoothing
	{ WWT_CHECKBOX,			1,	10,		290,	FRAME_RENDERING_START + 30,	FRAME_RENDERING_START + 41,		STR_GRIDLINES,					STR_GRIDLINES_TIP },					// Gridlines
	{ WWT_DROPDOWN,			1,	155,	299,	FRAME_RENDERING_START + 45,	FRAME_RENDERING_START + 55,		STR_NONE,						STR_NONE },								// Construction marker
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	FRAME_RENDERING_START + 46,	FRAME_RENDERING_START + 54,		STR_DROPDOWN_GLYPH,				STR_CONSTRUCTION_MARKER_COLOUR_TIP },
	{ WWT_CHECKBOX,			1,	10,		290,	FRAME_RENDERING_START + 60,	FRAME_RENDERING_START + 71,		STR_CYCLE_DAY_NIGHT,			STR_CYCLE_DAY_NIGHT_TIP },				// Cycle day-night
	{ WWT_CHECKBOX,			1,	10,		290,	FRAME_RENDERING_START + 75,	FRAME_RENDERING_START + 86,		STR_UPPERCASE_BANNERS,			STR_UPPERCASE_BANNERS_TIP },			// Uppercase banners
	{ WWT_CHECKBOX,			1,	10,		290,	FRAME_RENDERING_START + 90,	FRAME_RENDERING_START + 101,	STR_DISABLE_LIGHTNING_EFFECT,	STR_DISABLE_LIGHTNING_EFFECT_TIP },		// Disable lightning effect
#undef FRAME_RENDERING_START
	{ WIDGETS_END },
};

static rct_widget window_options_culture_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_DROPDOWN,			1,	155,	299,	53,		64,		STR_NONE,				STR_NONE },	// language
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	54,		63,		STR_DROPDOWN_GLYPH,		STR_LANGUAGE_TIP },
	{ WWT_DROPDOWN,			1,	155,	299,	68,		79,		871,					STR_NONE },	// Currency
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	69,		78,		STR_DROPDOWN_GLYPH,		STR_CURRENCY_TIP },
	{ WWT_DROPDOWN,			1,	155,	299,	83,		94,		872,					STR_NONE },	// Distance and speed
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	84,		93,		STR_DROPDOWN_GLYPH,		STR_DISTANCE_AND_SPEED_TIP },
	{ WWT_DROPDOWN,			1,	155,	299,	98,		110,	875,					STR_NONE },	// Temperature
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	99,		108,	STR_DROPDOWN_GLYPH,		STR_TEMPERATURE_FORMAT_TIP },
	{ WWT_DROPDOWN,			1,	155,	299,	113,	124,	868,					STR_NONE },	// Height labels
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	114,	123,	STR_DROPDOWN_GLYPH,		STR_HEIGHT_LABELS_UNITS_TIP },
	{ WWT_DROPDOWN,			1,	155,	299,	128,	139,	STR_NONE,				STR_NONE },	// Date format
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	129,	138,	STR_DROPDOWN_GLYPH,		STR_DATE_FORMAT_TIP },
	{ WIDGETS_END },
};

static rct_widget window_options_audio_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_DROPDOWN,			1,	10,		299,	53,		64,		865,					STR_NONE },						// Audio device
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	54,		63,		STR_DROPDOWN_GLYPH,		STR_AUDIO_DEVICE_TIP },
	{ WWT_CHECKBOX,			1,	10,		229,	69,		80,		STR_SOUND_EFFECTS,		STR_SOUND_EFFECTS_TIP },		// Enable / disable sound effects
	{ WWT_CHECKBOX,			1,	10,		229,	84,		95,		STR_RIDE_MUSIC,			STR_RIDE_MUSIC_TIP },			// Enable / disable ride music
	{ WWT_CHECKBOX,			1,	10,		229,	98,		110,	STR_AUDIO_FOCUS,		STR_AUDIO_FOCUS_TIP },			// Enable / disable audio disabled on focus lost
	{ WWT_DROPDOWN,			1,	155,	299,	112,	124,	STR_NONE,				STR_NONE },						// Title music
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	113,	123,	STR_DROPDOWN_GLYPH,		STR_TITLE_MUSIC_TIP },
	{ WWT_SCROLL,			1,	155,	299,	68,		80,		1,						STR_NONE }, 					// Sound effect volume
	{ WWT_SCROLL,			1,	155,	299,	83,		95,		1,						STR_NONE }, 					// Music volume
	{ WIDGETS_END },
};

static rct_widget window_options_controls_and_interface_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_GROUPBOX,			1,	5,      304,	53,			129,	STR_CONTROLS_GROUP,						STR_NONE },									// Controls group
	{ WWT_CHECKBOX,			2,	10,		299,	68,			79,		STR_SCREEN_EDGE_SCROLLING,				STR_SCREEN_EDGE_SCROLLING_TIP },			// Edge scrolling
	{ WWT_CHECKBOX,			2,	10,		299,	83,			94,		STR_TRAP_MOUSE,							STR_TRAP_MOUSE_TIP },						// Trap mouse
	{ WWT_CHECKBOX,			2,	10,		299,	98,			109,	STR_INVERT_RIGHT_MOUSE_DRAG,			STR_INVERT_RIGHT_MOUSE_DRAG_TIP },			// Invert right mouse dragging
	{ WWT_DROPDOWN_BUTTON,	1,	26,		185,	113,		124,	STR_HOTKEY,								STR_HOTKEY_TIP },							// Set hotkeys buttons

	{ WWT_GROUPBOX,			1,	5,      304,	133,		179,	STR_THEMES_GROUP,						STR_NONE },									// Toolbar buttons group
	{ WWT_DROPDOWN,			1,	155,	299,	147,		158,	STR_NONE,								STR_NONE },									// Themes
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	148,		157,	STR_DROPDOWN_GLYPH,						STR_CURRENT_THEME_TIP },
	{ WWT_DROPDOWN_BUTTON,	1,	10,		145,	163,		174,	STR_EDIT_THEMES_BUTTON,					STR_EDIT_THEMES_BUTTON_TIP },				// Themes button

	{ WWT_GROUPBOX,			1,	5,      304,	183,		245,	STR_TOOLBAR_BUTTONS_GROUP,				STR_NONE },									// Toolbar buttons group
	{ WWT_CHECKBOX,			2,	10,		145,	214,		225,	STR_FINANCES_BUTTON_ON_TOOLBAR,			STR_FINANCES_BUTTON_ON_TOOLBAR_TIP },		// Finances
	{ WWT_CHECKBOX,			2,	10,		145,	229,		240,	STR_RESEARCH_BUTTON_ON_TOOLBAR,			STR_RESEARCH_BUTTON_ON_TOOLBAR_TIP },		// Research
	{ WWT_CHECKBOX,			2,	155,	299,	214,		225,	STR_CHEATS_BUTTON_ON_TOOLBAR,			STR_CHEATS_BUTTON_ON_TOOLBAR_TIP },			// Cheats
	{ WWT_CHECKBOX,			2,	155,	299,	229,		240,	STR_SHOW_RECENT_MESSAGES_ON_TOOLBAR,	STR_SHOW_RECENT_MESSAGES_ON_TOOLBAR_TIP },	// Recent messages

	{ WWT_CHECKBOX,			2,	10,		299,	254,		265,	STR_SELECT_BY_TRACK_TYPE,				STR_SELECT_BY_TRACK_TYPE_TIP },				// Select by track type
	{ WWT_DROPDOWN,			2,	155,	299,	269,		280,	STR_NONE,								STR_NONE },									// Scenario select mode
	{ WWT_DROPDOWN_BUTTON,	2,	288,	298,	270,		279,	STR_DROPDOWN_GLYPH,						STR_SCENARIO_GROUPING_TIP },
	{ WWT_CHECKBOX,			2,	18,		299,	284,		295,	STR_OPTIONS_SCENARIO_UNLOCKING,			STR_SCENARIO_UNLOCKING_TIP },				// Unlocking of scenarios
	{ WIDGETS_END },
};

static rct_widget window_options_misc_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_CHECKBOX,			2,	10,		299,	54,		65,		STR_REAL_NAME,								STR_REAL_NAME_TIP },								// Show 'real' names of guests
	{ WWT_CHECKBOX,			2,	10,		299,	69,		80,		STR_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM,	STR_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM_TIP },	// Allow loading with incorrect checksum
	{ WWT_CHECKBOX,			2,	10,		299,	84,		95,		STR_SAVE_PLUGIN_DATA,						STR_SAVE_PLUGIN_DATA_TIP },							// Export plug-in objects with saved games
	{ WWT_CHECKBOX,			2,	10,		299,	99,		110,	STR_TEST_UNFINISHED_TRACKS,					STR_TEST_UNFINISHED_TRACKS_TIP },					// Test unfinished tracks
	{ WWT_CHECKBOX,			2,	10,		299,	114,	125,	STR_ENABLE_DEBUGGING_TOOLS,					STR_ENABLE_DEBUGGING_TOOLS_TIP },					// Enable debugging tools
	{ WWT_CHECKBOX,			2,	10,		299,	129,	140,	STR_STAY_CONNECTED_AFTER_DESYNC,			STR_STAY_CONNECTED_AFTER_DESYNC_TIP },				// Do not disconnect after the client desynchronises with the server
	{ WWT_DROPDOWN,			1,	155,	299,	144,	155,	STR_NONE,									STR_NONE },											// Autosave dropdown
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	145,	154,	STR_DROPDOWN_GLYPH,							STR_AUTOSAVE_FREQUENCY_TIP },						// Autosave dropdown button
	{ WWT_DROPDOWN,			1,	155,	299,	159,	170,	STR_NONE,									STR_NONE },											// Title sequence dropdown
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	160,	169,	STR_DROPDOWN_GLYPH,							STR_TITLE_SEQUENCE_TIP },							// Title seqeuence dropdown button
	{ WWT_DROPDOWN_BUTTON,	1,	26,		185,	174,	185,	STR_EDIT_TITLE_SEQUENCES_BUTTON,			STR_EDIT_TITLE_SEQUENCES_BUTTON_TIP },				// Edit title sequences button
	{ WWT_CHECKBOX,			2,	10,		299,	189,	200,	STR_AUTO_STAFF_PLACEMENT,					STR_AUTO_STAFF_PLACEMENT_TIP },						// Auto staff placement
	{ WWT_CHECKBOX,			2,	10,		299,	204,	215,	STR_HANDYMEN_MOW_BY_DEFAULT,				STR_HANDYMEN_MOW_BY_DEFAULT_TIP },					// Handymen mow by default
	{ WWT_CHECKBOX,			2,	10,		299,	219,	230,	STR_AUTO_OPEN_SHOPS,						STR_AUTO_OPEN_SHOPS_TIP },							// Automatically open shops & stalls
	{ WWT_DROPDOWN,			1,	155,	299,	234,	245,	STR_NONE,									STR_NONE },											// Default inspection time dropdown
	{ WWT_DROPDOWN_BUTTON,	1,	288,	298,	235,	244,	STR_DROPDOWN_GLYPH,							STR_DEFAULT_INSPECTION_INTERVAL_TIP },				// Default inspection time dropdown button
	{ WIDGETS_END },
};

static rct_widget window_options_twitch_widgets[] = {
	MAIN_OPTIONS_WIDGETS,
	{ WWT_DROPDOWN_BUTTON,	2,	10,		299,	54,		65,		STR_TWITCH_NAME,			STR_TWITCH_NAME_TIP },				// Twitch channel name
	{ WWT_CHECKBOX,			2,	10,		299,	69,		80,		STR_TWITCH_PEEP_FOLLOWERS,	STR_TWITCH_PEEP_FOLLOWERS_TIP },	// Twitch name peeps by follows
	{ WWT_CHECKBOX,			2,	10,		299,	84,		95,		STR_TWITCH_FOLLOWERS_TRACK,	STR_TWITCH_FOLLOWERS_TRACK_TIP },	// Twitch information on for follows
	{ WWT_CHECKBOX,			2,	10,		299,	99,		110,	STR_TWITCH_PEEP_CHAT,		STR_TWITCH_PEEP_CHAT_TIP },			// Twitch name peeps by chat
	{ WWT_CHECKBOX,			2,	10,		299,	114,	125,	STR_TWITCH_CHAT_TRACK,		STR_TWITCH_CHAT_TRACK_TIP  },		// Twitch information on for chat
	{ WWT_CHECKBOX,			2,	10,		299,	129,	140,	STR_TWITCH_CHAT_NEWS,		STR_TWITCH_CHAT_NEWS_TIP },			// Twitch chat !news as notifications in game
	{ WIDGETS_END },
};

rct_widget *window_options_page_widgets[] = {
	window_options_display_widgets,
	window_options_rendering_widgets,
	window_options_culture_widgets,
	window_options_audio_widgets,
	window_options_controls_and_interface_widgets,
	window_options_misc_widgets,
	window_options_twitch_widgets
};

#pragma endregion

const int window_options_tab_animation_divisor[] = { 4, 4, 8, 2, 2, 2, 1 };
const int window_options_tab_animation_frames[] = { 16, 8, 8, 16, 4, 16, 1 };

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
	(1 << WIDX_TAB_6) | \
	(1 << WIDX_TAB_7)

static uint32 window_options_page_enabled_widgets[] = {
	MAIN_OPTIONS_ENABLED_WIDGETS |
	(1 << WIDX_RESOLUTION) |
	(1 << WIDX_RESOLUTION_DROPDOWN) |
	(1 << WIDX_FULLSCREEN) |
	(1 << WIDX_FULLSCREEN_DROPDOWN) |
	(1 << WIDX_TILE_SMOOTHING_CHECKBOX) |
	(1 << WIDX_GRIDLINES_CHECKBOX) |
	(1 << WIDX_DRAWING_ENGINE) |
	(1 << WIDX_DRAWING_ENGINE_DROPDOWN) |
	(1 << WIDX_UNCAP_FPS_CHECKBOX) |
	(1 << WIDX_SHOW_FPS_CHECKBOX) |
	(1 << WIDX_MINIMIZE_FOCUS_LOSS) |
	(1 << WIDX_STEAM_OVERLAY_PAUSE) |
	(1 << WIDX_SCALE) |
	(1 << WIDX_SCALE_UP) |
	(1 << WIDX_SCALE_DOWN) |
	(1 << WIDX_SCALE_QUALITY) |
	(1 << WIDX_SCALE_QUALITY_DROPDOWN) |
	(1 << WIDX_SCALE_USE_NN_AT_INTEGER_SCALES_CHECKBOX),

	MAIN_OPTIONS_ENABLED_WIDGETS |
	(1 << WIDX_TILE_SMOOTHING_CHECKBOX) |
	(1 << WIDX_GRIDLINES_CHECKBOX) |
	(1 << WIDX_CONSTRUCTION_MARKER) |
	(1 << WIDX_CONSTRUCTION_MARKER_DROPDOWN) |
	(1 << WIDX_DAY_NIGHT_CHECKBOX) |
	(1 << WIDX_UPPER_CASE_BANNERS_CHECKBOX) |
	(1 << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX),

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
	(1 << WIDX_AUDIO_FOCUS_CHECKBOX) |
	(1 << WIDX_TITLE_MUSIC) |
	(1 << WIDX_TITLE_MUSIC_DROPDOWN),

	MAIN_OPTIONS_ENABLED_WIDGETS |
	(1 << WIDX_SCREEN_EDGE_SCROLLING) |
	(1 << WIDX_TRAP_CURSOR) |
	(1 << WIDX_INVERT_DRAG) |
	(1 << WIDX_HOTKEY_DROPDOWN) |
	(1 << WIDX_TOOLBAR_SHOW_FINANCES) |
	(1 << WIDX_TOOLBAR_SHOW_RESEARCH) |
	(1 << WIDX_TOOLBAR_SHOW_CHEATS) |
	(1 << WIDX_TOOLBAR_SHOW_NEWS) |
	(1 << WIDX_THEMES) |
	(1 << WIDX_THEMES_DROPDOWN) |
	(1 << WIDX_THEMES_BUTTON) |
	(1 << WIDX_SELECT_BY_TRACK_TYPE) |
	(1 << WIDX_SCENARIO_GROUPING) |
	(1 << WIDX_SCENARIO_GROUPING_DROPDOWN) |
	(1 << WIDX_SCENARIO_UNLOCKING),

	MAIN_OPTIONS_ENABLED_WIDGETS |
	(1 << WIDX_REAL_NAME_CHECKBOX) |
	(1 << WIDX_SAVE_PLUGIN_DATA_CHECKBOX) |
	(1 << WIDX_AUTOSAVE) |
	(1 << WIDX_AUTOSAVE_DROPDOWN) |
	(1 << WIDX_TEST_UNFINISHED_TRACKS) |
	(1 << WIDX_AUTO_STAFF_PLACEMENT) |
	(1 << WIDX_HANDYMEN_MOW_DEFAULT) |
	(1 << WIDX_DEBUGGING_TOOLS) |
	(1 << WIDX_TITLE_SEQUENCE) |
	(1 << WIDX_TITLE_SEQUENCE_DROPDOWN) |
	(1 << WIDX_TITLE_SEQUENCE_BUTTON) |
	(1 << WIDX_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM) |
	(1 << WIDX_STAY_CONNECTED_AFTER_DESYNC) |
	(1 << WIDX_AUTO_OPEN_SHOPS) |
	(1 << WIDX_DEFAULT_INSPECTION_INTERVAL) |
	(1 << WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN),

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

	w = window_create_centred(WW, WH, &window_options_events, WC_OPTIONS, 0);
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
	case WIDX_TAB_7:
		window_options_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	}

	switch (w->page) {
	case WINDOW_OPTIONS_PAGE_DISPLAY:
		switch (widgetIndex) {
		case WIDX_UNCAP_FPS_CHECKBOX:
			gConfigGeneral.uncap_fps ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_SHOW_FPS_CHECKBOX:
			gConfigGeneral.show_fps ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_MINIMIZE_FOCUS_LOSS:
			gConfigGeneral.minimize_fullscreen_focus_loss ^= 1;
			platform_refresh_video();
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_STEAM_OVERLAY_PAUSE:
			gConfigGeneral.steam_overlay_pause ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_SCALE_USE_NN_AT_INTEGER_SCALES_CHECKBOX:
			gConfigGeneral.use_nn_at_integer_scales ^= 1;
			config_save_default();
			gfx_invalidate_screen();
			platform_trigger_resize();
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_RENDERING:
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
		case WIDX_DAY_NIGHT_CHECKBOX:
			gConfigGeneral.day_night_cycle ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_UPPER_CASE_BANNERS_CHECKBOX:
			gConfigGeneral.upper_case_banners ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX:
			gConfigGeneral.disable_lightning_effect ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CULTURE:
		break;

	case WINDOW_OPTIONS_PAGE_AUDIO:
		switch (widgetIndex) {
		case WIDX_SOUND_CHECKBOX:
			gConfigSound.sound_enabled = !gConfigSound.sound_enabled;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_MUSIC_CHECKBOX:
			gConfigSound.ride_music_enabled = !gConfigSound.ride_music_enabled;
			if (!gConfigSound.ride_music_enabled) {
				audio_stop_ride_music();
			}
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_AUDIO_FOCUS_CHECKBOX:
			gConfigSound.audio_focus = !gConfigSound.audio_focus;
			config_save_default();
			window_invalidate(w);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE:
		switch (widgetIndex) {
		case WIDX_HOTKEY_DROPDOWN:
			window_shortcut_keys_open();
			break;
		case WIDX_SCREEN_EDGE_SCROLLING:
			gConfigGeneral.edge_scrolling ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_TRAP_CURSOR:
			gConfigGeneral.trap_cursor ^= 1;
			config_save_default();
			SDL_SetWindowGrab(gWindow, gConfigGeneral.trap_cursor ? SDL_TRUE : SDL_FALSE);
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
		case WIDX_TOOLBAR_SHOW_NEWS:
			gConfigInterface.toolbar_show_news ^= 1;
			config_save_default();
			window_invalidate(w);
			window_invalidate_by_class(WC_TOP_TOOLBAR);
			break;
		case WIDX_INVERT_DRAG:
			gConfigGeneral.invert_viewport_drag ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_THEMES_BUTTON:
			window_themes_open();
			window_invalidate(w);
			break;
		case WIDX_SELECT_BY_TRACK_TYPE:
			gConfigInterface.select_by_track_type ^= 1;
			config_save_default();
			window_invalidate(w);
			window_invalidate_by_class(WC_RIDE);
			window_invalidate_by_class(WC_CONSTRUCT_RIDE);
			break;
		case WIDX_SCENARIO_UNLOCKING:
			gConfigGeneral.scenario_unlocking_enabled ^= 1;
			config_save_default();
			window_close_by_class(WC_SCENARIO_SELECT);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_MISC:
		switch (widgetIndex) {
		case WIDX_DEBUGGING_TOOLS:
			gConfigGeneral.debugging_tools ^= 1;
			config_save_default();
			gfx_invalidate_screen();
			break;
		case WIDX_TEST_UNFINISHED_TRACKS:
			gConfigGeneral.test_unfinished_tracks ^= 1;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_REAL_NAME_CHECKBOX:
			peep_update_names(
				!(gParkFlags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES)
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
		case WIDX_HANDYMEN_MOW_DEFAULT:
			gConfigGeneral.handymen_mow_default = !gConfigGeneral.handymen_mow_default;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_TITLE_SEQUENCE_BUTTON:
			window_title_editor_open(0);
			break;
		case WIDX_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM:
			gConfigGeneral.allow_loading_with_incorrect_checksum = !gConfigGeneral.allow_loading_with_incorrect_checksum;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_STAY_CONNECTED_AFTER_DESYNC:
			gConfigNetwork.stay_connected = !gConfigNetwork.stay_connected;
			config_save_default();
			window_invalidate(w);
			break;
		case WIDX_AUTO_OPEN_SHOPS:
			gConfigGeneral.auto_open_shops = !gConfigGeneral.auto_open_shops;
			config_save_default();
			window_invalidate(w);
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

				gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;

				uint16 *args = (uint16*)&gDropdownItemsArgs[i];
				args[0] = 839;
				args[1] = resolution->width;
				args[2] = resolution->height;

				if (resolution->width == gConfigGeneral.fullscreen_width && resolution->height == gConfigGeneral.fullscreen_height)
					selectedResolution = i;
			}

			window_options_show_dropdown(w, widget, gNumResolutions);

			if (selectedResolution != -1 && selectedResolution < 32) {
				dropdown_set_checked(selectedResolution, true);
			}
		}

		break;
		case WIDX_FULLSCREEN_DROPDOWN:
			gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsFormat[2] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[0] = 2773;
			gDropdownItemsArgs[1] = 2774;
			gDropdownItemsArgs[2] = 2775;

			window_options_show_dropdown(w, widget, 3);

			dropdown_set_checked(gConfigGeneral.fullscreen_mode, true);
			break;
		case WIDX_DRAWING_ENGINE_DROPDOWN:
		{
			int numItems = 3;
#ifdef DISABLE_OPENGL
			numItems = 2;
#endif

			for (int i = 0; i < numItems; i++) {
				gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
				gDropdownItemsArgs[i] = DrawingEngineStringIds[i];
			}
			window_options_show_dropdown(w, widget, numItems);
			dropdown_set_checked(gConfigGeneral.drawing_engine, true);
			break;
		}
		case WIDX_SCALE_UP:
			gConfigGeneral.window_scale += 0.25f;
			config_save_default();
			gfx_invalidate_screen();
			platform_trigger_resize();
			break;
		case WIDX_SCALE_DOWN:
			gConfigGeneral.window_scale -= 0.25f;
			gConfigGeneral.window_scale = max(0.5f, gConfigGeneral.window_scale);
			config_save_default();
			gfx_invalidate_screen();
			platform_trigger_resize();
			break;
		case WIDX_SCALE_QUALITY_DROPDOWN:
			gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsFormat[2] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[0] = STR_SCALING_QUALITY_NN;
			gDropdownItemsArgs[1] = STR_SCALING_QUALITY_LINEAR;
			gDropdownItemsArgs[2] = STR_SCALING_QUALITY_ANISOTROPIC;

			window_options_show_dropdown(w, widget, 3);

			dropdown_set_checked(gConfigGeneral.scale_quality, true);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_RENDERING:
		switch (widgetIndex) {
		case WIDX_CONSTRUCTION_MARKER_DROPDOWN:
			gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[0] = STR_WHITE;
			gDropdownItemsArgs[1] = STR_TRANSLUCENT;

			window_options_show_dropdown(w, widget, 2);

			dropdown_set_checked(gConfigGeneral.construction_marker_colour, true);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CULTURE:
		switch (widgetIndex) {
		case WIDX_HEIGHT_LABELS_DROPDOWN:
			gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[0] = STR_UNITS;
			gDropdownItemsArgs[1] = STR_REAL_VALUES;

			window_options_show_dropdown(w, widget, 2);

			dropdown_set_checked(gConfigGeneral.show_height_as_units ? 0 : 1, true);
			break;
		case WIDX_CURRENCY_DROPDOWN:
			num_items = CURRENCY_END + 1; // All the currencies plus the separator
			int num_ordinary_currencies = CURRENCY_END - 1; // All the currencies except custom currency

			for (i = 0; i < num_ordinary_currencies; i++) {
				gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
				gDropdownItemsArgs[i] = CurrencyDescriptors[i].stringId;
			}

			gDropdownItemsFormat[num_ordinary_currencies] = DROPDOWN_SEPARATOR;

			gDropdownItemsFormat[num_ordinary_currencies+1] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[num_ordinary_currencies+1] = CurrencyDescriptors[CURRENCY_CUSTOM].stringId;


			window_options_show_dropdown(w, widget, num_items);

			if(gConfigGeneral.currency_format == CURRENCY_CUSTOM){
				dropdown_set_checked(gConfigGeneral.currency_format+1, true);
			} else {
				dropdown_set_checked(gConfigGeneral.currency_format, true);
			}
			break;
		case WIDX_DISTANCE_DROPDOWN:
			gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsFormat[2] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[0] = STR_IMPERIAL;
			gDropdownItemsArgs[1] = STR_METRIC;
			gDropdownItemsArgs[2] = STR_SI;

			window_options_show_dropdown(w, widget, 3);

			dropdown_set_checked(gConfigGeneral.measurement_format, true);
			break;
		case WIDX_TEMPERATURE_DROPDOWN:
			gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[0] = STR_CELSIUS;
			gDropdownItemsArgs[1] = STR_FAHRENHEIT;

			window_options_show_dropdown(w, widget, 2);

			dropdown_set_checked(gConfigGeneral.temperature_format, true);
			break;
		case WIDX_LANGUAGE_DROPDOWN:
			for (i = 1; i < LANGUAGE_COUNT; i++) {
				gDropdownItemsFormat[i - 1] = 2777;
				gDropdownItemsArgs[i - 1] = (sint32)LanguagesDescriptors[i].native_name;
			}
			window_options_show_dropdown(w, widget, LANGUAGE_COUNT - 1);
			dropdown_set_checked(gCurrentLanguage - 1, true);
			break;
		case WIDX_DATE_FORMAT_DROPDOWN:
			for (i = 0; i < 4; i++) {
				gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
				gDropdownItemsArgs[i] = DateFormatStringIds[i];
			}
			window_options_show_dropdown(w, widget, 4);
			dropdown_set_checked(gConfigGeneral.date_format, true);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_AUDIO:
		switch (widgetIndex) {
		case WIDX_SOUND_DROPDOWN:
			audio_populate_devices();

			// populate the list with the sound devices
			for (i = 0; i < gAudioDeviceCount; i++) {
				gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
				gDropdownItemsArgs[i] = 1170 | ((uint64)(intptr_t)gAudioDevices[i].name << 16);
			}

			window_options_show_dropdown(w, widget, gAudioDeviceCount);

			dropdown_set_checked(gAudioCurrentDevice, true);
			break;
		case WIDX_TITLE_MUSIC_DROPDOWN:
			num_items = 4;

			for (i = 0; i < num_items - 1; i++) {
				gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
				gDropdownItemsArgs[i] = 2739 + i;
			}
			// Random title music
			gDropdownItemsFormat[3] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[3] = 5126;

			window_options_show_dropdown(w, widget, num_items);

			dropdown_set_checked(gConfigSound.title_music, true);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE:
		switch (widgetIndex) {
		case WIDX_THEMES_DROPDOWN:
			num_items = (int)theme_manager_get_num_available_themes();

			for (int i = 0; i < num_items; i++) {
				gDropdownItemsFormat[i] = 2777;
				gDropdownItemsArgs[i] = (uint32)theme_manager_get_available_theme_name(i);
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

			dropdown_set_checked(theme_manager_get_active_available_theme_index(), true);
			widget_invalidate(w, WIDX_THEMES_DROPDOWN);
			break;

		case WIDX_SCENARIO_GROUPING_DROPDOWN:
			num_items = 2;

			gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[0] = STR_OPTIONS_SCENARIO_DIFFICULTY;
			gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[1] = STR_OPTIONS_SCENARIO_ORIGIN;

			window_dropdown_show_text_custom_width(
				w->x + widget->left,
				w->y + widget->top,
				widget->bottom - widget->top + 1,
				w->colours[1],
				DROPDOWN_FLAG_STAY_OPEN,
				num_items,
				widget->right - widget->left - 3
			);

			dropdown_set_checked(gConfigGeneral.scenario_select_mode, true);
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_MISC:
		switch (widgetIndex) {
		case WIDX_AUTOSAVE_DROPDOWN:
			for (i = AUTOSAVE_EVERY_MINUTE; i <= AUTOSAVE_NEVER; i++) {
				gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
				gDropdownItemsArgs[i] = STR_SAVE_EVERY_MINUTE + i;
			}

			window_options_show_dropdown(w, widget, AUTOSAVE_NEVER + 1);
			dropdown_set_checked(gConfigGeneral.autosave_frequency, true);
			break;
		case WIDX_TITLE_SEQUENCE_DROPDOWN:
			num_items = gConfigTitleSequences.num_presets;

			for (i = 0; i < num_items; i++) {
				gDropdownItemsFormat[i] = 2777;
				gDropdownItemsArgs[i] = (uint32)&gConfigTitleSequences.presets[i].name;
			}

			window_dropdown_show_text(
				w->x + widget->left,
				w->y + widget->top,
				widget->bottom - widget->top + 1,
				w->colours[1],
				DROPDOWN_FLAG_STAY_OPEN,
				num_items
			);

			dropdown_set_checked(gCurrentPreviewTitleSequence, true);
			break;
		case WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN:
			for (i = 0; i < 7; i++) {
				gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
				gDropdownItemsArgs[i] = STR_EVERY_10_MINUTES + i;
			}

			window_options_show_dropdown(w, widget, 7);
			dropdown_set_checked(gConfigGeneral.default_inspection_interval, true);
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
		case WIDX_DRAWING_ENGINE_DROPDOWN:
			if (dropdownIndex != gConfigGeneral.drawing_engine) {
				gConfigGeneral.drawing_engine = (uint8)dropdownIndex;
#ifdef __WINDOWS__
				// Windows is apparently able to switch to hardware rendering on the fly although
				// using the same window in an unaccelerated and accelerated context is unsupported by SDL2
				platform_refresh_video();
#else
				// Linux requires a restart. This could be improved in the future by recreating the window,
				// https://github.com/OpenRCT2/OpenRCT2/issues/2015
				window_error_open(STR_RESTART_REQUIRED, STR_NONE);
#endif
				config_save_default();
				window_invalidate(w);
			}
			break;
		case WIDX_SCALE_QUALITY_DROPDOWN:
			if (dropdownIndex != gConfigGeneral.scale_quality) {
				gConfigGeneral.scale_quality = (uint8)dropdownIndex;
				config_save_default();
				gfx_invalidate_screen();
				platform_trigger_resize();
			}
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_RENDERING:
		switch (widgetIndex) {
		case WIDX_CONSTRUCTION_MARKER_DROPDOWN:
			if (dropdownIndex != gConfigGeneral.construction_marker_colour) {
				gConfigGeneral.construction_marker_colour = (uint8)dropdownIndex;
				config_save_default();
				gfx_invalidate_screen();
			}
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CULTURE:
		switch (widgetIndex) {
		case WIDX_HEIGHT_LABELS_DROPDOWN:
			// reset flag and set it to 1 if height as units is selected
			gConfigGeneral.show_height_as_units = 0;

			if (dropdownIndex == 0) {
				gConfigGeneral.show_height_as_units = 1;
			}
			config_save_default();
			window_options_update_height_markers();
			break;
		case WIDX_CURRENCY_DROPDOWN:
			if(dropdownIndex == CURRENCY_CUSTOM+1) { // Add 1 because the separator occupies a position
				gConfigGeneral.currency_format = (sint8)dropdownIndex-1;
				window_custom_currency_open();
			} else {
				gConfigGeneral.currency_format = (sint8)dropdownIndex;
			}
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
			{
				int fallbackLanguage = gCurrentLanguage;
				if (dropdownIndex != gCurrentLanguage - 1) {
					if (!language_open(dropdownIndex + 1))
					{
						// Failed to open language file, try to recover by falling
						// back to previously used language
						if (language_open(fallbackLanguage))
						{
							// It worked, so we can say it with error message in-game
							window_error_open(STR_LANGUAGE_LOAD_FAILED, STR_NONE);
						}
						// report error to console regardless
						log_error("Failed to open language file.");
						dropdownIndex = fallbackLanguage - 1;
					} else {
						gConfigGeneral.language = dropdownIndex + 1;
						config_save_default();
						gfx_invalidate_screen();
					}
				}
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
			audio_init_ride_sounds(dropdownIndex);
			if (dropdownIndex < gAudioDeviceCount) {
				if (dropdownIndex == 0) {
					Mixer_Init(NULL);
					gConfigSound.device = NULL;
				}
				else {
					char* devicename = gAudioDevices[dropdownIndex].name;
					Mixer_Init(devicename);
					SafeFree(gConfigSound.device);
					gConfigSound.device = strndup(devicename, AUDIO_DEVICE_NAME_SIZE);
				}
				config_save_default();
			}
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

			audio_stop_title_music();
			if (dropdownIndex != 0)
				audio_start_title_music();
			break;
		}
		break;

	case WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE:
		switch (widgetIndex) {
		case WIDX_THEMES_DROPDOWN:
			if (dropdownIndex != -1) {
				theme_manager_set_active_available_theme(dropdownIndex);
			}
			config_save_default();
			break;
		case WIDX_SCENARIO_GROUPING_DROPDOWN:
			if (dropdownIndex != gConfigGeneral.scenario_select_mode) {
				gConfigGeneral.scenario_select_mode = dropdownIndex;
				config_save_default();
				window_invalidate(w);
				window_close_by_class(WC_SCENARIO_SELECT);
			}
			break;
		}
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
		case WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN:
			if (dropdownIndex != gConfigGeneral.default_inspection_interval) {
				gConfigGeneral.default_inspection_interval = (uint8)dropdownIndex;
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

	colour_scheme_update(w);

	if (window_options_page_widgets[w->page] != w->widgets) {
		w->widgets = window_options_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}
	window_options_set_pressed_tab(w);
	w->disabled_widgets = 0;

	switch (w->page) {
	case WINDOW_OPTIONS_PAGE_DISPLAY:
		set_format_arg(16, uint16, (uint16)gConfigGeneral.fullscreen_width);
		set_format_arg(18, uint16, (uint16)gConfigGeneral.fullscreen_height);
		set_format_arg(12, uint16, 2773 + gConfigGeneral.fullscreen_mode);

		// disable resolution dropdown on "Fullscreen (desktop)"
		if (gConfigGeneral.fullscreen_mode == 2){
			w->disabled_widgets |= (1 << WIDX_RESOLUTION_DROPDOWN);
			w->disabled_widgets |= (1 << WIDX_RESOLUTION);
		}
		else {
			w->disabled_widgets &= ~(1 << WIDX_RESOLUTION_DROPDOWN);
			w->disabled_widgets &= ~(1 << WIDX_RESOLUTION);
		}

		if (gConfigGeneral.drawing_engine == DRAWING_ENGINE_SOFTWARE) {
			w->disabled_widgets |= (1 << WIDX_SCALE_QUALITY);
			w->disabled_widgets |= (1 << WIDX_SCALE_QUALITY_DROPDOWN);
			w->disabled_widgets |= (1 << WIDX_SCALE_USE_NN_AT_INTEGER_SCALES_CHECKBOX);
			w->disabled_widgets |= (1 << WIDX_STEAM_OVERLAY_PAUSE);
		} else {
			w->disabled_widgets &= ~(1 << WIDX_SCALE_QUALITY);
			w->disabled_widgets &= ~(1 << WIDX_SCALE_QUALITY_DROPDOWN);
			w->disabled_widgets &= ~(1 << WIDX_SCALE_USE_NN_AT_INTEGER_SCALES_CHECKBOX);
			w->disabled_widgets &= ~(1 << WIDX_STEAM_OVERLAY_PAUSE);
		}

		widget_set_checkbox_value(w, WIDX_UNCAP_FPS_CHECKBOX, gConfigGeneral.uncap_fps);
		widget_set_checkbox_value(w, WIDX_SHOW_FPS_CHECKBOX, gConfigGeneral.show_fps);
		widget_set_checkbox_value(w, WIDX_MINIMIZE_FOCUS_LOSS, gConfigGeneral.minimize_fullscreen_focus_loss);
		widget_set_checkbox_value(w, WIDX_STEAM_OVERLAY_PAUSE, gConfigGeneral.steam_overlay_pause);
		widget_set_checkbox_value(w, WIDX_SCALE_USE_NN_AT_INTEGER_SCALES_CHECKBOX, gConfigGeneral.use_nn_at_integer_scales);

		window_options_display_widgets[WIDX_SCALE_QUALITY].image = STR_SCALING_QUALITY_NN + gConfigGeneral.scale_quality;

		window_options_display_widgets[WIDX_RESOLUTION].type = WWT_DROPDOWN;
		window_options_display_widgets[WIDX_RESOLUTION_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_display_widgets[WIDX_FULLSCREEN].type = WWT_DROPDOWN;
		window_options_display_widgets[WIDX_FULLSCREEN_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_display_widgets[WIDX_DRAWING_ENGINE].type = WWT_DROPDOWN;
		window_options_display_widgets[WIDX_DRAWING_ENGINE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_display_widgets[WIDX_UNCAP_FPS_CHECKBOX].type = WWT_CHECKBOX;
		window_options_display_widgets[WIDX_SHOW_FPS_CHECKBOX].type = WWT_CHECKBOX;
		window_options_display_widgets[WIDX_MINIMIZE_FOCUS_LOSS].type = WWT_CHECKBOX;
		window_options_display_widgets[WIDX_STEAM_OVERLAY_PAUSE].type = WWT_CHECKBOX;
		break;

	case WINDOW_OPTIONS_PAGE_RENDERING:
		widget_set_checkbox_value(w, WIDX_TILE_SMOOTHING_CHECKBOX, gConfigGeneral.landscape_smoothing);
		widget_set_checkbox_value(w, WIDX_GRIDLINES_CHECKBOX, gConfigGeneral.always_show_gridlines);
		widget_set_checkbox_value(w, WIDX_DAY_NIGHT_CHECKBOX, gConfigGeneral.day_night_cycle);
		widget_set_checkbox_value(w, WIDX_UPPER_CASE_BANNERS_CHECKBOX, gConfigGeneral.upper_case_banners);
		widget_set_checkbox_value(w, WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX, gConfigGeneral.disable_lightning_effect);

		// construction marker: white/translucent
		window_options_rendering_widgets[WIDX_CONSTRUCTION_MARKER].image = STR_WHITE + gConfigGeneral.construction_marker_colour;

		window_options_rendering_widgets[WIDX_TILE_SMOOTHING_CHECKBOX].type = WWT_CHECKBOX;
		window_options_rendering_widgets[WIDX_GRIDLINES_CHECKBOX].type = WWT_CHECKBOX;
		window_options_rendering_widgets[WIDX_CONSTRUCTION_MARKER].type = WWT_DROPDOWN;
		window_options_rendering_widgets[WIDX_CONSTRUCTION_MARKER_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_rendering_widgets[WIDX_DAY_NIGHT_CHECKBOX].type = WWT_CHECKBOX;
		window_options_rendering_widgets[WIDX_UPPER_CASE_BANNERS_CHECKBOX].type = WWT_CHECKBOX;
		window_options_rendering_widgets[WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX].type = WWT_CHECKBOX;
		break;

	case WINDOW_OPTIONS_PAGE_CULTURE:
		// currency: pounds, dollars, etc. (10 total)
		set_format_arg(12, uint16, CurrencyDescriptors[gConfigGeneral.currency_format].stringId);

		// distance: metric / imperial / si
		{
			rct_string_id stringId;
			switch (gConfigGeneral.measurement_format) {
			default:
			case MEASUREMENT_FORMAT_IMPERIAL: stringId = STR_IMPERIAL; break;
			case MEASUREMENT_FORMAT_METRIC: stringId = STR_METRIC; break;
			case MEASUREMENT_FORMAT_SI: stringId = STR_SI; break;
			}
			set_format_arg(14, uint16, stringId);
		}

		// temperature: celsius/fahrenheit
		set_format_arg(20, uint16, STR_CELSIUS + gConfigGeneral.temperature_format);

		// height: units/real values
		set_format_arg(6, uint16, gConfigGeneral.show_height_as_units ? STR_UNITS : STR_REAL_VALUES);

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
		// sound devices
		if (gAudioCurrentDevice == -1 || gAudioDeviceCount == 0) {
			set_format_arg(0, uint16, STR_SOUND_NONE);
		}
		else {
#ifndef __LINUX__
			if (gAudioCurrentDevice == 0)
				set_format_arg(0, uint16, 5510);
			else
#endif // __LINUX__
				set_format_arg(0, uint16, 1170);

			set_format_arg(2, uint32, (uint32)gAudioDevices[gAudioCurrentDevice].name);
		}

		// music: on/off
		set_format_arg(8, uint16, STR_OFF + gConfigSound.ride_music_enabled);

		widget_set_checkbox_value(w, WIDX_SOUND_CHECKBOX, gConfigSound.sound_enabled);
		widget_set_checkbox_value(w, WIDX_MUSIC_CHECKBOX, gConfigSound.ride_music_enabled);
		widget_set_checkbox_value(w, WIDX_AUDIO_FOCUS_CHECKBOX, gConfigSound.audio_focus);

		if(w->frame_no == 0){ // initialize only on first frame, otherwise the scrollbars wont be able to be modified
			widget = &window_options_audio_widgets[WIDX_SOUND_VOLUME];
			w->scrolls[0].h_left = (sint16)ceil((gConfigSound.sound_volume / 100.0f) * (w->scrolls[0].h_right - ((widget->right - widget->left) - 1)));
			widget = &window_options_audio_widgets[WIDX_MUSIC_VOLUME];
			w->scrolls[1].h_left = (sint16)ceil((gConfigSound.ride_music_volume / 100.0f) * (w->scrolls[1].h_right - ((widget->right - widget->left) - 1)));
		}

		widget_scroll_update_thumbs(w, WIDX_SOUND_VOLUME);
		widget_scroll_update_thumbs(w, WIDX_MUSIC_VOLUME);

		window_options_audio_widgets[WIDX_SOUND].type = WWT_DROPDOWN;
		window_options_audio_widgets[WIDX_SOUND_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_audio_widgets[WIDX_SOUND_CHECKBOX].type = WWT_CHECKBOX;
		window_options_audio_widgets[WIDX_MUSIC_CHECKBOX].type = WWT_CHECKBOX;
		window_options_audio_widgets[WIDX_AUDIO_FOCUS_CHECKBOX].type = WWT_CHECKBOX;
		window_options_audio_widgets[WIDX_TITLE_MUSIC].type = WWT_DROPDOWN;
		window_options_audio_widgets[WIDX_TITLE_MUSIC_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_audio_widgets[WIDX_SOUND_VOLUME].type = WWT_SCROLL;
		window_options_audio_widgets[WIDX_MUSIC_VOLUME].type = WWT_SCROLL;
		break;

	case WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE:
		widget_set_checkbox_value(w, WIDX_SCREEN_EDGE_SCROLLING, gConfigGeneral.edge_scrolling);
		widget_set_checkbox_value(w, WIDX_TRAP_CURSOR, gConfigGeneral.trap_cursor);
		widget_set_checkbox_value(w, WIDX_INVERT_DRAG, gConfigGeneral.invert_viewport_drag);
		widget_set_checkbox_value(w, WIDX_TOOLBAR_SHOW_FINANCES, gConfigInterface.toolbar_show_finances);
		widget_set_checkbox_value(w, WIDX_TOOLBAR_SHOW_RESEARCH, gConfigInterface.toolbar_show_research);
		widget_set_checkbox_value(w, WIDX_TOOLBAR_SHOW_CHEATS, gConfigInterface.toolbar_show_cheats);
		widget_set_checkbox_value(w, WIDX_TOOLBAR_SHOW_NEWS, gConfigInterface.toolbar_show_news);
		widget_set_checkbox_value(w, WIDX_SELECT_BY_TRACK_TYPE, gConfigInterface.select_by_track_type);
		widget_set_checkbox_value(w, WIDX_SCENARIO_UNLOCKING, gConfigGeneral.scenario_unlocking_enabled);

		if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN) {
			w->disabled_widgets &= ~(1ULL << WIDX_SCENARIO_UNLOCKING);
		} else {
			w->disabled_widgets |= (1ULL << WIDX_SCENARIO_UNLOCKING);
		}

		window_options_controls_and_interface_widgets[WIDX_THEMES].type = WWT_DROPDOWN;
		window_options_controls_and_interface_widgets[WIDX_THEMES_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_controls_and_interface_widgets[WIDX_THEMES_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_options_controls_and_interface_widgets[WIDX_SCREEN_EDGE_SCROLLING].type = WWT_CHECKBOX;
		window_options_controls_and_interface_widgets[WIDX_TRAP_CURSOR].type = WWT_CHECKBOX;
		window_options_controls_and_interface_widgets[WIDX_HOTKEY_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_controls_and_interface_widgets[WIDX_TOOLBAR_SHOW_FINANCES].type = WWT_CHECKBOX;
		window_options_controls_and_interface_widgets[WIDX_TOOLBAR_SHOW_RESEARCH].type = WWT_CHECKBOX;
		window_options_controls_and_interface_widgets[WIDX_TOOLBAR_SHOW_CHEATS].type = WWT_CHECKBOX;
		window_options_controls_and_interface_widgets[WIDX_TOOLBAR_SHOW_NEWS].type = WWT_CHECKBOX;
		window_options_controls_and_interface_widgets[WIDX_SELECT_BY_TRACK_TYPE].type = WWT_CHECKBOX;
		window_options_controls_and_interface_widgets[WIDX_SCENARIO_GROUPING].type = WWT_DROPDOWN;
		window_options_controls_and_interface_widgets[WIDX_SCENARIO_UNLOCKING].type = WWT_CHECKBOX;
		break;

	case WINDOW_OPTIONS_PAGE_MISC:
		// unknown park flag can disable real name checkbox
		if (gParkFlags & PARK_FLAGS_LOCK_REAL_NAMES_OPTION)
			w->disabled_widgets |= (1ULL << WIDX_REAL_NAME_CHECKBOX);

		// save plugin data checkbox: visible or not
		if (gNumInstalledCustomObjects == 0)
			window_options_misc_widgets[WIDX_SAVE_PLUGIN_DATA_CHECKBOX].type = WWT_EMPTY;
		else
			window_options_misc_widgets[WIDX_SAVE_PLUGIN_DATA_CHECKBOX].type = WWT_CHECKBOX;

		widget_set_checkbox_value(w, WIDX_REAL_NAME_CHECKBOX, gParkFlags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES);
		widget_set_checkbox_value(w, WIDX_SAVE_PLUGIN_DATA_CHECKBOX, gConfigGeneral.save_plugin_data);
		widget_set_checkbox_value(w, WIDX_TEST_UNFINISHED_TRACKS, gConfigGeneral.test_unfinished_tracks);
		widget_set_checkbox_value(w, WIDX_AUTO_STAFF_PLACEMENT, gConfigGeneral.auto_staff_placement);
		widget_set_checkbox_value(w, WIDX_HANDYMEN_MOW_DEFAULT, gConfigGeneral.handymen_mow_default);
		widget_set_checkbox_value(w, WIDX_DEBUGGING_TOOLS, gConfigGeneral.debugging_tools);
		widget_set_checkbox_value(w, WIDX_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM, gConfigGeneral.allow_loading_with_incorrect_checksum);
		widget_set_checkbox_value(w, WIDX_STAY_CONNECTED_AFTER_DESYNC, gConfigNetwork.stay_connected);
		widget_set_checkbox_value(w, WIDX_AUTO_OPEN_SHOPS, gConfigGeneral.auto_open_shops);

		window_options_misc_widgets[WIDX_REAL_NAME_CHECKBOX].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_SAVE_PLUGIN_DATA_CHECKBOX].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_AUTOSAVE].type = WWT_DROPDOWN;
		window_options_misc_widgets[WIDX_AUTOSAVE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_misc_widgets[WIDX_TEST_UNFINISHED_TRACKS].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_AUTO_STAFF_PLACEMENT].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_HANDYMEN_MOW_DEFAULT].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_DEBUGGING_TOOLS].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_TITLE_SEQUENCE].type = WWT_DROPDOWN;
		window_options_misc_widgets[WIDX_TITLE_SEQUENCE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_misc_widgets[WIDX_TITLE_SEQUENCE_BUTTON].type = WWT_DROPDOWN_BUTTON;
		window_options_misc_widgets[WIDX_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_STAY_CONNECTED_AFTER_DESYNC].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_AUTO_OPEN_SHOPS].type = WWT_CHECKBOX;
		window_options_misc_widgets[WIDX_DEFAULT_INSPECTION_INTERVAL].type = WWT_DROPDOWN;
		window_options_misc_widgets[WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
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

	// Automatically adjust window height to fit widgets
	int y = 0;
	for (widget = &w->widgets[WIDX_PAGE_START]; widget->type != WWT_LAST; widget++) {
		y = max(y, widget->bottom);
	}
	w->height = y + 6;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
}

static void window_options_update(rct_window *w)
{
	// Tab animation
	w->frame_no++;
	widget_invalidate(w, WIDX_TAB_1 + w->page);

	rct_widget *widget;
	if (w->page == WINDOW_OPTIONS_PAGE_AUDIO) {
		widget = &window_options_audio_widgets[WIDX_SOUND_VOLUME];
		uint8 sound_volume = (uint8)(((float)w->scrolls[0].h_left / (w->scrolls[0].h_right - ((widget->right - widget->left) - 1))) * 100);
		widget = &window_options_audio_widgets[WIDX_MUSIC_VOLUME];
		uint8 ride_music_volume = (uint8)(((float)w->scrolls[1].h_left / (w->scrolls[1].h_right - ((widget->right - widget->left) - 1))) * 100);
		if (sound_volume != gConfigSound.sound_volume) {
			gConfigSound.sound_volume = sound_volume;
			config_save_default();
		}
		if (ride_music_volume != gConfigSound.ride_music_volume) {
			gConfigSound.ride_music_volume = ride_music_volume;
			config_save_default();
		}
		widget_invalidate(w, WIDX_SOUND_VOLUME);
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
		gfx_draw_string_left(dpi, STR_FULLSCREEN_MODE, w, w->colours[1], w->x + 10, w->y + window_options_display_widgets[WIDX_FULLSCREEN].top + 1);

		int colour = w->colours[1];
		// disable resolution dropdown on "Fullscreen (desktop)"
		if (gConfigGeneral.fullscreen_mode == 2) {
			colour |= 0x40;
		}
		gfx_draw_string_left(dpi, STR_DISPLAY_RESOLUTION, w, colour, w->x + 10 + 15, w->y + window_options_display_widgets[WIDX_RESOLUTION].top + 1);
		gfx_draw_string_left(dpi, STR_UI_SCALING_DESC, w, w->colours[1], w->x + 10, w->y + window_options_display_widgets[WIDX_SCALE].top + 1);

		gfx_draw_string_left(dpi, STR_DRAWING_ENGINE, w, w->colours[1], w->x + 10, w->y + window_options_display_widgets[WIDX_DRAWING_ENGINE].top + 1);
		gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
		gfx_draw_string_left_clipped(
			dpi,
			DrawingEngineStringIds[gConfigGeneral.drawing_engine],
			NULL,
			w->colours[1],
			w->x + window_options_culture_widgets[WIDX_DRAWING_ENGINE].left + 1,
			w->y + window_options_culture_widgets[WIDX_DRAWING_ENGINE].top,
			window_options_culture_widgets[WIDX_DRAWING_ENGINE].right - window_options_culture_widgets[WIDX_DRAWING_ENGINE].left - 11
		);

		int scale = (int)(gConfigGeneral.window_scale * 100);
		gfx_draw_string_left(dpi, 3311, &scale, w->colours[1], w->x + w->widgets[WIDX_SCALE].left + 1, w->y + w->widgets[WIDX_SCALE].top + 1);

		colour = w->colours[1];
		if (gConfigGeneral.drawing_engine == DRAWING_ENGINE_SOFTWARE) {
			colour |= 0x40;
		}
		gfx_draw_string_left(dpi, STR_SCALING_QUALITY, w, colour, w->x + 25, w->y + window_options_display_widgets[WIDX_SCALE_QUALITY].top + 1);
		break;
	case WINDOW_OPTIONS_PAGE_RENDERING:
		gfx_draw_string_left(dpi, STR_CONSTRUCTION_MARKER, w, w->colours[1], w->x + 10, w->y + window_options_rendering_widgets[WIDX_CONSTRUCTION_MARKER].top + 1);
		break;
	case WINDOW_OPTIONS_PAGE_CULTURE:
		gfx_draw_string_left(dpi, 2776, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_LANGUAGE].top + 1);

		gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
		gfx_draw_string(
			dpi,
			(char*)LanguagesDescriptors[gCurrentLanguage].native_name,
			w->colours[1],
			w->x + window_options_culture_widgets[WIDX_LANGUAGE].left + 1,
			w->y + window_options_culture_widgets[WIDX_LANGUAGE].top
		);

		gfx_draw_string_left(dpi, STR_CURRENCY, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_CURRENCY].top + 1);
		gfx_draw_string_left(dpi, STR_DISTANCE_AND_SPEED, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_DISTANCE].top + 1);
		gfx_draw_string_left(dpi, STR_TEMPERATURE, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_TEMPERATURE].top + 1);
		gfx_draw_string_left(dpi, STR_HEIGHT_LABELS, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_HEIGHT_LABELS].top + 1);
		gfx_draw_string_left(dpi, STR_DATE_FORMAT, w, w->colours[1], w->x + 10, w->y + window_options_culture_widgets[WIDX_DATE_FORMAT].top + 1);
		gfx_draw_string_left(
			dpi,
			DateFormatStringIds[gConfigGeneral.date_format],
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
	case WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE:
		gfx_draw_string_left(dpi, STR_SHOW_TOOLBAR_BUTTONS_FOR, w, w->colours[1], w->x + 10, w->y + window_options_controls_and_interface_widgets[WIDX_TOOLBAR_BUTTONS_GROUP].top + 15);

		int activeAvailableThemeIndex = theme_manager_get_active_available_theme_index();
		const utf8 * activeThemeName = theme_manager_get_available_theme_name(activeAvailableThemeIndex);
		set_format_arg(0, uint32, (uint32)activeThemeName);

		gfx_draw_string_left(dpi, 5238, NULL, w->colours[1], w->x + 10, w->y + window_options_controls_and_interface_widgets[WIDX_THEMES].top + 1);
		gfx_draw_string_left_clipped(
			dpi,
			1170,
			gCommonFormatArgs,
			w->colours[1],
			w->x + window_options_controls_and_interface_widgets[WIDX_THEMES].left + 1,
			w->y + window_options_controls_and_interface_widgets[WIDX_THEMES].top,
			window_options_controls_and_interface_widgets[WIDX_THEMES_DROPDOWN].left - window_options_controls_and_interface_widgets[WIDX_THEMES].left - 4
		);
		gfx_draw_string_left(dpi, STR_OPTIONS_SCENARIO_GROUPING, NULL, w->colours[1], w->x + 10, w->y + window_options_controls_and_interface_widgets[WIDX_SCENARIO_GROUPING].top + 1);
		gfx_draw_string_left_clipped(
			dpi,
			gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_DIFFICULTY ?
				STR_OPTIONS_SCENARIO_DIFFICULTY :
				STR_OPTIONS_SCENARIO_ORIGIN,
			NULL,
			w->colours[1],
			w->x + window_options_controls_and_interface_widgets[WIDX_SCENARIO_GROUPING].left + 1,
			w->y + window_options_controls_and_interface_widgets[WIDX_SCENARIO_GROUPING].top,
			window_options_controls_and_interface_widgets[WIDX_SCENARIO_GROUPING_DROPDOWN].left - window_options_controls_and_interface_widgets[WIDX_SCENARIO_GROUPING].left - 4
		);
		break;
	case WINDOW_OPTIONS_PAGE_MISC:
		gfx_draw_string_left(dpi, 2700, w, w->colours[1], w->x + 10, w->y + window_options_misc_widgets[WIDX_AUTOSAVE].top + 1);
		gfx_draw_string_left(
			dpi,
			STR_SAVE_EVERY_MINUTE + gConfigGeneral.autosave_frequency,
			NULL,
			w->colours[1],
			w->x + window_options_misc_widgets[WIDX_AUTOSAVE].left + 1,
			w->y + window_options_misc_widgets[WIDX_AUTOSAVE].top
		);

		set_format_arg(0, uint32, (uint32)&gConfigTitleSequences.presets[gCurrentPreviewTitleSequence].name);
		gfx_draw_string_left(dpi, STR_TITLE_SEQUENCE, w, w->colours[1], w->x + 10, w->y + window_options_misc_widgets[WIDX_TITLE_SEQUENCE].top + 1);
		gfx_draw_string_left_clipped(
			dpi,
			1170,
			gCommonFormatArgs,
			w->colours[1],
			w->x + window_options_misc_widgets[WIDX_TITLE_SEQUENCE].left + 1,
			w->y + window_options_misc_widgets[WIDX_TITLE_SEQUENCE].top,
			window_options_misc_widgets[WIDX_TITLE_SEQUENCE_DROPDOWN].left - window_options_misc_widgets[WIDX_TITLE_SEQUENCE].left - 4
		);

		gfx_draw_string_left(dpi, STR_DEFAULT_INSPECTION_INTERVAL, w, w->colours[1], w->x + 10, w->y + window_options_misc_widgets[WIDX_DEFAULT_INSPECTION_INTERVAL].top + 1);
		gfx_draw_string_left(
			dpi,
			STR_EVERY_10_MINUTES + gConfigGeneral.default_inspection_interval,
			NULL,
			w->colours[1],
			w->x + window_options_misc_widgets[WIDX_DEFAULT_INSPECTION_INTERVAL].left + 1,
			w->y + window_options_misc_widgets[WIDX_DEFAULT_INSPECTION_INTERVAL].top
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
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_RENDERING, 5221);
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_CULTURE, 5229);
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_AUDIO, 5335);
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE, 5201);
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_MISC, 5205);
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_TWITCH, SPR_G2_TAB_TWITCH);
}

#pragma endregion
