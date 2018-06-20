/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/windows/Window.h>

#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/drawing/Drawing.h>
#include "../interface/Theme.h"

// clang-format off
static rct_widget window_map_tooltip_widgets[] = {
    { WWT_IMGBTN, 0, 0, 199, 0, 29, 0xFFFFFFFF, STR_NONE },
    { WIDGETS_END }
};

static void window_map_tooltip_update(rct_window *w);
static void window_map_tooltip_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_map_tooltip_events = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_map_tooltip_update,
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
    window_map_tooltip_paint,
    nullptr
};
// clang-format on

#define MAP_TOOLTIP_ARGS

static int32_t _lastCursorX;
static int32_t _lastCursorY;
static int32_t _cursorHoldDuration;

static void window_map_tooltip_open();

/**
 *
 *  rct2: 0x006EE77A
 */
void window_map_tooltip_update_visibility()
{
    if (theme_get_flags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
    {
        // The map tooltip is drawn by the bottom toolbar
        window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
        return;
    }

    int32_t cursorX, cursorY;

    const CursorState * state = context_get_cursor_state();
    cursorX = state->x;
    cursorY = state->y;

    // Check for cursor movement
    _cursorHoldDuration++;
    if (abs(cursorX - _lastCursorX) > 5 || abs(cursorY - _lastCursorY) > 5 || (input_test_flag(INPUT_FLAG_5)))
        _cursorHoldDuration = 0;

    _lastCursorX = cursorX;
    _lastCursorY = cursorY;

    // Show or hide tooltip
    rct_string_id stringId;
    memcpy(&stringId, gMapTooltipFormatArgs, sizeof(rct_string_id));

    if (_cursorHoldDuration < 25 ||
        stringId == STR_NONE ||
        input_test_place_object_modifier((PLACE_OBJECT_MODIFIER) (PLACE_OBJECT_MODIFIER_COPY_Z | PLACE_OBJECT_MODIFIER_SHIFT_Z)) ||
        window_find_by_class(WC_ERROR) != nullptr
    ) {
        window_close_by_class(WC_MAP_TOOLTIP);
    } else {
        window_map_tooltip_open();
    }
}

/**
 *
 *  rct2: 0x006A7C43
 */
static void window_map_tooltip_open()
{
    rct_window* w;
    int32_t x, y, width, height;

    width = 200;
    height = 44;
    const CursorState * state = context_get_cursor_state();
    x = state->x - (width / 2);
    y = state->y + 15;

    w = window_find_by_class(WC_MAP_TOOLTIP);
    if (w == nullptr) {
        w = window_create(
            x, y, width, height, &window_map_tooltip_events, WC_MAP_TOOLTIP, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND
        );
        w->widgets = window_map_tooltip_widgets;
    } else {
        window_invalidate(w);
        w->x = x;
        w->y = y;
        w->width = width;
        w->height = height;
    }
}

/**
 *
 *  rct2: 0x006EE8CE
 */
static void window_map_tooltip_update(rct_window *w)
{
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006EE894
 */
static void window_map_tooltip_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    rct_string_id stringId;
    memcpy(&stringId, gMapTooltipFormatArgs, sizeof(rct_string_id));
    if (stringId == STR_NONE) {
        return;
    }

    gfx_draw_string_centred_wrapped(dpi, gMapTooltipFormatArgs, w->x + (w->width / 2), w->y + (w->height / 2), w->width, STR_MAP_TOOLTIP_STRINGID, COLOUR_BLACK);
}
