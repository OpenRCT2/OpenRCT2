/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/paint/Paint.h>
#include <openrct2/paint/tile_element/Paint.TileElement.h>
#include <openrct2/ride/TrackPaint.h>

static int32_t ResizeLanguage = LANGUAGE_UNDEFINED;

// clang-format off
enum WINDOW_DEBUG_PAINT_WIDGET_IDX
{
    WIDX_BACKGROUND,
    WIDX_TOGGLE_SHOW_WIDE_PATHS,
    WIDX_TOGGLE_SHOW_BLOCKED_TILES,
    WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS,
    WIDX_TOGGLE_SHOW_BOUND_BOXES,
    WIDX_TOGGLE_SHOW_DIRTY_VISUALS,
};

#define WINDOW_WIDTH    (200)
#define WINDOW_HEIGHT   (8 + 15 + 15 + 15 + 15 + 11 + 8)

static rct_widget window_debug_paint_widgets[] = {
    { WWT_FRAME,    0,  0,  WINDOW_WIDTH - 1,   0,              WINDOW_HEIGHT - 1,  STR_NONE,                               STR_NONE },
    { WWT_CHECKBOX, 1,  8,  WINDOW_WIDTH - 8,   8 + 15 * 0,     8 + 15 * 0 + 11,    STR_DEBUG_PAINT_SHOW_WIDE_PATHS,        STR_NONE },
    { WWT_CHECKBOX, 1,  8,  WINDOW_WIDTH - 8,   8 + 15 * 1,     8 + 15 * 1 + 11,    STR_DEBUG_PAINT_SHOW_BLOCKED_TILES,     STR_NONE },
    { WWT_CHECKBOX, 1,  8,  WINDOW_WIDTH - 8,   8 + 15 * 2,     8 + 15 * 2 + 11,    STR_DEBUG_PAINT_SHOW_SEGMENT_HEIGHTS,   STR_NONE },
    { WWT_CHECKBOX, 1,  8,  WINDOW_WIDTH - 8,   8 + 15 * 3,     8 + 15 * 3 + 11,    STR_DEBUG_PAINT_SHOW_BOUND_BOXES,       STR_NONE },
    { WWT_CHECKBOX, 1,  8,  WINDOW_WIDTH - 8,   8 + 15 * 4,     8 + 15 * 4 + 11,    STR_DEBUG_PAINT_SHOW_DIRTY_VISUALS,     STR_NONE },
    { WIDGETS_END },
};

static void window_debug_paint_mouseup(rct_window * w, rct_widgetindex widgetIndex);
static void window_debug_paint_invalidate(rct_window * w);
static void window_debug_paint_paint(rct_window * w, rct_drawpixelinfo * dpi);

static rct_window_event_list window_debug_paint_events = {
    nullptr,
    window_debug_paint_mouseup,
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
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_debug_paint_invalidate,
    window_debug_paint_paint,
    nullptr
};
// clang-format on

rct_window * window_debug_paint_open()
{
    rct_window * window;

    // Check if window is already open
    window = window_find_by_class(WC_DEBUG_PAINT);
    if (window != nullptr)
        return window;

    window = window_create(
        16,
        context_get_height() - 16 - 33 - WINDOW_HEIGHT,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        &window_debug_paint_events,
        WC_DEBUG_PAINT,
        WF_STICK_TO_FRONT | WF_TRANSPARENT
    );

    window->widgets = window_debug_paint_widgets;
    window->enabled_widgets =
        (1 << WIDX_TOGGLE_SHOW_WIDE_PATHS) |
        (1 << WIDX_TOGGLE_SHOW_BLOCKED_TILES) |
        (1 << WIDX_TOGGLE_SHOW_BOUND_BOXES) |
        (1 << WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS) |
        (1 << WIDX_TOGGLE_SHOW_DIRTY_VISUALS);
    window_init_scroll_widgets(window);
    window_push_others_below(window);

    window->colours[0] = TRANSLUCENT(COLOUR_BLACK);
    window->colours[1] = COLOUR_GREY;

    ResizeLanguage = LANGUAGE_UNDEFINED;
    return window;
}

static void window_debug_paint_mouseup([[maybe_unused]] rct_window * w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
        case WIDX_TOGGLE_SHOW_WIDE_PATHS:
            gPaintWidePathsAsGhost = !gPaintWidePathsAsGhost;
            gfx_invalidate_screen();
            break;

        case WIDX_TOGGLE_SHOW_BLOCKED_TILES:
            gPaintBlockedTiles = !gPaintBlockedTiles;
            gfx_invalidate_screen();
            break;

        case WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS:
            gShowSupportSegmentHeights = !gShowSupportSegmentHeights;
            gfx_invalidate_screen();
            break;

        case WIDX_TOGGLE_SHOW_BOUND_BOXES:
            gPaintBoundingBoxes = !gPaintBoundingBoxes;
            gfx_invalidate_screen();
            break;

        case WIDX_TOGGLE_SHOW_DIRTY_VISUALS:
            gShowDirtyVisuals = !gShowDirtyVisuals;
            gfx_invalidate_screen();
            break;
    }
}

static void window_debug_paint_invalidate(rct_window * w)
{
    const auto& ls = OpenRCT2::GetContext()->GetLocalisationService();
    const auto currentLanguage = ls.GetCurrentLanguage();
    if (ResizeLanguage != currentLanguage)
    {
        ResizeLanguage = currentLanguage;
        window_invalidate(w);

        // Find the width of the longest string
        int16_t newWidth = 0;
        for (size_t widgetIndex = WIDX_TOGGLE_SHOW_WIDE_PATHS; widgetIndex <= WIDX_TOGGLE_SHOW_DIRTY_VISUALS; widgetIndex++)
        {
            auto stringIdx = w->widgets[widgetIndex].text;
            auto string = ls.GetString(stringIdx);
            Guard::ArgumentNotNull(string);
            auto width = gfx_get_string_width(string);
            newWidth = std::max<int16_t>(width, newWidth);
        }

        // Add padding for both sides (8) and the offset for the text after the checkbox (15)
        newWidth += 8 * 2 + 15;

        w->width = newWidth;
        w->max_width = newWidth;
        w->min_width = newWidth;
        w->widgets[WIDX_BACKGROUND].right = newWidth - 1;
        w->widgets[WIDX_TOGGLE_SHOW_WIDE_PATHS].right = newWidth - 8;
        w->widgets[WIDX_TOGGLE_SHOW_BLOCKED_TILES].right = newWidth - 8;
        w->widgets[WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS].right = newWidth - 8;
        w->widgets[WIDX_TOGGLE_SHOW_BOUND_BOXES].right = newWidth - 8;
        w->widgets[WIDX_TOGGLE_SHOW_DIRTY_VISUALS].right = newWidth - 8;

        window_invalidate(w);
    }

    widget_set_checkbox_value(w, WIDX_TOGGLE_SHOW_WIDE_PATHS, gPaintWidePathsAsGhost);
    widget_set_checkbox_value(w, WIDX_TOGGLE_SHOW_BLOCKED_TILES, gPaintBlockedTiles);
    widget_set_checkbox_value(w, WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS, gShowSupportSegmentHeights);
    widget_set_checkbox_value(w, WIDX_TOGGLE_SHOW_BOUND_BOXES, gPaintBoundingBoxes);
    widget_set_checkbox_value(w, WIDX_TOGGLE_SHOW_DIRTY_VISUALS, gShowDirtyVisuals);
}

static void window_debug_paint_paint(rct_window * w, rct_drawpixelinfo * dpi)
{
    window_draw_widgets(w, dpi);
}
