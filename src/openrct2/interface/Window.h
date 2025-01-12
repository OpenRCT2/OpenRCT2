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
#include "../drawing/ImageId.hpp"
#include "../localisation/Formatter.h"
#include "../ride/RideTypes.h"
#include "../windows/TileInspectorGlobals.h"
#include "../world/Location.hpp"
#include "../world/ScenerySelection.h"
#include "Colour.h"
#include "Widget.h"
#include "WindowClasses.h"
#include "ZoomLevel.h"

#include <functional>
#include <limits>
#include <list>
#include <memory>
#include <utility>
#include <variant>

struct DrawPixelInfo;
struct WindowBase;
struct TrackDesignFileRef;
struct ScenarioIndexEntry;
struct WindowCloseModifier;

enum class VisibilityCache : uint8_t;
enum class CursorID : uint8_t;
enum class CloseWindowModifier : uint8_t;

using rct_windownumber = uint16_t;

namespace OpenRCT2
{
    enum class RideConstructionState : uint8_t;
}

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

using WidgetFlags = uint32_t;
namespace OpenRCT2::WIDGET_FLAGS
{
    const WidgetFlags TEXT_IS_STRING = 1 << 0;
    const WidgetFlags IS_PRESSED = 1 << 2;
    const WidgetFlags IS_DISABLED = 1 << 3;
    const WidgetFlags TOOLTIP_IS_STRING = 1 << 4;
    const WidgetFlags IS_HIDDEN = 1 << 5;
    const WidgetFlags IS_HOLDABLE = 1 << 6;
} // namespace OpenRCT2::WIDGET_FLAGS

enum class WindowWidgetType : uint8_t;

struct Widget
{
    WindowWidgetType type;
    uint8_t colour;
    int16_t left;
    int16_t right;
    int16_t top;
    int16_t bottom;
    union
    {
        uint32_t content;
        ImageId image;
        StringId text;
        utf8* string;
    };
    StringId tooltip;

    // New properties
    WidgetFlags flags{};
    utf8* sztooltip{};

    int16_t width() const
    {
        return right - left;
    }

    int16_t height() const
    {
        return bottom - top;
    }

    int16_t midX() const
    {
        return (left + right) / 2;
    }

    int16_t midY() const
    {
        return (top + bottom) / 2;
    }

    int16_t textTop() const
    {
        if (height() >= 10)
            return std::max<int32_t>(top, top + (height() / 2) - 5);

        return top - 1;
    }

    void moveRight(int32_t amount)
    {
        left += amount;
        right += amount;
    }

    void moveDown(int32_t amount)
    {
        top += amount;
        bottom += amount;
    }

    void moveTo(ScreenCoordsXY coords)
    {
        moveRight(coords.x - left);
        moveDown(coords.y - top);
    }

    void moveToX(int16_t x)
    {
        moveRight(x - left);
    }

    void moveToY(int16_t y)
    {
        moveDown(y - top);
    }

    bool IsVisible() const
    {
        return !(flags & OpenRCT2::WIDGET_FLAGS::IS_HIDDEN);
    }
};

/**
 * Viewport structure
 */
struct Viewport
{
    int32_t width{};
    int32_t height{};
    ScreenCoordsXY pos{};
    ScreenCoordsXY viewPos{};
    uint32_t flags{};
    ZoomLevel zoom{};
    uint8_t rotation{};
    VisibilityCache visibility{};

    [[nodiscard]] constexpr int32_t ViewWidth() const
    {
        return zoom.ApplyTo(width);
    }

    [[nodiscard]] constexpr int32_t ViewHeight() const
    {
        return zoom.ApplyTo(height);
    }

    // Use this function on coordinates that are relative to the viewport zoom i.e. a peeps x, y position after transforming
    // from its x, y, z
    [[nodiscard]] constexpr bool Contains(const ScreenCoordsXY& vpos) const
    {
        return (
            vpos.y >= viewPos.y && vpos.y < viewPos.y + ViewHeight() && vpos.x >= viewPos.x
            && vpos.x < viewPos.x + ViewWidth());
    }

    // Use this function on coordinates that are relative to the screen that is been drawn i.e. the cursor position
    [[nodiscard]] constexpr bool ContainsScreen(const ScreenCoordsXY& sPos) const
    {
        return (sPos.x >= pos.x && sPos.x < pos.x + width && sPos.y >= pos.y && sPos.y < pos.y + height);
    }

    [[nodiscard]] ScreenCoordsXY ScreenToViewportCoord(const ScreenCoordsXY& screenCoord) const;

    void Invalidate() const;
};

struct Focus
{
    using CoordinateFocus = CoordsXYZ;
    using EntityFocus = EntityId;

    ZoomLevel zoom{};
    std::variant<CoordinateFocus, EntityFocus> data;

    template<typename T>
    constexpr explicit Focus(T newValue, ZoomLevel newZoom = {})
    {
        data = newValue;
        zoom = newZoom;
    }

    CoordsXYZ GetPos() const;

    constexpr bool operator==(const Focus& other) const
    {
        if (zoom != other.zoom)
        {
            return false;
        }
        return data == other.data;
    }
    constexpr bool operator!=(const Focus& other) const
    {
        return !(*this == other);
    }
};

struct WindowCloseModifier
{
    WindowIdentifier window;
    CloseWindowModifier modifier;
};

struct WindowBase;

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

#define validate_global_widx(wc, widx)                                                                                         \
    static_assert(widx == wc##__##widx, "Global WIDX of " #widx " doesn't match actual value.")

constexpr int32_t WC_MAIN_WINDOW__0 = 0;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT = 25;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE = 30;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_EXIT = 31;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_ROTATE = 32;
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

WindowBase* WindowBringToFront(WindowBase& w);
WindowBase* WindowBringToFrontByClass(WindowClass cls);
WindowBase* WindowBringToFrontByClassWithFlags(WindowClass cls, uint16_t flags);
WindowBase* WindowBringToFrontByNumber(WindowClass cls, rct_windownumber number);

void WindowClose(WindowBase& window);
void WindowCloseByClass(WindowClass cls);
void WindowCloseByNumber(WindowClass cls, rct_windownumber number);
void WindowCloseByNumber(WindowClass cls, EntityId number);
void WindowCloseTop();
void WindowCloseAll();
void WindowCloseAllExceptClass(WindowClass cls);
void WindowCloseAllExceptFlags(uint16_t flags);
void WindowCloseAllExceptNumberAndClass(rct_windownumber number, WindowClass cls);
void WindowInvalidateByClass(WindowClass cls);
void WindowInvalidateByNumber(WindowClass cls, rct_windownumber number);
void WindowInvalidateByNumber(WindowClass cls, EntityId id);
void WindowInvalidateAll();
void WidgetInvalidate(WindowBase& w, WidgetIndex widgetIndex);
void WidgetInvalidateByClass(WindowClass cls, WidgetIndex widgetIndex);
void WidgetInvalidateByNumber(WindowClass cls, rct_windownumber number, WidgetIndex widgetIndex);

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

void WindowCloseConstructionWindows();

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
