/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <bitset>
#include <iterator>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>

// The maximum number of rows to list before items overflow into new columns
constexpr int32_t DropdownTextMaxRows = 32;

constexpr int32_t DropdownItemHeight = 12;

static constexpr const uint8_t AppropriateImageDropdownItemsPerRow[34] = {
    1, 1, 1, 1, 2, 2, 3, 3, 4, 3, 5, 4, 4, 5, 5, 5, 4, 5, 6, 5, 5, 7, 4, 5, 6, 5, 6, 6, 6, 6, 6, 8, 8, 8,
};

enum
{
    WIDX_BACKGROUND,
};

static rct_widget _windowDropdownWidgets[] = {
    MakeWidget({ 0, 0 }, { 1, 1 }, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};

static int32_t _dropdownNumColumns;
static int32_t _dropdownNumRows;
static int32_t _dropdownItemWidth;
static int32_t _dropdownItemHeight;
static bool _dropdownListVertically;

int32_t gDropdownNumItems;
rct_string_id gDropdownItemsFormat[Dropdown::ItemsMaxSize];
int64_t gDropdownItemsArgs[Dropdown::ItemsMaxSize];
static std::bitset<Dropdown::ItemsMaxSize> _dropdownItemsChecked = {};
static std::bitset<Dropdown::ItemsMaxSize> _dropdownItemsDisabled = {};
bool gDropdownIsColour;
int32_t gDropdownLastColourHover;
int32_t gDropdownHighlightedIndex;
int32_t gDropdownDefaultIndex;

bool Dropdown::IsChecked(int32_t index)
{
    if (index < 0 || index >= static_cast<int32_t>(std::size(_dropdownItemsDisabled)))
    {
        return false;
    }
    return _dropdownItemsChecked[index];
}

bool Dropdown::IsDisabled(int32_t index)
{
    if (index < 0 || index >= static_cast<int32_t>(std::size(_dropdownItemsDisabled)))
    {
        return true;
    }
    return _dropdownItemsDisabled[index];
}

void Dropdown::SetChecked(int32_t index, bool value)
{
    if (index < 0 || index >= static_cast<int32_t>(std::size(_dropdownItemsDisabled)))
    {
        return;
    }
    _dropdownItemsChecked[index] = value;
}

void Dropdown::SetDisabled(int32_t index, bool value)
{
    if (index < 0 || index >= static_cast<int32_t>(std::size(_dropdownItemsDisabled)))
    {
        return;
    }
    _dropdownItemsDisabled[index] = value;
}

static void WindowDropdownPaint(rct_window* w, rct_drawpixelinfo* dpi);

// clang-format off
static rct_window_event_list _windowDropdownEvents([](auto& events)
{
    events.paint = &WindowDropdownPaint;
});
// clang-format on

/**
 * Shows a text dropdown menu.
 *  rct2: 0x006ECFB9
 *
 * @param x (cx)
 * @param y (dx)
 * @param extray (di)
 * @param flags (bh)
 * @param num_items (bx)
 * @param colour (al)
 */
void WindowDropdownShowText(const ScreenCoordsXY& screenPos, int32_t extray, uint8_t colour, uint8_t flags, size_t num_items)
{
    int32_t string_width, max_string_width;
    char buffer[256];

    // Calculate the longest string width
    max_string_width = 0;
    for (size_t i = 0; i < num_items; i++)
    {
        format_string(buffer, 256, gDropdownItemsFormat[i], static_cast<void*>(&gDropdownItemsArgs[i]));
        string_width = gfx_get_string_width(buffer, FontSpriteBase::MEDIUM);
        max_string_width = std::max(string_width, max_string_width);
    }

    WindowDropdownShowTextCustomWidth(screenPos, extray, colour, 0, flags, num_items, max_string_width + 3);
}

/**
 * Shows a text dropdown menu.
 *  rct2: 0x006ECFB9, although 0x006ECE50 is real version
 *
 * @param x (cx)
 * @param y (dx)
 * @param extray (di)
 * @param flags (bh)
 * @param num_items (bx)
 * @param colour (al)
 * @param custom_height (ah) requires flag set as well
 */
void WindowDropdownShowTextCustomWidth(
    const ScreenCoordsXY& screenPos, int32_t extray, uint8_t colour, uint8_t custom_height, uint8_t flags, size_t num_items,
    int32_t width)
{
    rct_window* w;

    input_set_flag(static_cast<INPUT_FLAGS>(INPUT_FLAG_DROPDOWN_STAY_OPEN | INPUT_FLAG_DROPDOWN_MOUSE_UP), false);
    if (flags & Dropdown::Flag::StayOpen)
        input_set_flag(INPUT_FLAG_DROPDOWN_STAY_OPEN, true);

    WindowDropdownClose();

    // Set and calculate num items, rows and columns
    _dropdownItemWidth = width;
    _dropdownItemHeight = (flags & Dropdown::Flag::CustomHeight) ? custom_height : DropdownItemHeight;
    gDropdownNumItems = static_cast<int32_t>(num_items);
    // There must always be at least one column to prevent dividing by zero
    if (gDropdownNumItems == 0)
    {
        _dropdownNumColumns = 1;
        _dropdownNumRows = 1;
    }
    else
    {
        _dropdownNumColumns = (gDropdownNumItems + DropdownTextMaxRows - 1) / DropdownTextMaxRows;
        _dropdownNumRows = (gDropdownNumItems + _dropdownNumColumns - 1) / _dropdownNumColumns;
    }

    // Text dropdowns are listed horizontally
    _dropdownListVertically = true;

    width = _dropdownItemWidth * _dropdownNumColumns + 3;
    int32_t height = _dropdownItemHeight * _dropdownNumRows + 3;
    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();
    auto boundedScreenPos = screenPos;
    if (screenPos.x + width > screenWidth)
        boundedScreenPos.x = std::max(0, screenWidth - width);
    if (screenPos.y + height > screenHeight)
        boundedScreenPos.y = std::max(0, screenHeight - height);

    _windowDropdownWidgets[WIDX_BACKGROUND].right = width;
    _windowDropdownWidgets[WIDX_BACKGROUND].bottom = height;

    // Create the window
    w = WindowCreate(
        boundedScreenPos + ScreenCoordsXY{ 0, extray }, _windowDropdownWidgets[WIDX_BACKGROUND].right + 1,
        _windowDropdownWidgets[WIDX_BACKGROUND].bottom + 1, &_windowDropdownEvents, WC_DROPDOWN, WF_STICK_TO_FRONT);
    w->widgets = _windowDropdownWidgets;
    if (colour & COLOUR_FLAG_TRANSLUCENT)
        w->flags |= WF_TRANSPARENT;
    w->colours[0] = colour;

    // Input state
    gDropdownHighlightedIndex = -1;
    _dropdownItemsDisabled.reset();
    _dropdownItemsChecked.reset();
    gDropdownIsColour = false;
    gDropdownDefaultIndex = -1;
    input_set_state(InputState::DropdownActive);
}

/**
 * Shows an image dropdown menu.
 *  rct2: 0x006ECFB9
 *
 * @param x (cx)
 * @param y (dx)
 * @param extray (di)
 * @param flags (bh)
 * @param numItems (bx)
 * @param colour (al)
 * @param itemWidth (bp)
 * @param itemHeight (ah)
 * @param numColumns (bl)
 */
void WindowDropdownShowImage(
    int32_t x, int32_t y, int32_t extray, uint8_t colour, uint8_t flags, int32_t numItems, int32_t itemWidth,
    int32_t itemHeight, int32_t numColumns)
{
    int32_t width, height;
    rct_window* w;

    input_set_flag(static_cast<INPUT_FLAGS>(INPUT_FLAG_DROPDOWN_STAY_OPEN | INPUT_FLAG_DROPDOWN_MOUSE_UP), false);
    if (flags & Dropdown::Flag::StayOpen)
        input_set_flag(INPUT_FLAG_DROPDOWN_STAY_OPEN, true);

    // Close existing dropdown
    WindowDropdownClose();

    // Set and calculate num items, rows and columns
    _dropdownItemWidth = itemWidth;
    _dropdownItemHeight = itemHeight;
    gDropdownNumItems = numItems;
    // There must always be at least one column and row to prevent dividing by zero
    if (gDropdownNumItems == 0)
    {
        _dropdownNumColumns = 1;
        _dropdownNumRows = 1;
    }
    else
    {
        _dropdownNumColumns = std::max(1, numColumns);
        _dropdownNumRows = gDropdownNumItems / _dropdownNumColumns;
        if (gDropdownNumItems % _dropdownNumColumns != 0)
            _dropdownNumRows++;
    }

    // image dropdowns are listed horizontally
    _dropdownListVertically = false;

    // Calculate position and size
    width = _dropdownItemWidth * _dropdownNumColumns + 3;
    height = _dropdownItemHeight * _dropdownNumRows + 3;

    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();
    if (x + width > screenWidth)
        x = std::max(0, screenWidth - width);
    if (y + height > screenHeight)
        y = std::max(0, screenHeight - height);
    _windowDropdownWidgets[WIDX_BACKGROUND].right = width;
    _windowDropdownWidgets[WIDX_BACKGROUND].bottom = height;

    // Create the window
    w = WindowCreate(
        ScreenCoordsXY(x, y + extray), _windowDropdownWidgets[WIDX_BACKGROUND].right + 1,
        _windowDropdownWidgets[WIDX_BACKGROUND].bottom + 1, &_windowDropdownEvents, WC_DROPDOWN, WF_STICK_TO_FRONT);
    w->widgets = _windowDropdownWidgets;
    if (colour & COLOUR_FLAG_TRANSLUCENT)
        w->flags |= WF_TRANSPARENT;
    w->colours[0] = colour;

    // Input state
    gDropdownHighlightedIndex = -1;
    _dropdownItemsDisabled.reset();
    _dropdownItemsChecked.reset();
    gDropdownIsColour = false;
    gDropdownDefaultIndex = -1;
    input_set_state(InputState::DropdownActive);
}

void WindowDropdownClose()
{
    window_close_by_class(WC_DROPDOWN);
}

static void WindowDropdownPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    int32_t highlightedIndex = gDropdownHighlightedIndex;
    for (int32_t i = 0; i < gDropdownNumItems; i++)
    {
        ScreenCoordsXY cellCoords;
        if (_dropdownListVertically)
            cellCoords = { i / _dropdownNumRows, i % _dropdownNumRows };
        else
            cellCoords = { i % _dropdownNumColumns, i / _dropdownNumColumns };

        ScreenCoordsXY screenCoords = w->windowPos
            + ScreenCoordsXY{ 2 + (cellCoords.x * _dropdownItemWidth), 2 + (cellCoords.y * _dropdownItemHeight) };

        if (gDropdownItemsFormat[i] == Dropdown::SeparatorString)
        {
            const ScreenCoordsXY leftTop = screenCoords + ScreenCoordsXY{ 0, (_dropdownItemHeight / 2) };
            const ScreenCoordsXY rightBottom = leftTop + ScreenCoordsXY{ _dropdownItemWidth - 1, 0 };
            const ScreenCoordsXY shadowOffset{ 0, 1 };

            if (w->colours[0] & COLOUR_FLAG_TRANSLUCENT)
            {
                translucent_window_palette palette = TranslucentWindowPalettes[BASE_COLOUR(w->colours[0])];
                gfx_filter_rect(dpi, { leftTop, rightBottom }, palette.highlight);
                gfx_filter_rect(dpi, { leftTop + shadowOffset, rightBottom + shadowOffset }, palette.shadow);
            }
            else
            {
                gfx_fill_rect(dpi, { leftTop, rightBottom }, ColourMapA[w->colours[0]].mid_dark);
                gfx_fill_rect(dpi, { leftTop + shadowOffset, rightBottom + shadowOffset }, ColourMapA[w->colours[0]].lightest);
            }
        }
        else
        {
            if (i == highlightedIndex)
            {
                // Darken the cell's background slightly when highlighted
                const ScreenCoordsXY rightBottom = screenCoords
                    + ScreenCoordsXY{ _dropdownItemWidth - 1, _dropdownItemHeight - 1 };
                gfx_filter_rect(dpi, { screenCoords, rightBottom }, FilterPaletteID::PaletteDarken3);
            }

            rct_string_id item = gDropdownItemsFormat[i];
            if (item == Dropdown::FormatLandPicker || item == Dropdown::FormatColourPicker)
            {
                // Image item
                auto image = static_cast<uint32_t>(gDropdownItemsArgs[i]);
                if (item == Dropdown::FormatColourPicker && highlightedIndex == i)
                    image++;

                gfx_draw_sprite(dpi, ImageId::FromUInt32(image), screenCoords);
            }
            else
            {
                // Text item
                if (i < Dropdown::ItemsMaxSize && Dropdown::IsChecked(i))
                    item++;

                // Calculate colour
                colour_t colour = NOT_TRANSLUCENT(w->colours[0]);
                if (i == highlightedIndex)
                    colour = COLOUR_WHITE;
                if (i < Dropdown::ItemsMaxSize && Dropdown::IsDisabled(i))
                    colour = NOT_TRANSLUCENT(w->colours[0]) | COLOUR_FLAG_INSET;

                // Draw item string
                Formatter ft(reinterpret_cast<uint8_t*>(&gDropdownItemsArgs[i]));
                DrawTextEllipsised(dpi, screenCoords, w->width - 5, item, ft, { colour });
            }
        }
    }
}

/**
 * New function based on 6e914e
 * returns -1 if index is invalid
 */
int32_t DropdownIndexFromPoint(const ScreenCoordsXY& loc, rct_window* w)
{
    int32_t top = loc.y - w->windowPos.y - 2;
    if (top < 0)
        return -1;

    int32_t left = loc.x - w->windowPos.x;
    if (left >= w->width)
        return -1;
    left -= 2;
    if (left < 0)
        return -1;

    int32_t column_no = left / _dropdownItemWidth;
    if (column_no >= _dropdownNumColumns)
        return -1;

    int32_t row_no = top / _dropdownItemHeight;
    if (row_no >= _dropdownNumRows)
        return -1;

    int32_t dropdown_index;
    if (_dropdownListVertically)
        dropdown_index = column_no * _dropdownNumRows + row_no;
    else
        dropdown_index = row_no * _dropdownNumColumns + column_no;

    if (dropdown_index >= gDropdownNumItems)
        return -1;

    return dropdown_index;
}

/**
 *  rct2: 0x006ED43D
 */
void WindowDropdownShowColour(rct_window* w, rct_widget* widget, uint8_t dropdownColour, uint8_t selectedColour)
{
    int32_t defaultIndex = -1;
    // Set items
    for (uint64_t i = 0; i < COLOUR_COUNT; i++)
    {
        if (selectedColour == i)
            defaultIndex = i;

        gDropdownItemsFormat[i] = Dropdown::FormatColourPicker;
        gDropdownItemsArgs[i] = (i << 32) | (SPRITE_ID_PALETTE_COLOUR_1(i) | SPR_PALETTE_BTN);
    }

    // Show dropdown
    WindowDropdownShowImage(
        w->windowPos.x + widget->left, w->windowPos.y + widget->top, widget->height() + 1, dropdownColour,
        Dropdown::Flag::StayOpen, COLOUR_COUNT, 12, 12, AppropriateImageDropdownItemsPerRow[COLOUR_COUNT]);

    gDropdownIsColour = true;
    gDropdownLastColourHover = -1;
    gDropdownDefaultIndex = defaultIndex;
}

uint32_t DropdownGetAppropriateImageDropdownItemsPerRow(uint32_t numItems)
{
    return numItems < std::size(AppropriateImageDropdownItemsPerRow) ? AppropriateImageDropdownItemsPerRow[numItems] : 8;
}
