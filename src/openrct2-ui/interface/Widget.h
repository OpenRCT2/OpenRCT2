/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Window.h"

#include <openrct2-ui/UiStringIds.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Widget.h>

namespace OpenRCT2::Ui
{
    // clang-format off
#define WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, CLOSE_STR) \
    { WindowWidgetType::Frame,    0,  0,          WIDTH - 1, 0, HEIGHT - 1, 0xFFFFFFFF,  kStringIdNone }, \
    { WindowWidgetType::Caption,  0,  1,          WIDTH - 2, 1, 14,         TITLE,       STR_WINDOW_TITLE_TIP }, \
    { WindowWidgetType::CloseBox, 0,  WIDTH - 13, WIDTH - 3, 2, 13,         CLOSE_STR,   STR_CLOSE_WINDOW_TIP }

#define WINDOW_SHIM(TITLE, WIDTH, HEIGHT) WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, STR_CLOSE_X)
#define WINDOW_SHIM_WHITE(TITLE, WIDTH, HEIGHT) WINDOW_SHIM_RAW(TITLE, WIDTH, HEIGHT, STR_CLOSE_X_WHITE)
    // clang-format on

    ImageId GetColourButtonImage(colour_t colour);
    Widget* GetWidgetByIndex(const WindowBase& w, WidgetIndex widgetIndex);

    constexpr uint32_t kWidgetContentEmpty = 0xFFFFFFFF;
    constexpr auto kBarBlink = (1u << 31);
    constexpr uint8_t kScrollBarWidth = 10;
    constexpr ScreenSize kTabSize = { 31, 27 };

    constexpr const char* kBlackUpArrowString = u8"{BLACK}▲";
    constexpr const char* kBlackDownArrowString = u8"{BLACK}▼";
    constexpr const char* kBlackLeftArrowString = u8"{BLACK}◀";
    constexpr const char* kBlackRightArrowString = u8"{BLACK}▶";
    constexpr const char* kCheckMarkString = u8"✓";
    constexpr const char* kEyeString = u8"👁";

    enum class WindowColour : uint8_t
    {
        Primary,
        Secondary,
        Tertiary,
        Quaternary,
    };

    constexpr Widget MakeWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, WindowWidgetType type, WindowColour colour,
        uint32_t content = kWidgetContentEmpty, StringId tooltip = kStringIdNone)
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
        StringId tooltip = kStringIdNone)
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
        StringId tooltip = kStringIdNone)
    {
        return MakeWidget(origin, size, type, colour, ImageId(content, FilterPaletteID::PaletteNull), tooltip);
    }

    constexpr Widget MakeTab(const ScreenCoordsXY& origin, StringId tooltip = kStringIdNone)
    {
        const ScreenSize size = kTabSize;
        const WindowWidgetType type = WindowWidgetType::Tab;
        const WindowColour colour = WindowColour::Secondary;
        const auto content = ImageId(kImageIndexUndefined);

        return MakeWidget(origin, size, type, colour, content, tooltip);
    }

    constexpr Widget MakeProgressBar(
        const ScreenCoordsXY& origin, const ScreenSize& size, colour_t colour, uint8_t lowerBlinkBound = 0,
        uint8_t upperBlinkBound = 0)
    {
        Widget out = {};
        out.left = origin.x;
        out.right = origin.x + size.width - 1;
        out.top = origin.y;
        out.bottom = origin.y + size.height - 1;
        out.type = WindowWidgetType::ProgressBar;
        out.colour = colour;
        out.content = 0 | (lowerBlinkBound << 8) | (upperBlinkBound << 16);
        out.tooltip = kStringIdNone;

        return out;
    }

// NOLINTBEGIN
#define MakeSpinnerWidgets(...)                                                                                                \
    MakeWidget(__VA_ARGS__), MakeSpinnerIncreaseWidget(__VA_ARGS__), MakeSpinnerDecreaseWidget(__VA_ARGS__)
    // NOLINTEND

    constexpr Widget MakeSpinnerDecreaseWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WindowWidgetType type, WindowColour colour,
        [[maybe_unused]] uint32_t content = kWidgetContentEmpty, StringId tooltip = kStringIdNone)
    {
        const int16_t xPos = origin.x + size.width - 26;
        const int16_t yPos = origin.y + 1;
        const uint16_t width = 13;
        const uint16_t height = size.height - 2;

        return MakeWidget({ xPos, yPos }, { width, height }, WindowWidgetType::Button, colour, STR_NUMERIC_DOWN, tooltip);
    }

    constexpr Widget MakeSpinnerIncreaseWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WindowWidgetType type, WindowColour colour,
        [[maybe_unused]] uint32_t content = kWidgetContentEmpty, StringId tooltip = kStringIdNone)
    {
        const int16_t xPos = origin.x + size.width - 13;
        const int16_t yPos = origin.y + 1;
        const uint16_t width = 12;
        const uint16_t height = size.height - 2;

        return MakeWidget({ xPos, yPos }, { width, height }, WindowWidgetType::Button, colour, STR_NUMERIC_UP, tooltip);
    }

// NOLINTNEXTLINE
#define MakeDropdownWidgets(...) MakeDropdownBoxWidget(__VA_ARGS__), MakeDropdownButtonWidget(__VA_ARGS__)

    constexpr Widget MakeDropdownBoxWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WindowWidgetType type, WindowColour colour,
        [[maybe_unused]] uint32_t content = kWidgetContentEmpty, StringId tooltip = kStringIdNone)
    {
        return MakeWidget(origin, size, type, colour, content);
    }

    constexpr Widget MakeDropdownButtonWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WindowWidgetType type, WindowColour colour,
        [[maybe_unused]] uint32_t content = kWidgetContentEmpty, StringId tooltip = kStringIdNone)
    {
        const int16_t xPos = origin.x + size.width - 11;
        const int16_t yPos = origin.y + 1;
        const uint16_t width = 11;
        const uint16_t height = size.height - 2;

        return MakeWidget({ xPos, yPos }, { width, height }, WindowWidgetType::Button, colour, STR_DROPDOWN_GLYPH, tooltip);
    }

    void WidgetDraw(DrawPixelInfo& dpi, WindowBase& w, WidgetIndex widgetIndex);

    bool WidgetIsDisabled(const WindowBase& w, WidgetIndex widgetIndex);
    bool WidgetIsHoldable(const WindowBase& w, WidgetIndex widgetIndex);
    bool WidgetIsVisible(const WindowBase& w, WidgetIndex widgetIndex);
    bool WidgetIsPressed(const WindowBase& w, WidgetIndex widgetIndex);
    bool WidgetIsHighlighted(const WindowBase& w, WidgetIndex widgetIndex);
    void WidgetScrollGetPart(
        WindowBase& w, const Widget* widget, const ScreenCoordsXY& screenCoords, ScreenCoordsXY& retScreenCoords,
        int32_t* output_scroll_area, int32_t* scroll_id);

    void WidgetSetEnabled(WindowBase& w, WidgetIndex widgetIndex, bool enabled);
    void WidgetSetDisabled(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void WidgetSetHoldable(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void WidgetSetVisible(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void WidgetSetPressed(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void WidgetSetCheckboxValue(WindowBase& w, WidgetIndex widgetIndex, bool value);

    void WidgetProgressBarSetNewPercentage(Widget& widget, uint8_t newPercentage);

    void WidgetScrollUpdateThumbs(WindowBase& w, WidgetIndex widget_index);
} // namespace OpenRCT2::Ui
