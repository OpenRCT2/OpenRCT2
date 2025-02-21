/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../core/EnumUtils.hpp"
#include "../windows/TileInspectorGlobals.h"
#include "../world/Location.hpp"
#include "Colour.h"
#include "Widget.h"
#include "WindowClasses.h"
#include "ZoomLevel.h"

#include <functional>
#include <list>
#include <memory>
#include <variant>

struct DrawPixelInfo;
struct TrackDesignFileRef;
struct ScenarioIndexEntry;

enum class VisibilityCache : uint8_t;
enum class CursorID : uint8_t;
enum class CloseWindowModifier : uint8_t;

namespace OpenRCT2
{
    using rct_windownumber = int16_t;

    struct WindowBase;
    struct WindowCloseModifier;

    enum class RideConstructionState : uint8_t;

    struct WindowIdentifier
    {
        WindowClass classification;
        rct_windownumber number;
    };

    struct WidgetIdentifier
    {
        WindowIdentifier window;
        WidgetIndex widget_index;
    };

    extern WindowCloseModifier gLastCloseModifier;

    struct WindowCloseModifier
    {
        WindowIdentifier window;
        CloseWindowModifier modifier;
    };

    struct Viewport;

#define RCT_WINDOW_RIGHT(w) ((w)->windowPos.x + (w)->width)
#define RCT_WINDOW_BOTTOM(w) ((w)->windowPos.y + (w)->height)

    enum WINDOW_FLAGS
    {
        /*
        WF_TIMEOUT_SHL = 0,
        WF_TIMEOUT_MASK = 7,
        WF_DRAGGING = 1 << 3,
        WF_SCROLLER_UP = 1 << 4,
        WF_SCROLLER_DOWN = 1 << 5,
        WF_SCROLLER_MIDDLE = 1 << 6,
        WF_DISABLE_VP_SCROLL = 1 << 9,
        */

        WF_STICK_TO_BACK = (1 << 0),
        WF_STICK_TO_FRONT = (1 << 1),
        WF_NO_SCROLLING = (1 << 2), // User is unable to scroll this viewport
        WF_SCROLLING_TO_LOCATION = (1 << 3),
        WF_TRANSPARENT = (1 << 4),
        WF_NO_BACKGROUND = (1 << 5), // Instead of half transparency, completely remove the window background
        WF_DEAD = (1u << 6),         // Window is closed and will be deleted in the next update.
        WF_7 = (1 << 7),
        WF_RESIZABLE = (1 << 8),
        WF_NO_AUTO_CLOSE = (1 << 9), // Don't auto close this window if too many windows are open
        WF_10 = (1 << 10),
        WF_WHITE_BORDER_ONE = (1 << 12),
        WF_WHITE_BORDER_MASK = (1 << 12) | (1 << 13),

        WF_NO_SNAPPING = (1 << 15),

        // Create only flags
        WF_AUTO_POSITION = (1 << 16),
        WF_CENTRE_SCREEN = (1 << 17),
    };

    enum
    {
        WV_PARK_AWARDS,
        WV_PARK_RATING,
        WV_PARK_OBJECTIVE,
        WV_PARK_GUESTS,
        WV_FINANCES_RESEARCH,
        WV_RIDE_RESEARCH,
        WV_MAZE_CONSTRUCTION,
        WV_NETWORK_PASSWORD,
        WV_EDITOR_BOTTOM_TOOLBAR,
        WV_CHANGELOG,
        WV_NEW_VERSION_INFO,
        WV_FINANCE_MARKETING,
        WV_CONTRIBUTORS,
    };

    enum WindowDetail
    {
        WD_BANNER,
        WD_NEW_CAMPAIGN,
        WD_DEMOLISH_RIDE,
        WD_REFURBISH_RIDE,
        WD_SIGN,
        WD_SIGN_SMALL,

        WD_PLAYER,

        WD_VEHICLE,
        WD_TRACK,

        WD_NULL = 255,
    };
} // namespace OpenRCT2

#define validate_global_widx(wc, widx)                                                                                         \
    static_assert(widx == wc##__##widx, "Global WIDX of " #widx " doesn't match actual value.")

constexpr int32_t WC_MAIN_WINDOW__0 = 0;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT = 27;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE = 32;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_EXIT = 33;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_ROTATE = 34;
constexpr int32_t WC_MAZE_CONSTRUCTION__WIDX_MAZE_DIRECTION_GROUPBOX = WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT;
constexpr int32_t WC_MAZE_CONSTRUCTION__WIDX_MAZE_ENTRANCE = WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE;
constexpr int32_t WC_MAZE_CONSTRUCTION__WIDX_MAZE_EXIT = WC_RIDE_CONSTRUCTION__WIDX_EXIT;
constexpr int32_t WC_SCENERY__WIDX_SCENERY_BACKGROUND = 0;
constexpr int32_t WC_SCENERY__WIDX_SCENERY_TAB_1 = 15;
constexpr int32_t WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON = 5;
constexpr int32_t WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON = 10;
constexpr int32_t WC_PEEP__WIDX_PATROL = 10;
constexpr int32_t WC_PEEP__WIDX_ACTION_LBL = 13;
constexpr int32_t WC_PEEP__WIDX_PICKUP = 14;
constexpr int32_t WC_TRACK_DESIGN_LIST__WIDX_ROTATE = 8;
constexpr int32_t WC_TRACK_DESIGN_PLACE__WIDX_ROTATE = 3;
constexpr int32_t WC_EDITOR_PARK_ENTRANCE__WIDX_ROTATE_ENTRANCE_BUTTON = 6;
constexpr int32_t WC_EDITOR_OBJECT_SELECTION__WIDX_TAB_1 = 22;
constexpr int32_t WC_STAFF__WIDX_PICKUP = 9;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE = 15;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_COPY = 18;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE = 17;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_SORT = 16;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE = 12;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP = 13;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN = 14;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE = 6;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE = 7;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE = 10;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE = 11;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SURFACE = EnumValue(TileInspectorPage::Surface);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_PATH = EnumValue(TileInspectorPage::Path);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_TRACK = EnumValue(TileInspectorPage::Track);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_INCREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_DECREASE = 31;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SCENERY = EnumValue(TileInspectorPage::Scenery);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_ENTRANCE = EnumValue(TileInspectorPage::Entrance);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_WALL = EnumValue(TileInspectorPage::Wall);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_LARGE_SCENERY = EnumValue(TileInspectorPage::LargeScenery);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_BANNER = EnumValue(TileInspectorPage::Banner);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_DECREASE = 30;

enum class PromptMode : uint8_t
{
    SaveBeforeLoad = 0,
    SaveBeforeQuit,
    SaveBeforeQuit2,
    SaveBeforeNewGame,
    Quit
};

enum BTM_TOOLBAR_DIRTY_FLAGS
{
    BTM_TB_DIRTY_FLAG_MONEY = (1 << 0),
    BTM_TB_DIRTY_FLAG_DATE = (1 << 1),
    BTM_TB_DIRTY_FLAG_PEEP_COUNT = (1 << 2),
    BTM_TB_DIRTY_FLAG_CLIMATE = (1 << 3),
    BTM_TB_DIRTY_FLAG_PARK_RATING = (1 << 4)
};

// 000N_TTTL
enum
{
    LOADSAVETYPE_LOAD = 0 << 0,
    LOADSAVETYPE_SAVE = 1 << 0,

    LOADSAVETYPE_GAME = 0 << 1,
    LOADSAVETYPE_LANDSCAPE = 1 << 1,
    LOADSAVETYPE_SCENARIO = 2 << 1,
    LOADSAVETYPE_TRACK = 3 << 1,
    LOADSAVETYPE_HEIGHTMAP = 4 << 1,
};

enum
{
    MODAL_RESULT_FAIL = -1,
    MODAL_RESULT_CANCEL,
    MODAL_RESULT_OK
};

enum class VisibilityCache : uint8_t
{
    Unknown,
    Visible,
    Covered
};

enum class CloseWindowModifier : uint8_t
{
    None,
    Shift,
    Control
};

enum class GuestListFilterType : int32_t
{
    GuestsOnRide,
    GuestsInQueue,
    GuestsThinkingAboutRide,
    GuestsThinkingX,
};

enum class Tool
{
    Arrow = 0,
    UpArrow = 2,
    UpDownArrow = 3,
    Picker = 7,
    Crosshair = 12,
    PathDown = 17,
    DigDown = 18,
    WaterDown = 19,
    WalkDown = 22,
    PaintDown = 23,
    EntranceDown = 24,
    Bulldozer = 27,
};

namespace OpenRCT2
{
    struct WidgetRef
    {
        WindowClass window_classification;
        rct_windownumber window_number;
        WidgetIndex widget_index;
    };

    extern Tool gCurrentToolId;
    extern WidgetRef gCurrentToolWidget;

    using modal_callback = void (*)(int32_t result);
    using CloseCallback = void (*)();

    constexpr int8_t kWindowLimitMin = 4;
    constexpr int8_t kWindowLimitMax = 64;
    constexpr int8_t kWindowLimitReserved = 4; // Used to reserve room for the main viewport, toolbars, etc.

    extern WindowBase* gWindowAudioExclusive;

    extern uint32_t gWindowUpdateTicks;

    extern colour_t gCurrentWindowColours[3];

    std::list<std::shared_ptr<WindowBase>>::iterator WindowGetIterator(const WindowBase* w);
    void WindowVisitEach(std::function<void(WindowBase*)> func);

    void WindowSetFlagForAllViewports(uint32_t viewportFlag, bool enabled);

    void WindowDispatchUpdateAll();
    void WindowUpdateAllViewports();
    void WindowUpdateAll();
    void WindowNotifyLanguageChange();

    void WindowSetWindowLimit(int32_t value);

    int32_t WindowGetScrollDataIndex(const WindowBase& w, WidgetIndex widget_index);

    void WindowPushOthersRight(WindowBase& w);
    void WindowPushOthersBelow(WindowBase& w1);

    WindowBase* WindowGetMain();

    void WindowScrollToLocation(WindowBase& w, const CoordsXYZ& coords);
    void WindowViewportGetMapCoordsByCursor(
        const WindowBase& w, int32_t* map_x, int32_t* map_y, int32_t* offset_x, int32_t* offset_y);
    void WindowViewportCentreTileAroundCursor(WindowBase& w, int32_t map_x, int32_t map_y, int32_t offset_x, int32_t offset_y);
    void WindowCheckAllValidZoom();
    void WindowZoomSet(WindowBase& w, ZoomLevel zoomLevel, bool atCursor);

    void WindowDrawAll(DrawPixelInfo& dpi, int32_t left, int32_t top, int32_t right, int32_t bottom);
    void WindowDraw(DrawPixelInfo& dpi, WindowBase& w, int32_t left, int32_t top, int32_t right, int32_t bottom);

    bool isToolActive(WindowClass cls);
    bool isToolActive(WindowClass cls, rct_windownumber number);
    bool isToolActive(WindowClass cls, WidgetIndex widgetIndex);
    bool isToolActive(WindowClass cls, WidgetIndex widgetIndex, rct_windownumber number);
    bool isToolActive(const WindowBase& w, WidgetIndex widgetIndex);
    bool ToolSet(const WindowBase& w, WidgetIndex widgetIndex, Tool tool);
    void ToolCancel();

    void WindowUpdateViewportRideMusic();

    Viewport* WindowGetViewport(WindowBase* window);

    // Open window functions
    void WindowResizeGui(int32_t width, int32_t height);
    void WindowResizeGuiScenarioEditor(int32_t width, int32_t height);

    void TextinputCancel();

    bool WindowIsVisible(WindowBase& w);

    Viewport* WindowGetPreviousViewport(Viewport* current);
    void WindowResetVisibilities();
    void WindowInitAll();

    void WindowFollowSprite(WindowBase& w, EntityId spriteIndex);
    void WindowUnfollowSprite(WindowBase& w);

} // namespace OpenRCT2
