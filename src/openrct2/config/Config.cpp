/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Config.h"

#include "../Context.h"
#include "../Date.h"
#include "../Diagnostic.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../core/Console.hpp"
#include "../core/EnumUtils.hpp"
#include "../core/File.h"
#include "../core/FileStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../drawing/IDrawingEngine.h"
#include "../interface/Window.h"
#include "../localisation/Currency.h"
#include "../localisation/Formatting.h"
#include "../localisation/Language.h"
#include "../localisation/StringIds.h"
#include "../network/Network.h"
#include "../paint/VirtualFloor.h"
#include "../platform/Platform.h"
#include "../rct1/Csg.h"
#include "../ride/Ride.h"
#include "../scenario/Scenario.h"
#include "../ui/UiContext.h"
#include "ConfigEnum.hpp"
#include "IniReader.hpp"
#include "IniWriter.hpp"

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

#ifdef __APPLE__
static constexpr bool kWindowButtonsOnTheLeftDefault = true;
#else
static constexpr bool kWindowButtonsOnTheLeftDefault = false;
#endif
#ifdef __ANDROID__
static constexpr bool kEnlargedUiDefault = true;
#else
static constexpr bool kEnlargedUiDefault = false;
#endif
#ifdef __HAIKU__ // Multi-threading is unstable here
static constexpr bool kMultiThreadingDefault = false;
#else
static constexpr bool kMultiThreadingDefault = true;
#endif

namespace OpenRCT2::Config
{
    static Config _config{};

    Config& Get()
    {
        return _config;
    }

#pragma region Enums

    static const auto Enum_MeasurementFormat = ConfigEnum<MeasurementFormat>({
        ConfigEnumEntry<MeasurementFormat>("IMPERIAL", MeasurementFormat::Imperial),
        ConfigEnumEntry<MeasurementFormat>("METRIC", MeasurementFormat::Metric),
        ConfigEnumEntry<MeasurementFormat>("SI", MeasurementFormat::SI),
    });

    static const auto Enum_Currency = ConfigEnum<CurrencyType>({
        ConfigEnumEntry<CurrencyType>("GBP", CurrencyType::pounds),
        ConfigEnumEntry<CurrencyType>("USD", CurrencyType::dollars),
        ConfigEnumEntry<CurrencyType>("FRF", CurrencyType::franc),
        ConfigEnumEntry<CurrencyType>("DEM", CurrencyType::deutscheMark),
        ConfigEnumEntry<CurrencyType>("JPY", CurrencyType::yen),
        ConfigEnumEntry<CurrencyType>("ESP", CurrencyType::peseta),
        ConfigEnumEntry<CurrencyType>("ITL", CurrencyType::lira),
        ConfigEnumEntry<CurrencyType>("NLG", CurrencyType::guilders),
        ConfigEnumEntry<CurrencyType>("SEK", CurrencyType::krona),
        ConfigEnumEntry<CurrencyType>("EUR", CurrencyType::euros),
        ConfigEnumEntry<CurrencyType>("KRW", CurrencyType::won),
        ConfigEnumEntry<CurrencyType>("RUB", CurrencyType::rouble),
        ConfigEnumEntry<CurrencyType>("CZK", CurrencyType::czechKoruna),
        ConfigEnumEntry<CurrencyType>("HKD", CurrencyType::hongKongDollar),
        ConfigEnumEntry<CurrencyType>("TWD", CurrencyType::newTaiwanDollar),
        ConfigEnumEntry<CurrencyType>("CNY", CurrencyType::yuan),
        ConfigEnumEntry<CurrencyType>("HUF", CurrencyType::forint),
        ConfigEnumEntry<CurrencyType>("CUSTOM", CurrencyType::custom),
    });

    static const auto Enum_CurrencySymbolAffix = ConfigEnum<CurrencyAffix>({
        ConfigEnumEntry<CurrencyAffix>("PREFIX", CurrencyAffix::prefix),
        ConfigEnumEntry<CurrencyAffix>("SUFFIX", CurrencyAffix::suffix),
    });

    static const auto Enum_DateFormat = ConfigEnum<int32_t>({
        ConfigEnumEntry<int32_t>("DD/MM/YY", DATE_FORMAT_DAY_MONTH_YEAR),
        ConfigEnumEntry<int32_t>("MM/DD/YY", DATE_FORMAT_MONTH_DAY_YEAR),
        ConfigEnumEntry<int32_t>("YY/MM/DD", DATE_FORMAT_YEAR_MONTH_DAY),
        ConfigEnumEntry<int32_t>("YY/DD/MM", DATE_FORMAT_YEAR_DAY_MONTH),
    });

    static const auto Enum_DrawingEngine = ConfigEnum<DrawingEngine>({
        ConfigEnumEntry<DrawingEngine>("SOFTWARE_HWD", DrawingEngine::SoftwareWithHardwareDisplay),
        ConfigEnumEntry<DrawingEngine>("OPENGL", DrawingEngine::OpenGL),
    });

    static const auto Enum_Temperature = ConfigEnum<TemperatureUnit>({
        ConfigEnumEntry<TemperatureUnit>("CELSIUS", TemperatureUnit::Celsius),
        ConfigEnumEntry<TemperatureUnit>("FAHRENHEIT", TemperatureUnit::Fahrenheit),
    });

    static const auto Enum_FileBrowserSort = ConfigEnum<FileBrowserSort>({
        ConfigEnumEntry<FileBrowserSort>("NAME_ASCENDING", FileBrowserSort::NameAscending),
        ConfigEnumEntry<FileBrowserSort>("NAME_DESCENDING", FileBrowserSort::NameDescending),
        ConfigEnumEntry<FileBrowserSort>("SIZE_ASCENDING", FileBrowserSort::SizeAscending),
        ConfigEnumEntry<FileBrowserSort>("SIZE_DESCENDING", FileBrowserSort::SizeDescending),
        ConfigEnumEntry<FileBrowserSort>("DATE_ASCENDING", FileBrowserSort::DateAscending),
        ConfigEnumEntry<FileBrowserSort>("DATE_DESCENDING", FileBrowserSort::DateDescending),
    });

    static const auto Enum_ParkPreviewPref = ConfigEnum<ParkPreviewPref>({
        ConfigEnumEntry<ParkPreviewPref>("DISABLED", ParkPreviewPref::disabled),
        ConfigEnumEntry<ParkPreviewPref>("SCREENSHOT", ParkPreviewPref::screenshot),
        ConfigEnumEntry<ParkPreviewPref>("MINIMAP", ParkPreviewPref::miniMap),
    });

    static const auto Enum_VirtualFloorStyle = ConfigEnum<VirtualFloorStyles>({
        ConfigEnumEntry<VirtualFloorStyles>("OFF", VirtualFloorStyles::Off),
        ConfigEnumEntry<VirtualFloorStyles>("CLEAR", VirtualFloorStyles::Clear),
        ConfigEnumEntry<VirtualFloorStyles>("GLASSY", VirtualFloorStyles::Glassy),
    });

    static const auto Enum_RideInspectionInterval = ConfigEnum<RideInspection>({
        ConfigEnumEntry<RideInspection>("EVERY_10_MINUTES", RideInspection::every10Minutes),
        ConfigEnumEntry<RideInspection>("EVERY_20_MINUTES", RideInspection::every20Minutes),
        ConfigEnumEntry<RideInspection>("EVERY_30_MINUTES", RideInspection::every30Minutes),
        ConfigEnumEntry<RideInspection>("EVERY_45_MINUTES", RideInspection::every45Minutes),
        ConfigEnumEntry<RideInspection>("EVERY_HOUR", RideInspection::everyHour),
        ConfigEnumEntry<RideInspection>("EVERY_2_HOURS", RideInspection::every2Hours),
        ConfigEnumEntry<RideInspection>("NEVER", RideInspection::never),
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
                if (String::equals(key.c_str(), langDesc.locale))
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
            auto model = &_config.general;
            model->alwaysShowGridlines = reader->GetBoolean("always_show_gridlines", false);
            model->autosaveFrequency = reader->GetInt32("autosave", AUTOSAVE_EVERY_5MINUTES);
            model->autosaveAmount = reader->GetInt32("autosave_amount", kDefaultNumAutosavesToKeep);
            model->confirmationPrompt = reader->GetBoolean("confirmation_prompt", false);
            model->currencyFormat = reader->GetEnum<CurrencyType>(
                "currency_format", Platform::GetLocaleCurrency(), Enum_Currency);
            model->customCurrencyRate = reader->GetInt32("custom_currency_rate", 10);
            model->customCurrencyAffix = reader->GetEnum<CurrencyAffix>(
                "custom_currency_affix", CurrencyAffix::suffix, Enum_CurrencySymbolAffix);
            model->customCurrencySymbol = reader->GetString("custom_currency_symbol", "Ctm");
            model->edgeScrolling = reader->GetBoolean("edge_scrolling", true);
            model->edgeScrollingSpeed = reader->GetInt32("edge_scrolling_speed", 12);
            model->fullscreenMode = reader->GetInt32("fullscreen_mode", 0);
            model->fullscreenHeight = reader->GetInt32("fullscreen_height", -1);
            model->fullscreenWidth = reader->GetInt32("fullscreen_width", -1);
            model->rct1Path = reader->GetString("rct1_path", "");
            model->rct2Path = reader->GetString("game_path", "");
            model->landscapeSmoothing = reader->GetBoolean("landscape_smoothing", true);
            model->language = reader->GetEnum<int32_t>("language", Platform::GetLocaleLanguage(), Enum_LanguageEnum);
            model->measurementFormat = reader->GetEnum<MeasurementFormat>(
                "measurement_format", Platform::GetLocaleMeasurementFormat(), Enum_MeasurementFormat);
            model->playIntro = reader->GetBoolean("play_intro", false);
            model->savePluginData = reader->GetBoolean("save_plugin_data", true);
            model->debuggingTools = reader->GetBoolean("debugging_tools", false);
            model->showHeightAsUnits = reader->GetBoolean("show_height_as_units", false);
            model->temperatureFormat = reader->GetEnum<TemperatureUnit>(
                "temperature_format", Platform::GetLocaleTemperatureFormat(), Enum_Temperature);
            model->windowHeight = reader->GetInt32("window_height", -1);
            model->windowSnapProximity = reader->GetInt32("window_snap_proximity", 5);
            model->windowWidth = reader->GetInt32("window_width", -1);
            model->defaultDisplay = reader->GetInt32("default_display", 0);
            model->drawingEngine = reader->GetEnum<DrawingEngine>(
                "drawing_engine", DrawingEngine::SoftwareWithHardwareDisplay, Enum_DrawingEngine);
            model->uncapFPS = reader->GetBoolean("uncap_fps", false);
            model->useVSync = reader->GetBoolean("use_vsync", true);
            model->virtualFloorStyle = reader->GetEnum<VirtualFloorStyles>(
                "virtual_floor_style", VirtualFloorStyles::Glassy, Enum_VirtualFloorStyle);
            model->dateFormat = reader->GetEnum<int32_t>("date_format", Platform::GetLocaleDateFormat(), Enum_DateFormat);
            model->autoStaffPlacement = reader->GetBoolean("auto_staff", true);
            model->handymenMowByDefault = reader->GetBoolean("handymen_mow_default", false);
            model->defaultInspectionInterval = reader->GetEnum<RideInspection>(
                "default_inspection_interval", RideInspection::every30Minutes, Enum_RideInspectionInterval);
            model->lastRunVersion = reader->GetString("last_run_version", "");
            model->invertViewportDrag = reader->GetBoolean("invert_viewport_drag", false);
            model->loadSaveSort = reader->GetEnum<FileBrowserSort>(
                "load_save_sort", FileBrowserSort::NameAscending, Enum_FileBrowserSort);
            model->minimizeFullscreenFocusLoss = reader->GetBoolean("minimize_fullscreen_focus_loss", true);
            model->disableScreensaver = reader->GetBoolean("disable_screensaver", true);

            // Default config setting is false until the games canvas can be separated from the effect
            model->dayNightCycle = reader->GetBoolean("day_night_cycle", false);
            const bool supportsLightFx = model->drawingEngine == DrawingEngine::SoftwareWithHardwareDisplay;
            model->enableLightFx = supportsLightFx && reader->GetBoolean("enable_light_fx", false);
            model->enableLightFxForVehicles = supportsLightFx && reader->GetBoolean("enable_light_fx_for_vehicles", false);
            model->upperCaseBanners = reader->GetBoolean("upper_case_banners", false);
            model->disableLightningEffect = reader->GetBoolean("disable_lightning_effect", false);
            model->windowScale = reader->GetFloat("window_scale", Platform::GetDefaultScale());
            model->inferDisplayDPI = reader->GetBoolean("infer_display_dpi", true);
            model->showFPS = reader->GetBoolean("show_fps", false);
#ifdef _DEBUG
            // Always have multi-threading disabled in debug builds, this makes things slower.
            model->multiThreading = false;
#else
            model->multiThreading = reader->GetBoolean("multithreading", kMultiThreadingDefault);
#endif // _DEBUG
            model->trapCursor = reader->GetBoolean("trap_cursor", false);
            model->autoOpenShops = reader->GetBoolean("auto_open_shops", false);

            // Gamepad settings
            model->gamepadDeadzone = reader->GetInt32("gamepad_deadzone", 3600);
            model->gamepadSensitivity = reader->GetFloat("gamepad_sensitivity", 1.5f);
            model->scenarioUnlockingEnabled = reader->GetBoolean("scenario_unlocking_enabled", true);
            model->scenarioHideMegaPark = reader->GetBoolean("scenario_hide_mega_park", true);
            model->lastSaveGameDirectory = reader->GetString("last_game_directory", "");
            model->lastSaveLandscapeDirectory = reader->GetString("last_landscape_directory", "");
            model->lastSaveScenarioDirectory = reader->GetString("last_scenario_directory", "");
            model->lastSaveTrackDirectory = reader->GetString("last_track_directory", "");
            model->useNativeBrowseDialog = reader->GetBoolean("use_native_browse_dialog", false);
            model->windowLimit = reader->GetInt32("window_limit", kWindowLimitMax);
            model->zoomToCursor = reader->GetBoolean("zoom_to_cursor", true);
            model->renderWeatherEffects = reader->GetBoolean("render_weather_effects", true);
            model->renderWeatherGloom = reader->GetBoolean("render_weather_gloom", true);
            model->showGuestPurchases = reader->GetBoolean("show_guest_purchases", false);
            model->showRealNamesOfGuests = reader->GetBoolean("show_real_names_of_guests", true);
            model->showRealNamesOfStaff = reader->GetBoolean("show_real_names_of_staff", false);
            model->allowEarlyCompletion = reader->GetBoolean("allow_early_completion", false);
            model->assetPackOrder = reader->GetString("asset_pack_order", "");
            model->enabledAssetPacks = reader->GetString("enabled_asset_packs", "");
            model->transparentScreenshot = reader->GetBoolean("transparent_screenshot", true);
            model->transparentWater = reader->GetBoolean("transparent_water", true);

            model->invisibleRides = reader->GetBoolean("invisible_rides", false);
            model->invisibleVehicles = reader->GetBoolean("invisible_vehicles", false);
            model->invisibleTrees = reader->GetBoolean("invisible_trees", false);
            model->invisibleScenery = reader->GetBoolean("invisible_scenery", false);
            model->invisiblePaths = reader->GetBoolean("invisible_paths", false);
            model->invisibleSupports = reader->GetBoolean("invisible_supports", true);

            model->lastVersionCheckTime = reader->GetInt64("last_version_check_time", 0);

            model->fileBrowserWidth = reader->GetInt32("file_browser_width", 0);
            model->fileBrowserHeight = reader->GetInt32("file_browser_height", 0);
            model->fileBrowserShowSizeColumn = reader->GetBoolean("file_browser_show_size_column", true);
            model->fileBrowserShowDateColumn = reader->GetBoolean("file_browser_show_date_column", true);
            model->fileBrowserPreviewType = reader->GetEnum<ParkPreviewPref>(
                "file_browser_preview_type", ParkPreviewPref::screenshot, Enum_ParkPreviewPref);
        }
    }

    static void WriteGeneral(IIniWriter* writer)
    {
        auto model = &_config.general;
        writer->WriteSection("general");
        writer->WriteBoolean("always_show_gridlines", model->alwaysShowGridlines);
        writer->WriteInt32("autosave", model->autosaveFrequency);
        writer->WriteInt32("autosave_amount", model->autosaveAmount);
        writer->WriteBoolean("confirmation_prompt", model->confirmationPrompt);
        writer->WriteEnum<CurrencyType>("currency_format", model->currencyFormat, Enum_Currency);
        writer->WriteInt32("custom_currency_rate", model->customCurrencyRate);
        writer->WriteEnum<CurrencyAffix>("custom_currency_affix", model->customCurrencyAffix, Enum_CurrencySymbolAffix);
        writer->WriteString("custom_currency_symbol", model->customCurrencySymbol);
        writer->WriteBoolean("edge_scrolling", model->edgeScrolling);
        writer->WriteInt32("edge_scrolling_speed", model->edgeScrollingSpeed);
        writer->WriteInt32("fullscreen_mode", model->fullscreenMode);
        writer->WriteInt32("fullscreen_height", model->fullscreenHeight);
        writer->WriteInt32("fullscreen_width", model->fullscreenWidth);
        writer->WriteString("rct1_path", model->rct1Path);
        writer->WriteString("game_path", model->rct2Path);
        writer->WriteBoolean("landscape_smoothing", model->landscapeSmoothing);
        writer->WriteEnum<int32_t>("language", model->language, Enum_LanguageEnum);
        writer->WriteEnum<MeasurementFormat>("measurement_format", model->measurementFormat, Enum_MeasurementFormat);
        writer->WriteBoolean("play_intro", model->playIntro);
        writer->WriteBoolean("save_plugin_data", model->savePluginData);
        writer->WriteBoolean("debugging_tools", model->debuggingTools);
        writer->WriteBoolean("show_height_as_units", model->showHeightAsUnits);
        writer->WriteEnum<TemperatureUnit>("temperature_format", model->temperatureFormat, Enum_Temperature);
        writer->WriteInt32("window_height", model->windowHeight);
        writer->WriteInt32("window_snap_proximity", model->windowSnapProximity);
        writer->WriteInt32("window_width", model->windowWidth);
        writer->WriteInt32("default_display", model->defaultDisplay);
        writer->WriteEnum<DrawingEngine>("drawing_engine", model->drawingEngine, Enum_DrawingEngine);
        writer->WriteBoolean("uncap_fps", model->uncapFPS);
        writer->WriteBoolean("use_vsync", model->useVSync);
        writer->WriteEnum<int32_t>("date_format", model->dateFormat, Enum_DateFormat);
        writer->WriteBoolean("auto_staff", model->autoStaffPlacement);
        writer->WriteBoolean("handymen_mow_default", model->handymenMowByDefault);
        writer->WriteEnum<RideInspection>(
            "default_inspection_interval", model->defaultInspectionInterval, Enum_RideInspectionInterval);
        writer->WriteString("last_run_version", model->lastRunVersion);
        writer->WriteBoolean("invert_viewport_drag", model->invertViewportDrag);
        writer->WriteEnum<FileBrowserSort>("load_save_sort", model->loadSaveSort, Enum_FileBrowserSort);
        writer->WriteBoolean("minimize_fullscreen_focus_loss", model->minimizeFullscreenFocusLoss);
        writer->WriteBoolean("disable_screensaver", model->disableScreensaver);
        writer->WriteBoolean("day_night_cycle", model->dayNightCycle);
        writer->WriteBoolean("enable_light_fx", model->enableLightFx);
        writer->WriteBoolean("enable_light_fx_for_vehicles", model->enableLightFxForVehicles);
        writer->WriteBoolean("upper_case_banners", model->upperCaseBanners);
        writer->WriteBoolean("disable_lightning_effect", model->disableLightningEffect);
        writer->WriteFloat("window_scale", model->windowScale);
        writer->WriteBoolean("infer_display_dpi", model->inferDisplayDPI);
        writer->WriteBoolean("show_fps", model->showFPS);
        writer->WriteBoolean("multithreading", model->multiThreading);
        writer->WriteBoolean("trap_cursor", model->trapCursor);
        writer->WriteBoolean("auto_open_shops", model->autoOpenShops);

        // Gamepad settings
        writer->WriteInt32("gamepad_deadzone", model->gamepadDeadzone);
        writer->WriteFloat("gamepad_sensitivity", model->gamepadSensitivity);
        writer->WriteBoolean("scenario_unlocking_enabled", model->scenarioUnlockingEnabled);
        writer->WriteBoolean("scenario_hide_mega_park", model->scenarioHideMegaPark);
        writer->WriteString("last_game_directory", model->lastSaveGameDirectory);
        writer->WriteString("last_landscape_directory", model->lastSaveLandscapeDirectory);
        writer->WriteString("last_scenario_directory", model->lastSaveScenarioDirectory);
        writer->WriteString("last_track_directory", model->lastSaveTrackDirectory);
        writer->WriteBoolean("use_native_browse_dialog", model->useNativeBrowseDialog);
        writer->WriteInt32("window_limit", model->windowLimit);
        writer->WriteBoolean("zoom_to_cursor", model->zoomToCursor);
        writer->WriteBoolean("render_weather_effects", model->renderWeatherEffects);
        writer->WriteBoolean("render_weather_gloom", model->renderWeatherGloom);
        writer->WriteBoolean("show_guest_purchases", model->showGuestPurchases);
        writer->WriteBoolean("show_real_names_of_guests", model->showRealNamesOfGuests);
        writer->WriteBoolean("show_real_names_of_staff", model->showRealNamesOfStaff);
        writer->WriteBoolean("allow_early_completion", model->allowEarlyCompletion);
        writer->WriteString("asset_pack_order", model->assetPackOrder);
        writer->WriteString("enabled_asset_packs", model->enabledAssetPacks);
        writer->WriteEnum<VirtualFloorStyles>("virtual_floor_style", model->virtualFloorStyle, Enum_VirtualFloorStyle);
        writer->WriteBoolean("transparent_screenshot", model->transparentScreenshot);
        writer->WriteBoolean("transparent_water", model->transparentWater);
        writer->WriteBoolean("invisible_rides", model->invisibleRides);
        writer->WriteBoolean("invisible_vehicles", model->invisibleVehicles);
        writer->WriteBoolean("invisible_trees", model->invisibleTrees);
        writer->WriteBoolean("invisible_scenery", model->invisibleScenery);
        writer->WriteBoolean("invisible_paths", model->invisiblePaths);
        writer->WriteBoolean("invisible_supports", model->invisibleSupports);
        writer->WriteInt64("last_version_check_time", model->lastVersionCheckTime);
        writer->WriteInt32("file_browser_width", model->fileBrowserWidth);
        writer->WriteInt32("file_browser_height", model->fileBrowserHeight);
        writer->WriteBoolean("file_browser_show_size_column", model->fileBrowserShowSizeColumn);
        writer->WriteBoolean("file_browser_show_date_column", model->fileBrowserShowDateColumn);
        writer->WriteEnum<ParkPreviewPref>("file_browser_preview_type", model->fileBrowserPreviewType, Enum_ParkPreviewPref);
    }

    static void ReadInterface(IIniReader* reader)
    {
        if (reader->ReadSection("interface"))
        {
            auto model = &_config.interface;
            model->toolbarButtonsCentred = reader->GetBoolean("toolbar_buttons_centred", false);
            model->toolbarShowFinances = reader->GetBoolean("toolbar_show_finances", true);
            model->toolbarShowResearch = reader->GetBoolean("toolbar_show_research", true);
            model->toolbarShowCheats = reader->GetBoolean("toolbar_show_cheats", false);
            model->toolbarShowNews = reader->GetBoolean("toolbar_show_news", false);
            model->toolbarShowMute = reader->GetBoolean("toolbar_show_mute", false);
            model->toolbarShowChat = reader->GetBoolean("toolbar_show_chat", false);
            model->toolbarShowZoom = reader->GetBoolean("toolbar_show_zoom", true);
            model->toolbarShowRotateAnticlockwise = reader->GetBoolean("toolbar_show_rotate_anti_clockwise", false);
            model->consoleSmallFont = reader->GetBoolean("console_small_font", false);
            model->currentThemePreset = reader->GetString("current_theme", "*RCT2");
            model->currentTitleSequencePreset = reader->GetString("current_title_sequence", "*OPENRCT2");
            model->randomTitleSequence = reader->GetBoolean("random_title_sequence", false);
            model->objectSelectionFilterFlags = reader->GetInt32("object_selection_filter_flags", 0x3FFF);
            model->scenarioSelectLastTab = reader->GetInt32("scenarioselect_last_tab", 0);
            model->scenarioPreviewScreenshots = reader->GetBoolean("scenario_preview_screenshots", true);
            model->listRideVehiclesSeparately = reader->GetBoolean("list_ride_vehicles_separately", false);
            model->windowButtonsOnTheLeft = reader->GetBoolean("window_buttons_on_the_left", kWindowButtonsOnTheLeftDefault);
            model->enlargedUi = reader->GetBoolean("enlarged_ui", kEnlargedUiDefault);
            model->touchEnhancements = reader->GetBoolean("touch_enhancements", kEnlargedUiDefault);
        }
    }

    static void WriteInterface(IIniWriter* writer)
    {
        auto model = &_config.interface;
        writer->WriteSection("interface");
        writer->WriteBoolean("toolbar_buttons_centred", model->toolbarButtonsCentred);
        writer->WriteBoolean("toolbar_show_finances", model->toolbarShowFinances);
        writer->WriteBoolean("toolbar_show_research", model->toolbarShowResearch);
        writer->WriteBoolean("toolbar_show_cheats", model->toolbarShowCheats);
        writer->WriteBoolean("toolbar_show_news", model->toolbarShowNews);
        writer->WriteBoolean("toolbar_show_mute", model->toolbarShowMute);
        writer->WriteBoolean("toolbar_show_chat", model->toolbarShowChat);
        writer->WriteBoolean("toolbar_show_zoom", model->toolbarShowZoom);
        writer->WriteBoolean("toolbar_show_rotate_anti_clockwise", model->toolbarShowRotateAnticlockwise);
        writer->WriteBoolean("console_small_font", model->consoleSmallFont);
        writer->WriteString("current_theme", model->currentThemePreset);
        writer->WriteString("current_title_sequence", model->currentTitleSequencePreset);
        writer->WriteBoolean("random_title_sequence", model->randomTitleSequence);
        writer->WriteInt32("object_selection_filter_flags", model->objectSelectionFilterFlags);
        writer->WriteInt32("scenarioselect_last_tab", model->scenarioSelectLastTab);
        writer->WriteBoolean("scenario_preview_screenshots", model->scenarioPreviewScreenshots);
        writer->WriteBoolean("list_ride_vehicles_separately", model->listRideVehiclesSeparately);
        writer->WriteBoolean("window_buttons_on_the_left", model->windowButtonsOnTheLeft);
        writer->WriteBoolean("enlarged_ui", model->enlargedUi);
        writer->WriteBoolean("touch_enhancements", model->touchEnhancements);
    }

    static void ReadSound(IIniReader* reader)
    {
        if (reader->ReadSection("sound"))
        {
            auto model = &_config.sound;
            model->device = reader->GetString("audio_device", "");
            model->masterSoundEnabled = reader->GetBoolean("master_sound", true);
            model->masterVolume = reader->GetInt32("master_volume", 100);
            model->titleMusic = static_cast<TitleMusicKind>(
                reader->GetInt32("title_theme", EnumValue(TitleMusicKind::OpenRCT2)));
            model->soundEnabled = reader->GetBoolean("sound", true);
            model->soundVolume = reader->GetInt32("sound_volume", 100);
            model->rideMusicEnabled = reader->GetBoolean("ride_music", true);
            model->rideMusicVolume = reader->GetInt32("ride_music_volume", 100);
            model->audioFocus = reader->GetBoolean("audio_focus", false);
        }
    }

    static void WriteSound(IIniWriter* writer)
    {
        auto model = &_config.sound;
        writer->WriteSection("sound");
        writer->WriteString("audio_device", model->device);
        writer->WriteBoolean("master_sound", model->masterSoundEnabled);
        writer->WriteInt32("master_volume", model->masterVolume);
        writer->WriteInt32("title_theme", EnumValue(model->titleMusic));
        writer->WriteBoolean("sound", model->soundEnabled);
        writer->WriteInt32("sound_volume", model->soundVolume);
        writer->WriteBoolean("ride_music", model->rideMusicEnabled);
        writer->WriteInt32("ride_music_volume", model->rideMusicVolume);
        writer->WriteBoolean("audio_focus", model->audioFocus);
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
            playerName = String::trim(playerName);

            auto model = &_config.network;
            model->playerName = std::move(playerName);
            model->defaultPort = reader->GetInt32("default_port", ::Network::kDefaultPort);
            model->listenAddress = reader->GetString("listen_address", "");
            model->defaultPassword = reader->GetString("default_password", "");
            model->stayConnected = reader->GetBoolean("stay_connected", true);
            model->advertise = reader->GetBoolean("advertise", true);
            model->advertiseAddress = reader->GetString("advertise_address", "");
            model->maxplayers = reader->GetInt32("maxplayers", 16);
            model->serverName = reader->GetString("server_name", "Server");
            model->serverDescription = reader->GetString("server_description", "");
            model->serverGreeting = reader->GetString("server_greeting", "");
            model->masterServerUrl = reader->GetString("master_server_url", "");
            model->providerName = reader->GetString("provider_name", "");
            model->providerEmail = reader->GetString("provider_email", "");
            model->providerWebsite = reader->GetString("provider_website", "");
            model->knownKeysOnly = reader->GetBoolean("known_keys_only", false);
            model->logChat = reader->GetBoolean("log_chat", false);
            model->logServerActions = reader->GetBoolean("log_server_actions", false);
            model->pauseServerIfNoClients = reader->GetBoolean("pause_server_if_no_clients", false);
            model->desyncDebugging = reader->GetBoolean("desync_debugging", false);
        }
    }

    static void WriteNetwork(IIniWriter* writer)
    {
        auto model = &_config.network;
        writer->WriteSection("network");
        writer->WriteString("player_name", model->playerName);
        writer->WriteInt32("default_port", model->defaultPort);
        writer->WriteString("listen_address", model->listenAddress);
        writer->WriteString("default_password", model->defaultPassword);
        writer->WriteBoolean("stay_connected", model->stayConnected);
        writer->WriteBoolean("advertise", model->advertise);
        writer->WriteString("advertise_address", model->advertiseAddress);
        writer->WriteInt32("maxplayers", model->maxplayers);
        writer->WriteString("server_name", model->serverName);
        writer->WriteString("server_description", model->serverDescription);
        writer->WriteString("server_greeting", model->serverGreeting);
        writer->WriteString("master_server_url", model->masterServerUrl);
        writer->WriteString("provider_name", model->providerName);
        writer->WriteString("provider_email", model->providerEmail);
        writer->WriteString("provider_website", model->providerWebsite);
        writer->WriteBoolean("known_keys_only", model->knownKeysOnly);
        writer->WriteBoolean("log_chat", model->logChat);
        writer->WriteBoolean("log_server_actions", model->logServerActions);
        writer->WriteBoolean("pause_server_if_no_clients", model->pauseServerIfNoClients);
        writer->WriteBoolean("desync_debugging", model->desyncDebugging);
    }

    static void ReadNotifications(IIniReader* reader)
    {
        if (reader->ReadSection("notifications"))
        {
            auto model = &_config.notifications;
            model->parkAward = reader->GetBoolean("park_award", true);
            model->parkMarketingCampaignFinished = reader->GetBoolean("park_marketing_campaign_finished", true);
            model->parkWarnings = reader->GetBoolean("park_warnings", true);
            model->parkRatingWarnings = reader->GetBoolean("park_rating_warnings", true);
            model->rideBrokenDown = reader->GetBoolean("ride_broken_down", true);
            model->rideCrashed = reader->GetBoolean("ride_crashed", true);
            model->rideCasualties = reader->GetBoolean("ride_casualties", true);
            model->rideWarnings = reader->GetBoolean("ride_warnings", true);
            model->rideResearched = reader->GetBoolean("ride_researched", true);
            model->rideStalledVehicles = reader->GetBoolean("ride_stalled_vehicles", true);
            model->guestWarnings = reader->GetBoolean("guest_warnings", true);
            model->guestLeftPark = reader->GetBoolean("guest_left_park", true);
            model->guestQueuingForRide = reader->GetBoolean("guest_queuing_for_ride", true);
            model->guestOnRide = reader->GetBoolean("guest_on_ride", true);
            model->guestLeftRide = reader->GetBoolean("guest_left_ride", true);
            model->guestBoughtItem = reader->GetBoolean("guest_bought_item", true);
            model->guestUsedFacility = reader->GetBoolean("guest_used_facility", true);
            model->guestDied = reader->GetBoolean("guest_died", true);
        }
    }

    static void WriteNotifications(IIniWriter* writer)
    {
        auto model = &_config.notifications;
        writer->WriteSection("notifications");
        writer->WriteBoolean("park_award", model->parkAward);
        writer->WriteBoolean("park_marketing_campaign_finished", model->parkMarketingCampaignFinished);
        writer->WriteBoolean("park_warnings", model->parkWarnings);
        writer->WriteBoolean("park_rating_warnings", model->parkRatingWarnings);
        writer->WriteBoolean("ride_broken_down", model->rideBrokenDown);
        writer->WriteBoolean("ride_crashed", model->rideCrashed);
        writer->WriteBoolean("ride_casualties", model->rideCasualties);
        writer->WriteBoolean("ride_warnings", model->rideWarnings);
        writer->WriteBoolean("ride_researched", model->rideResearched);
        writer->WriteBoolean("ride_stalled_vehicles", model->rideStalledVehicles);
        writer->WriteBoolean("guest_warnings", model->guestWarnings);
        writer->WriteBoolean("guest_left_park", model->guestLeftPark);
        writer->WriteBoolean("guest_queuing_for_ride", model->guestQueuingForRide);
        writer->WriteBoolean("guest_on_ride", model->guestOnRide);
        writer->WriteBoolean("guest_left_ride", model->guestLeftRide);
        writer->WriteBoolean("guest_bought_item", model->guestBoughtItem);
        writer->WriteBoolean("guest_used_facility", model->guestUsedFacility);
        writer->WriteBoolean("guest_died", model->guestDied);
    }

    static void ReadFont(IIniReader* reader)
    {
        if (reader->ReadSection("font"))
        {
            auto model = &_config.fonts;
            model->fileName = reader->GetString("file_name", "");
            model->fontName = reader->GetString("font_name", "");
            model->offsetX = reader->GetInt32("x_offset", false);
            model->offsetY = reader->GetInt32("y_offset", true);
            model->sizeTiny = reader->GetInt32("size_tiny", true);
            model->sizeSmall = reader->GetInt32("size_small", false);
            model->sizeMedium = reader->GetInt32("size_medium", false);
            model->sizeBig = reader->GetInt32("size_big", false);
            model->heightTiny = reader->GetInt32("height_tiny", false);
            model->heightSmall = reader->GetInt32("height_small", false);
            model->heightMedium = reader->GetInt32("height_medium", false);
            model->heightBig = reader->GetInt32("height_big", false);
            model->enableHinting = reader->GetBoolean("enable_hinting", true);
            model->hintingThreshold = reader->GetInt32("hinting_threshold", false);
        }
    }

    static void WriteFont(IIniWriter* writer)
    {
        auto model = &_config.fonts;
        writer->WriteSection("font");
        writer->WriteString("file_name", model->fileName);
        writer->WriteString("font_name", model->fontName);
        writer->WriteInt32("x_offset", model->offsetX);
        writer->WriteInt32("y_offset", model->offsetY);
        writer->WriteInt32("size_tiny", model->sizeTiny);
        writer->WriteInt32("size_small", model->sizeSmall);
        writer->WriteInt32("size_medium", model->sizeMedium);
        writer->WriteInt32("size_big", model->sizeBig);
        writer->WriteInt32("height_tiny", model->heightTiny);
        writer->WriteInt32("height_small", model->heightSmall);
        writer->WriteInt32("height_medium", model->heightMedium);
        writer->WriteInt32("height_big", model->heightBig);
        writer->WriteBoolean("enable_hinting", model->enableHinting);
        writer->WriteInt32("hinting_threshold", model->hintingThreshold);
    }

    static void ReadPlugin(IIniReader* reader)
    {
        if (reader->ReadSection("plugin"))
        {
            auto model = &_config.plugin;
            model->enableHotReloading = reader->GetBoolean("enable_hot_reloading", false);
            model->allowedHosts = reader->GetString("allowed_hosts", "");
        }
    }

    static void WritePlugin(IIniWriter* writer)
    {
        auto model = &_config.plugin;
        writer->WriteSection("plugin");
        writer->WriteBoolean("enable_hot_reloading", model->enableHotReloading);
        writer->WriteString("allowed_hosts", model->allowedHosts);
    }

    bool SetDefaults()
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
            auto fs = FileStream(path, FileMode::open);
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

            auto fs = FileStream(path, FileMode::write);
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

        static std::vector<u8string_view> searchLocations = Platform::GetSearchablePathsRCT1();

        for (const auto& location : searchLocations)
        {
            if (RCT1DataPresentAtLocation(location))
            {
                return u8string(location);
            }
        }

        auto steamPaths = Platform::GetSteamPaths();
        if (steamPaths.isSteamPresent())
        {
            for (const auto& root : steamPaths.roots)
            {
                auto nativePath = Path::Combine(root, steamPaths.nativeFolder, Platform::kSteamRCT1Data.nativeFolder);
                if (RCT1DataPresentAtLocation(nativePath))
                {
                    return nativePath;
                }
                if (!steamPaths.downloadDepotFolder.empty())
                {
                    auto downloadDepotPath = steamPaths.getDownloadDepotFolder(root, Platform::kSteamRCT1Data);
                    if (RCT1DataPresentAtLocation(downloadDepotPath))
                    {
                        return downloadDepotPath;
                    }
                }
            }
        }

        auto exePath = Path::GetDirectory(Platform::GetCurrentExecutablePath());
        if (RCT1DataPresentAtLocation(exePath))
        {
            return exePath;
        }
        return {};
    }

    static u8string FindRCT2SteamPath()
    {
        auto steamPaths = Platform::GetSteamPaths();
        if (steamPaths.isSteamPresent())
        {
            const std::array<Platform::SteamGameData, 2> gamesToCheck = {
                Platform::kSteamRCT2Data,
                Platform::kSteamRCTCData,
            };
            for (const auto& root : steamPaths.roots)
            {
                for (const auto& game : gamesToCheck)
                {
                    auto nativePath = Path::Combine(root, steamPaths.nativeFolder, game.nativeFolder);
                    if (Platform::OriginalGameDataExists(nativePath))
                    {
                        return nativePath;
                    }
                    if (!steamPaths.downloadDepotFolder.empty())
                    {
                        auto downloadDepotPath = steamPaths.getDownloadDepotFolder(root, game);
                        if (Platform::OriginalGameDataExists(downloadDepotPath))
                        {
                            return downloadDepotPath;
                        }
                    }
                }
            }
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

        static std::vector<u8string_view> searchLocations = Platform::GetSearchablePathsRCT2();

        for (const auto& location : searchLocations)
        {
            if (Platform::OriginalGameDataExists(location))
            {
                return u8string(location);
            }
        }

        // Will only return a path if the RCT2 data is present, so no need to check twice.
        auto steamPath = FindRCT2SteamPath();
        if (!steamPath.empty())
        {
            return steamPath;
        }

        auto discordPath = Platform::GetFolderPath(SpecialFolder::rct2Discord);
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

    static u8string SelectGogInstaller()
    {
        FileDialogDesc desc{};
        desc.Type = FileDialogType::Open;
        desc.Title = LanguageGetString(STR_SELECT_GOG_INSTALLER);
        desc.Filters.emplace_back(LanguageGetString(STR_GOG_INSTALLER), "*.exe");
        desc.Filters.emplace_back(LanguageGetString(STR_ALL_FILES), "*");

        const auto userHomePath = Platform::GetFolderPath(SpecialFolder::userHome);
        desc.InitialDirectory = userHomePath;

        return ContextOpenCommonFileDialog(desc);
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
        const char* args[] = { path.c_str(),   installerPath.c_str(), "--exclude-temp", "--silent",
                               "--output-dir", targetPath.c_str(),    nullptr };
        int32_t exit_status = Platform::Execute(args, &output);
        LOG_INFO("Exit status %d", exit_status);
        return exit_status == 0;
    }

    bool OpenFromPath(u8string_view path)
    {
        if (!File::Exists(path))
        {
            return false;
        }

        auto result = ReadFile(path);
        if (result)
        {
            CurrencyLoadCustomCurrencyConfig();
        }
        return result;
    }

    u8string GetDefaultPath()
    {
        auto& env = GetContext()->GetPlatformEnvironment();
        return Path::Combine(env.GetDirectoryPath(DirBase::user), u8"config.ini");
    }

    bool SaveToPath(u8string_view path)
    {
        return WriteFile(path);
    }

    bool Save()
    {
        auto path = GetDefaultPath();
        return SaveToPath(path);
    }

    bool FindOrBrowseInstallDirectory()
    {
        std::string path = FindRCT2Path();
        if (!path.empty())
        {
            Get().general.rct2Path = path;
        }
        else
        {
            if (gOpenRCT2Headless)
            {
                return false;
            }

            auto& uiContext = GetContext()->GetUiContext();
            if (!uiContext.HasFilePicker())
            {
                uiContext.ShowMessageBox(LanguageGetString(STR_NEEDS_RCT2_FILES_MANUAL));
                return false;
            }

            try
            {
                const char* g1DatPath = PATH_SEPARATOR "Data" PATH_SEPARATOR "g1.dat";
                while (true)
                {
                    uiContext.ShowMessageBox(LanguageGetString(STR_NEEDS_RCT2_FILES));
                    std::string gog = LanguageGetString(STR_OWN_ON_GOG);
                    std::string steam = LanguageGetString(STR_OWN_ON_STEAM);
                    std::string hdd = LanguageGetString(STR_INSTALLED_ON_HDD);
                    std::string exit = LanguageGetString(STR_QUIT_ONBOARDING);

                    std::string chosenOption;

                    if (uiContext.HasMenuSupport())
                    {
                        std::vector<std::string> options;
                        options.push_back(hdd);
                        options.push_back(gog);
                        options.push_back(steam);
                        options.push_back(exit);
                        int optionIndex = uiContext.ShowMenuDialog(
                            options, LanguageGetString(STR_OPENRCT2_SETUP), LanguageGetString(STR_WHICH_APPLIES_BEST));
                        // Error while trying to show menu options, fall back.
                        if (optionIndex < 0)
                        {
                            chosenOption = hdd;
                        }
                        // User clicked the Cancel or Close button
                        else if (static_cast<uint32_t>(optionIndex) >= options.size())
                        {
                            chosenOption = exit;
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

                    std::vector<std::string> possibleInstallPaths{};
                    if (chosenOption == hdd)
                    {
                        auto pickedPath = uiContext.ShowDirectoryDialog(LanguageGetString(STR_PICK_RCT2_DIR));
                        if (!pickedPath.empty())
                            possibleInstallPaths.emplace_back(pickedPath);
                    }
                    else if (chosenOption == gog)
                    {
                        // Check if innoextract is installed. If not, prompt the user to install it.
                        std::string dummy;
                        if (!Platform::FindApp("innoextract", &dummy))
                        {
                            uiContext.ShowMessageBox(LanguageGetString(STR_INSTALL_INNOEXTRACT));
                            return false;
                        }

                        const std::string dest = Path::Combine(
                            GetContext()->GetPlatformEnvironment().GetDirectoryPath(DirBase::config), "rct2");

                        while (true)
                        {
                            uiContext.ShowMessageBox(LanguageGetString(STR_PLEASE_SELECT_GOG_INSTALLER));
                            auto gogPath = SelectGogInstaller();
                            if (gogPath.empty())
                            {
                                // The user clicked "Cancel", so stop trying.
                                return false;
                            }

                            uiContext.ShowMessageBox(LanguageGetString(STR_THIS_WILL_TAKE_A_FEW_MINUTES));

                            if (ExtractGogInstaller(gogPath, dest))
                                break;

                            uiContext.ShowMessageBox(LanguageGetString(STR_NOT_THE_GOG_INSTALLER));
                        }

                        // New installer extracts to ‘dest’, old installer installs in ‘dest/app’.
                        possibleInstallPaths.emplace_back(dest);
                        possibleInstallPaths.emplace_back(Path::Combine(dest, u8"app"));
                    }
                    else if (chosenOption == steam)
                    {
                        uiContext.ShowMessageBox(LanguageGetString(STR_PLEASE_CLOSE_STEAM));

                        Platform::triggerSteamDownload();

                        uiContext.ShowMessageBox(LanguageGetString(STR_WAIT_FOR_STEAM_DOWNLOAD));

                        auto steamPath = FindRCT2SteamPath();
                        if (!steamPath.empty())
                        {
                            Get().general.rct2Path = steamPath;
                            return true;
                        }
                    }
                    else if (chosenOption == exit)
                    {
                        ContextQuit();
                    }
                    if (possibleInstallPaths.empty())
                    {
                        return false;
                    }

                    for (const auto& possiblePath : possibleInstallPaths)
                    {
                        if (Platform::OriginalGameDataExists(possiblePath))
                        {
                            Get().general.rct2Path = possiblePath;
                            return true;
                        }
                    }

                    uiContext.ShowMessageBox(FormatStringIDLegacy(STR_COULD_NOT_FIND_AT_PATH, &g1DatPath));
                }
            }
            catch (const std::exception& ex)
            {
                Console::Error::WriteLine(ex.what());
                return false;
            }
        }
        // While we're at it, also check if the player has RCT1
        std::string rct1Path = FindRCT1Path();
        if (!rct1Path.empty())
        {
            Get().general.rct1Path = std::move(rct1Path);
        }

        return true;
    }
} // namespace OpenRCT2::Config
