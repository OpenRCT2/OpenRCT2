/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiContext.h"
#include "../interface/InGameConsole.h"
#include "../scripting/CustomMenu.h"

#include <algorithm>
#include <iterator>
#include <limits>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/Version.h>
#include <openrct2/actions/BannerPlaceAction.h>
#include <openrct2/actions/BannerSetColourAction.h>
#include <openrct2/actions/ClearAction.h>
#include <openrct2/actions/FootpathAdditionPlaceAction.h>
#include <openrct2/actions/LandLowerAction.h>
#include <openrct2/actions/LandRaiseAction.h>
#include <openrct2/actions/LandSmoothAction.h>
#include <openrct2/actions/LargeSceneryPlaceAction.h>
#include <openrct2/actions/LargeScenerySetColourAction.h>
#include <openrct2/actions/LoadOrQuitAction.h>
#include <openrct2/actions/PauseToggleAction.h>
#include <openrct2/actions/SmallSceneryPlaceAction.h>
#include <openrct2/actions/SmallScenerySetColourAction.h>
#include <openrct2/actions/SurfaceSetStyleAction.h>
#include <openrct2/actions/WallPlaceAction.h>
#include <openrct2/actions/WallSetColourAction.h>
#include <openrct2/actions/WaterLowerAction.h>
#include <openrct2/actions/WaterRaiseAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/InteractiveConsole.h>
#include <openrct2/interface/Screenshot.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/network/network.h>
#include <openrct2/paint/VirtualFloor.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/util/Math.hpp>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/LargeScenery.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/SmallScenery.h>
#include <openrct2/world/Surface.h>
#include <openrct2/world/Wall.h>
#include <string>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

enum
{
    WIDX_PAUSE,
    WIDX_FILE_MENU,
    WIDX_MUTE,
    WIDX_ZOOM_OUT,
    WIDX_ZOOM_IN,
    WIDX_ROTATE,
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

validate_global_widx(WC_TOP_TOOLBAR, WIDX_PAUSE);
validate_global_widx(WC_TOP_TOOLBAR, WIDX_LAND);
validate_global_widx(WC_TOP_TOOLBAR, WIDX_WATER);
validate_global_widx(WC_TOP_TOOLBAR, WIDX_SCENERY);
validate_global_widx(WC_TOP_TOOLBAR, WIDX_PATH);

enum FileMenuDdidx
{
    DDIDX_NEW_GAME = 0,
    DDIDX_LOAD_GAME = 1,
    DDIDX_SAVE_GAME = 2,
    DDIDX_SAVE_GAME_AS = 3,
    // separator
    DDIDX_ABOUT = 5,
    DDIDX_OPTIONS = 6,
    DDIDX_SCREENSHOT = 7,
    DDIDX_GIANT_SCREENSHOT = 8,
    // separator
    DDIDX_FILE_BUG_ON_GITHUB = 10,
    DDIDX_UPDATE_AVAILABLE = 11,
    // separator
    DDIDX_QUIT_TO_MENU = 13,
    DDIDX_EXIT_OPENRCT2 = 14,
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
    DDIDX_OBJECTIVE_OPTIONS = 5,
    // 6 is a separator
    DDIDX_ENABLE_SANDBOX_MODE = 7,
    DDIDX_DISABLE_CLEARANCE_CHECKS = 8,
    DDIDX_DISABLE_SUPPORT_LIMITS = 9,

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
// from left to right
static constexpr const int32_t left_aligned_widgets_order[] = {
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
    WIDX_ROTATE,
    WIDX_VIEW_MENU,
    WIDX_MAP,
};

// from right to left
static constexpr const int32_t right_aligned_widgets_order[] = {
    WIDX_NEWS,
    WIDX_GUESTS,
    WIDX_STAFF,
    WIDX_PARK,
    WIDX_RIDES,
    WIDX_RESEARCH,
    WIDX_FINANCES,

    WIDX_SEPARATOR,

    WIDX_CONSTRUCT_RIDE,
    WIDX_PATH,
    WIDX_SCENERY,
    WIDX_WATER,
    WIDX_LAND,
    WIDX_CLEAR_SCENERY,
};

#pragma endregion

static Widget window_top_toolbar_widgets[] = {
    MakeRemapWidget({  0, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Primary   , SPR_TOOLBAR_PAUSE,          STR_PAUSE_GAME_TIP                ), // Pause
    MakeRemapWidget({ 60, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Primary   , SPR_TOOLBAR_FILE,           STR_DISC_AND_GAME_OPTIONS_TIP     ), // File menu
    MakeRemapWidget({250, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Primary   , SPR_G2_TOOLBAR_MUTE,        STR_TOOLBAR_MUTE_TIP              ), // Mute
    MakeRemapWidget({100, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Secondary , SPR_TOOLBAR_ZOOM_OUT,       STR_ZOOM_OUT_TIP                  ), // Zoom out
    MakeRemapWidget({130, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Secondary , SPR_TOOLBAR_ZOOM_IN,        STR_ZOOM_IN_TIP                   ), // Zoom in
    MakeRemapWidget({160, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Secondary , SPR_TOOLBAR_ROTATE,         STR_ROTATE_TIP                    ), // Rotate camera
    MakeRemapWidget({190, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Secondary , SPR_TOOLBAR_VIEW,           STR_VIEW_OPTIONS_TIP              ), // Transparency menu
    MakeRemapWidget({220, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Secondary , SPR_TOOLBAR_MAP,            STR_SHOW_MAP_TIP                  ), // Map
    MakeRemapWidget({267, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Tertiary  , SPR_TOOLBAR_LAND,           STR_ADJUST_LAND_TIP               ), // Land
    MakeRemapWidget({297, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Tertiary  , SPR_TOOLBAR_WATER,          STR_ADJUST_WATER_TIP              ), // Water
    MakeRemapWidget({327, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Tertiary  , SPR_TOOLBAR_SCENERY,        STR_PLACE_SCENERY_TIP             ), // Scenery
    MakeRemapWidget({357, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Tertiary  , SPR_TOOLBAR_FOOTPATH,       STR_BUILD_FOOTPATH_TIP            ), // Path
    MakeRemapWidget({387, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Tertiary  , SPR_TOOLBAR_CONSTRUCT_RIDE, STR_BUILD_RIDE_TIP                ), // Construct ride
    MakeRemapWidget({490, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Quaternary, SPR_TOOLBAR_RIDES,          STR_RIDES_IN_PARK_TIP             ), // Rides
    MakeRemapWidget({520, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Quaternary, SPR_TOOLBAR_PARK,           STR_PARK_INFORMATION_TIP          ), // Park
    MakeRemapWidget({550, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Quaternary, SPR_TAB_TOOLBAR,            STR_STAFF_TIP                     ), // Staff
    MakeRemapWidget({560, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Quaternary, SPR_TOOLBAR_GUESTS,         STR_GUESTS_TIP                    ), // Guests
    MakeRemapWidget({560, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Tertiary  , SPR_TOOLBAR_CLEAR_SCENERY,  STR_CLEAR_SCENERY_TIP             ), // Clear scenery
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Primary   , SPR_TAB_TOOLBAR,            STR_GAME_SPEED_TIP                ), // Fast forward
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Primary   , SPR_TAB_TOOLBAR,            STR_CHEATS_TIP                    ), // Cheats
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Primary   , SPR_TAB_TOOLBAR,            STR_DEBUG_TIP                     ), // Debug
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Quaternary, SPR_TAB_TOOLBAR,            STR_SCENARIO_OPTIONS_FINANCIAL_TIP), // Finances
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Quaternary, SPR_TAB_TOOLBAR,            STR_FINANCES_RESEARCH_TIP         ), // Research
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Quaternary, SPR_TAB_TOOLBAR,            STR_SHOW_RECENT_MESSAGES_TIP      ), // News
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Primary   , SPR_G2_TOOLBAR_MULTIPLAYER, STR_SHOW_MULTIPLAYER_STATUS_TIP   ), // Network
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WindowWidgetType::TrnBtn, WindowColour::Primary   , SPR_TAB_TOOLBAR,            STR_TOOLBAR_CHAT_TIP              ), // Chat
    MakeWidget     ({  0, 0}, {10,                      1}, WindowWidgetType::Empty,  WindowColour::Primary                                                                   ), // Artificial widget separator
    WIDGETS_END,
};
// clang-format on

static void WindowTopToolbarMouseup(rct_window* w, WidgetIndex widgetIndex);
static void WindowTopToolbarMousedown(rct_window* w, WidgetIndex widgetIndex, Widget* widget);
static void WindowTopToolbarDropdown(rct_window* w, WidgetIndex widgetIndex, int32_t dropdownIndex);
static void WindowTopToolbarToolUpdate(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowTopToolbarToolDown(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowTopToolbarToolDrag(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowTopToolbarToolUp(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoordsy);
static void WindowTopToolbarToolAbort(rct_window* w, WidgetIndex widgetIndex);
static void WindowTopToolbarInvalidate(rct_window* w);
static void WindowTopToolbarPaint(rct_window* w, DrawPixelInfo* dpi);

static WindowEventList window_top_toolbar_events([](auto& events) {
    events.mouse_up = &WindowTopToolbarMouseup;
    events.mouse_down = &WindowTopToolbarMousedown;
    events.dropdown = &WindowTopToolbarDropdown;
    events.tool_update = &WindowTopToolbarToolUpdate;
    events.tool_down = &WindowTopToolbarToolDown;
    events.tool_drag = &WindowTopToolbarToolDrag;
    events.tool_up = &WindowTopToolbarToolUp;
    events.tool_abort = &WindowTopToolbarToolAbort;
    events.invalidate = &WindowTopToolbarInvalidate;
    events.paint = &WindowTopToolbarPaint;
});

static void TopToolbarInitViewMenu(rct_window* window, Widget* widget);
static void TopToolbarViewMenuDropdown(int16_t dropdownIndex);
static void TopToolbarInitMapMenu(rct_window* window, Widget* widget);
static void TopToolbarMapMenuDropdown(int16_t dropdownIndex);
static void TopToolbarInitFastforwardMenu(rct_window* window, Widget* widget);
static void TopToolbarFastforwardMenuDropdown(int16_t dropdownIndex);
static void TopToolbarInitRotateMenu(rct_window* window, Widget* widget);
static void TopToolbarRotateMenuDropdown(int16_t dropdownIndex);
static void TopToolbarInitCheatsMenu(rct_window* window, Widget* widget);
static void TopToolbarCheatsMenuDropdown(int16_t dropdownIndex);
static void TopToolbarInitDebugMenu(rct_window* window, Widget* widget);
static void TopToolbarDebugMenuDropdown(int16_t dropdownIndex);
static void TopToolbarInitNetworkMenu(rct_window* window, Widget* widget);
static void TopToolbarNetworkMenuDropdown(int16_t dropdownIndex);

static void ToggleFootpathWindow();
static void ToggleLandWindow(rct_window* topToolbar, WidgetIndex widgetIndex);
static void ToggleClearSceneryWindow(rct_window* topToolbar, WidgetIndex widgetIndex);
static void ToggleWaterWindow(rct_window* topToolbar, WidgetIndex widgetIndex);

static money64 SelectionLowerLand(uint8_t flags);
static money64 SelectionRaiseLand(uint8_t flags);

static ClearAction GetClearAction();

static bool _landToolBlocked;
static uint8_t _unkF64F0E;
static int16_t _unkF64F0A;

/**
 * Creates the main game top toolbar window.
 *  rct2: 0x0066B485 (part of 0x0066B3E8)
 */
rct_window* WindowTopToolbarOpen()
{
    rct_window* window = WindowCreate(
        ScreenCoordsXY(0, 0), ContextGetWidth(), TOP_TOOLBAR_HEIGHT + 1, &window_top_toolbar_events, WindowClass::TopToolbar,
        WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND);
    window->widgets = window_top_toolbar_widgets;

    WindowInitScrollWidgets(*window);

    return window;
}

/**
 *
 *  rct2: 0x0066C957
 */
static void WindowTopToolbarMouseup(rct_window* w, WidgetIndex widgetIndex)
{
    rct_window* mainWindow;

    switch (widgetIndex)
    {
        case WIDX_PAUSE:
            if (NetworkGetMode() != NETWORK_MODE_CLIENT)
            {
                auto pauseToggleAction = PauseToggleAction();
                GameActions::Execute(&pauseToggleAction);
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
        case WIDX_CLEAR_SCENERY:
            ToggleClearSceneryWindow(w, WIDX_CLEAR_SCENERY);
            break;
        case WIDX_LAND:
            ToggleLandWindow(w, WIDX_LAND);
            break;
        case WIDX_WATER:
            ToggleWaterWindow(w, WIDX_WATER);
            break;
        case WIDX_SCENERY:
            if (!ToolSet(*w, WIDX_SCENERY, Tool::Arrow))
            {
                InputSetFlag(INPUT_FLAG_6, true);
                ContextOpenWindow(WindowClass::Scenery);
            }
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
                ContextShowError(STR_CHAT_UNAVAILABLE, STR_NONE, {});
            }
            break;
    }
}

/**
 *
 *  rct2: 0x0066CA3B
 */
static void WindowTopToolbarMousedown(rct_window* w, WidgetIndex widgetIndex, Widget* widget)
{
    int32_t numItems = 0;

    switch (widgetIndex)
    {
        case WIDX_FILE_MENU:
            if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
            {
                gDropdownItems[numItems++].Format = STR_ABOUT;
                gDropdownItems[numItems++].Format = STR_OPTIONS;
                gDropdownItems[numItems++].Format = STR_SCREENSHOT;
                gDropdownItems[numItems++].Format = STR_GIANT_SCREENSHOT;
                gDropdownItems[numItems++].Format = STR_EMPTY;
                gDropdownItems[numItems++].Format = STR_FILE_BUG_ON_GITHUB;

                if (OpenRCT2::GetContext()->HasNewVersionInfo())
                    gDropdownItems[numItems++].Format = STR_UPDATE_AVAILABLE;

                gDropdownItems[numItems++].Format = STR_EMPTY;

                if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
                    gDropdownItems[numItems++].Format = STR_QUIT_ROLLERCOASTER_DESIGNER;
                else
                    gDropdownItems[numItems++].Format = STR_QUIT_TRACK_DESIGNS_MANAGER;

                gDropdownItems[numItems++].Format = STR_EXIT_OPENRCT2;
            }
            else if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
            {
                gDropdownItems[numItems++].Format = STR_LOAD_LANDSCAPE;
                gDropdownItems[numItems++].Format = STR_SAVE_LANDSCAPE;
                gDropdownItems[numItems++].Format = STR_EMPTY;
                gDropdownItems[numItems++].Format = STR_ABOUT;
                gDropdownItems[numItems++].Format = STR_OPTIONS;
                gDropdownItems[numItems++].Format = STR_SCREENSHOT;
                gDropdownItems[numItems++].Format = STR_GIANT_SCREENSHOT;
                gDropdownItems[numItems++].Format = STR_EMPTY;
                gDropdownItems[numItems++].Format = STR_FILE_BUG_ON_GITHUB;

                if (OpenRCT2::GetContext()->HasNewVersionInfo())
                    gDropdownItems[numItems++].Format = STR_UPDATE_AVAILABLE;

                gDropdownItems[numItems++].Format = STR_EMPTY;
                gDropdownItems[numItems++].Format = STR_QUIT_SCENARIO_EDITOR;
                gDropdownItems[numItems++].Format = STR_EXIT_OPENRCT2;
            }
            else
            {
                gDropdownItems[numItems++].Format = STR_NEW_GAME;
                gDropdownItems[numItems++].Format = STR_LOAD_GAME;
                gDropdownItems[numItems++].Format = STR_SAVE_GAME;
                gDropdownItems[numItems++].Format = STR_SAVE_GAME_AS;
                gDropdownItems[numItems++].Format = STR_EMPTY;
                gDropdownItems[numItems++].Format = STR_ABOUT;
                gDropdownItems[numItems++].Format = STR_OPTIONS;
                gDropdownItems[numItems++].Format = STR_SCREENSHOT;
                gDropdownItems[numItems++].Format = STR_GIANT_SCREENSHOT;
                gDropdownItems[numItems++].Format = STR_EMPTY;
                gDropdownItems[numItems++].Format = STR_FILE_BUG_ON_GITHUB;

                if (OpenRCT2::GetContext()->HasNewVersionInfo())
                    gDropdownItems[numItems++].Format = STR_UPDATE_AVAILABLE;

                gDropdownItems[numItems++].Format = STR_EMPTY;
                gDropdownItems[numItems++].Format = STR_QUIT_TO_MENU;
                gDropdownItems[numItems++].Format = STR_EXIT_OPENRCT2;
            }

            WindowDropdownShowText(
                { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[0] | 0x80,
                Dropdown::Flag::StayOpen, numItems);
            break;
        case WIDX_CHEATS:
            TopToolbarInitCheatsMenu(w, widget);
            break;
        case WIDX_VIEW_MENU:
            TopToolbarInitViewMenu(w, widget);
            break;
        case WIDX_MAP:
            TopToolbarInitMapMenu(w, widget);
            break;
        case WIDX_FASTFORWARD:
            TopToolbarInitFastforwardMenu(w, widget);
            break;
        case WIDX_ROTATE:
            TopToolbarInitRotateMenu(w, widget);
            break;
        case WIDX_DEBUG:
            TopToolbarInitDebugMenu(w, widget);
            break;
        case WIDX_NETWORK:
            TopToolbarInitNetworkMenu(w, widget);
            break;
    }
}

/**
 *
 *  rct2: 0x0066C9EA
 */
static void WindowTopToolbarDropdown(rct_window* w, WidgetIndex widgetIndex, int32_t dropdownIndex)
{
    switch (widgetIndex)
    {
        case WIDX_FILE_MENU:

            // New game is only available in the normal game. Skip one position to avoid incorrect mappings in the menus of the
            // other modes.
            if (gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR))
                dropdownIndex += 1;

            // Quicksave is only available in the normal game. Skip one position to avoid incorrect mappings in the menus of the
            // other modes.
            if (gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR) && dropdownIndex > DDIDX_LOAD_GAME)
                dropdownIndex += 1;

            // Track designer and track designs manager start with About, not Load/save
            if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
                dropdownIndex += DDIDX_ABOUT;

            // The "Update available" menu item is only available when there is one
            if (dropdownIndex >= DDIDX_UPDATE_AVAILABLE && !OpenRCT2::GetContext()->HasNewVersionInfo())
                dropdownIndex += 1;

            switch (dropdownIndex)
            {
                case DDIDX_NEW_GAME:
                {
                    auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::OpenSavePrompt, PromptMode::SaveBeforeNewGame);
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
                    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                    {
                        auto intent = Intent(WindowClass::Loadsave);
                        intent.PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE);
                        intent.PutExtra(INTENT_EXTRA_PATH, gScenarioName);
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
                    std::string url = "https://github.com/OpenRCT2/OpenRCT2/issues/"
                                      "new?assignees=&labels=bug&template=bug_report.yaml";
                    auto versionStr = String::URLEncode(gVersionInfoFull);
                    url.append("&openrct2_build=" + versionStr);
                    OpenRCT2::GetContext()->GetUiContext()->OpenURL(url);
                }
                break;
                case DDIDX_UPDATE_AVAILABLE:
                    ContextOpenWindowView(WV_NEW_VERSION_INFO);
                    break;
                case DDIDX_QUIT_TO_MENU:
                {
                    WindowCloseByClass(WindowClass::ManageTrackDesign);
                    WindowCloseByClass(WindowClass::TrackDeletePrompt);
                    auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::OpenSavePrompt, PromptMode::SaveBeforeQuit);
                    GameActions::Execute(&loadOrQuitAction);
                    break;
                }
                case DDIDX_EXIT_OPENRCT2:
                    ContextQuit();
                    break;
            }
            break;
        case WIDX_CHEATS:
            TopToolbarCheatsMenuDropdown(dropdownIndex);
            break;
        case WIDX_VIEW_MENU:
            TopToolbarViewMenuDropdown(dropdownIndex);
            break;
        case WIDX_MAP:
            TopToolbarMapMenuDropdown(dropdownIndex);
            break;
        case WIDX_FASTFORWARD:
            TopToolbarFastforwardMenuDropdown(dropdownIndex);
            break;
        case WIDX_ROTATE:
            TopToolbarRotateMenuDropdown(dropdownIndex);
            break;
        case WIDX_DEBUG:
            TopToolbarDebugMenuDropdown(dropdownIndex);
            break;
        case WIDX_NETWORK:
            TopToolbarNetworkMenuDropdown(dropdownIndex);
            break;
    }
}

/**
 *
 *  rct2: 0x0066C810
 */
static void WindowTopToolbarInvalidate(rct_window* w)
{
    int32_t x, widgetIndex, widgetWidth, firstAlignment;
    Widget* widget;

    // Enable / disable buttons
    window_top_toolbar_widgets[WIDX_PAUSE].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_FILE_MENU].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_ZOOM_OUT].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_ZOOM_IN].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_ROTATE].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_VIEW_MENU].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_MAP].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_MUTE].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_CHAT].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_LAND].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_WATER].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_SCENERY].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_PATH].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_CONSTRUCT_RIDE].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_RIDES].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_PARK].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_STAFF].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_GUESTS].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_CLEAR_SCENERY].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_FINANCES].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_RESEARCH].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_FASTFORWARD].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_CHEATS].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_DEBUG].type = gConfigGeneral.DebuggingTools ? WindowWidgetType::TrnBtn
                                                                                : WindowWidgetType::Empty;
    window_top_toolbar_widgets[WIDX_NEWS].type = WindowWidgetType::TrnBtn;
    window_top_toolbar_widgets[WIDX_NETWORK].type = WindowWidgetType::TrnBtn;

    if (!gConfigInterface.ToolbarShowMute)
        window_top_toolbar_widgets[WIDX_MUTE].type = WindowWidgetType::Empty;

    if (!gConfigInterface.ToolbarShowChat)
        window_top_toolbar_widgets[WIDX_CHAT].type = WindowWidgetType::Empty;

    if (!gConfigInterface.ToolbarShowResearch)
        window_top_toolbar_widgets[WIDX_RESEARCH].type = WindowWidgetType::Empty;

    if (!gConfigInterface.ToolbarShowCheats)
        window_top_toolbar_widgets[WIDX_CHEATS].type = WindowWidgetType::Empty;

    if (!gConfigInterface.ToolbarShowNews)
        window_top_toolbar_widgets[WIDX_NEWS].type = WindowWidgetType::Empty;

    if (!gConfigInterface.ToolbarShowZoom)
    {
        window_top_toolbar_widgets[WIDX_ZOOM_IN].type = WindowWidgetType::Empty;
        window_top_toolbar_widgets[WIDX_ZOOM_OUT].type = WindowWidgetType::Empty;
    }

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR || gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        window_top_toolbar_widgets[WIDX_PAUSE].type = WindowWidgetType::Empty;
    }

    if ((gParkFlags & PARK_FLAGS_NO_MONEY) || !gConfigInterface.ToolbarShowFinances)
        window_top_toolbar_widgets[WIDX_FINANCES].type = WindowWidgetType::Empty;

    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
    {
        window_top_toolbar_widgets[WIDX_PARK].type = WindowWidgetType::Empty;
        window_top_toolbar_widgets[WIDX_STAFF].type = WindowWidgetType::Empty;
        window_top_toolbar_widgets[WIDX_GUESTS].type = WindowWidgetType::Empty;
        window_top_toolbar_widgets[WIDX_FINANCES].type = WindowWidgetType::Empty;
        window_top_toolbar_widgets[WIDX_RESEARCH].type = WindowWidgetType::Empty;
        window_top_toolbar_widgets[WIDX_NEWS].type = WindowWidgetType::Empty;
        window_top_toolbar_widgets[WIDX_NETWORK].type = WindowWidgetType::Empty;

        if (gEditorStep != EditorStep::LandscapeEditor)
        {
            window_top_toolbar_widgets[WIDX_LAND].type = WindowWidgetType::Empty;
            window_top_toolbar_widgets[WIDX_WATER].type = WindowWidgetType::Empty;
        }

        if (gEditorStep != EditorStep::RollercoasterDesigner)
        {
            window_top_toolbar_widgets[WIDX_RIDES].type = WindowWidgetType::Empty;
            window_top_toolbar_widgets[WIDX_CONSTRUCT_RIDE].type = WindowWidgetType::Empty;
            window_top_toolbar_widgets[WIDX_FASTFORWARD].type = WindowWidgetType::Empty;
        }

        if (gEditorStep != EditorStep::LandscapeEditor && gEditorStep != EditorStep::RollercoasterDesigner)
        {
            window_top_toolbar_widgets[WIDX_MAP].type = WindowWidgetType::Empty;
            window_top_toolbar_widgets[WIDX_SCENERY].type = WindowWidgetType::Empty;
            window_top_toolbar_widgets[WIDX_PATH].type = WindowWidgetType::Empty;
            window_top_toolbar_widgets[WIDX_CLEAR_SCENERY].type = WindowWidgetType::Empty;

            window_top_toolbar_widgets[WIDX_ZOOM_OUT].type = WindowWidgetType::Empty;
            window_top_toolbar_widgets[WIDX_ZOOM_IN].type = WindowWidgetType::Empty;
            window_top_toolbar_widgets[WIDX_ROTATE].type = WindowWidgetType::Empty;
            window_top_toolbar_widgets[WIDX_VIEW_MENU].type = WindowWidgetType::Empty;
        }
    }

    switch (NetworkGetMode())
    {
        case NETWORK_MODE_NONE:
            window_top_toolbar_widgets[WIDX_NETWORK].type = WindowWidgetType::Empty;
            window_top_toolbar_widgets[WIDX_CHAT].type = WindowWidgetType::Empty;
            break;
        case NETWORK_MODE_CLIENT:
            window_top_toolbar_widgets[WIDX_PAUSE].type = WindowWidgetType::Empty;
            [[fallthrough]];
        case NETWORK_MODE_SERVER:
            window_top_toolbar_widgets[WIDX_FASTFORWARD].type = WindowWidgetType::Empty;
            break;
    }

    // Align left hand side toolbar buttons
    firstAlignment = 1;
    x = 0;
    for (size_t i = 0; i < std::size(left_aligned_widgets_order); ++i)
    {
        widgetIndex = left_aligned_widgets_order[i];
        widget = &window_top_toolbar_widgets[widgetIndex];
        if (widget->type == WindowWidgetType::Empty && widgetIndex != WIDX_SEPARATOR)
            continue;

        if (firstAlignment && widgetIndex == WIDX_SEPARATOR)
            continue;

        widgetWidth = widget->width();
        widget->left = x;
        x += widgetWidth;
        widget->right = x;
        x += 1;
        firstAlignment = 0;
    }

    // Align right hand side toolbar buttons
    int32_t screenWidth = ContextGetWidth();
    firstAlignment = 1;
    x = std::max(640, screenWidth);
    for (size_t i = 0; i < std::size(right_aligned_widgets_order); ++i)
    {
        widgetIndex = right_aligned_widgets_order[i];
        widget = &window_top_toolbar_widgets[widgetIndex];
        if (widget->type == WindowWidgetType::Empty && widgetIndex != WIDX_SEPARATOR)
            continue;

        if (firstAlignment && widgetIndex == WIDX_SEPARATOR)
            continue;

        widgetWidth = widget->width();
        x -= 1;
        widget->right = x;
        x -= widgetWidth;
        widget->left = x;
        firstAlignment = 0;
    }

    // Footpath button pressed down
    if (WindowFindByClass(WindowClass::Footpath) == nullptr)
        w->pressed_widgets &= ~(1uLL << WIDX_PATH);
    else
        w->pressed_widgets |= (1uLL << WIDX_PATH);

    if (gGamePaused & GAME_PAUSED_NORMAL)
        w->pressed_widgets |= (1uLL << WIDX_PAUSE);
    else
        w->pressed_widgets &= ~(1uLL << WIDX_PAUSE);

    if (!OpenRCT2::Audio::gGameSoundsOff)
        window_top_toolbar_widgets[WIDX_MUTE].image = ImageId(SPR_G2_TOOLBAR_MUTE, FilterPaletteID::PaletteNull);
    else
        window_top_toolbar_widgets[WIDX_MUTE].image = ImageId(SPR_G2_TOOLBAR_UNMUTE, FilterPaletteID::PaletteNull);

    // Set map button to the right image.
    if (window_top_toolbar_widgets[WIDX_MAP].type != WindowWidgetType::Empty)
    {
        static constexpr uint32_t _imageIdByRotation[] = {
            SPR_G2_MAP_NORTH,
            SPR_G2_MAP_WEST,
            SPR_G2_MAP_SOUTH,
            SPR_G2_MAP_EAST,
        };

        uint32_t mapImageId = _imageIdByRotation[GetCurrentRotation()];
        window_top_toolbar_widgets[WIDX_MAP].image = ImageId(mapImageId, FilterPaletteID::PaletteNull);
    }

    // Zoomed out/in disable. Not sure where this code is in the original.
    const auto* mainWindow = WindowGetMain();
    if (mainWindow == nullptr || mainWindow->viewport == nullptr)
    {
        LOG_ERROR("mainWindow or mainWindow->viewport is null!");
        return;
    }

    if (mainWindow->viewport->zoom == ZoomLevel::min())
    {
        w->disabled_widgets |= (1uLL << WIDX_ZOOM_IN);
    }
    else if (mainWindow->viewport->zoom >= ZoomLevel::max())
    {
        w->disabled_widgets |= (1uLL << WIDX_ZOOM_OUT);
    }
    else
    {
        w->disabled_widgets &= ~((1uLL << WIDX_ZOOM_IN) | (1uLL << WIDX_ZOOM_OUT));
    }
}

/**
 *
 *  rct2: 0x0066C8EC
 */
static void WindowTopToolbarPaint(rct_window* w, DrawPixelInfo* dpi)
{
    int32_t imgId;

    WindowDrawWidgets(*w, dpi);

    ScreenCoordsXY screenPos{};
    // Draw staff button image (setting masks to the staff colours)
    if (window_top_toolbar_widgets[WIDX_STAFF].type != WindowWidgetType::Empty)
    {
        screenPos = { w->windowPos.x + window_top_toolbar_widgets[WIDX_STAFF].left,
                      w->windowPos.y + window_top_toolbar_widgets[WIDX_STAFF].top };
        imgId = SPR_TOOLBAR_STAFF;
        if (WidgetIsPressed(*w, WIDX_STAFF))
            imgId++;
        GfxDrawSprite(dpi, ImageId(imgId, gStaffHandymanColour, gStaffMechanicColour), screenPos);
    }

    // Draw fast forward button
    if (window_top_toolbar_widgets[WIDX_FASTFORWARD].type != WindowWidgetType::Empty)
    {
        screenPos = { w->windowPos.x + window_top_toolbar_widgets[WIDX_FASTFORWARD].left + 0,
                      w->windowPos.y + window_top_toolbar_widgets[WIDX_FASTFORWARD].top + 0 };
        if (WidgetIsPressed(*w, WIDX_FASTFORWARD))
            screenPos.y++;
        GfxDrawSprite(dpi, ImageId(SPR_G2_FASTFORWARD), screenPos + ScreenCoordsXY{ 6, 3 });

        for (int32_t i = 0; i < gGameSpeed && gGameSpeed <= 4; i++)
        {
            GfxDrawSprite(dpi, ImageId(SPR_G2_SPEED_ARROW), screenPos + ScreenCoordsXY{ 5 + i * 5, 15 });
        }
        for (int32_t i = 0; i < 3 && i < gGameSpeed - 4 && gGameSpeed >= 5; i++)
        {
            GfxDrawSprite(dpi, ImageId(SPR_G2_HYPER_ARROW), screenPos + ScreenCoordsXY{ 5 + i * 6, 15 });
        }
    }

    // Draw cheats button
    if (window_top_toolbar_widgets[WIDX_CHEATS].type != WindowWidgetType::Empty)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_CHEATS].left - 1,
                              window_top_toolbar_widgets[WIDX_CHEATS].top - 1 };
        if (WidgetIsPressed(*w, WIDX_CHEATS))
            screenPos.y++;
        GfxDrawSprite(dpi, ImageId(SPR_G2_SANDBOX), screenPos);

        // Draw an overlay if clearance checks are disabled
        if (gCheatsDisableClearanceChecks)
        {
            DrawTextBasic(
                dpi, screenPos + ScreenCoordsXY{ 26, 2 }, STR_OVERLAY_CLEARANCE_CHECKS_DISABLED, {},
                { COLOUR_DARK_ORANGE | static_cast<uint8_t>(COLOUR_FLAG_OUTLINE), TextAlignment::RIGHT });
        }
    }

    // Draw chat button
    if (window_top_toolbar_widgets[WIDX_CHAT].type != WindowWidgetType::Empty)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_CHAT].left, window_top_toolbar_widgets[WIDX_CHAT].top - 2 };
        if (WidgetIsPressed(*w, WIDX_CHAT))
            screenPos.y++;
        GfxDrawSprite(dpi, ImageId(SPR_G2_CHAT), screenPos);
    }

    // Draw debug button
    if (window_top_toolbar_widgets[WIDX_DEBUG].type != WindowWidgetType::Empty)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_DEBUG].left, window_top_toolbar_widgets[WIDX_DEBUG].top - 1 };
        if (WidgetIsPressed(*w, WIDX_DEBUG))
            screenPos.y++;
        GfxDrawSprite(dpi, ImageId(SPR_TAB_GEARS_0), screenPos);
    }

    // Draw research button
    if (window_top_toolbar_widgets[WIDX_RESEARCH].type != WindowWidgetType::Empty)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_RESEARCH].left - 1,
                              window_top_toolbar_widgets[WIDX_RESEARCH].top };
        if (WidgetIsPressed(*w, WIDX_RESEARCH))
            screenPos.y++;
        GfxDrawSprite(dpi, ImageId(SPR_TAB_FINANCES_RESEARCH_0), screenPos);
    }

    // Draw finances button
    if (window_top_toolbar_widgets[WIDX_FINANCES].type != WindowWidgetType::Empty)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_FINANCES].left + 3,
                              window_top_toolbar_widgets[WIDX_FINANCES].top + 1 };
        if (WidgetIsPressed(*w, WIDX_FINANCES))
            screenPos.y++;
        GfxDrawSprite(dpi, ImageId(SPR_FINANCE), screenPos);
    }

    // Draw news button
    if (window_top_toolbar_widgets[WIDX_NEWS].type != WindowWidgetType::Empty)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_NEWS].left + 3, window_top_toolbar_widgets[WIDX_NEWS].top + 0 };
        if (WidgetIsPressed(*w, WIDX_NEWS))
            screenPos.y++;
        GfxDrawSprite(dpi, ImageId(SPR_G2_TAB_NEWS), screenPos);
    }

    // Draw network button
    if (window_top_toolbar_widgets[WIDX_NETWORK].type != WindowWidgetType::Empty)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_NETWORK].left + 3,
                              window_top_toolbar_widgets[WIDX_NETWORK].top + 0 };
        if (WidgetIsPressed(*w, WIDX_NETWORK))
            screenPos.y++;

        // Draw (de)sync icon.
        imgId = (NetworkIsDesynchronised() ? SPR_G2_MULTIPLAYER_DESYNC : SPR_G2_MULTIPLAYER_SYNC);
        GfxDrawSprite(dpi, ImageId(imgId), screenPos + ScreenCoordsXY{ 3, 11 });

        // Draw number of players.
        auto ft = Formatter();
        ft.Add<int32_t>(NetworkGetNumVisiblePlayers());
        DrawTextBasic(
            dpi, screenPos + ScreenCoordsXY{ 23, 1 }, STR_COMMA16, ft,
            { COLOUR_WHITE | static_cast<uint8_t>(COLOUR_FLAG_OUTLINE), TextAlignment::RIGHT });
    }
}

/**
 *
 *  rct2: 0x006E3158
 */
static void RepaintSceneryToolDown(const ScreenCoordsXY& windowPos, WidgetIndex widgetIndex)
{
    auto flags = EnumsToFlags(
        ViewportInteractionItem::Scenery, ViewportInteractionItem::Wall, ViewportInteractionItem::LargeScenery,
        ViewportInteractionItem::Banner);
    auto info = GetMapCoordinatesFromPos(windowPos, flags);
    switch (info.SpriteType)
    {
        case ViewportInteractionItem::Scenery:
        {
            auto* sceneryEntry = info.Element->AsSmallScenery()->GetEntry();

            // If can't repaint
            if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR | SMALL_SCENERY_FLAG_HAS_GLASS))
                return;

            uint8_t quadrant = info.Element->AsSmallScenery()->GetSceneryQuadrant();
            auto repaintScenery = SmallScenerySetColourAction(
                { info.Loc, info.Element->GetBaseZ() }, quadrant, info.Element->AsSmallScenery()->GetEntryIndex(),
                gWindowSceneryPrimaryColour, gWindowScenerySecondaryColour, gWindowSceneryTertiaryColour);

            GameActions::Execute(&repaintScenery);
            break;
        }
        case ViewportInteractionItem::Wall:
        {
            auto* scenery_entry = info.Element->AsWall()->GetEntry();

            // If can't repaint
            if (!(scenery_entry->flags & (WALL_SCENERY_HAS_PRIMARY_COLOUR | WALL_SCENERY_HAS_GLASS)))
                return;

            auto repaintScenery = WallSetColourAction(
                { info.Loc, info.Element->GetBaseZ(), info.Element->GetDirection() }, gWindowSceneryPrimaryColour,
                gWindowScenerySecondaryColour, gWindowSceneryTertiaryColour);

            GameActions::Execute(&repaintScenery);
            break;
        }
        case ViewportInteractionItem::LargeScenery:
        {
            auto* sceneryEntry = info.Element->AsLargeScenery()->GetEntry();

            // If can't repaint
            if (!(sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
                return;

            auto repaintScenery = LargeScenerySetColourAction(
                { info.Loc, info.Element->GetBaseZ(), info.Element->GetDirection() },
                info.Element->AsLargeScenery()->GetSequenceIndex(), gWindowSceneryPrimaryColour, gWindowScenerySecondaryColour,
                gWindowSceneryTertiaryColour);

            GameActions::Execute(&repaintScenery);
            break;
        }
        case ViewportInteractionItem::Banner:
        {
            auto banner = info.Element->AsBanner()->GetBanner();
            if (banner != nullptr)
            {
                auto* bannerEntry = GetBannerEntry(banner->type);
                if (bannerEntry->flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
                {
                    auto repaintScenery = BannerSetColourAction(
                        { info.Loc, info.Element->GetBaseZ(), info.Element->AsBanner()->GetPosition() },
                        gWindowSceneryPrimaryColour);

                    GameActions::Execute(&repaintScenery);
                }
            }
            break;
        }
        default:
            return;
    }
}

static void SceneryEyedropperToolDown(const ScreenCoordsXY& windowPos, WidgetIndex widgetIndex)
{
    auto flags = EnumsToFlags(
        ViewportInteractionItem::Scenery, ViewportInteractionItem::Wall, ViewportInteractionItem::LargeScenery,
        ViewportInteractionItem::Banner, ViewportInteractionItem::FootpathItem);
    auto info = GetMapCoordinatesFromPos(windowPos, flags);
    switch (info.SpriteType)
    {
        case ViewportInteractionItem::Scenery:
        {
            SmallSceneryElement* sceneryElement = info.Element->AsSmallScenery();
            auto entryIndex = sceneryElement->GetEntryIndex();
            auto* sceneryEntry = GetSmallSceneryEntry(entryIndex);
            if (sceneryEntry != nullptr)
            {
                WindowScenerySetSelectedItem(
                    { SCENERY_TYPE_SMALL, entryIndex }, sceneryElement->GetPrimaryColour(),
                    sceneryElement->GetSecondaryColour(), std::nullopt,
                    sceneryElement->GetDirectionWithOffset(GetCurrentRotation()));
            }
            break;
        }
        case ViewportInteractionItem::Wall:
        {
            auto entryIndex = info.Element->AsWall()->GetEntryIndex();
            auto* sceneryEntry = GetWallEntry(entryIndex);
            if (sceneryEntry != nullptr)
            {
                WindowScenerySetSelectedItem(
                    { SCENERY_TYPE_WALL, entryIndex }, info.Element->AsWall()->GetPrimaryColour(),
                    info.Element->AsWall()->GetSecondaryColour(), info.Element->AsWall()->GetTertiaryColour(), std::nullopt);
            }
            break;
        }
        case ViewportInteractionItem::LargeScenery:
        {
            auto entryIndex = info.Element->AsLargeScenery()->GetEntryIndex();
            auto* sceneryEntry = GetLargeSceneryEntry(entryIndex);
            if (sceneryEntry != nullptr)
            {
                WindowScenerySetSelectedItem(
                    { SCENERY_TYPE_LARGE, entryIndex }, info.Element->AsLargeScenery()->GetPrimaryColour(),
                    info.Element->AsLargeScenery()->GetSecondaryColour(), std::nullopt,
                    (GetCurrentRotation() + info.Element->GetDirection()) & 3);
            }
            break;
        }
        case ViewportInteractionItem::Banner:
        {
            auto banner = info.Element->AsBanner()->GetBanner();
            if (banner != nullptr)
            {
                auto sceneryEntry = GetBannerEntry(banner->type);
                if (sceneryEntry != nullptr)
                {
                    WindowScenerySetSelectedItem(
                        { SCENERY_TYPE_BANNER, banner->type }, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
                }
            }
            break;
        }
        case ViewportInteractionItem::FootpathItem:
        {
            auto entryIndex = info.Element->AsPath()->GetAdditionEntryIndex();
            auto* pathBitEntry = GetFootpathItemEntry(entryIndex);
            if (pathBitEntry != nullptr)
            {
                WindowScenerySetSelectedItem(
                    { SCENERY_TYPE_PATH_ITEM, entryIndex }, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
            }
            break;
        }
        default:
            break;
    }
}

static void Sub6E1F34UpdateScreenCoordsAndButtonsPressed(bool canRaiseItem, ScreenCoordsXY& screenPos)
{
    if (!canRaiseItem && !gCheatsDisableSupportLimits)
    {
        gSceneryCtrlPressed = false;
        gSceneryShiftPressed = false;
    }
    else
    {
        if (!gSceneryCtrlPressed)
        {
            if (InputTestPlaceObjectModifier(PLACE_OBJECT_MODIFIER_COPY_Z))
            {
                // CTRL pressed
                constexpr auto flags = EnumsToFlags(
                    ViewportInteractionItem::Terrain, ViewportInteractionItem::Ride, ViewportInteractionItem::Scenery,
                    ViewportInteractionItem::Footpath, ViewportInteractionItem::Wall, ViewportInteractionItem::LargeScenery);
                auto info = GetMapCoordinatesFromPos(screenPos, flags);

                if (info.SpriteType != ViewportInteractionItem::None)
                {
                    gSceneryCtrlPressed = true;
                    gSceneryCtrlPressZ = info.Element->GetBaseZ();
                }
            }
        }
        else
        {
            if (!(InputTestPlaceObjectModifier(PLACE_OBJECT_MODIFIER_COPY_Z)))
            {
                // CTRL not pressed
                gSceneryCtrlPressed = false;
            }
        }

        if (!gSceneryShiftPressed)
        {
            if (InputTestPlaceObjectModifier(PLACE_OBJECT_MODIFIER_SHIFT_Z))
            {
                // SHIFT pressed
                gSceneryShiftPressed = true;
                gSceneryShiftPressX = screenPos.x;
                gSceneryShiftPressY = screenPos.y;
                gSceneryShiftPressZOffset = 0;
            }
        }
        else
        {
            if (InputTestPlaceObjectModifier(PLACE_OBJECT_MODIFIER_SHIFT_Z))
            {
                // SHIFT pressed
                gSceneryShiftPressZOffset = (gSceneryShiftPressY - screenPos.y + 4);
                // Scale delta by zoom to match mouse position.
                auto* mainWnd = WindowGetMain();
                if (mainWnd != nullptr && mainWnd->viewport != nullptr)
                {
                    gSceneryShiftPressZOffset = mainWnd->viewport->zoom.ApplyTo(gSceneryShiftPressZOffset);
                }
                gSceneryShiftPressZOffset = Floor2(gSceneryShiftPressZOffset, 8);

                screenPos.x = gSceneryShiftPressX;
                screenPos.y = gSceneryShiftPressY;
            }
            else
            {
                // SHIFT not pressed
                gSceneryShiftPressed = false;
            }
        }
    }
}

static void Sub6E1F34SmallScenery(
    const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, uint8_t* outQuadrant,
    Direction* outRotation)
{
    rct_window* w = WindowFindByClass(WindowClass::Scenery);

    if (w == nullptr)
    {
        gridPos.SetNull();
        return;
    }

    auto screenPos = sourceScreenPos;
    uint16_t maxPossibleHeight = ZoomLevel::max().ApplyTo(std::numeric_limits<decltype(TileElement::base_height)>::max() - 32);
    bool can_raise_item = false;

    const auto* sceneryEntry = GetSmallSceneryEntry(sceneryIndex);
    if (sceneryEntry == nullptr)
    {
        gridPos.SetNull();
        return;
    }

    maxPossibleHeight -= sceneryEntry->height;
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_STACKABLE))
    {
        can_raise_item = true;
    }

    Sub6E1F34UpdateScreenCoordsAndButtonsPressed(can_raise_item, screenPos);

    // Small scenery
    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
    {
        uint8_t quadrant = 0;

        // If CTRL not pressed
        if (!gSceneryCtrlPressed)
        {
            auto gridCoords = ScreenGetMapXYQuadrant(screenPos, &quadrant);
            if (!gridCoords.has_value())
            {
                gridPos.SetNull();
                return;
            }
            gridPos = gridCoords.value();

            gSceneryPlaceZ = 0;

            // If SHIFT pressed
            if (gSceneryShiftPressed)
            {
                auto* surfaceElement = MapGetSurfaceElementAt(gridPos);

                if (surfaceElement == nullptr)
                {
                    gridPos.SetNull();
                    return;
                }

                int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
                z += gSceneryShiftPressZOffset;

                z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                gSceneryPlaceZ = z;
            }
        }
        else
        {
            int16_t z = gSceneryCtrlPressZ;

            auto mapCoords = ScreenGetMapXYQuadrantWithZ(screenPos, z, &quadrant);
            if (!mapCoords.has_value())
            {
                gridPos.SetNull();
                return;
            }
            gridPos = mapCoords.value();

            // If SHIFT pressed
            if (gSceneryShiftPressed)
            {
                z += gSceneryShiftPressZOffset;
            }

            z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

            gSceneryPlaceZ = z;
        }

        if (gridPos.IsNull())
            return;

        uint8_t rotation = gWindowSceneryRotation;

        if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
        {
            rotation = UtilRand() & 0xFF;
        }

        rotation -= GetCurrentRotation();
        rotation &= 0x3;

        if (gConfigGeneral.VirtualFloorStyle != VirtualFloorStyles::Off)
        {
            VirtualFloorSetHeight(gSceneryPlaceZ);
        }

        *outQuadrant = quadrant ^ 2;
        *outRotation = rotation;

        return;
    }

    // If CTRL not pressed
    if (!gSceneryCtrlPressed)
    {
        constexpr auto flags = EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Water);

        auto info = GetMapCoordinatesFromPos(screenPos, flags);
        gridPos = info.Loc;

        if (info.SpriteType == ViewportInteractionItem::None)
        {
            gridPos.SetNull();
            return;
        }

        // If CTRL and SHIFT not pressed
        gSceneryPlaceZ = 0;

        // If SHIFT pressed
        if (gSceneryShiftPressed)
        {
            auto surfaceElement = MapGetSurfaceElementAt(gridPos);

            if (surfaceElement == nullptr)
            {
                gridPos.SetNull();
                return;
            }

            int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
            z += gSceneryShiftPressZOffset;

            z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

            gSceneryPlaceZ = z;
        }
    }
    else
    {
        int16_t z = gSceneryCtrlPressZ;
        auto coords = ScreenGetMapXYWithZ(screenPos, z);
        if (coords.has_value())
        {
            gridPos = *coords;
        }
        else
        {
            gridPos.SetNull();
        }
        // If SHIFT pressed
        if (gSceneryShiftPressed)
        {
            z += gSceneryShiftPressZOffset;
        }

        z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

        gSceneryPlaceZ = z;
    }

    if (gridPos.IsNull())
        return;

    gridPos = gridPos.ToTileStart();
    uint8_t rotation = gWindowSceneryRotation;

    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
    {
        rotation = UtilRand() & 0xFF;
    }

    rotation -= GetCurrentRotation();
    rotation &= 0x3;

    if (gConfigGeneral.VirtualFloorStyle != VirtualFloorStyles::Off)
    {
        VirtualFloorSetHeight(gSceneryPlaceZ);
    }

    *outQuadrant = 0;
    *outRotation = rotation;
}

static void Sub6E1F34PathItem(
    const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, int32_t* outZ)
{
    rct_window* w = WindowFindByClass(WindowClass::Scenery);

    if (w == nullptr)
    {
        gridPos.SetNull();
        return;
    }

    auto screenPos = sourceScreenPos;
    Sub6E1F34UpdateScreenCoordsAndButtonsPressed(false, screenPos);

    // Path bits
    constexpr auto flags = EnumsToFlags(ViewportInteractionItem::Footpath, ViewportInteractionItem::FootpathItem);
    auto info = GetMapCoordinatesFromPos(screenPos, flags);
    gridPos = info.Loc;

    if (info.SpriteType == ViewportInteractionItem::None)
    {
        gridPos.SetNull();
        return;
    }

    if (gConfigGeneral.VirtualFloorStyle != VirtualFloorStyles::Off)
    {
        VirtualFloorSetHeight(gSceneryPlaceZ);
    }

    *outZ = info.Element->GetBaseZ();
}

static void Sub6E1F34Wall(
    const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, uint8_t* outEdges)
{
    rct_window* w = WindowFindByClass(WindowClass::Scenery);

    if (w == nullptr)
    {
        gridPos.SetNull();
        return;
    }

    auto screenPos = sourceScreenPos;
    uint16_t maxPossibleHeight = ZoomLevel::max().ApplyTo(std::numeric_limits<decltype(TileElement::base_height)>::max() - 32);

    auto* wallEntry = GetWallEntry(sceneryIndex);
    if (wallEntry != nullptr)
    {
        maxPossibleHeight -= wallEntry->height;
    }

    Sub6E1F34UpdateScreenCoordsAndButtonsPressed(true, screenPos);

    // Walls
    uint8_t edge;
    // If CTRL not pressed
    if (!gSceneryCtrlPressed)
    {
        auto gridCoords = ScreenGetMapXYSide(screenPos, &edge);
        if (!gridCoords.has_value())
        {
            gridPos.SetNull();
            return;
        }
        gridPos = gridCoords.value();

        gSceneryPlaceZ = 0;

        // If SHIFT pressed
        if (gSceneryShiftPressed)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(gridPos);

            if (surfaceElement == nullptr)
            {
                gridPos.SetNull();
                return;
            }

            int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
            z += gSceneryShiftPressZOffset;

            z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

            gSceneryPlaceZ = z;
        }
    }
    else
    {
        int16_t z = gSceneryCtrlPressZ;
        auto mapCoords = ScreenGetMapXYSideWithZ(screenPos, z, &edge);
        if (!mapCoords.has_value())
        {
            gridPos.SetNull();
            return;
        }
        gridPos = mapCoords.value();

        // If SHIFT pressed
        if (gSceneryShiftPressed)
        {
            z += gSceneryShiftPressZOffset;
        }

        z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

        gSceneryPlaceZ = z;
    }

    if (gridPos.IsNull())
        return;

    if (gConfigGeneral.VirtualFloorStyle != VirtualFloorStyles::Off)
    {
        VirtualFloorSetHeight(gSceneryPlaceZ);
    }

    *outEdges = edge;
}

static void Sub6E1F34LargeScenery(
    const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, Direction* outDirection)
{
    rct_window* w = WindowFindByClass(WindowClass::Scenery);

    if (w == nullptr)
    {
        gridPos.SetNull();
        return;
    }

    auto screenPos = sourceScreenPos;
    uint16_t maxPossibleHeight = ZoomLevel::max().ApplyTo(std::numeric_limits<decltype(TileElement::base_height)>::max() - 32);

    auto* sceneryEntry = GetLargeSceneryEntry(sceneryIndex);
    if (sceneryEntry)
    {
        int16_t maxClearZ = 0;
        for (int32_t i = 0; sceneryEntry->tiles[i].x_offset != -1; ++i)
        {
            maxClearZ = std::max<int16_t>(maxClearZ, sceneryEntry->tiles[i].z_clearance);
        }
        maxPossibleHeight = std::max(0, maxPossibleHeight - maxClearZ);
    }

    Sub6E1F34UpdateScreenCoordsAndButtonsPressed(true, screenPos);

    // Large scenery
    // If CTRL not pressed
    if (!gSceneryCtrlPressed)
    {
        const CoordsXY mapCoords = ViewportInteractionGetTileStartAtCursor(screenPos);
        gridPos = mapCoords;

        if (gridPos.IsNull())
            return;

        gSceneryPlaceZ = 0;

        // If SHIFT pressed
        if (gSceneryShiftPressed)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(gridPos);

            if (surfaceElement == nullptr)
            {
                gridPos.SetNull();
                return;
            }

            int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
            z += gSceneryShiftPressZOffset;

            z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

            gSceneryPlaceZ = z;
        }
    }
    else
    {
        int16_t z = gSceneryCtrlPressZ;
        auto coords = ScreenGetMapXYWithZ(screenPos, z);
        if (coords.has_value())
        {
            gridPos = *coords;
        }
        else
        {
            gridPos.SetNull();
        }

        // If SHIFT pressed
        if (gSceneryShiftPressed)
        {
            z += gSceneryShiftPressZOffset;
        }

        z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

        gSceneryPlaceZ = z;
    }

    if (gridPos.IsNull())
        return;

    gridPos = gridPos.ToTileStart();

    Direction rotation = gWindowSceneryRotation;
    rotation -= GetCurrentRotation();
    rotation &= 0x3;

    if (gConfigGeneral.VirtualFloorStyle != VirtualFloorStyles::Off)
    {
        VirtualFloorSetHeight(gSceneryPlaceZ);
    }

    *outDirection = rotation;
}

static void Sub6E1F34Banner(
    const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, int32_t* outZ,
    Direction* outDirection)
{
    rct_window* w = WindowFindByClass(WindowClass::Scenery);

    if (w == nullptr)
    {
        gridPos.SetNull();
        return;
    }

    auto screenPos = sourceScreenPos;
    Sub6E1F34UpdateScreenCoordsAndButtonsPressed(false, screenPos);

    // Banner
    constexpr auto flags = EnumsToFlags(ViewportInteractionItem::Footpath, ViewportInteractionItem::FootpathItem);
    auto info = GetMapCoordinatesFromPos(screenPos, flags);
    gridPos = info.Loc;

    if (info.SpriteType == ViewportInteractionItem::None)
    {
        gridPos.SetNull();
        return;
    }

    uint8_t rotation = gWindowSceneryRotation;
    rotation -= GetCurrentRotation();
    rotation &= 0x3;

    auto z = info.Element->GetBaseZ();

    if (info.Element->AsPath()->IsSloped())
    {
        if (rotation != DirectionReverse(info.Element->AsPath()->GetSlopeDirection()))
        {
            z += (2 * COORDS_Z_STEP);
        }
    }

    if (gConfigGeneral.VirtualFloorStyle != VirtualFloorStyles::Off)
    {
        VirtualFloorSetHeight(gSceneryPlaceZ);
    }

    *outDirection = rotation;
    *outZ = z;
}

/**
 *
 *  rct2: 0x006E2CC6
 */
static void WindowTopToolbarSceneryToolDown(const ScreenCoordsXY& windowPos, rct_window* w, WidgetIndex widgetIndex)
{
    SceneryRemoveGhostToolPlacement();
    if (gWindowSceneryPaintEnabled & 1)
    {
        RepaintSceneryToolDown(windowPos, widgetIndex);
        return;
    }

    if (gWindowSceneryEyedropperEnabled)
    {
        SceneryEyedropperToolDown(windowPos, widgetIndex);
        return;
    }

    auto selectedTab = WindowSceneryGetTabSelection();
    uint8_t sceneryType = selectedTab.SceneryType;
    uint16_t selectedScenery = selectedTab.EntryIndex;
    CoordsXY gridPos;

    switch (sceneryType)
    {
        case SCENERY_TYPE_SMALL:
        {
            uint8_t quadrant;
            Direction rotation;
            Sub6E1F34SmallScenery(windowPos, selectedScenery, gridPos, &quadrant, &rotation);
            if (gridPos.IsNull())
                return;

            int32_t quantity = 1;
            bool isCluster = gWindowSceneryScatterEnabled
                && (NetworkGetMode() != NETWORK_MODE_CLIENT
                    || NetworkCanPerformCommand(NetworkGetCurrentPlayerGroupIndex(), -2));

            if (isCluster)
            {
                switch (gWindowSceneryScatterDensity)
                {
                    case ScatterToolDensity::LowDensity:
                        quantity = gWindowSceneryScatterSize;
                        break;

                    case ScatterToolDensity::MediumDensity:
                        quantity = gWindowSceneryScatterSize * 2;
                        break;

                    case ScatterToolDensity::HighDensity:
                        quantity = gWindowSceneryScatterSize * 3;
                        break;
                }
            }

            bool forceError = true;
            for (int32_t q = 0; q < quantity; q++)
            {
                int32_t zCoordinate = gSceneryPlaceZ;
                auto* sceneryEntry = GetSmallSceneryEntry(selectedScenery);

                int16_t cur_grid_x = gridPos.x;
                int16_t cur_grid_y = gridPos.y;

                if (isCluster)
                {
                    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
                    {
                        quadrant = UtilRand() & 3;
                    }

                    int16_t grid_x_offset = (UtilRand() % gWindowSceneryScatterSize) - (gWindowSceneryScatterSize / 2);
                    int16_t grid_y_offset = (UtilRand() % gWindowSceneryScatterSize) - (gWindowSceneryScatterSize / 2);
                    if (gWindowSceneryScatterSize % 2 == 0)
                    {
                        grid_x_offset += 1;
                        grid_y_offset += 1;
                    }
                    cur_grid_x += grid_x_offset * COORDS_XY_STEP;
                    cur_grid_y += grid_y_offset * COORDS_XY_STEP;

                    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
                    {
                        gSceneryPlaceRotation = (gSceneryPlaceRotation + 1) & 3;
                    }
                }

                uint8_t zAttemptRange = 1;
                if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
                {
                    zAttemptRange = 20;
                }

                auto success = GameActions::Status::Unknown;
                // Try find a valid z coordinate
                for (; zAttemptRange != 0; zAttemptRange--)
                {
                    auto smallSceneryPlaceAction = SmallSceneryPlaceAction(
                        { cur_grid_x, cur_grid_y, gSceneryPlaceZ, gSceneryPlaceRotation }, quadrant, selectedScenery,
                        gWindowSceneryPrimaryColour, gWindowScenerySecondaryColour, gWindowSceneryTertiaryColour);
                    auto res = GameActions::Query(&smallSceneryPlaceAction);
                    success = res.Error;
                    if (res.Error == GameActions::Status::Ok)
                    {
                        break;
                    }

                    if (res.Error == GameActions::Status::InsufficientFunds)
                    {
                        break;
                    }
                    if (zAttemptRange != 1)
                    {
                        gSceneryPlaceZ += 8;
                    }
                }

                // Actually place
                if (success == GameActions::Status::Ok || ((q + 1 == quantity) && forceError))
                {
                    auto smallSceneryPlaceAction = SmallSceneryPlaceAction(
                        { cur_grid_x, cur_grid_y, gSceneryPlaceZ, gSceneryPlaceRotation }, quadrant, selectedScenery,
                        gWindowSceneryPrimaryColour, gWindowScenerySecondaryColour, gWindowSceneryTertiaryColour);

                    smallSceneryPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
                        if (result->Error == GameActions::Status::Ok)
                        {
                            OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
                        }
                    });
                    auto res = GameActions::Execute(&smallSceneryPlaceAction);
                    if (res.Error == GameActions::Status::Ok)
                    {
                        forceError = false;
                    }

                    if (res.Error == GameActions::Status::InsufficientFunds)
                    {
                        break;
                    }
                }
                gSceneryPlaceZ = zCoordinate;
            }
            break;
        }
        case SCENERY_TYPE_PATH_ITEM:
        {
            int32_t z;
            Sub6E1F34PathItem(windowPos, selectedScenery, gridPos, &z);
            if (gridPos.IsNull())
                return;

            auto footpathAdditionPlaceAction = FootpathAdditionPlaceAction({ gridPos, z }, selectedScenery + 1);

            footpathAdditionPlaceAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error != GameActions::Status::Ok)
                {
                    return;
                }
                OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
            });
            auto res = GameActions::Execute(&footpathAdditionPlaceAction);
            break;
        }
        case SCENERY_TYPE_WALL:
        {
            uint8_t edges;
            Sub6E1F34Wall(windowPos, selectedScenery, gridPos, &edges);
            if (gridPos.IsNull())
                return;

            uint8_t zAttemptRange = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                zAttemptRange = 20;
            }

            for (; zAttemptRange != 0; zAttemptRange--)
            {
                auto wallPlaceAction = WallPlaceAction(
                    selectedScenery, { gridPos, gSceneryPlaceZ }, edges, gWindowSceneryPrimaryColour,
                    gWindowScenerySecondaryColour, gWindowSceneryTertiaryColour);

                auto res = GameActions::Query(&wallPlaceAction);
                if (res.Error == GameActions::Status::Ok)
                {
                    break;
                }

                if (const auto* message = std::get_if<StringId>(&res.ErrorMessage))
                {
                    if (*message == STR_NOT_ENOUGH_CASH_REQUIRES || *message == STR_CAN_ONLY_BUILD_THIS_ON_WATER)
                    {
                        break;
                    }
                }

                if (zAttemptRange != 1)
                {
                    gSceneryPlaceZ += 8;
                }
            }

            auto wallPlaceAction = WallPlaceAction(
                selectedScenery, { gridPos, gSceneryPlaceZ }, edges, gWindowSceneryPrimaryColour, gWindowScenerySecondaryColour,
                gWindowSceneryTertiaryColour);

            wallPlaceAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error == GameActions::Status::Ok)
                {
                    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
                }
            });
            auto res = GameActions::Execute(&wallPlaceAction);
            break;
        }
        case SCENERY_TYPE_LARGE:
        {
            Direction direction;
            Sub6E1F34LargeScenery(windowPos, selectedScenery, gridPos, &direction);
            if (gridPos.IsNull())
                return;

            uint8_t zAttemptRange = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                zAttemptRange = 20;
            }

            for (; zAttemptRange != 0; zAttemptRange--)
            {
                CoordsXYZD loc = { gridPos, gSceneryPlaceZ, direction };

                auto sceneryPlaceAction = LargeSceneryPlaceAction(
                    loc, selectedScenery, gWindowSceneryPrimaryColour, gWindowScenerySecondaryColour,
                    gWindowSceneryTertiaryColour);

                auto res = GameActions::Query(&sceneryPlaceAction);
                if (res.Error == GameActions::Status::Ok)
                {
                    break;
                }

                if (const auto* message = std::get_if<StringId>(&res.ErrorMessage))
                {
                    if (*message == STR_NOT_ENOUGH_CASH_REQUIRES || *message == STR_CAN_ONLY_BUILD_THIS_ON_WATER)
                    {
                        break;
                    }
                }

                if (zAttemptRange != 1)
                {
                    gSceneryPlaceZ += 8;
                }
            }

            CoordsXYZD loc = { gridPos, gSceneryPlaceZ, direction };

            auto sceneryPlaceAction = LargeSceneryPlaceAction(
                loc, selectedScenery, gWindowSceneryPrimaryColour, gWindowScenerySecondaryColour, gWindowSceneryTertiaryColour);
            sceneryPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error == GameActions::Status::Ok)
                {
                    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
                }
                else
                {
                    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::Error, { loc.x, loc.y, gSceneryPlaceZ });
                }
            });
            auto res = GameActions::Execute(&sceneryPlaceAction);
            break;
        }
        case SCENERY_TYPE_BANNER:
        {
            int32_t z;
            Direction direction;
            Sub6E1F34Banner(windowPos, selectedScenery, gridPos, &z, &direction);
            if (gridPos.IsNull())
                return;

            CoordsXYZD loc{ gridPos, z, direction };
            auto primaryColour = gWindowSceneryPrimaryColour;
            auto bannerPlaceAction = BannerPlaceAction(loc, selectedScenery, primaryColour);
            bannerPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error == GameActions::Status::Ok)
                {
                    auto data = result->GetData<BannerPlaceActionResult>();
                    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
                    ContextOpenDetailWindow(WD_BANNER, data.bannerId.ToUnderlying());
                }
            });
            GameActions::Execute(&bannerPlaceAction);
            break;
        }
    }
}

static uint8_t TopToolbarToolUpdateLandPaint(const ScreenCoordsXY& screenPos)
{
    uint8_t state_changed = 0;

    MapInvalidateSelectionRect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

    auto mapTile = ScreenGetMapXY(screenPos, nullptr);

    if (!mapTile.has_value())
    {
        if (gClearSceneryCost != MONEY64_UNDEFINED)
        {
            gClearSceneryCost = MONEY64_UNDEFINED;
            WindowInvalidateByClass(WindowClass::ClearScenery);
        }
        return state_changed;
    }

    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
    {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        state_changed++;
    }

    if (gMapSelectType != MAP_SELECT_TYPE_FULL)
    {
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        state_changed++;
    }

    int16_t tool_size = std::max<uint16_t>(1, gLandToolSize);
    int16_t tool_length = (tool_size - 1) * 32;

    // Move to tool bottom left
    mapTile->x -= (tool_size - 1) * 16;
    mapTile->y -= (tool_size - 1) * 16;
    mapTile = mapTile->ToTileStart();

    if (gMapSelectPositionA.x != mapTile->x)
    {
        gMapSelectPositionA.x = mapTile->x;
        state_changed++;
    }

    if (gMapSelectPositionA.y != mapTile->y)
    {
        gMapSelectPositionA.y = mapTile->y;
        state_changed++;
    }

    mapTile->x += tool_length;
    mapTile->y += tool_length;

    if (gMapSelectPositionB.x != mapTile->x)
    {
        gMapSelectPositionB.x = mapTile->x;
        state_changed++;
    }

    if (gMapSelectPositionB.y != mapTile->y)
    {
        gMapSelectPositionB.y = mapTile->y;
        state_changed++;
    }

    MapInvalidateSelectionRect();
    return state_changed;
}

/**
 *
 *  rct2: 0x0068E213
 */
static void TopToolbarToolUpdateSceneryClear(const ScreenCoordsXY& screenPos)
{
    if (!TopToolbarToolUpdateLandPaint(screenPos))
        return;

    auto action = GetClearAction();
    auto result = GameActions::Query(&action);
    auto cost = (result.Error == GameActions::Status::Ok ? result.Cost : MONEY64_UNDEFINED);
    if (gClearSceneryCost != cost)
    {
        gClearSceneryCost = cost;
        WindowInvalidateByClass(WindowClass::ClearScenery);
    }
}

/**
 *
 *  rct2: 0x00664280
 */
static void TopToolbarToolUpdateLand(const ScreenCoordsXY& screenPos)
{
    const bool mapCtrlPressed = InputTestPlaceObjectModifier(PLACE_OBJECT_MODIFIER_COPY_Z);

    MapInvalidateSelectionRect();

    if (gCurrentToolId == Tool::UpDownArrow)
    {
        if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
            return;

        money64 lower_cost = SelectionLowerLand(0);
        money64 raise_cost = SelectionRaiseLand(0);

        if (gLandToolRaiseCost != raise_cost || gLandToolLowerCost != lower_cost)
        {
            gLandToolRaiseCost = raise_cost;
            gLandToolLowerCost = lower_cost;
            WindowInvalidateByClass(WindowClass::Land);
        }
        return;
    }

    int16_t tool_size = gLandToolSize;
    std::optional<CoordsXY> mapTile;
    uint8_t side{};

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    if (tool_size == 1)
    {
        int32_t selectionType;
        // Get selection type and map coordinates from mouse x,y position
        ScreenPosToMapPos(screenPos, &selectionType);
        mapTile = ScreenGetMapXYSide(screenPos, &side);

        if (!mapTile.has_value())
        {
            money64 lower_cost = MONEY64_UNDEFINED;
            money64 raise_cost = MONEY64_UNDEFINED;

            if (gLandToolRaiseCost != raise_cost || gLandToolLowerCost != lower_cost)
            {
                gLandToolRaiseCost = raise_cost;
                gLandToolLowerCost = lower_cost;
                WindowInvalidateByClass(WindowClass::Land);
            }
            return;
        }

        uint8_t state_changed = 0;

        if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
        {
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
            state_changed++;
        }

        if (gMapSelectType != selectionType)
        {
            gMapSelectType = selectionType;
            state_changed++;
        }

        if ((gMapSelectType != MAP_SELECT_TYPE_EDGE_0 + (side & 0xFF)) && mapCtrlPressed)
        {
            gMapSelectType = MAP_SELECT_TYPE_EDGE_0 + (side & 0xFF);
            state_changed++;
        }

        if (gMapSelectPositionA.x != mapTile->x)
        {
            gMapSelectPositionA.x = mapTile->x;
            state_changed++;
        }

        if (gMapSelectPositionA.y != mapTile->y)
        {
            gMapSelectPositionA.y = mapTile->y;
            state_changed++;
        }

        if (gMapSelectPositionB.x != mapTile->x)
        {
            gMapSelectPositionB.x = mapTile->x;
            state_changed++;
        }

        if (gMapSelectPositionB.y != mapTile->y)
        {
            gMapSelectPositionB.y = mapTile->y;
            state_changed++;
        }

        MapInvalidateSelectionRect();
        if (!state_changed)
            return;

        money64 lower_cost = SelectionLowerLand(0);
        money64 raise_cost = SelectionRaiseLand(0);

        if (gLandToolRaiseCost != raise_cost || gLandToolLowerCost != lower_cost)
        {
            gLandToolRaiseCost = raise_cost;
            gLandToolLowerCost = lower_cost;
            WindowInvalidateByClass(WindowClass::Land);
        }
        return;
    }

    // Get map coordinates and the side of the tile that is being hovered over
    mapTile = ScreenGetMapXYSide(screenPos, &side);

    if (!mapTile.has_value())
    {
        money64 lower_cost = MONEY64_UNDEFINED;
        money64 raise_cost = MONEY64_UNDEFINED;

        if (gLandToolRaiseCost != raise_cost || gLandToolLowerCost != lower_cost)
        {
            gLandToolRaiseCost = raise_cost;
            gLandToolLowerCost = lower_cost;
            WindowInvalidateByClass(WindowClass::Land);
        }
        return;
    }

    uint8_t state_changed = 0;

    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
    {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        state_changed++;
    }

    if (gMapSelectType != MAP_SELECT_TYPE_FULL)
    {
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        state_changed++;
    }

    if ((gMapSelectType != MAP_SELECT_TYPE_EDGE_0 + (side & 0xFF)) && mapCtrlPressed)
    {
        gMapSelectType = MAP_SELECT_TYPE_EDGE_0 + (side & 0xFF);
        state_changed++;
    }

    if (tool_size == 0)
        tool_size = 1;

    int16_t tool_length = (tool_size - 1) * 32;

    // Decide on shape of the brush for bigger selection size
    switch (gMapSelectType)
    {
        case MAP_SELECT_TYPE_EDGE_0:
        case MAP_SELECT_TYPE_EDGE_2:
            // Line
            mapTile->y -= (tool_size - 1) * 16;
            mapTile->y = mapTile->ToTileStart().y;
            break;
        case MAP_SELECT_TYPE_EDGE_1:
        case MAP_SELECT_TYPE_EDGE_3:
            // Line
            mapTile->x -= (tool_size - 1) * 16;
            mapTile->x = mapTile->ToTileStart().x;
            break;
        default:
            // Move to tool bottom left
            mapTile->x -= (tool_size - 1) * 16;
            mapTile->y -= (tool_size - 1) * 16;
            mapTile = mapTile->ToTileStart();
            break;
    }

    if (gMapSelectPositionA.x != mapTile->x)
    {
        gMapSelectPositionA.x = mapTile->x;
        state_changed++;
    }

    if (gMapSelectPositionA.y != mapTile->y)
    {
        gMapSelectPositionA.y = mapTile->y;
        state_changed++;
    }

    // Go to other side
    switch (gMapSelectType)
    {
        case MAP_SELECT_TYPE_EDGE_0:
        case MAP_SELECT_TYPE_EDGE_2:
            // Line
            mapTile->y += tool_length;
            gMapSelectType = MAP_SELECT_TYPE_FULL;
            break;
        case MAP_SELECT_TYPE_EDGE_1:
        case MAP_SELECT_TYPE_EDGE_3:
            // Line
            mapTile->x += tool_length;
            gMapSelectType = MAP_SELECT_TYPE_FULL;
            break;
        default:
            mapTile->x += tool_length;
            mapTile->y += tool_length;
            break;
    }

    if (gMapSelectPositionB.x != mapTile->x)
    {
        gMapSelectPositionB.x = mapTile->x;
        state_changed++;
    }

    if (gMapSelectPositionB.y != mapTile->y)
    {
        gMapSelectPositionB.y = mapTile->y;
        state_changed++;
    }

    MapInvalidateSelectionRect();
    if (!state_changed)
        return;

    money64 lower_cost = SelectionLowerLand(0);
    money64 raise_cost = SelectionRaiseLand(0);

    if (gLandToolRaiseCost != raise_cost || gLandToolLowerCost != lower_cost)
    {
        gLandToolRaiseCost = raise_cost;
        gLandToolLowerCost = lower_cost;
        WindowInvalidateByClass(WindowClass::Land);
    }
}

/**
 *
 *  rct2: 0x006E6BDC
 */
static void TopToolbarToolUpdateWater(const ScreenCoordsXY& screenPos)
{
    MapInvalidateSelectionRect();

    if (gCurrentToolId == Tool::UpDownArrow)
    {
        if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
            return;

        auto waterLowerAction = WaterLowerAction(
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
        auto waterRaiseAction = WaterRaiseAction(
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });

        auto res = GameActions::Query(&waterLowerAction);
        money64 lowerCost = res.Error == GameActions::Status::Ok ? res.Cost : MONEY64_UNDEFINED;

        res = GameActions::Query(&waterRaiseAction);
        money64 raiseCost = res.Error == GameActions::Status::Ok ? res.Cost : MONEY64_UNDEFINED;

        if (gWaterToolRaiseCost != raiseCost || gWaterToolLowerCost != lowerCost)
        {
            gWaterToolRaiseCost = raiseCost;
            gWaterToolLowerCost = lowerCost;
            WindowInvalidateByClass(WindowClass::Water);
        }
        return;
    }

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

    auto info = GetMapCoordinatesFromPos(
        screenPos, EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Water));

    if (info.SpriteType == ViewportInteractionItem::None)
    {
        if (gWaterToolRaiseCost != MONEY64_UNDEFINED || gWaterToolLowerCost != MONEY64_UNDEFINED)
        {
            gWaterToolRaiseCost = MONEY64_UNDEFINED;
            gWaterToolLowerCost = MONEY64_UNDEFINED;
            WindowInvalidateByClass(WindowClass::Water);
        }
        return;
    }

    auto mapTile = info.Loc.ToTileCentre();

    uint8_t state_changed = 0;

    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
    {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        state_changed++;
    }

    if (gMapSelectType != MAP_SELECT_TYPE_FULL_WATER)
    {
        gMapSelectType = MAP_SELECT_TYPE_FULL_WATER;
        state_changed++;
    }

    int16_t tool_size = std::max<uint16_t>(1, gLandToolSize);
    int16_t tool_length = (tool_size - 1) * 32;

    // Move to tool bottom left
    mapTile.x -= (tool_size - 1) * 16;
    mapTile.y -= (tool_size - 1) * 16;
    mapTile.x &= 0xFFE0;
    mapTile.y &= 0xFFE0;

    if (gMapSelectPositionA.x != mapTile.x)
    {
        gMapSelectPositionA.x = mapTile.x;
        state_changed++;
    }

    if (gMapSelectPositionA.y != mapTile.y)
    {
        gMapSelectPositionA.y = mapTile.y;
        state_changed++;
    }

    mapTile.x += tool_length;
    mapTile.y += tool_length;

    if (gMapSelectPositionB.x != mapTile.x)
    {
        gMapSelectPositionB.x = mapTile.x;
        state_changed++;
    }

    if (gMapSelectPositionB.y != mapTile.y)
    {
        gMapSelectPositionB.y = mapTile.y;
        state_changed++;
    }

    MapInvalidateSelectionRect();
    if (!state_changed)
        return;

    auto waterLowerAction = WaterLowerAction(
        { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
    auto waterRaiseAction = WaterRaiseAction(
        { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });

    auto res = GameActions::Query(&waterLowerAction);
    money64 lowerCost = res.Error == GameActions::Status::Ok ? res.Cost : MONEY64_UNDEFINED;

    res = GameActions::Query(&waterRaiseAction);
    money64 raiseCost = res.Error == GameActions::Status::Ok ? res.Cost : MONEY64_UNDEFINED;

    if (gWaterToolRaiseCost != raiseCost || gWaterToolLowerCost != lowerCost)
    {
        gWaterToolRaiseCost = raiseCost;
        gWaterToolLowerCost = lowerCost;
        WindowInvalidateByClass(WindowClass::Water);
    }
}

/**
 *
 *  rct2: 0x006E24F6
 * On failure returns MONEY64_UNDEFINED
 * On success places ghost scenery and returns cost to place proper
 */
static money64 TryPlaceGhostSmallScenery(
    CoordsXYZD loc, uint8_t quadrant, ObjectEntryIndex entryIndex, colour_t primaryColour, colour_t secondaryColour,
    colour_t tertiaryColour)
{
    SceneryRemoveGhostToolPlacement();

    // 6e252b
    auto smallSceneryPlaceAction = SmallSceneryPlaceAction(
        loc, quadrant, entryIndex, primaryColour, secondaryColour, tertiaryColour);
    smallSceneryPlaceAction.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
    auto res = GameActions::Execute(&smallSceneryPlaceAction);
    if (res.Error != GameActions::Status::Ok)
        return MONEY64_UNDEFINED;

    const auto placementData = res.GetData<SmallSceneryPlaceActionResult>();

    gSceneryPlaceRotation = loc.direction;
    gSceneryPlaceObject.SceneryType = SCENERY_TYPE_SMALL;
    gSceneryPlaceObject.EntryIndex = entryIndex;

    gSceneryGhostPosition = { loc, placementData.BaseHeight };
    gSceneryQuadrant = placementData.SceneryQuadrant;
    if (placementData.GroundFlags & ELEMENT_IS_UNDERGROUND)
    {
        // Set underground on
        ViewportSetVisibility(4);
    }
    else
    {
        // Set underground off
        ViewportSetVisibility(5);
    }

    gSceneryGhostType |= SCENERY_GHOST_FLAG_0;
    return res.Cost;
}

static money64 TryPlaceGhostPathAddition(CoordsXYZ loc, ObjectEntryIndex entryIndex)
{
    SceneryRemoveGhostToolPlacement();

    // 6e265b
    auto footpathAdditionPlaceAction = FootpathAdditionPlaceAction(loc, entryIndex + 1);
    footpathAdditionPlaceAction.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
    footpathAdditionPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error != GameActions::Status::Ok)
        {
            return;
        }
        gSceneryGhostPosition = loc;
        gSceneryGhostType |= SCENERY_GHOST_FLAG_1;
    });
    auto res = GameActions::Execute(&footpathAdditionPlaceAction);
    if (res.Error != GameActions::Status::Ok)
        return MONEY64_UNDEFINED;

    return res.Cost;
}

static money64 TryPlaceGhostWall(
    CoordsXYZ loc, uint8_t edge, ObjectEntryIndex entryIndex, colour_t primaryColour, colour_t secondaryColour,
    colour_t tertiaryColour)
{
    SceneryRemoveGhostToolPlacement();

    // 6e26b0
    auto wallPlaceAction = WallPlaceAction(entryIndex, loc, edge, primaryColour, secondaryColour, tertiaryColour);
    wallPlaceAction.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND);
    wallPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error != GameActions::Status::Ok)
            return;

        const auto placementData = result->GetData<WallPlaceActionResult>();
        gSceneryGhostPosition = { loc, placementData.BaseHeight };
        gSceneryGhostWallRotation = edge;

        gSceneryGhostType |= SCENERY_GHOST_FLAG_2;
    });

    auto res = GameActions::Execute(&wallPlaceAction);
    if (res.Error != GameActions::Status::Ok)
        return MONEY64_UNDEFINED;

    return res.Cost;
}

static money64 TryPlaceGhostLargeScenery(
    CoordsXYZD loc, ObjectEntryIndex entryIndex, colour_t primaryColour, colour_t secondaryColour, colour_t tertiaryColour)
{
    SceneryRemoveGhostToolPlacement();

    // 6e25a7
    auto sceneryPlaceAction = LargeSceneryPlaceAction(loc, entryIndex, primaryColour, secondaryColour, tertiaryColour);
    sceneryPlaceAction.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND);
    auto res = GameActions::Execute(&sceneryPlaceAction);
    if (res.Error != GameActions::Status::Ok)
        return MONEY64_UNDEFINED;

    const auto placementData = res.GetData<LargeSceneryPlaceActionResult>();

    gSceneryPlaceRotation = loc.direction;

    gSceneryGhostPosition = { loc, placementData.firstTileHeight };
    if (placementData.GroundFlags & ELEMENT_IS_UNDERGROUND)
    {
        // Set underground on
        ViewportSetVisibility(4);
    }
    else
    {
        // Set underground off
        ViewportSetVisibility(5);
    }

    gSceneryGhostType |= SCENERY_GHOST_FLAG_3;
    return res.Cost;
}

static money64 TryPlaceGhostBanner(CoordsXYZD loc, ObjectEntryIndex entryIndex)
{
    SceneryRemoveGhostToolPlacement();

    // 6e2612
    auto primaryColour = gWindowSceneryPrimaryColour;
    auto bannerPlaceAction = BannerPlaceAction(loc, entryIndex, primaryColour);
    bannerPlaceAction.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND);
    auto res = GameActions::Execute(&bannerPlaceAction);
    if (res.Error != GameActions::Status::Ok)
        return MONEY64_UNDEFINED;

    gSceneryGhostPosition = loc;
    gSceneryGhostPosition.z += PATH_HEIGHT_STEP;
    gSceneryPlaceRotation = loc.direction;
    gSceneryGhostType |= SCENERY_GHOST_FLAG_4;
    return res.Cost;
}

/**
 *
 *  rct2: 0x006E287B
 */
static void TopToolbarToolUpdateScenery(const ScreenCoordsXY& screenPos)
{
    MapInvalidateSelectionRect();
    MapInvalidateMapSelectionTiles();

    if (gConfigGeneral.VirtualFloorStyle != VirtualFloorStyles::Off)
    {
        VirtualFloorInvalidate();
    }

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;

    if (gWindowSceneryPaintEnabled)
        return;
    if (gWindowSceneryEyedropperEnabled)
        return;

    const auto selection = WindowSceneryGetTabSelection();
    if (selection.IsUndefined())
    {
        SceneryRemoveGhostToolPlacement();
        return;
    }

    money64 cost = 0;

    switch (selection.SceneryType)
    {
        case SCENERY_TYPE_SMALL:
        {
            CoordsXY mapTile = {};
            uint8_t quadrant;
            Direction rotation;

            Sub6E1F34SmallScenery(screenPos, selection.EntryIndex, mapTile, &quadrant, &rotation);

            if (mapTile.IsNull())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
            if (gWindowSceneryScatterEnabled)
            {
                uint16_t cluster_size = (gWindowSceneryScatterSize - 1) * COORDS_XY_STEP;
                gMapSelectPositionA.x = mapTile.x - cluster_size / 2;
                gMapSelectPositionA.y = mapTile.y - cluster_size / 2;
                gMapSelectPositionB.x = mapTile.x + cluster_size / 2;
                gMapSelectPositionB.y = mapTile.y + cluster_size / 2;
                if (gWindowSceneryScatterSize % 2 == 0)
                {
                    gMapSelectPositionB.x += COORDS_XY_STEP;
                    gMapSelectPositionB.y += COORDS_XY_STEP;
                }
            }
            else
            {
                gMapSelectPositionA.x = mapTile.x;
                gMapSelectPositionA.y = mapTile.y;
                gMapSelectPositionB.x = mapTile.x;
                gMapSelectPositionB.y = mapTile.y;
            }

            auto* sceneryEntry = GetSmallSceneryEntry(selection.EntryIndex);

            gMapSelectType = MAP_SELECT_TYPE_FULL;
            if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE) && !gWindowSceneryScatterEnabled)
            {
                gMapSelectType = MAP_SELECT_TYPE_QUARTER_0 + (quadrant ^ 2);
            }

            MapInvalidateSelectionRect();

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_0) && mapTile == gSceneryGhostPosition && quadrant == _unkF64F0E
                && gSceneryPlaceZ == _unkF64F0A && gSceneryPlaceObject.SceneryType == SCENERY_TYPE_SMALL
                && gSceneryPlaceObject.EntryIndex == selection.EntryIndex)
            {
                return;
            }

            SceneryRemoveGhostToolPlacement();

            _unkF64F0E = quadrant;
            _unkF64F0A = gSceneryPlaceZ;

            uint8_t attemptsLeft = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                attemptsLeft = 20;
            }

            for (; attemptsLeft != 0; attemptsLeft--)
            {
                cost = TryPlaceGhostSmallScenery(
                    { mapTile, gSceneryPlaceZ, rotation }, quadrant, selection.EntryIndex, gWindowSceneryPrimaryColour,
                    gWindowScenerySecondaryColour, gWindowSceneryTertiaryColour);

                if (cost != MONEY64_UNDEFINED)
                    break;
                gSceneryPlaceZ += 8;
            }

            gSceneryPlaceCost = cost;
            break;
        }
        case SCENERY_TYPE_PATH_ITEM:
        {
            CoordsXY mapTile = {};
            int32_t z;

            Sub6E1F34PathItem(screenPos, selection.EntryIndex, mapTile, &z);

            if (mapTile.IsNull())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
            gMapSelectPositionA.x = mapTile.x;
            gMapSelectPositionA.y = mapTile.y;
            gMapSelectPositionB.x = mapTile.x;
            gMapSelectPositionB.y = mapTile.y;
            gMapSelectType = MAP_SELECT_TYPE_FULL;

            MapInvalidateSelectionRect();

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_1) && mapTile == gSceneryGhostPosition && z == gSceneryGhostPosition.z)
            {
                return;
            }

            SceneryRemoveGhostToolPlacement();

            cost = TryPlaceGhostPathAddition({ mapTile, z }, selection.EntryIndex);

            gSceneryPlaceCost = cost;
            break;
        }
        case SCENERY_TYPE_WALL:
        {
            CoordsXY mapTile = {};
            uint8_t edge;

            Sub6E1F34Wall(screenPos, selection.EntryIndex, mapTile, &edge);

            if (mapTile.IsNull())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
            gMapSelectPositionA.x = mapTile.x;
            gMapSelectPositionA.y = mapTile.y;
            gMapSelectPositionB.x = mapTile.x;
            gMapSelectPositionB.y = mapTile.y;
            gMapSelectType = MAP_SELECT_TYPE_EDGE_0 + edge;

            MapInvalidateSelectionRect();

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_2) && mapTile == gSceneryGhostPosition
                && edge == gSceneryGhostWallRotation && gSceneryPlaceZ == _unkF64F0A)
            {
                return;
            }

            SceneryRemoveGhostToolPlacement();

            gSceneryGhostWallRotation = edge;
            _unkF64F0A = gSceneryPlaceZ;

            uint8_t attemptsLeft = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                attemptsLeft = 20;
            }

            cost = 0;
            for (; attemptsLeft != 0; attemptsLeft--)
            {
                cost = TryPlaceGhostWall(
                    { mapTile, gSceneryPlaceZ }, edge, selection.EntryIndex, gWindowSceneryPrimaryColour,
                    gWindowScenerySecondaryColour, gWindowSceneryTertiaryColour);

                if (cost != MONEY64_UNDEFINED)
                    break;
                gSceneryPlaceZ += 8;
            }

            gSceneryPlaceCost = cost;
            break;
        }
        case SCENERY_TYPE_LARGE:
        {
            CoordsXY mapTile = {};
            Direction direction;

            Sub6E1F34LargeScenery(screenPos, selection.EntryIndex, mapTile, &direction);

            if (mapTile.IsNull())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            auto* sceneryEntry = GetLargeSceneryEntry(selection.EntryIndex);
            gMapSelectionTiles.clear();

            for (rct_large_scenery_tile* tile = sceneryEntry->tiles;
                 tile->x_offset != static_cast<int16_t>(static_cast<uint16_t>(0xFFFF)); tile++)
            {
                CoordsXY tileLocation = { tile->x_offset, tile->y_offset };
                auto rotatedTileCoords = tileLocation.Rotate(direction);

                rotatedTileCoords.x += mapTile.x;
                rotatedTileCoords.y += mapTile.y;

                gMapSelectionTiles.push_back(rotatedTileCoords);
            }

            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            MapInvalidateMapSelectionTiles();

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_3) && mapTile == gSceneryGhostPosition && gSceneryPlaceZ == _unkF64F0A
                && gSceneryPlaceObject.SceneryType == SCENERY_TYPE_LARGE
                && gSceneryPlaceObject.EntryIndex == selection.EntryIndex)
            {
                return;
            }

            SceneryRemoveGhostToolPlacement();

            gSceneryPlaceObject.SceneryType = SCENERY_TYPE_LARGE;
            gSceneryPlaceObject.EntryIndex = selection.EntryIndex;
            _unkF64F0A = gSceneryPlaceZ;

            uint8_t attemptsLeft = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                attemptsLeft = 20;
            }

            cost = 0;
            for (; attemptsLeft != 0; attemptsLeft--)
            {
                cost = TryPlaceGhostLargeScenery(
                    { mapTile, gSceneryPlaceZ, direction }, selection.EntryIndex, gWindowSceneryPrimaryColour,
                    gWindowScenerySecondaryColour, gWindowSceneryTertiaryColour);

                if (cost != MONEY64_UNDEFINED)
                    break;
                gSceneryPlaceZ += COORDS_Z_STEP;
            }

            gSceneryPlaceCost = cost;
            break;
        }
        case SCENERY_TYPE_BANNER:
        {
            CoordsXY mapTile = {};
            Direction direction;
            int32_t z;

            Sub6E1F34Banner(screenPos, selection.EntryIndex, mapTile, &z, &direction);

            if (mapTile.IsNull())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
            gMapSelectPositionA.x = mapTile.x;
            gMapSelectPositionA.y = mapTile.y;
            gMapSelectPositionB.x = mapTile.x;
            gMapSelectPositionB.y = mapTile.y;
            gMapSelectType = MAP_SELECT_TYPE_FULL;

            MapInvalidateSelectionRect();

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_4) && mapTile == gSceneryGhostPosition && z == gSceneryGhostPosition.z
                && direction == gSceneryPlaceRotation)
            {
                return;
            }

            SceneryRemoveGhostToolPlacement();

            cost = TryPlaceGhostBanner({ mapTile, z, direction }, selection.EntryIndex);

            gSceneryPlaceCost = cost;
            break;
        }
    }
}

/**
 *
 *  rct2: 0x0066CB25
 */
static void WindowTopToolbarToolUpdate(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_CLEAR_SCENERY:
            TopToolbarToolUpdateSceneryClear(screenCoords);
            break;
        case WIDX_LAND:
            if (gLandPaintMode)
                TopToolbarToolUpdateLandPaint(screenCoords);
            else
                TopToolbarToolUpdateLand(screenCoords);
            break;
        case WIDX_WATER:
            TopToolbarToolUpdateWater(screenCoords);
            break;
        case WIDX_SCENERY:
            TopToolbarToolUpdateScenery(screenCoords);
            break;
#ifdef ENABLE_SCRIPTING
        default:
            auto& customTool = OpenRCT2::Scripting::ActiveCustomTool;
            if (customTool)
            {
                customTool->OnUpdate(screenCoords);
            }
            break;
#endif
    }
}

/**
 *
 *  rct2: 0x0066CB73
 */
static void WindowTopToolbarToolDown(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_CLEAR_SCENERY:
            if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
            {
                auto action = GetClearAction();
                GameActions::Execute(&action);
                gCurrentToolId = Tool::Crosshair;
            }
            break;
        case WIDX_LAND:
            if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
            {
                auto surfaceSetStyleAction = SurfaceSetStyleAction(
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                    gLandToolTerrainSurface, gLandToolTerrainEdge);

                GameActions::Execute(&surfaceSetStyleAction);

                gCurrentToolId = Tool::UpDownArrow;
            }
            else
            {
                _landToolBlocked = true;
            }
            break;
        case WIDX_WATER:
            if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
            {
                gCurrentToolId = Tool::UpDownArrow;
            }
            else
            {
                _landToolBlocked = true;
            }
            break;
        case WIDX_SCENERY:
            WindowTopToolbarSceneryToolDown(screenCoords, w, widgetIndex);
            break;
#ifdef ENABLE_SCRIPTING
        default:
            auto& customTool = OpenRCT2::Scripting::ActiveCustomTool;
            if (customTool)
            {
                customTool->OnDown(screenCoords);
            }
            break;
#endif
    }
}

/**
 *
 *  rct2: 0x006644DD
 */
static money64 SelectionRaiseLand(uint8_t flags)
{
    int32_t centreX = (gMapSelectPositionA.x + gMapSelectPositionB.x) / 2;
    int32_t centreY = (gMapSelectPositionA.y + gMapSelectPositionB.y) / 2;
    centreX += 16;
    centreY += 16;

    if (gLandMountainMode)
    {
        auto landSmoothAction = LandSmoothAction(
            { centreX, centreY },
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y }, gMapSelectType,
            false);
        auto res = (flags & GAME_COMMAND_FLAG_APPLY) ? GameActions::Execute(&landSmoothAction)
                                                     : GameActions::Query(&landSmoothAction);
        return res.Error == GameActions::Status::Ok ? res.Cost : MONEY64_UNDEFINED;
    }

    auto landRaiseAction = LandRaiseAction(
        { centreX, centreY }, { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
        gMapSelectType);
    auto res = (flags & GAME_COMMAND_FLAG_APPLY) ? GameActions::Execute(&landRaiseAction)
                                                 : GameActions::Query(&landRaiseAction);

    return res.Error == GameActions::Status::Ok ? res.Cost : MONEY64_UNDEFINED;
}

/**
 *
 *  rct2: 0x006645B3
 */
static money64 SelectionLowerLand(uint8_t flags)
{
    int32_t centreX = (gMapSelectPositionA.x + gMapSelectPositionB.x) / 2;
    int32_t centreY = (gMapSelectPositionA.y + gMapSelectPositionB.y) / 2;
    centreX += 16;
    centreY += 16;

    if (gLandMountainMode)
    {
        auto landSmoothAction = LandSmoothAction(
            { centreX, centreY },
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y }, gMapSelectType,
            true);
        auto res = (flags & GAME_COMMAND_FLAG_APPLY) ? GameActions::Execute(&landSmoothAction)
                                                     : GameActions::Query(&landSmoothAction);
        return res.Error == GameActions::Status::Ok ? res.Cost : MONEY64_UNDEFINED;
    }

    auto landLowerAction = LandLowerAction(
        { centreX, centreY }, { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
        gMapSelectType);
    auto res = (flags & GAME_COMMAND_FLAG_APPLY) ? GameActions::Execute(&landLowerAction)
                                                 : GameActions::Query(&landLowerAction);

    return res.Error == GameActions::Status::Ok ? res.Cost : MONEY64_UNDEFINED;
}

/**
 *  part of window_top_toolbar_tool_drag(0x0066CB4E)
 *  rct2: 0x00664454
 */
static void WindowTopToolbarLandToolDrag(const ScreenCoordsXY& screenPos)
{
    rct_window* window = WindowFindFromPoint(screenPos);
    if (window == nullptr)
        return;
    WidgetIndex widget_index = WindowFindWidgetFromPoint(*window, screenPos);
    if (widget_index == -1)
        return;
    const auto& widget = window->widgets[widget_index];
    if (widget.type != WindowWidgetType::Viewport)
        return;
    rct_viewport* viewport = window->viewport;
    if (viewport == nullptr)
        return;

    int16_t tile_height = viewport->zoom.ApplyInversedTo(-16);

    int32_t y_diff = screenPos.y - gInputDragLast.y;

    if (y_diff <= tile_height)
    {
        gInputDragLast.y += tile_height;

        SelectionRaiseLand(GAME_COMMAND_FLAG_APPLY);

        gLandToolRaiseCost = MONEY64_UNDEFINED;
        gLandToolLowerCost = MONEY64_UNDEFINED;
    }
    else if (y_diff >= -tile_height)
    {
        gInputDragLast.y -= tile_height;

        SelectionLowerLand(GAME_COMMAND_FLAG_APPLY);

        gLandToolRaiseCost = MONEY64_UNDEFINED;
        gLandToolLowerCost = MONEY64_UNDEFINED;
    }
}

/**
 *  part of window_top_toolbar_tool_drag(0x0066CB4E)
 *  rct2: 0x006E6D4B
 */
static void WindowTopToolbarWaterToolDrag(const ScreenCoordsXY& screenPos)
{
    rct_window* window = WindowFindFromPoint(screenPos);
    if (!window)
        return;
    WidgetIndex widget_index = WindowFindWidgetFromPoint(*window, screenPos);
    if (widget_index == -1)
        return;
    const auto& widget = window->widgets[widget_index];
    if (widget.type != WindowWidgetType::Viewport)
        return;
    rct_viewport* viewport = window->viewport;
    if (viewport == nullptr)
        return;

    int16_t dx = viewport->zoom.ApplyInversedTo(-16);

    auto offsetPos = screenPos - ScreenCoordsXY{ 0, gInputDragLast.y };

    if (offsetPos.y <= dx)
    {
        gInputDragLast.y += dx;

        auto waterRaiseAction = WaterRaiseAction(
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
        GameActions::Execute(&waterRaiseAction);

        gWaterToolRaiseCost = MONEY64_UNDEFINED;
        gWaterToolLowerCost = MONEY64_UNDEFINED;

        return;
    }

    dx = -dx;

    if (offsetPos.y >= dx)
    {
        gInputDragLast.y += dx;

        auto waterLowerAction = WaterLowerAction(
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
        GameActions::Execute(&waterLowerAction);
        gWaterToolRaiseCost = MONEY64_UNDEFINED;
        gWaterToolLowerCost = MONEY64_UNDEFINED;

        return;
    }
}

/**
 *
 *  rct2: 0x0066CB4E
 */
static void WindowTopToolbarToolDrag(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_CLEAR_SCENERY:
            if (WindowFindByClass(WindowClass::Error) == nullptr && (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
            {
                auto action = GetClearAction();
                GameActions::Execute(&action);
                gCurrentToolId = Tool::Crosshair;
            }
            break;
        case WIDX_LAND:
            // Custom setting to only change land style instead of raising or lowering land
            if (gLandPaintMode)
            {
                if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
                {
                    auto surfaceSetStyleAction = SurfaceSetStyleAction(
                        { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                        gLandToolTerrainSurface, gLandToolTerrainEdge);

                    GameActions::Execute(&surfaceSetStyleAction);

                    // The tool is set to 12 here instead of 3 so that the dragging cursor is not the elevation change
                    // cursor
                    gCurrentToolId = Tool::Crosshair;
                }
            }
            else
            {
                if (!_landToolBlocked)
                {
                    WindowTopToolbarLandToolDrag(screenCoords);
                }
            }
            break;
        case WIDX_WATER:
            if (!_landToolBlocked)
            {
                WindowTopToolbarWaterToolDrag(screenCoords);
            }
            break;
        case WIDX_SCENERY:
            if (gWindowSceneryPaintEnabled & 1)
                WindowTopToolbarSceneryToolDown(screenCoords, w, widgetIndex);
            if (gWindowSceneryEyedropperEnabled)
                WindowTopToolbarSceneryToolDown(screenCoords, w, widgetIndex);
            break;
#ifdef ENABLE_SCRIPTING
        default:
            auto& customTool = OpenRCT2::Scripting::ActiveCustomTool;
            if (customTool)
            {
                customTool->OnDrag(screenCoords);
            }
            break;
#endif
    }
}

/**
 *
 *  rct2: 0x0066CC5B
 */
static void WindowTopToolbarToolUp(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    _landToolBlocked = false;
    switch (widgetIndex)
    {
        case WIDX_LAND:
            MapInvalidateSelectionRect();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            gCurrentToolId = Tool::DigDown;
            break;
        case WIDX_WATER:
            MapInvalidateSelectionRect();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            gCurrentToolId = Tool::WaterDown;
            break;
        case WIDX_CLEAR_SCENERY:
            MapInvalidateSelectionRect();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            gCurrentToolId = Tool::Crosshair;
            break;
#ifdef ENABLE_SCRIPTING
        default:
            auto& customTool = OpenRCT2::Scripting::ActiveCustomTool;
            if (customTool)
            {
                customTool->OnUp(screenCoords);
            }
            break;
#endif
    }
}

/**
 *
 *  rct2: 0x0066CA58
 */
static void WindowTopToolbarToolAbort(rct_window* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_LAND:
        case WIDX_WATER:
        case WIDX_CLEAR_SCENERY:
            HideGridlines();
            break;
#ifdef ENABLE_SCRIPTING
        default:
            auto& customTool = OpenRCT2::Scripting::ActiveCustomTool;
            if (customTool)
            {
                customTool->OnAbort();
                customTool = {};
            }
            break;
#endif
    }
}

static void TopToolbarInitMapMenu(rct_window* w, Widget* widget)
{
    auto i = 0;
    gDropdownItems[i++].Format = STR_SHORTCUT_SHOW_MAP;
    gDropdownItems[i++].Format = STR_EXTRA_VIEWPORT;
    if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && gEditorStep == EditorStep::LandscapeEditor)
    {
        gDropdownItems[i++].Format = STR_MAPGEN_WINDOW_TITLE;
    }

#ifdef ENABLE_SCRIPTING
    const auto& customMenuItems = OpenRCT2::Scripting::CustomMenuItems;
    if (!customMenuItems.empty())
    {
        gDropdownItems[i++].Format = STR_EMPTY;
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
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1] | 0x80, 0, i);
    gDropdownDefaultIndex = DDIDX_SHOW_MAP;
}

static void TopToolbarMapMenuDropdown(int16_t dropdownIndex)
{
    int32_t customStartIndex = 3;
    if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && gEditorStep == EditorStep::LandscapeEditor)
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

static void TopToolbarInitFastforwardMenu(rct_window* w, Widget* widget)
{
    int32_t num_items = 4;
    gDropdownItems[0].Format = STR_TOGGLE_OPTION;
    gDropdownItems[1].Format = STR_TOGGLE_OPTION;
    gDropdownItems[2].Format = STR_TOGGLE_OPTION;
    gDropdownItems[3].Format = STR_TOGGLE_OPTION;
    if (gConfigGeneral.DebuggingTools)
    {
        gDropdownItems[4].Format = STR_EMPTY;
        gDropdownItems[5].Format = STR_TOGGLE_OPTION;
        gDropdownItems[5].Args = STR_SPEED_HYPER;
        num_items = 6;
    }

    gDropdownItems[0].Args = STR_SPEED_NORMAL;
    gDropdownItems[1].Args = STR_SPEED_QUICK;
    gDropdownItems[2].Args = STR_SPEED_FAST;
    gDropdownItems[3].Args = STR_SPEED_TURBO;

    WindowDropdownShowText(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[0] | 0x80, 0,
        num_items);

    // Set checkmarks
    if (gGameSpeed <= 4)
    {
        Dropdown::SetChecked(gGameSpeed - 1, true);
    }
    if (gGameSpeed == 8)
    {
        Dropdown::SetChecked(5, true);
    }

    if (gConfigGeneral.DebuggingTools)
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

static void TopToolbarFastforwardMenuDropdown(int16_t dropdownIndex)
{
    rct_window* w = WindowGetMain();
    if (w)
    {
        if (dropdownIndex >= 0 && dropdownIndex <= 5)
        {
            gGameSpeed = dropdownIndex + 1;
            if (gGameSpeed >= 5)
                gGameSpeed = 8;
            w->Invalidate();
        }
    }
}

static void TopToolbarInitRotateMenu(rct_window* w, Widget* widget)
{
    gDropdownItems[0].Format = STR_ROTATE_CLOCKWISE;
    gDropdownItems[1].Format = STR_ROTATE_ANTI_CLOCKWISE;

    WindowDropdownShowText(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1] | 0x80, 0, 2);

    gDropdownDefaultIndex = DDIDX_ROTATE_CLOCKWISE;
}

static void TopToolbarRotateMenuDropdown(int16_t dropdownIndex)
{
    rct_window* w = WindowGetMain();
    if (w)
    {
        if (dropdownIndex == 0)
        {
            WindowRotateCamera(*w, 1);
            w->Invalidate();
        }
        else if (dropdownIndex == 1)
        {
            WindowRotateCamera(*w, -1);
            w->Invalidate();
        }
    }
}

static void TopToolbarInitCheatsMenu(rct_window* w, Widget* widget)
{
    using namespace Dropdown;

    constexpr ItemExt items[] = {
        ToggleOption(DDIDX_CHEATS, STR_CHEAT_TITLE),
        ToggleOption(DDIDX_TILE_INSPECTOR, STR_DEBUG_DROPDOWN_TILE_INSPECTOR),
        ToggleOption(DDIDX_OBJECT_SELECTION, STR_DEBUG_DROPDOWN_OBJECT_SELECTION),
        ToggleOption(DDIDX_INVENTIONS_LIST, STR_DEBUG_DROPDOWN_INVENTIONS_LIST),
        ToggleOption(DDIDX_SCENARIO_OPTIONS, STR_DEBUG_DROPDOWN_SCENARIO_OPTIONS),
        ToggleOption(DDIDX_OBJECTIVE_OPTIONS, STR_CHEATS_MENU_OBJECTIVE_OPTIONS),
        Separator(),
        ToggleOption(DDIDX_ENABLE_SANDBOX_MODE, STR_ENABLE_SANDBOX_MODE),
        ToggleOption(DDIDX_DISABLE_CLEARANCE_CHECKS, STR_DISABLE_CLEARANCE_CHECKS),
        ToggleOption(DDIDX_DISABLE_SUPPORT_LIMITS, STR_DISABLE_SUPPORT_LIMITS),
    };
    static_assert(ItemIDsMatchIndices(items));

    SetItems(items);

    WindowDropdownShowText(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[0] | 0x80, 0,
        TOP_TOOLBAR_CHEATS_COUNT);

    // Disable items that are not yet available in multiplayer
    if (NetworkGetMode() != NETWORK_MODE_NONE)
    {
        Dropdown::SetDisabled(DDIDX_OBJECT_SELECTION, true);
        Dropdown::SetDisabled(DDIDX_INVENTIONS_LIST, true);
        Dropdown::SetDisabled(DDIDX_OBJECTIVE_OPTIONS, true);
    }

    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
    {
        Dropdown::SetDisabled(DDIDX_OBJECT_SELECTION, true);
        Dropdown::SetDisabled(DDIDX_INVENTIONS_LIST, true);
        Dropdown::SetDisabled(DDIDX_SCENARIO_OPTIONS, true);
        Dropdown::SetDisabled(DDIDX_OBJECTIVE_OPTIONS, true);
        Dropdown::SetDisabled(DDIDX_ENABLE_SANDBOX_MODE, true);
    }

    if (gCheatsSandboxMode)
    {
        Dropdown::SetChecked(DDIDX_ENABLE_SANDBOX_MODE, true);
    }
    if (gCheatsDisableClearanceChecks)
    {
        Dropdown::SetChecked(DDIDX_DISABLE_CLEARANCE_CHECKS, true);
    }
    if (gCheatsDisableSupportLimits)
    {
        Dropdown::SetChecked(DDIDX_DISABLE_SUPPORT_LIMITS, true);
    }

    gDropdownDefaultIndex = DDIDX_CHEATS;
}

static void TopToolbarCheatsMenuDropdown(int16_t dropdownIndex)
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
            WindowCloseAll();
            ContextOpenWindow(WindowClass::EditorObjectSelection);
            break;
        case DDIDX_INVENTIONS_LIST:
            ContextOpenWindow(WindowClass::EditorInventionList);
            break;
        case DDIDX_SCENARIO_OPTIONS:
            ContextOpenWindow(WindowClass::EditorScenarioOptions);
            break;
        case DDIDX_OBJECTIVE_OPTIONS:
            ContextOpenWindow(WindowClass::EditorObjectiveOptions);
            break;
        case DDIDX_ENABLE_SANDBOX_MODE:
            CheatsSet(CheatType::SandboxMode, !gCheatsSandboxMode);
            break;
        case DDIDX_DISABLE_CLEARANCE_CHECKS:
            CheatsSet(CheatType::DisableClearanceChecks, !gCheatsDisableClearanceChecks);
            break;
        case DDIDX_DISABLE_SUPPORT_LIMITS:
            CheatsSet(CheatType::DisableSupportLimits, !gCheatsDisableSupportLimits);
            break;
    }
}

static void TopToolbarInitDebugMenu(rct_window* w, Widget* widget)
{
    gDropdownItems[DDIDX_CONSOLE].Format = STR_TOGGLE_OPTION;
    gDropdownItems[DDIDX_CONSOLE].Args = STR_DEBUG_DROPDOWN_CONSOLE;
    gDropdownItems[DDIDX_DEBUG_PAINT].Format = STR_TOGGLE_OPTION;
    gDropdownItems[DDIDX_DEBUG_PAINT].Args = STR_DEBUG_DROPDOWN_DEBUG_PAINT;

    WindowDropdownShowText(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[0] | 0x80,
        Dropdown::Flag::StayOpen, TOP_TOOLBAR_DEBUG_COUNT);

    Dropdown::SetChecked(DDIDX_DEBUG_PAINT, WindowFindByClass(WindowClass::DebugPaint) != nullptr);
}

static void TopToolbarInitNetworkMenu(rct_window* w, Widget* widget)
{
    gDropdownItems[DDIDX_MULTIPLAYER].Format = STR_MULTIPLAYER;
    gDropdownItems[DDIDX_MULTIPLAYER_RECONNECT].Format = STR_MULTIPLAYER_RECONNECT;

    WindowDropdownShowText(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[0] | 0x80, 0,
        TOP_TOOLBAR_NETWORK_COUNT);

    Dropdown::SetDisabled(DDIDX_MULTIPLAYER_RECONNECT, !NetworkIsDesynchronised());

    gDropdownDefaultIndex = DDIDX_MULTIPLAYER;
}

static void TopToolbarDebugMenuDropdown(int16_t dropdownIndex)
{
    rct_window* w = WindowGetMain();
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
                if (WindowFindByClass(WindowClass::DebugPaint) == nullptr)
                {
                    ContextOpenWindow(WindowClass::DebugPaint);
                }
                else
                {
                    WindowCloseByClass(WindowClass::DebugPaint);
                }
                break;
        }
    }
}

static void TopToolbarNetworkMenuDropdown(int16_t dropdownIndex)
{
    rct_window* w = WindowGetMain();
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

/**
 *
 *  rct2: 0x0066CDE4
 */
static void TopToolbarInitViewMenu(rct_window* w, Widget* widget)
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
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1] | 0x80, 0,
        TOP_TOOLBAR_VIEW_MENU_COUNT);

    // Set checkmarks
    rct_viewport* mainViewport = WindowGetMain()->viewport;
    if (mainViewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
        Dropdown::SetChecked(DDIDX_UNDERGROUND_INSIDE, true);
    if (gConfigGeneral.TransparentWater)
        Dropdown::SetChecked(DDIDX_TRANSPARENT_WATER, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_BASE)
        Dropdown::SetChecked(DDIDX_HIDE_BASE, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_VERTICAL)
        Dropdown::SetChecked(DDIDX_HIDE_VERTICAL, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_RIDES)
        Dropdown::SetChecked(DDIDX_HIDE_RIDES, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_VEHICLES)
        Dropdown::SetChecked(DDIDX_HIDE_VEHICLES, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_VEGETATION)
        Dropdown::SetChecked(DDIDX_HIDE_VEGETATION, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_SCENERY)
        Dropdown::SetChecked(DDIDX_HIDE_SCENERY, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_PATHS)
        Dropdown::SetChecked(DDIDX_HIDE_PATHS, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_SUPPORTS)
        Dropdown::SetChecked(DDIDX_HIDE_SUPPORTS, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_GUESTS)
        Dropdown::SetChecked(DDIDX_HIDE_GUESTS, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_STAFF)
        Dropdown::SetChecked(DDIDX_HIDE_STAFF, true);
    if (mainViewport->flags & VIEWPORT_FLAG_LAND_HEIGHTS)
        Dropdown::SetChecked(DDIDX_LAND_HEIGHTS, true);
    if (mainViewport->flags & VIEWPORT_FLAG_TRACK_HEIGHTS)
        Dropdown::SetChecked(DDIDX_TRACK_HEIGHTS, true);
    if (mainViewport->flags & VIEWPORT_FLAG_PATH_HEIGHTS)
        Dropdown::SetChecked(DDIDX_PATH_HEIGHTS, true);
    if (mainViewport->flags & VIEWPORT_FLAG_CLIP_VIEW)
        Dropdown::SetChecked(DDIDX_VIEW_CLIPPING, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
        Dropdown::SetChecked(DDIDX_HIGHLIGHT_PATH_ISSUES, true);

    gDropdownDefaultIndex = DDIDX_UNDERGROUND_INSIDE;

    // Opaque water relies on RCT1 sprites.
    if (!IsCsgLoaded())
    {
        Dropdown::SetDisabled(DDIDX_TRANSPARENT_WATER, true);
    }
}

/**
 *
 *  rct2: 0x0066CF8A
 */
static void TopToolbarViewMenuDropdown(int16_t dropdownIndex)
{
    rct_window* w = WindowGetMain();
    if (w != nullptr)
    {
        switch (dropdownIndex)
        {
            case DDIDX_UNDERGROUND_INSIDE:
                w->viewport->flags ^= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
                break;
            case DDIDX_TRANSPARENT_WATER:
                gConfigGeneral.TransparentWater ^= 1;
                ConfigSaveDefault();
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
                if (WindowFindByClass(WindowClass::ViewClipping) == nullptr)
                {
                    ContextOpenWindow(WindowClass::ViewClipping);
                }
                else
                {
                    // If window is already open, toggle the view clipping on/off
                    w->viewport->flags ^= VIEWPORT_FLAG_CLIP_VIEW;
                }
                break;
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

/**
 *
 *  rct2: 0x0066CCE7
 */
static void ToggleFootpathWindow()
{
    if (WindowFindByClass(WindowClass::Footpath) == nullptr)
    {
        ContextOpenWindow(WindowClass::Footpath);
    }
    else
    {
        ToolCancel();
        WindowCloseByClass(WindowClass::Footpath);
    }
}

/**
 *
 *  rct2: 0x0066CD54
 */
static void ToggleLandWindow(rct_window* topToolbar, WidgetIndex widgetIndex)
{
    if ((InputTestFlag(INPUT_FLAG_TOOL_ACTIVE)) && gCurrentToolWidget.window_classification == WindowClass::TopToolbar
        && gCurrentToolWidget.widget_index == WIDX_LAND)
    {
        ToolCancel();
    }
    else
    {
        _landToolBlocked = false;
        ShowGridlines();
        ToolSet(*topToolbar, widgetIndex, Tool::DigDown);
        InputSetFlag(INPUT_FLAG_6, true);
        ContextOpenWindow(WindowClass::Land);
    }
}

/**
 *
 *  rct2: 0x0066CD0C
 */
static void ToggleClearSceneryWindow(rct_window* topToolbar, WidgetIndex widgetIndex)
{
    if ((InputTestFlag(INPUT_FLAG_TOOL_ACTIVE) && gCurrentToolWidget.window_classification == WindowClass::TopToolbar
         && gCurrentToolWidget.widget_index == WIDX_CLEAR_SCENERY))
    {
        ToolCancel();
    }
    else
    {
        ShowGridlines();
        ToolSet(*topToolbar, widgetIndex, Tool::Crosshair);
        InputSetFlag(INPUT_FLAG_6, true);
        ContextOpenWindow(WindowClass::ClearScenery);
    }
}

/**
 *
 *  rct2: 0x0066CD9C
 */
static void ToggleWaterWindow(rct_window* topToolbar, WidgetIndex widgetIndex)
{
    if ((InputTestFlag(INPUT_FLAG_TOOL_ACTIVE)) && gCurrentToolWidget.window_classification == WindowClass::TopToolbar
        && gCurrentToolWidget.widget_index == WIDX_WATER)
    {
        ToolCancel();
    }
    else
    {
        _landToolBlocked = false;
        ShowGridlines();
        ToolSet(*topToolbar, widgetIndex, Tool::WaterDown);
        InputSetFlag(INPUT_FLAG_6, true);
        ContextOpenWindow(WindowClass::Water);
    }
}

/**
 *
 *  rct2: 0x0066D104
 */
bool LandToolIsActive()
{
    if (!(InputTestFlag(INPUT_FLAG_TOOL_ACTIVE)))
        return false;
    if (gCurrentToolWidget.window_classification != WindowClass::TopToolbar)
        return false;
    if (gCurrentToolWidget.widget_index != WIDX_LAND)
        return false;
    return true;
}

/**
 *
 *  rct2: 0x0066D125
 */
bool ClearSceneryToolIsActive()
{
    if (!(InputTestFlag(INPUT_FLAG_TOOL_ACTIVE)))
        return false;
    if (gCurrentToolWidget.window_classification != WindowClass::TopToolbar)
        return false;
    if (gCurrentToolWidget.widget_index != WIDX_CLEAR_SCENERY)
        return false;
    return true;
}

/**
 *
 *  rct2: 0x0066D125
 */
bool WaterToolIsActive()
{
    if (!(InputTestFlag(INPUT_FLAG_TOOL_ACTIVE)))
        return false;
    if (gCurrentToolWidget.window_classification != WindowClass::TopToolbar)
        return false;
    if (gCurrentToolWidget.widget_index != WIDX_WATER)
        return false;
    return true;
}

static ClearAction GetClearAction()
{
    auto range = MapRange(gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y);

    ClearableItems itemsToClear = 0;

    if (gClearSmallScenery)
        itemsToClear |= CLEARABLE_ITEMS::SCENERY_SMALL;
    if (gClearLargeScenery)
        itemsToClear |= CLEARABLE_ITEMS::SCENERY_LARGE;
    if (gClearFootpath)
        itemsToClear |= CLEARABLE_ITEMS::SCENERY_FOOTPATH;

    return ClearAction(range, itemsToClear);
}
