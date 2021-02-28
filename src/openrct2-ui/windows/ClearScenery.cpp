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
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>

// clang-format off
enum WINDOW_CLEAR_SCENERY_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PREVIEW,
    WIDX_DECREMENT,
    WIDX_INCREMENT,
    WIDX_SMALL_SCENERY,
    WIDX_LARGE_SCENERY,
    WIDX_FOOTPATH
};

static constexpr const rct_string_id WINDOW_TITLE = STR_CLEAR_SCENERY;
static constexpr const int32_t WW = 98;
static constexpr const int32_t WH = 94;

static rct_widget window_clear_scenery_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget     ({27, 17}, {44, 32}, WindowWidgetType::ImgBtn,  WindowColour::Primary  , SPR_LAND_TOOL_SIZE_0,        STR_NONE),                                   // preview box
    MakeRemapWidget({28, 18}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Secondary, SPR_LAND_TOOL_DECREASE,      STR_ADJUST_SMALLER_LAND_TIP),                // decrement size
    MakeRemapWidget({54, 32}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Secondary, SPR_LAND_TOOL_INCREASE,      STR_ADJUST_LARGER_LAND_TIP),                 // increment size
    MakeRemapWidget({ 7, 53}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_G2_BUTTON_TREES,         STR_CLEAR_SCENERY_REMOVE_SMALL_SCENERY_TIP), // small scenery
    MakeRemapWidget({37, 53}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_G2_BUTTON_LARGE_SCENERY, STR_CLEAR_SCENERY_REMOVE_LARGE_SCENERY_TIP), // large scenery
    MakeRemapWidget({67, 53}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_G2_BUTTON_FOOTPATH,      STR_CLEAR_SCENERY_REMOVE_FOOTPATHS_TIP),     // footpaths
    { WIDGETS_END },
};

static void window_clear_scenery_close(rct_window *w);
static void window_clear_scenery_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_clear_scenery_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_clear_scenery_update(rct_window *w);
static void window_clear_scenery_invalidate(rct_window *w);
static void window_clear_scenery_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_clear_scenery_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_clear_scenery_inputsize(rct_window *w);

static rct_window_event_list window_clear_scenery_events([](auto& events)
{
    events.close = &window_clear_scenery_close;
    events.mouse_up = &window_clear_scenery_mouseup;
    events.mouse_down = &window_clear_scenery_mousedown;
    events.update = &window_clear_scenery_update;
    events.text_input = &window_clear_scenery_textinput;
    events.invalidate = &window_clear_scenery_invalidate;
    events.paint = &window_clear_scenery_paint;
});
// clang-format on

/**
 *
 *  rct2: 0x0068E0A7
 */
rct_window* window_clear_scenery_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_find_by_class(WC_CLEAR_SCENERY);
    if (window != nullptr)
        return window;

    window = WindowCreate(
        ScreenCoordsXY(context_get_width() - WW, 29), WW, WH, &window_clear_scenery_events, WC_CLEAR_SCENERY, 0);
    window->widgets = window_clear_scenery_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_INCREMENT) | (1 << WIDX_DECREMENT) | (1 << WIDX_PREVIEW)
        | (1 << WIDX_SMALL_SCENERY) | (1 << WIDX_LARGE_SCENERY) | (1 << WIDX_FOOTPATH);
    window->hold_down_widgets = (1 << WIDX_INCREMENT) | (1 << WIDX_DECREMENT);
    WindowInitScrollWidgets(window);
    window_push_others_below(window);

    gLandToolSize = 2;
    gClearSceneryCost = MONEY32_UNDEFINED;

    gClearSmallScenery = true;
    gClearLargeScenery = false;
    gClearFootpath = false;

    return window;
}

/**
 *
 *  rct2: 0x006E6B65
 */
static void window_clear_scenery_close([[maybe_unused]] rct_window* w)
{
    // If the tool wasn't changed, turn tool off
    if (clear_scenery_tool_is_active())
        tool_cancel();
}

/**
 *
 *  rct2: 0x0068E185
 */
static void window_clear_scenery_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_PREVIEW:
            window_clear_scenery_inputsize(w);
            break;
        case WIDX_SMALL_SCENERY:
            gClearSmallScenery ^= 1;
            w->Invalidate();
            break;
        case WIDX_LARGE_SCENERY:
            gClearLargeScenery ^= 1;
            w->Invalidate();
            break;
        case WIDX_FOOTPATH:
            gClearFootpath ^= 1;
            w->Invalidate();
            break;
    }
}

static void window_clear_scenery_mousedown(rct_window* w, rct_widgetindex widgetIndex, [[maybe_unused]] rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_DECREMENT:
            // Decrement land tool size, if it stays within the limit
            gLandToolSize = std::max(MINIMUM_TOOL_SIZE, gLandToolSize - 1);

            // Invalidate the window
            w->Invalidate();
            break;
        case WIDX_INCREMENT:
            // Increment land tool size, if it stays within the limit
            gLandToolSize = std::min(MAXIMUM_TOOL_SIZE, gLandToolSize + 1);

            // Invalidate the window
            w->Invalidate();
            break;
    }
}

static void window_clear_scenery_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
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

static void window_clear_scenery_inputsize(rct_window* w)
{
    TextInputDescriptionArgs[0] = MINIMUM_TOOL_SIZE;
    TextInputDescriptionArgs[1] = MAXIMUM_TOOL_SIZE;
    window_text_input_open(w, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, STR_NONE, STR_NONE, 3);
}

/**
 *
 *  rct2: 0x0068E205
 */
static void window_clear_scenery_update(rct_window* w)
{
    w->frame_no++;
    // Close window if another tool is open
    if (!clear_scenery_tool_is_active())
        window_close(w);
}

/**
 *
 *  rct2: 0x0068E115
 */
static void window_clear_scenery_invalidate(rct_window* w)
{
    // Set the preview image button to be pressed down
    w->pressed_widgets = (1 << WIDX_PREVIEW) | (gClearSmallScenery ? (1 << WIDX_SMALL_SCENERY) : 0)
        | (gClearLargeScenery ? (1 << WIDX_LARGE_SCENERY) : 0) | (gClearFootpath ? (1 << WIDX_FOOTPATH) : 0);

    // Update the preview image (for tool sizes up to 7)
    window_clear_scenery_widgets[WIDX_PREVIEW].image = LandTool::SizeToSpriteIndex(gLandToolSize);
}

/**
 *
 *  rct2: 0x0068E130
 */
static void window_clear_scenery_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    // Draw number for tool sizes bigger than 7
    ScreenCoordsXY screenCoords = { w->windowPos.x + window_clear_scenery_widgets[WIDX_PREVIEW].midX(),
                                    w->windowPos.y + window_clear_scenery_widgets[WIDX_PREVIEW].midY() };
    if (gLandToolSize > MAX_TOOL_SIZE_WITH_SPRITE)
    {
        DrawTextBasic(
            dpi, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, &gLandToolSize, { TextAlignment::CENTRE });
    }

    // Draw cost amount
    if (gClearSceneryCost != MONEY32_UNDEFINED && gClearSceneryCost != 0 && !(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        screenCoords.x = window_clear_scenery_widgets[WIDX_PREVIEW].midX() + w->windowPos.x;
        screenCoords.y = window_clear_scenery_widgets[WIDX_PREVIEW].bottom + w->windowPos.y + 5 + 27;
        DrawTextBasic(dpi, screenCoords, STR_COST_AMOUNT, &gClearSceneryCost, { TextAlignment::CENTRE });
    }
}
