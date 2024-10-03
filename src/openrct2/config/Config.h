/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"
#include "../localisation/CurrencyTypes.h"
#include "ConfigTypes.h"

#include <atomic>

// windows.h defines an interface keyword
#ifdef interface
    #undef interface
#endif

namespace OpenRCT2::Config
{
    struct General
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
        bool InferDisplayDPI;
        ::DrawingEngine DrawingEngine;
        bool UncapFPS;
        bool UseVSync;
        bool ShowFPS;
        std::atomic_uint8_t MultiThreading;
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
        u8string CustomCurrencySymbol;

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
        bool ShowRealNamesOfStaff;
        bool AllowEarlyCompletion;
        u8string AssetPackOrder;
        u8string EnabledAssetPacks;

        // Loading and saving
        bool ConfirmationPrompt;
        FileBrowserSort LoadSaveSort;
        u8string LastSaveGameDirectory;
        u8string LastSaveLandscapeDirectory;
        u8string LastSaveScenarioDirectory;
        u8string LastSaveTrackDirectory;
        u8string LastRunVersion;
        bool UseNativeBrowseDialog;
        int64_t LastVersionCheckTime;
        int16_t FileBrowserWidth;
        int16_t FileBrowserHeight;
        bool FileBrowserShowSizeColumn;
        bool FileBrowserShowDateColumn;
    };

    struct Interface
    {
        bool ToolbarButtonsCentred;
        bool ToolbarShowFinances;
        bool ToolbarShowResearch;
        bool ToolbarShowCheats;
        bool ToolbarShowNews;
        bool ToolbarShowMute;
        bool ToolbarShowChat;
        bool ToolbarShowZoom;
        bool ToolbarShowRotateAnticlockwise;
        bool ConsoleSmallFont;
        bool RandomTitleSequence;
        u8string CurrentThemePreset;
        u8string CurrentTitleSequencePreset;
        int32_t ObjectSelectionFilterFlags;
        int32_t ScenarioselectLastTab;
        bool ListRideVehiclesSeparately;
        bool WindowButtonsOnTheLeft;
        bool EnlargedUi;
        bool TouchEnhancements;
    };

    struct Sound
    {
        u8string Device;
        bool MasterSoundEnabled;
        uint8_t MasterVolume;
        TitleMusicKind TitleMusic;
        bool SoundEnabled;
        uint8_t SoundVolume;
        bool RideMusicEnabled;
        uint8_t AudioFocus;
        bool audio_focus;
    };

    struct Network
    {
        u8string PlayerName;
        int32_t DefaultPort;
        u8string ListenAddress;
        u8string DefaultPassword;
        bool StayConnected;
        bool Advertise;
        u8string AdvertiseAddress;
        int32_t Maxplayers;
        u8string ServerName;
        u8string ServerDescription;
        u8string ServerGreeting;
        u8string MasterServerUrl;
        u8string ProviderName;
        u8string ProviderEmail;
        u8string ProviderWebsite;
        bool KnownKeysOnly;
        bool LogChat;
        bool LogServerActions;
        bool PauseServerIfNoClients;
        bool DesyncDebugging;
    };

    struct Notification
    {
        bool ParkAward;
        bool ParkMarketingCampaignFinished;
        bool ParkWarnings;
        bool ParkRatingWarnings;
        bool RideBrokenDown;
        bool RideCrashed;
        bool RideCasualties;
        bool RideWarnings;
        bool RideResearched;
        bool RideStalledVehicles;
        bool GuestWarnings;
        bool GuestLeftPark;
        bool GuestQueuingForRide;
        bool GuestOnRide;
        bool GuestLeftRide;
        bool GuestBoughtItem;
        bool GuestUsedFacility;
        bool GuestDied;
    };

    struct Font
    {
        u8string FileName;
        u8string FontName;
        int32_t OffsetX;
        int32_t OffsetY;
        int32_t SizeTiny;
        int32_t SizeSmall;
        int32_t SizeMedium;
        int32_t SizeBig;
        int32_t HeightTiny;
        int32_t HeightSmall;
        int32_t HeightMedium;
        int32_t HeightBig;
        bool EnableHinting;
        int32_t HintingThreshold;
    };

    struct Plugin
    {
        bool EnableHotReloading;
        u8string AllowedHosts;
    };

    struct Config
    {
        Config() = default;

        // Prevent accidental copies
        Config(const Config&) = delete;

        General general;
        Interface interface;
        Sound sound;
        Network network;
        Notification notifications;
        Font fonts;
        Plugin plugin;
    };

    Config& Get();
    bool OpenFromPath(u8string_view path);
    bool SaveToPath(u8string_view path);
    u8string GetDefaultPath();
    bool SetDefaults();
    bool Save();
    bool FindOrBrowseInstallDirectory();
} // namespace OpenRCT2::Config
