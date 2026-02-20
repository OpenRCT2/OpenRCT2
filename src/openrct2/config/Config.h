/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"
#include "ConfigTypes.h"

#include <atomic>

// windows.h defines an interface keyword
#ifdef interface
    #undef interface
#endif

enum class RideInspection : uint8_t;

namespace OpenRCT2::Config
{
    struct General
    {
        // Paths
        u8string rct1Path;
        u8string rct2Path;

        // Display
        int32_t defaultDisplay;
        int32_t windowWidth;
        int32_t windowHeight;
        int32_t fullscreenMode;
        int32_t fullscreenWidth;
        int32_t fullscreenHeight;
        float windowScale;
        bool inferDisplayDPI;
        ::DrawingEngine drawingEngine;
        bool uncapFPS;
        bool useVSync;
        bool showFPS;
        std::atomic_uint8_t multiThreading;
        bool minimizeFullscreenFocusLoss;
        bool disableScreensaver;

        // Map rendering
        bool landscapeSmoothing;
        bool alwaysShowGridlines;
        VirtualFloorStyles virtualFloorStyle;
        bool dayNightCycle;
        bool enableLightFx;
        bool enableLightFxForVehicles;
        bool upperCaseBanners;
        bool renderWeatherEffects;
        bool renderWeatherGloom;
        bool disableLightningEffect;
        bool showGuestPurchases;
        bool transparentScreenshot;
        bool transparentWater;

        bool invisibleRides;
        bool invisibleVehicles;
        bool invisibleTrees;
        bool invisibleScenery;
        bool invisiblePaths;
        bool invisibleSupports;

        // Localisation
        int32_t language;
        MeasurementFormat measurementFormat;
        TemperatureUnit temperatureFormat;
        bool showHeightAsUnits;
        int32_t dateFormat;
        CurrencyType currencyFormat;
        int32_t customCurrencyRate;
        CurrencyAffix customCurrencyAffix;
        u8string customCurrencySymbol;

        // Controls
        bool edgeScrolling;
        int32_t edgeScrollingSpeed;
        bool trapCursor;
        bool invertViewportDrag;
        bool zoomToCursor;

        // Gamepad
        int32_t gamepadDeadzone;
        float gamepadSensitivity;

        // Miscellaneous
        bool playIntro;
        int32_t windowSnapProximity;
        bool savePluginData;
        bool debuggingTools;
        int32_t autosaveFrequency;
        int32_t autosaveAmount;
        bool autoStaffPlacement;
        bool handymenMowByDefault;
        bool autoOpenShops;
        RideInspection defaultInspectionInterval;
        int32_t windowLimit;
        bool scenarioUnlockingEnabled;
        bool scenarioHideMegaPark;
        bool showRealNamesOfGuests;
        bool showRealNamesOfStaff;
        bool allowEarlyCompletion;
        u8string assetPackOrder;
        u8string enabledAssetPacks;

        // Loading and saving
        bool confirmationPrompt;
        FileBrowserSort loadSaveSort;
        u8string lastSaveGameDirectory;
        u8string lastSaveLandscapeDirectory;
        u8string lastSaveScenarioDirectory;
        u8string lastSaveTrackDirectory;
        u8string lastRunVersion;
        bool useNativeBrowseDialog;
        int64_t lastVersionCheckTime;
        int16_t fileBrowserWidth;
        int16_t fileBrowserHeight;
        bool fileBrowserShowSizeColumn;
        bool fileBrowserShowDateColumn;
        ParkPreviewPref fileBrowserPreviewType;
    };

    struct Interface
    {
        bool toolbarButtonsCentred;
        bool toolbarShowFinances;
        bool toolbarShowResearch;
        bool toolbarShowCheats;
        bool toolbarShowNews;
        bool toolbarShowMute;
        bool toolbarShowChat;
        bool toolbarShowZoom;
        bool toolbarShowRotateAnticlockwise;
        bool consoleSmallFont;
        bool randomTitleSequence;
        u8string currentThemePreset;
        u8string currentTitleSequencePreset;
        int32_t objectSelectionFilterFlags;
        int32_t scenarioSelectLastTab;
        bool scenarioPreviewScreenshots;
        bool listRideVehiclesSeparately;
        bool windowButtonsOnTheLeft;
        bool enlargedUi;
        bool touchEnhancements;
    };

    struct Sound
    {
        u8string device;
        bool masterSoundEnabled;
        uint8_t masterVolume;
        TitleMusicKind titleMusic;
        bool soundEnabled;
        uint8_t soundVolume;
        bool rideMusicEnabled;
        uint8_t rideMusicVolume;
        bool audioFocus;
    };

    struct Network
    {
        u8string playerName;
        int32_t defaultPort;
        u8string listenAddress;
        u8string defaultPassword;
        bool stayConnected;
        bool advertise;
        u8string advertiseAddress;
        int32_t maxplayers;
        u8string serverName;
        u8string serverDescription;
        u8string serverGreeting;
        u8string masterServerUrl;
        u8string providerName;
        u8string providerEmail;
        u8string providerWebsite;
        bool knownKeysOnly;
        bool logChat;
        bool logServerActions;
        bool pauseServerIfNoClients;
        bool desyncDebugging;
    };

    struct Notification
    {
        bool parkAward;
        bool parkMarketingCampaignFinished;
        bool parkWarnings;
        bool parkRatingWarnings;
        bool rideBrokenDown;
        bool rideCrashed;
        bool rideCasualties;
        bool rideWarnings;
        bool rideResearched;
        bool rideStalledVehicles;
        bool guestWarnings;
        bool guestLeftPark;
        bool guestQueuingForRide;
        bool guestOnRide;
        bool guestLeftRide;
        bool guestBoughtItem;
        bool guestUsedFacility;
        bool guestDied;
    };

    struct Font
    {
        u8string fileName;
        u8string fontName;
        int32_t offsetX;
        int32_t offsetY;
        int32_t sizeTiny;
        int32_t sizeSmall;
        int32_t sizeMedium;
        int32_t sizeBig;
        int32_t heightTiny;
        int32_t heightSmall;
        int32_t heightMedium;
        int32_t heightBig;
        bool enableHinting;
        int32_t hintingThreshold;
    };

    struct Plugin
    {
        bool enableHotReloading;
        u8string allowedHosts;
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
