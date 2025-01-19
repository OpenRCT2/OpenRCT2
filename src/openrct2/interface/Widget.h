/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"
#include "../drawing/ImageId.hpp"
#include "../localisation/StringIdType.h"
#include "../world/Location.hpp"

#include <cstdint>

namespace OpenRCT2
{
    using WidgetIndex = uint16_t;
    constexpr WidgetIndex kWidgetIndexNull = 0xFFFF;

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
        ProgressBar = 29,
        Custom = 28,
        TextBox = 27,
    };

    using WidgetFlags = uint32_t;
    namespace WIDGET_FLAGS
    {
        const WidgetFlags TEXT_IS_STRING = 1 << 0;
        const WidgetFlags IS_PRESSED = 1 << 2;
        const WidgetFlags IS_DISABLED = 1 << 3;
        const WidgetFlags TOOLTIP_IS_STRING = 1 << 4;
        const WidgetFlags IS_HIDDEN = 1 << 5;
        const WidgetFlags IS_HOLDABLE = 1 << 6;
    } // namespace WIDGET_FLAGS

    enum
    {
        SCROLL_HORIZONTAL = (1 << 0),
        SCROLL_VERTICAL = (1 << 1),
        SCROLL_BOTH = SCROLL_HORIZONTAL | SCROLL_VERTICAL
    };

    struct Widget
    {
        WindowWidgetType type{};
        uint8_t colour{};
        int16_t left{};
        int16_t right{};
        int16_t top{};
        int16_t bottom{};
        union
        {
            uint32_t content;
            ImageId image{};
            StringId text;
            utf8* string;
        };
        StringId tooltip{ kStringIdNone };

        // New properties
        WidgetFlags flags{};
        utf8* sztooltip{};

        int16_t width() const
        {
            return right - left;
        }

        int16_t height() const
        {
            return bottom - top;
        }

        int16_t midX() const
        {
            return (left + right) / 2;
        }

        int16_t midY() const
        {
            return (top + bottom) / 2;
        }

        int16_t textTop() const
        {
            if (height() >= 10)
                return std::max<int32_t>(top, top + (height() / 2) - 5);

            return top - 1;
        }

        void moveRight(int32_t amount)
        {
            left += amount;
            right += amount;
        }

        void moveDown(int32_t amount)
        {
            top += amount;
            bottom += amount;
        }

        void moveTo(ScreenCoordsXY coords)
        {
            moveRight(coords.x - left);
            moveDown(coords.y - top);
        }

        void moveToX(int16_t x)
        {
            moveRight(x - left);
        }

        void moveToY(int16_t y)
        {
            moveDown(y - top);
        }

        bool IsVisible() const
        {
            return !(flags & OpenRCT2::WIDGET_FLAGS::IS_HIDDEN);
        }
    };

    constexpr uint8_t kCloseButtonWidth = 10;

    constexpr int32_t kScrollableRowHeight = 12;
    constexpr uint8_t kListRowHeight = 12;
    constexpr uint8_t kTableCellHeight = 12;
    constexpr uint8_t kButtonFaceHeight = 12;
    constexpr uint8_t kSpinnerHeight = 12;
    constexpr uint8_t kDropdownHeight = 12;

    constexpr uint16_t kTextInputSize = 1024;
    constexpr uint16_t kTopToolbarHeight = 27;
} // namespace OpenRCT2
