/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
    u8string RCT1Path;
    u8string RCT2Path;

    // Display
    int32_t DefaultDisplay;
    int32_t WindowWidth;
    int32_t WindowHeight;
    int32_t FullscreenMode;
    int32_t FullscreenWidth;
    int32_t FullscreenHeight;
    float WindowScale;
    ::DrawingEngine DrawingEngine;
    bool UncapFPS;
    bool UseVSync;
    bool ShowFPS;
    bool MultiThreading;
    bool MinimizeFullscreenFocusLoss;
    bool DisableScreensaver;

    // Map rendering
    bool LandscapeSmoothing;
    bool AlwaysShowGridlines;
    VirtualFloorStyles VirtualFloorStyle;
    bool DayNightCycle;
    bool EnableLightFx;
    bool EnableLightFxForVehicles;
    bool UpperCaseBanners;
    bool RenderWeatherEffects;
    bool RenderWeatherGloom;
    bool DisableLightningEffect;
    bool ShowGuestPurchases;
    bool TransparentScreenshot;
    bool TransparentWater;

    bool InvisibleRides;
    bool InvisibleVehicles;
    bool InvisibleTrees;
    bool InvisibleScenery;
    bool InvisiblePaths;
    bool InvisibleSupports;

    // Localisation
    int32_t Language;
    ::MeasurementFormat MeasurementFormat;
    TemperatureUnit TemperatureFormat;
    bool ShowHeightAsUnits;
    int32_t DateFormat;
    CurrencyType CurrencyFormat;
    int32_t CustomCurrencyRate;
    CurrencyAffix CustomCurrencyAffix;
    utf8* CustomCurrencySymbol;

    // Controls
    bool EdgeScrolling;
    int32_t EdgeScrollingSpeed;
    bool TrapCursor;
    bool InvertViewportDrag;
    bool ZoomToCursor;

    // Miscellaneous
    bool PlayIntro;
    int32_t WindowSnapProximity;
    bool SavePluginData;
    bool DebuggingTools;
    int32_t AutosaveFrequency;
    int32_t AutosaveAmount;
    bool AutoStaffPlacement;
    bool HandymenMowByDefault;
    bool AutoOpenShops;
    int32_t DefaultInspectionInterval;
    int32_t WindowLimit;
    int32_t ScenarioSelectMode;
    bool ScenarioUnlockingEnabled;
    bool ScenarioHideMegaPark;
    bool SteamOverlayPause;
    bool ShowRealNamesOfGuests;
    bool AllowEarlyCompletion;
    u8string AssetPackOrder;
    u8string EnabledAssetPacks;

    // Loading and saving
    bool ConfirmationPrompt;
    Sort LoadSaveSort;
    u8string LastSaveGameDirectory;
    u8string LastSaveLandscapeDirectory;
    u8string LastSaveScenarioDirectory;
    u8string LastSaveTrackDirectory;
    u8string LastRunVersion;
    bool UseNativeBrowseDialog;
    int64_t LastVersionCheckTime;
};

struct InterfaceConfiguration
{
    bool ToolbarShowFinances;
    bool ToolbarShowResearch;
    bool ToolbarShowCheats;
    bool ToolbarShowNews;
    bool ToolbarShowMute;
    bool ToolbarShowChat;
    bool ToolbarShowZoom;
    bool ConsoleSmallFont;
    bool RandomTitleSequence;
    utf8* CurrentThemePreset;
    utf8* CurrentTitleSequencePreset;
    int32_t ObjectSelectionFilterFlags;
    int32_t ScenarioselectLastTab;
    bool ListRideVehiclesSeparately;
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
