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
#include <openrct2/core/BitSet.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>

using namespace OpenRCT2;

// The maximum number of rows to list before items overflow into new columns
constexpr int32_t DROPDOWN_TEXT_MAX_ROWS = 32;

constexpr int32_t DROPDOWN_ITEM_HEIGHT = 12;

static constexpr const uint8_t _appropriateImageDropdownItemsPerRow[34] = {
    1, 1, 1, 1, 2, 2, 3, 3, 4, 3, 5, 4, 4, 5, 5, 5, 4, 5, 6, 5, 5, 7, 4, 5, 6, 5, 6, 6, 6, 6, 6, 8, 8, 8,
};

enum
{
    WIDX_BACKGROUND,
};

static rct_widget window_dropdown_widgets[] = {
    MakeWidget({ 0, 0 }, { 1, 1 }, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};

static int32_t _dropdown_num_columns;
static int32_t _dropdown_num_rows;
static int32_t _dropdown_item_width;
static int32_t _dropdown_item_height;
static bool _dropdown_list_vertically;

int32_t gDropdownNumItems;
Dropdown::Item gDropdownItems[Dropdown::ItemsMaxSize];
static ImageId _dropdownItemsImages[Dropdown::ItemsMaxSize];
static BitSet<Dropdown::ItemsMaxSize> _dropdownItemsChecked = {};
static BitSet<Dropdown::ItemsMaxSize> _dropdownItemsDisabled = {};
bool gDropdownIsColour;
int32_t gDropdownLastColourHover;
int32_t gDropdownHighlightedIndex;
int32_t gDropdownDefaultIndex;
static bool _dropdownPrepareUseImages;
static bool _dropdownUseImages;

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

void Dropdown::SetImage(int32_t index, ImageId image)
{
    if (index < 0 || index >= static_cast<int32_t>(std::size(_dropdownItemsImages)))
    {
        return;
    }
    _dropdownItemsImages[index] = image;
    _dropdownPrepareUseImages = true;
}

static void WindowDropdownPaint(rct_window* w, rct_drawpixelinfo* dpi);

// clang-format off
static rct_window_event_list window_dropdown_events([](auto& events)
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
        format_string(buffer, 256, gDropdownItems[i].Format, static_cast<void*>(&gDropdownItems[i].Args));
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
    _dropdown_item_width = width;
    _dropdown_item_height = (flags & Dropdown::Flag::CustomHeight) ? custom_height : DROPDOWN_ITEM_HEIGHT;
    gDropdownNumItems = static_cast<int32_t>(num_items);
    // There must always be at least one column to prevent dividing by zero
    if (gDropdownNumItems == 0)
    {
        _dropdown_num_columns = 1;
        _dropdown_num_rows = 1;
    }
    else
    {
        _dropdown_num_columns = (gDropdownNumItems + DROPDOWN_TEXT_MAX_ROWS - 1) / DROPDOWN_TEXT_MAX_ROWS;
        _dropdown_num_rows = (gDropdownNumItems + _dropdown_num_columns - 1) / _dropdown_num_columns;
    }

    // Text dropdowns are listed horizontally
    _dropdown_list_vertically = true;

    width = _dropdown_item_width * _dropdown_num_columns + 3;
    int32_t height = _dropdown_item_height * _dropdown_num_rows + 3;
    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();
    auto boundedScreenPos = screenPos;
    if (screenPos.x + width > screenWidth)
        boundedScreenPos.x = std::max(0, screenWidth - width);
    if (screenPos.y + height > screenHeight)
        boundedScreenPos.y = std::max(0, screenHeight - height);

    window_dropdown_widgets[WIDX_BACKGROUND].right = width;
    window_dropdown_widgets[WIDX_BACKGROUND].bottom = height;

    // Create the window
    w = WindowCreate(
        boundedScreenPos + ScreenCoordsXY{ 0, extray }, window_dropdown_widgets[WIDX_BACKGROUND].right + 1,
        window_dropdown_widgets[WIDX_BACKGROUND].bottom + 1, &window_dropdown_events, WC_DROPDOWN, WF_STICK_TO_FRONT);
    w->widgets = window_dropdown_widgets;
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

    if (_dropdownPrepareUseImages)
    {
        _dropdownPrepareUseImages = false;
        _dropdownUseImages = true;
    }
    else
    {
        _dropdownUseImages = false;
    }

    // Set and calculate num items, rows and columns
    _dropdown_item_width = itemWidth;
    _dropdown_item_height = itemHeight;
    gDropdownNumItems = numItems;
    // There must always be at least one column and row to prevent dividing by zero
    if (gDropdownNumItems == 0)
    {
        _dropdown_num_columns = 1;
        _dropdown_num_rows = 1;
    }
    else
    {
        _dropdown_num_columns = std::max(1, numColumns);
        _dropdown_num_rows = gDropdownNumItems / _dropdown_num_columns;
        if (gDropdownNumItems % _dropdown_num_columns != 0)
            _dropdown_num_rows++;
    }

    // image dropdowns are listed horizontally
    _dropdown_list_vertically = false;

    // Calculate position and size
    width = _dropdown_item_width * _dropdown_num_columns + 3;
    height = _dropdown_item_height * _dropdown_num_rows + 3;

    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();
    if (x + width > screenWidth)
        x = std::max(0, screenWidth - width);
    if (y + height > screenHeight)
        y = std::max(0, screenHeight - height);
    window_dropdown_widgets[WIDX_BACKGROUND].right = width;
    window_dropdown_widgets[WIDX_BACKGROUND].bottom = height;

    // Create the window
    w = WindowCreate(
        ScreenCoordsXY(x, y + extray), window_dropdown_widgets[WIDX_BACKGROUND].right + 1,
        window_dropdown_widgets[WIDX_BACKGROUND].bottom + 1, &window_dropdown_events, WC_DROPDOWN, WF_STICK_TO_FRONT);
    w->widgets = window_dropdown_widgets;
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
        if (_dropdown_list_vertically)
            cellCoords = { i / _dropdown_num_rows, i % _dropdown_num_rows };
        else
            cellCoords = { i % _dropdown_num_columns, i / _dropdown_num_columns };

        ScreenCoordsXY screenCoords = w->windowPos
            + ScreenCoordsXY{ 2 + (cellCoords.x * _dropdown_item_width), 2 + (cellCoords.y * _dropdown_item_height) };

        if (gDropdownItems[i].IsSeparator())
        {
            const ScreenCoordsXY leftTop = screenCoords + ScreenCoordsXY{ 0, (_dropdown_item_height / 2) };
            const ScreenCoordsXY rightBottom = leftTop + ScreenCoordsXY{ _dropdown_item_width - 1, 0 };
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
                    + ScreenCoordsXY{ _dropdown_item_width - 1, _dropdown_item_height - 1 };
                gfx_filter_rect(dpi, { screenCoords, rightBottom }, FilterPaletteID::PaletteDarken3);
            }

            rct_string_id item = gDropdownItems[i].Format;
            if (item == Dropdown::FormatLandPicker || item == Dropdown::FormatColourPicker)
            {
                // Image item
                auto image = _dropdownUseImages ? _dropdownItemsImages[i]
                                                : ImageId::FromUInt32(static_cast<uint32_t>(gDropdownItems[i].Args));
                if (item == Dropdown::FormatColourPicker && highlightedIndex == i)
                    image = image.WithIndexOffset(1);
                gfx_draw_sprite(dpi, image, screenCoords);
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
                Formatter ft(reinterpret_cast<uint8_t*>(&gDropdownItems[i].Args));
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

    int32_t column_no = left / _dropdown_item_width;
    if (column_no >= _dropdown_num_columns)
        return -1;

    int32_t row_no = top / _dropdown_item_height;
    if (row_no >= _dropdown_num_rows)
        return -1;

    int32_t dropdown_index;
    if (_dropdown_list_vertically)
        dropdown_index = column_no * _dropdown_num_rows + row_no;
    else
        dropdown_index = row_no * _dropdown_num_columns + column_no;

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

        gDropdownItems[i].Format = Dropdown::FormatColourPicker;
        gDropdownItems[i].Args = (i << 32) | (SPRITE_ID_PALETTE_COLOUR_1(i) | SPR_PALETTE_BTN);
    }

    // Show dropdown
    WindowDropdownShowImage(
        w->windowPos.x + widget->left, w->windowPos.y + widget->top, widget->height() + 1, dropdownColour,
        Dropdown::Flag::StayOpen, COLOUR_COUNT, 12, 12, _appropriateImageDropdownItemsPerRow[COLOUR_COUNT]);

    gDropdownIsColour = true;
    gDropdownLastColourHover = -1;
    gDropdownDefaultIndex = defaultIndex;
}

uint32_t DropdownGetAppropriateImageDropdownItemsPerRow(uint32_t numItems)
{
    return numItems < std::size(_appropriateImageDropdownItemsPerRow) ? _appropriateImageDropdownItemsPerRow[numItems] : 8;
}
