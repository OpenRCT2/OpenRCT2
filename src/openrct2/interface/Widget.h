/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../localisation/StringIds.h"
#include "Window.h"

enum class WindowWidgetType : uint8_t
{
    Empty = 0,
    Frame = 1,
    Resize = 2,
    ImgBtn = 3,
    ColourBtn = 6,
    TrnBtn = 7,
    Tab = 8,
    FlatBtn = 9,
    Button = 10,
    LabelCentred = 12, // Centred text
    TableHeader = 13,  // Left-aligned textual button
    Label = 14,        // Left-aligned text
    Spinner = 15,
    DropdownMenu = 16,
    Viewport = 17,
    Groupbox = 19,
    Caption = 20,
    CloseBox = 21,
    Scroll = 22,
    Checkbox = 23,
    Placeholder = 25,
    Custom = 28,
    TextBox = 27,
    Last = 26,
};

constexpr const auto WIDGETS_END = rct_widget{ WindowWidgetType::Last, 0, 0, 0, 0, 0, 0, 0 };
#define BAR_BLINK (1u << 31)

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
    const ScreenCoordsXY& origin, const ScreenSize& size, WindowWidgetType type, WindowColour colour,
    uint32_t content = 0xFFFFFFFF, StringId tooltip = STR_NONE)
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
    const ScreenCoordsXY& origin, const ScreenSize& size, WindowWidgetType type, WindowColour colour,
    uint32_t content = 0xFFFFFFFF, StringId tooltip = STR_NONE)
{
    return MakeWidget(origin, size, type, colour, IMAGE_TYPE_REMAP | content, tooltip);
}

constexpr rct_widget MakeTab(const ScreenCoordsXY& origin, StringId tooltip = STR_NONE)
{
    const ScreenSize size = TAB_SIZE;
    const WindowWidgetType type = WindowWidgetType::Tab;
    const WindowColour colour = WindowColour::Secondary;
    const uint32_t content = 0xFFFFFFFF;

    return MakeWidget(origin, size, type, colour, content, tooltip);
}

#define MakeSpinnerWidgets(...)                                                                                                \
    MakeWidget(__VA_ARGS__), MakeSpinnerIncreaseWidget(__VA_ARGS__), MakeSpinnerDecreaseWidget(__VA_ARGS__)

constexpr rct_widget MakeSpinnerDecreaseWidget(
    const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WindowWidgetType type, WindowColour colour,
    [[maybe_unused]] uint32_t content = 0xFFFFFFFF, StringId tooltip = STR_NONE)
{
    const int16_t xPos = origin.x + size.width - 26;
    const int16_t yPos = origin.y + 1;
    const uint16_t width = 13;
    const uint16_t height = size.height - 2;

    return MakeWidget({ xPos, yPos }, { width, height }, WindowWidgetType::Button, colour, STR_NUMERIC_DOWN, tooltip);
}

constexpr rct_widget MakeSpinnerIncreaseWidget(
    const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WindowWidgetType type, WindowColour colour,
    [[maybe_unused]] uint32_t content = 0xFFFFFFFF, StringId tooltip = STR_NONE)
{
    const int16_t xPos = origin.x + size.width - 13;
    const int16_t yPos = origin.y + 1;
    const uint16_t width = 12;
    const uint16_t height = size.height - 2;

    return MakeWidget({ xPos, yPos }, { width, height }, WindowWidgetType::Button, colour, STR_NUMERIC_UP, tooltip);
}

#define MakeDropdownWidgets(...) MakeDropdownBoxWidget(__VA_ARGS__), MakeDropdownButtonWidget(__VA_ARGS__)

constexpr rct_widget MakeDropdownBoxWidget(
    const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WindowWidgetType type, WindowColour colour,
    [[maybe_unused]] uint32_t content = 0xFFFFFFFF, StringId tooltip = STR_NONE)
{
    return MakeWidget(origin, size, type, colour, content);
}

constexpr rct_widget MakeDropdownButtonWidget(
    const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WindowWidgetType type, WindowColour colour,
    [[maybe_unused]] uint32_t content = 0xFFFFFFFF, StringId tooltip = STR_NONE)
{
    const int16_t xPos = origin.x + size.width - 11;
    const int16_t yPos = origin.y + 1;
    const uint16_t width = 11;
    const uint16_t height = 10;

    return MakeWidget({ xPos, yPos }, { width, height }, WindowWidgetType::Button, colour, STR_DROPDOWN_GLYPH, tooltip);
}

void WidgetScrollUpdateThumbs(rct_window& w, rct_widgetindex widget_index);
void WidgetDraw(rct_drawpixelinfo* dpi, rct_window& w, rct_widgetindex widgetIndex);

bool WidgetIsDisabled(const rct_window& w, rct_widgetindex widgetIndex);
bool WidgetIsHoldable(const rct_window& w, rct_widgetindex widgetIndex);
bool WidgetIsVisible(const rct_window& w, rct_widgetindex widgetIndex);
bool WidgetIsPressed(const rct_window& w, rct_widgetindex widgetIndex);
bool WidgetIsHighlighted(const rct_window& w, rct_widgetindex widgetIndex);
bool WidgetIsActiveTool(const rct_window& w, rct_widgetindex widgetIndex);
void WidgetScrollGetPart(
    rct_window& w, const rct_widget* widget, const ScreenCoordsXY& screenCoords, ScreenCoordsXY& retScreenCoords,
    int32_t* output_scroll_area, int32_t* scroll_id);

void WidgetSetEnabled(rct_window& w, rct_widgetindex widgetIndex, bool enabled);
void WidgetSetDisabled(rct_window& w, rct_widgetindex widgetIndex, bool value);
void WidgetSetHoldable(rct_window& w, rct_widgetindex widgetIndex, bool value);
void WidgetSetVisible(rct_window& w, rct_widgetindex widgetIndex, bool value);
void WidgetSetPressed(rct_window& w, rct_widgetindex widgetIndex, bool value);
void WidgetSetCheckboxValue(rct_window& w, rct_widgetindex widgetIndex, bool value);
