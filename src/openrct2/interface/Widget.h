/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

struct WindowBase;

using WidgetIndex = int16_t;

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
    Last = 26,
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

enum
{
    SCROLL_HORIZONTAL = (1 << 0),
    SCROLL_VERTICAL = (1 << 1),
    SCROLL_BOTH = SCROLL_HORIZONTAL | SCROLL_VERTICAL
};

void WidgetScrollUpdateThumbs(WindowBase& w, WidgetIndex widget_index);
