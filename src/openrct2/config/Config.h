/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

struct GeneralConfiguration
{
    // Paths
    utf8 *      rct1_path;
    utf8 *      rct2_path;

    // Display
    int32_t      default_display;
    int32_t      window_width;
    int32_t      window_height;
    int32_t      fullscreen_mode;
    int32_t      fullscreen_width;
    int32_t      fullscreen_height;
    float       window_scale;
    int32_t      drawing_engine;
    int32_t      scale_quality;
    bool        uncap_fps;
    bool        use_vsync;
    bool        show_fps;
    bool        minimize_fullscreen_focus_loss;

    // Map rendering
    bool        landscape_smoothing;
    bool        always_show_gridlines;
    int32_t      virtual_floor_style;
    bool        day_night_cycle;
    bool        enable_light_fx;
    bool        upper_case_banners;
    bool        render_weather_effects;
    bool        render_weather_gloom;
    bool        disable_lightning_effect;
    bool        show_guest_purchases;

    // Localisation
    int32_t      language;
    int32_t      measurement_format;
    int32_t      temperature_format;
    bool        show_height_as_units;
    int32_t      date_format;
    int32_t      currency_format;
    int32_t      custom_currency_rate;
    int32_t      custom_currency_affix;
    utf8 *      custom_currency_symbol;

    // Controls
    bool        edge_scrolling;
    int32_t      edge_scrolling_speed;
    bool        trap_cursor;
    bool        invert_viewport_drag;
    bool        zoom_to_cursor;

    // Miscellaneous
    bool        play_intro;
    int32_t      window_snap_proximity;
    bool        allow_loading_with_incorrect_checksum;
    bool        save_plugin_data;
    bool        test_unfinished_tracks;
    bool        no_test_crashes;
    bool        debugging_tools;
    int32_t      autosave_frequency;
    bool        auto_staff_placement;
    bool        handymen_mow_default;
    bool        auto_open_shops;
    int32_t      default_inspection_interval;
    int32_t      window_limit;
    int32_t      scenario_select_mode;
    bool        scenario_unlocking_enabled;
    bool        scenario_hide_mega_park;
    bool        steam_overlay_pause;
    bool        show_real_names_of_guests;
    bool        allow_early_completion;

    bool        confirmation_prompt;
    int32_t      load_save_sort;
    utf8 *      last_save_game_directory;
    utf8 *      last_save_landscape_directory;
    utf8 *      last_save_scenario_directory;
    utf8 *      last_save_track_directory;
    utf8 *      last_run_version;
};

struct InterfaceConfiguration
{
    bool        toolbar_show_finances;
    bool        toolbar_show_research;
    bool        toolbar_show_cheats;
    bool        toolbar_show_news;
    bool        toolbar_show_mute;
    bool        console_small_font;
    utf8 *      current_theme_preset;
    utf8 *      current_title_sequence_preset;
    int32_t      object_selection_filter_flags;
};

struct SoundConfiguration
{
    utf8 *      device;
    uint8_t       master_volume;
    uint8_t       title_music;
    bool        sound_enabled;
    uint8_t       sound_volume;
    bool        ride_music_enabled;
    uint8_t       ride_music_volume;
    bool        audio_focus;
};

struct TwitchConfiguration
{
    utf8 *      channel;
    utf8 *      api_url;
    bool        enable_follower_peep_names;
    bool        enable_follower_peep_tracking;
    bool        enable_chat_peep_names;
    bool        enable_chat_peep_tracking;
    bool        enable_news;
};

struct NetworkConfiguration
{
    utf8 *      player_name;
    int32_t      default_port;
    char *      listen_address;
    utf8 *      default_password;
    bool        stay_connected;
    bool        advertise;
    int32_t      maxplayers;
    utf8 *      server_name;
    utf8 *      server_description;
    utf8 *      server_greeting;
    utf8 *      master_server_url;
    utf8 *      provider_name;
    utf8 *      provider_email;
    utf8 *      provider_website;
    bool        known_keys_only;
    bool        log_chat;
    bool        log_server_actions;
    bool        pause_server_if_no_clients;
};

struct NotificationConfiguration
{
    bool        park_award;
    bool        park_marketing_campaign_finished;
    bool        park_warnings;
    bool        park_rating_warnings;
    bool        ride_broken_down;
    bool        ride_crashed;
    bool        ride_warnings;
    bool        ride_researched;
    bool        guest_warnings;
    bool        guest_lost;
    bool        guest_left_park;
    bool        guest_queuing_for_ride;
    bool        guest_on_ride;
    bool        guest_left_ride;
    bool        guest_bought_item;
    bool        guest_used_facility;
    bool        guest_died;
};

struct FontConfiguration
{
    utf8 *      file_name;
    utf8 *      font_name;
    int32_t      x_offset;
    int32_t      y_offset;
    int32_t      size_tiny;
    int32_t      size_small;
    int32_t      size_medium;
    int32_t      size_big;
    int32_t      height_tiny;
    int32_t      height_small;
    int32_t      height_medium;
    int32_t      height_big;
    bool        enable_hinting;
    int32_t      hinting_threshold;
};

enum SORT
{
    SORT_NAME_ASCENDING,
    SORT_NAME_DESCENDING,
    SORT_DATE_ASCENDING,
    SORT_DATE_DESCENDING,
};

enum TEMPERATURE_FORMAT
{
    TEMPERATURE_FORMAT_C,
    TEMPERATURE_FORMAT_F
};

enum SCALE_QUALITY
{
    SCALE_QUALITY_NN,
    SCALE_QUALITY_LINEAR,
    SCALE_QUALITY_SMOOTH_NN
};

enum MEASUREMENT_FORMAT
{
    MEASUREMENT_FORMAT_IMPERIAL,
    MEASUREMENT_FORMAT_METRIC,
    MEASUREMENT_FORMAT_SI
};

extern GeneralConfiguration         gConfigGeneral;
extern InterfaceConfiguration       gConfigInterface;
extern SoundConfiguration           gConfigSound;
extern TwitchConfiguration          gConfigTwitch;
extern NetworkConfiguration         gConfigNetwork;
extern NotificationConfiguration    gConfigNotifications;
extern FontConfiguration            gConfigFonts;

bool config_open(const utf8 * path);
bool config_save(const utf8 * path);
void config_get_default_path(utf8 *outPath, size_t size);
void config_set_defaults();
void config_release();
bool config_save_default();
bool config_find_or_browse_install_directory();
