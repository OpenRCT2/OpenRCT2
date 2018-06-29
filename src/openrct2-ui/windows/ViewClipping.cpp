/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cmath>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Input.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/paint/Paint.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Location.hpp>

// clang-format off
enum WINDOW_VIEW_CLIPPING_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_CLIP_CHECKBOX_ENABLE,
    WIDX_GROUPBOX_VERTICAL,
    WIDX_CLIP_HEIGHT_VALUE,
    WIDX_CLIP_HEIGHT_INCREASE,
    WIDX_CLIP_HEIGHT_DECREASE,
    WIDX_CLIP_HEIGHT_SLIDER,
    WIDX_GROUPBOX_HORIZONTAL,
    WIDX_CLIP_SELECTOR,
    WIDX_CLIP_CLEAR,
};

enum class DISPLAY_TYPE {
    DISPLAY_RAW,
    DISPLAY_UNITS
};

static DISPLAY_TYPE gClipHeightDisplayType = DISPLAY_TYPE::DISPLAY_UNITS;

#pragma region Widgets

#define WW 180
#define WH 155

static rct_widget window_view_clipping_widgets[] = {
    { WWT_FRAME,        0,  0,          WW - 1,     0,      WH - 1,     STR_NONE,                               STR_NONE }, // panel / background
    { WWT_CAPTION,      0,  1,          WW - 2,     1,      14,         STR_VIEW_CLIPPING_TITLE,                STR_WINDOW_TITLE_TIP }, // title bar
    { WWT_CLOSEBOX,     0,  WW - 13,    WW - 3,     2,      13,         STR_CLOSE_X,                            STR_CLOSE_WINDOW_TIP }, // close x button
    { WWT_CHECKBOX,     0,  11,         WW - 11,    19,     29,         STR_VIEW_CLIPPING_HEIGHT_ENABLE,        STR_VIEW_CLIPPING_HEIGHT_ENABLE_TIP }, // clip enable/disable check box
    { WWT_GROUPBOX,     0,  5,          WW - 6,     36,     83,         STR_VIEW_CLIPPING_VERTICAL_CLIPPING,    STR_NONE },
      SPINNER_WIDGETS  (0,  90,         WW - 12,    51,     62,         STR_NONE,                               STR_VIEW_CLIPPING_HEIGHT_VALUE_TOGGLE), // clip height (3 widgets)
    { WWT_SCROLL,       0,  11,         WW - 12,    66,     78,         SCROLL_HORIZONTAL,                      STR_VIEW_CLIPPING_HEIGHT_SCROLL_TIP }, // clip height scrollbar
    { WWT_GROUPBOX,     0,  5,          WW - 6,     90,     WH - 6,     STR_VIEW_CLIPPING_HORIZONTAL_CLIPPING,  STR_NONE },
    { WWT_BUTTON,       0,  11,         WW - 12,    105,    121,        STR_VIEW_CLIPPING_SELECT_AREA,          STR_NONE }, // selector
    { WWT_BUTTON,       0,  11,         WW - 12,    126,    143,        STR_VIEW_CLIPPING_CLEAR_SELECTION,      STR_NONE }, // clear
    { WIDGETS_END }
};

#pragma endregion

#pragma region Members

static LocationXY16 _selectionStart;
static LocationXY8 _previousClipSelectionA;
static LocationXY8 _previousClipSelectionB;
static bool _toolActive;
static bool _dragging;

#pragma endregion

#pragma region Events

static void window_view_clipping_close_button(rct_window* w);
static void window_view_clipping_mouseup(rct_window* w, rct_widgetindex widgetIndex);
static void window_view_clipping_mousedown(rct_window*w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_view_clipping_update(rct_window* w);
static void window_view_clipping_tool_update(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_view_clipping_tool_down(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_view_clipping_tool_drag(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_view_clipping_tool_up(rct_window* w, rct_widgetindex, int32_t, int32_t);
static void window_view_clipping_invalidate(rct_window* w);
static void window_view_clipping_paint(rct_window* w, rct_drawpixelinfo* dpi);
static void window_view_clipping_scrollgetsize(rct_window* w, int scrollIndex, int* width, int* height);
static void window_view_clipping_close();

static rct_window_event_list window_view_clipping_events = {
    window_view_clipping_close_button,
    window_view_clipping_mouseup,
    nullptr,
    window_view_clipping_mousedown,
    nullptr,
    nullptr,
    window_view_clipping_update,
    nullptr,
    nullptr,
    window_view_clipping_tool_update,
    window_view_clipping_tool_down,
    window_view_clipping_tool_drag,
    window_view_clipping_tool_up,
    nullptr,
    nullptr,
    window_view_clipping_scrollgetsize,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_view_clipping_invalidate,
    window_view_clipping_paint,
    nullptr
};
// clang-format on

#pragma endregion

static void window_view_clipping_set_clipheight(rct_window *w, const uint8_t clipheight)
{
    gClipHeight = clipheight;
    rct_widget* widget = &window_view_clipping_widgets[WIDX_CLIP_HEIGHT_SLIDER];
    const float clip_height_ratio = (float)gClipHeight / 255;
    w->scrolls[0].h_left = (int16_t)std::ceil(clip_height_ratio * (w->scrolls[0].h_right - ((widget->right - widget->left) - 1)));
}

rct_window * window_view_clipping_open()
{
    rct_window* window;

    // Get the main viewport to set the view clipping flag.
    rct_window *mainWindow = window_get_main();

    // Check if window is already open
    window = window_find_by_class(WC_VIEW_CLIPPING);
    if (window != nullptr) {
        return window;
    }

    // Window is not open - create it.
    window = window_create(32, 32, WW, WH, &window_view_clipping_events, WC_VIEW_CLIPPING, 0);
    window->widgets = window_view_clipping_widgets;
    window->enabled_widgets = (1ULL << WIDX_CLOSE) |
        (1ULL << WIDX_CLIP_CHECKBOX_ENABLE) |
        (1ULL << WIDX_CLIP_HEIGHT_VALUE) |
        (1ULL << WIDX_CLIP_HEIGHT_INCREASE) |
        (1ULL << WIDX_CLIP_HEIGHT_DECREASE) |
        (1ULL << WIDX_CLIP_HEIGHT_SLIDER) |
        (1ULL << WIDX_CLIP_SELECTOR) |
        (1ULL << WIDX_CLIP_CLEAR);
    window->hold_down_widgets = (1ULL << WIDX_CLIP_HEIGHT_INCREASE) | (1UL << WIDX_CLIP_HEIGHT_DECREASE);

    window_init_scroll_widgets(window);

    // Initialise the clip height slider from the current clip height value.
    window_view_clipping_set_clipheight(window, gClipHeight);

    window_push_others_below(window);

    // Turn on view clipping when the window is opened.
    if (mainWindow != nullptr) {
        mainWindow->viewport->flags |= VIEWPORT_FLAG_CLIP_VIEW;
        window_invalidate(mainWindow);
    }

    _toolActive = false;
    _dragging = false;

    return window;
}

static void window_view_clipping_close()
{
    // Turn off view clipping when the window is closed.
    rct_window *mainWindow = window_get_main();
    if (mainWindow != nullptr) {
        mainWindow->viewport->flags &= ~VIEWPORT_FLAG_CLIP_VIEW;
        window_invalidate(mainWindow);
    }

}

static void window_view_clipping_close_button(rct_window *w)
{
    window_view_clipping_close();
}

// Returns true when the tool is active
static bool window_view_clipping_tool_is_active()
{
    if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
        return false;
    if (gCurrentToolWidget.window_classification != WC_VIEW_CLIPPING)
        return false;
    return _toolActive;
}

static void window_view_clipping_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    rct_window *mainWindow;

    // mouseup appears to be used for buttons, checkboxes
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_CLIP_CHECKBOX_ENABLE:
        // Toggle height clipping.
        mainWindow = window_get_main();
        if (mainWindow != nullptr) {
            mainWindow->viewport->flags ^= VIEWPORT_FLAG_CLIP_VIEW;
            window_invalidate(mainWindow);
        }
        window_invalidate(w);
        break;
    case WIDX_CLIP_HEIGHT_VALUE:
        // Toggle display of the cut height value in RAW vs UNITS
        if (gClipHeightDisplayType == DISPLAY_TYPE::DISPLAY_RAW)
        {
            gClipHeightDisplayType = DISPLAY_TYPE::DISPLAY_UNITS;
        }
        else
        {
            gClipHeightDisplayType = DISPLAY_TYPE::DISPLAY_RAW;
        }
        window_invalidate(w);
        break;
    case WIDX_CLIP_SELECTOR:
        // Activate the selection tool
        tool_set(w, WIDX_BACKGROUND, TOOL_CROSSHAIR);
        _toolActive = true;
        _dragging = false;

        // Reset clip selection to show all tiles
        _previousClipSelectionA = gClipSelectionA;
        _previousClipSelectionB = gClipSelectionB;
        gClipSelectionA = { 0, 0 };
        gClipSelectionB = { MAXIMUM_MAP_SIZE_TECHNICAL - 1, MAXIMUM_MAP_SIZE_TECHNICAL - 1 };
        gfx_invalidate_screen();
        break;
    case WIDX_CLIP_CLEAR:
        if (window_view_clipping_tool_is_active())
        {
            _toolActive = false;
            tool_cancel();
        }
        gClipSelectionA = { 0, 0 };
        gClipSelectionB = { MAXIMUM_MAP_SIZE_TECHNICAL - 1, MAXIMUM_MAP_SIZE_TECHNICAL - 1 };
        gfx_invalidate_screen();
        break;
    }
}

static void window_view_clipping_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    rct_window* mainWindow = window_get_main();

    switch (widgetIndex)
    {
        case WIDX_CLIP_HEIGHT_INCREASE:
            if (gClipHeight < 255)
                window_view_clipping_set_clipheight(w, gClipHeight + 1);
            mainWindow = window_get_main();
            if (mainWindow != nullptr)
                window_invalidate(mainWindow);
            break;
        case WIDX_CLIP_HEIGHT_DECREASE:
            if (gClipHeight > 0)
                window_view_clipping_set_clipheight(w, gClipHeight - 1);
            mainWindow = window_get_main();
            if (mainWindow != nullptr)
                window_invalidate(mainWindow);
            break;
    }
}

static void window_view_clipping_update(rct_window *w)
{
    const rct_widget *const widget = &window_view_clipping_widgets[WIDX_CLIP_HEIGHT_SLIDER];
    const rct_scroll *const scroll = &w->scrolls[0];
    const int16_t scroll_width = widget->right - widget->left - 1;
    const uint8_t clip_height = (uint8_t)(((float)scroll->h_left / (scroll->h_right - scroll_width)) * 255);
    if (clip_height != gClipHeight) {
        gClipHeight = clip_height;

        // Update the main window accordingly.
        rct_window *mainWindow = window_get_main();
        if (mainWindow != nullptr) {
            window_invalidate(mainWindow);
        }
    }

    // Restore previous selection if the tool has been interrupted
    if (_toolActive && !window_view_clipping_tool_is_active())
    {
        _toolActive = false;
        gClipSelectionA = _previousClipSelectionA;
        gClipSelectionB = _previousClipSelectionB;
    }

    widget_invalidate(w, WIDX_CLIP_HEIGHT_SLIDER);
}

static void window_view_clipping_tool_update(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    if (_dragging)
    {
        return;
    }

    int16_t mapX = x;
    int16_t mapY = y;
    int32_t direction;
    screen_pos_to_map_pos(&mapX, &mapY, &direction);
    if (mapX != LOCATION_NULL)
    {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        map_invalidate_tile_full(gMapSelectPositionA.x, gMapSelectPositionA.y);
        gMapSelectPositionA.x = gMapSelectPositionB.x = mapX;
        gMapSelectPositionA.y = gMapSelectPositionB.y = mapY;
        map_invalidate_tile_full(mapX, mapY);
        gMapSelectType = MAP_SELECT_TYPE_FULL;
    }
}

static void window_view_clipping_tool_down(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    int16_t mapX = x;
    int16_t mapY = y;
    int32_t direction;
    screen_pos_to_map_pos(&mapX, &mapY, &direction);
    if (mapX != LOCATION_NULL)
    {
        _dragging = true;
        _selectionStart = { mapX, mapY };
    }
}

static void window_view_clipping_tool_drag(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    if (!_dragging)
    {
        return;
    }

    int16_t mapX = x;
    int16_t mapY = y;
    int32_t direction;
    screen_pos_to_map_pos(&mapX, &mapY, &direction);
    if (mapX != LOCATION_NULL)
    {
        map_invalidate_selection_rect();
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        gMapSelectPositionA.x = std::min(_selectionStart.x, mapX);
        gMapSelectPositionB.x = std::max(_selectionStart.x, mapX);
        gMapSelectPositionA.y = std::min(_selectionStart.y, mapY);
        gMapSelectPositionB.y = std::max(_selectionStart.y, mapY);
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        map_invalidate_selection_rect();
    }
}

static void window_view_clipping_tool_up(struct rct_window*, rct_widgetindex, int32_t, int32_t)
{
    gClipSelectionA = { uint8_t(gMapSelectPositionA.x / 32), uint8_t(gMapSelectPositionA.y / 32) };
    gClipSelectionB = { uint8_t(gMapSelectPositionB.x / 32), uint8_t(gMapSelectPositionB.y / 32) };
    _toolActive = false;
    tool_cancel();
    gfx_invalidate_screen();
}

static void window_view_clipping_invalidate(rct_window *w)
{
    widget_scroll_update_thumbs(w, WIDX_CLIP_HEIGHT_SLIDER);

    rct_window *mainWindow = window_get_main();
    if (mainWindow != nullptr) {
        widget_set_checkbox_value(w, WIDX_CLIP_CHECKBOX_ENABLE, mainWindow->viewport->flags & VIEWPORT_FLAG_CLIP_VIEW);
    }

    if (window_view_clipping_tool_is_active())
    {
        w->pressed_widgets |= 1ULL << WIDX_CLIP_SELECTOR;
    }
    else
    {
        w->pressed_widgets &= ~(1ULL << WIDX_CLIP_SELECTOR);
    }
}

static void window_view_clipping_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    // Clip height value
    int32_t x = w->x + 8;
    int32_t y = w->y + w->widgets[WIDX_CLIP_HEIGHT_VALUE].top;
    gfx_draw_string_left(dpi, STR_VIEW_CLIPPING_HEIGHT_VALUE, nullptr, w->colours[0], x, y);

    x = w->x + w->widgets[WIDX_CLIP_HEIGHT_VALUE].left + 1;
    y = w->y + w->widgets[WIDX_CLIP_HEIGHT_VALUE].top;

    fixed16_1dp clipHeightValueInUnits;
    fixed32_2dp clipHeightValueInMeters;
    fixed16_1dp clipHeightValueInFeet;
    int32_t clipHeightRawValue = (int32_t)gClipHeight;
    switch (gClipHeightDisplayType) {
    case DISPLAY_TYPE::DISPLAY_RAW:
    default:
        gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &clipHeightRawValue, w->colours[0], x, y); //Printing the raw value.
        break;

    case DISPLAY_TYPE::DISPLAY_UNITS:
        // Print the value in the configured height label type:
        if (gConfigGeneral.show_height_as_units == 1) {
            // Height label is Units.
            clipHeightValueInUnits = (fixed16_1dp)(FIXED_1DP(gClipHeight, 0) / 2 - FIXED_1DP(7, 0));
            gfx_draw_string_left(dpi, STR_UNIT1DP_NO_SUFFIX, &clipHeightValueInUnits, w->colours[0], x, y); // Printing the value in Height Units.
        }
        else {
            // Height label is Real Values.
            // Print the value in the configured measurement units.
            switch (gConfigGeneral.measurement_format) {
            case MEASUREMENT_FORMAT_METRIC:
            case MEASUREMENT_FORMAT_SI:
                clipHeightValueInMeters = (fixed32_2dp)(FIXED_2DP(gClipHeight, 0) / 2 * 1.5f - FIXED_2DP(10, 50));
                gfx_draw_string_left(dpi, STR_UNIT2DP_SUFFIX_METRES, &clipHeightValueInMeters, w->colours[0], x, y);
                break;
            case MEASUREMENT_FORMAT_IMPERIAL:
            default:
                clipHeightValueInFeet = (fixed16_1dp)(FIXED_1DP(gClipHeight, 0) / 2.0f * 5 - FIXED_1DP(35, 0));
                gfx_draw_string_left(dpi, STR_UNIT1DP_SUFFIX_FEET, &clipHeightValueInFeet, w->colours[0], x, y);
                break;
            }
        }
    }
}

static void window_view_clipping_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
    *width = 1000;
}
