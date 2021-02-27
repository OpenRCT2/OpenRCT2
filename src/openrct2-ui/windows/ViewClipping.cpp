/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
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

static constexpr const rct_string_id WINDOW_TITLE = STR_VIEW_CLIPPING_TITLE;
static constexpr const int32_t WW = 180;
static constexpr const int32_t WH = 155;

static rct_widget window_view_clipping_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget        ({     11,  19}, {    159,  11}, WindowWidgetType::Checkbox, WindowColour::Primary, STR_VIEW_CLIPPING_HEIGHT_ENABLE,       STR_VIEW_CLIPPING_HEIGHT_ENABLE_TIP  ), // clip enable/disable check box
    MakeWidget        ({      5,  36}, {WW - 10,  48}, WindowWidgetType::Groupbox, WindowColour::Primary, STR_VIEW_CLIPPING_VERTICAL_CLIPPING                                         ),
    MakeSpinnerWidgets({     90,  51}, {     79,  12}, WindowWidgetType::Spinner,  WindowColour::Primary, STR_NONE,                              STR_VIEW_CLIPPING_HEIGHT_VALUE_TOGGLE), // clip height (3 widgets)
    MakeWidget        ({     11,  66}, {    158,  13}, WindowWidgetType::Scroll,   WindowColour::Primary, SCROLL_HORIZONTAL,                     STR_VIEW_CLIPPING_HEIGHT_SCROLL_TIP  ), // clip height scrollbar
    MakeWidget        ({      5,  90}, {WW - 10,  60}, WindowWidgetType::Groupbox, WindowColour::Primary, STR_VIEW_CLIPPING_HORIZONTAL_CLIPPING                                       ),
    MakeWidget        ({     11, 105}, {    158,  17}, WindowWidgetType::Button,   WindowColour::Primary, STR_VIEW_CLIPPING_SELECT_AREA                                               ), // selector
    MakeWidget        ({     11, 126}, {    158,  18}, WindowWidgetType::Button,   WindowColour::Primary, STR_VIEW_CLIPPING_CLEAR_SELECTION                                           ), // clear

    { WIDGETS_END }
};

#pragma endregion

#pragma region Members

static CoordsXY _selectionStart;
static CoordsXY _previousClipSelectionA;
static CoordsXY _previousClipSelectionB;
static bool _toolActive;
static bool _dragging;

#pragma endregion

#pragma region Events

static void window_view_clipping_close_button(rct_window* w);
static void window_view_clipping_mouseup(rct_window* w, rct_widgetindex widgetIndex);
static void window_view_clipping_mousedown(rct_window*w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_view_clipping_update(rct_window* w);
static void window_view_clipping_tool_update(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_view_clipping_tool_down(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_view_clipping_tool_drag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_view_clipping_tool_up(rct_window* w, rct_widgetindex, const ScreenCoordsXY&);
static void window_view_clipping_invalidate(rct_window* w);
static void window_view_clipping_paint(rct_window* w, rct_drawpixelinfo* dpi);
static void window_view_clipping_scrollgetsize(rct_window* w, int scrollIndex, int* width, int* height);
static void window_view_clipping_close();

static rct_window_event_list window_view_clipping_events([](auto& events)
{
    events.close = &window_view_clipping_close_button;
    events.mouse_up = &window_view_clipping_mouseup;
    events.mouse_down = &window_view_clipping_mousedown;
    events.update = &window_view_clipping_update;
    events.tool_update = &window_view_clipping_tool_update;
    events.tool_down = &window_view_clipping_tool_down;
    events.tool_drag = &window_view_clipping_tool_drag;
    events.tool_up = &window_view_clipping_tool_up;
    events.get_scroll_size = &window_view_clipping_scrollgetsize;
    events.invalidate = &window_view_clipping_invalidate;
    events.paint = &window_view_clipping_paint;
});
// clang-format on

#pragma endregion

static void window_view_clipping_set_clipheight(rct_window* w, const uint8_t clipheight)
{
    gClipHeight = clipheight;
    rct_widget* widget = &window_view_clipping_widgets[WIDX_CLIP_HEIGHT_SLIDER];
    const float clip_height_ratio = static_cast<float>(gClipHeight) / 255;
    w->scrolls[0].h_left = static_cast<int16_t>(std::ceil(clip_height_ratio * (w->scrolls[0].h_right - (widget->width() - 1))));
}

rct_window* window_view_clipping_open()
{
    rct_window* window;

    // Get the main viewport to set the view clipping flag.
    rct_window* mainWindow = window_get_main();

    // Check if window is already open
    window = window_find_by_class(WC_VIEW_CLIPPING);
    if (window != nullptr)
    {
        return window;
    }

    // Window is not open - create it.
    window = WindowCreate(ScreenCoordsXY(32, 32), WW, WH, &window_view_clipping_events, WC_VIEW_CLIPPING, 0);
    window->widgets = window_view_clipping_widgets;
    window->enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_CLIP_CHECKBOX_ENABLE) | (1ULL << WIDX_CLIP_HEIGHT_VALUE)
        | (1ULL << WIDX_CLIP_HEIGHT_INCREASE) | (1ULL << WIDX_CLIP_HEIGHT_DECREASE) | (1ULL << WIDX_CLIP_HEIGHT_SLIDER)
        | (1ULL << WIDX_CLIP_SELECTOR) | (1ULL << WIDX_CLIP_CLEAR);
    window->hold_down_widgets = (1ULL << WIDX_CLIP_HEIGHT_INCREASE) | (1UL << WIDX_CLIP_HEIGHT_DECREASE);

    WindowInitScrollWidgets(window);

    // Initialise the clip height slider from the current clip height value.
    window_view_clipping_set_clipheight(window, gClipHeight);

    window_push_others_below(window);

    // Turn on view clipping when the window is opened.
    if (mainWindow != nullptr)
    {
        mainWindow->viewport->flags |= VIEWPORT_FLAG_CLIP_VIEW;
        mainWindow->Invalidate();
    }

    _toolActive = false;
    _dragging = false;

    return window;
}

static void window_view_clipping_close()
{
    // Turn off view clipping when the window is closed.
    rct_window* mainWindow = window_get_main();
    if (mainWindow != nullptr)
    {
        mainWindow->viewport->flags &= ~VIEWPORT_FLAG_CLIP_VIEW;
        mainWindow->Invalidate();
    }
}

static void window_view_clipping_close_button(rct_window* w)
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

static void window_view_clipping_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    rct_window* mainWindow;

    // mouseup appears to be used for buttons, checkboxes
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_CLIP_CHECKBOX_ENABLE:
            // Toggle height clipping.
            mainWindow = window_get_main();
            if (mainWindow != nullptr)
            {
                mainWindow->viewport->flags ^= VIEWPORT_FLAG_CLIP_VIEW;
                mainWindow->Invalidate();
            }
            w->Invalidate();
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
            w->Invalidate();
            break;
        case WIDX_CLIP_SELECTOR:
            // Activate the selection tool
            tool_set(w, WIDX_BACKGROUND, Tool::Crosshair);
            _toolActive = true;
            _dragging = false;

            // Reset clip selection to show all tiles
            _previousClipSelectionA = gClipSelectionA;
            _previousClipSelectionB = gClipSelectionB;
            gClipSelectionA = { 0, 0 };
            gClipSelectionB = { MAXIMUM_MAP_SIZE_BIG - 1, MAXIMUM_MAP_SIZE_BIG - 1 };
            gfx_invalidate_screen();
            break;
        case WIDX_CLIP_CLEAR:
            if (window_view_clipping_tool_is_active())
            {
                _toolActive = false;
                tool_cancel();
            }
            gClipSelectionA = { 0, 0 };
            gClipSelectionB = { MAXIMUM_MAP_SIZE_BIG - 1, MAXIMUM_MAP_SIZE_BIG - 1 };
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
                mainWindow->Invalidate();
            break;
        case WIDX_CLIP_HEIGHT_DECREASE:
            if (gClipHeight > 0)
                window_view_clipping_set_clipheight(w, gClipHeight - 1);
            mainWindow = window_get_main();
            if (mainWindow != nullptr)
                mainWindow->Invalidate();
            break;
    }
}

static void window_view_clipping_update(rct_window* w)
{
    const rct_widget* const widget = &window_view_clipping_widgets[WIDX_CLIP_HEIGHT_SLIDER];
    const rct_scroll* const scroll = &w->scrolls[0];
    const int16_t scroll_width = widget->width() - 1;
    const uint8_t clip_height = static_cast<uint8_t>(
        (static_cast<float>(scroll->h_left) / (scroll->h_right - scroll_width)) * 255);
    if (clip_height != gClipHeight)
    {
        gClipHeight = clip_height;

        // Update the main window accordingly.
        rct_window* mainWindow = window_get_main();
        if (mainWindow != nullptr)
        {
            mainWindow->Invalidate();
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

static void window_view_clipping_tool_update(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (_dragging)
    {
        return;
    }

    int32_t direction;
    auto mapCoords = screen_pos_to_map_pos(screenCoords, &direction);
    if (mapCoords)
    {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        map_invalidate_tile_full(gMapSelectPositionA);
        gMapSelectPositionA = gMapSelectPositionB = *mapCoords;
        map_invalidate_tile_full(*mapCoords);
        gMapSelectType = MAP_SELECT_TYPE_FULL;
    }
}

static void window_view_clipping_tool_down(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t direction;
    auto mapCoords = screen_pos_to_map_pos(screenCoords, &direction);
    if (mapCoords)
    {
        _dragging = true;
        _selectionStart = *mapCoords;
    }
}

static void window_view_clipping_tool_drag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (!_dragging)
    {
        return;
    }

    int32_t direction;
    auto mapCoords = screen_pos_to_map_pos(screenCoords, &direction);
    if (mapCoords)
    {
        map_invalidate_selection_rect();
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        gMapSelectPositionA.x = std::min(_selectionStart.x, mapCoords->x);
        gMapSelectPositionB.x = std::max(_selectionStart.x, mapCoords->x);
        gMapSelectPositionA.y = std::min(_selectionStart.y, mapCoords->y);
        gMapSelectPositionB.y = std::max(_selectionStart.y, mapCoords->y);
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        map_invalidate_selection_rect();
    }
}

static void window_view_clipping_tool_up(struct rct_window*, rct_widgetindex, const ScreenCoordsXY&)
{
    gClipSelectionA = gMapSelectPositionA;
    gClipSelectionB = gMapSelectPositionB;
    _toolActive = false;
    tool_cancel();
    gfx_invalidate_screen();
}

static void window_view_clipping_invalidate(rct_window* w)
{
    WidgetScrollUpdateThumbs(w, WIDX_CLIP_HEIGHT_SLIDER);

    rct_window* mainWindow = window_get_main();
    if (mainWindow != nullptr)
    {
        WidgetSetCheckboxValue(w, WIDX_CLIP_CHECKBOX_ENABLE, mainWindow->viewport->flags & VIEWPORT_FLAG_CLIP_VIEW);
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

static void window_view_clipping_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    // Clip height value
    auto screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_CLIP_HEIGHT_VALUE].top };
    DrawTextBasic(dpi, screenCoords, STR_VIEW_CLIPPING_HEIGHT_VALUE, {}, { w->colours[0] });

    screenCoords = w->windowPos
        + ScreenCoordsXY{ w->widgets[WIDX_CLIP_HEIGHT_VALUE].left + 1, w->widgets[WIDX_CLIP_HEIGHT_VALUE].top };

    fixed16_1dp clipHeightValueInUnits;
    fixed32_2dp clipHeightValueInMeters;
    fixed16_1dp clipHeightValueInFeet;
    int32_t clipHeightRawValue = static_cast<int32_t>(gClipHeight);
    switch (gClipHeightDisplayType)
    {
        case DISPLAY_TYPE::DISPLAY_RAW:
        default:
            DrawTextBasic(
                dpi, screenCoords, STR_FORMAT_INTEGER, &clipHeightRawValue, { w->colours[0] }); // Printing the raw value.
            break;

        case DISPLAY_TYPE::DISPLAY_UNITS:
            // Print the value in the configured height label type:
            if (gConfigGeneral.show_height_as_units == 1)
            {
                // Height label is Units.
                clipHeightValueInUnits = static_cast<fixed16_1dp>(FIXED_1DP(gClipHeight, 0) / 2 - FIXED_1DP(7, 0));
                DrawTextBasic(
                    dpi, screenCoords, STR_UNIT1DP_NO_SUFFIX, &clipHeightValueInUnits,
                    { w->colours[0] }); // Printing the value in Height Units.
            }
            else
            {
                // Height label is Real Values.
                // Print the value in the configured measurement units.
                switch (gConfigGeneral.measurement_format)
                {
                    case MeasurementFormat::Metric:
                    case MeasurementFormat::SI:
                        clipHeightValueInMeters = static_cast<fixed32_2dp>(
                            FIXED_2DP(gClipHeight, 0) / 2 * 1.5f - FIXED_2DP(10, 50));
                        DrawTextBasic(
                            dpi, screenCoords, STR_UNIT2DP_SUFFIX_METRES, &clipHeightValueInMeters, { w->colours[0] });
                        break;
                    case MeasurementFormat::Imperial:
                        clipHeightValueInFeet = static_cast<fixed16_1dp>(
                            FIXED_1DP(gClipHeight, 0) / 2.0f * 5 - FIXED_1DP(35, 0));
                        DrawTextBasic(dpi, screenCoords, STR_UNIT1DP_SUFFIX_FEET, &clipHeightValueInFeet, { w->colours[0] });
                        break;
                }
            }
    }
}

static void window_view_clipping_scrollgetsize(rct_window* w, int scrollIndex, int* width, int* height)
{
    *width = 1000;
}
