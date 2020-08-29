/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include <openrct2/actions/BannerPlaceAction.hpp>
#include <openrct2/actions/BannerSetColourAction.hpp>
#include <openrct2/actions/ClearAction.hpp>
#include <openrct2/actions/FootpathSceneryPlaceAction.hpp>
#include <openrct2/actions/LandLowerAction.hpp>
#include <openrct2/actions/LandRaiseAction.hpp>
#include <openrct2/actions/LandSmoothAction.hpp>
#include <openrct2/actions/LargeSceneryPlaceAction.hpp>
#include <openrct2/actions/LargeScenerySetColourAction.hpp>
#include <openrct2/actions/LoadOrQuitAction.hpp>
#include <openrct2/actions/PauseToggleAction.hpp>
#include <openrct2/actions/SetCheatAction.hpp>
#include <openrct2/actions/SmallSceneryPlaceAction.hpp>
#include <openrct2/actions/SmallScenerySetColourAction.hpp>
#include <openrct2/actions/SurfaceSetStyleAction.hpp>
#include <openrct2/actions/WallPlaceAction.hpp>
#include <openrct2/actions/WallSetColourAction.hpp>
#include <openrct2/actions/WaterLowerAction.hpp>
#include <openrct2/actions/WaterRaiseAction.hpp>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/InteractiveConsole.h>
#include <openrct2/interface/Screenshot.h>
#include <openrct2/network/network.h>
#include <openrct2/paint/VirtualFloor.h>
#include <openrct2/peep/Staff.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/util/Util.h>
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

// clang-format off
enum {
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

enum FILE_MENU_DDIDX {
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
    DDIDX_QUIT_TO_MENU = 10,
    DDIDX_EXIT_OPENRCT2 = 11,
    // separator
    DDIDX_UPDATE_AVAILABLE = 13,
};

enum TOP_TOOLBAR_VIEW_MENU_DDIDX {
    DDIDX_UNDERGROUND_INSIDE = 0,
    DDIDX_HIDE_BASE = 1,
    DDIDX_HIDE_VERTICAL = 2,
    DDIDX_SEETHROUGH_RIDES = 4,
    DDIDX_SEETHROUGH_SCENARY = 5,
    DDIDX_SEETHROUGH_PATHS = 6,
    DDIDX_INVISIBLE_SUPPORTS = 7,
    DDIDX_INVISIBLE_PEEPS = 8,
    DDIDX_LAND_HEIGHTS = 10,
    DDIDX_TRACK_HEIGHTS = 11,
    DDIDX_PATH_HEIGHTS = 12,
    // 13 is a separator
    DDIDX_VIEW_CLIPPING = 14,
    DDIDX_HIGHLIGHT_PATH_ISSUES = 15,

    TOP_TOOLBAR_VIEW_MENU_COUNT
};

enum TOP_TOOLBAR_DEBUG_DDIDX {
    DDIDX_CONSOLE = 0,
    DDIDX_DEBUG_PAINT = 1,

    TOP_TOOLBAR_DEBUG_COUNT
};

enum TOP_TOOLBAR_NETWORK_DDIDX {
    DDIDX_MULTIPLAYER = 0,
    DDIDX_NETWORK = 1,
    DDIDX_MULTIPLAYER_RECONNECT = 2,

    TOP_TOOLBAR_NETWORK_COUNT
};

enum {
    DDIDX_CHEATS,
    DDIDX_TILE_INSPECTOR = 1,
    DDIDX_OBJECT_SELECTION = 2,
    DDIDX_INVENTIONS_LIST = 3,
    DDIDX_SCENARIO_OPTIONS = 4,
    // 5 is a separator
    DDIDX_ENABLE_SANDBOX_MODE = 6,
    DDIDX_DISABLE_CLEARANCE_CHECKS = 7,
    DDIDX_DISABLE_SUPPORT_LIMITS = 8,

    TOP_TOOLBAR_CHEATS_COUNT
};

enum {
    DDIDX_SHOW_MAP,
    DDIDX_OPEN_VIEWPORT,
};

enum {
    DDIDX_ROTATE_CLOCKWISE,
    DDIDX_ROTATE_ANTI_CLOCKWISE,
};

#pragma region Toolbar_widget_ordering

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
    WIDX_CLEAR_SCENERY
};

#pragma endregion

static rct_widget window_top_toolbar_widgets[] = {
    MakeRemapWidget({  0, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Primary   , SPR_TOOLBAR_PAUSE,          STR_PAUSE_GAME_TIP                ), // Pause
    MakeRemapWidget({ 60, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Primary   , SPR_TOOLBAR_FILE,           STR_DISC_AND_GAME_OPTIONS_TIP     ), // File menu
    MakeRemapWidget({250, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Primary   , SPR_G2_TOOLBAR_MUTE,        STR_TOOLBAR_MUTE_TIP              ), // Mute
    MakeRemapWidget({100, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Secondary , SPR_TOOLBAR_ZOOM_OUT,       STR_ZOOM_OUT_TIP                  ), // Zoom out
    MakeRemapWidget({130, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Secondary , SPR_TOOLBAR_ZOOM_IN,        STR_ZOOM_IN_TIP                   ), // Zoom in
    MakeRemapWidget({160, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Secondary , SPR_TOOLBAR_ROTATE,         STR_ROTATE_TIP                    ), // Rotate camera
    MakeRemapWidget({190, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Secondary , SPR_TOOLBAR_VIEW,           STR_VIEW_OPTIONS_TIP              ), // Transparency menu
    MakeRemapWidget({220, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Secondary , SPR_TOOLBAR_MAP,            STR_SHOW_MAP_TIP                  ), // Map
    MakeRemapWidget({267, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Tertiary  , SPR_TOOLBAR_LAND,           STR_ADJUST_LAND_TIP               ), // Land
    MakeRemapWidget({297, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Tertiary  , SPR_TOOLBAR_WATER,          STR_ADJUST_WATER_TIP              ), // Water
    MakeRemapWidget({327, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Tertiary  , SPR_TOOLBAR_SCENERY,        STR_PLACE_SCENERY_TIP             ), // Scenery
    MakeRemapWidget({357, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Tertiary  , SPR_TOOLBAR_FOOTPATH,       STR_BUILD_FOOTPATH_TIP            ), // Path
    MakeRemapWidget({387, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Tertiary  , SPR_TOOLBAR_CONSTRUCT_RIDE, STR_BUILD_RIDE_TIP                ), // Construct ride
    MakeRemapWidget({490, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Quaternary, SPR_TOOLBAR_RIDES,          STR_RIDES_IN_PARK_TIP             ), // Rides
    MakeRemapWidget({520, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Quaternary, SPR_TOOLBAR_PARK,           STR_PARK_INFORMATION_TIP          ), // Park
    MakeRemapWidget({550, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Quaternary, SPR_TAB_TOOLBAR,            STR_STAFF_TIP                     ), // Staff
    MakeRemapWidget({560, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Quaternary, SPR_TOOLBAR_GUESTS,         STR_GUESTS_TIP                    ), // Guests
    MakeRemapWidget({560, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Tertiary  , SPR_TOOLBAR_CLEAR_SCENERY,  STR_CLEAR_SCENERY_TIP             ), // Clear scenery
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Primary   , SPR_TAB_TOOLBAR,            STR_GAME_SPEED_TIP                ), // Fast forward
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Primary   , SPR_TAB_TOOLBAR,            STR_CHEATS_TIP                    ), // Cheats
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Primary   , SPR_TAB_TOOLBAR,            STR_DEBUG_TIP                     ), // Debug
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Quaternary, SPR_TAB_TOOLBAR,            STR_SCENARIO_OPTIONS_FINANCIAL_TIP), // Finances
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Quaternary, SPR_TAB_TOOLBAR,            STR_FINANCES_RESEARCH_TIP         ), // Research
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Quaternary, SPR_TAB_TOOLBAR,            STR_SHOW_RECENT_MESSAGES_TIP      ), // News
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Primary   , SPR_G2_TOOLBAR_MULTIPLAYER, STR_SHOW_MULTIPLAYER_STATUS_TIP   ), // Network
    MakeRemapWidget({ 30, 0}, {30, TOP_TOOLBAR_HEIGHT + 1}, WWT_TRNBTN, WindowColour::Primary   , SPR_TAB_TOOLBAR,            STR_TOOLBAR_CHAT_TIP              ), // Chat
    MakeWidget     ({  0, 0}, {10,                      1}, WWT_EMPTY,  WindowColour::Primary                                                                   ), // Artificial widget separator
    { WIDGETS_END },
};

static void window_top_toolbar_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_top_toolbar_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_top_toolbar_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_top_toolbar_tool_update(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_top_toolbar_tool_down(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_top_toolbar_tool_drag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_top_toolbar_tool_up(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoordsy);
static void window_top_toolbar_tool_abort(rct_window *w, rct_widgetindex widgetIndex);
static void window_top_toolbar_invalidate(rct_window *w);
static void window_top_toolbar_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_top_toolbar_events = {
    nullptr,
    window_top_toolbar_mouseup,
    nullptr,
    window_top_toolbar_mousedown,
    window_top_toolbar_dropdown,
    nullptr,
    nullptr,
    nullptr,
    nullptr,                                           // check if editor versions are significantly different...
    window_top_toolbar_tool_update,                 // editor: 0x0066fB0E
    window_top_toolbar_tool_down,                   // editor: 0x0066fB5C
    window_top_toolbar_tool_drag,                   // editor: 0x0066fB37
    window_top_toolbar_tool_up,                     // editor: 0x0066fC44 (Exactly the same)
    window_top_toolbar_tool_abort,                  // editor: 0x0066fA74 (Exactly the same)
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_top_toolbar_invalidate,
    window_top_toolbar_paint,
    nullptr
};
// clang-format on

static void top_toolbar_init_view_menu(rct_window* window, rct_widget* widget);
static void top_toolbar_view_menu_dropdown(int16_t dropdownIndex);
static void top_toolbar_init_map_menu(rct_window* window, rct_widget* widget);
static void top_toolbar_map_menu_dropdown(int16_t dropdownIndex);
static void top_toolbar_init_fastforward_menu(rct_window* window, rct_widget* widget);
static void top_toolbar_fastforward_menu_dropdown(int16_t dropdownIndex);
static void top_toolbar_init_rotate_menu(rct_window* window, rct_widget* widget);
static void top_toolbar_rotate_menu_dropdown(int16_t dropdownIndex);
static void top_toolbar_init_cheats_menu(rct_window* window, rct_widget* widget);
static void top_toolbar_cheats_menu_dropdown(int16_t dropdownIndex);
static void top_toolbar_init_debug_menu(rct_window* window, rct_widget* widget);
static void top_toolbar_debug_menu_dropdown(int16_t dropdownIndex);
static void top_toolbar_init_network_menu(rct_window* window, rct_widget* widget);
static void top_toolbar_network_menu_dropdown(int16_t dropdownIndex);

static void toggle_footpath_window();
static void toggle_land_window(rct_window* topToolbar, rct_widgetindex widgetIndex);
static void toggle_clear_scenery_window(rct_window* topToolbar, rct_widgetindex widgetIndex);
static void toggle_water_window(rct_window* topToolbar, rct_widgetindex widgetIndex);

static money32 selection_lower_land(uint8_t flags);
static money32 selection_raise_land(uint8_t flags);

static ClearAction GetClearAction();

static bool _landToolBlocked;
static uint8_t _unkF64F0E;
static int16_t _unkF64F0A;
// rct2: 0x00F64F15
static colour_t _secondaryColour;
// rct2: 0x00F64F16
static colour_t _tertiaryColour;

/**
 * Creates the main game top toolbar window.
 *  rct2: 0x0066B485 (part of 0x0066B3E8)
 */
rct_window* window_top_toolbar_open()
{
    rct_window* window = window_create(
        ScreenCoordsXY(0, 0), context_get_width(), TOP_TOOLBAR_HEIGHT + 1, &window_top_toolbar_events, WC_TOP_TOOLBAR,
        WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND);
    window->widgets = window_top_toolbar_widgets;

    window_init_scroll_widgets(window);

    return window;
}

/**
 *
 *  rct2: 0x0066C957
 */
static void window_top_toolbar_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    rct_window* mainWindow;

    switch (widgetIndex)
    {
        case WIDX_PAUSE:
            if (network_get_mode() != NETWORK_MODE_CLIENT)
            {
                auto pauseToggleAction = PauseToggleAction();
                GameActions::Execute(&pauseToggleAction);
            }
            break;
        case WIDX_ZOOM_OUT:
            if ((mainWindow = window_get_main()) != nullptr)
                window_zoom_out(mainWindow, false);
            break;
        case WIDX_ZOOM_IN:
            if ((mainWindow = window_get_main()) != nullptr)
                window_zoom_in(mainWindow, false);
            break;
        case WIDX_CLEAR_SCENERY:
            toggle_clear_scenery_window(w, WIDX_CLEAR_SCENERY);
            break;
        case WIDX_LAND:
            toggle_land_window(w, WIDX_LAND);
            break;
        case WIDX_WATER:
            toggle_water_window(w, WIDX_WATER);
            break;
        case WIDX_SCENERY:
            if (!tool_set(w, WIDX_SCENERY, TOOL_ARROW))
            {
                input_set_flag(INPUT_FLAG_6, true);
                context_open_window(WC_SCENERY);
            }
            break;
        case WIDX_PATH:
            toggle_footpath_window();
            break;
        case WIDX_CONSTRUCT_RIDE:
            context_open_window(WC_CONSTRUCT_RIDE);
            break;
        case WIDX_RIDES:
            context_open_window(WC_RIDE_LIST);
            break;
        case WIDX_PARK:
            context_open_window(WC_PARK_INFORMATION);
            break;
        case WIDX_STAFF:
            context_open_window(WC_STAFF_LIST);
            break;
        case WIDX_GUESTS:
            context_open_window(WC_GUEST_LIST);
            break;
        case WIDX_FINANCES:
            context_open_window(WC_FINANCES);
            break;
        case WIDX_RESEARCH:
            context_open_window(WC_RESEARCH);
            break;
        case WIDX_NEWS:
            context_open_window(WC_RECENT_NEWS);
            break;
        case WIDX_MUTE:
            audio_toggle_all_sounds();
            break;
        case WIDX_CHAT:
            if (chat_available())
            {
                chat_toggle();
            }
            else
            {
                context_show_error(STR_CHAT_UNAVAILABLE, STR_NONE);
            }
            break;
    }
}

/**
 *
 *  rct2: 0x0066CA3B
 */
static void window_top_toolbar_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    int32_t numItems;

    switch (widgetIndex)
    {
        case WIDX_FILE_MENU:
            if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
            {
                gDropdownItemsFormat[0] = STR_ABOUT;
                gDropdownItemsFormat[1] = STR_OPTIONS;
                gDropdownItemsFormat[2] = STR_SCREENSHOT;
                gDropdownItemsFormat[3] = STR_GIANT_SCREENSHOT;
                gDropdownItemsFormat[4] = STR_EMPTY;
                gDropdownItemsFormat[5] = STR_QUIT_TRACK_DESIGNS_MANAGER;
                gDropdownItemsFormat[6] = STR_EXIT_OPENRCT2;

                if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
                    gDropdownItemsFormat[5] = STR_QUIT_ROLLERCOASTER_DESIGNER;

                numItems = 7;
            }
            else if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
            {
                gDropdownItemsFormat[0] = STR_LOAD_LANDSCAPE;
                gDropdownItemsFormat[1] = STR_SAVE_LANDSCAPE;
                gDropdownItemsFormat[2] = STR_EMPTY;
                gDropdownItemsFormat[3] = STR_ABOUT;
                gDropdownItemsFormat[4] = STR_OPTIONS;
                gDropdownItemsFormat[5] = STR_SCREENSHOT;
                gDropdownItemsFormat[6] = STR_GIANT_SCREENSHOT;
                gDropdownItemsFormat[7] = STR_EMPTY;
                gDropdownItemsFormat[8] = STR_QUIT_SCENARIO_EDITOR;
                gDropdownItemsFormat[9] = STR_EXIT_OPENRCT2;
                numItems = 10;
            }
            else
            {
                gDropdownItemsFormat[0] = STR_NEW_GAME;
                gDropdownItemsFormat[1] = STR_LOAD_GAME;
                gDropdownItemsFormat[2] = STR_SAVE_GAME;
                gDropdownItemsFormat[3] = STR_SAVE_GAME_AS;
                gDropdownItemsFormat[4] = STR_EMPTY;
                gDropdownItemsFormat[5] = STR_ABOUT;
                gDropdownItemsFormat[6] = STR_OPTIONS;
                gDropdownItemsFormat[7] = STR_SCREENSHOT;
                gDropdownItemsFormat[8] = STR_GIANT_SCREENSHOT;
                gDropdownItemsFormat[9] = STR_EMPTY;
                gDropdownItemsFormat[10] = STR_QUIT_TO_MENU;
                gDropdownItemsFormat[11] = STR_EXIT_OPENRCT2;
                numItems = 12;
                if (OpenRCT2::GetContext()->HasNewVersionInfo())
                {
                    gDropdownItemsFormat[12] = STR_EMPTY;
                    gDropdownItemsFormat[13] = STR_UPDATE_AVAILABLE;
                    numItems += 2;
                }
            }
            window_dropdown_show_text(
                { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[0] | 0x80,
                DROPDOWN_FLAG_STAY_OPEN, numItems);
            break;
        case WIDX_CHEATS:
            top_toolbar_init_cheats_menu(w, widget);
            break;
        case WIDX_VIEW_MENU:
            top_toolbar_init_view_menu(w, widget);
            break;
        case WIDX_MAP:
            top_toolbar_init_map_menu(w, widget);
            break;
        case WIDX_FASTFORWARD:
            top_toolbar_init_fastforward_menu(w, widget);
            break;
        case WIDX_ROTATE:
            top_toolbar_init_rotate_menu(w, widget);
            break;
        case WIDX_DEBUG:
            top_toolbar_init_debug_menu(w, widget);
            break;
        case WIDX_NETWORK:
            top_toolbar_init_network_menu(w, widget);
            break;
    }
}

static void window_top_toolbar_scenarioselect_callback(const utf8* path)
{
    window_close_by_class(WC_EDITOR_OBJECT_SELECTION);
    context_load_park_from_file(path);
}

/**
 *
 *  rct2: 0x0066C9EA
 */
static void window_top_toolbar_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
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

            switch (dropdownIndex)
            {
                case DDIDX_NEW_GAME:
                {
                    auto intent = Intent(WC_SCENARIO_SELECT);
                    intent.putExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(window_top_toolbar_scenarioselect_callback));
                    context_open_intent(&intent);
                    break;
                }
                case DDIDX_LOAD_GAME:
                {
                    auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::OpenSavePrompt);
                    GameActions::Execute(&loadOrQuitAction);
                    break;
                }
                case DDIDX_SAVE_GAME:
                    tool_cancel();
                    save_game();
                    break;
                case DDIDX_SAVE_GAME_AS:
                    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                    {
                        auto intent = Intent(WC_LOADSAVE);
                        intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE);
                        intent.putExtra(INTENT_EXTRA_PATH, std::string{ gS6Info.name });
                        context_open_intent(&intent);
                    }
                    else
                    {
                        tool_cancel();
                        save_game_as();
                    }
                    break;
                case DDIDX_ABOUT:
                    context_open_window(WC_ABOUT);
                    break;
                case DDIDX_OPTIONS:
                    context_open_window(WC_OPTIONS);
                    break;
                case DDIDX_SCREENSHOT:
                    gScreenshotCountdown = 10;
                    break;
                case DDIDX_GIANT_SCREENSHOT:
                    screenshot_giant();
                    break;
                case DDIDX_QUIT_TO_MENU:
                {
                    window_close_by_class(WC_MANAGE_TRACK_DESIGN);
                    window_close_by_class(WC_TRACK_DELETE_PROMPT);
                    auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::OpenSavePrompt, PM_SAVE_BEFORE_QUIT);
                    GameActions::Execute(&loadOrQuitAction);
                    break;
                }
                case DDIDX_EXIT_OPENRCT2:
                    context_quit();
                    break;
                case DDIDX_UPDATE_AVAILABLE:
                    context_open_window_view(WV_NEW_VERSION_INFO);
                    break;
            }
            break;
        case WIDX_CHEATS:
            top_toolbar_cheats_menu_dropdown(dropdownIndex);
            break;
        case WIDX_VIEW_MENU:
            top_toolbar_view_menu_dropdown(dropdownIndex);
            break;
        case WIDX_MAP:
            top_toolbar_map_menu_dropdown(dropdownIndex);
            break;
        case WIDX_FASTFORWARD:
            top_toolbar_fastforward_menu_dropdown(dropdownIndex);
            break;
        case WIDX_ROTATE:
            top_toolbar_rotate_menu_dropdown(dropdownIndex);
            break;
        case WIDX_DEBUG:
            top_toolbar_debug_menu_dropdown(dropdownIndex);
            break;
        case WIDX_NETWORK:
            top_toolbar_network_menu_dropdown(dropdownIndex);
            break;
    }
}

/**
 *
 *  rct2: 0x0066C810
 */
static void window_top_toolbar_invalidate(rct_window* w)
{
    int32_t x, enabledWidgets, widgetIndex, widgetWidth, firstAlignment;
    rct_widget* widget;

    // Enable / disable buttons
    window_top_toolbar_widgets[WIDX_PAUSE].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_FILE_MENU].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_ZOOM_OUT].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_ZOOM_IN].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_ROTATE].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_VIEW_MENU].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_MAP].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_MUTE].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_CHAT].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_LAND].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_WATER].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_SCENERY].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_PATH].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_CONSTRUCT_RIDE].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_RIDES].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_PARK].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_STAFF].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_GUESTS].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_CLEAR_SCENERY].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_FINANCES].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_RESEARCH].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_FASTFORWARD].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_CHEATS].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_DEBUG].type = gConfigGeneral.debugging_tools ? WWT_TRNBTN : WWT_EMPTY;
    window_top_toolbar_widgets[WIDX_NEWS].type = WWT_TRNBTN;
    window_top_toolbar_widgets[WIDX_NETWORK].type = WWT_TRNBTN;

    if (!gConfigInterface.toolbar_show_mute)
    {
        window_top_toolbar_widgets[WIDX_MUTE].type = WWT_EMPTY;
    }

    if (!gConfigInterface.toolbar_show_chat)
    {
        window_top_toolbar_widgets[WIDX_CHAT].type = WWT_EMPTY;
    }

    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
    {
        window_top_toolbar_widgets[WIDX_PAUSE].type = WWT_EMPTY;
        window_top_toolbar_widgets[WIDX_RIDES].type = WWT_EMPTY;
        window_top_toolbar_widgets[WIDX_PARK].type = WWT_EMPTY;
        window_top_toolbar_widgets[WIDX_STAFF].type = WWT_EMPTY;
        window_top_toolbar_widgets[WIDX_GUESTS].type = WWT_EMPTY;
        window_top_toolbar_widgets[WIDX_FINANCES].type = WWT_EMPTY;
        window_top_toolbar_widgets[WIDX_RESEARCH].type = WWT_EMPTY;
        window_top_toolbar_widgets[WIDX_NEWS].type = WWT_EMPTY;
        window_top_toolbar_widgets[WIDX_NETWORK].type = WWT_EMPTY;

        if (gS6Info.editor_step != EDITOR_STEP_LANDSCAPE_EDITOR)
        {
            window_top_toolbar_widgets[WIDX_MAP].type = WWT_EMPTY;
            window_top_toolbar_widgets[WIDX_LAND].type = WWT_EMPTY;
            window_top_toolbar_widgets[WIDX_WATER].type = WWT_EMPTY;
            window_top_toolbar_widgets[WIDX_SCENERY].type = WWT_EMPTY;
            window_top_toolbar_widgets[WIDX_PATH].type = WWT_EMPTY;
            window_top_toolbar_widgets[WIDX_CLEAR_SCENERY].type = WWT_EMPTY;
        }

        if (gS6Info.editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER)
        {
            window_top_toolbar_widgets[WIDX_CONSTRUCT_RIDE].type = WWT_EMPTY;
            window_top_toolbar_widgets[WIDX_FASTFORWARD].type = WWT_EMPTY;
        }

        if (gS6Info.editor_step != EDITOR_STEP_LANDSCAPE_EDITOR && gS6Info.editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER)
        {
            window_top_toolbar_widgets[WIDX_ZOOM_OUT].type = WWT_EMPTY;
            window_top_toolbar_widgets[WIDX_ZOOM_IN].type = WWT_EMPTY;
            window_top_toolbar_widgets[WIDX_ROTATE].type = WWT_EMPTY;
            window_top_toolbar_widgets[WIDX_VIEW_MENU].type = WWT_EMPTY;
        }
    }
    else
    {
        if ((gParkFlags & PARK_FLAGS_NO_MONEY) || !gConfigInterface.toolbar_show_finances)
            window_top_toolbar_widgets[WIDX_FINANCES].type = WWT_EMPTY;

        if (!gConfigInterface.toolbar_show_research)
            window_top_toolbar_widgets[WIDX_RESEARCH].type = WWT_EMPTY;

        if (!gConfigInterface.toolbar_show_cheats)
            window_top_toolbar_widgets[WIDX_CHEATS].type = WWT_EMPTY;

        if (!gConfigInterface.toolbar_show_news)
            window_top_toolbar_widgets[WIDX_NEWS].type = WWT_EMPTY;

        switch (network_get_mode())
        {
            case NETWORK_MODE_NONE:
                window_top_toolbar_widgets[WIDX_NETWORK].type = WWT_EMPTY;
                window_top_toolbar_widgets[WIDX_CHAT].type = WWT_EMPTY;
                break;
            case NETWORK_MODE_CLIENT:
                window_top_toolbar_widgets[WIDX_PAUSE].type = WWT_EMPTY;
                [[fallthrough]];
            case NETWORK_MODE_SERVER:
                window_top_toolbar_widgets[WIDX_FASTFORWARD].type = WWT_EMPTY;
                break;
        }
    }

    enabledWidgets = 0;
    for (int i = WIDX_PAUSE; i <= WIDX_CHAT; i++)
        if (window_top_toolbar_widgets[i].type != WWT_EMPTY)
            enabledWidgets |= (1 << i);
    w->enabled_widgets = enabledWidgets;

    // Align left hand side toolbar buttons
    firstAlignment = 1;
    x = 0;
    for (size_t i = 0; i < std::size(left_aligned_widgets_order); ++i)
    {
        widgetIndex = left_aligned_widgets_order[i];
        widget = &window_top_toolbar_widgets[widgetIndex];
        if (widget->type == WWT_EMPTY && widgetIndex != WIDX_SEPARATOR)
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
    int32_t screenWidth = context_get_width();
    firstAlignment = 1;
    x = std::max(640, screenWidth);
    for (size_t i = 0; i < std::size(right_aligned_widgets_order); ++i)
    {
        widgetIndex = right_aligned_widgets_order[i];
        widget = &window_top_toolbar_widgets[widgetIndex];
        if (widget->type == WWT_EMPTY && widgetIndex != WIDX_SEPARATOR)
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
    if (window_find_by_class(WC_FOOTPATH) == nullptr)
        w->pressed_widgets &= ~(1 << WIDX_PATH);
    else
        w->pressed_widgets |= (1 << WIDX_PATH);

    if (gGamePaused & GAME_PAUSED_NORMAL)
        w->pressed_widgets |= (1 << WIDX_PAUSE);
    else
        w->pressed_widgets &= ~(1 << WIDX_PAUSE);

    if (!gGameSoundsOff)
        window_top_toolbar_widgets[WIDX_MUTE].image = IMAGE_TYPE_REMAP | SPR_G2_TOOLBAR_MUTE;
    else
        window_top_toolbar_widgets[WIDX_MUTE].image = IMAGE_TYPE_REMAP | SPR_G2_TOOLBAR_UNMUTE;

    // Set map button to the right image.
    if (window_top_toolbar_widgets[WIDX_MAP].type != WWT_EMPTY)
    {
        static constexpr uint32_t imageIdByRotation[] = {
            SPR_G2_MAP_NORTH,
            SPR_G2_MAP_WEST,
            SPR_G2_MAP_SOUTH,
            SPR_G2_MAP_EAST,
        };

        uint32_t mapImageId = imageIdByRotation[get_current_rotation()];
        window_top_toolbar_widgets[WIDX_MAP].image = IMAGE_TYPE_REMAP | mapImageId;
    }

    // Zoomed out/in disable. Not sure where this code is in the original.
    if (window_get_main()->viewport->zoom == ZoomLevel::min())
    {
        w->disabled_widgets |= (1 << WIDX_ZOOM_IN);
    }
    else if (window_get_main()->viewport->zoom >= ZoomLevel::max())
    {
        w->disabled_widgets |= (1 << WIDX_ZOOM_OUT);
    }
    else
    {
        w->disabled_widgets &= ~((1 << WIDX_ZOOM_IN) | (1 << WIDX_ZOOM_OUT));
    }
}

/**
 *
 *  rct2: 0x0066C8EC
 */
static void window_top_toolbar_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t imgId;

    window_draw_widgets(w, dpi);

    ScreenCoordsXY screenPos{};
    // Draw staff button image (setting masks to the staff colours)
    if (window_top_toolbar_widgets[WIDX_STAFF].type != WWT_EMPTY)
    {
        screenPos = { w->windowPos.x + window_top_toolbar_widgets[WIDX_STAFF].left,
                      w->windowPos.y + window_top_toolbar_widgets[WIDX_STAFF].top };
        imgId = SPR_TOOLBAR_STAFF;
        if (widget_is_pressed(w, WIDX_STAFF))
            imgId++;
        imgId |= SPRITE_ID_PALETTE_COLOUR_2(gStaffHandymanColour, gStaffMechanicColour);
        gfx_draw_sprite(dpi, imgId, screenPos, 0);
    }

    // Draw fast forward button
    if (window_top_toolbar_widgets[WIDX_FASTFORWARD].type != WWT_EMPTY)
    {
        screenPos = { w->windowPos.x + window_top_toolbar_widgets[WIDX_FASTFORWARD].left + 0,
                      w->windowPos.y + window_top_toolbar_widgets[WIDX_FASTFORWARD].top + 0 };
        if (widget_is_pressed(w, WIDX_FASTFORWARD))
            screenPos.y++;
        imgId = SPR_G2_FASTFORWARD;
        gfx_draw_sprite(dpi, imgId, screenPos + ScreenCoordsXY{ 6, 3 }, 0);

        for (int32_t i = 0; i < gGameSpeed && gGameSpeed <= 4; i++)
        {
            gfx_draw_sprite(dpi, SPR_G2_SPEED_ARROW, screenPos + ScreenCoordsXY{ 5 + i * 5, 15 }, 0);
        }
        for (int32_t i = 0; i < 3 && i < gGameSpeed - 4 && gGameSpeed >= 5; i++)
        {
            gfx_draw_sprite(dpi, SPR_G2_HYPER_ARROW, screenPos + ScreenCoordsXY{ 5 + i * 6, 15 }, 0);
        }
    }

    // Draw cheats button
    if (window_top_toolbar_widgets[WIDX_CHEATS].type != WWT_EMPTY)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_CHEATS].left - 1,
                              window_top_toolbar_widgets[WIDX_CHEATS].top - 1 };
        if (widget_is_pressed(w, WIDX_CHEATS))
            screenPos.y++;
        imgId = SPR_G2_SANDBOX;
        gfx_draw_sprite(dpi, imgId, screenPos, 3);

        // Draw an overlay if clearance checks are disabled
        if (gCheatsDisableClearanceChecks)
        {
            gfx_draw_string_right(
                dpi, STR_OVERLAY_CLEARANCE_CHECKS_DISABLED, nullptr, COLOUR_DARK_ORANGE | COLOUR_FLAG_OUTLINE,
                screenPos + ScreenCoordsXY{ 26, 2 });
        }
    }

    // Draw chat button
    if (window_top_toolbar_widgets[WIDX_CHAT].type != WWT_EMPTY)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_CHAT].left, window_top_toolbar_widgets[WIDX_CHAT].top - 2 };
        if (widget_is_pressed(w, WIDX_CHAT))
            screenPos.y++;
        imgId = SPR_G2_CHAT;
        gfx_draw_sprite(dpi, imgId, screenPos, 3);
    }

    // Draw debug button
    if (window_top_toolbar_widgets[WIDX_DEBUG].type != WWT_EMPTY)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_DEBUG].left, window_top_toolbar_widgets[WIDX_DEBUG].top - 1 };
        if (widget_is_pressed(w, WIDX_DEBUG))
            screenPos.y++;
        imgId = SPR_TAB_GEARS_0;
        gfx_draw_sprite(dpi, imgId, screenPos, 3);
    }

    // Draw research button
    if (window_top_toolbar_widgets[WIDX_RESEARCH].type != WWT_EMPTY)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_RESEARCH].left - 1,
                              window_top_toolbar_widgets[WIDX_RESEARCH].top };
        if (widget_is_pressed(w, WIDX_RESEARCH))
            screenPos.y++;
        imgId = SPR_TAB_FINANCES_RESEARCH_0;
        gfx_draw_sprite(dpi, imgId, screenPos, 0);
    }

    // Draw finances button
    if (window_top_toolbar_widgets[WIDX_FINANCES].type != WWT_EMPTY)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_FINANCES].left + 3,
                              window_top_toolbar_widgets[WIDX_FINANCES].top + 1 };
        if (widget_is_pressed(w, WIDX_FINANCES))
            screenPos.y++;
        imgId = SPR_FINANCE;
        gfx_draw_sprite(dpi, imgId, screenPos, 0);
    }

    // Draw news button
    if (window_top_toolbar_widgets[WIDX_NEWS].type != WWT_EMPTY)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_NEWS].left + 3, window_top_toolbar_widgets[WIDX_NEWS].top + 0 };
        if (widget_is_pressed(w, WIDX_NEWS))
            screenPos.y++;
        imgId = SPR_G2_TAB_NEWS;
        gfx_draw_sprite(dpi, imgId, screenPos, 0);
    }

    // Draw network button
    if (window_top_toolbar_widgets[WIDX_NETWORK].type != WWT_EMPTY)
    {
        screenPos = w->windowPos
            + ScreenCoordsXY{ window_top_toolbar_widgets[WIDX_NETWORK].left + 3,
                              window_top_toolbar_widgets[WIDX_NETWORK].top + 0 };
        if (widget_is_pressed(w, WIDX_NETWORK))
            screenPos.y++;

        // Draw (de)sync icon.
        imgId = (network_is_desynchronised() ? SPR_G2_MULTIPLAYER_DESYNC : SPR_G2_MULTIPLAYER_SYNC);
        gfx_draw_sprite(dpi, imgId, screenPos + ScreenCoordsXY{ 3, 11 }, 0);

        // Draw number of players.
        int32_t player_count = network_get_num_players();
        gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
        gfx_draw_string_right(
            dpi, STR_COMMA16, &player_count, COLOUR_WHITE | COLOUR_FLAG_OUTLINE, screenPos + ScreenCoordsXY{ 23, 1 });
    }
}

/**
 *
 *  rct2: 0x006E3158
 */
static void repaint_scenery_tool_down(const ScreenCoordsXY& windowPos, rct_widgetindex widgetIndex)
{
    auto flags = VIEWPORT_INTERACTION_MASK_SCENERY & VIEWPORT_INTERACTION_MASK_WALL & VIEWPORT_INTERACTION_MASK_LARGE_SCENERY
        & VIEWPORT_INTERACTION_MASK_BANNER;
    // This is -2 as banner is 12 but flags are offset different

    auto info = get_map_coordinates_from_pos(windowPos, flags);

    switch (info.SpriteType)
    {
        case VIEWPORT_INTERACTION_ITEM_SCENERY:
        {
            rct_scenery_entry* scenery_entry = info.Element->AsSmallScenery()->GetEntry();

            // If can't repaint
            if (!scenery_small_entry_has_flag(
                    scenery_entry, SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR | SMALL_SCENERY_FLAG_HAS_GLASS))
                return;

            uint8_t quadrant = info.Element->AsSmallScenery()->GetSceneryQuadrant();
            auto repaintScenery = SmallScenerySetColourAction(
                { info.Loc, info.Element->GetBaseZ() }, quadrant, info.Element->AsSmallScenery()->GetEntryIndex(),
                gWindowSceneryPrimaryColour, gWindowScenerySecondaryColour);

            GameActions::Execute(&repaintScenery);
            break;
        }
        case VIEWPORT_INTERACTION_ITEM_WALL:
        {
            rct_scenery_entry* scenery_entry = info.Element->AsWall()->GetEntry();

            // If can't repaint
            if (!(scenery_entry->wall.flags & (WALL_SCENERY_HAS_PRIMARY_COLOUR | WALL_SCENERY_HAS_GLASS)))
                return;

            auto repaintScenery = WallSetColourAction(
                { info.Loc, info.Element->GetBaseZ(), info.Element->GetDirection() }, gWindowSceneryPrimaryColour,
                gWindowScenerySecondaryColour, gWindowSceneryTertiaryColour);

            GameActions::Execute(&repaintScenery);
            break;
        }
        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        {
            rct_scenery_entry* scenery_entry = info.Element->AsLargeScenery()->GetEntry();

            // If can't repaint
            if (!(scenery_entry->large_scenery.flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
                return;

            auto repaintScenery = LargeScenerySetColourAction(
                { info.Loc, info.Element->GetBaseZ(), info.Element->GetDirection() },
                info.Element->AsLargeScenery()->GetSequenceIndex(), gWindowSceneryPrimaryColour, gWindowScenerySecondaryColour);

            GameActions::Execute(&repaintScenery);
            break;
        }
        case VIEWPORT_INTERACTION_ITEM_BANNER:
        {
            auto banner = info.Element->AsBanner()->GetBanner();
            if (banner != nullptr)
            {
                auto scenery_entry = get_banner_entry(banner->type);
                if (scenery_entry->banner.flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
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

static void scenery_eyedropper_tool_down(const ScreenCoordsXY& windowPos, rct_widgetindex widgetIndex)
{
    auto flags = VIEWPORT_INTERACTION_MASK_SCENERY & VIEWPORT_INTERACTION_MASK_WALL & VIEWPORT_INTERACTION_MASK_LARGE_SCENERY
        & VIEWPORT_INTERACTION_MASK_BANNER & VIEWPORT_INTERACTION_MASK_FOOTPATH_ITEM;

    auto info = get_map_coordinates_from_pos(windowPos, flags);

    switch (info.SpriteType)
    {
        case VIEWPORT_INTERACTION_ITEM_SCENERY:
        {
            SmallSceneryElement* sceneryElement = info.Element->AsSmallScenery();
            auto entryIndex = sceneryElement->GetEntryIndex();
            rct_scenery_entry* sceneryEntry = get_small_scenery_entry(entryIndex);
            if (sceneryEntry != nullptr)
            {
                if (window_scenery_set_selected_item({ SCENERY_TYPE_SMALL, entryIndex }))
                {
                    gWindowSceneryRotation = sceneryElement->GetDirectionWithOffset(get_current_rotation());
                    gWindowSceneryPrimaryColour = sceneryElement->GetPrimaryColour();
                    gWindowScenerySecondaryColour = sceneryElement->GetSecondaryColour();
                    gWindowSceneryEyedropperEnabled = false;
                }
            }
            break;
        }
        case VIEWPORT_INTERACTION_ITEM_WALL:
        {
            auto entryIndex = info.Element->AsWall()->GetEntryIndex();
            rct_scenery_entry* sceneryEntry = get_wall_entry(entryIndex);
            if (sceneryEntry != nullptr)
            {
                if (window_scenery_set_selected_item({ SCENERY_TYPE_WALL, entryIndex }))
                {
                    gWindowSceneryPrimaryColour = info.Element->AsWall()->GetPrimaryColour();
                    gWindowScenerySecondaryColour = info.Element->AsWall()->GetSecondaryColour();
                    gWindowSceneryTertiaryColour = info.Element->AsWall()->GetTertiaryColour();
                    gWindowSceneryEyedropperEnabled = false;
                }
            }
            break;
        }
        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        {
            auto entryIndex = info.Element->AsLargeScenery()->GetEntryIndex();
            rct_scenery_entry* sceneryEntry = get_large_scenery_entry(entryIndex);
            if (sceneryEntry != nullptr)
            {
                if (window_scenery_set_selected_item({ SCENERY_TYPE_LARGE, entryIndex }))
                {
                    gWindowSceneryRotation = (get_current_rotation() + info.Element->GetDirection()) & 3;
                    gWindowSceneryPrimaryColour = info.Element->AsLargeScenery()->GetPrimaryColour();
                    gWindowScenerySecondaryColour = info.Element->AsLargeScenery()->GetSecondaryColour();
                    gWindowSceneryEyedropperEnabled = false;
                }
            }
            break;
        }
        case VIEWPORT_INTERACTION_ITEM_BANNER:
        {
            auto banner = info.Element->AsBanner()->GetBanner();
            if (banner != nullptr)
            {
                auto sceneryEntry = get_banner_entry(banner->type);
                if (sceneryEntry != nullptr)
                {
                    if (window_scenery_set_selected_item({ SCENERY_TYPE_BANNER, banner->type }))
                    {
                        gWindowSceneryEyedropperEnabled = false;
                    }
                }
            }
            break;
        }
        case VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM:
        {
            auto entryIndex = info.Element->AsPath()->GetAdditionEntryIndex();
            rct_scenery_entry* sceneryEntry = get_footpath_item_entry(entryIndex);
            if (sceneryEntry != nullptr)
            {
                if (window_scenery_set_selected_item({ SCENERY_TYPE_PATH_ITEM, entryIndex }))
                {
                    gWindowSceneryEyedropperEnabled = false;
                }
            }
            break;
        }
        default:
            break;
    }
}

/**
 *
 *  rct2: 0x006E1F34
 * Outputs
 * eax : gridX
 * ebx : parameter_1
 * ecx : gridY
 * edx : parameter_2
 * edi : parameter_3
 */
static void sub_6E1F34(
    const ScreenCoordsXY& sourceScreenPos, ScenerySelection selection, CoordsXY& gridPos, uint32_t* parameter_1,
    uint32_t* parameter_2, uint32_t* parameter_3)
{
    rct_window* w = window_find_by_class(WC_SCENERY);

    if (w == nullptr)
    {
        gridPos.setNull();
        return;
    }

    auto screenPos = sourceScreenPos;
    uint16_t maxPossibleHeight = (std::numeric_limits<decltype(TileElement::base_height)>::max() - 32) * ZoomLevel::max();
    bool can_raise_item = false;

    if (selection.SceneryType == SCENERY_TYPE_SMALL)
    {
        rct_scenery_entry* scenery_entry = get_small_scenery_entry(selection.EntryIndex);
        maxPossibleHeight -= scenery_entry->small_scenery.height;
        if (scenery_small_entry_has_flag(scenery_entry, SMALL_SCENERY_FLAG_STACKABLE))
        {
            can_raise_item = true;
        }
    }
    else if (selection.SceneryType == SCENERY_TYPE_WALL)
    {
        rct_scenery_entry* scenery_entry = get_wall_entry(selection.EntryIndex);
        if (scenery_entry)
        {
            maxPossibleHeight -= scenery_entry->wall.height;
        }
        can_raise_item = true;
    }
    else if (selection.SceneryType == SCENERY_TYPE_LARGE)
    {
        rct_scenery_entry* scenery_entry = get_large_scenery_entry(selection.EntryIndex);
        if (scenery_entry)
        {
            int16_t maxClearZ = 0;
            for (int32_t i = 0; scenery_entry->large_scenery.tiles[i].x_offset != -1; ++i)
            {
                maxClearZ = std::max<int16_t>(maxClearZ, scenery_entry->large_scenery.tiles[i].z_clearance);
            }
            maxPossibleHeight = std::max(0, maxPossibleHeight - maxClearZ);
        }
        can_raise_item = true;
    }

    if (!can_raise_item && !gCheatsDisableSupportLimits)
    {
        gSceneryCtrlPressed = false;
        gSceneryShiftPressed = false;
    }
    else
    {
        if (!gSceneryCtrlPressed)
        {
            if (input_test_place_object_modifier(PLACE_OBJECT_MODIFIER_COPY_Z))
            {
                // CTRL pressed
                auto flags = VIEWPORT_INTERACTION_MASK_TERRAIN & VIEWPORT_INTERACTION_MASK_RIDE
                    & VIEWPORT_INTERACTION_MASK_SCENERY & VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_WALL
                    & VIEWPORT_INTERACTION_MASK_LARGE_SCENERY;
                auto info = get_map_coordinates_from_pos(screenPos, flags);

                if (info.SpriteType != VIEWPORT_INTERACTION_ITEM_NONE)
                {
                    gSceneryCtrlPressed = true;
                    gSceneryCtrlPressZ = info.Element->GetBaseZ();
                }
            }
        }
        else
        {
            if (!(input_test_place_object_modifier(PLACE_OBJECT_MODIFIER_COPY_Z)))
            {
                // CTRL not pressed
                gSceneryCtrlPressed = false;
            }
        }

        if (!gSceneryShiftPressed)
        {
            if (input_test_place_object_modifier(PLACE_OBJECT_MODIFIER_SHIFT_Z))
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
            if (input_test_place_object_modifier(PLACE_OBJECT_MODIFIER_SHIFT_Z))
            {
                // SHIFT pressed
                gSceneryShiftPressZOffset = (gSceneryShiftPressY - screenPos.y + 4);
                // Scale delta by zoom to match mouse position.
                auto* mainWnd = window_get_main();
                if (mainWnd && mainWnd->viewport)
                {
                    gSceneryShiftPressZOffset = gSceneryShiftPressZOffset * mainWnd->viewport->zoom;
                }
                gSceneryShiftPressZOffset = floor2(gSceneryShiftPressZOffset, 8);

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

    switch (selection.SceneryType)
    {
        case SCENERY_TYPE_SMALL:
        {
            // Small scenery
            rct_scenery_entry* scenery = get_small_scenery_entry(selection.EntryIndex);
            if (!scenery_small_entry_has_flag(scenery, SMALL_SCENERY_FLAG_FULL_TILE))
            {
                uint8_t quadrant = 0;

                // If CTRL not pressed
                if (!gSceneryCtrlPressed)
                {
                    auto gridCoords = screen_get_map_xy_quadrant(screenPos, &quadrant);
                    if (!gridCoords)
                    {
                        gridPos.setNull();
                        return;
                    }
                    gridPos = *gridCoords;

                    gSceneryPlaceZ = 0;

                    // If SHIFT pressed
                    if (gSceneryShiftPressed)
                    {
                        auto* surfaceElement = map_get_surface_element_at(gridPos);

                        if (surfaceElement == nullptr)
                        {
                            gridPos.setNull();
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

                    auto mapCoords = screen_get_map_xy_quadrant_with_z(screenPos, z, &quadrant);
                    if (!mapCoords)
                    {
                        gridPos.setNull();
                        return;
                    }
                    gridPos = *mapCoords;

                    // If SHIFT pressed
                    if (gSceneryShiftPressed)
                    {
                        z += gSceneryShiftPressZOffset;
                    }

                    z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                    gSceneryPlaceZ = z;
                }

                if (gridPos.isNull())
                    return;

                uint8_t rotation = gWindowSceneryRotation;

                if (!scenery_small_entry_has_flag(scenery, SMALL_SCENERY_FLAG_ROTATABLE))
                {
                    rotation = util_rand() & 0xFF;
                }

                rotation -= get_current_rotation();
                rotation &= 0x3;

                // Also places it in lower but think thats for clobbering
                *parameter_1 = selection.EntryIndex << 8;
                *parameter_2 = (quadrant ^ (1 << 1)) | (gWindowSceneryPrimaryColour << 8);
                *parameter_3 = rotation | (gWindowScenerySecondaryColour << 16);

                if (gConfigGeneral.virtual_floor_style != VirtualFloorStyles::Off)
                {
                    virtual_floor_set_height(gSceneryPlaceZ);
                }

                return;
            }

            // If CTRL not pressed
            if (!gSceneryCtrlPressed)
            {
                auto flags = VIEWPORT_INTERACTION_MASK_TERRAIN & VIEWPORT_INTERACTION_MASK_WATER;

                auto info = get_map_coordinates_from_pos(screenPos, flags);
                gridPos = info.Loc;

                if (info.SpriteType == VIEWPORT_INTERACTION_ITEM_NONE)
                {
                    gridPos.setNull();
                    return;
                }

                // If CTRL and SHIFT not pressed
                gSceneryPlaceZ = 0;

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    auto surfaceElement = map_get_surface_element_at(gridPos);

                    if (surfaceElement == nullptr)
                    {
                        gridPos.setNull();
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
                auto coords = screen_get_map_xy_with_z(screenPos, z);
                if (coords)
                {
                    gridPos = *coords;
                }
                else
                {
                    gridPos.setNull();
                }
                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    z += gSceneryShiftPressZOffset;
                }

                z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                gSceneryPlaceZ = z;
            }

            if (gridPos.isNull())
                return;

            gridPos = gridPos.ToTileStart();
            uint8_t rotation = gWindowSceneryRotation;

            if (!scenery_small_entry_has_flag(scenery, SMALL_SCENERY_FLAG_ROTATABLE))
            {
                rotation = util_rand() & 0xFF;
            }

            rotation -= get_current_rotation();
            rotation &= 0x3;

            // Also places it in lower but think thats for clobbering
            *parameter_1 = selection.EntryIndex << 8;
            *parameter_2 = 0 | (gWindowSceneryPrimaryColour << 8);
            *parameter_3 = rotation | (gWindowScenerySecondaryColour << 16);
            break;
        }
        case SCENERY_TYPE_PATH_ITEM:
        {
            // Path bits
            auto flags = VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_FOOTPATH_ITEM;

            auto info = get_map_coordinates_from_pos(screenPos, flags);
            gridPos = info.Loc;

            if (info.SpriteType == VIEWPORT_INTERACTION_ITEM_NONE)
            {
                gridPos.setNull();
                return;
            }

            *parameter_1 = info.Element->AsPath()->GetSlopeDirection() << 8;
            if (info.Element->AsPath()->IsSloped())
                *parameter_1 |= FOOTPATH_PROPERTIES_FLAG_IS_SLOPED << 8;
            *parameter_2 = info.Element->base_height;
            *parameter_2 |= (info.Element->AsPath()->GetSurfaceEntryIndex() << 8);
            if (info.Element->AsPath()->IsQueue())
            {
                *parameter_2 |= LOCATION_NULL;
            }
            *parameter_3 = selection.EntryIndex + 1;
            break;
        }
        case SCENERY_TYPE_WALL:
        {
            // Walls
            uint8_t cl;
            // If CTRL not pressed
            if (!gSceneryCtrlPressed)
            {
                auto gridCoords = screen_get_map_xy_side(screenPos, &cl);
                if (!gridCoords)
                {
                    gridPos.setNull();
                    return;
                }
                gridPos = *gridCoords;

                gSceneryPlaceZ = 0;

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    auto* surfaceElement = map_get_surface_element_at(gridPos);

                    if (surfaceElement == nullptr)
                    {
                        gridPos.setNull();
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
                auto mapCoords = screen_get_map_xy_side_with_z(screenPos, z, &cl);
                if (!mapCoords)
                {
                    gridPos.setNull();
                    return;
                }
                gridPos = *mapCoords;

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    z += gSceneryShiftPressZOffset;
                }

                z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                gSceneryPlaceZ = z;
            }

            if (gridPos.isNull())
                return;

            _secondaryColour = gWindowScenerySecondaryColour;
            _tertiaryColour = gWindowSceneryTertiaryColour;
            // Also places it in lower but think thats for clobbering
            *parameter_1 = selection.EntryIndex << 8;
            *parameter_2 = cl | (gWindowSceneryPrimaryColour << 8);
            *parameter_3 = 0;
            break;
        }
        case SCENERY_TYPE_LARGE:
        {
            // Large scenery
            // If CTRL not pressed
            if (!gSceneryCtrlPressed)
            {
                const CoordsXY mapCoords = sub_68A15E(screenPos);
                gridPos = mapCoords;

                if (gridPos.isNull())
                    return;

                gSceneryPlaceZ = 0;

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    auto* surfaceElement = map_get_surface_element_at(gridPos);

                    if (surfaceElement == nullptr)
                    {
                        gridPos.setNull();
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
                auto coords = screen_get_map_xy_with_z(screenPos, z);
                if (coords)
                {
                    gridPos = *coords;
                }
                else
                {
                    gridPos.setNull();
                }

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    z += gSceneryShiftPressZOffset;
                }

                z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                gSceneryPlaceZ = z;
            }

            if (gridPos.isNull())
                return;

            gridPos = gridPos.ToTileStart();

            uint8_t rotation = gWindowSceneryRotation;
            rotation -= get_current_rotation();
            rotation &= 0x3;

            *parameter_1 = (rotation << 8);
            *parameter_2 = gWindowSceneryPrimaryColour | (gWindowScenerySecondaryColour << 8);
            *parameter_3 = selection.EntryIndex;
            break;
        }
        case SCENERY_TYPE_BANNER:
        {
            // Banner
            auto flags = VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_FOOTPATH_ITEM;

            auto info = get_map_coordinates_from_pos(screenPos, flags);
            gridPos = info.Loc;

            if (info.SpriteType == VIEWPORT_INTERACTION_ITEM_NONE)
            {
                gridPos.setNull();
                return;
            }

            uint8_t rotation = gWindowSceneryRotation;
            rotation -= get_current_rotation();
            rotation &= 0x3;

            int16_t z = info.Element->base_height;

            if (info.Element->AsPath()->IsSloped())
            {
                if (rotation != direction_reverse(info.Element->AsPath()->GetSlopeDirection()))
                {
                    z += 2;
                }
            }

            z /= 2;

            // Also places it in lower but think thats for clobbering
            *parameter_1 = selection.EntryIndex << 8;
            *parameter_2 = z | (rotation << 8);
            *parameter_3 = gWindowSceneryPrimaryColour;
            break;
        }
    }

    if (gConfigGeneral.virtual_floor_style != VirtualFloorStyles::Off)
    {
        virtual_floor_set_height(gSceneryPlaceZ);
    }
}

static void sub_6E1F34_small_scenery(
    const ScreenCoordsXY& screenCoords, uint16_t sceneryIndex, CoordsXY& gridPos, uint8_t* outQuadrant,
    colour_t* outPrimaryColour, colour_t* outSecondaryColour)
{
    uint32_t parameter1 = 0, parameter2 = 0, parameter3 = 0;
    sub_6E1F34(screenCoords, { SCENERY_TYPE_SMALL, sceneryIndex }, gridPos, &parameter1, &parameter2, &parameter3);

    *outQuadrant = parameter2 & 0xFF;
    *outPrimaryColour = (parameter2 >> 8) & 0xFF;
    *outSecondaryColour = (parameter3 >> 16) & 0xFF;
}

static void sub_6E1F34_path_item(const ScreenCoordsXY& screenCoords, uint16_t sceneryIndex, CoordsXY& gridPos, int32_t* outZ)
{
    uint32_t parameter1 = 0, parameter2 = 0, parameter3 = 0;
    sub_6E1F34(screenCoords, { SCENERY_TYPE_PATH_ITEM, sceneryIndex }, gridPos, &parameter1, &parameter2, &parameter3);

    *outZ = (parameter2 & 0xFF) * COORDS_Z_STEP;
}

static void sub_6E1F34_wall(
    const ScreenCoordsXY& screenCoords, uint16_t sceneryIndex, CoordsXY& gridPos, colour_t* outPrimaryColour, uint8_t* outEdges)
{
    uint32_t parameter1 = 0, parameter2 = 0, parameter3 = 0;
    sub_6E1F34(screenCoords, { SCENERY_TYPE_WALL, sceneryIndex }, gridPos, &parameter1, &parameter2, &parameter3);

    *outPrimaryColour = (parameter2 >> 8) & 0xFF;
    *outEdges = parameter2 & 0xFF;
}

static void sub_6E1F34_large_scenery(
    const ScreenCoordsXY& screenCoords, uint16_t sceneryIndex, CoordsXY& gridPos, colour_t* outPrimaryColour,
    colour_t* outSecondaryColour, Direction* outDirection)
{
    uint32_t parameter1 = 0, parameter2 = 0, parameter3 = 0;
    sub_6E1F34(screenCoords, { SCENERY_TYPE_LARGE, sceneryIndex }, gridPos, &parameter1, &parameter2, &parameter3);

    *outPrimaryColour = parameter2 & 0xFF;
    *outSecondaryColour = (parameter2 >> 8) & 0xFF;
    *outDirection = (parameter1 & 0xFF00) >> 8;
}

static void sub_6E1F34_banner(
    const ScreenCoordsXY& screenCoords, uint16_t sceneryIndex, CoordsXY& gridPos, int32_t* outZ, Direction* outDirection)
{
    uint32_t parameter1 = 0, parameter2 = 0, parameter3 = 0;
    sub_6E1F34(screenCoords, { SCENERY_TYPE_BANNER, sceneryIndex }, gridPos, &parameter1, &parameter2, &parameter3);

    *outDirection = (parameter2 >> 8) & 0xFF;
    *outZ = (parameter2 & 0xFF) * COORDS_Z_PER_TINY_Z;
}

/**
 *
 *  rct2: 0x006E2CC6
 */
static void window_top_toolbar_scenery_tool_down(const ScreenCoordsXY& windowPos, rct_window* w, rct_widgetindex widgetIndex)
{
    scenery_remove_ghost_tool_placement();
    if (gWindowSceneryPaintEnabled & 1)
    {
        repaint_scenery_tool_down(windowPos, widgetIndex);
        return;
    }
    else if (gWindowSceneryEyedropperEnabled)
    {
        scenery_eyedropper_tool_down(windowPos, widgetIndex);
        return;
    }

    ScenerySelection selectedTab = gWindowSceneryTabSelections[gWindowSceneryActiveTabIndex];
    uint8_t sceneryType = selectedTab.SceneryType;
    uint16_t selectedScenery = selectedTab.EntryIndex;

    if (selectedTab.IsUndefined())
        return;

    CoordsXY gridPos;

    switch (sceneryType)
    {
        case SCENERY_TYPE_SMALL:
        {
            uint8_t quadrant;
            colour_t primaryColour;
            colour_t secondaryColour;
            sub_6E1F34_small_scenery(windowPos, selectedScenery, gridPos, &quadrant, &primaryColour, &secondaryColour);
            if (gridPos.isNull())
                return;

            int32_t quantity = 1;
            bool isCluster = gWindowSceneryScatterEnabled
                && (network_get_mode() != NETWORK_MODE_CLIENT
                    || network_can_perform_command(network_get_current_player_group_index(), -2));

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
                rct_scenery_entry* scenery = get_small_scenery_entry(selectedScenery);

                int16_t cur_grid_x = gridPos.x;
                int16_t cur_grid_y = gridPos.y;

                if (isCluster)
                {
                    if (!scenery_small_entry_has_flag(scenery, SMALL_SCENERY_FLAG_FULL_TILE))
                    {
                        quadrant = util_rand() & 3;
                    }

                    int16_t grid_x_offset = (util_rand() % gWindowSceneryScatterSize) - (gWindowSceneryScatterSize / 2);
                    int16_t grid_y_offset = (util_rand() % gWindowSceneryScatterSize) - (gWindowSceneryScatterSize / 2);
                    if (gWindowSceneryScatterSize % 2 == 0)
                    {
                        grid_x_offset += 1;
                        grid_y_offset += 1;
                    }
                    cur_grid_x += grid_x_offset * COORDS_XY_STEP;
                    cur_grid_y += grid_y_offset * COORDS_XY_STEP;

                    if (!scenery_small_entry_has_flag(scenery, SMALL_SCENERY_FLAG_ROTATABLE))
                    {
                        gSceneryPlaceRotation = (gSceneryPlaceRotation + 1) & 3;
                    }
                }

                uint8_t zAttemptRange = 1;
                if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
                {
                    zAttemptRange = 20;
                }

                auto success = GA_ERROR::UNKNOWN;
                // Try find a valid z coordinate
                for (; zAttemptRange != 0; zAttemptRange--)
                {
                    auto smallSceneryPlaceAction = SmallSceneryPlaceAction(
                        { cur_grid_x, cur_grid_y, gSceneryPlaceZ, gSceneryPlaceRotation }, quadrant, selectedScenery,
                        primaryColour, secondaryColour);
                    auto res = GameActions::Query(&smallSceneryPlaceAction);
                    success = res->Error;
                    if (res->Error == GA_ERROR::OK)
                    {
                        break;
                    }

                    if (res->Error == GA_ERROR::INSUFFICIENT_FUNDS)
                    {
                        break;
                    }
                    if (zAttemptRange != 1)
                    {
                        gSceneryPlaceZ += 8;
                    }
                }

                // Actually place
                if (success == GA_ERROR::OK || ((q + 1 == quantity) && forceError))
                {
                    auto smallSceneryPlaceAction = SmallSceneryPlaceAction(
                        { cur_grid_x, cur_grid_y, gSceneryPlaceZ, gSceneryPlaceRotation }, quadrant, selectedScenery,
                        primaryColour, secondaryColour);

                    smallSceneryPlaceAction.SetCallback([=](const GameAction* ga, const GameActionResult* result) {
                        if (result->Error == GA_ERROR::OK)
                        {
                            audio_play_sound_at_location(SoundId::PlaceItem, result->Position);
                        }
                    });
                    auto res = GameActions::Execute(&smallSceneryPlaceAction);
                    if (res->Error == GA_ERROR::OK)
                    {
                        forceError = false;
                    }

                    if (res->Error == GA_ERROR::INSUFFICIENT_FUNDS)
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
            sub_6E1F34_path_item(windowPos, selectedScenery, gridPos, &z);
            if (gridPos.isNull())
                return;

            auto footpathSceneryPlaceAction = FootpathSceneryPlaceAction({ gridPos, z }, selectedScenery + 1);

            footpathSceneryPlaceAction.SetCallback([](const GameAction* ga, const GameActionResult* result) {
                if (result->Error != GA_ERROR::OK)
                {
                    return;
                }
                audio_play_sound_at_location(SoundId::PlaceItem, result->Position);
            });
            auto res = GameActions::Execute(&footpathSceneryPlaceAction);
            break;
        }
        case SCENERY_TYPE_WALL:
        {
            colour_t primaryColour;
            uint8_t edges;
            sub_6E1F34_wall(windowPos, selectedScenery, gridPos, &primaryColour, &edges);
            if (gridPos.isNull())
                return;

            uint8_t zAttemptRange = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                zAttemptRange = 20;
            }

            for (; zAttemptRange != 0; zAttemptRange--)
            {
                auto wallPlaceAction = WallPlaceAction(
                    selectedScenery, { gridPos, gSceneryPlaceZ }, edges, primaryColour, _secondaryColour, _tertiaryColour);

                auto res = GameActions::Query(&wallPlaceAction);
                if (res->Error == GA_ERROR::OK)
                {
                    break;
                }

                if (auto message = res->ErrorMessage.AsStringId())
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
                selectedScenery, { gridPos, gSceneryPlaceZ }, edges, primaryColour, _secondaryColour, _tertiaryColour);

            wallPlaceAction.SetCallback([](const GameAction* ga, const GameActionResult* result) {
                if (result->Error == GA_ERROR::OK)
                {
                    audio_play_sound_at_location(SoundId::PlaceItem, result->Position);
                }
            });
            auto res = GameActions::Execute(&wallPlaceAction);
            break;
        }
        case SCENERY_TYPE_LARGE:
        {
            colour_t primaryColour;
            colour_t secondaryColour;
            Direction direction;
            sub_6E1F34_large_scenery(windowPos, selectedScenery, gridPos, &primaryColour, &secondaryColour, &direction);
            if (gridPos.isNull())
                return;

            uint8_t zAttemptRange = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                zAttemptRange = 20;
            }

            for (; zAttemptRange != 0; zAttemptRange--)
            {
                CoordsXYZD loc = { gridPos, gSceneryPlaceZ, direction };

                auto sceneryPlaceAction = LargeSceneryPlaceAction(loc, selectedScenery, primaryColour, secondaryColour);

                auto res = GameActions::Query(&sceneryPlaceAction);
                if (res->Error == GA_ERROR::OK)
                {
                    break;
                }

                if (auto message = res->ErrorMessage.AsStringId())
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

            auto sceneryPlaceAction = LargeSceneryPlaceAction(loc, selectedScenery, primaryColour, secondaryColour);
            sceneryPlaceAction.SetCallback([=](const GameAction* ga, const GameActionResult* result) {
                if (result->Error == GA_ERROR::OK)
                {
                    audio_play_sound_at_location(SoundId::PlaceItem, result->Position);
                }
                else
                {
                    audio_play_sound_at_location(SoundId::Error, { loc.x, loc.y, gSceneryPlaceZ });
                }
            });
            auto res = GameActions::Execute(&sceneryPlaceAction);
            break;
        }
        case SCENERY_TYPE_BANNER:
        {
            int32_t z;
            Direction direction;
            sub_6E1F34_banner(windowPos, selectedScenery, gridPos, &z, &direction);
            if (gridPos.isNull())
                return;

            CoordsXYZD loc{ gridPos, z, direction };
            auto primaryColour = gWindowSceneryPrimaryColour;
            auto bannerIndex = create_new_banner(0);
            if (bannerIndex == BANNER_INDEX_NULL)
            {
                context_show_error(STR_CANT_POSITION_THIS_HERE, STR_TOO_MANY_BANNERS_IN_GAME);
                break;
            }
            auto bannerPlaceAction = BannerPlaceAction(loc, selectedScenery, bannerIndex, primaryColour);
            bannerPlaceAction.SetCallback([=](const GameAction* ga, const GameActionResult* result) {
                if (result->Error == GA_ERROR::OK)
                {
                    audio_play_sound_at_location(SoundId::PlaceItem, result->Position);
                    context_open_detail_window(WD_BANNER, bannerIndex);
                }
            });
            GameActions::Execute(&bannerPlaceAction);
            break;
        }
    }
}

static uint8_t top_toolbar_tool_update_land_paint(const ScreenCoordsXY& screenPos)
{
    uint8_t state_changed = 0;

    map_invalidate_selection_rect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

    auto mapTile = screen_get_map_xy(screenPos, nullptr);

    if (!mapTile)
    {
        if (gClearSceneryCost != MONEY32_UNDEFINED)
        {
            gClearSceneryCost = MONEY32_UNDEFINED;
            window_invalidate_by_class(WC_CLEAR_SCENERY);
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

    map_invalidate_selection_rect();
    return state_changed;
}

/**
 *
 *  rct2: 0x0068E213
 */
static void top_toolbar_tool_update_scenery_clear(const ScreenCoordsXY& screenPos)
{
    if (!top_toolbar_tool_update_land_paint(screenPos))
        return;

    auto action = GetClearAction();
    auto result = GameActions::Query(&action);
    auto cost = (result->Error == GA_ERROR::OK ? result->Cost : MONEY32_UNDEFINED);
    if (gClearSceneryCost != cost)
    {
        gClearSceneryCost = cost;
        window_invalidate_by_class(WC_CLEAR_SCENERY);
    }
}

/**
 *
 *  rct2: 0x00664280
 */
static void top_toolbar_tool_update_land(const ScreenCoordsXY& screenPos)
{
    const bool mapCtrlPressed = input_test_place_object_modifier(PLACE_OBJECT_MODIFIER_COPY_Z);

    map_invalidate_selection_rect();

    if (gCurrentToolId == TOOL_UP_DOWN_ARROW)
    {
        if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
            return;

        money32 lower_cost = selection_lower_land(0);
        money32 raise_cost = selection_raise_land(0);

        if (gLandToolRaiseCost != raise_cost || gLandToolLowerCost != lower_cost)
        {
            gLandToolRaiseCost = raise_cost;
            gLandToolLowerCost = lower_cost;
            window_invalidate_by_class(WC_LAND);
        }
        return;
    }

    int16_t tool_size = gLandToolSize;
    std::optional<CoordsXY> mapTile;
    uint8_t side;

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    if (tool_size == 1)
    {
        int32_t selectionType;
        // Get selection type and map coordinates from mouse x,y position
        screen_pos_to_map_pos(screenPos, &selectionType);
        mapTile = screen_get_map_xy_side(screenPos, &side);

        if (!mapTile)
        {
            money32 lower_cost = MONEY32_UNDEFINED;
            money32 raise_cost = MONEY32_UNDEFINED;

            if (gLandToolRaiseCost != raise_cost || gLandToolLowerCost != lower_cost)
            {
                gLandToolRaiseCost = raise_cost;
                gLandToolLowerCost = lower_cost;
                window_invalidate_by_class(WC_LAND);
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

        map_invalidate_selection_rect();
        if (!state_changed)
            return;

        money32 lower_cost = selection_lower_land(0);
        money32 raise_cost = selection_raise_land(0);

        if (gLandToolRaiseCost != raise_cost || gLandToolLowerCost != lower_cost)
        {
            gLandToolRaiseCost = raise_cost;
            gLandToolLowerCost = lower_cost;
            window_invalidate_by_class(WC_LAND);
        }
        return;
    }

    // Get map coordinates and the side of the tile that is being hovered over
    mapTile = screen_get_map_xy_side(screenPos, &side);

    if (!mapTile)
    {
        money32 lower_cost = MONEY32_UNDEFINED;
        money32 raise_cost = MONEY32_UNDEFINED;

        if (gLandToolRaiseCost != raise_cost || gLandToolLowerCost != lower_cost)
        {
            gLandToolRaiseCost = raise_cost;
            gLandToolLowerCost = lower_cost;
            window_invalidate_by_class(WC_LAND);
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

    map_invalidate_selection_rect();
    if (!state_changed)
        return;

    money32 lower_cost = selection_lower_land(0);
    money32 raise_cost = selection_raise_land(0);

    if (gLandToolRaiseCost != raise_cost || gLandToolLowerCost != lower_cost)
    {
        gLandToolRaiseCost = raise_cost;
        gLandToolLowerCost = lower_cost;
        window_invalidate_by_class(WC_LAND);
    }
}

/**
 *
 *  rct2: 0x006E6BDC
 */
static void top_toolbar_tool_update_water(const ScreenCoordsXY& screenPos)
{
    map_invalidate_selection_rect();

    if (gCurrentToolId == TOOL_UP_DOWN_ARROW)
    {
        if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
            return;

        auto waterLowerAction = WaterLowerAction(
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
        auto waterRaiseAction = WaterRaiseAction(
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });

        auto res = GameActions::Query(&waterLowerAction);
        money32 lowerCost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;

        res = GameActions::Query(&waterRaiseAction);
        money32 raiseCost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;

        if (gWaterToolRaiseCost != raiseCost || gWaterToolLowerCost != lowerCost)
        {
            gWaterToolRaiseCost = raiseCost;
            gWaterToolLowerCost = lowerCost;
            window_invalidate_by_class(WC_WATER);
        }
        return;
    }

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

    auto info = get_map_coordinates_from_pos(screenPos, VIEWPORT_INTERACTION_MASK_TERRAIN & VIEWPORT_INTERACTION_MASK_WATER);

    if (info.SpriteType == VIEWPORT_INTERACTION_ITEM_NONE)
    {
        if (gWaterToolRaiseCost != MONEY32_UNDEFINED || gWaterToolLowerCost != MONEY32_UNDEFINED)
        {
            gWaterToolRaiseCost = MONEY32_UNDEFINED;
            gWaterToolLowerCost = MONEY32_UNDEFINED;
            window_invalidate_by_class(WC_WATER);
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

    map_invalidate_selection_rect();
    if (!state_changed)
        return;

    auto waterLowerAction = WaterLowerAction(
        { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
    auto waterRaiseAction = WaterRaiseAction(
        { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });

    auto res = GameActions::Query(&waterLowerAction);
    money32 lowerCost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;

    res = GameActions::Query(&waterRaiseAction);
    money32 raiseCost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;

    if (gWaterToolRaiseCost != raiseCost || gWaterToolLowerCost != lowerCost)
    {
        gWaterToolRaiseCost = raiseCost;
        gWaterToolLowerCost = lowerCost;
        window_invalidate_by_class(WC_WATER);
    }
}

/**
 *
 *  rct2: 0x006E24F6
 * On failure returns MONEY32_UNDEFINED
 * On success places ghost scenery and returns cost to place proper
 */
static money32 try_place_ghost_scenery(
    CoordsXY map_tile, uint32_t parameter_1, uint32_t parameter_2, uint32_t parameter_3, uint8_t scenery_type,
    ObjectEntryIndex entryIndex)
{
    scenery_remove_ghost_tool_placement();

    money32 cost = 0;
    TileElement* tileElement;

    switch (scenery_type)
    {
        case SCENERY_TYPE_SMALL:
        {
            // Small Scenery
            // 6e252b
            uint8_t quadrant = parameter_2 & 0xFF;
            uint8_t primaryColour = (parameter_2 >> 8) & 0xFF;
            uint8_t secondaryColour = (parameter_3 >> 16) & 0xFF;
            uint8_t rotation = parameter_3 & 0xFF;
            auto smallSceneryPlaceAction = SmallSceneryPlaceAction(
                { map_tile.x, map_tile.y, gSceneryPlaceZ, rotation }, quadrant, entryIndex, primaryColour, secondaryColour);
            smallSceneryPlaceAction.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
            auto res = GameActions::Execute(&smallSceneryPlaceAction);
            auto sspar = dynamic_cast<SmallSceneryPlaceActionResult*>(res.get());
            if (sspar == nullptr || res->Error != GA_ERROR::OK)
                return MONEY32_UNDEFINED;

            gSceneryPlaceRotation = static_cast<uint16_t>(parameter_3 & 0xFF);
            gSceneryPlaceObject.SceneryType = SCENERY_TYPE_SMALL;
            gSceneryPlaceObject.EntryIndex = entryIndex;

            tileElement = sspar->tileElement;
            gSceneryGhostPosition = { map_tile, tileElement->GetBaseZ() };
            gSceneryQuadrant = tileElement->AsSmallScenery()->GetSceneryQuadrant();
            if (sspar->GroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                // Set underground on
                viewport_set_visibility(4);
            }
            else
            {
                // Set underground off
                viewport_set_visibility(5);
            }

            gSceneryGhostType |= SCENERY_GHOST_FLAG_0;
            cost = res->Cost;
            break;
        }
        case SCENERY_TYPE_PATH_ITEM:
        {
            // Path Bits
            // 6e265b
            int32_t z = (parameter_2 & 0xFF) * COORDS_Z_STEP;
            auto footpathSceneryPlaceAction = FootpathSceneryPlaceAction({ map_tile.x, map_tile.y, z }, entryIndex + 1);
            footpathSceneryPlaceAction.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
            footpathSceneryPlaceAction.SetCallback([=](const GameAction* ga, const GameActionResult* result) {
                if (result->Error != GA_ERROR::OK)
                {
                    return;
                }
                gSceneryGhostPosition = { map_tile, static_cast<int32_t>((parameter_2 & 0xFF) * COORDS_Z_STEP) };
                gSceneryGhostType |= SCENERY_GHOST_FLAG_1;
            });
            auto res = GameActions::Execute(&footpathSceneryPlaceAction);
            if (res->Error != GA_ERROR::OK)
                return MONEY32_UNDEFINED;

            cost = res->Cost;
            break;
        }
        case SCENERY_TYPE_WALL:
        {
            // Walls
            // 6e26b0
            auto primaryColour = (parameter_2 >> 8) & 0xFF;
            auto edges = parameter_2 & 0xFF;
            auto wallPlaceAction = WallPlaceAction(
                entryIndex, { map_tile.x, map_tile.y, gSceneryPlaceZ }, edges, primaryColour, _secondaryColour,
                _tertiaryColour);
            wallPlaceAction.SetFlags(
                GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND);
            wallPlaceAction.SetCallback([=](const GameAction* ga, const WallPlaceActionResult* result) {
                if (result->Error != GA_ERROR::OK)
                    return;

                gSceneryGhostPosition = { map_tile, result->tileElement->GetBaseZ() };
                gSceneryGhostWallRotation = edges;

                gSceneryGhostType |= SCENERY_GHOST_FLAG_2;
            });

            auto res = GameActions::Execute(&wallPlaceAction);
            if (res->Error != GA_ERROR::OK)
                return MONEY32_UNDEFINED;
            cost = res->Cost;
            break;
        }
        case SCENERY_TYPE_LARGE:
        {
            // Large Scenery
            // 6e25a7
            auto primaryColour = parameter_2 & 0xFF;
            auto secondaryColour = (parameter_2 >> 8) & 0xFF;
            uint8_t direction = (parameter_1 & 0xFF00) >> 8;
            CoordsXYZD loc = { map_tile.x, map_tile.y, gSceneryPlaceZ, direction };

            auto sceneryPlaceAction = LargeSceneryPlaceAction(loc, entryIndex, primaryColour, secondaryColour);
            sceneryPlaceAction.SetFlags(
                GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND);
            auto res = GameActions::Execute(&sceneryPlaceAction);
            auto lspar = dynamic_cast<LargeSceneryPlaceActionResult*>(res.get());
            if (lspar == nullptr || res->Error != GA_ERROR::OK)
                return MONEY32_UNDEFINED;

            gSceneryPlaceRotation = loc.direction;

            tileElement = lspar->tileElement;
            gSceneryGhostPosition = { map_tile, tileElement->GetBaseZ() };

            if (lspar->GroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                // Set underground on
                viewport_set_visibility(4);
            }
            else
            {
                // Set underground off
                viewport_set_visibility(5);
            }

            gSceneryGhostType |= SCENERY_GHOST_FLAG_3;
            cost = res->Cost;
            break;
        }
        case SCENERY_TYPE_BANNER:
        {
            // Banners
            // 6e2612
            uint8_t direction = (parameter_2 >> 8) & 0xFF;
            int32_t z = (parameter_2 & 0xFF) * COORDS_Z_PER_TINY_Z;
            CoordsXYZD loc{ map_tile.x, map_tile.y, z, direction };
            auto primaryColour = gWindowSceneryPrimaryColour;
            auto bannerIndex = create_new_banner(0);
            if (bannerIndex == BANNER_INDEX_NULL)
            {
                // Silently fail as this is just for the ghost
                break;
            }
            auto bannerPlaceAction = BannerPlaceAction(loc, entryIndex, bannerIndex, primaryColour);
            bannerPlaceAction.SetFlags(
                GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND);
            auto res = GameActions::Execute(&bannerPlaceAction);
            if (res->Error != GA_ERROR::OK)
                return MONEY32_UNDEFINED;

            gSceneryGhostPosition = loc;
            gSceneryGhostPosition.z += PATH_HEIGHT_STEP;
            gSceneryPlaceRotation = direction;
            gSceneryGhostType |= SCENERY_GHOST_FLAG_4;
            cost = res->Cost;
            break;
        }
    }

    return cost;
}

/**
 *
 *  rct2: 0x006E287B
 */
static void top_toolbar_tool_update_scenery(const ScreenCoordsXY& screenPos)
{
    map_invalidate_selection_rect();
    map_invalidate_map_selection_tiles();

    if (gConfigGeneral.virtual_floor_style != VirtualFloorStyles::Off)
    {
        virtual_floor_invalidate();
    }

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;

    if (gWindowSceneryPaintEnabled)
        return;
    if (gWindowSceneryEyedropperEnabled)
        return;

    ScenerySelection selection = gWindowSceneryTabSelections[gWindowSceneryActiveTabIndex];

    if (selection.IsUndefined())
    {
        scenery_remove_ghost_tool_placement();
        return;
    }

    uint8_t sceneryType = selection.SceneryType;
    uint16_t selectedScenery = selection.EntryIndex;
    CoordsXY mapTile = {};
    uint32_t parameter1, parameter2, parameter3;

    sub_6E1F34(screenPos, selection, mapTile, &parameter1, &parameter2, &parameter3);

    if (mapTile.isNull())
    {
        scenery_remove_ghost_tool_placement();
        return;
    }

    rct_scenery_entry* scenery;
    uint8_t bl;
    money32 cost = 0;

    switch (sceneryType)
    {
        case SCENERY_TYPE_SMALL:
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

            scenery = get_small_scenery_entry(selectedScenery);

            gMapSelectType = MAP_SELECT_TYPE_FULL;
            if (!scenery_small_entry_has_flag(scenery, SMALL_SCENERY_FLAG_FULL_TILE) && !gWindowSceneryScatterEnabled)
            {
                gMapSelectType = MAP_SELECT_TYPE_QUARTER_0 + ((parameter2 & 0xFF) ^ 2);
            }

            map_invalidate_selection_rect();

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_0) && mapTile == gSceneryGhostPosition
                && (parameter2 & 0xFF) == _unkF64F0E && gSceneryPlaceZ == _unkF64F0A
                && gSceneryPlaceObject.SceneryType == SCENERY_TYPE_SMALL && gSceneryPlaceObject.EntryIndex == selectedScenery)
            {
                return;
            }

            scenery_remove_ghost_tool_placement();

            _unkF64F0E = (parameter2 & 0xFF);
            _unkF64F0A = gSceneryPlaceZ;

            bl = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                bl = 20;
            }

            for (; bl != 0; bl--)
            {
                cost = try_place_ghost_scenery(mapTile, parameter1, parameter2, parameter3, sceneryType, selectedScenery);

                if (cost != MONEY32_UNDEFINED)
                    break;
                gSceneryPlaceZ += 8;
            }

            gSceneryPlaceCost = cost;
            break;
        case SCENERY_TYPE_PATH_ITEM:
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
            gMapSelectPositionA.x = mapTile.x;
            gMapSelectPositionA.y = mapTile.y;
            gMapSelectPositionB.x = mapTile.x;
            gMapSelectPositionB.y = mapTile.y;
            gMapSelectType = MAP_SELECT_TYPE_FULL;

            map_invalidate_selection_rect();

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_1) && mapTile == gSceneryGhostPosition
                && static_cast<int16_t>(parameter2 & 0xFF) * COORDS_Z_STEP == gSceneryGhostPosition.z)
            {
                return;
            }

            scenery_remove_ghost_tool_placement();

            cost = try_place_ghost_scenery(mapTile, parameter1, parameter2, parameter3, sceneryType, selectedScenery);

            gSceneryPlaceCost = cost;
            break;
        case SCENERY_TYPE_WALL:
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
            gMapSelectPositionA.x = mapTile.x;
            gMapSelectPositionA.y = mapTile.y;
            gMapSelectPositionB.x = mapTile.x;
            gMapSelectPositionB.y = mapTile.y;
            gMapSelectType = MAP_SELECT_TYPE_EDGE_0 + (parameter2 & 0xFF);

            map_invalidate_selection_rect();

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_2) && mapTile == gSceneryGhostPosition
                && (parameter2 & 0xFF) == gSceneryGhostWallRotation && gSceneryPlaceZ == _unkF64F0A)
            {
                return;
            }

            scenery_remove_ghost_tool_placement();

            gSceneryGhostWallRotation = (parameter2 & 0xFF);
            _unkF64F0A = gSceneryPlaceZ;

            bl = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                bl = 20;
            }

            cost = 0;
            for (; bl != 0; bl--)
            {
                cost = try_place_ghost_scenery(mapTile, parameter1, parameter2, parameter3, sceneryType, selectedScenery);

                if (cost != MONEY32_UNDEFINED)
                    break;
                gSceneryPlaceZ += 8;
            }

            gSceneryPlaceCost = cost;
            break;
        case SCENERY_TYPE_LARGE:
        {
            scenery = get_large_scenery_entry(selectedScenery);
            gMapSelectionTiles.clear();

            for (rct_large_scenery_tile* tile = scenery->large_scenery.tiles;
                 tile->x_offset != static_cast<int16_t>(static_cast<uint16_t>(0xFFFF)); tile++)
            {
                CoordsXY tileLocation = { tile->x_offset, tile->y_offset };
                auto rotatedTileCoords = tileLocation.Rotate((parameter1 >> 8) & 0xFF);

                rotatedTileCoords.x += mapTile.x;
                rotatedTileCoords.y += mapTile.y;

                gMapSelectionTiles.push_back(rotatedTileCoords);
            }

            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            map_invalidate_map_selection_tiles();

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_3) && mapTile == gSceneryGhostPosition && gSceneryPlaceZ == _unkF64F0A
                && gSceneryPlaceObject.SceneryType == SCENERY_TYPE_LARGE && gSceneryPlaceObject.EntryIndex == selectedScenery)
            {
                return;
            }

            scenery_remove_ghost_tool_placement();

            gSceneryPlaceObject.SceneryType = SCENERY_TYPE_LARGE;
            gSceneryPlaceObject.EntryIndex = selectedScenery;
            _unkF64F0A = gSceneryPlaceZ;

            bl = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                bl = 20;
            }

            cost = 0;
            for (; bl != 0; bl--)
            {
                cost = try_place_ghost_scenery(mapTile, parameter1, parameter2, parameter3, sceneryType, selectedScenery);

                if (cost != MONEY32_UNDEFINED)
                    break;
                gSceneryPlaceZ += 8;
            }

            gSceneryPlaceCost = cost;
            break;
        }
        case SCENERY_TYPE_BANNER:
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
            gMapSelectPositionA.x = mapTile.x;
            gMapSelectPositionA.y = mapTile.y;
            gMapSelectPositionB.x = mapTile.x;
            gMapSelectPositionB.y = mapTile.y;
            gMapSelectType = MAP_SELECT_TYPE_FULL;

            map_invalidate_selection_rect();

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_4) && mapTile == gSceneryGhostPosition
                && static_cast<int16_t>(parameter2 & 0xFF) * COORDS_Z_STEP == gSceneryGhostPosition.z
                && ((parameter2 >> 8) & 0xFF) == gSceneryPlaceRotation)
            {
                return;
            }

            scenery_remove_ghost_tool_placement();

            cost = try_place_ghost_scenery(mapTile, parameter1, parameter2, parameter3, sceneryType, selectedScenery);

            gSceneryPlaceCost = cost;
            break;
    }
}

/**
 *
 *  rct2: 0x0066CB25
 */
static void window_top_toolbar_tool_update(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_CLEAR_SCENERY:
            top_toolbar_tool_update_scenery_clear(screenCoords);
            break;
        case WIDX_LAND:
            if (gLandPaintMode)
                top_toolbar_tool_update_land_paint(screenCoords);
            else
                top_toolbar_tool_update_land(screenCoords);
            break;
        case WIDX_WATER:
            top_toolbar_tool_update_water(screenCoords);
            break;
        case WIDX_SCENERY:
            top_toolbar_tool_update_scenery(screenCoords);
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
static void window_top_toolbar_tool_down(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_CLEAR_SCENERY:
            if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
            {
                auto action = GetClearAction();
                GameActions::Execute(&action);
                gCurrentToolId = TOOL_CROSSHAIR;
            }
            break;
        case WIDX_LAND:
            if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
            {
                auto surfaceSetStyleAction = SurfaceSetStyleAction(
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                    gLandToolTerrainSurface, gLandToolTerrainEdge);

                GameActions::Execute(&surfaceSetStyleAction);

                gCurrentToolId = TOOL_UP_DOWN_ARROW;
            }
            else
            {
                _landToolBlocked = true;
            }
            break;
        case WIDX_WATER:
            if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
            {
                gCurrentToolId = TOOL_UP_DOWN_ARROW;
            }
            else
            {
                _landToolBlocked = true;
            }
            break;
        case WIDX_SCENERY:
            window_top_toolbar_scenery_tool_down(screenCoords, w, widgetIndex);
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
static money32 selection_raise_land(uint8_t flags)
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
        return res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
    }
    else
    {
        auto landRaiseAction = LandRaiseAction(
            { centreX, centreY },
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y }, gMapSelectType);
        auto res = (flags & GAME_COMMAND_FLAG_APPLY) ? GameActions::Execute(&landRaiseAction)
                                                     : GameActions::Query(&landRaiseAction);

        return res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
    }
}

/**
 *
 *  rct2: 0x006645B3
 */
static money32 selection_lower_land(uint8_t flags)
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
        return res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
    }
    else
    {
        auto landLowerAction = LandLowerAction(
            { centreX, centreY },
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y }, gMapSelectType);
        auto res = (flags & GAME_COMMAND_FLAG_APPLY) ? GameActions::Execute(&landLowerAction)
                                                     : GameActions::Query(&landLowerAction);

        return res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
    }
}

/**
 *  part of window_top_toolbar_tool_drag(0x0066CB4E)
 *  rct2: 0x00664454
 */
static void window_top_toolbar_land_tool_drag(const ScreenCoordsXY& screenPos)
{
    rct_window* window = window_find_from_point(screenPos);
    if (!window)
        return;
    rct_widgetindex widget_index = window_find_widget_from_point(window, screenPos);
    if (widget_index == -1)
        return;
    rct_widget* widget = &window->widgets[widget_index];
    if (widget->type != WWT_VIEWPORT)
        return;
    rct_viewport* viewport = window->viewport;
    if (!viewport)
        return;

    int16_t tile_height = -16 / viewport->zoom;

    int32_t y_diff = screenPos.y - gInputDragLast.y;

    if (y_diff <= tile_height)
    {
        gInputDragLast.y += tile_height;

        selection_raise_land(GAME_COMMAND_FLAG_APPLY);

        gLandToolRaiseCost = MONEY32_UNDEFINED;
        gLandToolLowerCost = MONEY32_UNDEFINED;
    }
    else if (y_diff >= -tile_height)
    {
        gInputDragLast.y -= tile_height;

        selection_lower_land(GAME_COMMAND_FLAG_APPLY);

        gLandToolRaiseCost = MONEY32_UNDEFINED;
        gLandToolLowerCost = MONEY32_UNDEFINED;
    }
}

/**
 *  part of window_top_toolbar_tool_drag(0x0066CB4E)
 *  rct2: 0x006E6D4B
 */
static void window_top_toolbar_water_tool_drag(const ScreenCoordsXY& screenPos)
{
    rct_window* window = window_find_from_point(screenPos);
    if (!window)
        return;
    rct_widgetindex widget_index = window_find_widget_from_point(window, screenPos);
    if (widget_index == -1)
        return;
    rct_widget* widget = &window->widgets[widget_index];
    if (widget->type != WWT_VIEWPORT)
        return;
    rct_viewport* viewport = window->viewport;
    if (!viewport)
        return;

    int16_t dx = -16 / viewport->zoom;

    auto offsetPos = screenPos - ScreenCoordsXY{ 0, gInputDragLast.y };

    if (offsetPos.y <= dx)
    {
        gInputDragLast.y += dx;

        auto waterRaiseAction = WaterRaiseAction(
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
        GameActions::Execute(&waterRaiseAction);

        gWaterToolRaiseCost = MONEY32_UNDEFINED;
        gWaterToolLowerCost = MONEY32_UNDEFINED;

        return;
    }

    dx = -dx;

    if (offsetPos.y >= dx)
    {
        gInputDragLast.y += dx;

        auto waterLowerAction = WaterLowerAction(
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
        GameActions::Execute(&waterLowerAction);
        gWaterToolRaiseCost = MONEY32_UNDEFINED;
        gWaterToolLowerCost = MONEY32_UNDEFINED;

        return;
    }
}

/**
 *
 *  rct2: 0x0066CB4E
 */
static void window_top_toolbar_tool_drag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_CLEAR_SCENERY:
            if (window_find_by_class(WC_ERROR) == nullptr && (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
            {
                auto action = GetClearAction();
                GameActions::Execute(&action);
                gCurrentToolId = TOOL_CROSSHAIR;
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
                    gCurrentToolId = TOOL_CROSSHAIR;
                }
            }
            else
            {
                if (!_landToolBlocked)
                {
                    window_top_toolbar_land_tool_drag(screenCoords);
                }
            }
            break;
        case WIDX_WATER:
            if (!_landToolBlocked)
            {
                window_top_toolbar_water_tool_drag(screenCoords);
            }
            break;
        case WIDX_SCENERY:
            if (gWindowSceneryPaintEnabled & 1)
                window_top_toolbar_scenery_tool_down(screenCoords, w, widgetIndex);
            if (gWindowSceneryEyedropperEnabled)
                window_top_toolbar_scenery_tool_down(screenCoords, w, widgetIndex);
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
static void window_top_toolbar_tool_up(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    _landToolBlocked = false;
    switch (widgetIndex)
    {
        case WIDX_LAND:
            map_invalidate_selection_rect();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            gCurrentToolId = TOOL_DIG_DOWN;
            break;
        case WIDX_WATER:
            map_invalidate_selection_rect();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            gCurrentToolId = TOOL_WATER_DOWN;
            break;
        case WIDX_CLEAR_SCENERY:
            map_invalidate_selection_rect();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            gCurrentToolId = TOOL_CROSSHAIR;
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
static void window_top_toolbar_tool_abort(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_LAND:
        case WIDX_WATER:
        case WIDX_CLEAR_SCENERY:
            hide_gridlines();
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

static void top_toolbar_init_map_menu(rct_window* w, rct_widget* widget)
{
    auto i = 0;
    gDropdownItemsFormat[i++] = STR_SHORTCUT_SHOW_MAP;
    gDropdownItemsFormat[i++] = STR_EXTRA_VIEWPORT;
    if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR)
    {
        gDropdownItemsFormat[i++] = STR_MAPGEN_WINDOW_TITLE;
    }

#ifdef ENABLE_SCRIPTING
    const auto& customMenuItems = OpenRCT2::Scripting::CustomMenuItems;
    if (!customMenuItems.empty())
    {
        gDropdownItemsFormat[i++] = STR_EMPTY;
        for (const auto& item : customMenuItems)
        {
            gDropdownItemsFormat[i] = STR_STRING;
            auto sz = item.Text.c_str();
            std::memcpy(&gDropdownItemsArgs[i], &sz, sizeof(const char*));
            i++;
        }
    }
#endif

    window_dropdown_show_text(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1] | 0x80, 0, i);
    gDropdownDefaultIndex = DDIDX_SHOW_MAP;
}

static void top_toolbar_map_menu_dropdown(int16_t dropdownIndex)
{
    int32_t customStartIndex = 3;
    if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR)
    {
        customStartIndex++;
    }

    if (dropdownIndex < customStartIndex)
    {
        switch (dropdownIndex)
        {
            case 0:
                context_open_window(WC_MAP);
                break;
            case 1:
                context_open_window(WC_VIEWPORT);
                break;
            case 2:
                context_open_window(WC_MAPGEN);
                break;
        }
    }
    else
    {
#ifdef ENABLE_SCRIPTING
        const auto& customMenuItems = OpenRCT2::Scripting::CustomMenuItems;
        auto customIndex = static_cast<size_t>(dropdownIndex - customStartIndex);
        if (customMenuItems.size() > customIndex)
        {
            customMenuItems[customIndex].Invoke();
        }
#endif
    }
}

static void top_toolbar_init_fastforward_menu(rct_window* w, rct_widget* widget)
{
    int32_t num_items = 4;
    gDropdownItemsFormat[0] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[1] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[2] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[3] = STR_TOGGLE_OPTION;
    if (gConfigGeneral.debugging_tools)
    {
        gDropdownItemsFormat[4] = STR_EMPTY;
        gDropdownItemsFormat[5] = STR_TOGGLE_OPTION;
        gDropdownItemsArgs[5] = STR_SPEED_HYPER;
        num_items = 6;
    }

    gDropdownItemsArgs[0] = STR_SPEED_NORMAL;
    gDropdownItemsArgs[1] = STR_SPEED_QUICK;
    gDropdownItemsArgs[2] = STR_SPEED_FAST;
    gDropdownItemsArgs[3] = STR_SPEED_TURBO;

    window_dropdown_show_text(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[0] | 0x80, 0,
        num_items);

    // Set checkmarks
    if (gGameSpeed <= 4)
    {
        dropdown_set_checked(gGameSpeed - 1, true);
    }
    if (gGameSpeed == 8)
    {
        dropdown_set_checked(5, true);
    }

    if (gConfigGeneral.debugging_tools)
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

static void top_toolbar_fastforward_menu_dropdown(int16_t dropdownIndex)
{
    rct_window* w = window_get_main();
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

static void top_toolbar_init_rotate_menu(rct_window* w, rct_widget* widget)
{
    gDropdownItemsFormat[0] = STR_ROTATE_CLOCKWISE;
    gDropdownItemsFormat[1] = STR_ROTATE_ANTI_CLOCKWISE;

    window_dropdown_show_text(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1] | 0x80, 0, 2);

    gDropdownDefaultIndex = DDIDX_ROTATE_CLOCKWISE;
}

static void top_toolbar_rotate_menu_dropdown(int16_t dropdownIndex)
{
    rct_window* w = window_get_main();
    if (w)
    {
        if (dropdownIndex == 0)
        {
            window_rotate_camera(w, 1);
            w->Invalidate();
        }
        else if (dropdownIndex == 1)
        {
            window_rotate_camera(w, -1);
            w->Invalidate();
        }
    }
}

static void top_toolbar_init_cheats_menu(rct_window* w, rct_widget* widget)
{
    gDropdownItemsFormat[DDIDX_CHEATS] = STR_TOGGLE_OPTION;
    gDropdownItemsArgs[DDIDX_CHEATS] = STR_CHEAT_TITLE;

    gDropdownItemsFormat[DDIDX_TILE_INSPECTOR] = STR_TOGGLE_OPTION;
    gDropdownItemsArgs[DDIDX_TILE_INSPECTOR] = STR_DEBUG_DROPDOWN_TILE_INSPECTOR;

    gDropdownItemsFormat[DDIDX_OBJECT_SELECTION] = STR_TOGGLE_OPTION;
    gDropdownItemsArgs[DDIDX_OBJECT_SELECTION] = STR_DEBUG_DROPDOWN_OBJECT_SELECTION;

    gDropdownItemsFormat[DDIDX_INVENTIONS_LIST] = STR_TOGGLE_OPTION;
    gDropdownItemsArgs[DDIDX_INVENTIONS_LIST] = STR_DEBUG_DROPDOWN_INVENTIONS_LIST;

    gDropdownItemsFormat[DDIDX_SCENARIO_OPTIONS] = STR_TOGGLE_OPTION;
    gDropdownItemsArgs[DDIDX_SCENARIO_OPTIONS] = STR_DEBUG_DROPDOWN_SCENARIO_OPTIONS;

    gDropdownItemsFormat[5] = STR_EMPTY;

    gDropdownItemsFormat[DDIDX_ENABLE_SANDBOX_MODE] = STR_TOGGLE_OPTION;
    gDropdownItemsArgs[DDIDX_ENABLE_SANDBOX_MODE] = STR_ENABLE_SANDBOX_MODE;

    gDropdownItemsFormat[DDIDX_DISABLE_CLEARANCE_CHECKS] = STR_TOGGLE_OPTION;
    gDropdownItemsArgs[DDIDX_DISABLE_CLEARANCE_CHECKS] = STR_DISABLE_CLEARANCE_CHECKS;

    gDropdownItemsFormat[DDIDX_DISABLE_SUPPORT_LIMITS] = STR_TOGGLE_OPTION;
    gDropdownItemsArgs[DDIDX_DISABLE_SUPPORT_LIMITS] = STR_DISABLE_SUPPORT_LIMITS;

    window_dropdown_show_text(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[0] | 0x80, 0,
        TOP_TOOLBAR_CHEATS_COUNT);

    // Disable items that are not yet available in multiplayer
    if (network_get_mode() != NETWORK_MODE_NONE)
    {
        dropdown_set_disabled(DDIDX_OBJECT_SELECTION, true);
        dropdown_set_disabled(DDIDX_INVENTIONS_LIST, true);
    }

    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
    {
        dropdown_set_disabled(DDIDX_OBJECT_SELECTION, true);
        dropdown_set_disabled(DDIDX_INVENTIONS_LIST, true);
        dropdown_set_disabled(DDIDX_SCENARIO_OPTIONS, true);
        dropdown_set_disabled(DDIDX_ENABLE_SANDBOX_MODE, true);
    }

    if (gCheatsSandboxMode)
    {
        dropdown_set_checked(DDIDX_ENABLE_SANDBOX_MODE, true);
    }
    if (gCheatsDisableClearanceChecks)
    {
        dropdown_set_checked(DDIDX_DISABLE_CLEARANCE_CHECKS, true);
    }
    if (gCheatsDisableSupportLimits)
    {
        dropdown_set_checked(DDIDX_DISABLE_SUPPORT_LIMITS, true);
    }

    gDropdownDefaultIndex = DDIDX_CHEATS;
}

static void top_toolbar_cheats_menu_dropdown(int16_t dropdownIndex)
{
    switch (dropdownIndex)
    {
        case DDIDX_CHEATS:
            context_open_window(WC_CHEATS);
            break;
        case DDIDX_TILE_INSPECTOR:
            context_open_window(WC_TILE_INSPECTOR);
            break;
        case DDIDX_OBJECT_SELECTION:
            window_close_all();
            context_open_window(WC_EDITOR_OBJECT_SELECTION);
            break;
        case DDIDX_INVENTIONS_LIST:
            context_open_window(WC_EDITOR_INVENTION_LIST);
            break;
        case DDIDX_SCENARIO_OPTIONS:
            context_open_window(WC_EDITOR_SCENARIO_OPTIONS);
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

static void top_toolbar_init_debug_menu(rct_window* w, rct_widget* widget)
{
    gDropdownItemsFormat[DDIDX_CONSOLE] = STR_TOGGLE_OPTION;
    gDropdownItemsArgs[DDIDX_CONSOLE] = STR_DEBUG_DROPDOWN_CONSOLE;
    gDropdownItemsFormat[DDIDX_DEBUG_PAINT] = STR_TOGGLE_OPTION;
    gDropdownItemsArgs[DDIDX_DEBUG_PAINT] = STR_DEBUG_DROPDOWN_DEBUG_PAINT;

    window_dropdown_show_text(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[0] | 0x80,
        DROPDOWN_FLAG_STAY_OPEN, TOP_TOOLBAR_DEBUG_COUNT);

    dropdown_set_checked(DDIDX_DEBUG_PAINT, window_find_by_class(WC_DEBUG_PAINT) != nullptr);
}

static void top_toolbar_init_network_menu(rct_window* w, rct_widget* widget)
{
    gDropdownItemsFormat[DDIDX_MULTIPLAYER] = STR_MULTIPLAYER;
    gDropdownItemsFormat[DDIDX_NETWORK] = STR_NETWORK;
    gDropdownItemsFormat[DDIDX_MULTIPLAYER_RECONNECT] = STR_MULTIPLAYER_RECONNECT;

    window_dropdown_show_text(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[0] | 0x80, 0,
        TOP_TOOLBAR_NETWORK_COUNT);

    dropdown_set_disabled(DDIDX_MULTIPLAYER_RECONNECT, !network_is_desynchronised());

    gDropdownDefaultIndex = DDIDX_MULTIPLAYER;
}

static void top_toolbar_debug_menu_dropdown(int16_t dropdownIndex)
{
    rct_window* w = window_get_main();
    if (w)
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
                if (window_find_by_class(WC_DEBUG_PAINT) == nullptr)
                {
                    context_open_window(WC_DEBUG_PAINT);
                }
                else
                {
                    window_close_by_class(WC_DEBUG_PAINT);
                }
                break;
        }
    }
}

static void top_toolbar_network_menu_dropdown(int16_t dropdownIndex)
{
    rct_window* w = window_get_main();
    if (w)
    {
        switch (dropdownIndex)
        {
            case DDIDX_MULTIPLAYER:
                context_open_window(WC_MULTIPLAYER);
                break;
            case DDIDX_NETWORK:
                context_open_window(WC_NETWORK);
                break;
            case DDIDX_MULTIPLAYER_RECONNECT:
                network_reconnect();
                break;
        }
    }
}

/**
 *
 *  rct2: 0x0066CDE4
 */
static void top_toolbar_init_view_menu(rct_window* w, rct_widget* widget)
{
    gDropdownItemsFormat[0] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[1] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[2] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[3] = STR_EMPTY;
    gDropdownItemsFormat[4] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[5] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[6] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[7] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[8] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[9] = STR_EMPTY;
    gDropdownItemsFormat[10] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[11] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[12] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[13] = DROPDOWN_SEPARATOR;
    gDropdownItemsFormat[DDIDX_VIEW_CLIPPING] = STR_TOGGLE_OPTION;
    gDropdownItemsFormat[DDIDX_HIGHLIGHT_PATH_ISSUES] = STR_TOGGLE_OPTION;

    gDropdownItemsArgs[0] = STR_UNDERGROUND_VIEW;
    gDropdownItemsArgs[1] = STR_REMOVE_BASE_LAND;
    gDropdownItemsArgs[2] = STR_REMOVE_VERTICAL_FACES;
    gDropdownItemsArgs[4] = STR_SEE_THROUGH_RIDES;
    gDropdownItemsArgs[5] = STR_SEE_THROUGH_SCENERY;
    gDropdownItemsArgs[6] = STR_SEE_THROUGH_PATHS;
    gDropdownItemsArgs[7] = STR_INVISIBLE_SUPPORTS;
    gDropdownItemsArgs[8] = STR_INVISIBLE_PEOPLE;
    gDropdownItemsArgs[10] = STR_HEIGHT_MARKS_ON_LAND;
    gDropdownItemsArgs[11] = STR_HEIGHT_MARKS_ON_RIDE_TRACKS;
    gDropdownItemsArgs[12] = STR_HEIGHT_MARKS_ON_PATHS;
    gDropdownItemsArgs[DDIDX_VIEW_CLIPPING] = STR_VIEW_CLIPPING_MENU;
    gDropdownItemsArgs[DDIDX_HIGHLIGHT_PATH_ISSUES] = STR_HIGHLIGHT_PATH_ISSUES_MENU;

    window_dropdown_show_text(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1] | 0x80, 0,
        TOP_TOOLBAR_VIEW_MENU_COUNT);

    // Set checkmarks
    rct_viewport* mainViewport = window_get_main()->viewport;
    if (mainViewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
        dropdown_set_checked(0, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_BASE)
        dropdown_set_checked(1, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIDE_VERTICAL)
        dropdown_set_checked(2, true);
    if (mainViewport->flags & VIEWPORT_FLAG_SEETHROUGH_RIDES)
        dropdown_set_checked(4, true);
    if (mainViewport->flags & VIEWPORT_FLAG_SEETHROUGH_SCENERY)
        dropdown_set_checked(5, true);
    if (mainViewport->flags & VIEWPORT_FLAG_SEETHROUGH_PATHS)
        dropdown_set_checked(6, true);
    if (mainViewport->flags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        dropdown_set_checked(7, true);
    if (mainViewport->flags & VIEWPORT_FLAG_INVISIBLE_PEEPS)
        dropdown_set_checked(8, true);
    if (mainViewport->flags & VIEWPORT_FLAG_LAND_HEIGHTS)
        dropdown_set_checked(10, true);
    if (mainViewport->flags & VIEWPORT_FLAG_TRACK_HEIGHTS)
        dropdown_set_checked(11, true);
    if (mainViewport->flags & VIEWPORT_FLAG_PATH_HEIGHTS)
        dropdown_set_checked(12, true);
    if (mainViewport->flags & VIEWPORT_FLAG_CLIP_VIEW)
        dropdown_set_checked(DDIDX_VIEW_CLIPPING, true);
    if (mainViewport->flags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
        dropdown_set_checked(DDIDX_HIGHLIGHT_PATH_ISSUES, true);

    gDropdownDefaultIndex = DDIDX_UNDERGROUND_INSIDE;
}

/**
 *
 *  rct2: 0x0066CF8A
 */
static void top_toolbar_view_menu_dropdown(int16_t dropdownIndex)
{
    rct_window* w = window_get_main();
    if (w)
    {
        switch (dropdownIndex)
        {
            case DDIDX_UNDERGROUND_INSIDE:
                w->viewport->flags ^= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
                break;
            case DDIDX_HIDE_BASE:
                w->viewport->flags ^= VIEWPORT_FLAG_HIDE_BASE;
                break;
            case DDIDX_HIDE_VERTICAL:
                w->viewport->flags ^= VIEWPORT_FLAG_HIDE_VERTICAL;
                break;
            case DDIDX_SEETHROUGH_RIDES:
                w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_RIDES;
                break;
            case DDIDX_SEETHROUGH_SCENARY:
                w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_SCENERY;
                break;
            case DDIDX_SEETHROUGH_PATHS:
                w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_PATHS;
                break;
            case DDIDX_INVISIBLE_SUPPORTS:
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_SUPPORTS;
                break;
            case DDIDX_INVISIBLE_PEEPS:
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_PEEPS;
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
                if (window_find_by_class(WC_VIEW_CLIPPING) == nullptr)
                {
                    context_open_window(WC_VIEW_CLIPPING);
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
static void toggle_footpath_window()
{
    if (window_find_by_class(WC_FOOTPATH) == nullptr)
    {
        context_open_window(WC_FOOTPATH);
    }
    else
    {
        tool_cancel();
        window_close_by_class(WC_FOOTPATH);
    }
}

/**
 *
 *  rct2: 0x0066CD54
 */
static void toggle_land_window(rct_window* topToolbar, rct_widgetindex widgetIndex)
{
    if ((input_test_flag(INPUT_FLAG_TOOL_ACTIVE)) && gCurrentToolWidget.window_classification == WC_TOP_TOOLBAR
        && gCurrentToolWidget.widget_index == WIDX_LAND)
    {
        tool_cancel();
    }
    else
    {
        _landToolBlocked = false;
        show_gridlines();
        tool_set(topToolbar, widgetIndex, TOOL_DIG_DOWN);
        input_set_flag(INPUT_FLAG_6, true);
        context_open_window(WC_LAND);
    }
}

/**
 *
 *  rct2: 0x0066CD0C
 */
static void toggle_clear_scenery_window(rct_window* topToolbar, rct_widgetindex widgetIndex)
{
    if ((input_test_flag(INPUT_FLAG_TOOL_ACTIVE) && gCurrentToolWidget.window_classification == WC_TOP_TOOLBAR
         && gCurrentToolWidget.widget_index == WIDX_CLEAR_SCENERY))
    {
        tool_cancel();
    }
    else
    {
        show_gridlines();
        tool_set(topToolbar, widgetIndex, TOOL_CROSSHAIR);
        input_set_flag(INPUT_FLAG_6, true);
        context_open_window(WC_CLEAR_SCENERY);
    }
}

/**
 *
 *  rct2: 0x0066CD9C
 */
static void toggle_water_window(rct_window* topToolbar, rct_widgetindex widgetIndex)
{
    if ((input_test_flag(INPUT_FLAG_TOOL_ACTIVE)) && gCurrentToolWidget.window_classification == WC_TOP_TOOLBAR
        && gCurrentToolWidget.widget_index == WIDX_WATER)
    {
        tool_cancel();
    }
    else
    {
        _landToolBlocked = false;
        show_gridlines();
        tool_set(topToolbar, widgetIndex, TOOL_WATER_DOWN);
        input_set_flag(INPUT_FLAG_6, true);
        context_open_window(WC_WATER);
    }
}

/**
 *
 *  rct2: 0x0066D104
 */
bool land_tool_is_active()
{
    if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
        return false;
    if (gCurrentToolWidget.window_classification != WC_TOP_TOOLBAR)
        return false;
    if (gCurrentToolWidget.widget_index != WIDX_LAND)
        return false;
    return true;
}

/**
 *
 *  rct2: 0x0066D125
 */
bool clear_scenery_tool_is_active()
{
    if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
        return false;
    if (gCurrentToolWidget.window_classification != WC_TOP_TOOLBAR)
        return false;
    if (gCurrentToolWidget.widget_index != WIDX_CLEAR_SCENERY)
        return false;
    return true;
}

/**
 *
 *  rct2: 0x0066D125
 */
bool water_tool_is_active()
{
    if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
        return false;
    if (gCurrentToolWidget.window_classification != WC_TOP_TOOLBAR)
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
