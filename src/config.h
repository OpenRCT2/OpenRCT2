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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "common.h"
#include "localisation/currency.h"
#include "platform/platform.h"

enum {
	CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES = (1 << 0),
	CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS = (1 << 1),
	CONFIG_FLAG_DISABLE_SMOOTH_LANDSCAPE = (1 << 2),
	CONFIG_FLAG_SAVE_PLUGIN_DATA = (1 << 3)
};

enum {
	SHORTCUT_CLOSE_TOP_MOST_WINDOW,
	SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS,
	SHORTCUT_CANCEL_CONSTRUCTION_MODE,
	SHORTCUT_PAUSE_GAME,
	SHORTCUT_ZOOM_VIEW_OUT,
	SHORTCUT_ZOOM_VIEW_IN,
	SHORTCUT_ROTATE_VIEW_CLOCKWISE,
	SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE,
	SHORTCUT_ROTATE_CONSTRUCTION_OBJECT,
	SHORTCUT_UNDERGROUND_VIEW_TOGGLE,
	SHORTCUT_REMOVE_BASE_LAND_TOGGLE,
	SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE,
	SHORTCUT_SEE_THROUGH_RIDES_TOGGLE,
	SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE,
	SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE,
	SHORTCUT_INVISIBLE_PEOPLE_TOGGLE,
	SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE,
	SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE,
	SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE,
	SHORTCUT_ADJUST_LAND,
	SHORTCUT_ADJUST_WATER,
	SHORTCUT_BUILD_SCENERY,
	SHORTCUT_BUILD_PATHS,
	SHORTCUT_BUILD_NEW_RIDE,
	SHORTCUT_SHOW_FINANCIAL_INFORMATION,
	SHORTCUT_SHOW_RESEARCH_INFORMATION,
	SHORTCUT_SHOW_RIDES_LIST,
	SHORTCUT_SHOW_PARK_INFORMATION,
	SHORTCUT_SHOW_GUEST_LIST,
	SHORTCUT_SHOW_STAFF_LIST,
	SHORTCUT_SHOW_RECENT_MESSAGES,
	SHORTCUT_SHOW_MAP,
	SHORTCUT_SCREENSHOT,

	// New
	SHORTCUT_REDUCE_GAME_SPEED,
	SHORTCUT_INCREASE_GAME_SPEED,
	SHORTCUT_OPEN_CHEAT_WINDOW,
	SHORTCUT_REMOVE_TOP_BOTTOM_TOOLBAR_TOGGLE,
	SHORTCUT_SCROLL_MAP_UP,
	SHORTCUT_SCROLL_MAP_LEFT,
	SHORTCUT_SCROLL_MAP_DOWN,
	SHORTCUT_SCROLL_MAP_RIGHT,
	SHORTCUT_OPEN_CHAT_WINDOW,
	SHORTCUT_QUICK_SAVE_GAME,
	SHORTCUT_SHOW_OPTIONS,

	SHORTCUT_COUNT
};

enum {
	SCREENSHOT_FORMAT_BMP,
	SCREENSHOT_FORMAT_PNG
};

enum {
	TEMPERATURE_FORMAT_C,
	TEMPERATURE_FORMAT_F
};

enum {
	MEASUREMENT_FORMAT_IMPERIAL,
	MEASUREMENT_FORMAT_METRIC,
	MEASUREMENT_FORMAT_SI
};

enum {
	AUTOSAVE_EVERY_MINUTE,
	AUTOSAVE_EVERY_5MINUTES,
	AUTOSAVE_EVERY_15MINUTES,
	AUTOSAVE_EVERY_30MINUTES,
	AUTOSAVE_EVERY_HOUR,
	AUTOSAVE_NEVER
};

enum {
	DATE_FORMAT_DMY,
	DATE_FORMAT_MDY,
	DATE_FORMAT_YMD,
	DATE_FORMAT_YDM
};

enum {
	TITLE_SEQUENCE_RCT1,
	TITLE_SEQUENCE_RCT1_AA,
	TITLE_SEQUENCE_RCT1_AA_LL,
	TITLE_SEQUENCE_RCT2,
	TITLE_SEQUENCE_OPENRCT2,
	TITLE_SEQUENCE_RANDOM
};

enum {
	SORT_NAME_ASCENDING,
	SORT_NAME_DESCENDING,
	SORT_DATE_ASCENDING,
	SORT_DATE_DESCENDING,
};

enum {
	SCENARIO_SELECT_MODE_DIFFICULTY,
	SCENARIO_SELECT_MODE_ORIGIN,
};

typedef struct {
	uint8 play_intro;
	uint8 confirmation_prompt;
	uint8 screenshot_format;
	utf8string game_path;
	sint8 measurement_format;
	sint8 temperature_format;
	sint8 currency_format;
	sint8 construction_marker_colour;
	sint8 edge_scrolling;
	sint8 always_show_gridlines;
	sint8 landscape_smoothing;
	sint8 show_height_as_units;
	sint8 save_plugin_data;
	uint8 debugging_tools;

	//new
	uint8 fullscreen_mode;
	sint32 fullscreen_width;
	sint32 fullscreen_height;
	sint32 window_width;
	sint32 window_height;
	uint16 language;
	uint8 window_snap_proximity;
	uint8 autosave_frequency;
	uint8 hardware_display;
	uint8 uncap_fps;
	uint8 test_unfinished_tracks;
	uint8 no_test_crashes;
	uint8 date_format;
	uint8 auto_staff_placement;
	uint8 handymen_mow_default;
	utf8string last_run_version;
	uint8 invert_viewport_drag;
	uint8 load_save_sort;
	uint8 minimize_fullscreen_focus_loss;
	uint8 day_night_cycle;
	uint8 upper_case_banners;
	uint8 allow_loading_with_incorrect_checksum;
	uint8 steam_overlay_pause;
	float window_scale;
	uint8 scale_quality;
	uint8 use_nn_at_integer_scales;
	uint8 show_fps;
	uint8 trap_cursor;
	uint8 auto_open_shops;
	uint8 scenario_select_mode;
	uint8 scenario_unlocking_enabled;
	uint8 scenario_hide_mega_park;
} general_configuration;

typedef struct {
	uint8 toolbar_show_finances;
	uint8 toolbar_show_research;
	uint8 toolbar_show_cheats;
	uint8 toolbar_show_news;
	uint8 select_by_track_type;
	uint8 console_small_font;
	utf8string current_theme_preset;
	utf8string current_title_sequence_preset;
	uint32 object_selection_filter_flags;
} interface_configuration;

typedef struct {
	uint8 title_music;
	uint8 sound;
	uint8 ride_music;
	uint8 audio_focus;
	uint8 master_volume;
	uint8 music_volume;
	utf8string device;
} sound_configuration;

typedef struct {
	utf8string channel;
	uint8 enable_follower_peep_names;
	uint8 enable_follower_peep_tracking;
	uint8 enable_chat_peep_names;
	uint8 enable_chat_peep_tracking;
	uint8 enable_news;
} twitch_configuration;

typedef struct {
	utf8string player_name;
	uint32 default_port;
	utf8string default_password;
	uint8 stay_connected;
	uint8 advertise;
	uint8 maxplayers;
	utf8string server_name;
	utf8string server_description;
	utf8string master_server_url;
	utf8string provider_name;
	utf8string provider_email;
	utf8string provider_website;
} network_configuration;

typedef struct {
	bool park_award;
	bool park_marketing_campaign_finished;
	bool park_warnings;
	bool park_rating_warnings;
	bool ride_broken_down;
	bool ride_crashed;
	bool ride_warnings;
	bool ride_researched;
	bool guest_warnings;
	bool guest_lost;
	bool guest_left_park;
	bool guest_queuing_for_ride;
	bool guest_on_ride;
	bool guest_left_ride;
	bool guest_bought_item;
	bool guest_used_facility;
	bool guest_died;
} notification_configuration;

typedef struct theme_window {
	uint8 colours[6];

	// Define any other settings for all windows here

} theme_window;

// Define structures for any other settings here
typedef struct {
	uint8 rct1_ride_lights;
	uint8 rct1_park_lights;
	uint8 rct1_scenario_font;
} theme_features;

#define THEME_PRESET_NAME_SIZE 256

typedef struct theme_preset {
	char name[THEME_PRESET_NAME_SIZE];
	theme_window *windows;

	// Add structures for any other settings here
	theme_features features;

} theme_preset;

typedef struct {
	theme_preset *presets;
	uint16 num_presets;
} themes_configuration;

#define TITLE_SEQUENCE_MAX_SAVE_LENGTH 51

typedef struct {
	uint8 command;
	union {
		uint8 saveIndex;	// LOAD (this index is internal only)
		uint8 x;			// LOCATION
		uint8 rotations;	// ROTATE (counter-clockwise)
		uint8 zoom;			// ZOOM
		uint8 speed;		// SPEED
		uint8 seconds;		// WAIT
	};
	uint8 y;				// LOCATION
} title_command;

#define TITLE_SEQUENCE_NAME_SIZE 256

typedef struct {
	char name[TITLE_SEQUENCE_NAME_SIZE];
	char path[MAX_PATH]; // Needed for non-modifiable presets
	char (*saves)[TITLE_SEQUENCE_MAX_SAVE_LENGTH];
	title_command *commands;
	uint8 num_saves;
	uint16 num_commands;

} title_sequence;

typedef struct {
	title_sequence *presets;
	uint16 num_presets;

} title_sequences_configuration;

typedef struct {
	uint8 key;
	uint8 modifier;
} shortcut_entry;

extern general_configuration gConfigGeneral;
extern interface_configuration gConfigInterface;
extern sound_configuration gConfigSound;
extern twitch_configuration gConfigTwitch;
extern network_configuration gConfigNetwork;
extern notification_configuration gConfigNotifications;
extern themes_configuration gConfigThemes;
extern title_sequences_configuration gConfigTitleSequences;

extern uint16 gShortcutKeys[SHORTCUT_COUNT];

void config_get_default_path(utf8 *outPath);
void config_set_defaults();
void config_release();
bool config_open_default();
bool config_save_default();

uint16 getLanguage();

void config_reset_shortcut_keys();
bool config_shortcut_keys_load();
bool config_shortcut_keys_save();

bool config_find_or_browse_install_directory();

void themes_set_default();
void themes_load_presets();
bool themes_save_preset(int preset);

void title_sequences_set_default();
void title_sequences_load_presets();
void title_sequence_save_preset_script(int preset);

#endif
