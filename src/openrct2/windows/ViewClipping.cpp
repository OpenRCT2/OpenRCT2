#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../config/Config.h"

extern "C"
{
    #include "../interface/widget.h"
    #include "../interface/viewport.h"
    #include "../localisation/localisation.h"
}

enum WINDOW_VIEW_CLIPPING_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_CLIP_HEIGHT_CHECKBOX,
    WIDX_CLIP_HEIGHT_VALUE,
    WIDX_CLIP_HEIGHT_INCREASE,
    WIDX_CLIP_HEIGHT_DECREASE,
    WIDX_CLIP_HEIGHT_SLIDER
};

enum class DISPLAY_TYPE {
    DISPLAY_RAW,
    DISPLAY_UNITS
};

DISPLAY_TYPE gClipHeightDisplayType = DISPLAY_TYPE::DISPLAY_UNITS;

#pragma region Widgets

#define WW 160
#define WH 70

rct_widget window_view_clipping_widgets[] = {
    { WWT_FRAME,        0,  0,      WW - 1, 0,  WH - 1, STR_NONE,               STR_NONE }, // panel / background
    { WWT_CAPTION,      0,  1,      WW - 2, 1,  14,     STR_VIEW_CLIPPING_TITLE,        STR_WINDOW_TITLE_TIP }, // title bar
    { WWT_CLOSEBOX,     0,  WW - 13,    WW - 3, 2,  13,     STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP }, // close x button
    { WWT_CHECKBOX,     0,  11,     149,    19, 29,     STR_VIEW_CLIPPING_HEIGHT_ENABLE,    STR_VIEW_CLIPPING_HEIGHT_ENABLE_TIP }, // clip height enable/disable check box
    { WWT_SPINNER,      0,  90,     149,    34, 45,     STR_NONE,               STR_VIEW_CLIPPING_HEIGHT_VALUE_TOGGLE }, // clip height value
    { WWT_DROPDOWN_BUTTON,  0,  138,        148,    35, 39,     STR_NUMERIC_UP,             STR_NONE }, // clip height increase
    { WWT_DROPDOWN_BUTTON,  0,  138,        148,    40, 44,     STR_NUMERIC_DOWN,           STR_NONE }, // clip height decrease
    { WWT_SCROLL,       0,  11,     149,    49, 61,     SCROLL_HORIZONTAL,          STR_VIEW_CLIPPING_HEIGHT_SCROLL_TIP }, // clip height scrollbar
    { WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_view_clipping_close_button(rct_window *w);
static void window_view_clipping_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_view_clipping_update(rct_window *w);
static void window_view_clipping_invalidate(rct_window *w);
static void window_view_clipping_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_view_clipping_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);

static rct_window_event_list window_view_clipping_events = {
    window_view_clipping_close_button,
    window_view_clipping_mouseup,
    NULL,
    NULL,
    NULL,
    NULL,
    window_view_clipping_update,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_view_clipping_scrollgetsize,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_view_clipping_invalidate,
    window_view_clipping_paint,
    NULL
};

#pragma endregion

static void window_view_clipping_set_clipheight(rct_window *w, const uint8 clipheight)
{
    gClipHeight = clipheight;
    rct_widget* widget = &window_view_clipping_widgets[WIDX_CLIP_HEIGHT_SLIDER];
    const float clip_height_ratio = (float)gClipHeight / 255;
    w->scrolls[0].h_left = (sint16)ceil(clip_height_ratio * (w->scrolls[0].h_right - ((widget->right - widget->left) - 1)));
}

static void _window_view_clipping_open()
{
    rct_window* window;

    // Get the main viewport to set the view clipping flag.
    rct_window *mainWindow = window_get_main();

    // Check if window is already open
    if (window_find_by_class(WC_VIEW_CLIPPING) != NULL) {
        // If window is already open, toggle the view clipping on/off
        if (mainWindow != NULL) {
            mainWindow->viewport->flags ^= VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT;
            window_invalidate(mainWindow);
        }

        return;
    }

    // Window is not open - create it.
    window = window_create(32, 32, WW, WH, &window_view_clipping_events, WC_VIEW_CLIPPING, 0);
    window->widgets = window_view_clipping_widgets;
    window->enabled_widgets = (1ULL << WIDX_CLOSE) |
        (1ULL << WIDX_CLIP_HEIGHT_CHECKBOX) |
        (1ULL << WIDX_CLIP_HEIGHT_VALUE) |
        (1ULL << WIDX_CLIP_HEIGHT_INCREASE) |
        (1ULL << WIDX_CLIP_HEIGHT_DECREASE) |
        (1ULL << WIDX_CLIP_HEIGHT_SLIDER);

    window_init_scroll_widgets(window);

    // Initialise the clip height slider from the current clip height value.
    window_view_clipping_set_clipheight(window, gClipHeight);

    window_push_others_below(window);

    // Turn on view clipping when the window is opened.
    if (mainWindow != NULL) {
        mainWindow->viewport->flags |= VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT;
        window_invalidate(mainWindow);
    }

    window_invalidate(window);
}

void window_view_clipping_close()
{
    // Turn off view clipping when the window is closed.
    rct_window *mainWindow = window_get_main();
    if (mainWindow != NULL) {
        mainWindow->viewport->flags &= ~VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT;
        window_invalidate(mainWindow);
    }

}

static void window_view_clipping_close_button(rct_window *w)
{
    window_view_clipping_close();
}

static void window_view_clipping_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    rct_window *mainWindow;

    // mouseup appears to be used for buttons, checkboxes
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_CLIP_HEIGHT_CHECKBOX:
        // Toggle height clipping.
        mainWindow = window_get_main();
        if (mainWindow != NULL) {
            mainWindow->viewport->flags ^= VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT;
            window_invalidate(mainWindow);
        }
        window_invalidate(w);
        break;
    case WIDX_CLIP_HEIGHT_INCREASE:
        if (gClipHeight < 255)
            window_view_clipping_set_clipheight(w, gClipHeight + 1);
        mainWindow = window_get_main();
        if (mainWindow != NULL)
            window_invalidate(mainWindow);
        break;
    case WIDX_CLIP_HEIGHT_DECREASE:
        if (gClipHeight > 0)
            window_view_clipping_set_clipheight(w, gClipHeight - 1);
        mainWindow = window_get_main();
        if (mainWindow != NULL)
            window_invalidate(mainWindow);
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
    }
}

static void window_view_clipping_update(rct_window *w)
{
    const rct_widget *const widget = &window_view_clipping_widgets[WIDX_CLIP_HEIGHT_SLIDER];
    const rct_scroll *const scroll = &w->scrolls[0];
    const sint16 scroll_width = widget->right - widget->left - 1;
    const uint8 clip_height = (uint8)(((float)scroll->h_left / (scroll->h_right - scroll_width)) * 255);
    if (clip_height != gClipHeight) {
        gClipHeight = clip_height;

        // Update the main window accordingly.
        rct_window *mainWindow = window_get_main();
        if (mainWindow != NULL) {
            window_invalidate(mainWindow);
        }
    }
    widget_invalidate(w, WIDX_CLIP_HEIGHT_SLIDER);
}

static void window_view_clipping_invalidate(rct_window *w)
{
    widget_scroll_update_thumbs(w, WIDX_CLIP_HEIGHT_SLIDER);

    rct_window *mainWindow = window_get_main();
    if (mainWindow != NULL) {
        widget_set_checkbox_value(w, WIDX_CLIP_HEIGHT_CHECKBOX, mainWindow->viewport->flags & VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT);
    }
}

static void window_view_clipping_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    // Clip height value
    sint32 x = w->x + 8;
    sint32 y = w->y + w->widgets[WIDX_CLIP_HEIGHT_VALUE].top;
    gfx_draw_string_left(dpi, STR_VIEW_CLIPPING_HEIGHT_VALUE, NULL, w->colours[0], x, y);

    x = w->x + w->widgets[WIDX_CLIP_HEIGHT_VALUE].left + 1;
    y = w->y + w->widgets[WIDX_CLIP_HEIGHT_VALUE].top;

    fixed16_1dp clipHeightValueInUnits;
    fixed32_2dp clipHeightValueInMeters;
    fixed16_1dp clipHeightValueInFeet;
    sint32 clipHeightRawValue = (sint32)gClipHeight;
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

extern "C"
{
    void window_view_clipping_open()
    {
        _window_view_clipping_open();
    }
}

