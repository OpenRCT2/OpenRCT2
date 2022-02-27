/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Config.h"

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../core/Console.hpp"
#include "../core/File.h"
#include "../core/FileStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../drawing/IDrawingEngine.h"
#include "../interface/Window.h"
#include "../localisation/Currency.h"
#include "../localisation/Date.h"
#include "../localisation/Language.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../network/network.h"
#include "../paint/VirtualFloor.h"
#include "../platform/Platform.h"
#include "../rct1/Limits.h"
#include "../scenario/Scenario.h"
#include "../ui/UiContext.h"
#include "../util/Util.h"
#include "ConfigEnum.hpp"
#include "IniReader.hpp"
#include "IniWriter.hpp"

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

namespace Config
{
#pragma region Enums

    static const auto Enum_MeasurementFormat = ConfigEnum<MeasurementFormat>({
        ConfigEnumEntry<MeasurementFormat>("IMPERIAL", MeasurementFormat::Imperial),
        ConfigEnumEntry<MeasurementFormat>("METRIC", MeasurementFormat::Metric),
        ConfigEnumEntry<MeasurementFormat>("SI", MeasurementFormat::SI),
    });

    static const auto Enum_Currency = ConfigEnum<CurrencyType>({
        ConfigEnumEntry<CurrencyType>("GBP", CurrencyType::Pounds),
        ConfigEnumEntry<CurrencyType>("USD", CurrencyType::Dollars),
        ConfigEnumEntry<CurrencyType>("FRF", CurrencyType::Franc),
        ConfigEnumEntry<CurrencyType>("DEM", CurrencyType::DeutscheMark),
        ConfigEnumEntry<CurrencyType>("JPY", CurrencyType::Yen),
        ConfigEnumEntry<CurrencyType>("ESP", CurrencyType::Peseta),
        ConfigEnumEntry<CurrencyType>("ITL", CurrencyType::Lira),
        ConfigEnumEntry<CurrencyType>("NLG", CurrencyType::Guilders),
        ConfigEnumEntry<CurrencyType>("SEK", CurrencyType::Krona),
        ConfigEnumEntry<CurrencyType>("EUR", CurrencyType::Euros),
        ConfigEnumEntry<CurrencyType>("KRW", CurrencyType::Won),
        ConfigEnumEntry<CurrencyType>("RUB", CurrencyType::Rouble),
        ConfigEnumEntry<CurrencyType>("CZK", CurrencyType::CzechKoruna),
        ConfigEnumEntry<CurrencyType>("HKD", CurrencyType::HKD),
        ConfigEnumEntry<CurrencyType>("TWD", CurrencyType::TWD),
        ConfigEnumEntry<CurrencyType>("CNY", CurrencyType::Yuan),
        ConfigEnumEntry<CurrencyType>("HUF", CurrencyType::Forint),
        ConfigEnumEntry<CurrencyType>("CUSTOM", CurrencyType::Custom),
    });

    static const auto Enum_CurrencySymbolAffix = ConfigEnum<CurrencyAffix>({
        ConfigEnumEntry<CurrencyAffix>("PREFIX", CurrencyAffix::Prefix),
        ConfigEnumEntry<CurrencyAffix>("SUFFIX", CurrencyAffix::Suffix),
    });

    static const auto Enum_DateFormat = ConfigEnum<int32_t>({
        ConfigEnumEntry<int32_t>("DD/MM/YY", DATE_FORMAT_DAY_MONTH_YEAR),
        ConfigEnumEntry<int32_t>("MM/DD/YY", DATE_FORMAT_MONTH_DAY_YEAR),
        ConfigEnumEntry<int32_t>("YY/MM/DD", DATE_FORMAT_YEAR_MONTH_DAY),
        ConfigEnumEntry<int32_t>("YY/DD/MM", DATE_FORMAT_YEAR_DAY_MONTH),
    });

    static const auto Enum_DrawingEngine = ConfigEnum<DrawingEngine>({
        ConfigEnumEntry<DrawingEngine>("SOFTWARE", DrawingEngine::Software),
        ConfigEnumEntry<DrawingEngine>("SOFTWARE_HWD", DrawingEngine::SoftwareWithHardwareDisplay),
        ConfigEnumEntry<DrawingEngine>("OPENGL", DrawingEngine::OpenGL),
    });

    static const auto Enum_Temperature = ConfigEnum<TemperatureUnit>({
        ConfigEnumEntry<TemperatureUnit>("CELSIUS", TemperatureUnit::Celsius),
        ConfigEnumEntry<TemperatureUnit>("FAHRENHEIT", TemperatureUnit::Fahrenheit),
    });

    static const auto Enum_Sort = ConfigEnum<Sort>({
        ConfigEnumEntry<Sort>("NAME_ASCENDING", Sort::NameAscending),
        ConfigEnumEntry<Sort>("NAME_DESCENDING", Sort::NameDescending),
        ConfigEnumEntry<Sort>("DATE_ASCENDING", Sort::DateAscending),
        ConfigEnumEntry<Sort>("DATE_DESCENDING", Sort::DateDescending),
    });

    static const auto Enum_VirtualFloorStyle = ConfigEnum<VirtualFloorStyles>({
        ConfigEnumEntry<VirtualFloorStyles>("OFF", VirtualFloorStyles::Off),
        ConfigEnumEntry<VirtualFloorStyles>("CLEAR", VirtualFloorStyles::Clear),
        ConfigEnumEntry<VirtualFloorStyles>("GLASSY", VirtualFloorStyles::Glassy),
    });

    /**
     * Config enum wrapping LanguagesDescriptors.
     */
    static class LanguageConfigEnum final : public IConfigEnum<int32_t>
    {
    public:
        std::string GetName(int32_t value) const override
        {
            return LanguagesDescriptors[value].locale;
        }

        int32_t GetValue(const std::string& key, int32_t defaultValue) const override
        {
            int32_t i = 0;
            for (const auto& langDesc : LanguagesDescriptors)
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

    static void ReadGeneral(IIniReader* reader)
    {
        if (reader->ReadSection("general"))
        {
            auto model = &gConfigGeneral;
            model->always_show_gridlines = reader->GetBoolean("always_show_gridlines", false);
            model->autosave_frequency = reader->GetInt32("autosave", AUTOSAVE_EVERY_5MINUTES);
            model->autosave_amount = reader->GetInt32("autosave_amount", DEFAULT_NUM_AUTOSAVES_TO_KEEP);
            model->confirmation_prompt = reader->GetBoolean("confirmation_prompt", false);
            model->currency_format = reader->GetEnum<CurrencyType>(
                "currency_format", Platform::GetLocaleCurrency(), Enum_Currency);
            model->custom_currency_rate = reader->GetInt32("custom_currency_rate", 10);
            model->custom_currency_affix = reader->GetEnum<CurrencyAffix>(
                "custom_currency_affix", CurrencyAffix::Suffix, Enum_CurrencySymbolAffix);
            model->custom_currency_symbol = reader->GetCString("custom_currency_symbol", "Ctm");
            model->edge_scrolling = reader->GetBoolean("edge_scrolling", true);
            model->edge_scrolling_speed = reader->GetInt32("edge_scrolling_speed", 12);
            model->fullscreen_mode = reader->GetInt32("fullscreen_mode", 0);
            model->fullscreen_height = reader->GetInt32("fullscreen_height", -1);
            model->fullscreen_width = reader->GetInt32("fullscreen_width", -1);
            model->rct1_path = reader->GetString("rct1_path", "");
            model->rct2_path = reader->GetString("game_path", "");
            model->landscape_smoothing = reader->GetBoolean("landscape_smoothing", true);
            model->language = reader->GetEnum<int32_t>("language", Platform::GetLocaleLanguage(), Enum_LanguageEnum);
            model->measurement_format = reader->GetEnum<MeasurementFormat>(
                "measurement_format", Platform::GetLocaleMeasurementFormat(), Enum_MeasurementFormat);
            model->play_intro = reader->GetBoolean("play_intro", false);
            model->save_plugin_data = reader->GetBoolean("save_plugin_data", true);
            model->debugging_tools = reader->GetBoolean("debugging_tools", false);
            model->show_height_as_units = reader->GetBoolean("show_height_as_units", false);
            model->temperature_format = reader->GetEnum<TemperatureUnit>(
                "temperature_format", Platform::GetLocaleTemperatureFormat(), Enum_Temperature);
            model->window_height = reader->GetInt32("window_height", -1);
            model->window_snap_proximity = reader->GetInt32("window_snap_proximity", 5);
            model->window_width = reader->GetInt32("window_width", -1);
            model->default_display = reader->GetInt32("default_display", 0);
            model->drawing_engine = reader->GetEnum<DrawingEngine>(
                "drawing_engine", DrawingEngine::Software, Enum_DrawingEngine);
            model->uncap_fps = reader->GetBoolean("uncap_fps", false);
            model->use_vsync = reader->GetBoolean("use_vsync", true);
            model->virtual_floor_style = reader->GetEnum<VirtualFloorStyles>(
                "virtual_floor_style", VirtualFloorStyles::Glassy, Enum_VirtualFloorStyle);
            model->date_format = reader->GetEnum<int32_t>("date_format", Platform::GetLocaleDateFormat(), Enum_DateFormat);
            model->auto_staff_placement = reader->GetBoolean("auto_staff", true);
            model->handymen_mow_default = reader->GetBoolean("handymen_mow_default", false);
            model->default_inspection_interval = reader->GetInt32("default_inspection_interval", 2);
            model->last_run_version = reader->GetString("last_run_version", "");
            model->invert_viewport_drag = reader->GetBoolean("invert_viewport_drag", false);
            model->load_save_sort = reader->GetEnum<Sort>("load_save_sort", Sort::NameAscending, Enum_Sort);
            model->minimize_fullscreen_focus_loss = reader->GetBoolean("minimize_fullscreen_focus_loss", true);
            model->disable_screensaver = reader->GetBoolean("disable_screensaver", true);

            // Default config setting is false until the games canvas can be separated from the effect
            model->day_night_cycle = reader->GetBoolean("day_night_cycle", false);
            const bool isHardware = model->drawing_engine != DrawingEngine::Software;
            model->enable_light_fx = isHardware && reader->GetBoolean("enable_light_fx", false);
            model->enable_light_fx_for_vehicles = isHardware && reader->GetBoolean("enable_light_fx_for_vehicles", false);
            model->upper_case_banners = reader->GetBoolean("upper_case_banners", false);
            model->disable_lightning_effect = reader->GetBoolean("disable_lightning_effect", false);
            model->allow_loading_with_incorrect_checksum = reader->GetBoolean("allow_loading_with_incorrect_checksum", true);
            model->steam_overlay_pause = reader->GetBoolean("steam_overlay_pause", true);
            model->window_scale = reader->GetFloat("window_scale", Platform::GetDefaultScale());
            model->show_fps = reader->GetBoolean("show_fps", false);
            model->multithreading = reader->GetBoolean("multi_threading", false);
            model->trap_cursor = reader->GetBoolean("trap_cursor", false);
            model->auto_open_shops = reader->GetBoolean("auto_open_shops", false);
            model->scenario_select_mode = reader->GetInt32("scenario_select_mode", SCENARIO_SELECT_MODE_ORIGIN);
            model->scenario_unlocking_enabled = reader->GetBoolean("scenario_unlocking_enabled", true);
            model->scenario_hide_mega_park = reader->GetBoolean("scenario_hide_mega_park", true);
            model->last_save_game_directory = reader->GetString("last_game_directory", "");
            model->last_save_landscape_directory = reader->GetString("last_landscape_directory", "");
            model->last_save_scenario_directory = reader->GetString("last_scenario_directory", "");
            model->last_save_track_directory = reader->GetString("last_track_directory", "");
            model->use_native_browse_dialog = reader->GetBoolean("use_native_browse_dialog", false);
            model->window_limit = reader->GetInt32("window_limit", WINDOW_LIMIT_MAX);
            model->zoom_to_cursor = reader->GetBoolean("zoom_to_cursor", true);
            model->render_weather_effects = reader->GetBoolean("render_weather_effects", true);
            model->render_weather_gloom = reader->GetBoolean("render_weather_gloom", true);
            model->show_guest_purchases = reader->GetBoolean("show_guest_purchases", false);
            model->show_real_names_of_guests = reader->GetBoolean("show_real_names_of_guests", true);
            model->allow_early_completion = reader->GetBoolean("allow_early_completion", false);
            model->transparent_screenshot = reader->GetBoolean("transparent_screenshot", true);
            model->transparent_water = reader->GetBoolean("transparent_water", true);
            model->last_version_check_time = reader->GetInt64("last_version_check_time", 0);
        }
    }

    static void WriteGeneral(IIniWriter* writer)
    {
        auto model = &gConfigGeneral;
        writer->WriteSection("general");
        writer->WriteBoolean("always_show_gridlines", model->always_show_gridlines);
        writer->WriteInt32("autosave", model->autosave_frequency);
        writer->WriteInt32("autosave_amount", model->autosave_amount);
        writer->WriteBoolean("confirmation_prompt", model->confirmation_prompt);
        writer->WriteEnum<CurrencyType>("currency_format", model->currency_format, Enum_Currency);
        writer->WriteInt32("custom_currency_rate", model->custom_currency_rate);
        writer->WriteEnum<CurrencyAffix>("custom_currency_affix", model->custom_currency_affix, Enum_CurrencySymbolAffix);
        writer->WriteString("custom_currency_symbol", model->custom_currency_symbol);
        writer->WriteBoolean("edge_scrolling", model->edge_scrolling);
        writer->WriteInt32("edge_scrolling_speed", model->edge_scrolling_speed);
        writer->WriteInt32("fullscreen_mode", model->fullscreen_mode);
        writer->WriteInt32("fullscreen_height", model->fullscreen_height);
        writer->WriteInt32("fullscreen_width", model->fullscreen_width);
        writer->WriteString("rct1_path", model->rct1_path);
        writer->WriteString("game_path", model->rct2_path);
        writer->WriteBoolean("landscape_smoothing", model->landscape_smoothing);
        writer->WriteEnum<int32_t>("language", model->language, Enum_LanguageEnum);
        writer->WriteEnum<MeasurementFormat>("measurement_format", model->measurement_format, Enum_MeasurementFormat);
        writer->WriteBoolean("play_intro", model->play_intro);
        writer->WriteBoolean("save_plugin_data", model->save_plugin_data);
        writer->WriteBoolean("debugging_tools", model->debugging_tools);
        writer->WriteBoolean("show_height_as_units", model->show_height_as_units);
        writer->WriteEnum<TemperatureUnit>("temperature_format", model->temperature_format, Enum_Temperature);
        writer->WriteInt32("window_height", model->window_height);
        writer->WriteInt32("window_snap_proximity", model->window_snap_proximity);
        writer->WriteInt32("window_width", model->window_width);
        writer->WriteInt32("default_display", model->default_display);
        writer->WriteEnum<DrawingEngine>("drawing_engine", model->drawing_engine, Enum_DrawingEngine);
        writer->WriteBoolean("uncap_fps", model->uncap_fps);
        writer->WriteBoolean("use_vsync", model->use_vsync);
        writer->WriteEnum<int32_t>("date_format", model->date_format, Enum_DateFormat);
        writer->WriteBoolean("auto_staff", model->auto_staff_placement);
        writer->WriteBoolean("handymen_mow_default", model->handymen_mow_default);
        writer->WriteInt32("default_inspection_interval", model->default_inspection_interval);
        writer->WriteString("last_run_version", model->last_run_version);
        writer->WriteBoolean("invert_viewport_drag", model->invert_viewport_drag);
        writer->WriteEnum<Sort>("load_save_sort", model->load_save_sort, Enum_Sort);
        writer->WriteBoolean("minimize_fullscreen_focus_loss", model->minimize_fullscreen_focus_loss);
        writer->WriteBoolean("disable_screensaver", model->disable_screensaver);
        writer->WriteBoolean("day_night_cycle", model->day_night_cycle);
        writer->WriteBoolean("enable_light_fx", model->enable_light_fx);
        writer->WriteBoolean("enable_light_fx_for_vehicles", model->enable_light_fx_for_vehicles);
        writer->WriteBoolean("upper_case_banners", model->upper_case_banners);
        writer->WriteBoolean("disable_lightning_effect", model->disable_lightning_effect);
        writer->WriteBoolean("allow_loading_with_incorrect_checksum", model->allow_loading_with_incorrect_checksum);
        writer->WriteBoolean("steam_overlay_pause", model->steam_overlay_pause);
        writer->WriteFloat("window_scale", model->window_scale);
        writer->WriteBoolean("show_fps", model->show_fps);
        writer->WriteBoolean("multi_threading", model->multithreading);
        writer->WriteBoolean("trap_cursor", model->trap_cursor);
        writer->WriteBoolean("auto_open_shops", model->auto_open_shops);
        writer->WriteInt32("scenario_select_mode", model->scenario_select_mode);
        writer->WriteBoolean("scenario_unlocking_enabled", model->scenario_unlocking_enabled);
        writer->WriteBoolean("scenario_hide_mega_park", model->scenario_hide_mega_park);
        writer->WriteString("last_game_directory", model->last_save_game_directory);
        writer->WriteString("last_landscape_directory", model->last_save_landscape_directory);
        writer->WriteString("last_scenario_directory", model->last_save_scenario_directory);
        writer->WriteString("last_track_directory", model->last_save_track_directory);
        writer->WriteBoolean("use_native_browse_dialog", model->use_native_browse_dialog);
        writer->WriteInt32("window_limit", model->window_limit);
        writer->WriteBoolean("zoom_to_cursor", model->zoom_to_cursor);
        writer->WriteBoolean("render_weather_effects", model->render_weather_effects);
        writer->WriteBoolean("render_weather_gloom", model->render_weather_gloom);
        writer->WriteBoolean("show_guest_purchases", model->show_guest_purchases);
        writer->WriteBoolean("show_real_names_of_guests", model->show_real_names_of_guests);
        writer->WriteBoolean("allow_early_completion", model->allow_early_completion);
        writer->WriteEnum<VirtualFloorStyles>("virtual_floor_style", model->virtual_floor_style, Enum_VirtualFloorStyle);
        writer->WriteBoolean("transparent_screenshot", model->transparent_screenshot);
        writer->WriteBoolean("transparent_water", model->transparent_water);
        writer->WriteInt64("last_version_check_time", model->last_version_check_time);
    }

    static void ReadInterface(IIniReader* reader)
    {
        if (reader->ReadSection("interface"))
        {
            auto model = &gConfigInterface;
            model->toolbar_show_finances = reader->GetBoolean("toolbar_show_finances", true);
            model->toolbar_show_research = reader->GetBoolean("toolbar_show_research", true);
            model->toolbar_show_cheats = reader->GetBoolean("toolbar_show_cheats", false);
            model->toolbar_show_news = reader->GetBoolean("toolbar_show_news", false);
            model->toolbar_show_mute = reader->GetBoolean("toolbar_show_mute", false);
            model->toolbar_show_chat = reader->GetBoolean("toolbar_show_chat", false);
            model->toolbar_show_zoom = reader->GetBoolean("toolbar_show_zoom", true);
            model->console_small_font = reader->GetBoolean("console_small_font", false);
            model->current_theme_preset = reader->GetCString("current_theme", "*RCT2");
            model->current_title_sequence_preset = reader->GetCString("current_title_sequence", "*OPENRCT2");
            model->random_title_sequence = reader->GetBoolean("random_title_sequence", false);
            model->object_selection_filter_flags = reader->GetInt32("object_selection_filter_flags", 0x3FFF);
            model->scenarioselect_last_tab = reader->GetInt32("scenarioselect_last_tab", 0);
        }
    }

    static void WriteInterface(IIniWriter* writer)
    {
        auto model = &gConfigInterface;
        writer->WriteSection("interface");
        writer->WriteBoolean("toolbar_show_finances", model->toolbar_show_finances);
        writer->WriteBoolean("toolbar_show_research", model->toolbar_show_research);
        writer->WriteBoolean("toolbar_show_cheats", model->toolbar_show_cheats);
        writer->WriteBoolean("toolbar_show_news", model->toolbar_show_news);
        writer->WriteBoolean("toolbar_show_mute", model->toolbar_show_mute);
        writer->WriteBoolean("toolbar_show_chat", model->toolbar_show_chat);
        writer->WriteBoolean("toolbar_show_zoom", model->toolbar_show_zoom);
        writer->WriteBoolean("console_small_font", model->console_small_font);
        writer->WriteString("current_theme", model->current_theme_preset);
        writer->WriteString("current_title_sequence", model->current_title_sequence_preset);
        writer->WriteBoolean("random_title_sequence", model->random_title_sequence);
        writer->WriteInt32("object_selection_filter_flags", model->object_selection_filter_flags);
        writer->WriteInt32("scenarioselect_last_tab", model->scenarioselect_last_tab);
    }

    static void ReadSound(IIniReader* reader)
    {
        if (reader->ReadSection("sound"))
        {
            auto model = &gConfigSound;
            model->device = reader->GetCString("audio_device", nullptr);
            model->master_sound_enabled = reader->GetBoolean("master_sound", true);
            model->master_volume = reader->GetInt32("master_volume", 100);
            model->title_music = reader->GetInt32("title_music", 2);
            model->sound_enabled = reader->GetBoolean("sound", true);
            model->sound_volume = reader->GetInt32("sound_volume", 100);
            model->ride_music_enabled = reader->GetBoolean("ride_music", true);
            model->ride_music_volume = reader->GetInt32("ride_music_volume", 100);
            model->audio_focus = reader->GetBoolean("audio_focus", false);
        }
    }

    static void WriteSound(IIniWriter* writer)
    {
        auto model = &gConfigSound;
        writer->WriteSection("sound");
        writer->WriteString("audio_device", model->device);
        writer->WriteBoolean("master_sound", model->master_sound_enabled);
        writer->WriteInt32("master_volume", model->master_volume);
        writer->WriteInt32("title_music", model->title_music);
        writer->WriteBoolean("sound", model->sound_enabled);
        writer->WriteInt32("sound_volume", model->sound_volume);
        writer->WriteBoolean("ride_music", model->ride_music_enabled);
        writer->WriteInt32("ride_music_volume", model->ride_music_volume);
        writer->WriteBoolean("audio_focus", model->audio_focus);
    }

    static void ReadNetwork(IIniReader* reader)
    {
        if (reader->ReadSection("network"))
        {
            // If the `player_name` setting is missing or equal to the empty string
            // use the logged-in user's username instead
            auto playerName = reader->GetString("player_name", "");
            if (playerName.empty())
            {
                playerName = Platform::GetUsername();
                if (playerName.empty())
                {
                    playerName = "Player";
                }
            }

            // Trim any whitespace before or after the player's name,
            // to avoid people pretending to be someone else
            playerName = String::Trim(playerName);

            auto model = &gConfigNetwork;
            model->player_name = playerName;
            model->default_port = reader->GetInt32("default_port", NETWORK_DEFAULT_PORT);
            model->listen_address = reader->GetString("listen_address", "");
            model->default_password = reader->GetString("default_password", "");
            model->stay_connected = reader->GetBoolean("stay_connected", true);
            model->advertise = reader->GetBoolean("advertise", true);
            model->advertise_address = reader->GetString("advertise_address", "");
            model->maxplayers = reader->GetInt32("maxplayers", 16);
            model->server_name = reader->GetString("server_name", "Server");
            model->server_description = reader->GetString("server_description", "");
            model->server_greeting = reader->GetString("server_greeting", "");
            model->master_server_url = reader->GetString("master_server_url", "");
            model->provider_name = reader->GetString("provider_name", "");
            model->provider_email = reader->GetString("provider_email", "");
            model->provider_website = reader->GetString("provider_website", "");
            model->known_keys_only = reader->GetBoolean("known_keys_only", false);
            model->log_chat = reader->GetBoolean("log_chat", false);
            model->log_server_actions = reader->GetBoolean("log_server_actions", false);
            model->pause_server_if_no_clients = reader->GetBoolean("pause_server_if_no_clients", false);
            model->desync_debugging = reader->GetBoolean("desync_debugging", false);
        }
    }

    static void WriteNetwork(IIniWriter* writer)
    {
        auto model = &gConfigNetwork;
        writer->WriteSection("network");
        writer->WriteString("player_name", model->player_name);
        writer->WriteInt32("default_port", model->default_port);
        writer->WriteString("listen_address", model->listen_address);
        writer->WriteString("default_password", model->default_password);
        writer->WriteBoolean("stay_connected", model->stay_connected);
        writer->WriteBoolean("advertise", model->advertise);
        writer->WriteString("advertise_address", model->advertise_address);
        writer->WriteInt32("maxplayers", model->maxplayers);
        writer->WriteString("server_name", model->server_name);
        writer->WriteString("server_description", model->server_description);
        writer->WriteString("server_greeting", model->server_greeting);
        writer->WriteString("master_server_url", model->master_server_url);
        writer->WriteString("provider_name", model->provider_name);
        writer->WriteString("provider_email", model->provider_email);
        writer->WriteString("provider_website", model->provider_website);
        writer->WriteBoolean("known_keys_only", model->known_keys_only);
        writer->WriteBoolean("log_chat", model->log_chat);
        writer->WriteBoolean("log_server_actions", model->log_server_actions);
        writer->WriteBoolean("pause_server_if_no_clients", model->pause_server_if_no_clients);
        writer->WriteBoolean("desync_debugging", model->desync_debugging);
    }

    static void ReadNotifications(IIniReader* reader)
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
            model->ride_casualties = reader->GetBoolean("ride_casualties", true);
            model->ride_warnings = reader->GetBoolean("ride_warnings", true);
            model->ride_researched = reader->GetBoolean("ride_researched", true);
            model->ride_stalled_vehicles = reader->GetBoolean("ride_stalled_vehicles", true);
            model->guest_warnings = reader->GetBoolean("guest_warnings", true);
            model->guest_left_park = reader->GetBoolean("guest_left_park", true);
            model->guest_queuing_for_ride = reader->GetBoolean("guest_queuing_for_ride", true);
            model->guest_on_ride = reader->GetBoolean("guest_on_ride", true);
            model->guest_left_ride = reader->GetBoolean("guest_left_ride", true);
            model->guest_bought_item = reader->GetBoolean("guest_bought_item", true);
            model->guest_used_facility = reader->GetBoolean("guest_used_facility", true);
            model->guest_died = reader->GetBoolean("guest_died", true);
        }
    }

    static void WriteNotifications(IIniWriter* writer)
    {
        auto model = &gConfigNotifications;
        writer->WriteSection("notifications");
        writer->WriteBoolean("park_award", model->park_award);
        writer->WriteBoolean("park_marketing_campaign_finished", model->park_marketing_campaign_finished);
        writer->WriteBoolean("park_warnings", model->park_warnings);
        writer->WriteBoolean("park_rating_warnings", model->park_rating_warnings);
        writer->WriteBoolean("ride_broken_down", model->ride_broken_down);
        writer->WriteBoolean("ride_crashed", model->ride_crashed);
        writer->WriteBoolean("ride_casualties", model->ride_casualties);
        writer->WriteBoolean("ride_warnings", model->ride_warnings);
        writer->WriteBoolean("ride_researched", model->ride_researched);
        writer->WriteBoolean("ride_stalled_vehicles", model->ride_stalled_vehicles);
        writer->WriteBoolean("guest_warnings", model->guest_warnings);
        writer->WriteBoolean("guest_left_park", model->guest_left_park);
        writer->WriteBoolean("guest_queuing_for_ride", model->guest_queuing_for_ride);
        writer->WriteBoolean("guest_on_ride", model->guest_on_ride);
        writer->WriteBoolean("guest_left_ride", model->guest_left_ride);
        writer->WriteBoolean("guest_bought_item", model->guest_bought_item);
        writer->WriteBoolean("guest_used_facility", model->guest_used_facility);
        writer->WriteBoolean("guest_died", model->guest_died);
    }

    static void ReadFont(IIniReader* reader)
    {
        if (reader->ReadSection("font"))
        {
            auto model = &gConfigFonts;
            model->file_name = reader->GetCString("file_name", nullptr);
            model->font_name = reader->GetCString("font_name", nullptr);
            model->x_offset = reader->GetInt32("x_offset", false);
            model->y_offset = reader->GetInt32("y_offset", true);
            model->size_tiny = reader->GetInt32("size_tiny", true);
            model->size_small = reader->GetInt32("size_small", false);
            model->size_medium = reader->GetInt32("size_medium", false);
            model->size_big = reader->GetInt32("size_big", false);
            model->height_tiny = reader->GetInt32("height_tiny", false);
            model->height_small = reader->GetInt32("height_small", false);
            model->height_medium = reader->GetInt32("height_medium", false);
            model->height_big = reader->GetInt32("height_big", false);
            model->enable_hinting = reader->GetBoolean("enable_hinting", true);
            model->hinting_threshold = reader->GetInt32("hinting_threshold", false);
        }
    }

    static void WriteFont(IIniWriter* writer)
    {
        auto model = &gConfigFonts;
        writer->WriteSection("font");
        writer->WriteString("file_name", model->file_name);
        writer->WriteString("font_name", model->font_name);
        writer->WriteInt32("x_offset", model->x_offset);
        writer->WriteInt32("y_offset", model->y_offset);
        writer->WriteInt32("size_tiny", model->size_tiny);
        writer->WriteInt32("size_small", model->size_small);
        writer->WriteInt32("size_medium", model->size_medium);
        writer->WriteInt32("size_big", model->size_big);
        writer->WriteInt32("height_tiny", model->height_tiny);
        writer->WriteInt32("height_small", model->height_small);
        writer->WriteInt32("height_medium", model->height_medium);
        writer->WriteInt32("height_big", model->height_big);
        writer->WriteBoolean("enable_hinting", model->enable_hinting);
        writer->WriteInt32("hinting_threshold", model->hinting_threshold);
    }

    static void ReadPlugin(IIniReader* reader)
    {
        if (reader->ReadSection("plugin"))
        {
            auto model = &gConfigPlugin;
            model->enable_hot_reloading = reader->GetBoolean("enable_hot_reloading", false);
            model->allowed_hosts = reader->GetString("allowed_hosts", "");
        }
    }

    static void WritePlugin(IIniWriter* writer)
    {
        auto model = &gConfigPlugin;
        writer->WriteSection("plugin");
        writer->WriteBoolean("enable_hot_reloading", model->enable_hot_reloading);
        writer->WriteString("allowed_hosts", model->allowed_hosts);
    }

    static bool SetDefaults()
    {
        try
        {
            auto reader = CreateDefaultIniReader();
            ReadGeneral(reader.get());
            ReadInterface(reader.get());
            ReadSound(reader.get());
            ReadNetwork(reader.get());
            ReadNotifications(reader.get());
            ReadFont(reader.get());
            ReadPlugin(reader.get());
            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

    static bool ReadFile(u8string_view path)
    {
        try
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            auto reader = CreateIniReader(&fs);
            ReadGeneral(reader.get());
            ReadInterface(reader.get());
            ReadSound(reader.get());
            ReadNetwork(reader.get());
            ReadNotifications(reader.get());
            ReadFont(reader.get());
            ReadPlugin(reader.get());
            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

    static bool WriteFile(u8string_view path)
    {
        try
        {
            auto directory = Path::GetDirectory(path);
            Path::CreateDirectory(directory);

            auto fs = FileStream(path, FILE_MODE_WRITE);
            auto writer = CreateIniWriter(&fs);
            WriteGeneral(writer.get());
            WriteInterface(writer.get());
            WriteSound(writer.get());
            WriteNetwork(writer.get());
            WriteNotifications(writer.get());
            WriteFont(writer.get());
            WritePlugin(writer.get());
            return true;
        }
        catch (const std::exception& ex)
        {
            Console::WriteLine("Error saving to '%s'", u8string(path).c_str());
            Console::WriteLine(ex.what());
            return false;
        }
    }

    /**
     * Attempts to find the RCT1 installation directory.
     * @returns Path to RCT1, if found. Empty string otherwise.
     */
    static u8string FindRCT1Path()
    {
        log_verbose("config_find_rct1_path(...)");

        static constexpr u8string_view searchLocations[] = {
            R"(C:\Program Files\Steam\steamapps\common\Rollercoaster Tycoon Deluxe)",
            R"(C:\Program Files (x86)\Steam\steamapps\common\Rollercoaster Tycoon Deluxe)",
            R"(C:\GOG Games\RollerCoaster Tycoon Deluxe)",
            R"(C:\Program Files\GalaxyClient\Games\RollerCoaster Tycoon Deluxe)",
            R"(C:\Program Files (x86)\GalaxyClient\Games\RollerCoaster Tycoon Deluxe)",
            R"(C:\Program Files\Hasbro Interactive\RollerCoaster Tycoon)",
            R"(C:\Program Files (x86)\Hasbro Interactive\RollerCoaster Tycoon)",
        };

        for (const auto& location : searchLocations)
        {
            if (RCT1DataPresentAtLocation(location))
            {
                return u8string(location);
            }
        }

        auto steamPath = Platform::GetSteamPath();
        if (!steamPath.empty())
        {
            std::string location = Path::Combine(steamPath, Platform::GetRCT1SteamDir());
            if (RCT1DataPresentAtLocation(location))
            {
                return location;
            }
        }

        auto exePath = Path::GetDirectory(Platform::GetCurrentExecutablePath());
        if (RCT1DataPresentAtLocation(exePath))
        {
            return exePath;
        }
        return {};
    }

    /**
     * Attempts to find the RCT2 installation directory.
     * This should be created from some other resource when OpenRCT2 grows.
     * @returns Path to RCT2, if found. Empty string otherwise.
     */
    static u8string FindRCT2Path()
    {
        log_verbose("config_find_rct2_path(...)");

        static constexpr u8string_view searchLocations[] = {
            R"(C:\Program Files\Steam\steamapps\common\Rollercoaster Tycoon 2)",
            R"(C:\Program Files (x86)\Steam\steamapps\common\Rollercoaster Tycoon 2)",
            R"(C:\GOG Games\RollerCoaster Tycoon 2 Triple Thrill Pack)",
            R"(C:\Program Files\GalaxyClient\Games\RollerCoaster Tycoon 2 Triple Thrill Pack)",
            R"(C:\Program Files (x86)\GalaxyClient\Games\RollerCoaster Tycoon 2 Triple Thrill Pack)",
            R"(C:\Program Files\Atari\RollerCoaster Tycoon 2)",
            R"(C:\Program Files (x86)\Atari\RollerCoaster Tycoon 2)",
            R"(C:\Program Files\Infogrames\RollerCoaster Tycoon 2)",
            R"(C:\Program Files (x86)\Infogrames\RollerCoaster Tycoon 2)",
            R"(C:\Program Files\Infogrames Interactive\RollerCoaster Tycoon 2)",
            R"(C:\Program Files (x86)\Infogrames Interactive\RollerCoaster Tycoon 2)",
        };

        for (const auto& location : searchLocations)
        {
            if (Platform::OriginalGameDataExists(location))
            {
                return u8string(location);
            }
        }

        auto steamPath = Platform::GetSteamPath();
        if (!steamPath.empty())
        {
            std::string location = Path::Combine(steamPath, Platform::GetRCT2SteamDir());
            if (Platform::OriginalGameDataExists(location))
            {
                return location;
            }
        }

        auto discordPath = Platform::GetFolderPath(SPECIAL_FOLDER::RCT2_DISCORD);
        if (!discordPath.empty() && Platform::OriginalGameDataExists(discordPath))
        {
            return discordPath;
        }

        auto exePath = Path::GetDirectory(Platform::GetCurrentExecutablePath());
        if (Platform::OriginalGameDataExists(exePath))
        {
            return exePath;
        }
        return {};
    }

    static bool SelectGogInstaller(utf8* installerPath)
    {
        FileDialogDesc desc{};
        desc.Type = FileDialogType::Open;
        desc.Title = language_get_string(STR_SELECT_GOG_INSTALLER);
        desc.Filters.emplace_back(language_get_string(STR_GOG_INSTALLER), "*.exe");
        desc.Filters.emplace_back(language_get_string(STR_ALL_FILES), "*");

        const auto userHomePath = Platform::GetFolderPath(SPECIAL_FOLDER::USER_HOME);
        desc.InitialDirectory = userHomePath.c_str();

        return ContextOpenCommonFileDialog(installerPath, desc, 4096);
    }

    static bool ExtractGogInstaller(u8string_view installerPath, u8string_view targetPath)
    {
        std::string path;
        std::string output;

        if (!Platform::FindApp("innoextract", &path))
        {
            log_error("Please install innoextract to extract files from GOG.");
            return false;
        }
        int32_t exit_status = Platform::Execute(
            String::Format("%s '%s' --exclude-temp --output-dir '%s'", path.c_str(), installerPath, targetPath), &output);
        log_info("Exit status %d", exit_status);
        return exit_status == 0;
    }
} // namespace Config

GeneralConfiguration gConfigGeneral;
InterfaceConfiguration gConfigInterface;
SoundConfiguration gConfigSound;
NetworkConfiguration gConfigNetwork;
NotificationConfiguration gConfigNotifications;
FontConfiguration gConfigFonts;
PluginConfiguration gConfigPlugin;

void config_set_defaults()
{
    config_release();
    Config::SetDefaults();
}

bool config_open(u8string_view path)
{
    if (!File::Exists(path))
    {
        return false;
    }

    config_release();
    auto result = Config::ReadFile(path);
    if (result)
    {
        currency_load_custom_currency_config();
    }
    return result;
}

bool config_save(u8string_view path)
{
    return Config::WriteFile(path);
}

void config_release()
{
    SafeFree(gConfigGeneral.custom_currency_symbol);
    SafeFree(gConfigInterface.current_theme_preset);
    SafeFree(gConfigInterface.current_title_sequence_preset);
    SafeFree(gConfigSound.device);
    SafeFree(gConfigFonts.file_name);
    SafeFree(gConfigFonts.font_name);
}

u8string config_get_default_path()
{
    auto env = GetContext()->GetPlatformEnvironment();
    return Path::Combine(env->GetDirectoryPath(DIRBASE::USER), u8"config.ini");
}

bool config_save_default()
{
    auto path = config_get_default_path();
    return config_save(path);
}

bool config_find_or_browse_install_directory()
{
    std::string path = Config::FindRCT2Path();
    if (!path.empty())
    {
        gConfigGeneral.rct2_path = path;
    }
    else
    {
        if (gOpenRCT2Headless)
        {
            return false;
        }

        auto uiContext = GetContext()->GetUiContext();
        if (!uiContext->HasFilePicker())
        {
            uiContext->ShowMessageBox(format_string(STR_NEEDS_RCT2_FILES_MANUAL, nullptr));
            return false;
        }

        try
        {
            const char* g1DatPath = PATH_SEPARATOR "Data" PATH_SEPARATOR "g1.dat";
            while (true)
            {
                uiContext->ShowMessageBox(format_string(STR_NEEDS_RCT2_FILES, nullptr));
                std::string gog = language_get_string(STR_OWN_ON_GOG);
                std::string hdd = language_get_string(STR_INSTALLED_ON_HDD);

                std::vector<std::string> options;
                std::string chosenOption;

                if (uiContext->HasMenuSupport())
                {
                    options.push_back(hdd);
                    options.push_back(gog);
                    int optionIndex = uiContext->ShowMenuDialog(
                        options, language_get_string(STR_OPENRCT2_SETUP), language_get_string(STR_WHICH_APPLIES_BEST));
                    if (optionIndex < 0 || static_cast<uint32_t>(optionIndex) >= options.size())
                    {
                        // graceful fallback if app errors or user exits out of window
                        chosenOption = hdd;
                    }
                    else
                    {
                        chosenOption = options[optionIndex];
                    }
                }
                else
                {
                    chosenOption = hdd;
                }

                std::string installPath;
                if (chosenOption == hdd)
                {
                    installPath = uiContext->ShowDirectoryDialog(language_get_string(STR_PICK_RCT2_DIR));
                }
                else if (chosenOption == gog)
                {
                    // Check if innoextract is installed. If not, prompt the user to install it.
                    std::string dummy;
                    if (!Platform::FindApp("innoextract", &dummy))
                    {
                        uiContext->ShowMessageBox(format_string(STR_INSTALL_INNOEXTRACT, nullptr));
                        return false;
                    }

                    const std::string dest = Path::Combine(
                        GetContext()->GetPlatformEnvironment()->GetDirectoryPath(DIRBASE::CONFIG), "rct2");

                    while (true)
                    {
                        uiContext->ShowMessageBox(language_get_string(STR_PLEASE_SELECT_GOG_INSTALLER));
                        utf8 gogPath[4096];
                        if (!Config::SelectGogInstaller(gogPath))
                        {
                            // The user clicked "Cancel", so stop trying.
                            return false;
                        }

                        uiContext->ShowMessageBox(language_get_string(STR_THIS_WILL_TAKE_A_FEW_MINUTES));

                        if (Config::ExtractGogInstaller(gogPath, dest))
                            break;

                        uiContext->ShowMessageBox(language_get_string(STR_NOT_THE_GOG_INSTALLER));
                    }

                    installPath = Path::Combine(dest, u8"app");
                }
                if (installPath.empty())
                {
                    return false;
                }
                gConfigGeneral.rct2_path = installPath;

                if (Platform::OriginalGameDataExists(installPath))
                {
                    return true;
                }

                uiContext->ShowMessageBox(format_string(STR_COULD_NOT_FIND_AT_PATH, &g1DatPath));
            }
        }
        catch (const std::exception& ex)
        {
            Console::Error::WriteLine(ex.what());
            return false;
        }
    }
    // While we're at it, also check if the player has RCT1
    std::string rct1Path = Config::FindRCT1Path();
    if (!rct1Path.empty())
    {
        gConfigGeneral.rct1_path = std::move(rct1Path);
    }

    return true;
}

std::string FindCsg1datAtLocation(u8string_view path)
{
    auto checkPath1 = Path::Combine(path, u8"Data", u8"CSG1.DAT");
    auto checkPath2 = Path::Combine(path, u8"Data", u8"CSG1.1");

    // Since Linux is case sensitive (and macOS sometimes too), make sure we handle case properly.
    std::string path1result = Path::ResolveCasing(checkPath1);
    if (!path1result.empty())
    {
        return path1result;
    }

    std::string path2result = Path::ResolveCasing(checkPath2);
    return path2result;
}

bool Csg1datPresentAtLocation(u8string_view path)
{
    auto location = FindCsg1datAtLocation(path);
    return !location.empty();
}

u8string FindCsg1idatAtLocation(u8string_view path)
{
    auto result1 = Path::ResolveCasing(Path::Combine(path, u8"Data", u8"CSG1I.DAT"));
    if (!result1.empty())
    {
        return result1;
    }
    auto result2 = Path::ResolveCasing(Path::Combine(path, u8"RCTdeluxe_install", u8"Data", u8"CSG1I.DAT"));
    return result2;
}

bool Csg1idatPresentAtLocation(u8string_view path)
{
    std::string location = FindCsg1idatAtLocation(path);
    return !location.empty();
}

bool RCT1DataPresentAtLocation(u8string_view path)
{
    return Csg1datPresentAtLocation(path) && Csg1idatPresentAtLocation(path) && CsgAtLocationIsUsable(path);
}

bool CsgIsUsable(const rct_gx& csg)
{
    return csg.header.total_size == RCT1::Limits::LL_CSG1_DAT_FileSize
        && csg.header.num_entries == RCT1::Limits::Num_LL_CSG_Entries;
}

bool CsgAtLocationIsUsable(u8string_view path)
{
    auto csg1HeaderPath = FindCsg1idatAtLocation(path);
    if (csg1HeaderPath.empty())
    {
        return false;
    }

    auto csg1DataPath = FindCsg1datAtLocation(path);
    if (csg1DataPath.empty())
    {
        return false;
    }

    auto fileHeader = FileStream(csg1HeaderPath, FILE_MODE_OPEN);
    auto fileData = FileStream(csg1DataPath, FILE_MODE_OPEN);
    size_t fileHeaderSize = fileHeader.GetLength();
    size_t fileDataSize = fileData.GetLength();

    rct_gx csg = {};
    csg.header.num_entries = static_cast<uint32_t>(fileHeaderSize / sizeof(rct_g1_element_32bit));
    csg.header.total_size = static_cast<uint32_t>(fileDataSize);
    return CsgIsUsable(csg);
}
