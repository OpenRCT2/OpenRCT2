#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#pragma once

#include "../common.h"

typedef struct GeneralConfiguration
{
    // Paths
    utf8 *      rct1_path;
    utf8 *      rct2_path;

    // Display
    sint32      default_display;
    sint32      window_width;
    sint32      window_height;
    sint32      fullscreen_mode;
    sint32      fullscreen_width;
    sint32      fullscreen_height;
    float       window_scale;
    sint32      drawing_engine;
    sint32      scale_quality;
    bool        use_nn_at_integer_scales;
    bool        uncap_fps;
    bool        show_fps;
    bool        minimize_fullscreen_focus_loss;

    // Map rendering
    bool        landscape_smoothing;
    bool        always_show_gridlines;
    bool        construction_marker_colour;
    bool        day_night_cycle;
    bool        enable_light_fx;
    bool        upper_case_banners;
    bool        render_weather_effects;
    bool        render_weather_gloom;
    bool        disable_lightning_effect;
    bool        show_guest_purchases;

    // Localisation
    sint32      language;
    sint32      measurement_format;
    sint32      temperature_format;
    bool        show_height_as_units;
    sint32      date_format;
    sint32      currency_format;
    sint32      custom_currency_rate;
    sint32      custom_currency_affix;
    utf8 *      custom_currency_symbol;

    // Controls
    bool        edge_scrolling;
    bool        trap_cursor;
    bool        invert_viewport_drag;
    bool        zoom_to_cursor;

    // Miscellaneous
    bool        play_intro;
    sint32      window_snap_proximity;
    bool        allow_loading_with_incorrect_checksum;
    bool        save_plugin_data;
    bool        test_unfinished_tracks;
    bool        no_test_crashes;
    bool        debugging_tools;
    sint32      autosave_frequency;
    bool        auto_staff_placement;
    bool        handymen_mow_default;
    bool        auto_open_shops;
    sint32      default_inspection_interval;
    sint32      window_limit;
    sint32      scenario_select_mode;
    bool        scenario_unlocking_enabled;
    bool        scenario_hide_mega_park;
    bool        steam_overlay_pause;

    bool        confirmation_prompt;
    sint32      load_save_sort;
    utf8 *      last_save_game_directory;
    utf8 *      last_save_landscape_directory;
    utf8 *      last_save_scenario_directory;
    utf8 *      last_save_track_directory;
    utf8 *      last_run_version;

    sint32      screenshot_format;
} GeneralConfiguration;

typedef struct InterfaceConfiguration
{
    bool        toolbar_show_finances;
    bool        toolbar_show_research;
    bool        toolbar_show_cheats;
    bool        toolbar_show_news;
    bool        toolbar_show_mute;
    bool        select_by_track_type;
    bool        console_small_font;
    utf8 *      current_theme_preset;
    utf8 *      current_title_sequence_preset;
    sint32      object_selection_filter_flags;
} InterfaceConfiguration;

typedef struct SoundConfiguration
{
    utf8 *      device;
    uint8       master_volume;
    uint8       title_music;
    bool        sound_enabled;
    uint8       sound_volume;
    bool        ride_music_enabled;
    uint8       ride_music_volume;
    bool        audio_focus;
} SoundConfiguration;

typedef struct TwitchConfiguration
{
    utf8 *      channel;
    bool        enable_follower_peep_names;
    bool        enable_follower_peep_tracking;
    bool        enable_chat_peep_names;
    bool        enable_chat_peep_tracking;
    bool        enable_news;
} TwitchConfiguration;

typedef struct NetworkConfiguration
{
    utf8 *      player_name;
    sint32      default_port;
    char *      listen_address;
    utf8 *      default_password;
    bool        stay_connected;
    bool        advertise;
    sint32      maxplayers;
    utf8 *      server_name;
    utf8 *      server_description;
    utf8 *      server_greeting;
    utf8 *      master_server_url;
    utf8 *      provider_name;
    utf8 *      provider_email;
    utf8 *      provider_website;
    bool        known_keys_only;
    bool        log_chat;
} NetworkConfiguration;

typedef struct NotificationConfiguration
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
} NotificationConfiguration;

typedef struct FontConfiguration
{
    utf8 *      file_name;
    utf8 *      font_name;
    sint32      x_offset;
    sint32      y_offset;
    sint32      size_tiny;
    sint32      size_small;
    sint32      size_medium;
    sint32      size_big;
    sint32      height_tiny;
    sint32      height_small;
    sint32      height_medium;
    sint32      height_big;
} FontConfiguration;

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

enum MEASUREMENT_FORMAT
{
    MEASUREMENT_FORMAT_IMPERIAL,
    MEASUREMENT_FORMAT_METRIC,
    MEASUREMENT_FORMAT_SI
};

#ifdef __cplusplus
extern "C"
{
#endif
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
    bool config_open_default();
    bool config_save_default();
    bool config_find_or_browse_install_directory();
#ifdef __cplusplus
}
#endif
