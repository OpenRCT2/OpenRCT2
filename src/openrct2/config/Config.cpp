/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
            model->AlwaysShowGridlines = reader->GetBoolean("always_show_gridlines", false);
            model->AutosaveFrequency = reader->GetInt32("autosave", AUTOSAVE_EVERY_5MINUTES);
            model->AutosaveAmount = reader->GetInt32("autosave_amount", DEFAULT_NUM_AUTOSAVES_TO_KEEP);
            model->ConfirmationPrompt = reader->GetBoolean("confirmation_prompt", false);
            model->CurrencyFormat = reader->GetEnum<CurrencyType>(
                "currency_format", Platform::GetLocaleCurrency(), Enum_Currency);
            model->CustomCurrencyRate = reader->GetInt32("custom_currency_rate", 10);
            model->CustomCurrencyAffix = reader->GetEnum<CurrencyAffix>(
                "custom_currency_affix", CurrencyAffix::Suffix, Enum_CurrencySymbolAffix);
            model->CustomCurrencySymbol = reader->GetString("custom_currency_symbol", "Ctm");
            model->EdgeScrolling = reader->GetBoolean("edge_scrolling", true);
            model->EdgeScrollingSpeed = reader->GetInt32("edge_scrolling_speed", 12);
            model->FullscreenMode = reader->GetInt32("fullscreen_mode", 0);
            model->FullscreenHeight = reader->GetInt32("fullscreen_height", -1);
            model->FullscreenWidth = reader->GetInt32("fullscreen_width", -1);
            model->RCT1Path = reader->GetString("rct1_path", "");
            model->RCT2Path = reader->GetString("game_path", "");
            model->LandscapeSmoothing = reader->GetBoolean("landscape_smoothing", true);
            model->Language = reader->GetEnum<int32_t>("language", Platform::GetLocaleLanguage(), Enum_LanguageEnum);
            model->MeasurementFormat = reader->GetEnum<MeasurementFormat>(
                "measurement_format", Platform::GetLocaleMeasurementFormat(), Enum_MeasurementFormat);
            model->PlayIntro = reader->GetBoolean("play_intro", false);
            model->SavePluginData = reader->GetBoolean("save_plugin_data", true);
            model->DebuggingTools = reader->GetBoolean("debugging_tools", false);
            model->ShowHeightAsUnits = reader->GetBoolean("show_height_as_units", false);
            model->TemperatureFormat = reader->GetEnum<TemperatureUnit>(
                "temperature_format", Platform::GetLocaleTemperatureFormat(), Enum_Temperature);
            model->WindowHeight = reader->GetInt32("window_height", -1);
            model->WindowSnapProximity = reader->GetInt32("window_snap_proximity", 5);
            model->WindowWidth = reader->GetInt32("window_width", -1);
            model->DefaultDisplay = reader->GetInt32("default_display", 0);
            model->DrawingEngine = reader->GetEnum<DrawingEngine>(
                "drawing_engine", DrawingEngine::Software, Enum_DrawingEngine);
            model->UncapFPS = reader->GetBoolean("uncap_fps", false);
            model->UseVSync = reader->GetBoolean("use_vsync", true);
            model->VirtualFloorStyle = reader->GetEnum<VirtualFloorStyles>(
                "virtual_floor_style", VirtualFloorStyles::Glassy, Enum_VirtualFloorStyle);
            model->DateFormat = reader->GetEnum<int32_t>("date_format", Platform::GetLocaleDateFormat(), Enum_DateFormat);
            model->AutoStaffPlacement = reader->GetBoolean("auto_staff", true);
            model->HandymenMowByDefault = reader->GetBoolean("handymen_mow_default", false);
            model->DefaultInspectionInterval = reader->GetInt32("default_inspection_interval", 2);
            model->LastRunVersion = reader->GetString("last_run_version", "");
            model->InvertViewportDrag = reader->GetBoolean("invert_viewport_drag", false);
            model->LoadSaveSort = reader->GetEnum<Sort>("load_save_sort", Sort::NameAscending, Enum_Sort);
            model->MinimizeFullscreenFocusLoss = reader->GetBoolean("minimize_fullscreen_focus_loss", true);
            model->DisableScreensaver = reader->GetBoolean("disable_screensaver", true);

            // Default config setting is false until the games canvas can be separated from the effect
            model->DayNightCycle = reader->GetBoolean("day_night_cycle", false);
            const bool isHardware = model->DrawingEngine != DrawingEngine::Software;
            model->EnableLightFx = isHardware && reader->GetBoolean("enable_light_fx", false);
            model->EnableLightFxForVehicles = isHardware && reader->GetBoolean("enable_light_fx_for_vehicles", false);
            model->UpperCaseBanners = reader->GetBoolean("upper_case_banners", false);
            model->DisableLightningEffect = reader->GetBoolean("disable_lightning_effect", false);
            model->SteamOverlayPause = reader->GetBoolean("steam_overlay_pause", true);
            model->WindowScale = reader->GetFloat("window_scale", Platform::GetDefaultScale());
            model->ShowFPS = reader->GetBoolean("show_fps", false);
            model->MultiThreading = reader->GetBoolean("multi_threading", false);
            model->TrapCursor = reader->GetBoolean("trap_cursor", false);
            model->AutoOpenShops = reader->GetBoolean("auto_open_shops", false);
            model->ShowOnlyBaseGameColours = reader->GetBoolean("show_only_base_game_colours", false);
            model->ScenarioSelectMode = reader->GetInt32("scenario_select_mode", SCENARIO_SELECT_MODE_ORIGIN);
            model->ScenarioUnlockingEnabled = reader->GetBoolean("scenario_unlocking_enabled", true);
            model->ScenarioHideMegaPark = reader->GetBoolean("scenario_hide_mega_park", true);
            model->LastSaveGameDirectory = reader->GetString("last_game_directory", "");
            model->LastSaveLandscapeDirectory = reader->GetString("last_landscape_directory", "");
            model->LastSaveScenarioDirectory = reader->GetString("last_scenario_directory", "");
            model->LastSaveTrackDirectory = reader->GetString("last_track_directory", "");
            model->UseNativeBrowseDialog = reader->GetBoolean("use_native_browse_dialog", false);
            model->WindowLimit = reader->GetInt32("window_limit", WINDOW_LIMIT_MAX);
            model->ZoomToCursor = reader->GetBoolean("zoom_to_cursor", true);
            model->RenderWeatherEffects = reader->GetBoolean("render_weather_effects", true);
            model->RenderWeatherGloom = reader->GetBoolean("render_weather_gloom", true);
            model->ShowGuestPurchases = reader->GetBoolean("show_guest_purchases", false);
            model->ShowRealNamesOfGuests = reader->GetBoolean("show_real_names_of_guests", true);
            model->AllowEarlyCompletion = reader->GetBoolean("allow_early_completion", false);
            model->AssetPackOrder = reader->GetString("asset_pack_order", "");
            model->EnabledAssetPacks = reader->GetString("enabled_asset_packs", "");
            model->TransparentScreenshot = reader->GetBoolean("transparent_screenshot", true);
            model->TransparentWater = reader->GetBoolean("transparent_water", true);

            model->InvisibleRides = reader->GetBoolean("invisible_rides", false);
            model->InvisibleVehicles = reader->GetBoolean("invisible_vehicles", false);
            model->InvisibleTrees = reader->GetBoolean("invisible_trees", false);
            model->InvisibleScenery = reader->GetBoolean("invisible_scenery", false);
            model->InvisiblePaths = reader->GetBoolean("invisible_paths", false);
            model->InvisibleSupports = reader->GetBoolean("invisible_supports", true);

            model->LastVersionCheckTime = reader->GetInt64("last_version_check_time", 0);
        }
    }

    static void WriteGeneral(IIniWriter* writer)
    {
        auto model = &gConfigGeneral;
        writer->WriteSection("general");
        writer->WriteBoolean("always_show_gridlines", model->AlwaysShowGridlines);
        writer->WriteInt32("autosave", model->AutosaveFrequency);
        writer->WriteInt32("autosave_amount", model->AutosaveAmount);
        writer->WriteBoolean("confirmation_prompt", model->ConfirmationPrompt);
        writer->WriteEnum<CurrencyType>("currency_format", model->CurrencyFormat, Enum_Currency);
        writer->WriteInt32("custom_currency_rate", model->CustomCurrencyRate);
        writer->WriteEnum<CurrencyAffix>("custom_currency_affix", model->CustomCurrencyAffix, Enum_CurrencySymbolAffix);
        writer->WriteString("custom_currency_symbol", model->CustomCurrencySymbol);
        writer->WriteBoolean("edge_scrolling", model->EdgeScrolling);
        writer->WriteInt32("edge_scrolling_speed", model->EdgeScrollingSpeed);
        writer->WriteInt32("fullscreen_mode", model->FullscreenMode);
        writer->WriteInt32("fullscreen_height", model->FullscreenHeight);
        writer->WriteInt32("fullscreen_width", model->FullscreenWidth);
        writer->WriteString("rct1_path", model->RCT1Path);
        writer->WriteString("game_path", model->RCT2Path);
        writer->WriteBoolean("landscape_smoothing", model->LandscapeSmoothing);
        writer->WriteEnum<int32_t>("language", model->Language, Enum_LanguageEnum);
        writer->WriteEnum<MeasurementFormat>("measurement_format", model->MeasurementFormat, Enum_MeasurementFormat);
        writer->WriteBoolean("play_intro", model->PlayIntro);
        writer->WriteBoolean("save_plugin_data", model->SavePluginData);
        writer->WriteBoolean("debugging_tools", model->DebuggingTools);
        writer->WriteBoolean("show_height_as_units", model->ShowHeightAsUnits);
        writer->WriteEnum<TemperatureUnit>("temperature_format", model->TemperatureFormat, Enum_Temperature);
        writer->WriteInt32("window_height", model->WindowHeight);
        writer->WriteInt32("window_snap_proximity", model->WindowSnapProximity);
        writer->WriteInt32("window_width", model->WindowWidth);
        writer->WriteInt32("default_display", model->DefaultDisplay);
        writer->WriteEnum<DrawingEngine>("drawing_engine", model->DrawingEngine, Enum_DrawingEngine);
        writer->WriteBoolean("uncap_fps", model->UncapFPS);
        writer->WriteBoolean("use_vsync", model->UseVSync);
        writer->WriteEnum<int32_t>("date_format", model->DateFormat, Enum_DateFormat);
        writer->WriteBoolean("auto_staff", model->AutoStaffPlacement);
        writer->WriteBoolean("handymen_mow_default", model->HandymenMowByDefault);
        writer->WriteInt32("default_inspection_interval", model->DefaultInspectionInterval);
        writer->WriteString("last_run_version", model->LastRunVersion);
        writer->WriteBoolean("invert_viewport_drag", model->InvertViewportDrag);
        writer->WriteEnum<Sort>("load_save_sort", model->LoadSaveSort, Enum_Sort);
        writer->WriteBoolean("minimize_fullscreen_focus_loss", model->MinimizeFullscreenFocusLoss);
        writer->WriteBoolean("disable_screensaver", model->DisableScreensaver);
        writer->WriteBoolean("day_night_cycle", model->DayNightCycle);
        writer->WriteBoolean("enable_light_fx", model->EnableLightFx);
        writer->WriteBoolean("enable_light_fx_for_vehicles", model->EnableLightFxForVehicles);
        writer->WriteBoolean("upper_case_banners", model->UpperCaseBanners);
        writer->WriteBoolean("disable_lightning_effect", model->DisableLightningEffect);
        writer->WriteBoolean("steam_overlay_pause", model->SteamOverlayPause);
        writer->WriteFloat("window_scale", model->WindowScale);
        writer->WriteBoolean("show_fps", model->ShowFPS);
        writer->WriteBoolean("multi_threading", model->MultiThreading);
        writer->WriteBoolean("trap_cursor", model->TrapCursor);
        writer->WriteBoolean("auto_open_shops", model->AutoOpenShops);
        writer->WriteBoolean("show_only_base_game_colours", model->ShowOnlyBaseGameColours);
        writer->WriteInt32("scenario_select_mode", model->ScenarioSelectMode);
        writer->WriteBoolean("scenario_unlocking_enabled", model->ScenarioUnlockingEnabled);
        writer->WriteBoolean("scenario_hide_mega_park", model->ScenarioHideMegaPark);
        writer->WriteString("last_game_directory", model->LastSaveGameDirectory);
        writer->WriteString("last_landscape_directory", model->LastSaveLandscapeDirectory);
        writer->WriteString("last_scenario_directory", model->LastSaveScenarioDirectory);
        writer->WriteString("last_track_directory", model->LastSaveTrackDirectory);
        writer->WriteBoolean("use_native_browse_dialog", model->UseNativeBrowseDialog);
        writer->WriteInt32("window_limit", model->WindowLimit);
        writer->WriteBoolean("zoom_to_cursor", model->ZoomToCursor);
        writer->WriteBoolean("render_weather_effects", model->RenderWeatherEffects);
        writer->WriteBoolean("render_weather_gloom", model->RenderWeatherGloom);
        writer->WriteBoolean("show_guest_purchases", model->ShowGuestPurchases);
        writer->WriteBoolean("show_real_names_of_guests", model->ShowRealNamesOfGuests);
        writer->WriteBoolean("allow_early_completion", model->AllowEarlyCompletion);
        writer->WriteString("asset_pack_order", model->AssetPackOrder);
        writer->WriteString("enabled_asset_packs", model->EnabledAssetPacks);
        writer->WriteEnum<VirtualFloorStyles>("virtual_floor_style", model->VirtualFloorStyle, Enum_VirtualFloorStyle);
        writer->WriteBoolean("transparent_screenshot", model->TransparentScreenshot);
        writer->WriteBoolean("transparent_water", model->TransparentWater);
        writer->WriteBoolean("invisible_rides", model->InvisibleRides);
        writer->WriteBoolean("invisible_vehicles", model->InvisibleVehicles);
        writer->WriteBoolean("invisible_trees", model->InvisibleTrees);
        writer->WriteBoolean("invisible_scenery", model->InvisibleScenery);
        writer->WriteBoolean("invisible_paths", model->InvisiblePaths);
        writer->WriteBoolean("invisible_supports", model->InvisibleSupports);
        writer->WriteInt64("last_version_check_time", model->LastVersionCheckTime);
    }

    static void ReadInterface(IIniReader* reader)
    {
        if (reader->ReadSection("interface"))
        {
            auto model = &gConfigInterface;
            model->ToolbarShowFinances = reader->GetBoolean("toolbar_show_finances", true);
            model->ToolbarShowResearch = reader->GetBoolean("toolbar_show_research", true);
            model->ToolbarShowCheats = reader->GetBoolean("toolbar_show_cheats", false);
            model->ToolbarShowNews = reader->GetBoolean("toolbar_show_news", false);
            model->ToolbarShowMute = reader->GetBoolean("toolbar_show_mute", false);
            model->ToolbarShowChat = reader->GetBoolean("toolbar_show_chat", false);
            model->ToolbarShowZoom = reader->GetBoolean("toolbar_show_zoom", true);
            model->ConsoleSmallFont = reader->GetBoolean("console_small_font", false);
            model->CurrentThemePreset = reader->GetString("current_theme", "*RCT2");
            model->CurrentTitleSequencePreset = reader->GetString("current_title_sequence", "*OPENRCT2");
            model->RandomTitleSequence = reader->GetBoolean("random_title_sequence", false);
            model->ObjectSelectionFilterFlags = reader->GetInt32("object_selection_filter_flags", 0x3FFF);
            model->ScenarioselectLastTab = reader->GetInt32("scenarioselect_last_tab", 0);
            model->ListRideVehiclesSeparately = reader->GetBoolean("list_ride_vehicles_separately", false);
        }
    }

    static void WriteInterface(IIniWriter* writer)
    {
        auto model = &gConfigInterface;
        writer->WriteSection("interface");
        writer->WriteBoolean("toolbar_show_finances", model->ToolbarShowFinances);
        writer->WriteBoolean("toolbar_show_research", model->ToolbarShowResearch);
        writer->WriteBoolean("toolbar_show_cheats", model->ToolbarShowCheats);
        writer->WriteBoolean("toolbar_show_news", model->ToolbarShowNews);
        writer->WriteBoolean("toolbar_show_mute", model->ToolbarShowMute);
        writer->WriteBoolean("toolbar_show_chat", model->ToolbarShowChat);
        writer->WriteBoolean("toolbar_show_zoom", model->ToolbarShowZoom);
        writer->WriteBoolean("console_small_font", model->ConsoleSmallFont);
        writer->WriteString("current_theme", model->CurrentThemePreset);
        writer->WriteString("current_title_sequence", model->CurrentTitleSequencePreset);
        writer->WriteBoolean("random_title_sequence", model->RandomTitleSequence);
        writer->WriteInt32("object_selection_filter_flags", model->ObjectSelectionFilterFlags);
        writer->WriteInt32("scenarioselect_last_tab", model->ScenarioselectLastTab);
        writer->WriteBoolean("list_ride_vehicles_separately", model->ListRideVehiclesSeparately);
    }

    static void ReadSound(IIniReader* reader)
    {
        if (reader->ReadSection("sound"))
        {
            auto model = &gConfigSound;
            model->Device = reader->GetString("audio_device", "");
            model->MasterSoundEnabled = reader->GetBoolean("master_sound", true);
            model->MasterVolume = reader->GetInt32("master_volume", 100);
            model->TitleMusic = static_cast<TitleMusicKind>(reader->GetInt32("title_music", EnumValue(TitleMusicKind::RCT2)));
            model->SoundEnabled = reader->GetBoolean("sound", true);
            model->SoundVolume = reader->GetInt32("sound_volume", 100);
            model->RideMusicEnabled = reader->GetBoolean("ride_music", true);
            model->AudioFocus = reader->GetInt32("ride_music_volume", 100);
            model->audio_focus = reader->GetBoolean("audio_focus", false);
        }
    }

    static void WriteSound(IIniWriter* writer)
    {
        auto model = &gConfigSound;
        writer->WriteSection("sound");
        writer->WriteString("audio_device", model->Device);
        writer->WriteBoolean("master_sound", model->MasterSoundEnabled);
        writer->WriteInt32("master_volume", model->MasterVolume);
        writer->WriteInt32("title_music", EnumValue(model->TitleMusic));
        writer->WriteBoolean("sound", model->SoundEnabled);
        writer->WriteInt32("sound_volume", model->SoundVolume);
        writer->WriteBoolean("ride_music", model->RideMusicEnabled);
        writer->WriteInt32("ride_music_volume", model->AudioFocus);
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
            model->PlayerName = std::move(playerName);
            model->DefaultPort = reader->GetInt32("default_port", NETWORK_DEFAULT_PORT);
            model->ListenAddress = reader->GetString("listen_address", "");
            model->DefaultPassword = reader->GetString("default_password", "");
            model->StayConnected = reader->GetBoolean("stay_connected", true);
            model->Advertise = reader->GetBoolean("advertise", true);
            model->AdvertiseAddress = reader->GetString("advertise_address", "");
            model->Maxplayers = reader->GetInt32("maxplayers", 16);
            model->ServerName = reader->GetString("server_name", "Server");
            model->ServerDescription = reader->GetString("server_description", "");
            model->ServerGreeting = reader->GetString("server_greeting", "");
            model->MasterServerUrl = reader->GetString("master_server_url", "");
            model->ProviderName = reader->GetString("provider_name", "");
            model->ProviderEmail = reader->GetString("provider_email", "");
            model->ProviderWebsite = reader->GetString("provider_website", "");
            model->KnownKeysOnly = reader->GetBoolean("known_keys_only", false);
            model->LogChat = reader->GetBoolean("log_chat", false);
            model->LogServerActions = reader->GetBoolean("log_server_actions", false);
            model->PauseServerIfNoClients = reader->GetBoolean("pause_server_if_no_clients", false);
            model->DesyncDebugging = reader->GetBoolean("desync_debugging", false);
        }
    }

    static void WriteNetwork(IIniWriter* writer)
    {
        auto model = &gConfigNetwork;
        writer->WriteSection("network");
        writer->WriteString("player_name", model->PlayerName);
        writer->WriteInt32("default_port", model->DefaultPort);
        writer->WriteString("listen_address", model->ListenAddress);
        writer->WriteString("default_password", model->DefaultPassword);
        writer->WriteBoolean("stay_connected", model->StayConnected);
        writer->WriteBoolean("advertise", model->Advertise);
        writer->WriteString("advertise_address", model->AdvertiseAddress);
        writer->WriteInt32("maxplayers", model->Maxplayers);
        writer->WriteString("server_name", model->ServerName);
        writer->WriteString("server_description", model->ServerDescription);
        writer->WriteString("server_greeting", model->ServerGreeting);
        writer->WriteString("master_server_url", model->MasterServerUrl);
        writer->WriteString("provider_name", model->ProviderName);
        writer->WriteString("provider_email", model->ProviderEmail);
        writer->WriteString("provider_website", model->ProviderWebsite);
        writer->WriteBoolean("known_keys_only", model->KnownKeysOnly);
        writer->WriteBoolean("log_chat", model->LogChat);
        writer->WriteBoolean("log_server_actions", model->LogServerActions);
        writer->WriteBoolean("pause_server_if_no_clients", model->PauseServerIfNoClients);
        writer->WriteBoolean("desync_debugging", model->DesyncDebugging);
    }

    static void ReadNotifications(IIniReader* reader)
    {
        if (reader->ReadSection("notifications"))
        {
            auto model = &gConfigNotifications;
            model->ParkAward = reader->GetBoolean("park_award", true);
            model->ParkMarketingCampaignFinished = reader->GetBoolean("park_marketing_campaign_finished", true);
            model->ParkWarnings = reader->GetBoolean("park_warnings", true);
            model->ParkRatingWarnings = reader->GetBoolean("park_rating_warnings", true);
            model->RideBrokenDown = reader->GetBoolean("ride_broken_down", true);
            model->RideCrashed = reader->GetBoolean("ride_crashed", true);
            model->RideCasualties = reader->GetBoolean("ride_casualties", true);
            model->RideWarnings = reader->GetBoolean("ride_warnings", true);
            model->RideResearched = reader->GetBoolean("ride_researched", true);
            model->RideStalledVehicles = reader->GetBoolean("ride_stalled_vehicles", true);
            model->GuestWarnings = reader->GetBoolean("guest_warnings", true);
            model->GuestLeftPark = reader->GetBoolean("guest_left_park", true);
            model->GuestQueuingForRide = reader->GetBoolean("guest_queuing_for_ride", true);
            model->GuestOnRide = reader->GetBoolean("guest_on_ride", true);
            model->GuestLeftRide = reader->GetBoolean("guest_left_ride", true);
            model->GuestBoughtItem = reader->GetBoolean("guest_bought_item", true);
            model->GuestUsedFacility = reader->GetBoolean("guest_used_facility", true);
            model->GuestDied = reader->GetBoolean("guest_died", true);
        }
    }

    static void WriteNotifications(IIniWriter* writer)
    {
        auto model = &gConfigNotifications;
        writer->WriteSection("notifications");
        writer->WriteBoolean("park_award", model->ParkAward);
        writer->WriteBoolean("park_marketing_campaign_finished", model->ParkMarketingCampaignFinished);
        writer->WriteBoolean("park_warnings", model->ParkWarnings);
        writer->WriteBoolean("park_rating_warnings", model->ParkRatingWarnings);
        writer->WriteBoolean("ride_broken_down", model->RideBrokenDown);
        writer->WriteBoolean("ride_crashed", model->RideCrashed);
        writer->WriteBoolean("ride_casualties", model->RideCasualties);
        writer->WriteBoolean("ride_warnings", model->RideWarnings);
        writer->WriteBoolean("ride_researched", model->RideResearched);
        writer->WriteBoolean("ride_stalled_vehicles", model->RideStalledVehicles);
        writer->WriteBoolean("guest_warnings", model->GuestWarnings);
        writer->WriteBoolean("guest_left_park", model->GuestLeftPark);
        writer->WriteBoolean("guest_queuing_for_ride", model->GuestQueuingForRide);
        writer->WriteBoolean("guest_on_ride", model->GuestOnRide);
        writer->WriteBoolean("guest_left_ride", model->GuestLeftRide);
        writer->WriteBoolean("guest_bought_item", model->GuestBoughtItem);
        writer->WriteBoolean("guest_used_facility", model->GuestUsedFacility);
        writer->WriteBoolean("guest_died", model->GuestDied);
    }

    static void ReadFont(IIniReader* reader)
    {
        if (reader->ReadSection("font"))
        {
            auto model = &gConfigFonts;
            model->FileName = reader->GetString("file_name", "");
            model->FontName = reader->GetString("font_name", "");
            model->OffsetX = reader->GetInt32("x_offset", false);
            model->OffsetY = reader->GetInt32("y_offset", true);
            model->SizeTiny = reader->GetInt32("size_tiny", true);
            model->SizeSmall = reader->GetInt32("size_small", false);
            model->SizeMedium = reader->GetInt32("size_medium", false);
            model->SizeBig = reader->GetInt32("size_big", false);
            model->HeightTiny = reader->GetInt32("height_tiny", false);
            model->HeightSmall = reader->GetInt32("height_small", false);
            model->HeightMedium = reader->GetInt32("height_medium", false);
            model->HeightBig = reader->GetInt32("height_big", false);
            model->EnableHinting = reader->GetBoolean("enable_hinting", true);
            model->HintingThreshold = reader->GetInt32("hinting_threshold", false);
        }
    }

    static void WriteFont(IIniWriter* writer)
    {
        auto model = &gConfigFonts;
        writer->WriteSection("font");
        writer->WriteString("file_name", model->FileName);
        writer->WriteString("font_name", model->FontName);
        writer->WriteInt32("x_offset", model->OffsetX);
        writer->WriteInt32("y_offset", model->OffsetY);
        writer->WriteInt32("size_tiny", model->SizeTiny);
        writer->WriteInt32("size_small", model->SizeSmall);
        writer->WriteInt32("size_medium", model->SizeMedium);
        writer->WriteInt32("size_big", model->SizeBig);
        writer->WriteInt32("height_tiny", model->HeightTiny);
        writer->WriteInt32("height_small", model->HeightSmall);
        writer->WriteInt32("height_medium", model->HeightMedium);
        writer->WriteInt32("height_big", model->HeightBig);
        writer->WriteBoolean("enable_hinting", model->EnableHinting);
        writer->WriteInt32("hinting_threshold", model->HintingThreshold);
    }

    static void ReadPlugin(IIniReader* reader)
    {
        if (reader->ReadSection("plugin"))
        {
            auto model = &gConfigPlugin;
            model->EnableHotReloading = reader->GetBoolean("enable_hot_reloading", false);
            model->AllowedHosts = reader->GetString("allowed_hosts", "");
        }
    }

    static void WritePlugin(IIniWriter* writer)
    {
        auto model = &gConfigPlugin;
        writer->WriteSection("plugin");
        writer->WriteBoolean("enable_hot_reloading", model->EnableHotReloading);
        writer->WriteString("allowed_hosts", model->AllowedHosts);
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
        LOG_VERBOSE("config_find_rct1_path(...)");

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
        LOG_VERBOSE("config_find_rct2_path(...)");

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
        desc.Title = LanguageGetString(STR_SELECT_GOG_INSTALLER);
        desc.Filters.emplace_back(LanguageGetString(STR_GOG_INSTALLER), "*.exe");
        desc.Filters.emplace_back(LanguageGetString(STR_ALL_FILES), "*");

        const auto userHomePath = Platform::GetFolderPath(SPECIAL_FOLDER::USER_HOME);
        desc.InitialDirectory = userHomePath;

        return ContextOpenCommonFileDialog(installerPath, desc, 4096);
    }

    static bool ExtractGogInstaller(const u8string& installerPath, const u8string& targetPath)
    {
        std::string path;
        std::string output;

        if (!Platform::FindApp("innoextract", &path))
        {
            LOG_ERROR("Please install innoextract to extract files from GOG.");
            return false;
        }
        int32_t exit_status = Platform::Execute(
            String::StdFormat(
                "%s '%s' --exclude-temp --output-dir '%s'", path.c_str(), installerPath.c_str(), targetPath.c_str()),
            &output);
        LOG_INFO("Exit status %d", exit_status);
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

void ConfigSetDefaults()
{
    Config::SetDefaults();
}

bool ConfigOpen(u8string_view path)
{
    if (!File::Exists(path))
    {
        return false;
    }

    auto result = Config::ReadFile(path);
    if (result)
    {
        CurrencyLoadCustomCurrencyConfig();
    }
    return result;
}

bool ConfigSave(u8string_view path)
{
    return Config::WriteFile(path);
}

u8string ConfigGetDefaultPath()
{
    auto env = GetContext()->GetPlatformEnvironment();
    return Path::Combine(env->GetDirectoryPath(DIRBASE::USER), u8"config.ini");
}

bool ConfigSaveDefault()
{
    auto path = ConfigGetDefaultPath();
    return ConfigSave(path);
}

bool ConfigFindOrBrowseInstallDirectory()
{
    std::string path = Config::FindRCT2Path();
    if (!path.empty())
    {
        gConfigGeneral.RCT2Path = path;
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
            uiContext->ShowMessageBox(FormatStringID(STR_NEEDS_RCT2_FILES_MANUAL, nullptr));
            return false;
        }

        try
        {
            const char* g1DatPath = PATH_SEPARATOR "Data" PATH_SEPARATOR "g1.dat";
            while (true)
            {
                uiContext->ShowMessageBox(FormatStringID(STR_NEEDS_RCT2_FILES, nullptr));
                std::string gog = LanguageGetString(STR_OWN_ON_GOG);
                std::string hdd = LanguageGetString(STR_INSTALLED_ON_HDD);

                std::vector<std::string> options;
                std::string chosenOption;

                if (uiContext->HasMenuSupport())
                {
                    options.push_back(hdd);
                    options.push_back(gog);
                    int optionIndex = uiContext->ShowMenuDialog(
                        options, LanguageGetString(STR_OPENRCT2_SETUP), LanguageGetString(STR_WHICH_APPLIES_BEST));
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
                    installPath = uiContext->ShowDirectoryDialog(LanguageGetString(STR_PICK_RCT2_DIR));
                }
                else if (chosenOption == gog)
                {
                    // Check if innoextract is installed. If not, prompt the user to install it.
                    std::string dummy;
                    if (!Platform::FindApp("innoextract", &dummy))
                    {
                        uiContext->ShowMessageBox(FormatStringID(STR_INSTALL_INNOEXTRACT, nullptr));
                        return false;
                    }

                    const std::string dest = Path::Combine(
                        GetContext()->GetPlatformEnvironment()->GetDirectoryPath(DIRBASE::CONFIG), "rct2");

                    while (true)
                    {
                        uiContext->ShowMessageBox(LanguageGetString(STR_PLEASE_SELECT_GOG_INSTALLER));
                        utf8 gogPath[4096];
                        if (!Config::SelectGogInstaller(gogPath))
                        {
                            // The user clicked "Cancel", so stop trying.
                            return false;
                        }

                        uiContext->ShowMessageBox(LanguageGetString(STR_THIS_WILL_TAKE_A_FEW_MINUTES));

                        if (Config::ExtractGogInstaller(gogPath, dest))
                            break;

                        uiContext->ShowMessageBox(LanguageGetString(STR_NOT_THE_GOG_INSTALLER));
                    }

                    installPath = Path::Combine(dest, u8"app");
                }
                if (installPath.empty())
                {
                    return false;
                }
                gConfigGeneral.RCT2Path = installPath;

                if (Platform::OriginalGameDataExists(installPath))
                {
                    return true;
                }

                uiContext->ShowMessageBox(FormatStringID(STR_COULD_NOT_FIND_AT_PATH, &g1DatPath));
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
        gConfigGeneral.RCT1Path = std::move(rct1Path);
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

bool CsgIsUsable(const Gx& csg)
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

    Gx csg = {};
    csg.header.num_entries = static_cast<uint32_t>(fileHeaderSize / sizeof(RCTG1Element));
    csg.header.total_size = static_cast<uint32_t>(fileDataSize);
    return CsgIsUsable(csg);
}
