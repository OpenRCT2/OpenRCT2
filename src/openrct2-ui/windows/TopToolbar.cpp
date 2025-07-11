/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiContext.h"
#include "../interface/InGameConsole.h"
#include "../scripting/CustomMenu.h"

#include <iterator>
#include <limits>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Cheats.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/Version.h>
#include <openrct2/actions/GameSetSpeedAction.h>
#include <openrct2/actions/LoadOrQuitAction.h>
#include <openrct2/actions/PauseToggleAction.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Numerics.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/entity/Staff.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/Screenshot.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/network/Network.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <string>

using namespace OpenRCT2::Numerics;

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WIDX_PAUSE,
        WIDX_FILE_MENU,
        WIDX_MUTE,
        WIDX_ZOOM_OUT,
        WIDX_ZOOM_IN,
        WIDX_ROTATE_ANTI_CLOCKWISE,
        WIDX_ROTATE_CLOCKWISE,
        WIDX_VIEW_MENU,
        WIDX_MAP,

        WIDX_LAND,
        WIDX_WATER,
        WIDX_SCENERY,
        WIDX_PATH,
        WIDX_CONSTRUCT_RIDE,
        WIDX_RIDES,
        WIDX_PARK,
        WIDX_STAFF,
        WIDX_GUESTS,
        WIDX_CLEAR_SCENERY,

        WIDX_FASTFORWARD,
        WIDX_CHEATS,
        WIDX_DEBUG,
        WIDX_FINANCES,
        WIDX_RESEARCH,
        WIDX_NEWS,
        WIDX_NETWORK,
        WIDX_CHAT,

        WIDX_SEPARATOR,
    };

    enum FileMenuDdidx
    {
        DDIDX_NEW_GAME = 0,
        DDIDX_LOAD_GAME = 1,
        // separator
        DDIDX_SAVE_GAME = 3,
        DDIDX_SAVE_GAME_AS = 4,
        // separator
        DDIDX_SCREENSHOT = 6,
        DDIDX_GIANT_SCREENSHOT = 7,
        // separator
        DDIDX_ABOUT = 9,
        DDIDX_FILE_BUG_ON_GITHUB = 10,
        DDIDX_UPDATE_AVAILABLE = 11,
        DDIDX_OPTIONS = 12,
        // separator
        DDIDX_QUIT_TO_MENU = 14,
        DDIDX_EXIT_OPENRCT2 = 15,
    };

    enum TopToolbarViewMenuDdidx
    {
        DDIDX_UNDERGROUND_INSIDE = 0,
        DDIDX_TRANSPARENT_WATER = 1,
        DDIDX_HIDE_BASE = 2,
        DDIDX_HIDE_VERTICAL = 3,
        // separator
        DDIDX_HIDE_RIDES = 5,
        DDIDX_HIDE_VEHICLES = 6,
        DDIDX_HIDE_VEGETATION = 7,
        DDIDX_HIDE_SCENERY = 8,
        DDIDX_HIDE_PATHS = 9,
        DDIDX_HIDE_SUPPORTS = 10,
        DDIDX_HIDE_GUESTS = 11,
        DDIDX_HIDE_STAFF = 12,
        // separator
        DDIDX_LAND_HEIGHTS = 14,
        DDIDX_TRACK_HEIGHTS = 15,
        DDIDX_PATH_HEIGHTS = 16,
        // separator
        DDIDX_VIEW_CLIPPING = 18,
        DDIDX_HIGHLIGHT_PATH_ISSUES = 19,
        // separator
        DDIDX_TRANSPARENCY = 21,

        TOP_TOOLBAR_VIEW_MENU_COUNT,
    };

    enum TopToolbarDebugDdidx
    {
        DDIDX_CONSOLE = 0,
        DDIDX_DEBUG_PAINT = 1,

        TOP_TOOLBAR_DEBUG_COUNT,
    };

    enum TopToolbarNetworkDdidx
    {
        DDIDX_MULTIPLAYER = 0,
        DDIDX_MULTIPLAYER_RECONNECT = 1,

        TOP_TOOLBAR_NETWORK_COUNT,
    };

    enum
    {
        DDIDX_CHEATS,
        DDIDX_TILE_INSPECTOR = 1,
        DDIDX_OBJECT_SELECTION = 2,
        DDIDX_INVENTIONS_LIST = 3,
        DDIDX_SCENARIO_OPTIONS = 4,
        // 5 is a separator
        DDIDX_ENABLE_SANDBOX_MODE = 6,
        DDIDX_DISABLE_CLEARANCE_CHECKS = 7,
        DDIDX_DISABLE_SUPPORT_LIMITS = 8,

        TOP_TOOLBAR_CHEATS_COUNT,
    };

    enum
    {
        DDIDX_SHOW_MAP,
        DDIDX_OPEN_VIEWPORT,
    };

    enum
    {
        DDIDX_ROTATE_CLOCKWISE,
        DDIDX_ROTATE_ANTI_CLOCKWISE,
    };

#pragma region Toolbar_widget_ordering

    // clang-format off
    static constexpr std::array kWidgetOrderLeftGroup = {
        WIDX_PAUSE,
        WIDX_FASTFORWARD,
        WIDX_FILE_MENU,
        WIDX_MUTE,
        WIDX_NETWORK,
        WIDX_CHAT,
        WIDX_CHEATS,
        WIDX_DEBUG,

        WIDX_SEPARATOR,

        WIDX_ZOOM_OUT,
        WIDX_ZOOM_IN,
        WIDX_ROTATE_ANTI_CLOCKWISE,
        WIDX_ROTATE_CLOCKWISE,
        WIDX_VIEW_MENU,
        WIDX_MAP,
    };

    static constexpr std::array kWidgetOrderRightGroup = {
        WIDX_CLEAR_SCENERY,
        WIDX_LAND,
        WIDX_WATER,
        WIDX_SCENERY,
        WIDX_PATH,
        WIDX_CONSTRUCT_RIDE,

        WIDX_SEPARATOR,

        WIDX_FINANCES,
        WIDX_RESEARCH,
        WIDX_RIDES,
        WIDX_PARK,
        WIDX_STAFF,
        WIDX_GUESTS,
        WIDX_NEWS,
    };

    static constexpr size_t _totalToolbarElements = kWidgetOrderLeftGroup.size() + kWidgetOrderRightGroup.size();

    // Make a combined version of both halves of the toolbar, with a separator halfway.
    static constexpr std::array<int, _totalToolbarElements + 1> kWidgetOrderCombined = []() {
        std::array<int, _totalToolbarElements + 1> combined;

        auto halfWayPoint = std::copy(kWidgetOrderLeftGroup.begin(), kWidgetOrderLeftGroup.end(), combined.begin());
        *halfWayPoint = WIDX_SEPARATOR;
        std::copy(kWidgetOrderRightGroup.begin(), kWidgetOrderRightGroup.end(), halfWayPoint + 1);

        return combined;
    }();

#pragma endregion

    static constexpr auto _topToolbarWidgets = makeWidgets(
        makeRemapWidget({  0, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::primary   , SPR_TOOLBAR_PAUSE,          STR_PAUSE_GAME_TIP                ), // Pause
        makeRemapWidget({ 60, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::primary   , SPR_TOOLBAR_FILE,           STR_DISC_AND_GAME_OPTIONS_TIP     ), // File menu
        makeRemapWidget({250, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::primary   , SPR_G2_TOOLBAR_MUTE,        STR_TOOLBAR_MUTE_TIP              ), // Mute
        makeRemapWidget({100, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::secondary , SPR_TOOLBAR_ZOOM_OUT,       STR_ZOOM_OUT_TIP                  ), // Zoom out
        makeRemapWidget({130, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::secondary , SPR_TOOLBAR_ZOOM_IN,        STR_ZOOM_IN_TIP                   ), // Zoom in
        makeRemapWidget({190, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::secondary , SPR_TAB_TOOLBAR,            STR_ROTATE_ANTI_CLOCKWISE         ), // Rotate camera anti-clockwise
        makeRemapWidget({160, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::secondary , SPR_TOOLBAR_ROTATE,         STR_ROTATE_CLOCKWISE              ), // Rotate camera clockwise
        makeRemapWidget({220, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::secondary , SPR_TOOLBAR_VIEW,           STR_VIEW_OPTIONS_TIP              ), // Transparency menu
        makeRemapWidget({267, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::secondary , SPR_TOOLBAR_MAP,            STR_SHOW_MAP_TIP                  ), // Map
        makeRemapWidget({297, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::tertiary  , SPR_TOOLBAR_LAND,           STR_ADJUST_LAND_TIP               ), // Land
        makeRemapWidget({327, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::tertiary  , SPR_TOOLBAR_WATER,          STR_ADJUST_WATER_TIP              ), // Water
        makeRemapWidget({357, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::tertiary  , SPR_TOOLBAR_SCENERY,        STR_PLACE_SCENERY_TIP             ), // Scenery
        makeRemapWidget({387, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::tertiary  , SPR_TOOLBAR_FOOTPATH,       STR_BUILD_FOOTPATH_TIP            ), // Path
        makeRemapWidget({490, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::tertiary  , SPR_TOOLBAR_CONSTRUCT_RIDE, STR_BUILD_RIDE_TIP                ), // Construct ride
        makeRemapWidget({520, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::quaternary, SPR_TOOLBAR_RIDES,          STR_RIDES_IN_PARK_TIP             ), // Rides
        makeRemapWidget({550, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::quaternary, SPR_TOOLBAR_PARK,           STR_PARK_INFORMATION_TIP          ), // Park
        makeRemapWidget({560, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::quaternary, SPR_TAB_TOOLBAR,            STR_STAFF_TIP                     ), // Staff
        makeRemapWidget({560, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::quaternary, SPR_TOOLBAR_GUESTS,         STR_GUESTS_TIP                    ), // Guests
        makeRemapWidget({560, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::tertiary  , SPR_TOOLBAR_CLEAR_SCENERY,  STR_CLEAR_SCENERY_TIP             ), // Clear scenery
        makeRemapWidget({ 30, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::primary   , SPR_TAB_TOOLBAR,            STR_GAME_SPEED_TIP                ), // Fast forward
        makeRemapWidget({ 30, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::primary   , SPR_TAB_TOOLBAR,            STR_CHEATS_TIP                    ), // Cheats
        makeRemapWidget({ 30, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::primary   , SPR_TAB_TOOLBAR,            STR_DEBUG_TIP                     ), // Debug
        makeRemapWidget({ 30, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::quaternary, SPR_TAB_TOOLBAR,            STR_SCENARIO_OPTIONS_FINANCIAL_TIP), // Finances
        makeRemapWidget({ 30, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::quaternary, SPR_TAB_TOOLBAR,            STR_FINANCES_RESEARCH_TIP         ), // Research
        makeRemapWidget({ 30, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::quaternary, SPR_TAB_TOOLBAR,            STR_SHOW_RECENT_MESSAGES_TIP      ), // News
        makeRemapWidget({ 30, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::primary   , SPR_G2_TOOLBAR_MULTIPLAYER, STR_SHOW_MULTIPLAYER_STATUS_TIP   ), // Network
        makeRemapWidget({ 30, 0}, {30, kTopToolbarHeight + 1}, WidgetType::trnBtn, WindowColour::primary   , SPR_TAB_TOOLBAR,            STR_TOOLBAR_CHAT_TIP              ), // Chat
        makeWidget     ({  0, 0}, {10,                     1}, WidgetType::empty,  WindowColour::primary                                                                   )  // Artificial widget separator
    );
    // clang-format on

    class TopToolbar final : public Window
    {
    private:
        bool _waitingForPause{ false };

        void InitViewMenu(Widget& widget);

        void ViewMenuDropdown(int16_t dropdownIndex);

        void InitMapMenu(Widget& widget);

        void MapMenuDropdown(int16_t dropdownIndex);

        void InitFastforwardMenu(Widget& widget);

        void FastforwardMenuDropdown(int16_t dropdownIndex);

        void InitFileMenu(Widget& widget);

        void InitCheatsMenu(Widget& widget);

        void CheatsMenuDropdown(int16_t dropdownIndex);

        void InitDebugMenu(Widget& widget);

        void DebugMenuDropdown(int16_t dropdownIndex);

        void InitNetworkMenu(Widget& widget);

        void NetworkMenuDropdown(int16_t dropdownIndex);

    public:
        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            WindowBase* mainWindow;

            switch (widgetIndex)
            {
                case WIDX_PAUSE:
                    if (NetworkGetMode() != NETWORK_MODE_CLIENT)
                    {
                        auto pauseToggleAction = PauseToggleAction();
                        GameActions::Execute(&pauseToggleAction);
                        _waitingForPause = true;
                    }
                    break;
                case WIDX_ZOOM_OUT:
                    if ((mainWindow = WindowGetMain()) != nullptr)
                        WindowZoomOut(*mainWindow, false);
                    break;
                case WIDX_ZOOM_IN:
                    if ((mainWindow = WindowGetMain()) != nullptr)
                        WindowZoomIn(*mainWindow, false);
                    break;
                case WIDX_ROTATE_CLOCKWISE:
                    ViewportRotateAll(1);
                    break;
                case WIDX_ROTATE_ANTI_CLOCKWISE:
                    ViewportRotateAll(-1);
                    break;
                case WIDX_CLEAR_SCENERY:
                    ToggleClearSceneryWindow();
                    break;
                case WIDX_LAND:
                    ToggleLandWindow();
                    break;
                case WIDX_WATER:
                    ToggleWaterWindow();
                    break;
                case WIDX_SCENERY:
                    ToggleSceneryWindow();
                    break;
                case WIDX_PATH:
                    ToggleFootpathWindow();
                    break;
                case WIDX_CONSTRUCT_RIDE:
                    ContextOpenWindow(WindowClass::ConstructRide);
                    break;
                case WIDX_RIDES:
                    ContextOpenWindow(WindowClass::RideList);
                    break;
                case WIDX_PARK:
                    ContextOpenWindow(WindowClass::ParkInformation);
                    break;
                case WIDX_STAFF:
                    ContextOpenWindow(WindowClass::StaffList);
                    break;
                case WIDX_GUESTS:
                    ContextOpenWindow(WindowClass::GuestList);
                    break;
                case WIDX_FINANCES:
                    ContextOpenWindow(WindowClass::Finances);
                    break;
                case WIDX_RESEARCH:
                    ContextOpenWindow(WindowClass::Research);
                    break;
                case WIDX_NEWS:
                    ContextOpenWindow(WindowClass::RecentNews);
                    break;
                case WIDX_MUTE:
                    OpenRCT2::Audio::ToggleAllSounds();
                    break;
                case WIDX_CHAT:
                    if (ChatAvailable())
                    {
                        ChatToggle();
                    }
                    else
                    {
                        ContextShowError(STR_CHAT_UNAVAILABLE, kStringIdNone, {});
                    }
                    break;
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            Widget& widget = widgets[widgetIndex];

            switch (widgetIndex)
            {
                case WIDX_FILE_MENU:
                    InitFileMenu(widget);
                    break;
                case WIDX_CHEATS:
                    InitCheatsMenu(widget);
                    break;
                case WIDX_VIEW_MENU:
                    InitViewMenu(widget);
                    break;
                case WIDX_MAP:
                    InitMapMenu(widget);
                    break;
                case WIDX_FASTFORWARD:
                    InitFastforwardMenu(widget);
                    break;
                case WIDX_DEBUG:
                    InitDebugMenu(widget);
                    break;
                case WIDX_NETWORK:
                    InitNetworkMenu(widget);
                    break;
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (selectedIndex == -1)
            {
                return;
            }
            switch (widgetIndex)
            {
                case WIDX_FILE_MENU:

                    // New game is only available in the normal game. Skip one position to avoid incorrect mappings in the menus
                    // of the other modes.
                    if (gLegacyScene == LegacyScene::scenarioEditor)
                        selectedIndex += 1;

                    // Quicksave is only available in the normal game. Skip one position to avoid incorrect mappings in the
                    // menus of the other modes.
                    if (gLegacyScene == LegacyScene::scenarioEditor && selectedIndex > DDIDX_LOAD_GAME)
                        selectedIndex += 1;

                    // Track designer and track designs manager start with Screenshot, not Load/save
                    if (isInTrackDesignerOrManager())
                        selectedIndex += DDIDX_SCREENSHOT;

                    // The "Update available" menu item is only available when there is one
                    if (selectedIndex >= DDIDX_UPDATE_AVAILABLE && !OpenRCT2::GetContext()->HasNewVersionInfo())
                        selectedIndex += 1;

                    switch (selectedIndex)
                    {
                        case DDIDX_NEW_GAME:
                        {
                            auto loadOrQuitAction = LoadOrQuitAction(
                                LoadOrQuitModes::OpenSavePrompt, PromptMode::saveBeforeNewGame);
                            GameActions::Execute(&loadOrQuitAction);
                            break;
                        }
                        case DDIDX_LOAD_GAME:
                        {
                            auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::OpenSavePrompt);
                            GameActions::Execute(&loadOrQuitAction);
                            break;
                        }
                        case DDIDX_SAVE_GAME:
                            ToolCancel();
                            SaveGame();
                            break;
                        case DDIDX_SAVE_GAME_AS:
                            if (gLegacyScene == LegacyScene::scenarioEditor)
                            {
                                auto intent = Intent(WindowClass::Loadsave);
                                intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::save);
                                intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::landscape);
                                intent.PutExtra(INTENT_EXTRA_PATH, getGameState().scenarioName);
                                ContextOpenIntent(&intent);
                            }
                            else
                            {
                                ToolCancel();
                                SaveGameAs();
                            }
                            break;
                        case DDIDX_ABOUT:
                            ContextOpenWindow(WindowClass::About);
                            break;
                        case DDIDX_OPTIONS:
                            ContextOpenWindow(WindowClass::Options);
                            break;
                        case DDIDX_SCREENSHOT:
                            gScreenshotCountdown = 10;
                            break;
                        case DDIDX_GIANT_SCREENSHOT:
                            ScreenshotGiant();
                            break;
                        case DDIDX_FILE_BUG_ON_GITHUB:
                        {
                            std::string url = "https://github.com/OpenRCT2/OpenRCT2/issues/new?"
                                              "assignees=&labels=bug&template=bug_report.yaml";
                            // Automatically fill the "OpenRCT2 build" input
                            auto versionStr = String::urlEncode(gVersionInfoFull);
                            url.append("&f299dd2a20432827d99b648f73eb4649b23f8ec98d158d6f82b81e43196ee36b=" + versionStr);
                            OpenRCT2::GetContext()->GetUiContext().OpenURL(url);
                        }
                        break;
                        case DDIDX_UPDATE_AVAILABLE:
                            ContextOpenWindowView(WV_NEW_VERSION_INFO);
                            break;
                        case DDIDX_QUIT_TO_MENU:
                        {
                            auto* windowMgr = Ui::GetWindowManager();
                            windowMgr->CloseByClass(WindowClass::ManageTrackDesign);
                            windowMgr->CloseByClass(WindowClass::TrackDeletePrompt);
                            auto loadOrQuitAction = LoadOrQuitAction(
                                LoadOrQuitModes::OpenSavePrompt, PromptMode::saveBeforeQuit);
                            GameActions::Execute(&loadOrQuitAction);
                            break;
                        }
                        case DDIDX_EXIT_OPENRCT2:
                            ContextQuit();
                            break;
                    }
                    break;
                case WIDX_CHEATS:
                    CheatsMenuDropdown(selectedIndex);
                    break;
                case WIDX_VIEW_MENU:
                    ViewMenuDropdown(selectedIndex);
                    break;
                case WIDX_MAP:
                    MapMenuDropdown(selectedIndex);
                    break;
                case WIDX_FASTFORWARD:
                    FastforwardMenuDropdown(selectedIndex);
                    break;
                case WIDX_DEBUG:
                    DebugMenuDropdown(selectedIndex);
                    break;
                case WIDX_NETWORK:
                    NetworkMenuDropdown(selectedIndex);
                    break;
            }
        }

#ifdef ENABLE_SCRIPTING
        // The following are tool events for custom tools set by user scripts.
        // NB: these can't go into CustomWindow.cpp, as tools may be active without a visible window.
        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto& customTool = OpenRCT2::Scripting::ActiveCustomTool;
            if (customTool)
            {
                customTool->OnUpdate(screenCoords);
            }
        }

        void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto& customTool = OpenRCT2::Scripting::ActiveCustomTool;
            if (customTool)
            {
                customTool->OnDown(screenCoords);
            }
        }

        void OnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto& customTool = OpenRCT2::Scripting::ActiveCustomTool;
            if (customTool)
            {
                customTool->OnDrag(screenCoords);
            }
        }

        void OnToolUp(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto& customTool = OpenRCT2::Scripting::ActiveCustomTool;
            if (customTool)
            {
                customTool->OnUp(screenCoords);
            }
        }

        void OnToolAbort(WidgetIndex widgetIndex) override
        {
            auto& customTool = OpenRCT2::Scripting::ActiveCustomTool;
            if (customTool)
            {
                customTool->OnAbort();
                customTool = {};
            }
        }
#endif

        void ResetWidgetToDefaultState()
        {
            // Enable / disable buttons
            widgets[WIDX_PAUSE].type = WidgetType::trnBtn;
            widgets[WIDX_FILE_MENU].type = WidgetType::trnBtn;
            widgets[WIDX_ZOOM_OUT].type = WidgetType::trnBtn;
            widgets[WIDX_ZOOM_IN].type = WidgetType::trnBtn;
            widgets[WIDX_ROTATE_CLOCKWISE].type = WidgetType::trnBtn;
            widgets[WIDX_ROTATE_ANTI_CLOCKWISE].type = WidgetType::trnBtn;
            widgets[WIDX_VIEW_MENU].type = WidgetType::trnBtn;
            widgets[WIDX_MAP].type = WidgetType::trnBtn;
            widgets[WIDX_MUTE].type = WidgetType::trnBtn;
            widgets[WIDX_CHAT].type = WidgetType::trnBtn;
            widgets[WIDX_LAND].type = WidgetType::trnBtn;
            widgets[WIDX_WATER].type = WidgetType::trnBtn;
            widgets[WIDX_SCENERY].type = WidgetType::trnBtn;
            widgets[WIDX_PATH].type = WidgetType::trnBtn;
            widgets[WIDX_CONSTRUCT_RIDE].type = WidgetType::trnBtn;
            widgets[WIDX_RIDES].type = WidgetType::trnBtn;
            widgets[WIDX_PARK].type = WidgetType::trnBtn;
            widgets[WIDX_STAFF].type = WidgetType::trnBtn;
            widgets[WIDX_GUESTS].type = WidgetType::trnBtn;
            widgets[WIDX_CLEAR_SCENERY].type = WidgetType::trnBtn;
            widgets[WIDX_FINANCES].type = WidgetType::trnBtn;
            widgets[WIDX_RESEARCH].type = WidgetType::trnBtn;
            widgets[WIDX_FASTFORWARD].type = WidgetType::trnBtn;
            widgets[WIDX_CHEATS].type = WidgetType::trnBtn;
            widgets[WIDX_DEBUG].type = Config::Get().general.DebuggingTools ? WidgetType::trnBtn : WidgetType::empty;
            widgets[WIDX_NEWS].type = WidgetType::trnBtn;
            widgets[WIDX_NETWORK].type = WidgetType::trnBtn;
        }

        void HideDisabledButtons()
        {
            if (!Config::Get().interface.ToolbarShowMute)
                widgets[WIDX_MUTE].type = WidgetType::empty;

            if (!Config::Get().interface.ToolbarShowChat)
                widgets[WIDX_CHAT].type = WidgetType::empty;

            if (!Config::Get().interface.ToolbarShowResearch)
                widgets[WIDX_RESEARCH].type = WidgetType::empty;

            if (!Config::Get().interface.ToolbarShowCheats)
                widgets[WIDX_CHEATS].type = WidgetType::empty;

            if (!Config::Get().interface.ToolbarShowNews)
                widgets[WIDX_NEWS].type = WidgetType::empty;

            if (!Config::Get().interface.ToolbarShowZoom)
            {
                widgets[WIDX_ZOOM_IN].type = WidgetType::empty;
                widgets[WIDX_ZOOM_OUT].type = WidgetType::empty;
            }

            if (!Config::Get().interface.ToolbarShowRotateAnticlockwise)
                widgets[WIDX_ROTATE_ANTI_CLOCKWISE].type = WidgetType::empty;

            if (gLegacyScene == LegacyScene::scenarioEditor || gLegacyScene == LegacyScene::trackDesignsManager)
            {
                widgets[WIDX_PAUSE].type = WidgetType::empty;
            }

            if ((getGameState().park.Flags & PARK_FLAGS_NO_MONEY) || !Config::Get().interface.ToolbarShowFinances)
                widgets[WIDX_FINANCES].type = WidgetType::empty;
        }

        void ApplyEditorMode()
        {
            if (isInEditorMode() == 0)
            {
                return;
            }

            widgets[WIDX_PARK].type = WidgetType::empty;
            widgets[WIDX_STAFF].type = WidgetType::empty;
            widgets[WIDX_GUESTS].type = WidgetType::empty;
            widgets[WIDX_FINANCES].type = WidgetType::empty;
            widgets[WIDX_RESEARCH].type = WidgetType::empty;
            widgets[WIDX_NEWS].type = WidgetType::empty;
            widgets[WIDX_NETWORK].type = WidgetType::empty;

            auto& gameState = getGameState();
            if (gameState.editorStep != EditorStep::LandscapeEditor)
            {
                widgets[WIDX_LAND].type = WidgetType::empty;
                widgets[WIDX_WATER].type = WidgetType::empty;
            }

            if (gameState.editorStep != EditorStep::RollercoasterDesigner)
            {
                widgets[WIDX_RIDES].type = WidgetType::empty;
                widgets[WIDX_CONSTRUCT_RIDE].type = WidgetType::empty;
                widgets[WIDX_FASTFORWARD].type = WidgetType::empty;
            }

            if (gameState.editorStep != EditorStep::LandscapeEditor
                && gameState.editorStep != EditorStep::RollercoasterDesigner)
            {
                widgets[WIDX_MAP].type = WidgetType::empty;
                widgets[WIDX_SCENERY].type = WidgetType::empty;
                widgets[WIDX_PATH].type = WidgetType::empty;
                widgets[WIDX_CLEAR_SCENERY].type = WidgetType::empty;

                widgets[WIDX_ZOOM_OUT].type = WidgetType::empty;
                widgets[WIDX_ZOOM_IN].type = WidgetType::empty;
                widgets[WIDX_ROTATE_ANTI_CLOCKWISE].type = WidgetType::empty;
                widgets[WIDX_ROTATE_CLOCKWISE].type = WidgetType::empty;
                widgets[WIDX_VIEW_MENU].type = WidgetType::empty;
            }
        }

        void ApplyNetworkMode()
        {
            switch (NetworkGetMode())
            {
                case NETWORK_MODE_NONE:
                    widgets[WIDX_NETWORK].type = WidgetType::empty;
                    widgets[WIDX_CHAT].type = WidgetType::empty;
                    break;
                case NETWORK_MODE_CLIENT:
                    widgets[WIDX_PAUSE].type = WidgetType::empty;
                    [[fallthrough]];
                case NETWORK_MODE_SERVER:
                    widgets[WIDX_FASTFORWARD].type = WidgetType::empty;
                    break;
            }
        }

        void ApplyZoomState()
        {
            // Zoomed out/in disable. Not sure where this code is in the original.
            const auto* mainWindow = WindowGetMain();
            if (mainWindow == nullptr || mainWindow->viewport == nullptr)
            {
                LOG_ERROR("mainWindow or mainWindow->viewport is null!");
                return;
            }

            if (mainWindow->viewport->zoom == ZoomLevel::min())
            {
                disabled_widgets |= (1uLL << WIDX_ZOOM_IN);
            }
            else if (mainWindow->viewport->zoom >= ZoomLevel::max())
            {
                disabled_widgets |= (1uLL << WIDX_ZOOM_OUT);
            }
            else
            {
                disabled_widgets &= ~((1uLL << WIDX_ZOOM_IN) | (1uLL << WIDX_ZOOM_OUT));
            }
        }

        void ApplyPausedState()
        {
            bool paused = (gGamePaused & GAME_PAUSED_NORMAL);
            if (paused || _waitingForPause)
            {
                pressed_widgets |= (1uLL << WIDX_PAUSE);
                if (paused)
                    _waitingForPause = false;
            }
            else
                pressed_widgets &= ~(1uLL << WIDX_PAUSE);
        }

        void ApplyMapRotation()
        {
            // Set map button to the right image.
            if (widgets[WIDX_MAP].type != WidgetType::empty)
            {
                static constexpr uint32_t _imageIdByRotation[] = {
                    SPR_G2_MAP_NORTH,
                    SPR_G2_MAP_WEST,
                    SPR_G2_MAP_SOUTH,
                    SPR_G2_MAP_EAST,
                };

                uint32_t mapImageId = _imageIdByRotation[GetCurrentRotation()];
                widgets[WIDX_MAP].image = ImageId(mapImageId, FilterPaletteID::PaletteNull);
            }
        }

        void ApplyAudioState()
        {
            if (!OpenRCT2::Audio::gGameSoundsOff)
                widgets[WIDX_MUTE].image = ImageId(SPR_G2_TOOLBAR_MUTE, FilterPaletteID::PaletteNull);
            else
                widgets[WIDX_MUTE].image = ImageId(SPR_G2_TOOLBAR_UNMUTE, FilterPaletteID::PaletteNull);
        }

        void ApplyFootpathPressed()
        {
            // Footpath button pressed down
            auto* windowMgr = GetWindowManager();
            if (windowMgr->FindByClass(WindowClass::Footpath) == nullptr)
                pressed_widgets &= ~(1uLL << WIDX_PATH);
            else
                pressed_widgets |= (1uLL << WIDX_PATH);
        }

        // TODO: look into using std::span
        template<typename T>
        uint16_t GetToolbarWidth(T toolbarItems)
        {
            bool firstItem = true;
            auto totalWidth = 0;
            for (auto widgetIndex : toolbarItems)
            {
                auto* widget = &widgets[widgetIndex];
                if (widget->type == WidgetType::empty && widgetIndex != WIDX_SEPARATOR)
                    continue;

                if (firstItem && widgetIndex == WIDX_SEPARATOR)
                    continue;

                totalWidth += widget->width() + 1;
                firstItem = false;
            }
            return totalWidth;
        }

        // TODO: look into using std::span
        template<typename T>
        void AlignButtons(T toolbarItems, uint16_t xPos)
        {
            bool firstItem = true;
            for (auto widgetIndex : toolbarItems)
            {
                auto* widget = &widgets[widgetIndex];
                if (widget->type == WidgetType::empty && widgetIndex != WIDX_SEPARATOR)
                    continue;

                if (firstItem && widgetIndex == WIDX_SEPARATOR)
                    continue;

                auto widgetWidth = widget->width();
                widget->left = xPos;
                xPos += widgetWidth;
                widget->right = xPos;
                xPos += 1;

                firstItem = false;
            }
        }

        void AlignButtonsLeftRight()
        {
            // Align left hand side toolbar buttons
            AlignButtons(kWidgetOrderLeftGroup, 0);

            // Align right hand side toolbar buttons
            auto totalWidth = GetToolbarWidth(kWidgetOrderRightGroup);
            auto xPos = ContextGetWidth() - totalWidth;
            AlignButtons(kWidgetOrderRightGroup, xPos);
        }

        void AlignButtonsCentre()
        {
            // First, we figure out how much space we'll be needing
            auto totalWidth = GetToolbarWidth(kWidgetOrderCombined);

            // We'll start from the centre of the UI...
            auto xPos = (ContextGetWidth() - totalWidth) / 2;

            // And finally, align the buttons in the centre
            AlignButtons(kWidgetOrderCombined, xPos);
        }

        void OnPrepareDraw() override
        {
            ResetWidgetToDefaultState();
            HideDisabledButtons();
            ApplyEditorMode();

            ApplyPausedState();
            ApplyAudioState();
            ApplyNetworkMode();

            ApplyZoomState();
            ApplyMapRotation();
            ApplyFootpathPressed();

            if (!Config::Get().interface.ToolbarButtonsCentred)
                AlignButtonsLeftRight();
            else
                AlignButtonsCentre();
        }

        void OnDraw(RenderTarget& rt) override
        {
            const auto& gameState = getGameState();
            int32_t imgId;

            WindowDrawWidgets(*this, rt);

            ScreenCoordsXY screenPos{};
            // Draw staff button image (setting masks to the staff colours)
            if (widgets[WIDX_STAFF].type != WidgetType::empty)
            {
                screenPos = { windowPos.x + widgets[WIDX_STAFF].left, windowPos.y + widgets[WIDX_STAFF].top };
                imgId = SPR_TOOLBAR_STAFF;
                if (widgetIsPressed(*this, WIDX_STAFF))
                    imgId++;
                GfxDrawSprite(rt, ImageId(imgId, gameState.staffHandymanColour, gameState.staffMechanicColour), screenPos);
            }

            // Draw fast forward button
            if (widgets[WIDX_FASTFORWARD].type != WidgetType::empty)
            {
                screenPos = { windowPos.x + widgets[WIDX_FASTFORWARD].left + 0,
                              windowPos.y + widgets[WIDX_FASTFORWARD].top + 0 };
                if (widgetIsPressed(*this, WIDX_FASTFORWARD))
                    screenPos.y++;
                GfxDrawSprite(rt, ImageId(SPR_G2_FASTFORWARD), screenPos + ScreenCoordsXY{ 6, 3 });

                for (int32_t i = 0; i < gGameSpeed && gGameSpeed <= 4; i++)
                {
                    GfxDrawSprite(rt, ImageId(SPR_G2_SPEED_ARROW), screenPos + ScreenCoordsXY{ 5 + i * 5, 15 });
                }
                for (int32_t i = 0; i < 3 && i < gGameSpeed - 4 && gGameSpeed >= 5; i++)
                {
                    GfxDrawSprite(rt, ImageId(SPR_G2_HYPER_ARROW), screenPos + ScreenCoordsXY{ 5 + i * 6, 15 });
                }
            }

            // Draw cheats button
            if (widgets[WIDX_CHEATS].type != WidgetType::empty)
            {
                screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_CHEATS].left - 1, widgets[WIDX_CHEATS].top - 1 };
                if (widgetIsPressed(*this, WIDX_CHEATS))
                    screenPos.y++;
                GfxDrawSprite(rt, ImageId(SPR_G2_SANDBOX), screenPos);

                // Draw an overlay if clearance checks are disabled
                if (getGameState().cheats.disableClearanceChecks)
                {
                    auto colour = ColourWithFlags{ COLOUR_DARK_ORANGE }.withFlag(ColourFlag::withOutline, true);
                    DrawTextBasic(
                        rt, screenPos + ScreenCoordsXY{ 26, 2 }, STR_OVERLAY_CLEARANCE_CHECKS_DISABLED, {},
                        { colour, TextAlignment::RIGHT });
                }
            }

            // Draw chat button
            if (widgets[WIDX_CHAT].type != WidgetType::empty)
            {
                screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_CHAT].left, widgets[WIDX_CHAT].top - 2 };
                if (widgetIsPressed(*this, WIDX_CHAT))
                    screenPos.y++;
                GfxDrawSprite(rt, ImageId(SPR_G2_CHAT), screenPos);
            }

            // Draw debug button
            if (widgets[WIDX_DEBUG].type != WidgetType::empty)
            {
                screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_DEBUG].left, widgets[WIDX_DEBUG].top - 1 };
                if (widgetIsPressed(*this, WIDX_DEBUG))
                    screenPos.y++;
                GfxDrawSprite(rt, ImageId(SPR_TAB_GEARS_0), screenPos);
            }

            // Draw research button
            if (widgets[WIDX_RESEARCH].type != WidgetType::empty)
            {
                screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_RESEARCH].left - 1, widgets[WIDX_RESEARCH].top };
                if (widgetIsPressed(*this, WIDX_RESEARCH))
                    screenPos.y++;
                GfxDrawSprite(rt, ImageId(SPR_TAB_FINANCES_RESEARCH_0), screenPos);
            }

            // Draw finances button
            if (widgets[WIDX_FINANCES].type != WidgetType::empty)
            {
                screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_FINANCES].left + 3, widgets[WIDX_FINANCES].top + 1 };
                if (widgetIsPressed(*this, WIDX_FINANCES))
                    screenPos.y++;
                GfxDrawSprite(rt, ImageId(SPR_FINANCE), screenPos);
            }

            // Draw news button
            if (widgets[WIDX_NEWS].type != WidgetType::empty)
            {
                screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_NEWS].left + 3, widgets[WIDX_NEWS].top + 0 };
                if (widgetIsPressed(*this, WIDX_NEWS))
                    screenPos.y++;
                GfxDrawSprite(rt, ImageId(SPR_G2_TAB_NEWS), screenPos);
            }

            // Draw network button
            if (widgets[WIDX_NETWORK].type != WidgetType::empty)
            {
                screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_NETWORK].left + 3, widgets[WIDX_NETWORK].top + 0 };
                if (widgetIsPressed(*this, WIDX_NETWORK))
                    screenPos.y++;

                // Draw (de)sync icon.
                imgId = (NetworkIsDesynchronised() ? SPR_G2_MULTIPLAYER_DESYNC : SPR_G2_MULTIPLAYER_SYNC);
                GfxDrawSprite(rt, ImageId(imgId), screenPos + ScreenCoordsXY{ 3, 11 });

                // Draw number of players.
                auto ft = Formatter();
                ft.Add<int32_t>(NetworkGetNumVisiblePlayers());
                auto colour = ColourWithFlags{ COLOUR_WHITE }.withFlag(ColourFlag::withOutline, true);
                DrawTextBasic(rt, screenPos + ScreenCoordsXY{ 23, 1 }, STR_COMMA16, ft, { colour, TextAlignment::RIGHT });
            }

            if (widgets[WIDX_ROTATE_ANTI_CLOCKWISE].type != WidgetType::empty)
            {
                screenPos = windowPos
                    + ScreenCoordsXY{ widgets[WIDX_ROTATE_ANTI_CLOCKWISE].left + 2,
                                      widgets[WIDX_ROTATE_ANTI_CLOCKWISE].top + 0 };
                if (IsWidgetPressed(WIDX_ROTATE_ANTI_CLOCKWISE))
                    screenPos.y++;
                GfxDrawSprite(rt, ImageId(SPR_G2_ICON_ROTATE_ANTI_CLOCKWISE), screenPos);
            }
        }
    };

    /**
     * Creates the main game top toolbar window.
     *  rct2: 0x0066B485 (part of 0x0066B3E8)
     */
    WindowBase* TopToolbarOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->Create<TopToolbar>(
            WindowClass::TopToolbar, ScreenCoordsXY(0, 0), { ContextGetWidth(), kTopToolbarHeight + 1 },
            WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND | WF_NO_TITLE_BAR);

        window->SetWidgets(_topToolbarWidgets);

        WindowInitScrollWidgets(*window);

        return window;
    }

    void TopToolbar::InitViewMenu(Widget& widget)
    {
        using namespace Dropdown;
        constexpr ItemExt items[] = {
            ToggleOption(DDIDX_UNDERGROUND_INSIDE, STR_UNDERGROUND_VIEW),
            ToggleOption(DDIDX_TRANSPARENT_WATER, STR_VIEWPORT_TRANSPARENT_WATER),
            ToggleOption(DDIDX_HIDE_BASE, STR_REMOVE_BASE_LAND),
            ToggleOption(DDIDX_HIDE_VERTICAL, STR_REMOVE_VERTICAL_FACES),
            Separator(),
            ToggleOption(DDIDX_HIDE_RIDES, STR_SEE_THROUGH_RIDES),
            ToggleOption(DDIDX_HIDE_VEHICLES, STR_SEE_THROUGH_VEHICLES),
            ToggleOption(DDIDX_HIDE_VEGETATION, STR_SEE_THROUGH_VEGETATION),
            ToggleOption(DDIDX_HIDE_SCENERY, STR_SEE_THROUGH_SCENERY),
            ToggleOption(DDIDX_HIDE_PATHS, STR_SEE_THROUGH_PATHS),
            ToggleOption(DDIDX_HIDE_SUPPORTS, STR_SEE_THROUGH_SUPPORTS),
            ToggleOption(DDIDX_HIDE_GUESTS, STR_SEE_THROUGH_GUESTS),
            ToggleOption(DDIDX_HIDE_STAFF, STR_SEE_THROUGH_STAFF),
            Separator(),
            ToggleOption(DDIDX_LAND_HEIGHTS, STR_HEIGHT_MARKS_ON_LAND),
            ToggleOption(DDIDX_TRACK_HEIGHTS, STR_HEIGHT_MARKS_ON_RIDE_TRACKS),
            ToggleOption(DDIDX_PATH_HEIGHTS, STR_HEIGHT_MARKS_ON_PATHS),
            Separator(),
            ToggleOption(DDIDX_VIEW_CLIPPING, STR_VIEW_CLIPPING_MENU),
            ToggleOption(DDIDX_HIGHLIGHT_PATH_ISSUES, STR_HIGHLIGHT_PATH_ISSUES_MENU),
            Separator(),
            ToggleOption(DDIDX_TRANSPARENCY, STR_TRANSPARENCY_OPTIONS),
        };

        static_assert(ItemIDsMatchIndices(items));

        SetItems(items);

        WindowDropdownShowText(
            { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height() + 1,
            colours[1].withFlag(ColourFlag::translucent, true), 0, TOP_TOOLBAR_VIEW_MENU_COUNT);

        auto mvpFlags = WindowGetMain()->viewport->flags;
        SetChecked(DDIDX_UNDERGROUND_INSIDE, mvpFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE);
        SetChecked(DDIDX_TRANSPARENT_WATER, Config::Get().general.TransparentWater);
        SetChecked(DDIDX_HIDE_BASE, mvpFlags & VIEWPORT_FLAG_HIDE_BASE);
        SetChecked(DDIDX_HIDE_VERTICAL, mvpFlags & VIEWPORT_FLAG_HIDE_VERTICAL);
        SetChecked(DDIDX_HIDE_RIDES, mvpFlags & VIEWPORT_FLAG_HIDE_RIDES);
        SetChecked(DDIDX_HIDE_VEHICLES, mvpFlags & VIEWPORT_FLAG_HIDE_VEHICLES);
        SetChecked(DDIDX_HIDE_VEGETATION, mvpFlags & VIEWPORT_FLAG_HIDE_VEGETATION);
        SetChecked(DDIDX_HIDE_SCENERY, mvpFlags & VIEWPORT_FLAG_HIDE_SCENERY);
        SetChecked(DDIDX_HIDE_PATHS, mvpFlags & VIEWPORT_FLAG_HIDE_PATHS);
        SetChecked(DDIDX_HIDE_SUPPORTS, mvpFlags & VIEWPORT_FLAG_HIDE_SUPPORTS);
        SetChecked(DDIDX_HIDE_GUESTS, mvpFlags & VIEWPORT_FLAG_HIDE_GUESTS);
        SetChecked(DDIDX_HIDE_STAFF, mvpFlags & VIEWPORT_FLAG_HIDE_STAFF);
        SetChecked(DDIDX_LAND_HEIGHTS, mvpFlags & VIEWPORT_FLAG_LAND_HEIGHTS);
        SetChecked(DDIDX_TRACK_HEIGHTS, mvpFlags & VIEWPORT_FLAG_TRACK_HEIGHTS);
        SetChecked(DDIDX_PATH_HEIGHTS, mvpFlags & VIEWPORT_FLAG_PATH_HEIGHTS);
        SetChecked(DDIDX_VIEW_CLIPPING, mvpFlags & VIEWPORT_FLAG_CLIP_VIEW);
        SetChecked(DDIDX_HIGHLIGHT_PATH_ISSUES, mvpFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES);

        gDropdownDefaultIndex = DDIDX_UNDERGROUND_INSIDE;
    }

    void TopToolbar::ViewMenuDropdown(int16_t dropdownIndex)
    {
        auto* w = WindowGetMain();
        if (w != nullptr)
        {
            switch (dropdownIndex)
            {
                case DDIDX_UNDERGROUND_INSIDE:
                    w->viewport->flags ^= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
                    break;
                case DDIDX_TRANSPARENT_WATER:
                    Config::Get().general.TransparentWater ^= 1;
                    Config::Save();
                    break;
                case DDIDX_HIDE_BASE:
                    w->viewport->flags ^= VIEWPORT_FLAG_HIDE_BASE;
                    break;
                case DDIDX_HIDE_VERTICAL:
                    w->viewport->flags ^= VIEWPORT_FLAG_HIDE_VERTICAL;
                    break;
                case DDIDX_HIDE_RIDES:
                    w->viewport->flags ^= VIEWPORT_FLAG_HIDE_RIDES;
                    break;
                case DDIDX_HIDE_VEHICLES:
                    w->viewport->flags ^= VIEWPORT_FLAG_HIDE_VEHICLES;
                    break;
                case DDIDX_HIDE_VEGETATION:
                    w->viewport->flags ^= VIEWPORT_FLAG_HIDE_VEGETATION;
                    break;
                case DDIDX_HIDE_SCENERY:
                    w->viewport->flags ^= VIEWPORT_FLAG_HIDE_SCENERY;
                    break;
                case DDIDX_HIDE_PATHS:
                    w->viewport->flags ^= VIEWPORT_FLAG_HIDE_PATHS;
                    break;
                case DDIDX_HIDE_SUPPORTS:
                    w->viewport->flags ^= VIEWPORT_FLAG_HIDE_SUPPORTS;
                    break;
                case DDIDX_HIDE_GUESTS:
                    w->viewport->flags ^= VIEWPORT_FLAG_HIDE_GUESTS;
                    break;
                case DDIDX_HIDE_STAFF:
                    w->viewport->flags ^= VIEWPORT_FLAG_HIDE_STAFF;
                    break;
                case DDIDX_LAND_HEIGHTS:
                    w->viewport->flags ^= VIEWPORT_FLAG_LAND_HEIGHTS;
                    break;
                case DDIDX_TRACK_HEIGHTS:
                    w->viewport->flags ^= VIEWPORT_FLAG_TRACK_HEIGHTS;
                    break;
                case DDIDX_PATH_HEIGHTS:
                    w->viewport->flags ^= VIEWPORT_FLAG_PATH_HEIGHTS;
                    break;
                case DDIDX_VIEW_CLIPPING:
                {
                    auto* windowMgr = GetWindowManager();
                    if (windowMgr->FindByClass(WindowClass::ViewClipping) == nullptr)
                    {
                        ContextOpenWindow(WindowClass::ViewClipping);
                    }
                    else
                    {
                        // If window is already open, toggle the view clipping on/off
                        w->viewport->flags ^= VIEWPORT_FLAG_CLIP_VIEW;
                    }
                    break;
                }
                case DDIDX_HIGHLIGHT_PATH_ISSUES:
                    w->viewport->flags ^= VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES;
                    break;
                case DDIDX_TRANSPARENCY:
                    ContextOpenWindow(WindowClass::Transparency);
                    break;
                default:
                    return;
            }
            w->Invalidate();
        }
    }

    void TopToolbar::InitMapMenu(Widget& widget)
    {
        auto i = 0;
        gDropdownItems[i++].Format = STR_SHORTCUT_SHOW_MAP;
        gDropdownItems[i++].Format = STR_EXTRA_VIEWPORT;
        if (gLegacyScene == LegacyScene::scenarioEditor && getGameState().editorStep == EditorStep::LandscapeEditor)
        {
            gDropdownItems[i++].Format = STR_MAPGEN_MENU_ITEM;
        }

#ifdef ENABLE_SCRIPTING
        const auto& customMenuItems = OpenRCT2::Scripting::CustomMenuItems;
        if (!customMenuItems.empty())
        {
            gDropdownItems[i++].Format = kStringIdEmpty;
            for (const auto& item : customMenuItems)
            {
                if (item.Kind == OpenRCT2::Scripting::CustomToolbarMenuItemKind::Standard)
                {
                    gDropdownItems[i].Format = STR_STRING;
                    auto sz = item.Text.c_str();
                    std::memcpy(&gDropdownItems[i].Args, &sz, sizeof(const char*));
                    i++;
                }
            }
        }
#endif

        WindowDropdownShowText(
            { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height() + 1,
            colours[1].withFlag(ColourFlag::translucent, true), 0, i);
        gDropdownDefaultIndex = DDIDX_SHOW_MAP;
    }

    void TopToolbar::MapMenuDropdown(int16_t dropdownIndex)
    {
        int32_t customStartIndex = 3;
        if (gLegacyScene == LegacyScene::scenarioEditor && getGameState().editorStep == EditorStep::LandscapeEditor)
        {
            customStartIndex++;
        }

        if (dropdownIndex < customStartIndex)
        {
            switch (dropdownIndex)
            {
                case 0:
                    ContextOpenWindow(WindowClass::Map);
                    break;
                case 1:
                    ContextOpenWindow(WindowClass::Viewport);
                    break;
                case 2:
                    ContextOpenWindow(WindowClass::Mapgen);
                    break;
            }
        }
        else
        {
#ifdef ENABLE_SCRIPTING
            const auto& customMenuItems = OpenRCT2::Scripting::CustomMenuItems;
            auto customIndex = static_cast<size_t>(dropdownIndex - customStartIndex);
            size_t i = 0;
            for (const auto& item : customMenuItems)
            {
                if (item.Kind == OpenRCT2::Scripting::CustomToolbarMenuItemKind::Standard)
                {
                    if (i == customIndex)
                    {
                        item.Invoke();
                        break;
                    }
                    i++;
                }
            }
#endif
        }
    }

    void TopToolbar::InitFastforwardMenu(Widget& widget)
    {
        int32_t num_items = 4;
        gDropdownItems[0].Format = STR_TOGGLE_OPTION;
        gDropdownItems[1].Format = STR_TOGGLE_OPTION;
        gDropdownItems[2].Format = STR_TOGGLE_OPTION;
        gDropdownItems[3].Format = STR_TOGGLE_OPTION;
        if (Config::Get().general.DebuggingTools)
        {
            gDropdownItems[4].Format = kStringIdEmpty;
            gDropdownItems[5].Format = STR_TOGGLE_OPTION;
            gDropdownItems[5].Args = STR_SPEED_HYPER;
            num_items = 6;
        }

        gDropdownItems[0].Args = STR_SPEED_NORMAL;
        gDropdownItems[1].Args = STR_SPEED_QUICK;
        gDropdownItems[2].Args = STR_SPEED_FAST;
        gDropdownItems[3].Args = STR_SPEED_TURBO;

        WindowDropdownShowText(
            { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height() + 1,
            colours[0].withFlag(ColourFlag::translucent, true), 0, num_items);

        // Set checkmarks
        if (gGameSpeed <= 4)
        {
            Dropdown::SetChecked(gGameSpeed - 1, true);
        }
        if (gGameSpeed == 8)
        {
            Dropdown::SetChecked(5, true);
        }

        if (Config::Get().general.DebuggingTools)
        {
            gDropdownDefaultIndex = (gGameSpeed == 8 ? 0 : gGameSpeed);
        }
        else
        {
            gDropdownDefaultIndex = (gGameSpeed >= 4 ? 0 : gGameSpeed);
        }
        if (gDropdownDefaultIndex == 4)
        {
            gDropdownDefaultIndex = 5;
        }
    }

    void TopToolbar::FastforwardMenuDropdown(int16_t dropdownIndex)
    {
        auto* w = WindowGetMain();
        if (w != nullptr)
        {
            if (dropdownIndex >= 0 && dropdownIndex <= 5)
            {
                auto newSpeed = dropdownIndex + 1;
                if (newSpeed >= 5)
                    newSpeed = 8;

                auto setSpeedAction = GameSetSpeedAction(newSpeed);
                GameActions::Execute(&setSpeedAction);
            }
        }
    }

    void TopToolbar::InitFileMenu(Widget& widget)
    {
        int32_t numItems = 0;
        if (isInTrackDesignerOrManager())
        {
            gDropdownItems[numItems++].Format = STR_SCREENSHOT;
            gDropdownItems[numItems++].Format = STR_GIANT_SCREENSHOT;
            gDropdownItems[numItems++].Format = kStringIdEmpty;
            gDropdownItems[numItems++].Format = STR_ABOUT;
            gDropdownItems[numItems++].Format = STR_FILE_BUG_ON_GITHUB;

            if (OpenRCT2::GetContext()->HasNewVersionInfo())
                gDropdownItems[numItems++].Format = STR_UPDATE_AVAILABLE;

            gDropdownItems[numItems++].Format = STR_OPTIONS;
            gDropdownItems[numItems++].Format = kStringIdEmpty;

            if (gLegacyScene == LegacyScene::trackDesigner)
                gDropdownItems[numItems++].Format = STR_QUIT_ROLLERCOASTER_DESIGNER;
            else
                gDropdownItems[numItems++].Format = STR_QUIT_TRACK_DESIGNS_MANAGER;

            gDropdownItems[numItems++].Format = STR_EXIT_OPENRCT2;
        }
        else if (gLegacyScene == LegacyScene::scenarioEditor)
        {
            gDropdownItems[numItems++].Format = STR_LOAD_LANDSCAPE;
            gDropdownItems[numItems++].Format = kStringIdEmpty;
            gDropdownItems[numItems++].Format = STR_SAVE_LANDSCAPE;
            gDropdownItems[numItems++].Format = kStringIdEmpty;
            gDropdownItems[numItems++].Format = STR_SCREENSHOT;
            gDropdownItems[numItems++].Format = STR_GIANT_SCREENSHOT;
            gDropdownItems[numItems++].Format = kStringIdEmpty;
            gDropdownItems[numItems++].Format = STR_ABOUT;
            gDropdownItems[numItems++].Format = STR_FILE_BUG_ON_GITHUB;

            if (OpenRCT2::GetContext()->HasNewVersionInfo())
                gDropdownItems[numItems++].Format = STR_UPDATE_AVAILABLE;

            gDropdownItems[numItems++].Format = STR_OPTIONS;
            gDropdownItems[numItems++].Format = kStringIdEmpty;
            gDropdownItems[numItems++].Format = STR_QUIT_SCENARIO_EDITOR;
            gDropdownItems[numItems++].Format = STR_EXIT_OPENRCT2;
        }
        else
        {
            gDropdownItems[numItems++].Format = STR_NEW_GAME;
            gDropdownItems[numItems++].Format = STR_LOAD_GAME;
            gDropdownItems[numItems++].Format = kStringIdEmpty;
            gDropdownItems[numItems++].Format = STR_SAVE_GAME;
            gDropdownItems[numItems++].Format = STR_SAVE_GAME_AS;
            gDropdownItems[numItems++].Format = kStringIdEmpty;
            gDropdownItems[numItems++].Format = STR_SCREENSHOT;
            gDropdownItems[numItems++].Format = STR_GIANT_SCREENSHOT;
            gDropdownItems[numItems++].Format = kStringIdEmpty;
            gDropdownItems[numItems++].Format = STR_ABOUT;
            gDropdownItems[numItems++].Format = STR_FILE_BUG_ON_GITHUB;

            if (OpenRCT2::GetContext()->HasNewVersionInfo())
                gDropdownItems[numItems++].Format = STR_UPDATE_AVAILABLE;

            gDropdownItems[numItems++].Format = STR_OPTIONS;
            gDropdownItems[numItems++].Format = kStringIdEmpty;
            gDropdownItems[numItems++].Format = STR_QUIT_TO_MENU;
            gDropdownItems[numItems++].Format = STR_EXIT_OPENRCT2;
        }

        WindowDropdownShowText(
            { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height() + 1,
            colours[0].withFlag(ColourFlag::translucent, true), Dropdown::Flag::StayOpen, numItems);
    }

    void TopToolbar::InitCheatsMenu(Widget& widget)
    {
        using namespace Dropdown;

        constexpr ItemExt items[] = {
            ToggleOption(DDIDX_CHEATS, STR_CHEAT_TITLE),
            ToggleOption(DDIDX_TILE_INSPECTOR, STR_DEBUG_DROPDOWN_TILE_INSPECTOR),
            ToggleOption(DDIDX_OBJECT_SELECTION, STR_DEBUG_DROPDOWN_OBJECT_SELECTION),
            ToggleOption(DDIDX_INVENTIONS_LIST, STR_DEBUG_DROPDOWN_INVENTIONS_LIST),
            ToggleOption(DDIDX_SCENARIO_OPTIONS, STR_DEBUG_DROPDOWN_SCENARIO_OPTIONS),
            Separator(),
            ToggleOption(DDIDX_ENABLE_SANDBOX_MODE, STR_ENABLE_SANDBOX_MODE),
            ToggleOption(DDIDX_DISABLE_CLEARANCE_CHECKS, STR_DISABLE_CLEARANCE_CHECKS),
            ToggleOption(DDIDX_DISABLE_SUPPORT_LIMITS, STR_DISABLE_SUPPORT_LIMITS),
        };
        static_assert(ItemIDsMatchIndices(items));

        SetItems(items);

        WindowDropdownShowText(
            { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height() + 1,
            colours[0].withFlag(ColourFlag::translucent, true), Dropdown::Flag::StayOpen, TOP_TOOLBAR_CHEATS_COUNT);

        // Disable items that are not yet available in multiplayer
        if (NetworkGetMode() != NETWORK_MODE_NONE)
        {
            Dropdown::SetDisabled(DDIDX_OBJECT_SELECTION, true);
            Dropdown::SetDisabled(DDIDX_INVENTIONS_LIST, true);
        }

        if (isInEditorMode())
        {
            Dropdown::SetDisabled(DDIDX_OBJECT_SELECTION, true);
            Dropdown::SetDisabled(DDIDX_INVENTIONS_LIST, true);
            Dropdown::SetDisabled(DDIDX_SCENARIO_OPTIONS, true);
            Dropdown::SetDisabled(DDIDX_ENABLE_SANDBOX_MODE, true);
        }

        auto& gameState = getGameState();
        if (gameState.cheats.sandboxMode)
        {
            Dropdown::SetChecked(DDIDX_ENABLE_SANDBOX_MODE, true);
        }
        if (gameState.cheats.disableClearanceChecks)
        {
            Dropdown::SetChecked(DDIDX_DISABLE_CLEARANCE_CHECKS, true);
        }
        if (gameState.cheats.disableSupportLimits)
        {
            Dropdown::SetChecked(DDIDX_DISABLE_SUPPORT_LIMITS, true);
        }

        gDropdownDefaultIndex = DDIDX_CHEATS;
    }

    void TopToolbar::CheatsMenuDropdown(int16_t dropdownIndex)
    {
        switch (dropdownIndex)
        {
            case DDIDX_CHEATS:
                ContextOpenWindow(WindowClass::Cheats);
                break;
            case DDIDX_TILE_INSPECTOR:
                ContextOpenWindow(WindowClass::TileInspector);
                break;
            case DDIDX_OBJECT_SELECTION:
            {
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->CloseAll();
                ContextOpenWindow(WindowClass::EditorObjectSelection);
                break;
            }
            case DDIDX_INVENTIONS_LIST:
                ContextOpenWindow(WindowClass::EditorInventionList);
                break;
            case DDIDX_SCENARIO_OPTIONS:
                ContextOpenWindow(WindowClass::EditorScenarioOptions);
                break;
            case DDIDX_ENABLE_SANDBOX_MODE:
                CheatsSet(CheatType::SandboxMode, !getGameState().cheats.sandboxMode);
                break;
            case DDIDX_DISABLE_CLEARANCE_CHECKS:
                CheatsSet(CheatType::DisableClearanceChecks, !getGameState().cheats.disableClearanceChecks);
                break;
            case DDIDX_DISABLE_SUPPORT_LIMITS:
                CheatsSet(CheatType::DisableSupportLimits, !getGameState().cheats.disableSupportLimits);
                break;
        }
    }

    void TopToolbar::InitDebugMenu(Widget& widget)
    {
        gDropdownItems[DDIDX_CONSOLE].Format = STR_TOGGLE_OPTION;
        gDropdownItems[DDIDX_CONSOLE].Args = STR_DEBUG_DROPDOWN_CONSOLE;
        gDropdownItems[DDIDX_DEBUG_PAINT].Format = STR_TOGGLE_OPTION;
        gDropdownItems[DDIDX_DEBUG_PAINT].Args = STR_DEBUG_DROPDOWN_DEBUG_PAINT;

        WindowDropdownShowText(
            { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height() + 1,
            colours[0].withFlag(ColourFlag::translucent, true), Dropdown::Flag::StayOpen, TOP_TOOLBAR_DEBUG_COUNT);

        auto* windowMgr = GetWindowManager();
        Dropdown::SetChecked(DDIDX_DEBUG_PAINT, windowMgr->FindByClass(WindowClass::DebugPaint) != nullptr);
    }

    void TopToolbar::DebugMenuDropdown(int16_t dropdownIndex)
    {
        auto* w = WindowGetMain();
        if (w != nullptr)
        {
            switch (dropdownIndex)
            {
                case DDIDX_CONSOLE:
                {
                    auto& console = GetInGameConsole();
                    console.Open();
                    break;
                }
                case DDIDX_DEBUG_PAINT:
                {
                    auto* windowMgr = GetWindowManager();
                    if (windowMgr->FindByClass(WindowClass::DebugPaint) == nullptr)
                    {
                        ContextOpenWindow(WindowClass::DebugPaint);
                    }
                    else
                    {
                        windowMgr->CloseByClass(WindowClass::DebugPaint);
                    }
                    break;
                }
            }
        }
    }

    void TopToolbar::InitNetworkMenu(Widget& widget)
    {
        gDropdownItems[DDIDX_MULTIPLAYER].Format = STR_MULTIPLAYER;
        gDropdownItems[DDIDX_MULTIPLAYER_RECONNECT].Format = STR_MULTIPLAYER_RECONNECT;

        WindowDropdownShowText(
            { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height() + 1,
            colours[0].withFlag(ColourFlag::translucent, true), 0, TOP_TOOLBAR_NETWORK_COUNT);

        Dropdown::SetDisabled(DDIDX_MULTIPLAYER_RECONNECT, !NetworkIsDesynchronised());

        gDropdownDefaultIndex = DDIDX_MULTIPLAYER;
    }

    void TopToolbar::NetworkMenuDropdown(int16_t dropdownIndex)
    {
        auto* w = WindowGetMain();
        if (w != nullptr)
        {
            switch (dropdownIndex)
            {
                case DDIDX_MULTIPLAYER:
                    ContextOpenWindow(WindowClass::Multiplayer);
                    break;
                case DDIDX_MULTIPLAYER_RECONNECT:
                    NetworkReconnect();
                    break;
            }
        }
    }
} // namespace OpenRCT2::Ui::Windows
