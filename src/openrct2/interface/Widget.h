/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "../localisation/StringIds.h"
#include "Window.h"

#define WIDGETS_END WWT_LAST, 0, 0, 0, 0, 0, 0, 0
#define BAR_BLINK (1u << 31)

#define SPINNER_INCREASE(l, r, t, b) (r) - 12, (r)-1, (t) + 1, (b)-1
#define SPINNER_DECREASE(l, r, t, b) (r) - 25, (r)-13, (t) + 1, (b)-1
#define SPINNER_WIDGETS(colour, left, right, top, bottom, text, tooltip)                                                       \
    { WWT_SPINNER, colour, left, right, top, bottom, text, tooltip },                                                          \
        { WWT_BUTTON, colour, SPINNER_INCREASE(left, right, top, bottom), STR_NUMERIC_UP, STR_NONE },                          \
    {                                                                                                                          \
        WWT_BUTTON, colour, SPINNER_DECREASE(left, right, top, bottom), STR_NUMERIC_DOWN, STR_NONE                             \
    }

enum
{
    SCROLL_HORIZONTAL = (1 << 0),
    SCROLL_VERTICAL = (1 << 1),
    SCROLL_BOTH = SCROLL_HORIZONTAL | SCROLL_VERTICAL
};

enum class WindowColour : uint8_t
{
    Primary,
    Secondary,
    Tertiary,
    Quaternary,
};

constexpr uint8_t SCROLLBAR_WIDTH = 10;

constexpr const ScreenSize TAB_SIZE = { 31, 27 };

constexpr rct_widget MakeWidget(
    const ScreenCoordsXY& origin, const ScreenSize& size, WINDOW_WIDGET_TYPES type, WindowColour colour,
    uint32_t content = 0xFFFFFFFF, rct_string_id tooltip = STR_NONE)
{
    rct_widget out = {};
    out.left = origin.x;
    out.right = origin.x + size.width - 1;
    out.top = origin.y;
    out.bottom = origin.y + size.height - 1;
    out.type = type;
    out.colour = static_cast<uint8_t>(colour);
    out.content = content;
    out.tooltip = tooltip;

    return out;
}

constexpr rct_widget MakeRemapWidget(
    const ScreenCoordsXY& origin, const ScreenSize& size, WINDOW_WIDGET_TYPES type, WindowColour colour,
    uint32_t content = 0xFFFFFFFF, rct_string_id tooltip = STR_NONE)
{
    return MakeWidget(origin, size, type, colour, IMAGE_TYPE_REMAP | content, tooltip);
}

constexpr rct_widget MakeTab(const ScreenCoordsXY& origin, rct_string_id tooltip = STR_NONE)
{
    const ScreenSize size = TAB_SIZE;
    const WINDOW_WIDGET_TYPES type = WWT_TAB;
    const WindowColour colour = WindowColour::Secondary;
    const uint32_t content = 0xFFFFFFFF;

    return MakeWidget(origin, size, type, colour, content, tooltip);
}

#define MakeSpinnerWidgets(...)                                                                                                \
    MakeWidget(__VA_ARGS__), MakeSpinnerIncreaseWidget(__VA_ARGS__), MakeSpinnerDecreaseWidget(__VA_ARGS__)

constexpr rct_widget MakeSpinnerDecreaseWidget(
    const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] uint8_t type, WindowColour colour,
    [[maybe_unused]] uint32_t content = 0xFFFFFFFF, rct_string_id tooltip = STR_NONE)
{
    const int16_t xPos = origin.x + size.width - 26;
    const int16_t yPos = origin.y + 1;
    const uint16_t width = 13;
    const uint16_t height = size.height - 2;

    return MakeWidget({ xPos, yPos }, { width, height }, WWT_BUTTON, colour, STR_NUMERIC_DOWN, tooltip);
}

constexpr rct_widget MakeSpinnerIncreaseWidget(
    const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] uint8_t type, WindowColour colour,
    [[maybe_unused]] uint32_t content = 0xFFFFFFFF, rct_string_id tooltip = STR_NONE)
{
    const int16_t xPos = origin.x + size.width - 13;
    const int16_t yPos = origin.y + 1;
    const uint16_t width = 12;
    const uint16_t height = size.height - 2;

    return MakeWidget({ xPos, yPos }, { width, height }, WWT_BUTTON, colour, STR_NUMERIC_UP, tooltip);
}

void WidgetScrollUpdateThumbs(rct_window* w, rct_widgetindex widget_index);
void WidgetDraw(rct_drawpixelinfo* dpi, rct_window* w, rct_widgetindex widgetIndex);

bool WidgetIsEnabled(rct_window* w, rct_widgetindex widgetIndex);
bool WidgetIsDisabled(rct_window* w, rct_widgetindex widgetIndex);
bool WidgetIsPressed(rct_window* w, rct_widgetindex widgetIndex);
bool WidgetIsHighlighted(rct_window* w, rct_widgetindex widgetIndex);
bool WidgetIsActiveTool(rct_window* w, rct_widgetindex widgetIndex);
void WidgetScrollGetPart(
    rct_window* w, rct_widget* widget, const ScreenCoordsXY& screenCoords, ScreenCoordsXY& retScreenCoords,
    int32_t* output_scroll_area, int32_t* scroll_id);

void WidgetSetEnabled(rct_window* w, rct_widgetindex widgetIndex, bool enabled);
void WidgetSetCheckboxValue(rct_window* w, rct_widgetindex widgetIndex, int32_t value);

#endif
