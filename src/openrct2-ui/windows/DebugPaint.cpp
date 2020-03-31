/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Dropdown.h>
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
    WIDX_WIDE_GROUP_BOX,
    WIDX_WIDE_ID_LEVEL,
    WIDX_WIDE_ID_LEVEL_UP,
    WIDX_WIDE_ID_LEVEL_DOWN,
    WIDX_WIDE_ID_GROUP,
    WIDX_WIDE_ID_GROUP_DROPDOWN,
    WIDX_TOGGLE_SHOW_BLOCKED_TILES,
    WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS,
    WIDX_TOGGLE_SHOW_BOUND_BOXES,
    WIDX_TOGGLE_SHOW_DIRTY_VISUALS,
};

constexpr int32_t WINDOW_WIDTH = 200;
constexpr int32_t WINDOW_HEIGHT = 8 + 15 + 15 + 15 + 15 + 11 + 8;

static rct_widget window_debug_paint_widgets[] = {
    { WWT_FRAME,    0,  0,                  WINDOW_WIDTH - 1,     0,              WINDOW_HEIGHT - 1,    STR_NONE,                               STR_NONE },
    { WWT_GROUPBOX, 1,  2,                  WINDOW_WIDTH - 2,     2 + 15 * 0,      2 + 15 * 0 + 33,     STR_DEBUG_WIDE_PATH_GROUP_BOX,          STR_NONE },
    SPINNER_WIDGETS (1, 10,                 WINDOW_WIDTH/2 - 1,   16 + 15 * 0,     16 + 15 * 0 + 11,    STR_NONE,                               STR_NONE),
    { WWT_DROPDOWN, 1,  WINDOW_WIDTH/2 + 1, WINDOW_WIDTH - 10,    16 + 15 * 0,     16 + 15 * 0 + 11,    STR_EMPTY,                              STR_NONE }, // Wide Paths
    { WWT_BUTTON,   1,  WINDOW_WIDTH - 15,  WINDOW_WIDTH - 11,    16 + 15 * 0 + 1, 16 + 15 * 0 + 10,    STR_DROPDOWN_GLYPH,                     STR_NONE },
    { WWT_CHECKBOX, 1,  8,                  WINDOW_WIDTH - 8,     22 + 15 * 1,     22 + 15 * 1 + 11,    STR_DEBUG_PAINT_SHOW_BLOCKED_TILES,     STR_NONE },
    { WWT_CHECKBOX, 1,  8,                  WINDOW_WIDTH - 8,     22 + 15 * 2,     22 + 15 * 2 + 11,    STR_DEBUG_PAINT_SHOW_SEGMENT_HEIGHTS,   STR_NONE },
    { WWT_CHECKBOX, 1,  8,                  WINDOW_WIDTH - 8,     22 + 15 * 3,     22 + 15 * 3 + 11,    STR_DEBUG_PAINT_SHOW_BOUND_BOXES,       STR_NONE },
    { WWT_CHECKBOX, 1,  8,                  WINDOW_WIDTH - 8,     22 + 15 * 4,     22 + 15 * 4 + 11,    STR_DEBUG_PAINT_SHOW_DIRTY_VISUALS,     STR_NONE },
    { WIDGETS_END },
};

static void window_options_show_dropdown(rct_window *w, rct_widget *widget, int32_t num_items);
static void window_debug_paint_mouseup(rct_window * w, rct_widgetindex widgetIndex);
static void window_debug_paint_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_debug_paint_invalidate(rct_window * w);
static void window_debug_paint_paint(rct_window * w, rct_drawpixelinfo * dpi);
static void window_debug_paint_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);


static rct_window_event_list window_debug_paint_events = {
    nullptr,
    window_debug_paint_mouseup,
    nullptr,
    window_debug_paint_mousedown,
    window_debug_paint_dropdown,
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

rct_window* window_debug_paint_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_find_by_class(WC_DEBUG_PAINT);
    if (window != nullptr)
        return window;

    window = window_create(
        ScreenCoordsXY(16, context_get_height() - ((WIDE_GROUPS_PER_LEVEL + 3) * 15 + 8)), WINDOW_WIDTH, WINDOW_HEIGHT,
        &window_debug_paint_events, WC_DEBUG_PAINT, WF_STICK_TO_FRONT | WF_TRANSPARENT);

    window->widgets = window_debug_paint_widgets;
    window->enabled_widgets = (1 << WIDX_WIDE_GROUP_BOX) | (1 << WIDX_WIDE_ID_LEVEL) | (1 << WIDX_WIDE_ID_LEVEL_UP)
        | (1 << WIDX_WIDE_ID_LEVEL_DOWN) | (1 << WIDX_WIDE_ID_GROUP) | (1 << WIDX_WIDE_ID_GROUP_DROPDOWN)
        | (1 << WIDX_TOGGLE_SHOW_BLOCKED_TILES) | (1 << WIDX_TOGGLE_SHOW_BOUND_BOXES) | (1 << WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS)
        | (1 << WIDX_TOGGLE_SHOW_DIRTY_VISUALS);
    window_init_scroll_widgets(window);
    window_push_others_below(window);

    window->colours[0] = TRANSLUCENT(COLOUR_BLACK);
    window->colours[1] = COLOUR_GREY;

    ResizeLanguage = LANGUAGE_UNDEFINED;
    return window;
}

static void window_debug_paint_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_WIDE_ID_GROUP_DROPDOWN:
            gPaintWideGroupAsGhost = dropdownIndex - 3;
            gfx_invalidate_screen();
            break;
    }
}

static void window_options_show_dropdown(rct_window* w, rct_widget* widget, int32_t num_items)
{
    window_dropdown_show_text_custom_width(
        w->windowPos.x + widget->left, w->windowPos.y + widget->top, widget->bottom - widget->top + 1, w->colours[1], 0,
        DROPDOWN_FLAG_STAY_OPEN, num_items, widget->right - widget->left - 3);
}

static void window_debug_paint_mouseup([[maybe_unused]] rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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

static void window_debug_paint_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    widget = &w->widgets[widgetIndex - 1];
    switch (widgetIndex)
    {
        case WIDX_WIDE_ID_GROUP_DROPDOWN:
            gDropdownItemsFormat[0] = STR_STRINGID;
            gDropdownItemsArgs[0] = STR_EMPTY;
            gDropdownItemsFormat[1] = STR_STRINGID;
            gDropdownItemsArgs[1] = STR_SHOW_WIDE_PATH_GROUP_ANY;
            gDropdownItemsFormat[2] = STR_STRINGID;
            gDropdownItemsArgs[2] = STR_SHOW_WIDE_PATH_GROUP_ALL;
            for (uint16_t i = 3; i < WIDE_GROUPS_PER_LEVEL + 3; i++)
            {
                gDropdownItemsFormat[i] = STR_SHOW_WIDE_PATH_GROUP_NUMBER;
                gDropdownItemsArgs[i] = i - 3;
            }
            window_options_show_dropdown(w, widget, WIDE_GROUPS_PER_LEVEL + 3);
            gfx_invalidate_screen();
            break;
        case WIDX_WIDE_ID_LEVEL_UP:
            if (gPaintWideLevelAsGhost < gMaxWideLevels - 1)
            {
                gPaintWideLevelAsGhost += 1;
                gfx_invalidate_screen();
            }
            break;
        case WIDX_WIDE_ID_LEVEL_DOWN:
            if (gPaintWideLevelAsGhost > -2)
            {
                gPaintWideLevelAsGhost -= 1;
                gfx_invalidate_screen();
            }
            break;
    }
}

static void window_debug_paint_invalidate(rct_window* w)
{
    const auto& ls = OpenRCT2::GetContext()->GetLocalisationService();
    const auto currentLanguage = ls.GetCurrentLanguage();
    if (ResizeLanguage != currentLanguage)
    {
        ResizeLanguage = currentLanguage;
        w->Invalidate();

        // Find the width of the longest string
        int16_t newWidth = 0;
        for (size_t widgetIndex = WIDX_WIDE_ID_GROUP; widgetIndex <= WIDX_TOGGLE_SHOW_DIRTY_VISUALS; widgetIndex++)
        {
            auto stringIdx = w->widgets[widgetIndex].text;
            auto string = ls.GetString(stringIdx);
            Guard::ArgumentNotNull(string);
            auto width = gfx_get_string_width(string);
            newWidth = std::max<int16_t>(width, newWidth);
        }

        int16_t widestWideFlagStr = std::max<int16_t>(
            gfx_get_string_width(ls.GetString(STR_SHOW_WIDE_PATH_GROUP_ANY)),
            gfx_get_string_width(ls.GetString(STR_SHOW_WIDE_PATH_GROUP_ALL)));
        widestWideFlagStr = std::max<int16_t>(
            gfx_get_string_width(ls.GetString(STR_SHOW_WIDE_PATH_GROUP_NUMBER)) + 2, widestWideFlagStr);
        widestWideFlagStr += gfx_get_string_width(ls.GetString(STR_SHOW_WIDE_PATH_LEVEL_NONE)) + 50;

        newWidth = std::max<int16_t>(widestWideFlagStr, newWidth);

        // Add padding for both sides (8) and the offset for the text after the checkbox (15)
        newWidth += 8 * 2 + 15;

        w->width = newWidth;
        w->max_width = newWidth;
        w->min_width = newWidth;
        w->widgets[WIDX_BACKGROUND].right = newWidth - 1;
        w->widgets[WIDX_WIDE_GROUP_BOX].right = newWidth - 2;
        w->widgets[WIDX_WIDE_ID_LEVEL_UP].left = newWidth / 2 - 17;
        w->widgets[WIDX_WIDE_ID_LEVEL_DOWN].left = newWidth / 2 - 32;
        w->widgets[WIDX_WIDE_ID_LEVEL].right = newWidth / 2 - 1;
        w->widgets[WIDX_WIDE_ID_LEVEL_UP].right = newWidth / 2 - 2;
        w->widgets[WIDX_WIDE_ID_LEVEL_DOWN].right = newWidth / 2 - 17;
        w->widgets[WIDX_WIDE_ID_GROUP].left = newWidth / 2 + 2;
        w->widgets[WIDX_WIDE_ID_GROUP_DROPDOWN].left = newWidth - 16;
        w->widgets[WIDX_WIDE_ID_GROUP].right = newWidth - 10;
        w->widgets[WIDX_WIDE_ID_GROUP_DROPDOWN].right = newWidth - 11;
        w->widgets[WIDX_TOGGLE_SHOW_BLOCKED_TILES].right = newWidth - 8;
        w->widgets[WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS].right = newWidth - 8;
        w->widgets[WIDX_TOGGLE_SHOW_BOUND_BOXES].right = newWidth - 8;
        w->widgets[WIDX_TOGGLE_SHOW_DIRTY_VISUALS].right = newWidth - 8;

        w->widgets[WIDX_BACKGROUND].bottom = w->widgets[WIDX_TOGGLE_SHOW_DIRTY_VISUALS].bottom + 8;
        w->height = w->widgets[WIDX_BACKGROUND].bottom;
    }

    rct_string_id wideGroupStringId = STR_NONE;

    if (gPaintWideGroupAsGhost == -3)
        wideGroupStringId = STR_SHOW_WIDE_PATH_GROUP_NONE;
    else if (gPaintWideGroupAsGhost == -2)
        wideGroupStringId = STR_SHOW_WIDE_PATH_GROUP_ANY;
    else if (gPaintWideGroupAsGhost == -1)
        wideGroupStringId = STR_SHOW_WIDE_PATH_GROUP_ALL;
    else if (gPaintWideGroupAsGhost >= 0)
        wideGroupStringId = STR_SHOW_WIDE_PATH_GROUP_NUMBER;

    rct_string_id wideLevelStringId = STR_NONE;
    if (gPaintWideLevelAsGhost == -2)
    {
        wideLevelStringId = STR_SHOW_WIDE_PATH_LEVEL_NONE;
        wideGroupStringId = STR_EMPTY;
        w->disabled_widgets |= 1 << WIDX_WIDE_ID_GROUP;
        w->disabled_widgets |= 1 << WIDX_WIDE_ID_GROUP_DROPDOWN;
    }
    else if (gPaintWideLevelAsGhost == -1)
    {
        wideLevelStringId = STR_SHOW_WIDE_PATH_LEVEL_BASIC;
        wideGroupStringId = STR_EMPTY;
        w->disabled_widgets |= 1 << WIDX_WIDE_ID_GROUP;
        w->disabled_widgets |= 1 << WIDX_WIDE_ID_GROUP_DROPDOWN;
    }
    else if (gPaintWideLevelAsGhost >= 0)
    {
        wideLevelStringId = STR_SHOW_WIDE_PATH_LEVEL_NUMBER;
        w->disabled_widgets &= ~(1 << WIDX_WIDE_ID_GROUP);
        w->disabled_widgets &= ~(1 << WIDX_WIDE_ID_GROUP_DROPDOWN);
    }

    w->widgets[WIDX_WIDE_ID_GROUP].text = wideGroupStringId;
    if (wideGroupStringId == STR_SHOW_WIDE_PATH_GROUP_NUMBER)
        set_format_arg(0, uint16_t, gPaintWideGroupAsGhost);

    w->widgets[WIDX_WIDE_ID_LEVEL].text = wideLevelStringId;
    if (wideLevelStringId == STR_SHOW_WIDE_PATH_LEVEL_NUMBER)
        set_format_arg(2, uint16_t, gPaintWideLevelAsGhost);

    widget_set_checkbox_value(w, WIDX_TOGGLE_SHOW_BLOCKED_TILES, gPaintBlockedTiles);
    widget_set_checkbox_value(w, WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS, gShowSupportSegmentHeights);
    widget_set_checkbox_value(w, WIDX_TOGGLE_SHOW_BOUND_BOXES, gPaintBoundingBoxes);
    widget_set_checkbox_value(w, WIDX_TOGGLE_SHOW_DIRTY_VISUALS, gShowDirtyVisuals);
}

static void window_debug_paint_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);
}
