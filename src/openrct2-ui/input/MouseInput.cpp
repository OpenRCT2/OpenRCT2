/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <cmath>
#include <openrct2-ui/input/Input.h>
#include <openrct2-ui/input/Scrollbars.h>
#include <openrct2-ui/input/Widget.h>
#include <openrct2-ui/input/WindowPositioning.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Util.hpp>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/Cursors.h>
#include <openrct2/interface/InteractiveConsole.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/platform/platform.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/Sprite.h>

// TODO: Change to <optional> when Xcode gets std::optional support
#if defined(__has_include) && __has_include(<optional>)
#    include <optional>
using std::optional;
#else
#    include <experimental/optional>
using std::experimental::optional;
#endif

static input_mouse_data MouseInputQueue[64];
static size_t MouseInputQueueReadIndex = 0;
static size_t MouseInputQueueWriteIndex = 0;

int32_t gInputDragLastX;
int32_t gInputDragLastY;

uint16_t gTooltipTimeout;
widget_ref gTooltipWidget;
int32_t gTooltipCursorX;
int32_t gTooltipCursorY;

#pragma region Mouse input
/**
 *
 *  rct2: 0x00407074
 */
static optional<input_mouse_data> GetMouseInput()
{
    // Check if that location has been written to yet
    if (MouseInputQueueReadIndex == MouseInputQueueWriteIndex)
    {
        return {};
    }

    input_mouse_data result = MouseInputQueue[MouseInputQueueReadIndex];
    MouseInputQueueReadIndex = (MouseInputQueueReadIndex + 1) % Util::CountOf(MouseInputQueue);
    return result;
}

/**
 *
 *  rct2: 0x006E83C7
 */
static input_mouse_data GetNextInput()
{
    if (optional<input_mouse_data> input = GetMouseInput())
    {
        return *input;
    }

    const CursorState* cursorState = context_get_cursor_state();
    return { .X = cursorState->x, .Y = cursorState->y, .State = 0 };
}

/**
 *
 *  rct2: 0x006ED833
 */
static void ProcessMouseOver(const input_mouse_data& mouseData)
{
    optional<int32_t> cursorId;
    set_map_tooltip_format_arg(0, rct_string_id, STR_NONE);

    if (rct_window* window = window_find_from_point(mouseData.X, mouseData.Y); window != nullptr)
    {
        if (rct_widgetindex widgetId = window_find_widget_from_point(window, mouseData.X, mouseData.Y); widgetId != -1)
        {
            cursorId = input_get_widget_cursor_id(window, widgetId, mouseData);
        }
    }

    if (cursorId)
    {
        viewport_interaction_right_over(mouseData.X, mouseData.Y);
        input_set_mouse_cursor(*cursorId);
    }
}

/**
 *
 *  rct2: 0x006ED801
 */
static void ProcessMouseTool(const input_mouse_data& mouseData)
{
    if (!(gInputFlags & INPUT_FLAG_TOOL_ACTIVE))
    {
        return;
    }

    rct_window* w = window_find_by_number(gCurrentToolWidget.window_classification, gCurrentToolWidget.window_number);
    if (w == nullptr)
    {
        tool_cancel();
    }
    else
    {
        window_event_tool_update_call(w, gCurrentToolWidget.widget_index, mouseData.X, mouseData.Y);
    }
}

static void HandleInputNormal(
    rct_window* w, const rct_widget* widget, const rct_widgetindex widgetIndex, const input_mouse_data& mouseData)
{
    switch (mouseData.State)
    {
        case MOUSE_STATE_RELEASED:
            input_widget_over(w, mouseData, widgetIndex);
            break;

        case MOUSE_STATE_LEFT_PRESS:
            input_widget_left(w, mouseData, widgetIndex);
            break;

        case MOUSE_STATE_RIGHT_PRESS:
            input_widget_right(w, mouseData, widget, widgetIndex);
            break;

        default:
            break;
    }
}

/**
 *
 *  rct2: 0x006E8655
 */
static void HandleInputMouse(const input_mouse_data& mouseData)
{
    // Get window and widget under cursor position
    rct_window* w = window_find_from_point(mouseData.X, mouseData.Y);
    rct_widgetindex widgetIndex = -1;
    rct_widget* widget = nullptr;

    if (w != nullptr)
    {
        widgetIndex = window_find_widget_from_point(w, mouseData.X, mouseData.Y);
        widget = (widgetIndex == -1) ? nullptr : &w->widgets[widgetIndex];
    }

    switch (gInputState)
    {
        case INPUT_STATE_RESET:
            window_tooltip_reset(mouseData.X, mouseData.Y);
            [[fallthrough]];

        case INPUT_STATE_NORMAL:
            HandleInputNormal(w, widget, widgetIndex, mouseData);
            break;

        case INPUT_STATE_WIDGET_PRESSED:
            input_widget_pressed(widgetIndex, w, widget, mouseData);
            break;

        case INPUT_STATE_POSITIONING_WINDOW:
            input_window_positioning(mouseData);
            break;

        case INPUT_STATE_VIEWPORT_RIGHT:
            input_viewport_right(mouseData);
            break;

        case INPUT_STATE_DROPDOWN_ACTIVE:
            input_widget_pressed(widgetIndex, w, widget, mouseData);
            break;

        case INPUT_STATE_VIEWPORT_LEFT:
            input_viewport_left(mouseData);
            break;

        case INPUT_STATE_SCROLL_LEFT:
            input_scroll_left(w, widgetIndex, mouseData);
            break;

        case INPUT_STATE_RESIZING:
            input_window_resizing(mouseData);
            break;

        case INPUT_STATE_SCROLL_RIGHT:
            input_scroll_right(mouseData);
            break;
    }
}

#pragma endregion

/**
 * rct2: 0x00406C96
 */
void input_store_mouse_input(int32_t state, int32_t x, int32_t y)
{
    size_t writeIndex = MouseInputQueueWriteIndex;
    size_t nextWriteIndex = (writeIndex + 1) % Util::CountOf(MouseInputQueue);

    // Check if the queue is full
    if (nextWriteIndex != MouseInputQueueReadIndex)
    {
        input_mouse_data* item = &MouseInputQueue[writeIndex];
        item->X = x;
        item->Y = y;
        item->State = static_cast<uint32_t>(state);

        MouseInputQueueWriteIndex = nextWriteIndex;
    }
}

void input_handle_edge_scroll()
{
    rct_window* mainWindow = window_get_main();
    if (mainWindow == nullptr || (mainWindow->flags & WF_NO_SCROLLING)
        || (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TITLE_DEMO)) || mainWindow->viewport == nullptr
        || !context_has_focus())
    {
        return;
    }

    int32_t scrollX = 0;
    int32_t scrollY = 0;

    // Scroll left / right
    const CursorState* cursorState = context_get_cursor_state();
    if (cursorState->x == 0)
    {
        scrollX = -1;
    }
    else if (cursorState->x >= context_get_width() - 1)
    {
        scrollX = 1;
    }

    // Scroll up / down
    if (cursorState->y == 0)
    {
        scrollY = -1;
    }
    else if (cursorState->y >= context_get_height() - 1)
    {
        scrollY = 1;
    }

    input_scroll_viewport(scrollX, scrollY);
}

bool input_test_place_object_modifier(PLACE_OBJECT_MODIFIER modifier)
{
    return gInputPlaceObjectModifier & modifier;
}

void input_scroll_viewport(int32_t scrollX, int32_t scrollY)
{
    rct_window* mainWindow = window_get_main();
    rct_viewport* viewport = mainWindow->viewport;

    const int32_t speed = gConfigGeneral.edge_scrolling_speed;

    int32_t dx = scrollX * (speed << viewport->zoom);
    int32_t dy = scrollY * (speed << viewport->zoom);

    if (scrollX != 0)
    {
        // Speed up scrolling horizontally when at the edge of the map
        // so that the speed is consistent with vertical edge scrolling.
        int32_t x = mainWindow->saved_view_x + viewport->view_width / 2 + dx;
        int32_t y = mainWindow->saved_view_y + viewport->view_height / 2;
        int32_t y_dy = mainWindow->saved_view_y + viewport->view_height / 2 + dy;

        LocationXY16 mapCoord = viewport_coord_to_map_coord(x, y, 0);
        LocationXY16 mapCoord_dy = viewport_coord_to_map_coord(x, y_dy, 0);

        // Check if we're crossing the boundary
        // Clamp to the map minimum value
        int32_t at_map_edge = 0;
        int32_t at_map_edge_dy = 0;
        if (mapCoord.x < MAP_MINIMUM_X_Y || mapCoord.y < MAP_MINIMUM_X_Y)
        {
            at_map_edge = 1;
        }
        if (mapCoord_dy.x < MAP_MINIMUM_X_Y || mapCoord_dy.y < MAP_MINIMUM_X_Y)
        {
            at_map_edge_dy = 1;
        }

        // Clamp to the map maximum value (scenario specific)
        if (mapCoord.x > gMapSizeMinus2 || mapCoord.y > gMapSizeMinus2)
        {
            at_map_edge = 1;
        }
        if (mapCoord_dy.x > gMapSizeMinus2 || mapCoord_dy.y > gMapSizeMinus2)
        {
            at_map_edge_dy = 1;
        }

        // If we crossed the boundary, multiply the distance by 2
        if (at_map_edge && at_map_edge_dy)
        {
            dx *= 2;
        }

        mainWindow->saved_view_x += dx;
        gInputFlags |= INPUT_FLAG_VIEWPORT_SCROLLING;
    }
    if (scrollY != 0)
    {
        mainWindow->saved_view_y += dy;
        gInputFlags |= INPUT_FLAG_VIEWPORT_SCROLLING;
    }
}

/**
 *
 *  rct2: 0x006EA627
 */
void input_handle()
{
    for (auto& w : g_window_list)
    {
        window_event_unknown_07_call(w.get());
    }

    invalidate_all_windows_after_input();

    input_mouse_data mouseData = { .X = 0, .Y = 0, .State = 1 };
    while (mouseData.State != MOUSE_STATE_RELEASED)
    {
        mouseData = GetNextInput();
        mouseData.State &= 0xFF;
        HandleInputMouse(mouseData);
    }

    if (gInputFlags & INPUT_FLAG_5)
    {
        HandleInputMouse(mouseData);
    }
    if (mouseData.X != MONEY32_UNDEFINED)
    {
        int32_t screenWidth = context_get_width();
        int32_t screenHeight = context_get_height();
        mouseData.X = std::clamp(0, mouseData.X, screenWidth - 1);
        mouseData.Y = std::clamp(0, mouseData.Y, screenHeight - 1);

        HandleInputMouse(mouseData);
        ProcessMouseOver(mouseData);
        ProcessMouseTool(mouseData);
    }

    for (auto& w : g_window_list)
    {
        window_event_unknown_08_call(w.get());
    }
}
