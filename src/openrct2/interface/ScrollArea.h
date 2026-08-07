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

#include <cstdint>

namespace OpenRCT2
{
    enum class ScrollFlag : uint8_t
    {
        hScrollbarVisible,
        hScrollbarThumbPressed,
        hScrollbarLeftPressed,
        hScrollbarRightPressed,
        vScrollbarVisible,
        vScrollbarThumbPressed,
        vScrollbarUpPressed,
        vScrollbarDownPressed,
    };
    using ScrollFlags = FlagHolder<uint16_t, ScrollFlag>;

    struct ScrollArea
    {
        ScrollFlags flags{};
        int32_t contentOffsetX{};
        int32_t contentWidth{};
        int32_t hThumbLeft{};
        int32_t hThumbRight{};
        int32_t contentOffsetY{};
        int32_t contentHeight{};
        int32_t vThumbTop{};
        int32_t vThumbBottom{};
    };

    enum
    {
        SCROLL_PART_NONE = -1,
        SCROLL_PART_VIEW = 0,
        SCROLL_PART_HSCROLLBAR_LEFT = 1,
        SCROLL_PART_HSCROLLBAR_RIGHT = 2,
        SCROLL_PART_HSCROLLBAR_LEFT_TROUGH = 3,
        SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH = 4,
        SCROLL_PART_HSCROLLBAR_THUMB = 5,
        SCROLL_PART_VSCROLLBAR_TOP = 6,
        SCROLL_PART_VSCROLLBAR_BOTTOM = 7,
        SCROLL_PART_VSCROLLBAR_TOP_TROUGH = 8,
        SCROLL_PART_VSCROLLBAR_BOTTOM_TROUGH = 9,
        SCROLL_PART_VSCROLLBAR_THUMB = 10,
    };
} // namespace OpenRCT2
