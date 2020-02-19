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

#define MAX_AMOUNT 100

enum WINDOW_CLEAR_SCENERY_WIDGET_IDX
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PREVIEW,
    WIDX_DECREMENT,
    WIDX_INCREMENT,
    WIDX_AMOUNT,
    WIDX_ADECREMENT,
    WIDX_AINCREMENT
};

// clang-format off
static rct_widget window_scenery_scatter_widgets[] = {
    { WWT_FRAME,    1,  0,  70, 0,  113, 0xFFFFFFFF,                                STR_NONE },                    // panel / background
    { WWT_CAPTION,  0,  0,  57, 1,  14,  STR_SCENERY_SCATTER,                       STR_WINDOW_TITLE_TIP },        // title bar
    { WWT_CLOSEBOX, 0,  57, 68, 2,  13,  STR_CLOSE_X,                               STR_CLOSE_WINDOW_TIP },        // close x button

    { WWT_IMGBTN,   1,  5, 48, 17, 48,   SPR_LAND_TOOL_SIZE_0,                      STR_NONE },                    // preview box
    { WWT_TRNBTN,   1,  6, 23, 18, 33,   IMAGE_TYPE_REMAP | SPR_LAND_TOOL_DECREASE, STR_ADJUST_SMALLER_LAND_TIP }, // decrement size
    { WWT_TRNBTN,   1,  32, 49, 32, 47,  IMAGE_TYPE_REMAP | SPR_LAND_TOOL_INCREASE, STR_ADJUST_LARGER_LAND_TIP },  // increment size

    { WWT_IMGBTN,   1,  5, 48, 65, 96,   0xFFFFFFFF,                                STR_NONE },                    // preview box
    { WWT_TRNBTN,   1,  6, 23, 66, 81,   IMAGE_TYPE_REMAP | SPR_LAND_TOOL_DECREASE, STR_ADJUST_SMALLER_LAND_TIP }, // decrement size amount
    { WWT_TRNBTN,   1,  32, 49, 80, 94,  IMAGE_TYPE_REMAP | SPR_LAND_TOOL_INCREASE, STR_ADJUST_LARGER_LAND_TIP },  // increment size amount
    { WIDGETS_END },
};
// clang-format on

static void window_scenery_scatter_close(rct_window* w);
static void window_scenery_scatter_mouseup(rct_window* w, rct_widgetindex widgetIndex);
static void window_scenery_scatter_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_scenery_scatter_update(rct_window* w);
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
    window_scenery_scatter_update,
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

    window = window_create(
        ScreenCoordsXY(context_get_width() - 70, 29), 70, 114, &window_clear_scenery_events, WC_SCENERY_SCATTER, 0);
    window->widgets = window_scenery_scatter_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_INCREMENT) | (1 << WIDX_DECREMENT) | (1 << WIDX_PREVIEW)
        | (1 << WIDX_AINCREMENT) | (1 << WIDX_ADECREMENT) | (1 << WIDX_AMOUNT);
    window->hold_down_widgets = (1 << WIDX_INCREMENT) | (1 << WIDX_DECREMENT) | (1 << WIDX_AINCREMENT) | (1 << WIDX_ADECREMENT);
    window_init_scroll_widgets(window);
    window_push_others_below(window);

    return window;
}

static void window_scenery_scatter_close([[maybe_unused]] rct_window* w)
{
}

static void window_scenery_scatter_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            gWindowSceneryScatterEnabled = 0;
            window_close(w);
            break;
        case WIDX_PREVIEW:
        case WIDX_AMOUNT:
            window_scenery_scatter_inputsize(w, widgetIndex);
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

            // Invalidate the window
            w->Invalidate();
            break;
        case WIDX_INCREMENT:
            // Increment land tool size, if it stays within the limit
            gWindowSceneryScatterSize = std::min(MAXIMUM_TOOL_SIZE, gWindowSceneryScatterSize + 1);

            // Invalidate the window
            w->Invalidate();
            break;

        case WIDX_ADECREMENT:
            // Decrement land tool size, if it stays within the limit
            gWindowSceneryScatterAmount = std::max(1, gWindowSceneryScatterAmount - 1);

            // Invalidate the window
            w->Invalidate();
            break;
        case WIDX_AINCREMENT:
            // Increment land tool size, if it stays within the limit
            gWindowSceneryScatterAmount = std::min(MAX_AMOUNT, gWindowSceneryScatterAmount + 1);

            // Invalidate the window
            w->Invalidate();
            break;
    }
}

static void window_scenery_scatter_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    int32_t size;
    char* end;

    if (!(widgetIndex == WIDX_PREVIEW || widgetIndex == WIDX_AMOUNT) || text == nullptr)
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
            case WIDX_AMOUNT:
                size = std::max(1, size);
                size = std::min(MAX_AMOUNT, size);
                gWindowSceneryScatterAmount = size;
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
        case WIDX_AMOUNT:
            TextInputDescriptionArgs[0] = 1;
            TextInputDescriptionArgs[1] = MAX_AMOUNT;
            maxlen = 4; // Catch 100
            break;
    }
    window_text_input_open(w, widgetindex, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, STR_NONE, STR_NONE, maxlen);
}

static void window_scenery_scatter_update(rct_window* w)
{
    w->frame_no++;
}

static void window_scenery_scatter_invalidate(rct_window* w)
{
    // Set the preview image button to be pressed down
    w->pressed_widgets = (1 << WIDX_PREVIEW) | (1 << WIDX_AMOUNT);
    // Update the preview image (for tool sizes up to 7)
    window_scenery_scatter_widgets[WIDX_PREVIEW].image = land_tool_size_to_sprite_index(gWindowSceneryScatterSize);
}

static void window_scenery_scatter_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);

    int32_t x, y;
    x = w->x + window_scenery_scatter_widgets[WIDX_PREVIEW].left;
    y = w->y + window_scenery_scatter_widgets[WIDX_PREVIEW].bottom + 2;
    gfx_draw_string_left(dpi, STR_SMALL_SIZE, nullptr, COLOUR_BLACK, x, y);

    x = w->x + window_scenery_scatter_widgets[WIDX_AMOUNT].left;
    y = w->y + window_scenery_scatter_widgets[WIDX_AMOUNT].bottom + 2;
    gfx_draw_string_left(dpi, STR_SMALL_AMOUNT, nullptr, COLOUR_BLACK, x, y);

    // Draw number for tool sizes bigger than 7

    if (gWindowSceneryScatterSize > MAX_TOOL_SIZE_WITH_SPRITE)
    {
        x = w->x + (window_scenery_scatter_widgets[WIDX_PREVIEW].left + window_scenery_scatter_widgets[WIDX_PREVIEW].right) / 2;
        y = w->y + (window_scenery_scatter_widgets[WIDX_PREVIEW].top + window_scenery_scatter_widgets[WIDX_PREVIEW].bottom) / 2;
        gfx_draw_string_centred(dpi, STR_LAND_TOOL_SIZE_VALUE, x, y, COLOUR_BLACK, &gWindowSceneryScatterSize);
    }

    x = w->x + (window_scenery_scatter_widgets[WIDX_AMOUNT].left + window_scenery_scatter_widgets[WIDX_AMOUNT].right) / 2;
    y = w->y + (window_scenery_scatter_widgets[WIDX_AMOUNT].top + window_scenery_scatter_widgets[WIDX_AMOUNT].bottom) / 2;
    gfx_draw_string_centred(dpi, STR_LAND_TOOL_SIZE_VALUE, x, y, COLOUR_BLACK, &gWindowSceneryScatterAmount);
}
