/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
static Widget window_map_tooltip_widgets[] = {
    MakeWidget({0, 0}, {200, 30}, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};

static void WindowMapTooltipUpdate(WindowBase *w);
static void WindowMapTooltipPaint(WindowBase *w, DrawPixelInfo *dpi);

static WindowEventList window_map_tooltip_events([](auto& events)
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
        WindowInvalidateByClass(WindowClass::BottomToolbar);
        return;
    }

    const CursorState* state = ContextGetCursorState();
    auto cursor = state->position;
    auto cursorChange = cursor - _lastCursor;

    // Check for cursor movement
    _cursorHoldDuration++;
    if (abs(cursorChange.x) > 5 || abs(cursorChange.y) > 5 || (InputTestFlag(INPUT_FLAG_5))
        || InputGetState() == InputState::ViewportRight)
        _cursorHoldDuration = 0;

    _lastCursor = cursor;

    // Show or hide tooltip
    StringId stringId;
    std::memcpy(&stringId, _mapTooltipArgs.Data(), sizeof(StringId));

    if (_cursorHoldDuration < 25 || stringId == STR_NONE
        || InputTestPlaceObjectModifier(
            static_cast<PLACE_OBJECT_MODIFIER>(PLACE_OBJECT_MODIFIER_COPY_Z | PLACE_OBJECT_MODIFIER_SHIFT_Z))
        || WindowFindByClass(WindowClass::Error) != nullptr)
    {
        WindowCloseByClass(WindowClass::MapTooltip);
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
    WindowBase* w;

    constexpr int32_t width = 200;
    constexpr int32_t height = 44;
    const CursorState* state = ContextGetCursorState();
    ScreenCoordsXY pos = { state->position.x - (width / 2), state->position.y + 15 };

    w = WindowFindByClass(WindowClass::MapTooltip);
    if (w == nullptr)
    {
        w = WindowCreate(
            pos, width, height, &window_map_tooltip_events, WindowClass::MapTooltip,
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
static void WindowMapTooltipUpdate(WindowBase* w)
{
    w->Invalidate();
}

/**
 *
 *  rct2: 0x006EE894
 */
static void WindowMapTooltipPaint(WindowBase* w, DrawPixelInfo* dpi)
{
    StringId stringId;
    std::memcpy(&stringId, _mapTooltipArgs.Data(), sizeof(StringId));
    if (stringId == STR_NONE)
    {
        return;
    }

    ScreenCoordsXY stringCoords(w->windowPos.x + (w->width / 2), w->windowPos.y + (w->height / 2));
    DrawTextWrapped(dpi, stringCoords, w->width, STR_MAP_TOOLTIP_STRINGID, _mapTooltipArgs, { TextAlignment::CENTRE });
}
