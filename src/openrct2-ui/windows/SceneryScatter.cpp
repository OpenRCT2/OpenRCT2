/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/world/Scenery.h>

enum WINDOW_SCENERY_SCATTER_WIDGET_IDX
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PREVIEW,
    WIDX_DECREMENT,
    WIDX_INCREMENT,
    WIDX_DENSITY,
    WIDX_DENSITY_LOW,
    WIDX_DENSITY_MEDIUM,
    WIDX_DENSITY_HIGH
};

bool gWindowSceneryScatterEnabled = false;
uint16_t gWindowSceneryScatterSize;
ScatterToolDensity gWindowSceneryScatterDensity;

// clang-format off
static rct_widget window_scenery_scatter_widgets[] = {
    MakeWidget     ({ 0,  0}, {86, 100}, WindowWidgetType::Frame,    WindowColour::Secondary                                                                ), // panel / background
    MakeWidget     ({ 1,  1}, {84,  14}, WindowWidgetType::Caption,  WindowColour::Primary  , STR_SCENERY_SCATTER,           STR_WINDOW_TITLE_TIP           ), // title bar
    MakeWidget     ({73,  2}, {11,  12}, WindowWidgetType::CloseBox, WindowColour::Primary  , STR_CLOSE_X,                   STR_CLOSE_WINDOW_TIP           ), // close x button

    MakeWidget     ({20, 17}, {44,  32}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_LAND_TOOL_SIZE_0                                          ), // preview box
    MakeRemapWidget({21, 18}, {16,  16}, WindowWidgetType::TrnBtn,   WindowColour::Secondary, SPR_LAND_TOOL_DECREASE,        STR_ADJUST_SMALLER_LAND_TIP    ), // decrement size
    MakeRemapWidget({47, 32}, {16,  16}, WindowWidgetType::TrnBtn,   WindowColour::Secondary, SPR_LAND_TOOL_INCREASE,        STR_ADJUST_LARGER_LAND_TIP     ), // increment size

    MakeWidget     ({ 3, 55}, {80,  42}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_SCATTER_TOOL_DENSITY                                      ),
    MakeRemapWidget({ 7, 68}, {24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_G2_SCENERY_SCATTER_LOW,    STR_SCATTER_TOOL_DENSITY_LOW   ), // low amount
    MakeRemapWidget({31, 68}, {24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_G2_SCENERY_SCATTER_MEDIUM, STR_SCATTER_TOOL_DENSITY_MEDIUM), // medium amount
    MakeRemapWidget({55, 68}, {24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_G2_SCENERY_SCATTER_HIGH,   STR_SCATTER_TOOL_DENSITY_HIGH  ), // high amount
    { WIDGETS_END },
};
// clang-format on

static void window_scenery_scatter_close(rct_window* w);
static void window_scenery_scatter_mouseup(rct_window* w, rct_widgetindex widgetIndex);
static void window_scenery_scatter_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_scenery_scatter_invalidate(rct_window* w);
static void window_scenery_scatter_paint(rct_window* w, rct_drawpixelinfo* dpi);
static void window_scenery_scatter_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text);
static void window_scenery_scatter_inputsize(rct_window* w, rct_widgetindex widgetindex);

// clang-format off
static rct_window_event_list window_clear_scenery_events([](auto& events)
{
    events.close = &window_scenery_scatter_close;
    events.mouse_up = &window_scenery_scatter_mouseup;
    events.mouse_down = &window_scenery_scatter_mousedown;
    events.text_input = &window_scenery_scatter_textinput;
    events.invalidate = &window_scenery_scatter_invalidate;
    events.paint = &window_scenery_scatter_paint;
});
// clang-format on

rct_window* window_scenery_scatter_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_find_by_class(WC_SCENERY_SCATTER);
    if (window != nullptr)
        return window;

    window = WindowCreateAutoPos(86, 100, &window_clear_scenery_events, WC_SCENERY_SCATTER, 0);

    window->widgets = window_scenery_scatter_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_INCREMENT) | (1 << WIDX_DECREMENT) | (1 << WIDX_PREVIEW)
        | (1 << WIDX_DENSITY_LOW) | (1 << WIDX_DENSITY_MEDIUM) | (1 << WIDX_DENSITY_HIGH);
    window->hold_down_widgets = (1 << WIDX_INCREMENT) | (1 << WIDX_DECREMENT);
    WindowInitScrollWidgets(window);
    window_push_others_below(window);

    gWindowSceneryScatterEnabled = true;
    gWindowSceneryScatterSize = 16;
    gWindowSceneryScatterDensity = ScatterToolDensity::MediumDensity;

    return window;
}

static void window_scenery_scatter_close([[maybe_unused]] rct_window* w)
{
    gWindowSceneryScatterEnabled = false;
}

static void window_scenery_scatter_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;

        case WIDX_PREVIEW:
            window_scenery_scatter_inputsize(w, widgetIndex);
            break;

        case WIDX_DENSITY_LOW:
            gWindowSceneryScatterDensity = ScatterToolDensity::LowDensity;
            break;

        case WIDX_DENSITY_MEDIUM:
            gWindowSceneryScatterDensity = ScatterToolDensity::MediumDensity;
            break;

        case WIDX_DENSITY_HIGH:
            gWindowSceneryScatterDensity = ScatterToolDensity::HighDensity;
            break;
    }
}

static void window_scenery_scatter_mousedown(rct_window* w, rct_widgetindex widgetIndex, [[maybe_unused]] rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_DECREMENT:
            // Decrement land tool size, if it stays within the limit
            gWindowSceneryScatterSize = std::max(MINIMUM_TOOL_SIZE, gWindowSceneryScatterSize - 1);
            w->Invalidate();
            break;

        case WIDX_INCREMENT:
            // Increment land tool size, if it stays within the limit
            gWindowSceneryScatterSize = std::min(MAXIMUM_TOOL_SIZE, gWindowSceneryScatterSize + 1);
            w->Invalidate();
            break;
    }
}

static void window_scenery_scatter_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    int32_t size;
    char* end;

    if (widgetIndex != WIDX_PREVIEW || text == nullptr)
        return;

    size = strtol(text, &end, 10);
    if (*end == '\0')
    {
        switch (widgetIndex)
        {
            case WIDX_PREVIEW:
                size = std::max(MINIMUM_TOOL_SIZE, size);
                size = std::min(MAXIMUM_TOOL_SIZE, size);
                gWindowSceneryScatterSize = size;
                break;
        }
        w->Invalidate();
    }
}

static void window_scenery_scatter_inputsize(rct_window* w, rct_widgetindex widgetindex)
{
    uint8_t maxlen = 0;
    switch (widgetindex)
    {
        case WIDX_PREVIEW:
            TextInputDescriptionArgs[0] = MINIMUM_TOOL_SIZE;
            TextInputDescriptionArgs[1] = MAXIMUM_TOOL_SIZE;
            maxlen = 3;
            break;
    }
    window_text_input_open(w, widgetindex, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, STR_NONE, STR_NONE, maxlen);
}

static void window_scenery_scatter_invalidate(rct_window* w)
{
    // Set the preview image button to be pressed down
    w->pressed_widgets = (1 << WIDX_PREVIEW);

    // Set density buttons' pressed state.
    switch (gWindowSceneryScatterDensity)
    {
        case ScatterToolDensity::LowDensity:
            w->pressed_widgets |= (1 << WIDX_DENSITY_LOW);
            break;

        case ScatterToolDensity::MediumDensity:
            w->pressed_widgets |= (1 << WIDX_DENSITY_MEDIUM);
            break;

        case ScatterToolDensity::HighDensity:
            w->pressed_widgets |= (1 << WIDX_DENSITY_HIGH);
            break;
    }

    // Update the preview image (for tool sizes up to 7)
    window_scenery_scatter_widgets[WIDX_PREVIEW].image = LandTool::SizeToSpriteIndex(gWindowSceneryScatterSize);
}

static void window_scenery_scatter_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    // Draw area as a number for tool sizes bigger than 7
    if (gWindowSceneryScatterSize > MAX_TOOL_SIZE_WITH_SPRITE)
    {
        auto preview = window_scenery_scatter_widgets[WIDX_PREVIEW];
        auto screenCoords = ScreenCoordsXY{ w->windowPos.x + preview.midX(), w->windowPos.y + preview.midY() };
        DrawTextBasic(
            dpi, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, &gWindowSceneryScatterSize,
            { TextAlignment::CENTRE });
    }
}
