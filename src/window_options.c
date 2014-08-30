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

#include <stdint.h>

#include "addresses.h"
#include "audio.h"
#include "config.h"
#include "gfx.h"
#include "language.h"
#include "osinterface.h"
#include "sprites.h"
#include "string_ids.h"
#include "viewport.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"

enum {
	WINDOW_OPTIONS_PAGE_DISPLAY,
	WINDOW_OPTIONS_PAGE_CULTURE,
	WINDOW_OPTIONS_PAGE_AUDIO,
	WINDOW_OPTIONS_PAGE_INPUT,
	WINDOW_OPTIONS_PAGE_MISC,
	WINDOW_OPTIONS_PAGE_COUNT
};

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

	WIDX_RESOLUTION,
	WIDX_RESOLUTION_DROPDOWN,
	WIDX_FULLSCREEN,
	WIDX_FULLSCREEN_DROPDOWN,
	WIDX_TILE_SMOOTHING_CHECKBOX,
	WIDX_GRIDLINES_CHECKBOX,
	WIDX_CONSTRUCTION_MARKER,
	WIDX_CONSTRUCTION_MARKER_DROPDOWN,
	
	WIDX_LANGUAGE,
	WIDX_LANGUAGE_DROPDOWN,
	WIDX_CURRENCY,
	WIDX_CURRENCY_DROPDOWN,
	WIDX_DISTANCE,
	WIDX_DISTANCE_DROPDOWN,
	WIDX_TEMPERATURE,
	WIDX_TEMPERATURE_DROPDOWN,
	WIDX_HEIGHT_LABELS,
	WIDX_HEIGHT_LABELS_DROPDOWN,

	WIDX_SOUND,
	WIDX_SOUND_DROPDOWN,
	WIDX_MUSIC,
	WIDX_MUSIC_DROPDOWN,
	WIDX_SOUND_QUALITY,
	WIDX_SOUND_QUALITY_DROPDOWN,
	WIDX_SOUND_SW_BUFFER_CHECKBOX,
	WIDX_SOUND_PAUSED_CHECKBOX,

	WIDX_SCREEN_EDGE_SCROLLING,
	WIDX_HOTKEY_DROPDOWN,

	WIDX_REAL_NAME_CHECKBOX,
	WIDX_SAVE_PLUGIN_DATA_CHECKBOX,
};

#define WW 310
#define WH 135

static rct_widget window_options_widgets[] = {
	{ WWT_FRAME,			0,	0,		WW - 1,	0,		WH - 1,	STR_NONE,		STR_NONE },
	{ WWT_CAPTION,			0,	1,		WW - 2,	1,		14,		STR_OPTIONS,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	WW-13,	WW - 3,	2,		13,		STR_CLOSE_X,	STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		WW - 1,	43,		WH - 1,	0xFFFFFFFF,		STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,		STR_NONE },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,		STR_NONE },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,		STR_NONE },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,		STR_NONE },
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,		STR_NONE },

	// Display tab
	{ WWT_DROPDOWN,			0,	155,	299,	53,		64,		840,			STR_NONE },	// resolution
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	54,		63,		876,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	68,		79,		871,			STR_NONE },	// fullscreen
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	69,		78,		876,			STR_NONE },
	{ WWT_CHECKBOX,			0,	10,		299,	84,		95,		STR_TILE_SMOOTHING, STR_TILE_SMOOTHING_TIP },
	{ WWT_CHECKBOX,			0,	10,		299,	99,		110,	STR_GRIDLINES,	STR_GRIDLINES_TIP },
	{ WWT_DROPDOWN,			0,	155,	299,	113,	124,	STR_NONE,		STR_NONE },	// construction marker
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	114,	123,	876,			STR_NONE },

	// Culture / units tab
	{ WWT_DROPDOWN,			0,	155,	299,	53,		64,		STR_NONE,		STR_NONE },	// language
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	54,		63,		876,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	68,		79,		871,			STR_NONE },	// currency
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	69,		78,		876,			STR_NONE }, //
	{ WWT_DROPDOWN,			0,	155,	299,	83,		94,		872,			STR_NONE },	// distance
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	84,		93,		876,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	98,		110,	875,			STR_NONE },	// temperature
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	99,		108,	876,			STR_NONE }, //jjj
	{ WWT_DROPDOWN,			0,	155,	299,	113,	124,	868,			STR_NONE },	// height labels
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	114,	123,	876,			STR_NONE },

	// Audio tab
	{ WWT_DROPDOWN,			0,	10,		299,	53,		64,		865,			STR_NONE },	// sound
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	54,		63,		876,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	68,		79,		869,			STR_NONE },	// music
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	69,		78,		876,			STR_NONE },
	{ WWT_DROPDOWN,			0,	155,	299,	83,		94,		870,			STR_NONE },	// sound quality
	{ WWT_DROPDOWN_BUTTON,	0,	288,	298,	84,		93,		876,			STR_NONE },
	{ WWT_CHECKBOX,			0,	10,		299,	99,		110,	STR_SOUND_FORCED_SOFTWARE_BUFFER_MIXING, STR_SOUND_FORCED_SOFTWARE_BUFFER_MIXING_TIP },
	{ WWT_CHECKBOX,			0,	10,		229,	114,	125,	STR_SOUND,		STR_NONE }, // enable/disable sound

	// Controls tab
	{ WWT_CHECKBOX,			2,	10,		299,	53,		64,		STR_SCREEN_EDGE_SCROLLING, STR_SCREEN_EDGE_SCROLLING_TIP },
	{ WWT_DROPDOWN_BUTTON,	0,	26,		185,	68,		78,		STR_HOTKEY,		STR_HOTKEY_TIP },

	// Misc
	{ WWT_CHECKBOX,			2,	10,		299,	53,		64,		STR_REAL_NAME,	STR_REAL_NAME_TIP },
	{ WWT_CHECKBOX,			2,	10,		299,	68,		79,		STR_SAVE_PLUGIN_DATA, STR_SAVE_PLUGIN_DATA_TIP },
	{ WIDGETS_END },
};

const int window_options_tab_animation_divisor[] = { 4, 8, 2, 2, 2 };
const int window_options_tab_animation_frames[] = { 16, 8, 16, 4, 16 };

static void window_options_set_page(rct_window *w, int page);
static void window_options_set_pressed_tab(rct_window *w);
static void window_options_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int page, int spriteIndex);
static void window_options_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

static void window_options_emptysub() { }
static void window_options_mouseup();
static void window_options_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_options_dropdown();
static void window_options_update(rct_window *w);
static void window_options_invalidate();
static void window_options_paint();
static void window_options_show_dropdown(rct_window *w, rct_widget *widget, int num_items);
static void window_options_update_height_markers();

static void* window_options_events[] = {
	window_options_emptysub,
	window_options_mouseup,
	window_options_emptysub,
	window_options_mousedown,
	window_options_dropdown,
	window_options_emptysub,
	window_options_update,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_emptysub,
	window_options_invalidate,
	window_options_paint,
	window_options_emptysub
};

/**
*
*  rct2: 0x006BAC5B
*/
void window_options_open()
{
	rct_window* w;

	// Check if window is already open
	w = window_bring_to_front_by_id(WC_OPTIONS, 0);
	if (w != NULL)
		return;

	w = window_create_auto_pos(WW, WH, (uint32*)window_options_events, WC_OPTIONS, 0);
	w->widgets = window_options_widgets;
	w->enabled_widgets =
		(1ULL << WIDX_CLOSE) |
		(1ULL << WIDX_TAB_1) |
		(1ULL << WIDX_TAB_2) |
		(1ULL << WIDX_TAB_3) |
		(1ULL << WIDX_TAB_4) |
		(1ULL << WIDX_TAB_5) |
		(1ULL << WIDX_SOUND) |
		(1ULL << WIDX_SOUND_DROPDOWN) |
		(1ULL << WIDX_MUSIC) |
		(1ULL << WIDX_MUSIC_DROPDOWN) |
		(1ULL << WIDX_SOUND_QUALITY) |
		(1ULL << WIDX_SOUND_QUALITY_DROPDOWN) |
		(1ULL << WIDX_LANGUAGE) |
		(1ULL << WIDX_LANGUAGE_DROPDOWN) |
		(1ULL << WIDX_CURRENCY) |
		(1ULL << WIDX_CURRENCY_DROPDOWN) |
		(1ULL << WIDX_DISTANCE) |
		(1ULL << WIDX_DISTANCE_DROPDOWN) |
		(1ULL << WIDX_RESOLUTION) |
		(1ULL << WIDX_RESOLUTION_DROPDOWN) |
		(1ULL << WIDX_FULLSCREEN) |
		(1ULL << WIDX_FULLSCREEN_DROPDOWN) |
		(1ULL << WIDX_TEMPERATURE) |
		(1ULL << WIDX_TEMPERATURE_DROPDOWN) |
		(1ULL << WIDX_HOTKEY_DROPDOWN) |
		(1ULL << WIDX_SCREEN_EDGE_SCROLLING) |
		(1ULL << WIDX_REAL_NAME_CHECKBOX) |
		(1ULL << WIDX_CONSTRUCTION_MARKER) |
		(1ULL << WIDX_CONSTRUCTION_MARKER_DROPDOWN) |
		(1ULL << WIDX_HEIGHT_LABELS) |
		(1ULL << WIDX_HEIGHT_LABELS_DROPDOWN) |
		(1ULL << WIDX_TILE_SMOOTHING_CHECKBOX) |
		(1ULL << WIDX_GRIDLINES_CHECKBOX) |
		(1ULL << WIDX_SOUND_SW_BUFFER_CHECKBOX) |
		(1ULL << WIDX_SOUND_PAUSED_CHECKBOX) |
		(1ULL << WIDX_SAVE_PLUGIN_DATA_CHECKBOX);	// doesn't seem to work?

	w->page = WINDOW_OPTIONS_PAGE_DISPLAY;
	window_init_scroll_widgets(w);
	w->colours[0] = 7;
	w->colours[1] = 7;
	w->colours[2] = 7;
}

/**
*
*  rct2: 0x006BAFCA
*/
static void window_options_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
		window_options_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_HOTKEY_DROPDOWN:
		RCT2_CALLPROC_EBPSAFE(0x006E3884);
		break;
	case WIDX_SCREEN_EDGE_SCROLLING:
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_EDGE_SCROLLING, uint8) ^= 1;
		gGeneral_config.edge_scrolling ^= 1;
		config_save();
		window_invalidate(w);
		break;
	case WIDX_REAL_NAME_CHECKBOX:
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) ^= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;

		if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_SHOW_REAL_GUEST_NAMES)
			#ifdef _MSC_VER
	__asm xor al, al
	#else
	__asm__ ( "xor al, al "  );
	#endif

		else
			#ifdef _MSC_VER
	__asm mov al, 1
	#else
	__asm__ ( "mov al, 1 "  );
	#endif


		RCT2_CALLPROC_EBPSAFE(0x0069C52F);
		break;
	case WIDX_TILE_SMOOTHING_CHECKBOX:
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) ^= CONFIG_FLAG_DISABLE_SMOOTH_LANDSCAPE;
		gGeneral_config.landscape_smoothing = !(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8)
												& CONFIG_FLAG_DISABLE_SMOOTH_LANDSCAPE);
		config_save();
		gfx_invalidate_screen();
		break;
	case WIDX_GRIDLINES_CHECKBOX:
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) ^= CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES;
		gGeneral_config.always_show_gridlines = RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) 
												 & CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES;
		config_save();
		gfx_invalidate_screen();

		if ((w = window_get_main()) != NULL) {
			if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES)
				w->viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
			else 
				w->viewport->flags &= ~VIEWPORT_FLAG_GRIDLINES;
		}
		break;
	case WIDX_SAVE_PLUGIN_DATA_CHECKBOX:
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) ^= CONFIG_FLAG_SAVE_PLUGIN_DATA;
		gGeneral_config.save_plugin_data = !(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8)
												& CONFIG_FLAG_SAVE_PLUGIN_DATA);
		config_save();
		window_invalidate(w);
		break;
	case WIDX_SOUND_SW_BUFFER_CHECKBOX:
		pause_sounds();
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, uint8) ^= 1;
		gSound_config.forced_software_buffering ^= 1;
		config_save();
		unpause_sounds();
		window_invalidate(w);
		break;
	case WIDX_SOUND_PAUSED_CHECKBOX:
		if (g_sounds_disabled){
			unpause_sounds();
		}
		else{
			pause_sounds();
		}
		window_invalidate(w);
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

	switch (widgetIndex) {
	case WIDX_SOUND_DROPDOWN:
		// populate the list with the sound devices
		for (i = 0; i < gAudioDeviceCount; i++) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = 1170 | ((uint64)(intptr_t)gAudioDevices[i].name << 16);
		}

		window_options_show_dropdown(w, widget, gAudioDeviceCount);

		gDropdownItemsChecked |= (1 << RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32));
		break;
	case WIDX_HEIGHT_LABELS_DROPDOWN:
		gDropdownItemsFormat[0] = 1142;
		gDropdownItemsFormat[1] = 1142;
		gDropdownItemsArgs[0] = STR_UNITS;
		gDropdownItemsArgs[1] = STR_REAL_VALUES;

		window_options_show_dropdown(w, widget, 2);

		gDropdownItemsChecked =
			(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) &
			CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS) ? 1 : 2;
		break;
	case WIDX_MUSIC_DROPDOWN:
		gDropdownItemsFormat[0] = 1142;
		gDropdownItemsFormat[1] = 1142;
		gDropdownItemsArgs[0] = STR_OFF;
		gDropdownItemsArgs[1] = STR_ON;

		window_options_show_dropdown(w, widget, 2);

		gDropdownItemsChecked = 1 << RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_MUSIC, uint8);
		break;
	case WIDX_SOUND_QUALITY_DROPDOWN:
		num_items = 3;

		for (i = 0; i < num_items; i++) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = STR_SOUND_LOW + i; // low, medium, high
		}

		window_options_show_dropdown(w, widget, num_items);

		gDropdownItemsChecked = 1 << RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, uint8);
		break;
	case WIDX_CURRENCY_DROPDOWN:
		num_items = 10;

		for (i = 0; i < num_items; i++) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = STR_POUNDS + i; // all different currencies
		}

		window_options_show_dropdown(w, widget, num_items);

		gDropdownItemsChecked = 1 << (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CURRENCY, uint8) & 0x3F);
		break;
	case WIDX_DISTANCE_DROPDOWN:
		gDropdownItemsFormat[0] = 1142;
		gDropdownItemsFormat[1] = 1142;
		gDropdownItemsArgs[0] = STR_IMPERIAL;
		gDropdownItemsArgs[1] = STR_METRIC;

		window_options_show_dropdown(w, widget, 2);

		gDropdownItemsChecked = 1 << RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, uint8);
		break;
	case WIDX_RESOLUTION_DROPDOWN:
		// RCT2_CALLPROC_EBPSAFE(0x006BB2AF);
		break;
	case WIDX_FULLSCREEN_DROPDOWN:
		gDropdownItemsFormat[0] = 1142;
		gDropdownItemsFormat[1] = 1142;
		gDropdownItemsFormat[2] = 1142;
		gDropdownItemsArgs[0] = 2773;
		gDropdownItemsArgs[1] = 2774;
		gDropdownItemsArgs[2] = 2775;

		window_options_show_dropdown(w, widget, 3);

		gDropdownItemsChecked = 1 << gGeneral_config.fullscreen_mode;
		break;
	case WIDX_TEMPERATURE_DROPDOWN:
		gDropdownItemsFormat[0] = 1142;
		gDropdownItemsFormat[1] = 1142;
		gDropdownItemsArgs[0] = STR_CELSIUS;
		gDropdownItemsArgs[1] = STR_FAHRENHEIT;

		window_options_show_dropdown(w, widget, 2);

		gDropdownItemsChecked = 1 << RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, uint8);
		break;
	case WIDX_CONSTRUCTION_MARKER_DROPDOWN:
		gDropdownItemsFormat[0] = 1142;
		gDropdownItemsFormat[1] = 1142;
		gDropdownItemsArgs[0] = STR_WHITE;
		gDropdownItemsArgs[1] = STR_TRANSLUCENT;

		window_options_show_dropdown(w, widget, 2);

		gDropdownItemsChecked = 1 << RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CONSTRUCTION_MARKER, uint8);
		break;
	case WIDX_LANGUAGE_DROPDOWN:
		for (i = 1; i < LANGUAGE_COUNT; i++) {
			gDropdownItemsFormat[i - 1] = 2777;
			gDropdownItemsArgs[i - 1] = (sint64)language_names[i];
		}
		window_options_show_dropdown(w, widget, LANGUAGE_COUNT - 1);
		gDropdownItemsChecked = 1 << (gCurrentLanguage - 1);
		break;
	}
}

/**
*
*  rct2: 0x006BB076
*/
static void window_options_dropdown()
{
	short dropdownIndex;
	short widgetIndex;
	rct_window *w;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (dropdownIndex == -1)
		return;

	switch (widgetIndex) {
	case WIDX_SOUND_DROPDOWN:
		audio_init2(dropdownIndex);
		/*#ifdef _MSC_VER
		__asm movzx ax, dropdownIndex		
		#else
		__asm__ ( "movzx ax, %[dropdownIndex]		 " : : [dropdownIndex] "g" ((char)dropdownIndex) );
		#endif
		// the switch replaces ax value
		RCT2_CALLPROC_EBPSAFE(0x006BA9B5);	// part of init audio*/

		window_invalidate(w);
		break;
	case WIDX_HEIGHT_LABELS_DROPDOWN:
		// reset flag and set it to 1 if height as units is selected
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) &= ~CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS;
		gGeneral_config.show_height_as_units = 0;

		if (dropdownIndex == 0) {
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) |= CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS;
			gGeneral_config.show_height_as_units = 1;
		}

		window_options_update_height_markers();
		break;
	case WIDX_MUSIC_DROPDOWN:
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_MUSIC, uint8) = (uint8)dropdownIndex;
		config_save();
		RCT2_CALLPROC_EBPSAFE(0x006BCA9F);
		window_invalidate(w);
		break;
	case WIDX_SOUND_QUALITY_DROPDOWN:
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, uint8) = (uint8)dropdownIndex;

		// TODO: no clue what this does (and if it's correct)
		RCT2_GLOBAL(0x009AAC75, uint8) = RCT2_GLOBAL(0x009AF601 + dropdownIndex, uint8);
		RCT2_GLOBAL(0x009AAC76, uint8) = RCT2_GLOBAL(0x009AF604 + dropdownIndex, uint8);
		gSound_config.sound_quality = (sint8)dropdownIndex;
		config_save();
		window_invalidate(w);
		break;
	case WIDX_CURRENCY_DROPDOWN:
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CURRENCY, uint8) = dropdownIndex | 0xC0;
		gGeneral_config.currency_format = (sint8)dropdownIndex;
		config_save();
		gfx_invalidate_screen();
		break;
	case WIDX_DISTANCE_DROPDOWN:
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, uint8) = (uint8)dropdownIndex;
		gGeneral_config.measurement_format = (sint8)dropdownIndex;
		config_save();
		window_options_update_height_markers();
		break;
	case WIDX_RESOLUTION_DROPDOWN:
		#ifdef _MSC_VER
		__asm movzx ax, dropdownIndex		
		#else
		__asm__ ( "movzx ax, %[dropdownIndex]		 " : : [dropdownIndex] "g" ((char)dropdownIndex) );
		#endif
		// the switch replaces ax value
		RCT2_CALLPROC_EBPSAFE(0x006BB37D);
		break;
	case WIDX_FULLSCREEN_DROPDOWN:
		if (dropdownIndex != gGeneral_config.fullscreen_mode){
			if (dropdownIndex == 2){
				w->disabled_widgets |= (1 << WIDX_RESOLUTION_DROPDOWN);
				w->disabled_widgets |= (1 << WIDX_RESOLUTION);
			} else {
				w->disabled_widgets &= ~(1 << WIDX_RESOLUTION_DROPDOWN);
				w->disabled_widgets &= ~(1 << WIDX_RESOLUTION);
			}
			osinterface_set_fullscreen_mode(dropdownIndex);
		}
		break;
	case WIDX_TEMPERATURE_DROPDOWN:
		if (dropdownIndex != RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, uint8)) {
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, uint8) = (uint8)dropdownIndex;
			gGeneral_config.temperature_format = (sint8)dropdownIndex;
			config_save();
			gfx_invalidate_screen();
		}
		break;
	case WIDX_CONSTRUCTION_MARKER_DROPDOWN:
		if (dropdownIndex != RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CONSTRUCTION_MARKER, uint8)) {
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CONSTRUCTION_MARKER, uint8) = (uint8)dropdownIndex;
			config_save();
			gfx_invalidate_screen();
		}
		break;
	case WIDX_LANGUAGE_DROPDOWN:
		if (dropdownIndex != gCurrentLanguage - 1) {
			language_open(dropdownIndex + 1);
			gGeneral_config.language = dropdownIndex + 1;
			config_save();
			gfx_invalidate_screen();
		}
		break;
	}
}

/**
*
*  rct2: 0x006BAD48
*/
static void window_options_invalidate()
{
	rct_window *w;
	int i;
	sint32 currentSoundDevice;

	window_get_register(w);

	window_options_set_pressed_tab(w);
	for (i = WIDX_RESOLUTION; i <= WIDX_SAVE_PLUGIN_DATA_CHECKBOX; i++) {
		window_options_widgets[i].type = WWT_EMPTY;
	}

	switch (w->page) {
	case WINDOW_OPTIONS_PAGE_DISPLAY:
		// resolution
		RCT2_GLOBAL(0x013CE952 + 16, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_RESOLUTION_WIDTH, uint16);
		RCT2_GLOBAL(0x013CE952 + 18, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_RESOLUTION_HEIGHT, uint16);
		RCT2_GLOBAL(0x013CE952 + 12, uint16) = 2773 + gGeneral_config.fullscreen_mode;

		// landscape tile smoothing checkbox
		if ((RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_DISABLE_SMOOTH_LANDSCAPE))
			w->pressed_widgets &= ~(1ULL << WIDX_TILE_SMOOTHING_CHECKBOX);
		else
			w->pressed_widgets |= (1ULL << WIDX_TILE_SMOOTHING_CHECKBOX);

		// show gridlines checkbox
		if ((RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES))
			w->pressed_widgets |= (1ULL << WIDX_GRIDLINES_CHECKBOX);
		else
			w->pressed_widgets &= ~(1ULL << WIDX_GRIDLINES_CHECKBOX);

		// construction marker: celsius/fahrenheit
		window_options_widgets[WIDX_CONSTRUCTION_MARKER].image = STR_WHITE + RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CONSTRUCTION_MARKER, uint8);

		window_options_widgets[WIDX_RESOLUTION].type = WWT_DROPDOWN;
		window_options_widgets[WIDX_RESOLUTION_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_widgets[WIDX_FULLSCREEN].type = WWT_DROPDOWN;
		window_options_widgets[WIDX_FULLSCREEN_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_widgets[WIDX_TILE_SMOOTHING_CHECKBOX].type = WWT_CHECKBOX;
		window_options_widgets[WIDX_GRIDLINES_CHECKBOX].type = WWT_CHECKBOX;
		window_options_widgets[WIDX_CONSTRUCTION_MARKER].type = WWT_DROPDOWN;
		window_options_widgets[WIDX_CONSTRUCTION_MARKER_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		break;
	case WINDOW_OPTIONS_PAGE_CULTURE:
		// currency: pounds, dollars, etc. (10 total)
		RCT2_GLOBAL(0x013CE952 + 12, uint16) = STR_POUNDS + (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CURRENCY, uint8) & 0x3F);

		// distance: metric/imperial
		RCT2_GLOBAL(0x013CE952 + 14, uint16) = STR_IMPERIAL + RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, uint8);

		// temperature: celsius/fahrenheit
		RCT2_GLOBAL(0x013CE952 + 20, uint16) = STR_CELSIUS + RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_TEMPERATURE, uint8);

		// height: units/real values
		RCT2_GLOBAL(0x013CE952 + 6, uint16) =
			((RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS)) ? STR_UNITS : STR_REAL_VALUES;

		window_options_widgets[WIDX_LANGUAGE].type = WWT_DROPDOWN;
		window_options_widgets[WIDX_LANGUAGE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_widgets[WIDX_CURRENCY].type = WWT_DROPDOWN;
		window_options_widgets[WIDX_CURRENCY_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_widgets[WIDX_DISTANCE].type = WWT_DROPDOWN;
		window_options_widgets[WIDX_DISTANCE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_widgets[WIDX_TEMPERATURE].type = WWT_DROPDOWN;
		window_options_widgets[WIDX_TEMPERATURE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_widgets[WIDX_HEIGHT_LABELS].type = WWT_DROPDOWN;
		window_options_widgets[WIDX_HEIGHT_LABELS_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		break;
	case WINDOW_OPTIONS_PAGE_AUDIO:
		currentSoundDevice = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, sint32);

		// sound devices
		if (currentSoundDevice == -1 || gAudioDeviceCount == 0) {
			RCT2_GLOBAL(0x013CE952, uint16) = STR_SOUND_NONE;
		} else {
			RCT2_GLOBAL(0x013CE952, uint16) = 1170;
			RCT2_GLOBAL(0x013CE952 + 2, uint32) = (uint32)gAudioDevices[currentSoundDevice].name;
		}

		// music: on/off
		RCT2_GLOBAL(0x013CE952 + 8, uint16) = STR_OFF + RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_MUSIC, uint8);

		// sound quality: low/medium/high
		RCT2_GLOBAL(0x013CE952 + 10, uint16) = STR_SOUND_LOW + RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_QUALITY, uint8);

		//Sound pause checkbox
		if (!g_sounds_disabled)
			w->pressed_widgets |= (1ULL << WIDX_SOUND_PAUSED_CHECKBOX);
		else
			w->pressed_widgets &= ~(1ULL << WIDX_SOUND_PAUSED_CHECKBOX);
		
		// sound software mixing buffer checkbox
		if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, uint8))
			w->pressed_widgets |= (1ULL << WIDX_SOUND_SW_BUFFER_CHECKBOX);
		else 
			w->pressed_widgets &= ~(1ULL << WIDX_SOUND_SW_BUFFER_CHECKBOX);

		window_options_widgets[WIDX_SOUND].type = WWT_DROPDOWN;
		window_options_widgets[WIDX_SOUND_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_widgets[WIDX_MUSIC].type = WWT_DROPDOWN;
		window_options_widgets[WIDX_MUSIC_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_widgets[WIDX_SOUND_QUALITY].type = WWT_DROPDOWN;
		window_options_widgets[WIDX_SOUND_QUALITY_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_options_widgets[WIDX_SOUND_SW_BUFFER_CHECKBOX].type = WWT_CHECKBOX;
		window_options_widgets[WIDX_SOUND_PAUSED_CHECKBOX].type = WWT_CHECKBOX;
		break;
	case WINDOW_OPTIONS_PAGE_INPUT:
		// screen edge scrolling checkbox
		if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_EDGE_SCROLLING, uint8))
			w->pressed_widgets |= (1ULL << WIDX_SCREEN_EDGE_SCROLLING);
		else
			w->pressed_widgets &= ~(1ULL << WIDX_SCREEN_EDGE_SCROLLING);

		window_options_widgets[WIDX_SCREEN_EDGE_SCROLLING].type = WWT_CHECKBOX;
		window_options_widgets[WIDX_HOTKEY_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		break;
	case WINDOW_OPTIONS_PAGE_MISC:
		// real name checkbox
		if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_SHOW_REAL_GUEST_NAMES)
			w->pressed_widgets |= (1ULL << WIDX_REAL_NAME_CHECKBOX);
		else
			w->pressed_widgets &= ~(1ULL << WIDX_REAL_NAME_CHECKBOX);

		// save plugin data checkbox
		if ((RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SAVE_PLUGIN_DATA))
			w->pressed_widgets |= (1ULL << WIDX_SAVE_PLUGIN_DATA_CHECKBOX);
		else
			w->pressed_widgets &= ~(1ULL << WIDX_SAVE_PLUGIN_DATA_CHECKBOX);

		// unknown park flag can disable real name checkbox
		if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & 0x8000)
			w->disabled_widgets |= (1ULL << WIDX_REAL_NAME_CHECKBOX);

		// save plugin data checkbox: visible or not
		if (RCT2_GLOBAL(0x00F42BDA, uint8) == 1)
			window_options_widgets[WIDX_SAVE_PLUGIN_DATA_CHECKBOX].type = WWT_EMPTY;
		else
			window_options_widgets[WIDX_SAVE_PLUGIN_DATA_CHECKBOX].type = WWT_CHECKBOX;

		window_options_widgets[WIDX_REAL_NAME_CHECKBOX].type = WWT_CHECKBOX;
		window_options_widgets[WIDX_SAVE_PLUGIN_DATA_CHECKBOX].type = WWT_CHECKBOX;
		break;
	}
}

static void window_options_update(rct_window *w)
{
	// Tab animation
	w->frame_no++;
	widget_invalidate(w->classification, w->number, WIDX_TAB_1 + w->page);
}

/**
*
*  rct2: 0x006BAEB4
*/
static void window_options_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	char buffer[256];

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_options_draw_tab_images(dpi, w);

	switch (w->page) {
	case WINDOW_OPTIONS_PAGE_DISPLAY:
		gfx_draw_string_left(dpi, STR_DISPLAY_RESOLUTION, w, 0, w->x + 10, w->y + window_options_widgets[WIDX_RESOLUTION].top + 1);

		sprintf(buffer, "%c%c%s", FORMAT_MEDIUMFONT, FORMAT_WINDOW_COLOUR_2, "Fullscreen mode:");
		gfx_draw_string(dpi, buffer, 0, w->x + 10, w->y + window_options_widgets[WIDX_FULLSCREEN].top + 1);

		gfx_draw_string_left(dpi, STR_CONSTRUCTION_MARKER, w, 0, w->x + 10, w->y + window_options_widgets[WIDX_CONSTRUCTION_MARKER].top + 1);
		break;
	case WINDOW_OPTIONS_PAGE_CULTURE:
		gfx_draw_string_left(dpi, 2776, w, 12, w->x + 10, w->y + window_options_widgets[WIDX_LANGUAGE].top + 1);
		gfx_draw_string(
			dpi,
			(char*)language_names[gCurrentLanguage],
			12,
			w->x + window_options_widgets[WIDX_LANGUAGE].left + 1,
			w->y + window_options_widgets[WIDX_LANGUAGE].top
		);

		gfx_draw_string_left(dpi, STR_CURRENCY, w, 0, w->x + 10, w->y + window_options_widgets[WIDX_CURRENCY].top + 1);
		gfx_draw_string_left(dpi, STR_DISTANCE_AND_SPEED, w, 0, w->x + 10, w->y + window_options_widgets[WIDX_DISTANCE].top + 1);
		gfx_draw_string_left(dpi, STR_TEMPERATURE, w, 0, w->x + 10, w->y + window_options_widgets[WIDX_TEMPERATURE].top + 1);
		gfx_draw_string_left(dpi, STR_HEIGHT_LABELS, w, 0, w->x + 10, w->y + window_options_widgets[WIDX_HEIGHT_LABELS].top + 1);
		break;
	case WINDOW_OPTIONS_PAGE_AUDIO:
		gfx_draw_string_left(dpi, STR_MUSIC, w, 0, w->x + 10, w->y + window_options_widgets[WIDX_MUSIC].top + 1);
		gfx_draw_string_left(dpi, STR_SOUND_QUALITY, w, 0, w->x + 10, w->y + window_options_widgets[WIDX_SOUND_QUALITY].top + 1);
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
		0x80,
		num_items,
		widget->right - widget->left - 3
	);
}

static void window_options_update_height_markers() 
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS) {
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, uint16) = 0;
	} else { // use real values (metric or imperial)
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, uint16) =
			(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, uint16) + 1) * 256;
	}

	config_save();
	gfx_invalidate_screen();
}

#pragma region Common

static void window_options_set_page(rct_window *w, int page)
{
	w->page = page;
	w->frame_no = 0;

	window_invalidate(w);

	RCT2_CALLPROC_X(w->event_handlers[WE_RESIZE], 0, 0, 0, 0, (int)w, 0, 0);
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);

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
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_INPUT, 5201);
	window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_MISC, 5205);
}

#pragma endregion