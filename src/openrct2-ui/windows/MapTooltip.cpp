/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>

// clang-format off
static rct_widget window_map_tooltip_widgets[] = {
    MakeWidget({0, 0}, {200, 30}, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};

static void WindowMapTooltipUpdate(rct_window *w);
static void WindowMapTooltipPaint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_map_tooltip_events([](auto& events)
{
    events.update = &WindowMapTooltipUpdate;
    events.paint = &WindowMapTooltipPaint;
});
// clang-format on

#define MAP_TOOLTIP_ARGS

static ScreenCoordsXY _lastCursor;
static int32_t _cursorHoldDuration;

static void WindowMapTooltipOpen();

static Formatter _mapTooltipArgs;

void SetMapTooltip(Formatter& ft)
{
    _mapTooltipArgs = ft;
}

const Formatter& GetMapTooltip()
{
    return _mapTooltipArgs;
}

/**
 *
 *  rct2: 0x006EE77A
 */
void WindowMapTooltipUpdateVisibility()
{
    if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
    {
        // The map tooltip is drawn by the bottom toolbar
        window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
        return;
    }

    const CursorState* state = context_get_cursor_state();
    auto cursor = state->position;
    auto cursorChange = cursor - _lastCursor;

    // Check for cursor movement
    if (abs(cursorChange.x) > 5 || abs(cursorChange.y) > 5 || (input_test_flag(INPUT_FLAG_5))
        || input_get_state() == InputState::ViewportRight)
    {
        _cursorHoldDuration = 0;
    }
    else
    {
        _cursorHoldDuration++;
    }

    _lastCursor = cursor;

    // Show or hide tooltip
    rct_string_id stringId;
    std::memcpy(&stringId, _mapTooltipArgs.Data(), sizeof(rct_string_id));

    if (_cursorHoldDuration < 25 || stringId == STR_NONE
        || InputTestPlaceObjectModifier(
            static_cast<PLACE_OBJECT_MODIFIER>(PLACE_OBJECT_MODIFIER_COPY_Z | PLACE_OBJECT_MODIFIER_SHIFT_Z))
        || window_find_by_class(WC_ERROR) != nullptr)
    {
        window_close_by_class(WC_MAP_TOOLTIP);
    }
    else
    {
        WindowMapTooltipOpen();
    }
}

/**
 *
 *  rct2: 0x006A7C43
 */
static void WindowMapTooltipOpen()
{
    rct_window* w;

    constexpr int32_t width = 200;
    constexpr int32_t height = 44;
    const CursorState* state = context_get_cursor_state();
    ScreenCoordsXY pos = { state->position.x - (width / 2), state->position.y + 15 };

    w = window_find_by_class(WC_MAP_TOOLTIP);
    if (w == nullptr)
    {
        w = WindowCreate(
            pos, width, height, &window_map_tooltip_events, WC_MAP_TOOLTIP,
            WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND);
        w->widgets = window_map_tooltip_widgets;
    }
    else
    {
        w->Invalidate();
        w->windowPos = pos;
        w->width = width;
        w->height = height;
    }
}

/**
 *
 *  rct2: 0x006EE8CE
 */
static void WindowMapTooltipUpdate(rct_window* w)
{
    w->Invalidate();
}

/**
 *
 *  rct2: 0x006EE894
 */
static void WindowMapTooltipPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    rct_string_id stringId;
    std::memcpy(&stringId, _mapTooltipArgs.Data(), sizeof(rct_string_id));
    if (stringId == STR_NONE)
    {
        return;
    }

    ScreenCoordsXY stringCoords(w->windowPos.x + (w->width / 2), w->windowPos.y + (w->height / 2));
    DrawTextWrapped(dpi, stringCoords, w->width, STR_MAP_TOOLTIP_STRINGID, _mapTooltipArgs, { TextAlignment::CENTRE });
}
