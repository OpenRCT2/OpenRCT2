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
    1, 1, 1, 1, 2, 2, 3, 3, 4, 3, // 10
    5, 4, 4, 5, 5, 5, 4, 5, 6, 5, // 20
    5, 7, 4, 5, 6, 5, 6, 6, 6, 6, // 30
    6, 8, 8, 8,                   // 34
};

enum
{
    WIDX_BACKGROUND,
};

static rct_widget window_dropdown_widgets[] = {
    MakeWidget({ 0, 0 }, { 1, 1 }, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};

int32_t gDropdownNumItems;
Dropdown::Item gDropdownItems[Dropdown::ItemsMaxSize];
static ImageId _dropdownItemsImages[Dropdown::ItemsMaxSize];
bool gDropdownIsColour;
int32_t gDropdownLastColourHover;
int32_t gDropdownHighlightedIndex;
int32_t gDropdownDefaultIndex;
static bool _dropdownPrepareUseImages;

static void ResetDropdownFlags()
{
    for (size_t i = 0; i < std::size(gDropdownItems); i++)
    {
        gDropdownItems[i].Flags = 0;
    }
}

bool Dropdown::IsChecked(int32_t index)
{
    if (index < 0 || index >= static_cast<int32_t>(std::size(gDropdownItems)))
    {
        return false;
    }
    return gDropdownItems[index].IsChecked();
}

bool Dropdown::IsDisabled(int32_t index)
{
    if (index < 0 || index >= static_cast<int32_t>(std::size(gDropdownItems)))
    {
        return true;
    }
    return gDropdownItems[index].IsDisabled();
}

void Dropdown::SetChecked(int32_t index, bool value)
{
    if (index < 0 || index >= static_cast<int32_t>(std::size(gDropdownItems)))
    {
        return;
    }
    if (value)
        gDropdownItems[index].Flags |= EnumValue(Dropdown::ItemFlag::IsChecked);
    else
        gDropdownItems[index].Flags &= ~EnumValue(Dropdown::ItemFlag::IsChecked);
}

void Dropdown::SetDisabled(int32_t index, bool value)
{
    if (index < 0 || index >= static_cast<int32_t>(std::size(gDropdownItems)))
    {
        return;
    }
    if (value)
        gDropdownItems[index].Flags |= EnumValue(Dropdown::ItemFlag::IsDisabled);
    else
        gDropdownItems[index].Flags &= ~EnumValue(Dropdown::ItemFlag::IsDisabled);
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

class DropdownWindow final : public Window
{
    bool UseImages;
    int32_t NumColumns;
    int32_t NumRows;
    int32_t ItemWidth;
    int32_t ItemHeight;
    bool ListVertically;

public:
    void OnOpen() override
    {
        widgets = window_dropdown_widgets;

        // Input state
        gDropdownHighlightedIndex = -1;
        ResetDropdownFlags();
        gDropdownIsColour = false;
        gDropdownDefaultIndex = -1;
        input_set_state(InputState::DropdownActive);
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);

        int32_t highlightedIndex = gDropdownHighlightedIndex;
        for (int32_t i = 0; i < gDropdownNumItems; i++)
        {
            ScreenCoordsXY cellCoords;
            if (ListVertically)
                cellCoords = { i / NumRows, i % NumRows };
            else
                cellCoords = { i % NumColumns, i / NumColumns };

            ScreenCoordsXY screenCoords = windowPos
                + ScreenCoordsXY{ 2 + (cellCoords.x * ItemWidth), 2 + (cellCoords.y * ItemHeight) };

            if (gDropdownItems[i].IsSeparator())
            {
                const ScreenCoordsXY leftTop = screenCoords + ScreenCoordsXY{ 0, (ItemHeight / 2) };
                const ScreenCoordsXY rightBottom = leftTop + ScreenCoordsXY{ ItemWidth - 1, 0 };
                const ScreenCoordsXY shadowOffset{ 0, 1 };

                if (colours[0] & COLOUR_FLAG_TRANSLUCENT)
                {
                    translucent_window_palette palette = TranslucentWindowPalettes[BASE_COLOUR(colours[0])];
                    gfx_filter_rect(&dpi, { leftTop, rightBottom }, palette.highlight);
                    gfx_filter_rect(&dpi, { leftTop + shadowOffset, rightBottom + shadowOffset }, palette.shadow);
                }
                else
                {
                    gfx_fill_rect(&dpi, { leftTop, rightBottom }, ColourMapA[colours[0]].mid_dark);
                    gfx_fill_rect(
                        &dpi, { leftTop + shadowOffset, rightBottom + shadowOffset }, ColourMapA[colours[0]].lightest);
                }
            }
            else
            {
                if (i == highlightedIndex)
                {
                    // Darken the cell's background slightly when highlighted
                    const ScreenCoordsXY rightBottom = screenCoords + ScreenCoordsXY{ ItemWidth - 1, ItemHeight - 1 };
                    gfx_filter_rect(&dpi, { screenCoords, rightBottom }, FilterPaletteID::PaletteDarken3);
                }

                StringId item = gDropdownItems[i].Format;
                if (item == Dropdown::FormatLandPicker || item == Dropdown::FormatColourPicker)
                {
                    // Image item
                    auto image = UseImages ? _dropdownItemsImages[i]
                                           : ImageId::FromUInt32(static_cast<uint32_t>(gDropdownItems[i].Args));
                    if (item == Dropdown::FormatColourPicker && highlightedIndex == i)
                        image = image.WithIndexOffset(1);
                    gfx_draw_sprite(&dpi, image, screenCoords);
                }
                else
                {
                    // Text item
                    if (i < Dropdown::ItemsMaxSize && Dropdown::IsChecked(i))
                        item++;

                    // Calculate colour
                    colour_t colour = NOT_TRANSLUCENT(colours[0]);
                    if (i == highlightedIndex)
                        colour = COLOUR_WHITE;
                    if (i < Dropdown::ItemsMaxSize && Dropdown::IsDisabled(i))
                        colour = NOT_TRANSLUCENT(colours[0]) | COLOUR_FLAG_INSET;

                    // Draw item string
                    Formatter ft(reinterpret_cast<uint8_t*>(&gDropdownItems[i].Args));
                    DrawTextEllipsised(&dpi, screenCoords, width - 5, item, ft, { colour });
                }
            }
        }
    }

    void SetTextItems(
        const ScreenCoordsXY& screenPos, int32_t extraY, uint8_t colour, uint8_t customHeight, uint8_t txtFlags,
        size_t numItems, int32_t itemWidth)
    {
        // Set and calculate num items, rows and columns
        ItemWidth = itemWidth;
        ItemHeight = (txtFlags & Dropdown::Flag::CustomHeight) ? customHeight : DROPDOWN_ITEM_HEIGHT;
        gDropdownNumItems = static_cast<int32_t>(numItems);
        // There must always be at least one column to prevent dividing by zero
        if (gDropdownNumItems == 0)
        {
            NumColumns = 1;
            NumRows = 1;
        }
        else
        {
            NumColumns = (gDropdownNumItems + DROPDOWN_TEXT_MAX_ROWS - 1) / DROPDOWN_TEXT_MAX_ROWS;
            NumRows = (gDropdownNumItems + NumColumns - 1) / NumColumns;
        }

        // Text dropdowns are listed horizontally
        ListVertically = true;

        UpdateSizeAndPosition(screenPos, extraY);

        if (colour & COLOUR_FLAG_TRANSLUCENT)
            flags |= WF_TRANSPARENT;
        colours[0] = colour;
    }

    void SetImageItems(
        const ScreenCoordsXY& screenPos, int32_t extraY, uint8_t colour, int32_t numItems, int32_t itemWidth,
        int32_t itemHeight, int32_t numColumns)
    {
        UseImages = _dropdownPrepareUseImages;
        _dropdownPrepareUseImages = false;

        // Set and calculate num items, rows and columns
        ItemWidth = itemWidth;
        ItemHeight = itemHeight;
        gDropdownNumItems = numItems;
        // There must always be at least one column and row to prevent dividing by zero
        if (gDropdownNumItems == 0)
        {
            NumColumns = 1;
            NumRows = 1;
        }
        else
        {
            NumColumns = std::max(1, numColumns);
            NumRows = gDropdownNumItems / NumColumns;
            if (gDropdownNumItems % NumColumns != 0)
                NumRows++;
        }

        // image dropdowns are listed horizontally
        ListVertically = false;

        UpdateSizeAndPosition(screenPos, extraY);

        if (colour & COLOUR_FLAG_TRANSLUCENT)
            flags |= WF_TRANSPARENT;
        colours[0] = colour;
    }

    int32_t GetIndexFromPoint(const ScreenCoordsXY& loc)
    {
        int32_t top = loc.y - windowPos.y - 2;
        if (top < 0)
            return -1;

        int32_t left = loc.x - windowPos.x;
        if (left >= width)
            return -1;
        left -= 2;
        if (left < 0)
            return -1;

        int32_t columnNum = left / ItemWidth;
        if (columnNum >= NumColumns)
            return -1;

        int32_t rowNum = top / ItemHeight;
        if (rowNum >= NumRows)
            return -1;

        int32_t dropdownIndex;
        if (ListVertically)
            dropdownIndex = columnNum * NumRows + rowNum;
        else
            dropdownIndex = rowNum * NumColumns + columnNum;

        if (dropdownIndex >= gDropdownNumItems)
            return -1;

        return dropdownIndex;
    }

private:
    void UpdateSizeAndPosition(const ScreenCoordsXY& screenPos, const int32_t extraY)
    {
        // Calculate position and size
        const auto ddWidth = ItemWidth * NumColumns + 3;
        const auto ddHeight = ItemHeight * NumRows + 3;

        int32_t screenWidth = context_get_width();
        int32_t screenHeight = context_get_height();
        auto boundedScreenPos = screenPos;
        if (screenPos.x + ddWidth > screenWidth)
            boundedScreenPos.x = std::max(0, screenWidth - ddWidth);
        if (screenPos.y + ddHeight > screenHeight)
            boundedScreenPos.y = std::max(0, screenHeight - ddHeight);
        window_dropdown_widgets[WIDX_BACKGROUND].right = ddWidth;
        window_dropdown_widgets[WIDX_BACKGROUND].bottom = ddHeight;

        Invalidate();
        width = ddWidth + 1;
        height = ddHeight + 1;
        windowPos = boundedScreenPos + ScreenCoordsXY{ 0, extraY };
        Invalidate();
    }
};

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
    input_set_flag(static_cast<INPUT_FLAGS>(INPUT_FLAG_DROPDOWN_STAY_OPEN | INPUT_FLAG_DROPDOWN_MOUSE_UP), false);
    if (flags & Dropdown::Flag::StayOpen)
        input_set_flag(INPUT_FLAG_DROPDOWN_STAY_OPEN, true);

    WindowDropdownClose();

    // Create the window (width/height position are set later)
    auto* w = WindowCreate<DropdownWindow>(WC_DROPDOWN, width, custom_height, WF_STICK_TO_FRONT);
    if (w != nullptr)
    {
        w->SetTextItems(screenPos, extray, colour, custom_height, flags, num_items, width);
    }
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
    input_set_flag(static_cast<INPUT_FLAGS>(INPUT_FLAG_DROPDOWN_STAY_OPEN | INPUT_FLAG_DROPDOWN_MOUSE_UP), false);
    if (flags & Dropdown::Flag::StayOpen)
        input_set_flag(INPUT_FLAG_DROPDOWN_STAY_OPEN, true);

    // Close existing dropdown
    WindowDropdownClose();

    // Create the window (width/height position are set later)
    auto* w = WindowCreate<DropdownWindow>(WC_DROPDOWN, itemWidth, itemHeight, WF_STICK_TO_FRONT);
    if (w != nullptr)
    {
        w->SetImageItems({ x, y }, extray, colour, numItems, itemWidth, itemHeight, numColumns);
    }
}

void WindowDropdownClose()
{
    window_close_by_class(WC_DROPDOWN);
}

/**
 * New function based on 6e914e
 * returns -1 if index is invalid
 */
int32_t DropdownIndexFromPoint(const ScreenCoordsXY& loc, rct_window* w)
{
    if (w->classification == WC_DROPDOWN)
    {
        auto* ddWnd = static_cast<DropdownWindow*>(w);
        return ddWnd->GetIndexFromPoint(loc);
    }
    return -1;
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
