/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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

enum WINDOW_CLEAR_SCENERY_WIDGET_IDX
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
    { WWT_FRAME,    1,  0,  85, 0,  99, 0xFFFFFFFF,         STR_NONE },                     // panel / background
    { WWT_CAPTION,  0,  1,  84, 1,  14, STR_SCENERY_SCATTER, STR_WINDOW_TITLE_TIP },        // title bar
    { WWT_CLOSEBOX, 0,  73, 83, 2,  13, STR_CLOSE_X,         STR_CLOSE_WINDOW_TIP },        // close x button

    { WWT_IMGBTN,   1,  20, 63, 17, 48, SPR_LAND_TOOL_SIZE_0,                      STR_NONE },                         // preview box
    { WWT_TRNBTN,   1,  21, 36, 18, 33, IMAGE_TYPE_REMAP | SPR_LAND_TOOL_DECREASE, STR_ADJUST_SMALLER_LAND_TIP },      // decrement size
    { WWT_TRNBTN,   1,  47, 62, 32, 47, IMAGE_TYPE_REMAP | SPR_LAND_TOOL_INCREASE, STR_ADJUST_LARGER_LAND_TIP },       // increment size

    { WWT_GROUPBOX, 1,  3,  82, 55, 96, STR_SCATTER_TOOL_DENSITY,                         STR_NONE },
    { WWT_FLATBTN,  1,  7,  30, 68, 91, IMAGE_TYPE_REMAP | SPR_G2_SCENERY_SCATTER_LOW,    STR_SCATTER_TOOL_DENSITY_LOW },     // low amount
    { WWT_FLATBTN,  1,  31, 54, 68, 91, IMAGE_TYPE_REMAP | SPR_G2_SCENERY_SCATTER_MEDIUM, STR_SCATTER_TOOL_DENSITY_MEDIUM },  // medium amount
    { WWT_FLATBTN,  1,  55, 78, 68, 91, IMAGE_TYPE_REMAP | SPR_G2_SCENERY_SCATTER_HIGH,   STR_SCATTER_TOOL_DENSITY_HIGH },    // high amount
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
static rct_window_event_list window_clear_scenery_events = {
    window_scenery_scatter_close,
    window_scenery_scatter_mouseup,
    nullptr,
    window_scenery_scatter_mousedown,
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
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_scenery_scatter_textinput,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_scenery_scatter_invalidate,
    window_scenery_scatter_paint,
    nullptr
};
// clang-format on

rct_window* window_scenery_scatter_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_find_by_class(WC_SCENERY_SCATTER);
    if (window != nullptr)
        return window;

    window = window_create_auto_pos(86, 100, &window_clear_scenery_events, WC_SCENERY_SCATTER, 0);

    window->widgets = window_scenery_scatter_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_INCREMENT) | (1 << WIDX_DECREMENT) | (1 << WIDX_PREVIEW)
        | (1 << WIDX_DENSITY_LOW) | (1 << WIDX_DENSITY_MEDIUM) | (1 << WIDX_DENSITY_HIGH);
    window->hold_down_widgets = (1 << WIDX_INCREMENT) | (1 << WIDX_DECREMENT);
    window_init_scroll_widgets(window);
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
    window_scenery_scatter_widgets[WIDX_PREVIEW].image = land_tool_size_to_sprite_index(gWindowSceneryScatterSize);
}

static void window_scenery_scatter_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);

    // Draw area as a number for tool sizes bigger than 7
    if (gWindowSceneryScatterSize > MAX_TOOL_SIZE_WITH_SPRITE)
    {
        auto preview = window_scenery_scatter_widgets[WIDX_PREVIEW];
        int32_t x = w->x + (preview.left + preview.right) / 2;
        int32_t y = w->y + (preview.top + preview.bottom) / 2;
        gfx_draw_string_centred(dpi, STR_LAND_TOOL_SIZE_VALUE, x, y - 2, COLOUR_BLACK, &gWindowSceneryScatterSize);
    }
}
