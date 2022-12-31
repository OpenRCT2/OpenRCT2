/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

struct rct_drawpixelinfo;
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

/**
 * Widget structure
 * size: 0x10
 */
struct rct_widget
{
    WindowWidgetType type; // 0x00
    uint8_t colour;        // 0x01
    int16_t left;          // 0x02
    int16_t right;         // 0x04
    int16_t top;           // 0x06
    int16_t bottom;        // 0x08
    union
    { // 0x0A
        uint32_t image;
        StringId text;
        uint32_t content;
        utf8* string;
    };
    StringId tooltip; // 0x0E

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
    void (*mouse_down)(struct rct_window*, WidgetIndex, rct_widget*){};
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
    void (*paint)(struct rct_window*, rct_drawpixelinfo*){};
    void (*scroll_paint)(struct rct_window*, rct_drawpixelinfo*, int32_t){};

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
#define WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT 23
#define WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE 29
#define WC_RIDE_CONSTRUCTION__WIDX_EXIT 30
#define WC_RIDE_CONSTRUCTION__WIDX_ROTATE 32
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

std::list<std::shared_ptr<rct_window>>::iterator window_get_iterator(const rct_window* w);
void window_visit_each(std::function<void(rct_window*)> func);

void window_dispatch_update_all();
void window_update_all_viewports();
void window_update_all();

void window_set_window_limit(int32_t value);

rct_window* window_bring_to_front(rct_window& w);
rct_window* window_bring_to_front_by_class(WindowClass cls);
rct_window* window_bring_to_front_by_class_with_flags(WindowClass cls, uint16_t flags);
rct_window* window_bring_to_front_by_number(WindowClass cls, rct_windownumber number);

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
    auto* w = window_bring_to_front_by_class(cls);
    if (w == nullptr)
    {
        w = WindowCreate<T>(cls, pos, width, height, flags);
    }
    return static_cast<T*>(w);
}
template<typename T, typename std::enable_if<std::is_base_of<rct_window, T>::value>::type* = nullptr>
T* WindowFocusOrCreate(WindowClass cls, int32_t width, int32_t height, uint32_t flags = 0)
{
    auto* w = window_bring_to_front_by_class(cls);
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

void window_close(rct_window& window);
void window_close_by_class(WindowClass cls);
void window_close_by_number(WindowClass cls, rct_windownumber number);
void window_close_by_number(WindowClass cls, EntityId number);
void window_close_top();
void window_close_all();
void window_close_all_except_class(WindowClass cls);
void window_close_all_except_flags(uint16_t flags);
void window_close_all_except_number_and_class(rct_windownumber number, WindowClass cls);
rct_window* window_find_by_class(WindowClass cls);
rct_window* window_find_by_number(WindowClass cls, rct_windownumber number);
rct_window* window_find_by_number(WindowClass cls, EntityId id);
rct_window* window_find_from_point(const ScreenCoordsXY& screenCoords);
WidgetIndex window_find_widget_from_point(rct_window& w, const ScreenCoordsXY& screenCoords);
void window_invalidate_by_class(WindowClass cls);
void window_invalidate_by_number(WindowClass cls, rct_windownumber number);
void window_invalidate_by_number(WindowClass cls, EntityId id);
void window_invalidate_all();
void widget_invalidate(rct_window& w, WidgetIndex widgetIndex);
void widget_invalidate_by_class(WindowClass cls, WidgetIndex widgetIndex);
void widget_invalidate_by_number(WindowClass cls, rct_windownumber number, WidgetIndex widgetIndex);
void WindowInitScrollWidgets(rct_window& w);
void window_update_scroll_widgets(rct_window& w);
int32_t window_get_scroll_data_index(const rct_window& w, WidgetIndex widget_index);

void window_push_others_right(rct_window& w);
void window_push_others_below(rct_window& w1);

rct_window* window_get_main();

void window_scroll_to_location(rct_window& w, const CoordsXYZ& coords);
void window_rotate_camera(rct_window& w, int32_t direction);
void window_viewport_get_map_coords_by_cursor(
    const rct_window& w, int32_t* map_x, int32_t* map_y, int32_t* offset_x, int32_t* offset_y);
void window_viewport_centre_tile_around_cursor(rct_window& w, int32_t map_x, int32_t map_y, int32_t offset_x, int32_t offset_y);
void window_check_all_valid_zoom();
void window_zoom_set(rct_window& w, ZoomLevel zoomLevel, bool atCursor);
void window_zoom_in(rct_window& w, bool atCursor);
void window_zoom_out(rct_window& w, bool atCursor);
void main_window_zoom(bool zoomIn, bool atCursor);

void window_draw_all(rct_drawpixelinfo* dpi, int32_t left, int32_t top, int32_t right, int32_t bottom);
void window_draw(rct_drawpixelinfo* dpi, rct_window& w, int32_t left, int32_t top, int32_t right, int32_t bottom);
void WindowDrawWidgets(rct_window& w, rct_drawpixelinfo* dpi);
void window_draw_viewport(rct_drawpixelinfo* dpi, rct_window& w);

void window_set_position(rct_window& w, const ScreenCoordsXY& screenCoords);
void window_move_position(rct_window& w, const ScreenCoordsXY& screenCoords);
void window_resize(rct_window& w, int32_t dw, int32_t dh);
void window_set_resize(rct_window& w, int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight);

bool tool_set(const rct_window& w, WidgetIndex widgetIndex, Tool tool);
void tool_cancel();

void window_close_construction_windows();

void window_update_viewport_ride_music();

rct_viewport* window_get_viewport(rct_window* window);

// Open window functions
void window_relocate_windows(int32_t width, int32_t height);
void window_resize_gui(int32_t width, int32_t height);
void window_resize_gui_scenario_editor(int32_t width, int32_t height);
void ride_construction_toolupdate_entrance_exit(const ScreenCoordsXY& screenCoords);
void ride_construction_toolupdate_construct(const ScreenCoordsXY& screenCoords);
void ride_construction_tooldown_construct(const ScreenCoordsXY& screenCoords);

void window_event_close_call(rct_window* w);
void window_event_mouse_up_call(rct_window* w, WidgetIndex widgetIndex);
void window_event_resize_call(rct_window* w);
void window_event_mouse_down_call(rct_window* w, WidgetIndex widgetIndex);
void window_event_dropdown_call(rct_window* w, WidgetIndex widgetIndex, int32_t dropdownIndex);
void window_event_unknown_05_call(rct_window* w);
void window_event_update_call(rct_window* w);
void window_event_periodic_update_call(rct_window* w);
void window_event_tool_update_call(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void window_event_tool_down_call(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void window_event_tool_drag_call(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void window_event_tool_up_call(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void window_event_tool_abort_call(rct_window* w, WidgetIndex widgetIndex);
void window_get_scroll_size(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height);
void window_event_scroll_mousedown_call(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
void window_event_scroll_mousedrag_call(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
void window_event_scroll_mouseover_call(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
void window_event_textinput_call(rct_window* w, WidgetIndex widgetIndex, char* text);
void window_event_viewport_rotate_call(rct_window* w);
void window_event_scroll_select_call(rct_window* w, int32_t scrollIndex, int32_t scrollAreaType);
OpenRCT2String window_event_tooltip_call(rct_window* w, const WidgetIndex widgetIndex, const StringId fallback);
CursorID window_event_cursor_call(rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords);
void window_event_moved_call(rct_window* w, const ScreenCoordsXY& screenCoords);
void window_event_invalidate_call(rct_window* w);
void window_event_paint_call(rct_window* w, rct_drawpixelinfo* dpi);
void window_event_scroll_paint_call(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex);

void InvalidateAllWindowsAfterInput();
void textinput_cancel();

void window_move_and_snap(rct_window& w, ScreenCoordsXY newWindowCoords, int32_t snapProximity);
int32_t window_can_resize(const rct_window& w);

void window_start_textbox(
    rct_window& call_w, WidgetIndex call_widget, StringId existing_text, char* existing_args, int32_t maxLength);
void window_cancel_textbox();
void window_update_textbox_caret();
void window_update_textbox();

bool window_is_visible(rct_window& w);

bool scenery_tool_is_active();

rct_viewport* window_get_previous_viewport(rct_viewport* current);
void window_reset_visibilities();
void window_init_all();

void window_ride_construction_keyboard_shortcut_turn_left();
void window_ride_construction_keyboard_shortcut_turn_right();
void window_ride_construction_keyboard_shortcut_use_track_default();
void window_ride_construction_keyboard_shortcut_slope_down();
void window_ride_construction_keyboard_shortcut_slope_up();
void window_ride_construction_keyboard_shortcut_chain_lift_toggle();
void window_ride_construction_keyboard_shortcut_bank_left();
void window_ride_construction_keyboard_shortcut_bank_right();
void window_ride_construction_keyboard_shortcut_previous_track();
void window_ride_construction_keyboard_shortcut_next_track();
void window_ride_construction_keyboard_shortcut_build_current();
void window_ride_construction_keyboard_shortcut_demolish_current();

void window_footpath_keyboard_shortcut_turn_left();
void window_footpath_keyboard_shortcut_turn_right();
void window_footpath_keyboard_shortcut_slope_down();
void window_footpath_keyboard_shortcut_slope_up();
void window_footpath_keyboard_shortcut_build_current();
void window_footpath_keyboard_shortcut_demolish_current();

void window_follow_sprite(rct_window& w, EntityId spriteIndex);
void window_unfollow_sprite(rct_window& w);

bool window_ride_construction_update_state(
    int32_t* trackType, int32_t* trackDirection, RideId* rideIndex, int32_t* _liftHillAndAlternativeState, CoordsXYZ* trackPos,
    int32_t* properties);
money32 place_provisional_track_piece(
    RideId rideIndex, int32_t trackType, int32_t trackDirection, int32_t liftHillAndAlternativeState,
    const CoordsXYZ& trackPos);

extern RideConstructionState _rideConstructionState2;

rct_window* window_get_listening();
WindowClass window_get_classification(const rct_window& window);
