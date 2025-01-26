/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/BitSet.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    constexpr int32_t kDropdownItemHeight = 12;
    constexpr int32_t kDropdownItemHeightTouch = 24;

    static constexpr std::array<uint8_t, 57> kAppropriateImageDropdownItemsPerRow = {
        1, 1, 1, 1, 2, 2, 3, 3, 4, 3, // 10
        5, 4, 4, 5, 5, 5, 4, 5, 6, 5, // 20
        5, 7, 4, 5, 6, 5, 6, 6, 6, 6, // 30
        6, 8, 8, 8, 9, 9, 9, 9, 9, 9, // 40
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, // 50
        9, 9, 9, 9, 9, 9, 9,          // 56
    };

    enum
    {
        WIDX_BACKGROUND,
    };

    static constexpr Widget kWindowDropdownWidgets[] = {
        MakeWidget({ 0, 0 }, { 1, 1 }, WindowWidgetType::ImgBtn, WindowColour::Primary),
    };

    int32_t gDropdownNumItems;
    Dropdown::Item gDropdownItems[Dropdown::kItemsMaxSize];
    static ImageId _dropdownItemsImages[Dropdown::kItemsMaxSize];
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
            SetWidgets(kWindowDropdownWidgets);

            // Input state
            gDropdownHighlightedIndex = -1;
            ResetDropdownFlags();
            gDropdownIsColour = false;
            gDropdownDefaultIndex = -1;
            InputSetState(InputState::DropdownActive);
        }

        static int32_t GetDefaultRowHeight()
        {
            return Config::Get().interface.EnlargedUi ? kDropdownItemHeightTouch : kDropdownItemHeight;
        }

        static int32_t GetAdditionalRowPadding()
        {
            return Config::Get().interface.EnlargedUi ? 6 : 0;
        }

        void OnDraw(DrawPixelInfo& dpi) override
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
                    const auto leftTop = screenCoords + ScreenCoordsXY{ 2, (ItemHeight / 2) - 1 };
                    const auto rightBottom = leftTop + ScreenCoordsXY{ ItemWidth - 4, 0 };
                    const auto shadowOffset = ScreenCoordsXY{ 0, 1 };

                    if (colours[0].hasFlag(ColourFlag::translucent))
                    {
                        TranslucentWindowPalette palette = TranslucentWindowPalettes[colours[0].colour];
                        GfxFilterRect(dpi, { leftTop, rightBottom }, palette.highlight);
                        GfxFilterRect(dpi, { leftTop + shadowOffset, rightBottom + shadowOffset }, palette.shadow);
                    }
                    else
                    {
                        GfxFillRect(dpi, { leftTop, rightBottom }, ColourMapA[colours[0].colour].mid_dark);
                        GfxFillRect(
                            dpi, { leftTop + shadowOffset, rightBottom + shadowOffset },
                            ColourMapA[colours[0].colour].lightest);
                    }
                }
                else
                {
                    if (i == highlightedIndex)
                    {
                        // Darken the cell's background slightly when highlighted
                        const ScreenCoordsXY rightBottom = screenCoords + ScreenCoordsXY{ ItemWidth - 1, ItemHeight - 1 };
                        GfxFilterRect(dpi, { screenCoords, rightBottom }, FilterPaletteID::PaletteDarken3);
                    }

                    StringId item = gDropdownItems[i].Format;
                    if (item == Dropdown::kFormatLandPicker || item == Dropdown::kFormatColourPicker)
                    {
                        // Image item
                        auto image = UseImages ? _dropdownItemsImages[i]
                                               : ImageId(static_cast<uint32_t>(gDropdownItems[i].Args));
                        if (item == Dropdown::kFormatColourPicker && highlightedIndex == i)
                            image = image.WithIndexOffset(1);
                        GfxDrawSprite(dpi, image, screenCoords);
                    }
                    else
                    {
                        // Text item
                        if (i < Dropdown::kItemsMaxSize && Dropdown::IsChecked(i))
                            item++;

                        // Calculate colour
                        ColourWithFlags colour = { colours[0].colour };
                        if (i == highlightedIndex)
                            colour.colour = COLOUR_WHITE;
                        if (i < Dropdown::kItemsMaxSize && Dropdown::IsDisabled(i))
                            colour = { colours[0].colour, EnumToFlag(ColourFlag::inset) };

                        // Draw item string
                        auto yOffset = GetAdditionalRowPadding();
                        Formatter ft(reinterpret_cast<uint8_t*>(&gDropdownItems[i].Args));
                        DrawTextEllipsised(
                            dpi, { screenCoords.x + 2, screenCoords.y + yOffset }, width - 7, item, ft, { colour });
                    }
                }
            }
        }

        static int32_t getSpaceUntilBottom(const ScreenCoordsXY& screenPos, int32_t dropdownButtonHeight)
        {
            auto* mainWindow = WindowGetMain();
            if (mainWindow == nullptr)
                return 400;

            return std::max(1, mainWindow->height - (screenPos.y + dropdownButtonHeight + 5));
        }

        void SetTextItems(
            const ScreenCoordsXY& screenPos, int32_t extraY, ColourWithFlags colour, uint8_t customItemHeight, uint8_t txtFlags,
            size_t numItems, int32_t itemWidth, int32_t numRowsPerColumn)
        {
            // Set and calculate num items, rows and columns
            ItemHeight = (txtFlags & Dropdown::Flag::CustomHeight) ? customItemHeight : GetDefaultRowHeight();

            gDropdownNumItems = static_cast<int32_t>(numItems);
            if (gDropdownNumItems > 1)
            {
                int32_t numAvailableRows = std::max(1, getSpaceUntilBottom(screenPos, extraY) / ItemHeight);
                NumRows = std::min({ gDropdownNumItems, numAvailableRows, numRowsPerColumn });
                NumColumns = (gDropdownNumItems + NumRows - 1) / NumRows;
            }
            else
            {
                // There must always be at least one column to prevent dividing by zero
                NumRows = 1;
                NumColumns = 1;
            }

            ItemWidth = itemWidth;

            // Text dropdowns are listed horizontally
            ListVertically = true;

            UpdateSizeAndPosition(screenPos, extraY);

            if (colour.hasFlag(ColourFlag::translucent))
                flags |= WF_TRANSPARENT;
            colours[0] = colour;
        }

        void SetImageItems(
            const ScreenCoordsXY& screenPos, int32_t extraY, ColourWithFlags colour, int32_t numItems, int32_t itemWidth,
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

            if (colour.hasFlag(ColourFlag::translucent))
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

            int32_t screenWidth = ContextGetWidth();
            int32_t screenHeight = ContextGetHeight();
            auto boundedScreenPos = screenPos;
            if (screenPos.x + ddWidth > screenWidth)
                boundedScreenPos.x = std::max(0, screenWidth - ddWidth);
            if (screenPos.y + ddHeight > screenHeight)
                boundedScreenPos.y = std::max(0, screenHeight - ddHeight);
            widgets[WIDX_BACKGROUND].right = ddWidth;
            widgets[WIDX_BACKGROUND].bottom = ddHeight;

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
    void WindowDropdownShowText(
        const ScreenCoordsXY& screenPos, int32_t extray, ColourWithFlags colour, uint8_t flags, size_t num_items,
        size_t prefRowsPerColumn)
    {
        char buffer[256];

        // Calculate the longest string width
        int32_t max_string_width = 0;
        for (size_t i = 0; i < num_items; i++)
        {
            FormatStringLegacy(buffer, 256, gDropdownItems[i].Format, static_cast<void*>(&gDropdownItems[i].Args));
            int32_t string_width = GfxGetStringWidth(buffer, FontStyle::Medium);
            max_string_width = std::max(string_width, max_string_width);
        }

        WindowDropdownShowTextCustomWidth(
            screenPos, extray, colour, 0, flags, num_items, max_string_width + 3, prefRowsPerColumn);
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
     * @param customItemHeight (ah) requires flag set as well
     */
    void WindowDropdownShowTextCustomWidth(
        const ScreenCoordsXY& screenPos, int32_t extray, ColourWithFlags colour, uint8_t customItemHeight, uint8_t flags,
        size_t num_items, int32_t width, size_t prefRowsPerColumn)
    {
        InputSetFlag(static_cast<INPUT_FLAGS>(INPUT_FLAG_DROPDOWN_STAY_OPEN | INPUT_FLAG_DROPDOWN_MOUSE_UP), false);
        if (flags & Dropdown::Flag::StayOpen || Config::Get().interface.TouchEnhancements)
            InputSetFlag(INPUT_FLAG_DROPDOWN_STAY_OPEN, true);

        WindowDropdownClose();

        // Create the window (width/height position are set later)
        auto* windowMgr = GetWindowManager();
        auto* w = windowMgr->Create<DropdownWindow>(WindowClass::Dropdown, width, customItemHeight, WF_STICK_TO_FRONT);
        if (w != nullptr)
        {
            auto numRowsPerColumn = prefRowsPerColumn > 0 ? static_cast<int32_t>(prefRowsPerColumn) : Dropdown::kItemsMaxSize;
            w->SetTextItems(screenPos, extray, colour, customItemHeight, flags, num_items, width, numRowsPerColumn);
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
        int32_t x, int32_t y, int32_t extray, ColourWithFlags colour, uint8_t flags, int32_t numItems, int32_t itemWidth,
        int32_t itemHeight, int32_t numColumns)
    {
        InputSetFlag(static_cast<INPUT_FLAGS>(INPUT_FLAG_DROPDOWN_STAY_OPEN | INPUT_FLAG_DROPDOWN_MOUSE_UP), false);
        if (flags & Dropdown::Flag::StayOpen || Config::Get().interface.TouchEnhancements)
            InputSetFlag(INPUT_FLAG_DROPDOWN_STAY_OPEN, true);

        // Close existing dropdown
        WindowDropdownClose();

        // Create the window (width/height position are set later)
        auto* windowMgr = GetWindowManager();
        auto* w = windowMgr->Create<DropdownWindow>(WindowClass::Dropdown, itemWidth, itemHeight, WF_STICK_TO_FRONT);
        if (w != nullptr)
        {
            w->SetImageItems({ x, y }, extray, colour, numItems, itemWidth, itemHeight, numColumns);
        }
    }

    void WindowDropdownClose()
    {
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::Dropdown);
    }

    /**
     * New function based on 6e914e
     * returns -1 if index is invalid
     */
    int32_t DropdownIndexFromPoint(const ScreenCoordsXY& loc, WindowBase* w)
    {
        if (w->classification == WindowClass::Dropdown)
        {
            auto* ddWnd = static_cast<DropdownWindow*>(w);
            return ddWnd->GetIndexFromPoint(loc);
        }
        return -1;
    }

    // colour_t ordered for use in color dropdown
    static constexpr colour_t kColoursDropdownOrder[] = {
        COLOUR_BLACK,
        COLOUR_SATURATED_RED,
        COLOUR_DARK_ORANGE,
        COLOUR_DARK_YELLOW,
        COLOUR_GRASS_GREEN_DARK,
        COLOUR_SATURATED_GREEN,
        COLOUR_AQUA_DARK,
        COLOUR_DARK_BLUE,
        COLOUR_SATURATED_PURPLE_DARK,

        COLOUR_GREY,
        COLOUR_BRIGHT_RED,
        COLOUR_LIGHT_ORANGE,
        COLOUR_YELLOW,
        COLOUR_MOSS_GREEN,
        COLOUR_BRIGHT_GREEN,
        COLOUR_TEAL,
        COLOUR_LIGHT_BLUE,
        COLOUR_BRIGHT_PURPLE,

        COLOUR_WHITE,
        COLOUR_LIGHT_PINK,
        COLOUR_ORANGE_LIGHT,
        COLOUR_BRIGHT_YELLOW,
        COLOUR_GRASS_GREEN_LIGHT,
        COLOUR_SATURATED_GREEN_LIGHT,
        COLOUR_AQUAMARINE,
        COLOUR_ICY_BLUE,
        COLOUR_SATURATED_PURPLE_LIGHT,

        COLOUR_DULL_BROWN_DARK,
        COLOUR_BORDEAUX_RED_DARK,
        COLOUR_TAN_DARK,
        COLOUR_SATURATED_BROWN,
        COLOUR_DARK_OLIVE_DARK,
        COLOUR_OLIVE_DARK,
        COLOUR_DULL_GREEN_DARK,
        COLOUR_DARK_PURPLE,
        COLOUR_DARK_PINK,

        COLOUR_DARK_BROWN,
        COLOUR_BORDEAUX_RED,
        COLOUR_SALMON_PINK,
        COLOUR_LIGHT_BROWN,
        COLOUR_DARK_OLIVE_GREEN,
        COLOUR_OLIVE_GREEN,
        COLOUR_DARK_GREEN,
        COLOUR_LIGHT_PURPLE,
        COLOUR_BRIGHT_PINK,

        COLOUR_DULL_BROWN_LIGHT,
        COLOUR_BORDEAUX_RED_LIGHT,
        COLOUR_TAN_LIGHT,
        COLOUR_SATURATED_BROWN_LIGHT,
        COLOUR_DARK_OLIVE_LIGHT,
        COLOUR_OLIVE_LIGHT,
        COLOUR_DULL_GREEN_LIGHT,
        COLOUR_DULL_PURPLE_LIGHT,
        COLOUR_MAGENTA_LIGHT,

        COLOUR_INVISIBLE,
        COLOUR_VOID,
    };

    colour_t ColourDropDownIndexToColour(uint8_t ddidx)
    {
        return kColoursDropdownOrder[ddidx];
    }

    /**
     *  rct2: 0x006ED43D
     */
    void WindowDropdownShowColour(
        WindowBase* w, Widget* widget, ColourWithFlags dropdownColour, colour_t selectedColour, bool alwaysHideSpecialColours)
    {
        int32_t defaultIndex = -1;

        const bool specialColoursEnabled = !alwaysHideSpecialColours && GetGameState().Cheats.allowSpecialColourSchemes;
        auto numColours = specialColoursEnabled ? static_cast<uint8_t>(COLOUR_COUNT) : kColourNumNormal;
        // Set items
        for (uint64_t i = 0; i < numColours; i++)
        {
            auto orderedColour = ColourDropDownIndexToColour(i);
            if (selectedColour == orderedColour)
                defaultIndex = i;

            // Use special graphic for Invisible colour
            auto imageId = (orderedColour == COLOUR_INVISIBLE) ? ImageId(SPR_G2_ICON_PALETTE_INVISIBLE, COLOUR_WHITE)
                                                               : ImageId(SPR_PALETTE_BTN, orderedColour);

            gDropdownItems[i].Format = Dropdown::kFormatColourPicker;
            Dropdown::SetImage(i, imageId);
        }

        // Show dropdown
        auto squareSize = DropdownWindow::GetDefaultRowHeight();
        WindowDropdownShowImage(
            w->windowPos.x + widget->left, w->windowPos.y + widget->top, widget->height() + 1, dropdownColour,
            Dropdown::Flag::StayOpen, numColours, squareSize, squareSize,
            DropdownGetAppropriateImageDropdownItemsPerRow(static_cast<uint32_t>(numColours)));

        gDropdownIsColour = true;
        gDropdownLastColourHover = -1;
        gDropdownDefaultIndex = defaultIndex;
    }

    uint32_t DropdownGetAppropriateImageDropdownItemsPerRow(uint32_t numItems)
    {
        // If above the table size return the last element
        return kAppropriateImageDropdownItemsPerRow[std::min<uint32_t>(
            numItems, static_cast<uint32_t>(std::size(kAppropriateImageDropdownItemsPerRow) - 1))];
    }
} // namespace OpenRCT2::Ui::Windows

using namespace OpenRCT2::Ui::Windows;
using namespace OpenRCT2;

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
