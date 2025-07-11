/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

enum class WindowClass : uint8_t
{
    MainWindow = 0,
    TopToolbar = 1,
    BottomToolbar = 2,
    Tooltip = 5,
    Dropdown = 6,
    About = 8,
    // WC_PUBLISHER_CREDITS = 9,
    // WC_MUSIC_CREDITS = 10,
    Error = 11,
    Ride = 12,
    RideConstruction = 13,
    SavePrompt = 14,
    RideList = 15,
    ConstructRide = 16,
    DemolishRidePrompt = 17,
    Scenery = 18,
    Options = 19,
    Footpath = 20,
    Land = 21,
    Water = 22,
    Peep = 23,
    GuestList = 24,
    StaffList = 25,
    FirePrompt = 26,
    ParkInformation = 27,
    Finances = 28,
    TitleMenu = 29,
    TitleExit = 30,
    RecentNews = 31,
    ScenarioSelect = 32,
    TrackDesignList = 33,
    TrackDesignPlace = 34,
    NewCampaign = 35,
    KeyboardShortcutList = 36,
    ChangeKeyboardShortcut = 37,
    Map = 38,
    TitleLogo = 39,
    Banner = 40,
    MapTooltip = 41,
    EditorObjectSelection = 42,
    EditorInventionList = 43,
    EditorInventionListDrag = 44,
    EditorScenarioOptions = 45,
    ManageTrackDesign = 47,
    TrackDeletePrompt = 48,
    InstallTrack = 49,
    ClearScenery = 50,
    SceneryScatter = 51,

    Cheats = 110,
    Research = 111,
    Viewport = 112,
    Textinput = 113,
    Mapgen = 114,
    Loadsave = 115,
    LoadsaveOverwritePrompt = 116,
    TitleOptions = 117,
    LandRights = 118,
    Themes = 119,
    TileInspector = 120,
    Changelog = 121,
    Multiplayer = 124,
    Player = 125,
    NetworkStatus = 126,
    ServerList = 127,
    ServerStart = 128,
    CustomCurrencyConfig = 129,
    DebugPaint = 130,
    ViewClipping = 131,
    ObjectLoadError = 132,
    PatrolArea = 133,
    Transparency = 134,
    AssetPacks = 135,
    ResetShortcutKeysPrompt = 136,
    ProgressWindow = 137,
    TitleVersion = 138,
    EditorParkEntrance = 139,

    // Only used for colour schemes
    Staff = 220,
    EditorTrackBottomToolbar = 221,
    EditorScenarioBottomToolbar = 222,
    Chat = 223,
    Console = 224,
    Custom = 225,

    Null = 255,
};
