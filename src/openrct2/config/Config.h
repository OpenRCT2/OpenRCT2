/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Currency.h"

#include <string>

enum class MeasurementFormat : int32_t;
enum class TemperatureUnit : int32_t;
enum class ScaleQuality : int32_t;
enum class Sort : int32_t;
enum class VirtualFloorStyles : int32_t;
enum class DrawingEngine : int32_t;
enum class TitleMusicKind : int32_t;

struct GeneralConfiguration
{
    // Paths
    u8string rct1_path;
    u8string rct2_path;

    // Display
    int32_t default_display;
    int32_t window_width;
    int32_t window_height;
    int32_t fullscreen_mode;
    int32_t fullscreen_width;
    int32_t fullscreen_height;
    float window_scale;
    DrawingEngine drawing_engine;
    bool uncap_fps;
    bool use_vsync;
    bool show_fps;
    bool multithreading;
    bool minimize_fullscreen_focus_loss;
    bool disable_screensaver;

    // Map rendering
    bool landscape_smoothing;
    bool always_show_gridlines;
    VirtualFloorStyles virtual_floor_style;
    bool day_night_cycle;
    bool enable_light_fx;
    bool enable_light_fx_for_vehicles;
    bool upper_case_banners;
    bool render_weather_effects;
    bool render_weather_gloom;
    bool disable_lightning_effect;
    bool show_guest_purchases;
    bool transparent_screenshot;
    bool transparent_water;

    bool invisible_rides;
    bool invisible_vehicles;
    bool invisible_trees;
    bool invisible_scenery;
    bool invisible_paths;
    bool invisible_supports;

    // Localisation
    int32_t language;
    MeasurementFormat measurement_format;
    TemperatureUnit temperature_format;
    bool show_height_as_units;
    int32_t date_format;
    CurrencyType currency_format;
    int32_t custom_currency_rate;
    CurrencyAffix custom_currency_affix;
    utf8* custom_currency_symbol;

    // Controls
    bool edge_scrolling;
    int32_t edge_scrolling_speed;
    bool trap_cursor;
    bool invert_viewport_drag;
    bool zoom_to_cursor;

    // Miscellaneous
    bool play_intro;
    int32_t window_snap_proximity;
    bool allow_loading_with_incorrect_checksum;
    bool save_plugin_data;
    bool debugging_tools;
    int32_t autosave_frequency;
    int32_t autosave_amount;
    bool auto_staff_placement;
    bool handymen_mow_default;
    bool auto_open_shops;
    int32_t default_inspection_interval;
    int32_t window_limit;
    int32_t scenario_select_mode;
    bool scenario_unlocking_enabled;
    bool scenario_hide_mega_park;
    bool steam_overlay_pause;
    bool show_real_names_of_guests;
    bool allow_early_completion;

    // Loading and saving
    bool confirmation_prompt;
    Sort load_save_sort;
    u8string last_save_game_directory;
    u8string last_save_landscape_directory;
    u8string last_save_scenario_directory;
    u8string last_save_track_directory;
    u8string last_run_version;
    bool use_native_browse_dialog;
    int64_t last_version_check_time;
};

struct InterfaceConfiguration
{
    bool toolbar_show_finances;
    bool toolbar_show_research;
    bool toolbar_show_cheats;
    bool toolbar_show_news;
    bool toolbar_show_mute;
    bool toolbar_show_chat;
    bool toolbar_show_zoom;
    bool console_small_font;
    bool random_title_sequence;
    utf8* current_theme_preset;
    utf8* current_title_sequence_preset;
    int32_t object_selection_filter_flags;
    int32_t scenarioselect_last_tab;
    bool list_ride_vehicles_separately;
};

struct SoundConfiguration
{
    std::string device;
    bool master_sound_enabled;
    uint8_t master_volume;
    TitleMusicKind title_music;
    bool sound_enabled;
    uint8_t sound_volume;
    bool ride_music_enabled;
    uint8_t ride_music_volume;
    bool audio_focus;
};

struct NetworkConfiguration
{
    std::string player_name;
    int32_t default_port;
    std::string listen_address;
    std::string default_password;
    bool stay_connected;
    bool advertise;
    std::string advertise_address;
    int32_t maxplayers;
    std::string server_name;
    std::string server_description;
    std::string server_greeting;
    std::string master_server_url;
    std::string provider_name;
    std::string provider_email;
    std::string provider_website;
    bool known_keys_only;
    bool log_chat;
    bool log_server_actions;
    bool pause_server_if_no_clients;
    bool desync_debugging;
};

struct NotificationConfiguration
{
    bool park_award;
    bool park_marketing_campaign_finished;
    bool park_warnings;
    bool park_rating_warnings;
    bool ride_broken_down;
    bool ride_crashed;
    bool ride_casualties;
    bool ride_warnings;
    bool ride_researched;
    bool ride_stalled_vehicles;
    bool guest_warnings;
    bool guest_left_park;
    bool guest_queuing_for_ride;
    bool guest_on_ride;
    bool guest_left_ride;
    bool guest_bought_item;
    bool guest_used_facility;
    bool guest_died;
};

struct FontConfiguration
{
    utf8* file_name;
    utf8* font_name;
    int32_t x_offset;
    int32_t y_offset;
    int32_t size_tiny;
    int32_t size_small;
    int32_t size_medium;
    int32_t size_big;
    int32_t height_tiny;
    int32_t height_small;
    int32_t height_medium;
    int32_t height_big;
    bool enable_hinting;
    int32_t hinting_threshold;
};

struct PluginConfiguration
{
    bool enable_hot_reloading;
    std::string allowed_hosts;
};

enum class Sort : int32_t
{
    NameAscending,
    NameDescending,
    DateAscending,
    DateDescending,
};

enum class TemperatureUnit : int32_t
{
    Celsius,
    Fahrenheit
};

enum class ScaleQuality : int32_t
{
    NearestNeighbour,
    Linear,
    SmoothNearestNeighbour
};

enum class MeasurementFormat : int32_t
{
    Imperial,
    Metric,
    SI
};

enum class TitleMusicKind : int32_t
{
    None,
    Rct1,
    Rct2,
    Random
};

extern GeneralConfiguration gConfigGeneral;
extern InterfaceConfiguration gConfigInterface;
extern SoundConfiguration gConfigSound;
extern NetworkConfiguration gConfigNetwork;
extern NotificationConfiguration gConfigNotifications;
extern FontConfiguration gConfigFonts;
extern PluginConfiguration gConfigPlugin;

bool config_open(u8string_view path);
bool config_save(u8string_view path);
u8string config_get_default_path();
void config_set_defaults();
void config_release();
bool config_save_default();
bool config_find_or_browse_install_directory();

bool RCT1DataPresentAtLocation(u8string_view path);
std::string FindCsg1datAtLocation(u8string_view path);
bool Csg1datPresentAtLocation(u8string_view path);
std::string FindCsg1idatAtLocation(u8string_view path);
bool Csg1idatPresentAtLocation(u8string_view path);
bool CsgIsUsable(const rct_gx& csg);
bool CsgAtLocationIsUsable(u8string_view path);
