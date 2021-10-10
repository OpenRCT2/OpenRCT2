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
    WIDGETS_END,
};
// clang-format on

static void WindowSceneryScatterClose(rct_window* w);
static void WindowSceneryScatterMouseup(rct_window* w, rct_widgetindex widgetIndex);
static void WindowSceneryScatterMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowSceneryScatterInvalidate(rct_window* w);
static void WindowSceneryScatterPaint(rct_window* w, rct_drawpixelinfo* dpi);
static void WindowSceneryScatterTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text);
static void WindowSceneryScatterInputsize(rct_window* w, rct_widgetindex widgetindex);

// clang-format off
static rct_window_event_list window_clear_scenery_events([](auto& events)
{
    events.close = &WindowSceneryScatterClose;
    events.mouse_up = &WindowSceneryScatterMouseup;
    events.mouse_down = &WindowSceneryScatterMousedown;
    events.text_input = &WindowSceneryScatterTextinput;
    events.invalidate = &WindowSceneryScatterInvalidate;
    events.paint = &WindowSceneryScatterPaint;
});
// clang-format on

class SceneryScatterWindow final : public rct_window
{
private:
public:
    void OnOpen() override
    {
        widgets = window_scenery_scatter_widgets;
        enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_INCREMENT) | (1ULL << WIDX_DECREMENT)
            | (1ULL << WIDX_PREVIEW) | (1ULL << WIDX_DENSITY_LOW) | (1ULL << WIDX_DENSITY_MEDIUM) | (1ULL << WIDX_DENSITY_HIGH);
        hold_down_widgets = (1ULL << WIDX_INCREMENT) | (1ULL << WIDX_DECREMENT);
        WindowInitScrollWidgets(this);
        window_push_others_below(this);

        gWindowSceneryScatterEnabled = true;
        gWindowSceneryScatterSize = 16;
        gWindowSceneryScatterDensity = ScatterToolDensity::MediumDensity;
    }

    void OnClose() override
    {
        gWindowSceneryScatterEnabled = false;
    }
};

rct_window* WindowSceneryScatterOpen()
{
    rct_window* window;

    // Check if window is already open
    window = window_find_by_class(WC_SCENERY_SCATTER);
    if (window == nullptr)
    {
        window = WindowCreate<SceneryScatterWindow>(WC_SCENERY_SCATTER, 86, 100);
    }

    return window;
}

static void WindowSceneryScatterMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;

        case WIDX_PREVIEW:
            WindowSceneryScatterInputsize(w, widgetIndex);
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

static void WindowSceneryScatterMousedown(rct_window* w, rct_widgetindex widgetIndex, [[maybe_unused]] rct_widget* widget)
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

static void WindowSceneryScatterTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
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

static void WindowSceneryScatterInputsize(rct_window* w, rct_widgetindex widgetindex)
{
    uint8_t maxlen = 0;
    Formatter ft;

    switch (widgetindex)
    {
        case WIDX_PREVIEW:
            ft.Add<int16_t>(MINIMUM_TOOL_SIZE);
            ft.Add<int16_t>(MAXIMUM_TOOL_SIZE);
            maxlen = 3;
            break;
    }
    WindowTextInputOpen(w, widgetindex, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, STR_NONE, STR_NONE, maxlen);
}

static void WindowSceneryScatterInvalidate(rct_window* w)
{
    // Set the preview image button to be pressed down
    w->pressed_widgets = (1ULL << WIDX_PREVIEW);

    // Set density buttons' pressed state.
    switch (gWindowSceneryScatterDensity)
    {
        case ScatterToolDensity::LowDensity:
            w->pressed_widgets |= (1ULL << WIDX_DENSITY_LOW);
            break;

        case ScatterToolDensity::MediumDensity:
            w->pressed_widgets |= (1ULL << WIDX_DENSITY_MEDIUM);
            break;

        case ScatterToolDensity::HighDensity:
            w->pressed_widgets |= (1ULL << WIDX_DENSITY_HIGH);
            break;
    }

    // Update the preview image (for tool sizes up to 7)
    window_scenery_scatter_widgets[WIDX_PREVIEW].image = LandTool::SizeToSpriteIndex(gWindowSceneryScatterSize);
}

static void WindowSceneryScatterPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    // Draw area as a number for tool sizes bigger than 7
    if (gWindowSceneryScatterSize > MAX_TOOL_SIZE_WITH_SPRITE)
    {
        auto preview = window_scenery_scatter_widgets[WIDX_PREVIEW];
        auto screenCoords = ScreenCoordsXY{ w->windowPos.x + preview.midX(), w->windowPos.y + preview.midY() };
        auto ft = Formatter();
        ft.Add<uint16_t>(gWindowSceneryScatterSize);
        DrawTextBasic(dpi, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::CENTRE });
    }
}
