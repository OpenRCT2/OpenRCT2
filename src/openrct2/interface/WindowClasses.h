/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    mainWindow = 0,
    topToolbar = 1,
    bottomToolbar = 2,
    tooltip = 5,
    dropdown = 6,
    about = 8,
    // WC_PUBLISHER_CREDITS = 9,
    // WC_MUSIC_CREDITS = 10,
    error = 11,
    ride = 12,
    rideConstruction = 13,
    savePrompt = 14,
    rideList = 15,
    constructRide = 16,
    demolishRidePrompt = 17,
    scenery = 18,
    options = 19,
    footpath = 20,
    land = 21,
    water = 22,
    peep = 23,
    guestList = 24,
    staffList = 25,
    firePrompt = 26,
    parkInformation = 27,
    finances = 28,
    titleMenu = 29,
    titleExit = 30,
    recentNews = 31,
    scenarioSelect = 32,
    trackDesignList = 33,
    trackDesignPlace = 34,
    newCampaign = 35,
    keyboardShortcutList = 36,
    changeKeyboardShortcut = 37,
    map = 38,
    titleLogo = 39,
    banner = 40,
    mapTooltip = 41,
    editorObjectSelection = 42,
    editorInventionList = 43,
    editorInventionListDrag = 44,
    editorScenarioOptions = 45,
    manageTrackDesign = 47,
    trackDeletePrompt = 48,
    installTrack = 49,
    clearScenery = 50,
    sceneryScatter = 51,

    cheats = 110,
    research = 111,
    viewport = 112,
    textinput = 113,
    mapgen = 114,
    loadsave = 115,
    loadsaveOverwritePrompt = 116,
    titleOptions = 117,
    landRights = 118,
    themes = 119,
    tileInspector = 120,
    changelog = 121,
    multiplayer = 124,
    player = 125,
    networkStatus = 126,
    serverList = 127,
    serverStart = 128,
    customCurrencyConfig = 129,
    debugPaint = 130,
    viewClipping = 131,
    objectLoadError = 132,
    patrolArea = 133,
    transparency = 134,
    assetPacks = 135,
    resetShortcutKeysPrompt = 136,
    progressWindow = 137,
    titleVersion = 138,
    editorParkEntrance = 139,

    // Only used for colour schemes
    staff = 220,
    editorTrackBottomToolbar = 221,
    editorScenarioBottomToolbar = 222,
    chat = 223,
    console = 224,
    custom = 225,

    null = 255,
};
