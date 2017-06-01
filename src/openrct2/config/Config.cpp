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

#include <memory>
#include "../Context.h"
#include "../core/Console.hpp"
#include "../core/Exception.hpp"
#include "../core/FileStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../drawing/IDrawingEngine.h"
#include "../interface/window.h"
#include "../network/network.h"
#include "../OpenRCT2.h"
#include "../ui/UiContext.h"
#include "Config.h"
#include "IniReader.hpp"
#include "IniWriter.hpp"

extern "C"
{
    #include "../localisation/currency.h"
    #include "../localisation/date.h"
    #include "../localisation/language.h"
    #include "../platform/platform.h"
    #include "../scenario/scenario.h"
}

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

namespace Config
{
    #pragma region Enums

    static auto Enum_MeasurementFormat = ConfigEnum<sint32>(
    {
        ConfigEnumEntry<sint32>("IMPERIAL", MEASUREMENT_FORMAT_IMPERIAL),
        ConfigEnumEntry<sint32>("METRIC", MEASUREMENT_FORMAT_METRIC),
        ConfigEnumEntry<sint32>("SI", MEASUREMENT_FORMAT_SI),
    });

    static auto Enum_Currency = ConfigEnum<sint32>(
    {
        ConfigEnumEntry<sint32>("GBP", CURRENCY_POUNDS),
        ConfigEnumEntry<sint32>("USD", CURRENCY_DOLLARS),
        ConfigEnumEntry<sint32>("FRF", CURRENCY_FRANC),
        ConfigEnumEntry<sint32>("DEM", CURRENCY_DEUTSCHMARK),
        ConfigEnumEntry<sint32>("JPY", CURRENCY_YEN),
        ConfigEnumEntry<sint32>("ESP", CURRENCY_PESETA),
        ConfigEnumEntry<sint32>("ITL", CURRENCY_LIRA),
        ConfigEnumEntry<sint32>("NLG", CURRENCY_GUILDERS),
        ConfigEnumEntry<sint32>("SEK", CURRENCY_KRONA),
        ConfigEnumEntry<sint32>("EUR", CURRENCY_EUROS),
        ConfigEnumEntry<sint32>("KRW", CURRENCY_WON),
        ConfigEnumEntry<sint32>("RUB", CURRENCY_ROUBLE),
        ConfigEnumEntry<sint32>("CZK", CURRENCY_CZECH_KORUNA),
        ConfigEnumEntry<sint32>("HKD", CURRENCY_HKD),
        ConfigEnumEntry<sint32>("TWD", CURRENCY_TWD),
        ConfigEnumEntry<sint32>("CNY", CURRENCY_YUAN),
        ConfigEnumEntry<sint32>("CUSTOM", CURRENCY_CUSTOM),
    });

    static auto Enum_CurrencySymbolAffix = ConfigEnum<sint32>(
    {
        ConfigEnumEntry<sint32>("PREFIX", CURRENCY_PREFIX),
        ConfigEnumEntry<sint32>("SUFFIX", CURRENCY_SUFFIX),
    });

    static auto Enum_DateFormat = ConfigEnum<sint32>(
    {
        ConfigEnumEntry<sint32>("DD/MM/YY", DATE_FORMAT_DAY_MONTH_YEAR),
        ConfigEnumEntry<sint32>("MM/DD/YY", DATE_FORMAT_MONTH_DAY_YEAR),
        ConfigEnumEntry<sint32>("YY/MM/DD", DATE_FORMAT_YEAR_MONTH_DAY),
        ConfigEnumEntry<sint32>("YY/DD/MM", DATE_FORMAT_YEAR_DAY_MONTH),
    });

    static auto Enum_DrawingEngine = ConfigEnum<sint32>(
    {
        ConfigEnumEntry<sint32>("SOFTWARE", DRAWING_ENGINE_SOFTWARE),
        ConfigEnumEntry<sint32>("SOFTWARE_HWD", DRAWING_ENGINE_SOFTWARE_WITH_HARDWARE_DISPLAY),
        ConfigEnumEntry<sint32>("OPENGL", DRAWING_ENGINE_OPENGL),
    });

    static auto Enum_Temperature = ConfigEnum<sint32>(
    {
        ConfigEnumEntry<sint32>("CELSIUS", TEMPERATURE_FORMAT_C),
        ConfigEnumEntry<sint32>("FAHRENHEIT", TEMPERATURE_FORMAT_F),
    });

    /**
     * Config enum wrapping LanguagesDescriptors.
     */
    static class LanguageConfigEnum final : public IConfigEnum<sint32>
    {
    public:
        std::string GetName(sint32 value) const override
        {
            return LanguagesDescriptors[value].locale;
        }

        sint32 GetValue(const std::string &key, sint32 defaultValue) const override
        {
            sint32 i = 0;
            for (const auto &langDesc : LanguagesDescriptors)
            {
                if (String::Equals(key.c_str(), langDesc.locale))
                {
                    return i;
                }
                i++;
            }
            return defaultValue;
        }
    } Enum_LanguageEnum;

    #pragma endregion

    static void ReadGeneral(IIniReader * reader)
    {
        if (reader->ReadSection("general"))
        {
            auto model = &gConfigGeneral;
            model->always_show_gridlines = reader->GetBoolean("always_show_gridlines", false);
            model->autosave_frequency = reader->GetSint32("autosave", AUTOSAVE_EVERY_5MINUTES);
            model->confirmation_prompt = reader->GetBoolean("confirmation_prompt", false);
            model->construction_marker_colour = reader->GetBoolean("construction_marker_colour", false);
            model->currency_format = reader->GetEnum<sint32>("currency_format", platform_get_locale_currency(), Enum_Currency);
            model->custom_currency_rate = reader->GetSint32("custom_currency_rate", 10);
            model->custom_currency_affix = reader->GetEnum<sint32>("custom_currency_affix", CURRENCY_SUFFIX, Enum_CurrencySymbolAffix);
            model->custom_currency_symbol = reader->GetCString("custom_currency_symbol", "Ctm");
            model->edge_scrolling = reader->GetBoolean("edge_scrolling", true);
            model->fullscreen_mode = reader->GetSint32("fullscreen_mode", 0);
            model->fullscreen_height = reader->GetSint32("fullscreen_height", -1);
            model->fullscreen_width = reader->GetSint32("fullscreen_width", -1);
            model->rct1_path = reader->GetCString("rct1_path", nullptr);
            model->rct2_path = reader->GetCString("game_path", nullptr);
            model->landscape_smoothing = reader->GetBoolean("landscape_smoothing", true);
            model->language = reader->GetEnum<sint32>("language", platform_get_locale_language(), Enum_LanguageEnum);
            model->measurement_format = reader->GetEnum<sint32>("measurement_format", platform_get_locale_measurement_format(), Enum_MeasurementFormat);
            model->play_intro = reader->GetBoolean("play_intro", false);
            model->save_plugin_data = reader->GetBoolean("save_plugin_data", true);
            model->debugging_tools = reader->GetBoolean("debugging_tools", false);
            model->show_height_as_units = reader->GetBoolean("show_height_as_units", false);
            model->temperature_format = reader->GetEnum<sint32>("temperature_format", platform_get_locale_temperature_format(), Enum_Temperature);
            model->window_height = reader->GetSint32("window_height", -1);
            model->window_snap_proximity = reader->GetSint32("window_snap_proximity", 5);
            model->window_width = reader->GetSint32("window_width", -1);
            model->default_display = reader->GetSint32("default_display", 0);
            model->drawing_engine = reader->GetEnum<sint32>("drawing_engine", DRAWING_ENGINE_SOFTWARE, Enum_DrawingEngine);
            model->uncap_fps = reader->GetBoolean("uncap_fps", false);

            // Default config setting is false until ghost trains are implemented #4540
            model->test_unfinished_tracks = reader->GetBoolean("test_unfinished_tracks", false);

            model->no_test_crashes = reader->GetBoolean("no_test_crashes", false);
            model->date_format = reader->GetEnum<sint32>("date_format", platform_get_locale_date_format(), Enum_DateFormat);
            model->auto_staff_placement = reader->GetBoolean("auto_staff", true);
            model->handymen_mow_default = reader->GetBoolean("handymen_mow_default", false);
            model->default_inspection_interval = reader->GetSint32("default_inspection_interval", 2);
            model->last_run_version = reader->GetCString("last_run_version", nullptr);
            model->invert_viewport_drag = reader->GetBoolean("invert_viewport_drag", false);
            model->load_save_sort = reader->GetSint32("load_save_sort", SORT_NAME_ASCENDING);
            model->minimize_fullscreen_focus_loss = reader->GetBoolean("minimize_fullscreen_focus_loss", true);

            // Default config setting is false until the games canvas can be separated from the effect
            model->day_night_cycle = reader->GetBoolean("day_night_cycle", false);

            model->enable_light_fx = reader->GetBoolean("enable_light_fx", false);
            model->upper_case_banners = reader->GetBoolean("upper_case_banners", false);
            model->disable_lightning_effect = reader->GetBoolean("disable_lightning_effect", false);
            model->allow_loading_with_incorrect_checksum = reader->GetBoolean("allow_loading_with_incorrect_checksum", true);
            model->steam_overlay_pause = reader->GetBoolean("steam_overlay_pause", true);
            model->window_scale = reader->GetFloat("window_scale", 1.0f);
            model->scale_quality = reader->GetSint32("scale_quality", 1);
            model->use_nn_at_integer_scales = reader->GetBoolean("use_nn_at_integer_scales", true);
            model->show_fps = reader->GetBoolean("show_fps", false);
            model->trap_cursor = reader->GetBoolean("trap_cursor", false);
            model->auto_open_shops = reader->GetBoolean("auto_open_shops", false);
            model->scenario_select_mode = reader->GetSint32("scenario_select_mode", SCENARIO_SELECT_MODE_ORIGIN);
            model->scenario_unlocking_enabled = reader->GetBoolean("scenario_unlocking_enabled", true);
            model->scenario_hide_mega_park = reader->GetBoolean("scenario_hide_mega_park", true);
            model->last_save_game_directory = reader->GetCString("last_game_directory", nullptr);
            model->last_save_landscape_directory = reader->GetCString("last_landscape_directory", nullptr);
            model->last_save_scenario_directory = reader->GetCString("last_scenario_directory", nullptr);
            model->last_save_track_directory = reader->GetCString("last_track_directory", nullptr);
            model->window_limit = reader->GetSint32("window_limit", WINDOW_LIMIT_MAX);
            model->zoom_to_cursor = reader->GetBoolean("zoom_to_cursor", true);
            model->render_weather_effects = reader->GetBoolean("render_weather_effects", true);
            model->render_weather_gloom = reader->GetBoolean("render_weather_gloom", true);
        }
    }

    static void WriteGeneral(IIniWriter * writer)
    {
        auto model = &gConfigGeneral;
        writer->WriteSection("general");
        writer->WriteBoolean("always_show_gridlines", model->always_show_gridlines);
        writer->WriteSint32("autosave", model->autosave_frequency);
        writer->WriteBoolean("confirmation_prompt", model->confirmation_prompt);
        writer->WriteBoolean("construction_marker_colour", model->construction_marker_colour);
        writer->WriteEnum<sint32>("currency_format", model->currency_format, Enum_Currency);
        writer->WriteSint32("custom_currency_rate", model->custom_currency_rate);
        writer->WriteEnum<sint32>("custom_currency_affix", model->custom_currency_affix, Enum_CurrencySymbolAffix);
        writer->WriteString("custom_currency_symbol", model->custom_currency_symbol);
        writer->WriteBoolean("edge_scrolling", model->edge_scrolling);
        writer->WriteSint32("fullscreen_mode", model->fullscreen_mode);
        writer->WriteSint32("fullscreen_height", model->fullscreen_height);
        writer->WriteSint32("fullscreen_width", model->fullscreen_width);
        writer->WriteString("rct1_path", model->rct1_path);
        writer->WriteString("game_path", model->rct2_path);
        writer->WriteBoolean("landscape_smoothing", model->landscape_smoothing);
        writer->WriteEnum<sint32>("language", model->language, Enum_LanguageEnum);
        writer->WriteEnum<sint32>("measurement_format", model->measurement_format, Enum_MeasurementFormat);
        writer->WriteBoolean("play_intro", model->play_intro);
        writer->WriteBoolean("save_plugin_data", model->save_plugin_data);
        writer->WriteBoolean("debugging_tools", model->debugging_tools);
        writer->WriteBoolean("show_height_as_units", model->show_height_as_units);
        writer->WriteEnum<sint32>("temperature_format", model->temperature_format, Enum_Temperature);
        writer->WriteSint32("window_height", model->window_height);
        writer->WriteSint32("window_snap_proximity", model->window_snap_proximity);
        writer->WriteSint32("window_width", model->window_width);
        writer->WriteSint32("default_display", model->default_display);
        writer->WriteEnum<sint32>("drawing_engine", model->drawing_engine, Enum_DrawingEngine);
        writer->WriteBoolean("uncap_fps", model->uncap_fps);
        writer->WriteBoolean("test_unfinished_tracks", model->test_unfinished_tracks);
        writer->WriteBoolean("no_test_crashes", model->no_test_crashes);
        writer->WriteEnum<sint32>("date_format", model->date_format, Enum_DateFormat);
        writer->WriteBoolean("auto_staff", model->auto_staff_placement);
        writer->WriteBoolean("handymen_mow_default", model->handymen_mow_default);
        writer->WriteSint32("default_inspection_interval", model->default_inspection_interval);
        writer->WriteString("last_run_version", model->last_run_version);
        writer->WriteBoolean("invert_viewport_drag", model->invert_viewport_drag);
        writer->WriteSint32("load_save_sort", model->load_save_sort);
        writer->WriteBoolean("minimize_fullscreen_focus_loss", model->minimize_fullscreen_focus_loss);
        writer->WriteBoolean("day_night_cycle", model->day_night_cycle);
        writer->WriteBoolean("enable_light_fx", model->enable_light_fx);
        writer->WriteBoolean("upper_case_banners", model->upper_case_banners);
        writer->WriteBoolean("disable_lightning_effect", model->disable_lightning_effect);
        writer->WriteBoolean("allow_loading_with_incorrect_checksum", model->allow_loading_with_incorrect_checksum);
        writer->WriteBoolean("steam_overlay_pause", model->steam_overlay_pause);
        writer->WriteFloat("window_scale", model->window_scale);
        writer->WriteSint32("scale_quality", model->scale_quality);
        writer->WriteBoolean("use_nn_at_integer_scales", model->use_nn_at_integer_scales);
        writer->WriteBoolean("show_fps", model->show_fps);
        writer->WriteBoolean("trap_cursor", model->trap_cursor);
        writer->WriteBoolean("auto_open_shops", model->auto_open_shops);
        writer->WriteSint32("scenario_select_mode", model->scenario_select_mode);
        writer->WriteBoolean("scenario_unlocking_enabled", model->scenario_unlocking_enabled);
        writer->WriteBoolean("scenario_hide_mega_park", model->scenario_hide_mega_park);
        writer->WriteString("last_game_directory", model->last_save_game_directory);
        writer->WriteString("last_landscape_directory", model->last_save_landscape_directory);
        writer->WriteString("last_scenario_directory", model->last_save_scenario_directory);
        writer->WriteString("last_track_directory", model->last_save_track_directory);
        writer->WriteSint32("window_limit", model->window_limit);
        writer->WriteBoolean("zoom_to_cursor", model->zoom_to_cursor);
        writer->WriteBoolean("render_weather_effects", model->render_weather_effects);
        writer->WriteBoolean("render_weather_gloom", model->render_weather_gloom);
    }

    static void ReadInterface(IIniReader * reader)
    {
        if (reader->ReadSection("interface"))
        {
            auto model = &gConfigInterface;
            model->toolbar_show_finances = reader->GetBoolean("toolbar_show_finances", true);
            model->toolbar_show_research = reader->GetBoolean("toolbar_show_research", true);
            model->toolbar_show_cheats = reader->GetBoolean("toolbar_show_cheats", false);
            model->toolbar_show_news = reader->GetBoolean("toolbar_show_news", false);
            model->toolbar_show_mute = reader->GetBoolean("toolbar_show_mute", false);
            model->select_by_track_type = reader->GetBoolean("select_by_track_type", false);
            model->console_small_font = reader->GetBoolean("console_small_font", false);
            model->current_theme_preset = reader->GetCString("current_theme", "*RCT2");
            model->current_title_sequence_preset = reader->GetCString("current_title_sequence", "*OPENRCT2");
            model->object_selection_filter_flags = reader->GetSint32("object_selection_filter_flags", 0x7EF);
        }
    }

    static void WriteInterface(IIniWriter * writer)
    {
        auto model = &gConfigInterface;
        writer->WriteSection("interface");
        writer->WriteBoolean("toolbar_show_finances", model->toolbar_show_finances);
        writer->WriteBoolean("toolbar_show_research", model->toolbar_show_research);
        writer->WriteBoolean("toolbar_show_cheats", model->toolbar_show_cheats);
        writer->WriteBoolean("toolbar_show_news", model->toolbar_show_news);
        writer->WriteBoolean("toolbar_show_mute", model->toolbar_show_mute);
        writer->WriteBoolean("select_by_track_type", model->select_by_track_type);
        writer->WriteBoolean("console_small_font", model->console_small_font);
        writer->WriteString("current_theme", model->current_theme_preset);
        writer->WriteString("current_title_sequence", model->current_title_sequence_preset);
        writer->WriteSint32("object_selection_filter_flags", model->object_selection_filter_flags);
    }

    static void ReadSound(IIniReader * reader)
    {
        if (reader->ReadSection("sound"))
        {
            auto model = &gConfigSound;
            model->master_volume = reader->GetSint32("master_volume", 100);
            model->title_music = reader->GetSint32("title_music", 2);
            model->sound_enabled = reader->GetBoolean("sound", true);
            model->sound_volume = reader->GetSint32("sound_volume", 100);
            model->ride_music_enabled = reader->GetBoolean("ride_music", true);
            model->ride_music_volume = reader->GetSint32("ride_music_volume", 100);
            model->audio_focus = reader->GetBoolean("audio_focus", false);
            model->device = reader->GetCString("audio_device", nullptr);
        }
    }

    static void WriteSound(IIniWriter * writer)
    {
        auto model = &gConfigSound;
        writer->WriteSection("sound");
        writer->WriteSint32("master_volume", model->master_volume);
        writer->WriteSint32("title_music", model->title_music);
        writer->WriteBoolean("sound", model->sound_enabled);
        writer->WriteSint32("sound_volume", model->sound_volume);
        writer->WriteBoolean("ride_music", model->ride_music_enabled);
        writer->WriteSint32("ride_music_volume", model->ride_music_volume);
        writer->WriteBoolean("audio_focus", model->audio_focus);
        writer->WriteString("audio_device", model->device);
    }

    static void ReadNetwork(IIniReader * reader)
    {
        if (reader->ReadSection("network"))
        {
            auto model = &gConfigNetwork;
            model->player_name = reader->GetCString("player_name", "Player");
            model->default_port = reader->GetSint32("default_port", NETWORK_DEFAULT_PORT);
            model->default_password = reader->GetCString("default_password", nullptr);
            model->stay_connected = reader->GetBoolean("stay_connected", true);
            model->advertise = reader->GetBoolean("advertise", true);
            model->maxplayers = reader->GetSint32("maxplayers", 16);
            model->server_name = reader->GetCString("server_name", "Server");
            model->server_description = reader->GetCString("server_description", nullptr);
            model->server_greeting = reader->GetCString("server_greeting", nullptr);
            model->master_server_url = reader->GetCString("master_server_url", nullptr);
            model->provider_name = reader->GetCString("provider_name", nullptr);
            model->provider_email = reader->GetCString("provider_email", nullptr);
            model->provider_website = reader->GetCString("provider_website", nullptr);
            model->known_keys_only = reader->GetBoolean("known_keys_only", false);
            model->log_chat = reader->GetBoolean("log_chat", false);
        }
    }

    static void WriteNetwork(IIniWriter * writer)
    {
        auto model = &gConfigNetwork;
        writer->WriteSection("network");
        writer->WriteString("player_name", model->player_name);
        writer->WriteSint32("default_port", model->default_port);
        writer->WriteString("default_password", model->default_password);
        writer->WriteBoolean("stay_connected", model->stay_connected);
        writer->WriteBoolean("advertise", model->advertise);
        writer->WriteSint32("maxplayers", model->maxplayers);
        writer->WriteString("server_name", model->server_name);
        writer->WriteString("server_description", model->server_description);
        writer->WriteString("server_greeting", model->server_greeting);
        writer->WriteString("master_server_url", model->master_server_url);
        writer->WriteString("provider_name", model->provider_name);
        writer->WriteString("provider_email", model->provider_email);
        writer->WriteString("provider_website", model->provider_website);
        writer->WriteBoolean("known_keys_only", model->known_keys_only);
        writer->WriteBoolean("log_chat", model->log_chat);
    }

    static void ReadNotifications(IIniReader * reader)
    {
        if (reader->ReadSection("notifications"))
        {
            auto model = &gConfigNotifications;
            model->park_award = reader->GetBoolean("park_award", true);
            model->park_marketing_campaign_finished = reader->GetBoolean("park_marketing_campaign_finished", true);
            model->park_warnings = reader->GetBoolean("park_warnings", true);
            model->park_rating_warnings = reader->GetBoolean("park_rating_warnings", true);
            model->ride_broken_down = reader->GetBoolean("ride_broken_down", true);
            model->ride_crashed = reader->GetBoolean("ride_crashed", true);
            model->ride_warnings = reader->GetBoolean("ride_warnings", true);
            model->ride_researched = reader->GetBoolean("ride_researched", true);
            model->guest_warnings = reader->GetBoolean("guest_warnings", true);
            model->guest_lost = reader->GetBoolean("guest_lost", false);
            model->guest_left_park = reader->GetBoolean("guest_entered_left_park", true);
            model->guest_queuing_for_ride = reader->GetBoolean("guest_queuing_for_ride", true);
            model->guest_on_ride = reader->GetBoolean("guest_on_ride", true);
            model->guest_left_ride = reader->GetBoolean("guest_left_ride", true);
            model->guest_bought_item = reader->GetBoolean("guest_bought_item", true);
            model->guest_used_facility = reader->GetBoolean("guest_used_facility", true);
            model->guest_died = reader->GetBoolean("guest_died", true);
        }
    }

    static void WriteNotifications(IIniWriter * writer)
    {
        auto model = &gConfigNotifications;
        writer->WriteSection("notifications");
        writer->WriteBoolean("park_award", model->park_award);
        writer->WriteBoolean("park_marketing_campaign_finished", model->park_marketing_campaign_finished);
        writer->WriteBoolean("park_warnings", model->park_warnings);
        writer->WriteBoolean("park_rating_warnings", model->park_rating_warnings);
        writer->WriteBoolean("ride_broken_down", model->ride_broken_down);
        writer->WriteBoolean("ride_crashed", model->ride_crashed);
        writer->WriteBoolean("ride_warnings", model->ride_warnings);
        writer->WriteBoolean("ride_researched", model->ride_researched);
        writer->WriteBoolean("guest_warnings", model->guest_warnings);
        writer->WriteBoolean("guest_lost", model->guest_lost);
        writer->WriteBoolean("guest_left_park", model->guest_left_park);
        writer->WriteBoolean("guest_queuing_for_ride", model->guest_queuing_for_ride);
        writer->WriteBoolean("guest_on_ride", model->guest_on_ride);
        writer->WriteBoolean("guest_left_ride", model->guest_left_ride);
        writer->WriteBoolean("guest_bought_item", model->guest_bought_item);
        writer->WriteBoolean("guest_used_facility", model->guest_used_facility);
        writer->WriteBoolean("guest_died", model->guest_died);
    }

    static void ReadTwitch(IIniReader * reader)
    {
        if (reader->ReadSection("sound"))
        {
            auto model = &gConfigTwitch;
            model->channel = reader->GetCString("channel", nullptr);
            model->enable_follower_peep_names = reader->GetBoolean("follower_peep_names", true);
            model->enable_follower_peep_tracking = reader->GetBoolean("follower_peep_tracking", false);
            model->enable_chat_peep_names = reader->GetBoolean("chat_peep_names", true);
            model->enable_chat_peep_tracking = reader->GetBoolean("chat_peep_tracking", true);
            model->enable_news = reader->GetBoolean("news", false);
        }
    }

    static void WriteTwitch(IIniWriter * writer)
    {
        auto model = &gConfigTwitch;
        writer->WriteSection("twitch");
        writer->WriteString("channel", model->channel);
        writer->WriteBoolean("follower_peep_names", model->enable_follower_peep_names);
        writer->WriteBoolean("follower_peep_tracking", model->enable_follower_peep_tracking);
        writer->WriteBoolean("chat_peep_names", model->enable_chat_peep_names);
        writer->WriteBoolean("chat_peep_tracking", model->enable_chat_peep_tracking);
        writer->WriteBoolean("news", model->enable_news);
    }

    static void ReadFont(IIniReader * reader)
    {
        if (reader->ReadSection("font"))
        {
            auto model = &gConfigFonts;
            model->file_name = reader->GetCString("file_name", nullptr);
            model->font_name = reader->GetCString("font_name", nullptr);
            model->x_offset = reader->GetSint32("x_offset", false);
            model->y_offset = reader->GetSint32("y_offset", true);
            model->size_tiny = reader->GetSint32("size_tiny", true);
            model->size_small = reader->GetSint32("size_small", false);
            model->size_medium = reader->GetSint32("size_medium", false);
            model->size_big = reader->GetSint32("size_big", false);
            model->height_tiny = reader->GetSint32("height_tiny", false);
            model->height_small = reader->GetSint32("height_small", false);
            model->height_medium = reader->GetSint32("height_medium", false);
            model->height_big = reader->GetSint32("height_big", false);
        }
    }

    static void WriteFont(IIniWriter * writer)
    {
        auto model = &gConfigFonts;
        writer->WriteSection("font");
        writer->WriteString("file_name", model->file_name);
        writer->WriteString("font_name", model->font_name);
        writer->WriteSint32("x_offset", model->x_offset);
        writer->WriteSint32("y_offset", model->y_offset);
        writer->WriteSint32("size_tiny", model->size_tiny);
        writer->WriteSint32("size_small", model->size_small);
        writer->WriteSint32("size_medium", model->size_medium);
        writer->WriteSint32("size_big", model->size_big);
        writer->WriteSint32("height_tiny", model->height_tiny);
        writer->WriteSint32("height_small", model->height_small);
        writer->WriteSint32("height_medium", model->height_medium);
        writer->WriteSint32("height_big", model->height_big);
    }

    static bool SetDefaults()
    {
        try
        {
            auto reader = std::unique_ptr<IIniReader>(CreateDefaultIniReader());
            ReadGeneral(reader.get());
            ReadInterface(reader.get());
            ReadSound(reader.get());
            ReadNetwork(reader.get());
            ReadNotifications(reader.get());
            ReadTwitch(reader.get());
            ReadFont(reader.get());
            return true;
        }
        catch (const Exception &)
        {
            return false;
        }
    }

    static bool ReadFile(const std::string &path)
    {
        try
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            auto reader = std::unique_ptr<IIniReader>(CreateIniReader(&fs));
            ReadGeneral(reader.get());
            ReadInterface(reader.get());
            ReadSound(reader.get());
            ReadNetwork(reader.get());
            ReadNotifications(reader.get());
            ReadTwitch(reader.get());
            ReadFont(reader.get());
            return true;
        }
        catch (const Exception &)
        {
            return false;
        }
    }

    static bool WriteFile(const std::string &path)
    {
        try
        {
            auto fs = FileStream(path, FILE_MODE_WRITE);
            auto writer = std::unique_ptr<IIniWriter>(CreateIniWriter(&fs));
            WriteGeneral(writer.get());
            WriteInterface(writer.get());
            WriteSound(writer.get());
            WriteNetwork(writer.get());
            WriteNotifications(writer.get());
            WriteTwitch(writer.get());
            WriteFont(writer.get());
            return true;
        }
        catch (const Exception &ex)
        {
            Console::WriteLine("Error saving to '%s'", path.c_str());
            Console::WriteLine(ex.GetMessage());
            return false;
        }
    }

    /**
     * Attempts to find the RCT2 installation directory.
     * This should be created from some other resource when OpenRCT2 grows.
     * @param resultPath Pointer to where the absolute path of the RCT2 installation directory will be copied to.
     * @returns 1 if successful, otherwise 0.
     */
    static std::string FindRCT2Path()
    {
        log_verbose("config_find_rct2_path(...)");

        static const utf8 * searchLocations[] =
        {
            "C:\\GOG Games\\RollerCoaster Tycoon 2 Triple Thrill Pack",
            "C:\\Program Files\\Atari\\RollerCoaster Tycoon 2",
            "C:\\Program Files\\GalaxyClient\\Games\\RollerCoaster Tycoon 2 Triple Thrill Pack",
            "C:\\Program Files\\Infogrames\\RollerCoaster Tycoon 2",
            "C:\\Program Files\\Infogrames Interactive\\RollerCoaster Tycoon 2",
            "C:\\Program Files\\Steam\\steamapps\\common\\Rollercoaster Tycoon 2",
            "C:\\Program Files (x86)\\Atari\\RollerCoaster Tycoon 2",
            "C:\\Program Files (x86)\\GalaxyClient\\Games\\RollerCoaster Tycoon 2 Triple Thrill Pack",
            "C:\\Program Files (x86)\\Infogrames\\RollerCoaster Tycoon 2",
            "C:\\Program Files (x86)\\Infogrames Interactive\\RollerCoaster Tycoon 2",
            "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Rollercoaster Tycoon 2"
        };

        for (const utf8 * location : searchLocations)
        {
            if (platform_original_game_data_exists(location))
            {
                return location;
            }
        }
        if (platform_original_game_data_exists(gExePath))
        {
            return gExePath;
        }
        return std::string();
    }
}

extern "C"
{
    GeneralConfiguration         gConfigGeneral;
    InterfaceConfiguration       gConfigInterface;
    SoundConfiguration           gConfigSound;
    TwitchConfiguration          gConfigTwitch;
    NetworkConfiguration         gConfigNetwork;
    NotificationConfiguration    gConfigNotifications;
    FontConfiguration            gConfigFonts;

    void config_set_defaults()
    {
        config_release();
        Config::SetDefaults();
    }

    bool config_open(const utf8 * path)
    {
        config_release();
        return Config::ReadFile(path);
    }

    bool config_save(const utf8 * path)
    {
        return Config::WriteFile(path);
    }

    void config_release()
    {
        SafeFree(gConfigGeneral.rct1_path);
        SafeFree(gConfigGeneral.rct2_path);
        SafeFree(gConfigGeneral.custom_currency_symbol);
        SafeFree(gConfigGeneral.last_save_game_directory);
        SafeFree(gConfigGeneral.last_save_landscape_directory);
        SafeFree(gConfigGeneral.last_save_scenario_directory);
        SafeFree(gConfigGeneral.last_save_track_directory);
        SafeFree(gConfigGeneral.last_run_version);
        SafeFree(gConfigInterface.current_theme_preset);
        SafeFree(gConfigInterface.current_title_sequence_preset);
        SafeFree(gConfigSound.device);
        SafeFree(gConfigTwitch.channel);
        SafeFree(gConfigNetwork.player_name);
        SafeFree(gConfigNetwork.default_password);
        SafeFree(gConfigNetwork.server_name);
        SafeFree(gConfigNetwork.server_description);
        SafeFree(gConfigNetwork.server_greeting);
        SafeFree(gConfigNetwork.master_server_url);
        SafeFree(gConfigNetwork.provider_name);
        SafeFree(gConfigNetwork.provider_email);
        SafeFree(gConfigNetwork.provider_website);
        SafeFree(gConfigFonts.file_name);
        SafeFree(gConfigFonts.font_name);
    }

    void config_get_default_path(utf8 * outPath, size_t size)
    {
        platform_get_user_directory(outPath, nullptr, size);
        Path::Append(outPath, size, "config.ini");
    }

    bool config_open_default()
    {
        utf8 path[MAX_PATH];
        config_get_default_path(path, sizeof(path));
        if (config_open(path))
        {
            currency_load_custom_currency_config();
            return true;
        }
        return false;
    }

    bool config_save_default()
    {
        utf8 path[MAX_PATH];
        config_get_default_path(path, sizeof(path));
        if (config_save(path))
        {
            return true;
        }
        return false;
    }

    bool config_find_or_browse_install_directory()
    {
        std::string path = Config::FindRCT2Path();
        if (!path.empty())
        {
            Memory::Free(gConfigGeneral.rct2_path);
            gConfigGeneral.rct2_path = String::Duplicate(path.c_str());
        }
        else
        {
            if (gOpenRCT2Headless)
            {
                return false;
            }
            while (1)
            {
                IUiContext * uiContext = GetContext()->GetUiContext();
                uiContext->ShowMessageBox("OpenRCT2 needs files from the original RollerCoaster Tycoon 2 in order to work. Please select the directory where you installed RollerCoaster Tycoon 2.");
                utf8 * installPath = platform_open_directory_browser("Please select your RCT2 directory");
                if (installPath == nullptr)
                {
                    return false;
                }

                Memory::Free(gConfigGeneral.rct2_path);
                gConfigGeneral.rct2_path = installPath;

                if (platform_original_game_data_exists(installPath))
                {
                    return true;
                }

                std::string message = String::StdFormat("Could not find %s" PATH_SEPARATOR "Data" PATH_SEPARATOR "g1.dat at this path", installPath);
                uiContext->ShowMessageBox(message);
            }
        }
        return true;
    }
}
