/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"
#include "../core/StringTypes.h"
#include "../drawing/ImageId.hpp"
#include "../localisation/StringIdType.h"
#include "../world/Location.hpp"

#include <cstdint>

namespace OpenRCT2
{
    using WidgetIndex = uint16_t;
    constexpr WidgetIndex kWidgetIndexNull = 0xFFFF;

    enum class WidgetType : uint8_t
    {
        empty = 0,
        frame = 1,
        resize = 2,
        imgBtn = 3,
        colourBtn = 6,
        trnBtn = 7,
        tab = 8,
        flatBtn = 9,
        /**
         * For stuff that mostly acts as a label, while providing a way for power users to quickly
         * open the associated window. For example: the money display in the lower left of the screen.
         */
        hiddenButton = 11,
        button = 10,
        labelCentred = 12, // Centred text
        tableHeader = 13,  // Left-aligned textual button
        label = 14,        // Left-aligned text
        spinner = 15,
        dropdownMenu = 16,
        viewport = 17,
        groupbox = 19,
        caption = 20,
        closeBox = 21,
        scroll = 22,
        checkbox = 23,
        placeholder = 25,
        progressBar = 29,
        custom = 28,
        textBox = 27,
        horizontalSeparator = 30,
    };

    enum class WidgetFlag : uint8_t
    {
        textIsString = 0,
        isPressed = 2,
        isDisabled = 3,
        tooltipIsString = 4,
        isHidden = 5,
        isHoldable = 6,
    };
    using WidgetFlags = FlagHolder<uint8_t, WidgetFlag>;

    enum
    {
        SCROLL_HORIZONTAL = (1 << 0),
        SCROLL_VERTICAL = (1 << 1),
        SCROLL_BOTH = SCROLL_HORIZONTAL | SCROLL_VERTICAL
    };

    constexpr const char* kCloseBoxStringBlackNormal = u8"{BLACK}❌";
    constexpr const char* kCloseBoxStringBlackLarge = u8"{BLACK}X";
    constexpr const char* kCloseBoxStringWhiteNormal = u8"{WHITE}❌";
    constexpr const char* kCloseBoxStringWhiteLarge = u8"{WHITE}X";

    struct Widget
    {
        WidgetType type{};
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
            const utf8* string;
        };
        StringId tooltip{ kStringIdNone };

        // New properties
        WidgetFlags flags{};
        utf8* sztooltip{};

        int16_t width() const
        {
            return right - left + 1;
        }

        void setWidth(int16_t newWidth)
        {
            right = left + newWidth - 1;
        }

        int16_t height() const
        {
            return bottom - top + 1;
        }

        void setHeight(int16_t newHeight)
        {
            bottom = top + newHeight - 1;
        }

        int16_t midX() const
        {
            return left + (width() / 2);
        }

        int16_t midY() const
        {
            return top + (height() / 2);
        }

        int16_t textTop() const
        {
            if (height() >= 11)
                return std::max<int32_t>(top, top + (height() / 2) - 6);

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
            return !flags.has(WidgetFlag::isHidden);
        }
    };

    constexpr uint8_t kTitleHeightNormal = 13;
    constexpr uint8_t kTitleHeightLarge = 24;

    constexpr uint8_t kCloseButtonSize = 10;
    constexpr uint8_t kCloseButtonSizeTouch = 20;

    constexpr int32_t kScrollableRowHeight = 12;
    constexpr uint8_t kListRowHeight = 12;
    constexpr uint8_t kTableCellHeight = 12;
    constexpr uint8_t kButtonFaceHeight = 12;
    constexpr uint8_t kSpinnerHeight = 12;
    constexpr uint8_t kDropdownHeight = 12;

    constexpr uint16_t kTextInputSize = 1024;
    constexpr uint16_t kTopToolbarHeight = 27;
} // namespace OpenRCT2
