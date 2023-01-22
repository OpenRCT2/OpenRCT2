/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../localisation/StringIds.h"
#include "Window.h"
namespace OpenRCT2
{
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

    constexpr const auto WIDGETS_END = Widget{ WindowWidgetType::Last, 0, 0, 0, 0, 0, 0, 0 };
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

    constexpr Widget MakeWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, WindowWidgetType type, WindowColour colour,
        uint32_t content = 0xFFFFFFFF, StringId tooltip = STR_NONE)
    {
        Widget out = {};
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

    constexpr Widget MakeWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, WindowWidgetType type, WindowColour colour, ImageId image,
        StringId tooltip = STR_NONE)
    {
        Widget out = {};
        out.left = origin.x;
        out.right = origin.x + size.width - 1;
        out.top = origin.y;
        out.bottom = origin.y + size.height - 1;
        out.type = type;
        out.colour = static_cast<uint8_t>(colour);
        out.image = image;
        out.tooltip = tooltip;

        return out;
    }

    constexpr Widget MakeRemapWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, WindowWidgetType type, WindowColour colour, ImageIndex content,
        StringId tooltip = STR_NONE)
    {
        return MakeWidget(origin, size, type, colour, ImageId(content, FilterPaletteID::PaletteNull), tooltip);
    }

    constexpr Widget MakeTab(const ScreenCoordsXY& origin, StringId tooltip = STR_NONE)
    {
        const ScreenSize size = TAB_SIZE;
        const WindowWidgetType type = WindowWidgetType::Tab;
        const WindowColour colour = WindowColour::Secondary;
        const auto content = ImageId(ImageIndexUndefined);

        return MakeWidget(origin, size, type, colour, content, tooltip);
    }

#define MakeSpinnerWidgets(...)                                                                                                \
    MakeWidget(__VA_ARGS__), MakeSpinnerIncreaseWidget(__VA_ARGS__), MakeSpinnerDecreaseWidget(__VA_ARGS__)

    constexpr Widget MakeSpinnerDecreaseWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WindowWidgetType type, WindowColour colour,
        [[maybe_unused]] uint32_t content = 0xFFFFFFFF, StringId tooltip = STR_NONE)
    {
        const int16_t xPos = origin.x + size.width - 26;
        const int16_t yPos = origin.y + 1;
        const uint16_t width = 13;
        const uint16_t height = size.height - 2;

        return MakeWidget({ xPos, yPos }, { width, height }, WindowWidgetType::Button, colour, STR_NUMERIC_DOWN, tooltip);
    }

    constexpr Widget MakeSpinnerIncreaseWidget(
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

    constexpr Widget MakeDropdownBoxWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WindowWidgetType type, WindowColour colour,
        [[maybe_unused]] uint32_t content = 0xFFFFFFFF, StringId tooltip = STR_NONE)
    {
        return MakeWidget(origin, size, type, colour, content);
    }

    constexpr Widget MakeDropdownButtonWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WindowWidgetType type, WindowColour colour,
        [[maybe_unused]] uint32_t content = 0xFFFFFFFF, StringId tooltip = STR_NONE)
    {
        const int16_t xPos = origin.x + size.width - 11;
        const int16_t yPos = origin.y + 1;
        const uint16_t width = 11;
        const uint16_t height = 10;

        return MakeWidget({ xPos, yPos }, { width, height }, WindowWidgetType::Button, colour, STR_DROPDOWN_GLYPH, tooltip);
    }

    void WidgetScrollUpdateThumbs(WindowBase& w, WidgetIndex widget_index);
    void WidgetDraw(DrawPixelInfo* dpi, WindowBase& w, WidgetIndex widgetIndex);

    bool WidgetIsDisabled(const WindowBase& w, WidgetIndex widgetIndex);
    bool WidgetIsHoldable(const WindowBase& w, WidgetIndex widgetIndex);
    bool WidgetIsVisible(const WindowBase& w, WidgetIndex widgetIndex);
    bool WidgetIsPressed(const WindowBase& w, WidgetIndex widgetIndex);
    bool WidgetIsHighlighted(const WindowBase& w, WidgetIndex widgetIndex);
    bool WidgetIsActiveTool(const WindowBase& w, WidgetIndex widgetIndex);
    void WidgetScrollGetPart(
        WindowBase& w, const Widget* widget, const ScreenCoordsXY& screenCoords, ScreenCoordsXY& retScreenCoords,
        int32_t* output_scroll_area, int32_t* scroll_id);

    void WidgetSetEnabled(WindowBase& w, WidgetIndex widgetIndex, bool enabled);
    void WidgetSetDisabled(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void WidgetSetHoldable(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void WidgetSetVisible(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void WidgetSetPressed(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void WidgetSetCheckboxValue(WindowBase& w, WidgetIndex widgetIndex, bool value);
} // namespace OpenRCT2
