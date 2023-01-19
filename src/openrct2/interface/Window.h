/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../common.h"
#include "../localisation/Formatter.h"
#include "../ride/RideTypes.h"
#include "../world/Location.hpp"
#include "../world/ScenerySelection.h"
#include "WindowClasses.h"
#include "ZoomLevel.h"

#include <functional>
#include <limits>
#include <list>
#include <memory>
#include <utility>
#include <variant>

struct DrawPixelInfo;
struct rct_window;
struct TrackDesignFileRef;
struct TextInputSession;
struct scenario_index_entry;
struct window_close_modifier;

enum class VisibilityCache : uint8_t;
enum class CursorID : uint8_t;
enum class RideConstructionState : uint8_t;
enum class CloseWindowModifier : uint8_t;

#define SCROLLABLE_ROW_HEIGHT 12
#define LIST_ROW_HEIGHT 12
#define TABLE_CELL_HEIGHT 12
#define BUTTON_FACE_HEIGHT 12

#define TEXT_INPUT_SIZE 1024
#define TOP_TOOLBAR_HEIGHT 27

extern char gTextBoxInput[TEXT_INPUT_SIZE];
extern int32_t gTextBoxFrameNo;
extern bool gUsingWidgetTextBox;
extern struct TextInputSession* gTextInput;

using rct_windownumber = uint16_t;
using WidgetIndex = int16_t;

struct window_identifier
{
    WindowClass classification;
    rct_windownumber number;
};

struct widget_identifier
{
    window_identifier window;
    WidgetIndex widget_index;
};

extern widget_identifier gCurrentTextBox;
extern window_close_modifier gLastCloseModifier;

using WidgetFlags = uint32_t;
namespace WIDGET_FLAGS
{
    const WidgetFlags TEXT_IS_STRING = 1 << 0;
    const WidgetFlags IS_PRESSED = 1 << 2;
    const WidgetFlags IS_DISABLED = 1 << 3;
    const WidgetFlags TOOLTIP_IS_STRING = 1 << 4;
    const WidgetFlags IS_HIDDEN = 1 << 5;
    const WidgetFlags IS_HOLDABLE = 1 << 6;
} // namespace WIDGET_FLAGS

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

    bool IsVisible() const
    {
        return !(flags & WIDGET_FLAGS::IS_HIDDEN);
    }
};

/**
 * Viewport structure
 */
struct rct_viewport
{
    int32_t width;
    int32_t height;
    ScreenCoordsXY pos;
    ScreenCoordsXY viewPos;
    int32_t view_width;
    int32_t view_height;
    uint32_t flags;
    ZoomLevel zoom;
    uint8_t var_11;
    VisibilityCache visibility;

    // Use this function on coordinates that are relative to the viewport zoom i.e. a peeps x, y position after transforming
    // from its x, y, z
    [[nodiscard]] constexpr bool Contains(const ScreenCoordsXY& vpos) const
    {
        return (
            vpos.y >= viewPos.y && vpos.y < viewPos.y + view_height && vpos.x >= viewPos.x && vpos.x < viewPos.x + view_width);
    }

    // Use this function on coordinates that are relative to the screen that is been drawn i.e. the cursor position
    [[nodiscard]] constexpr bool ContainsScreen(const ScreenCoordsXY& sPos) const
    {
        return (sPos.x >= pos.x && sPos.x < pos.x + width && sPos.y >= pos.y && sPos.y < pos.y + height);
    }

    [[nodiscard]] ScreenCoordsXY ScreenToViewportCoord(const ScreenCoordsXY& screenCoord) const;

    void Invalidate() const;
};

/**
 * Scroll structure
 * size: 0x12
 */
struct rct_scroll
{
    uint16_t flags{};          // 0x00
    uint16_t h_left{};         // 0x02
    uint16_t h_right{};        // 0x04
    uint16_t h_thumb_left{};   // 0x06
    uint16_t h_thumb_right{};  // 0x08
    uint16_t v_top{};          // 0x0A
    uint16_t v_bottom{};       // 0x0C
    uint16_t v_thumb_top{};    // 0x0E
    uint16_t v_thumb_bottom{}; // 0x10
};

constexpr auto WINDOW_SCROLL_UNDEFINED = std::numeric_limits<uint16_t>::max();

struct Focus
{
    using CoordinateFocus = CoordsXYZ;
    using EntityFocus = EntityId;

    ZoomLevel zoom{};
    std::variant<CoordinateFocus, EntityFocus> data;

    template<typename T> constexpr explicit Focus(T newValue, ZoomLevel newZoom = {})
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

struct WindowEventList
{
    void (*close)(struct rct_window*){};
    void (*mouse_up)(struct rct_window*, WidgetIndex){};
    void (*resize)(struct rct_window*){};
    void (*mouse_down)(struct rct_window*, WidgetIndex, Widget*){};
    void (*dropdown)(struct rct_window*, WidgetIndex, int32_t){};
    void (*unknown_05)(struct rct_window*){};
    void (*update)(struct rct_window*){};
    void (*periodic_update)(struct rct_window*){};
    void (*tool_update)(struct rct_window*, WidgetIndex, const ScreenCoordsXY&){};
    void (*tool_down)(struct rct_window*, WidgetIndex, const ScreenCoordsXY&){};
    void (*tool_drag)(struct rct_window*, WidgetIndex, const ScreenCoordsXY&){};
    void (*tool_up)(struct rct_window*, WidgetIndex, const ScreenCoordsXY&){};
    void (*tool_abort)(struct rct_window*, WidgetIndex){};
    void (*get_scroll_size)(struct rct_window*, int32_t, int32_t*, int32_t*){};
    void (*scroll_mousedown)(struct rct_window*, int32_t, const ScreenCoordsXY&){};
    void (*scroll_mousedrag)(struct rct_window*, int32_t, const ScreenCoordsXY&){};
    void (*scroll_mouseover)(struct rct_window*, int32_t, const ScreenCoordsXY&){};
    void (*text_input)(struct rct_window*, WidgetIndex, char*){};
    void (*viewport_rotate)(struct rct_window*){};
    void (*scroll_select)(struct rct_window*, int32_t, int32_t){};
    OpenRCT2String (*tooltip)(struct rct_window*, const WidgetIndex, const StringId){};
    void (*cursor)(struct rct_window*, WidgetIndex, const ScreenCoordsXY&, CursorID*){};
    void (*moved)(struct rct_window*, const ScreenCoordsXY&){};
    void (*invalidate)(struct rct_window*){};
    void (*paint)(struct rct_window*, DrawPixelInfo*){};
    void (*scroll_paint)(struct rct_window*, DrawPixelInfo*, int32_t){};

    typedef void (*fnEventInitializer)(WindowEventList&);
    WindowEventList(fnEventInitializer fn)
    {
        fn(*this);
    }
};

struct campaign_variables
{
    int16_t campaign_type;
    int16_t no_weeks; // 0x482
    union
    {
        ::RideId RideId;
        ObjectEntryIndex ShopItemId;
    };
    uint32_t pad_486;
};

struct new_ride_variables
{
    RideSelection SelectedRide;    // 0x480
    RideSelection HighlightedRide; // 0x482
    uint16_t pad_484;
    uint16_t pad_486;
    uint16_t selected_ride_countdown; // 488
};

struct ride_variables
{
    int16_t view;
    int32_t var_482;
    int32_t var_486;
};

struct track_list_variables
{
    bool track_list_being_updated;
    bool reload_track_designs;
};

struct error_variables
{
    uint16_t var_480;
};

struct window_close_modifier
{
    window_identifier window;
    CloseWindowModifier modifier;
};

struct rct_window;

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

enum SCROLL_FLAGS
{
    HSCROLLBAR_VISIBLE = (1 << 0),
    HSCROLLBAR_THUMB_PRESSED = (1 << 1),
    HSCROLLBAR_LEFT_PRESSED = (1 << 2),
    HSCROLLBAR_RIGHT_PRESSED = (1 << 3),
    VSCROLLBAR_VISIBLE = (1 << 4),
    VSCROLLBAR_THUMB_PRESSED = (1 << 5),
    VSCROLLBAR_UP_PRESSED = (1 << 6),
    VSCROLLBAR_DOWN_PRESSED = (1 << 7),
};

#define SCROLLBAR_SIZE 16

enum
{
    SCROLL_PART_NONE = -1,
    SCROLL_PART_VIEW = 0,
    SCROLL_PART_HSCROLLBAR_LEFT = 1,
    SCROLL_PART_HSCROLLBAR_RIGHT = 2,
    SCROLL_PART_HSCROLLBAR_LEFT_TROUGH = 3,
    SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH = 4,
    SCROLL_PART_HSCROLLBAR_THUMB = 5,
    SCROLL_PART_VSCROLLBAR_TOP = 6,
    SCROLL_PART_VSCROLLBAR_BOTTOM = 7,
    SCROLL_PART_VSCROLLBAR_TOP_TROUGH = 8,
    SCROLL_PART_VSCROLLBAR_BOTTOM_TROUGH = 9,
    SCROLL_PART_VSCROLLBAR_THUMB = 10,
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

#define WC_MAIN_WINDOW__0 0
#define WC_TOP_TOOLBAR__WIDX_PAUSE 0
#define WC_TOP_TOOLBAR__WIDX_LAND 8
#define WC_TOP_TOOLBAR__WIDX_WATER 9
#define WC_TOP_TOOLBAR__WIDX_SCENERY 10
#define WC_TOP_TOOLBAR__WIDX_PATH 11
#define WC_TOP_TOOLBAR__WIDX_CLEAR_SCENERY 17
#define WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT 25
#define WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE 30
#define WC_RIDE_CONSTRUCTION__WIDX_EXIT 31
#define WC_RIDE_CONSTRUCTION__WIDX_ROTATE 32
#define WC_MAZE_CONSTRUCTION__WIDX_MAZE_DIRECTION_GROUPBOX WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT
#define WC_MAZE_CONSTRUCTION__WIDX_MAZE_ENTRANCE WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE
#define WC_MAZE_CONSTRUCTION__WIDX_MAZE_EXIT WC_RIDE_CONSTRUCTION__WIDX_EXIT
#define WC_SCENERY__WIDX_SCENERY_TAB_1 12
#define WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON 5
#define WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON 10
#define WC_PEEP__WIDX_PATROL 10
#define WC_PEEP__WIDX_ACTION_LBL 13
#define WC_PEEP__WIDX_PICKUP 14
#define WC_TRACK_DESIGN_LIST__WIDX_ROTATE 8
#define WC_TRACK_DESIGN_PLACE__WIDX_ROTATE 3
#define WC_MAP__WIDX_ROTATE_90 24
#define WC_EDITOR_OBJECT_SELECTION__WIDX_TAB_1 21
#define WC_STAFF__WIDX_PICKUP 9
#define WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE 13
#define WC_TILE_INSPECTOR__WIDX_BUTTON_COPY 16
#define WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE 15
#define WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE 10
#define WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP 11
#define WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN 12
#define WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE 5
#define WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE 6
#define WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE 8
#define WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE 9
#define WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SURFACE TileInspectorPage::Surface
#define WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_INCREASE 27
#define WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_DECREASE 28
#define WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_PATH TileInspectorPage::Path
#define WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_INCREASE 27
#define WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_DECREASE 28
#define WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_TRACK TileInspectorPage::Track
#define WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_INCREASE 28
#define WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_DECREASE 29
#define WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SCENERY TileInspectorPage::Scenery
#define WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_INCREASE 27
#define WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_DECREASE 28
#define WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_ENTRANCE TileInspectorPage::Entrance
#define WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE 27
#define WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE 28
#define WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_WALL TileInspectorPage::Wall
#define WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_INCREASE 27
#define WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_DECREASE 28
#define WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_LARGE_SCENERY TileInspectorPage::LargeScenery
#define WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE 27
#define WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE 28
#define WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_BANNER TileInspectorPage::Banner
#define WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_INCREASE 27
#define WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_DECREASE 28

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
};

using modal_callback = void (*)(int32_t result);
using close_callback = void (*)();

#define WINDOW_LIMIT_MIN 4
#define WINDOW_LIMIT_MAX 64
#define WINDOW_LIMIT_RESERVED 4 // Used to reserve room for the main viewport, toolbars, etc.

extern rct_window* gWindowAudioExclusive;

extern uint16_t gWindowUpdateTicks;
namespace MapFlashingFlags
{
    constexpr uint16_t GuestListOpen = (1 << 0);
    constexpr uint16_t FlashGuests = (1 << 1);
    constexpr uint16_t StaffListOpen = (1 << 2);
    constexpr uint16_t FlashStaff = (1 << 3);
    constexpr uint16_t SwitchColour = (1 << 15); // Every couple ticks the colour switches
} // namespace MapFlashingFlags
extern uint16_t gWindowMapFlashingFlags;

extern colour_t gCurrentWindowColours[4];

extern bool gDisableErrorWindowSound;

std::list<std::shared_ptr<rct_window>>::iterator WindowGetIterator(const rct_window* w);
void WindowVisitEach(std::function<void(rct_window*)> func);

void WindowDispatchUpdateAll();
void WindowUpdateAllViewports();
void WindowUpdateAll();

void WindowSetWindowLimit(int32_t value);

rct_window* WindowBringToFront(rct_window& w);
rct_window* WindowBringToFrontByClass(WindowClass cls);
rct_window* WindowBringToFrontByClassWithFlags(WindowClass cls, uint16_t flags);
rct_window* WindowBringToFrontByNumber(WindowClass cls, rct_windownumber number);

rct_window* WindowCreate(
    std::unique_ptr<rct_window>&& w, WindowClass cls, ScreenCoordsXY pos, int32_t width, int32_t height, uint32_t flags);
template<typename T, typename std::enable_if<std::is_base_of<rct_window, T>::value>::type* = nullptr>
T* WindowCreate(WindowClass cls, const ScreenCoordsXY& pos = {}, int32_t width = 0, int32_t height = 0, uint32_t flags = 0)
{
    return static_cast<T*>(WindowCreate(std::make_unique<T>(), cls, pos, width, height, flags));
}
template<typename T, typename... TArgs, typename std::enable_if<std::is_base_of<rct_window, T>::value>::type* = nullptr>
T* WindowCreate(WindowClass cls, int32_t width, int32_t height, uint32_t flags = 0, TArgs&&... args)
{
    return static_cast<T*>(
        WindowCreate(std::make_unique<T>(std::forward<TArgs>(args)...), cls, {}, width, height, flags | WF_AUTO_POSITION));
}
template<typename T, typename std::enable_if<std::is_base_of<rct_window, T>::value>::type* = nullptr>
T* WindowFocusOrCreate(WindowClass cls, const ScreenCoordsXY& pos, int32_t width, int32_t height, uint32_t flags = 0)
{
    auto* w = WindowBringToFrontByClass(cls);
    if (w == nullptr)
    {
        w = WindowCreate<T>(cls, pos, width, height, flags);
    }
    return static_cast<T*>(w);
}
template<typename T, typename std::enable_if<std::is_base_of<rct_window, T>::value>::type* = nullptr>
T* WindowFocusOrCreate(WindowClass cls, int32_t width, int32_t height, uint32_t flags = 0)
{
    auto* w = WindowBringToFrontByClass(cls);
    if (w == nullptr)
    {
        w = WindowCreate<T>(cls, width, height, flags);
    }
    return static_cast<T*>(w);
}

rct_window* WindowCreate(
    const ScreenCoordsXY& pos, int32_t width, int32_t height, WindowEventList* event_handlers, WindowClass cls, uint32_t flags);
rct_window* WindowCreateAutoPos(
    int32_t width, int32_t height, WindowEventList* event_handlers, WindowClass cls, uint32_t flags);
rct_window* WindowCreateCentred(
    int32_t width, int32_t height, WindowEventList* event_handlers, WindowClass cls, uint32_t flags);

void WindowClose(rct_window& window);
void WindowCloseByClass(WindowClass cls);
void WindowCloseByNumber(WindowClass cls, rct_windownumber number);
void WindowCloseByNumber(WindowClass cls, EntityId number);
void WindowCloseTop();
void WindowCloseAll();
void WindowCloseAllExceptClass(WindowClass cls);
void WindowCloseAllExceptFlags(uint16_t flags);
void WindowCloseAllExceptNumberAndClass(rct_windownumber number, WindowClass cls);
rct_window* WindowFindByClass(WindowClass cls);
rct_window* WindowFindByNumber(WindowClass cls, rct_windownumber number);
rct_window* WindowFindByNumber(WindowClass cls, EntityId id);
rct_window* WindowFindFromPoint(const ScreenCoordsXY& screenCoords);
WidgetIndex WindowFindWidgetFromPoint(rct_window& w, const ScreenCoordsXY& screenCoords);
void WindowInvalidateByClass(WindowClass cls);
void WindowInvalidateByNumber(WindowClass cls, rct_windownumber number);
void WindowInvalidateByNumber(WindowClass cls, EntityId id);
void WindowInvalidateAll();
void WidgetInvalidate(rct_window& w, WidgetIndex widgetIndex);
void WidgetInvalidateByClass(WindowClass cls, WidgetIndex widgetIndex);
void WidgetInvalidateByNumber(WindowClass cls, rct_windownumber number, WidgetIndex widgetIndex);
void WindowInitScrollWidgets(rct_window& w);
void WindowUpdateScrollWidgets(rct_window& w);
int32_t WindowGetScrollDataIndex(const rct_window& w, WidgetIndex widget_index);

void WindowPushOthersRight(rct_window& w);
void WindowPushOthersBelow(rct_window& w1);

rct_window* WindowGetMain();

void WindowScrollToLocation(rct_window& w, const CoordsXYZ& coords);
void WindowRotateCamera(rct_window& w, int32_t direction);
void WindowViewportGetMapCoordsByCursor(
    const rct_window& w, int32_t* map_x, int32_t* map_y, int32_t* offset_x, int32_t* offset_y);
void WindowViewportCentreTileAroundCursor(rct_window& w, int32_t map_x, int32_t map_y, int32_t offset_x, int32_t offset_y);
void WindowCheckAllValidZoom();
void WindowZoomSet(rct_window& w, ZoomLevel zoomLevel, bool atCursor);
void WindowZoomIn(rct_window& w, bool atCursor);
void WindowZoomOut(rct_window& w, bool atCursor);
void MainWindowZoom(bool zoomIn, bool atCursor);

void WindowDrawAll(DrawPixelInfo* dpi, int32_t left, int32_t top, int32_t right, int32_t bottom);
void WindowDraw(DrawPixelInfo* dpi, rct_window& w, int32_t left, int32_t top, int32_t right, int32_t bottom);
void WindowDrawWidgets(rct_window& w, DrawPixelInfo* dpi);
void WindowDrawViewport(DrawPixelInfo* dpi, rct_window& w);

void WindowSetPosition(rct_window& w, const ScreenCoordsXY& screenCoords);
void WindowMovePosition(rct_window& w, const ScreenCoordsXY& screenCoords);
void WindowResize(rct_window& w, int32_t dw, int32_t dh);
void WindowSetResize(rct_window& w, int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight);

bool ToolSet(const rct_window& w, WidgetIndex widgetIndex, Tool tool);
void ToolCancel();

void WindowCloseConstructionWindows();

void WindowUpdateViewportRideMusic();

rct_viewport* WindowGetViewport(rct_window* window);

// Open window functions
void WindowRelocateWindows(int32_t width, int32_t height);
void WindowResizeGui(int32_t width, int32_t height);
void WindowResizeGuiScenarioEditor(int32_t width, int32_t height);
void RideConstructionToolupdateEntranceExit(const ScreenCoordsXY& screenCoords);
void RideConstructionToolupdateConstruct(const ScreenCoordsXY& screenCoords);
void RideConstructionTooldownConstruct(const ScreenCoordsXY& screenCoords);

void WindowEventCloseCall(rct_window* w);
void WindowEventMouseUpCall(rct_window* w, WidgetIndex widgetIndex);
void WindowEventResizeCall(rct_window* w);
void WindowEventMouseDownCall(rct_window* w, WidgetIndex widgetIndex);
void WindowEventDropdownCall(rct_window* w, WidgetIndex widgetIndex, int32_t dropdownIndex);
void WindowEventUnknown05Call(rct_window* w);
void WindowEventUpdateCall(rct_window* w);
void WindowEventPeriodicUpdateCall(rct_window* w);
void WindowEventToolUpdateCall(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void WindowEventToolDownCall(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void WindowEventToolDragCall(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void WindowEventToolUpCall(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void WindowEventToolAbortCall(rct_window* w, WidgetIndex widgetIndex);
void WindowGetScrollSize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height);
void WindowEventScrollMousedownCall(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
void WindowEventScrollMousedragCall(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
void WindowEventScrollMouseoverCall(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
void WindowEventTextinputCall(rct_window* w, WidgetIndex widgetIndex, char* text);
void WindowEventViewportRotateCall(rct_window* w);
void WindowEventScrollSelectCall(rct_window* w, int32_t scrollIndex, int32_t scrollAreaType);
OpenRCT2String WindowEventTooltipCall(rct_window* w, const WidgetIndex widgetIndex, const StringId fallback);
CursorID WindowEventCursorCall(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void WindowEventMovedCall(rct_window* w, const ScreenCoordsXY& screenCoords);
void WindowEventInvalidateCall(rct_window* w);
void WindowEventPaintCall(rct_window* w, DrawPixelInfo* dpi);
void WindowEventScrollPaintCall(rct_window* w, DrawPixelInfo* dpi, int32_t scrollIndex);

void InvalidateAllWindowsAfterInput();
void TextinputCancel();

void WindowMoveAndSnap(rct_window& w, ScreenCoordsXY newWindowCoords, int32_t snapProximity);
int32_t WindowCanResize(const rct_window& w);

void WindowStartTextbox(
    rct_window& call_w, WidgetIndex call_widget, StringId existing_text, char* existing_args, int32_t maxLength);
void WindowCancelTextbox();
void WindowUpdateTextboxCaret();
void WindowUpdateTextbox();

bool WindowIsVisible(rct_window& w);

bool SceneryToolIsActive();

rct_viewport* WindowGetPreviousViewport(rct_viewport* current);
void WindowResetVisibilities();
void WindowInitAll();

void WindowRideConstructionKeyboardShortcutTurnLeft();
void WindowRideConstructionKeyboardShortcutTurnRight();
void WindowRideConstructionKeyboardShortcutUseTrackDefault();
void WindowRideConstructionKeyboardShortcutSlopeDown();
void WindowRideConstructionKeyboardShortcutSlopeUp();
void WindowRideConstructionKeyboardShortcutChainLiftToggle();
void WindowRideConstructionKeyboardShortcutBankLeft();
void WindowRideConstructionKeyboardShortcutBankRight();
void WindowRideConstructionKeyboardShortcutPreviousTrack();
void WindowRideConstructionKeyboardShortcutNextTrack();
void WindowRideConstructionKeyboardShortcutBuildCurrent();
void WindowRideConstructionKeyboardShortcutDemolishCurrent();

void WindowFootpathKeyboardShortcutTurnLeft();
void WindowFootpathKeyboardShortcutTurnRight();
void WindowFootpathKeyboardShortcutSlopeDown();
void WindowFootpathKeyboardShortcutSlopeUp();
void WindowFootpathKeyboardShortcutBuildCurrent();
void WindowFootpathKeyboardShortcutDemolishCurrent();

void WindowFollowSprite(rct_window& w, EntityId spriteIndex);
void WindowUnfollowSprite(rct_window& w);

bool WindowRideConstructionUpdateState(
    int32_t* trackType, int32_t* trackDirection, RideId* rideIndex, int32_t* _liftHillAndAlternativeState, CoordsXYZ* trackPos,
    int32_t* properties);
money32 PlaceProvisionalTrackPiece(
    RideId rideIndex, int32_t trackType, int32_t trackDirection, int32_t liftHillAndAlternativeState,
    const CoordsXYZ& trackPos);

extern RideConstructionState _rideConstructionState2;

rct_window* WindowGetListening();
WindowClass WindowGetClassification(const rct_window& window);
