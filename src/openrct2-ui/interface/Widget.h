/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Window.h"

#include <openrct2-ui/UiStringIds.h>
#include <openrct2/drawing/FilterPaletteIds.h>
#include <openrct2/interface/Widget.h>

namespace OpenRCT2::Ui
{
    ImageId getColourButtonImage(Drawing::Colour colour);
    Widget* getWidgetByIndex(WindowBase& w, WidgetIndex widgetIndex);

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
        primary,
        secondary,
        tertiary,
        quaternary,
    };

    constexpr Widget makeWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, WidgetType type, WindowColour colour,
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

    constexpr Widget makeWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, WidgetType type, WindowColour colour, ImageId image,
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

    constexpr Widget makeRemapWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, WidgetType type, WindowColour colour, ImageIndex content,
        StringId tooltip = kStringIdNone)
    {
        return makeWidget(origin, size, type, colour, ImageId(content, Drawing::FilterPaletteID::paletteNull), tooltip);
    }

    constexpr Widget makeTab(const ScreenCoordsXY& origin, StringId tooltip = kStringIdNone)
    {
        const ScreenSize size = kTabSize;
        const WidgetType type = WidgetType::tab;
        const WindowColour colour = WindowColour::secondary;
        const auto content = ImageId(kImageIndexUndefined);

        return makeWidget(origin, size, type, colour, content, tooltip);
    }

    constexpr Widget makeProgressBar(
        const ScreenCoordsXY& origin, const ScreenSize& size, Drawing::Colour colour, uint8_t lowerBlinkBound = 0,
        uint8_t upperBlinkBound = 0)
    {
        Widget out = {};
        out.left = origin.x;
        out.right = origin.x + size.width - 1;
        out.top = origin.y;
        out.bottom = origin.y + size.height - 1;
        out.type = WidgetType::progressBar;
        out.colour = EnumValue(colour);
        out.content = 0 | (lowerBlinkBound << 8) | (upperBlinkBound << 16);
        out.tooltip = kStringIdNone;

        return out;
    }

    namespace Detail
    {
        template<typename T, typename Enable = void>
        struct WidgetsCount
        {
            static constexpr size_t count = 0;
        };

        template<typename T>
        struct WidgetsCount<T, std::enable_if_t<std::is_base_of_v<Widget, T>>>
        {
            static constexpr size_t count = 1;
        };

        template<typename T, std::size_t N>
        struct WidgetsCount<std::array<T, N>, std::enable_if_t<std::is_base_of_v<Widget, T>>>
        {
            static constexpr size_t count = N;
        };

        template<typename T>
        struct IsWidgetsArray : std::false_type
        {
        };

        template<std::size_t N>
        struct IsWidgetsArray<std::array<Widget, N>> : std::true_type
        {
        };
    } // namespace Detail

    template<typename... TArgs>
    constexpr auto makeWidgets(TArgs&&... args)
    {
        constexpr auto totalCount = [&]() {
            size_t count = 0;
            ((count += Detail::WidgetsCount<std::decay_t<decltype(args)>>::count), ...);
            return count;
        }();

        std::array<Widget, totalCount> res{};
        size_t index = 0;

        const auto append = [&](auto&& val) {
            if constexpr (Detail::IsWidgetsArray<std::decay_t<decltype(val)>>::value)
            {
                for (auto&& widget : val)
                {
                    res[index] = std::move(widget);
                    index++;
                }
            }
            else
            {
                res[index] = std::move(val);
                index++;
            }
        };

        ((append(args)), ...);

        return res;
    }

    constexpr std::array<Widget, 3> makeWindowShim(StringId title, ScreenSize size)
    {
        // clang-format off
        std::array<Widget, 3> out = {
            makeWidget({ 0, 0 }, { size.width, size.height }, WidgetType::frame, WindowColour::primary),
            makeWidget({ 1, 1 }, { size.width - 1, kTitleHeightNormal }, WidgetType::caption, WindowColour::primary, title, STR_WINDOW_TITLE_TIP),
            makeWidget({ size.width - 12, 2 }, { 11, 11 }, WidgetType::closeBox, WindowColour::primary, kWidgetContentEmpty, STR_CLOSE_WINDOW_TIP),
        };
        // clang-format on

        out[2].string = kCloseBoxStringBlackNormal;
        return out;
    }

    constexpr Widget makeSpinnerDecreaseWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WidgetType type, WindowColour colour,
        [[maybe_unused]] uint32_t content = kWidgetContentEmpty, StringId tooltip = kStringIdNone)
    {
        const int16_t xPos = origin.x + size.width - 26;
        const int16_t yPos = origin.y + 1;
        const uint16_t width = 13;
        const uint16_t height = size.height - 2;

        return makeWidget({ xPos, yPos }, { width, height }, WidgetType::button, colour, STR_NUMERIC_DOWN, tooltip);
    }

    constexpr Widget makeSpinnerIncreaseWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WidgetType type, WindowColour colour,
        [[maybe_unused]] uint32_t content = kWidgetContentEmpty, StringId tooltip = kStringIdNone)
    {
        const int16_t xPos = origin.x + size.width - 13;
        const int16_t yPos = origin.y + 1;
        const uint16_t width = 12;
        const uint16_t height = size.height - 2;

        return makeWidget({ xPos, yPos }, { width, height }, WidgetType::button, colour, STR_NUMERIC_UP, tooltip);
    }

    constexpr std::array<Widget, 3> makeSpinnerWidgets(
        const ScreenCoordsXY& origin, const ScreenSize& size, WidgetType type, WindowColour colour,
        uint32_t content = kWidgetContentEmpty, StringId tooltip = kStringIdNone)
    {
        return makeWidgets(
            makeWidget(origin, size, type, colour, content, tooltip),
            makeSpinnerIncreaseWidget(origin, size, type, colour, content, tooltip),
            makeSpinnerDecreaseWidget(origin, size, type, colour, content, tooltip));
    };

    constexpr Widget makeDropdownBoxWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WidgetType type, WindowColour colour,
        [[maybe_unused]] uint32_t content = kWidgetContentEmpty, StringId tooltip = kStringIdNone)
    {
        return makeWidget(origin, size, type, colour, content);
    }

    constexpr Widget makeDropdownButtonWidget(
        const ScreenCoordsXY& origin, const ScreenSize& size, [[maybe_unused]] WidgetType type, WindowColour colour,
        [[maybe_unused]] uint32_t content = kWidgetContentEmpty, StringId tooltip = kStringIdNone)
    {
        const int16_t xPos = origin.x + size.width - 11;
        const int16_t yPos = origin.y + 1;
        const uint16_t width = 11;
        const uint16_t height = size.height - 2;

        return makeWidget({ xPos, yPos }, { width, height }, WidgetType::button, colour, STR_DROPDOWN_GLYPH, tooltip);
    }

    constexpr std::array<Widget, 2> makeDropdownWidgets(
        const ScreenCoordsXY& origin, const ScreenSize& size, WidgetType type, WindowColour colour, uint32_t content,
        StringId tooltip = kStringIdNone)
    {
        return makeWidgets(
            makeDropdownBoxWidget(origin, size, type, colour, content, tooltip),
            makeDropdownButtonWidget(origin, size, type, colour, content, tooltip));
    };

    void widgetDraw(Drawing::RenderTarget& rt, WindowBase& w, WidgetIndex widgetIndex);

    bool widgetIsDisabled(const WindowBase& w, WidgetIndex widgetIndex);
    bool widgetIsHoldable(const WindowBase& w, WidgetIndex widgetIndex);
    bool widgetIsVisible(const WindowBase& w, WidgetIndex widgetIndex);
    bool widgetIsPressed(const WindowBase& w, WidgetIndex widgetIndex);
    bool widgetIsHighlighted(const WindowBase& w, WidgetIndex widgetIndex);
    void widgetScrollGetPart(
        WindowBase& w, const Widget* widget, const ScreenCoordsXY& screenCoords, ScreenCoordsXY& retScreenCoords,
        int32_t* output_scroll_area, int32_t* scroll_id);

    void widgetSetEnabled(WindowBase& w, WidgetIndex widgetIndex, bool enabled);
    void widgetSetDisabled(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void widgetSetHoldable(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void widgetSetVisible(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void widgetSetPressed(WindowBase& w, WidgetIndex widgetIndex, bool value);
    void widgetSetCheckboxValue(WindowBase& w, WidgetIndex widgetIndex, bool value);

    void widgetProgressBarSetNewPercentage(Widget& widget, uint8_t newPercentage);

    void widgetScrollUpdateThumbs(WindowBase& w, WidgetIndex widget_index);
} // namespace OpenRCT2::Ui
