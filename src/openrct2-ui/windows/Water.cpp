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
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/world/Park.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_WATER;
static constexpr const int32_t WH = 77;
static constexpr const int32_t WW = 76;

// clang-format off
enum WINDOW_WATER_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PREVIEW,
    WIDX_DECREMENT,
    WIDX_INCREMENT
};

static rct_widget window_water_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget     ({16, 17}, {44, 32}, WWT_IMGBTN, WindowColour::Primary , SPR_LAND_TOOL_SIZE_0,   STR_NONE),                     // preview box
    MakeRemapWidget({17, 18}, {16, 16}, WWT_TRNBTN, WindowColour::Tertiary, SPR_LAND_TOOL_DECREASE, STR_ADJUST_SMALLER_WATER_TIP), // decrement size
    MakeRemapWidget({43, 32}, {16, 16}, WWT_TRNBTN, WindowColour::Tertiary, SPR_LAND_TOOL_INCREASE, STR_ADJUST_LARGER_WATER_TIP),  // increment size
    { WIDGETS_END },
};

static void window_water_close(rct_window *w);
static void window_water_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_water_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_water_update(rct_window *w);
static void window_water_invalidate(rct_window *w);
static void window_water_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_water_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_water_inputsize(rct_window *w);

static rct_window_event_list window_water_events = {
    window_water_close,
    window_water_mouseup,
    nullptr,
    window_water_mousedown,
    nullptr,
    nullptr,
    window_water_update,
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
    window_water_textinput,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_water_invalidate,
    window_water_paint,
    nullptr
};
// clang-format on

/**
 *
 *  rct2: 0x006E6A40
 */
rct_window* window_water_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_find_by_class(WC_WATER);
    if (window != nullptr)
        return window;

    window = window_create(ScreenCoordsXY(context_get_width() - 76, 29), 76, 77, &window_water_events, WC_WATER, 0);
    window->widgets = window_water_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_DECREMENT) | (1 << WIDX_INCREMENT) | (1 << WIDX_PREVIEW);
    window->hold_down_widgets = (1 << WIDX_INCREMENT) | (1 << WIDX_DECREMENT);
    window_init_scroll_widgets(window);
    window_push_others_below(window);

    gLandToolSize = 1;
    gWaterToolRaiseCost = MONEY32_UNDEFINED;
    gWaterToolLowerCost = MONEY32_UNDEFINED;

    return window;
}

/**
 *
 *  rct2: 0x006E6B65
 */
static void window_water_close(rct_window* w)
{
    // If the tool wasn't changed, turn tool off
    if (water_tool_is_active())
        tool_cancel();
}

/**
 *
 *  rct2: 0x006E6B4E
 */
static void window_water_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_PREVIEW:
            window_water_inputsize(w);
            break;
    }
}

static void window_water_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_DECREMENT:
            // Decrement land tool size
            gLandToolSize = std::max(MINIMUM_TOOL_SIZE, gLandToolSize - 1);

            // Invalidate the window
            w->Invalidate();
            break;
        case WIDX_INCREMENT:
            // Increment land tool size
            gLandToolSize = std::min(MAXIMUM_TOOL_SIZE, gLandToolSize + 1);

            // Invalidate the window
            w->Invalidate();
            break;
    }
}

static void window_water_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    int32_t size;
    char* end;

    if (widgetIndex != WIDX_PREVIEW || text == nullptr)
        return;

    size = strtol(text, &end, 10);
    if (*end == '\0')
    {
        size = std::max(MINIMUM_TOOL_SIZE, size);
        size = std::min(MAXIMUM_TOOL_SIZE, size);
        gLandToolSize = size;

        w->Invalidate();
    }
}

static void window_water_inputsize(rct_window* w)
{
    TextInputDescriptionArgs[0] = MINIMUM_TOOL_SIZE;
    TextInputDescriptionArgs[1] = MAXIMUM_TOOL_SIZE;
    window_text_input_open(w, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, STR_NONE, STR_NONE, 3);
}

/**
 *
 *  rct2: 0x006E6BCE
 */
static void window_water_update(rct_window* w)
{
    // Close window if another tool is open
    if (!water_tool_is_active())
        window_close(w);
}

/**
 *
 *  rct2: 0x006E6AB8
 */
static void window_water_invalidate(rct_window* w)
{
    // Set the preview image button to be pressed down
    w->pressed_widgets |= (1 << WIDX_PREVIEW);

    // Update the preview image
    window_water_widgets[WIDX_PREVIEW].image = land_tool_size_to_sprite_index(gLandToolSize);
}

/**
 *
 *  rct2: 0x006E6ACF
 */
static void window_water_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    auto screenCoords = ScreenCoordsXY{ w->windowPos.x + window_water_widgets[WIDX_PREVIEW].midX(),
                                        w->windowPos.y + window_water_widgets[WIDX_PREVIEW].midY() };

    window_draw_widgets(w, dpi);
    // Draw number for tool sizes bigger than 7
    if (gLandToolSize > MAX_TOOL_SIZE_WITH_SPRITE)
    {
        gfx_draw_string_centred(
            dpi, STR_LAND_TOOL_SIZE_VALUE, screenCoords - ScreenCoordsXY{ 0, 2 }, COLOUR_BLACK, &gLandToolSize);
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        // Draw raise cost amount
        screenCoords = { window_water_widgets[WIDX_PREVIEW].midX() + w->windowPos.x,
                         window_water_widgets[WIDX_PREVIEW].bottom + w->windowPos.y + 5 };
        if (gWaterToolRaiseCost != MONEY32_UNDEFINED && gWaterToolRaiseCost != 0)
            gfx_draw_string_centred(dpi, STR_RAISE_COST_AMOUNT, screenCoords, COLOUR_BLACK, &gWaterToolRaiseCost);
        screenCoords.y += 10;

        // Draw lower cost amount
        if (gWaterToolLowerCost != MONEY32_UNDEFINED && gWaterToolLowerCost != 0)
            gfx_draw_string_centred(dpi, STR_LOWER_COST_AMOUNT, screenCoords, COLOUR_BLACK, &gWaterToolLowerCost);
    }
}
