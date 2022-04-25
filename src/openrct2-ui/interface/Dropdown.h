/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2-ui/interface/Window.h>
#include <openrct2/common.h>
#include <openrct2/drawing/ImageId.hpp>
#include <openrct2/localisation/StringIds.h>

namespace Dropdown
{
    struct Item;

    constexpr const rct_string_id SeparatorString = 0;
    constexpr const rct_string_id FormatColourPicker = 0xFFFE;
    constexpr const rct_string_id FormatLandPicker = 0xFFFF;
    constexpr const int32_t ItemsMaxSize = 512;

    enum Flag
    {
        CustomHeight = (1 << 6),
        StayOpen = (1 << 7)
    };

    bool IsChecked(int32_t index);
    bool IsDisabled(int32_t index);
    void SetChecked(int32_t index, bool value);
    void SetDisabled(int32_t index, bool value);
    void SetImage(int32_t index, ImageId image);
} // namespace Dropdown

extern int32_t gDropdownNumItems;
extern Dropdown::Item gDropdownItems[Dropdown::ItemsMaxSize];
extern bool gDropdownIsColour;
extern int32_t gDropdownLastColourHover;
extern int32_t gDropdownHighlightedIndex;
extern int32_t gDropdownDefaultIndex;

void WindowDropdownShowText(const ScreenCoordsXY& screenPos, int32_t extray, uint8_t colour, uint8_t flags, size_t num_items);
void WindowDropdownShowTextCustomWidth(
    const ScreenCoordsXY& screenPos, int32_t extray, uint8_t colour, uint8_t custom_height, uint8_t flags, size_t num_items,
    int32_t width);
void WindowDropdownShowImage(
    int32_t x, int32_t y, int32_t extray, uint8_t colour, uint8_t flags, int32_t numItems, int32_t itemWidth,
    int32_t itemHeight, int32_t numColumns);
void WindowDropdownClose();
int32_t DropdownIndexFromPoint(const ScreenCoordsXY& loc, rct_window* w);
void WindowDropdownShowColour(rct_window* w, rct_widget* widget, uint8_t dropdownColour, uint8_t selectedColour);
void WindowDropdownShowColourAvailable(
    rct_window* w, rct_widget* widget, uint8_t dropdownColour, uint8_t selectedColour, uint32_t availableColours);
uint32_t DropdownGetAppropriateImageDropdownItemsPerRow(uint32_t numItems);

namespace Dropdown
{
    enum class ItemFlag : uint8_t
    {
        IsDisabled = (1 << 0),
        IsChecked = (1 << 1),
    };

    struct Item
    {
        rct_string_id Format;
        int64_t Args;
        uint8_t Flags;

        constexpr bool IsSeparator() const
        {
            return Format == SeparatorString;
        }

        constexpr bool IsDisabled() const
        {
            return (Flags & EnumValue(ItemFlag::IsDisabled));
        }

        constexpr bool IsChecked() const
        {
            return (Flags & EnumValue(ItemFlag::IsChecked));
        }
    };

    struct ItemExt
    {
        constexpr ItemExt(int32_t _expectedItemIndex, uint32_t _itemFormat, rct_string_id _stringId)
            : expectedItemIndex(_expectedItemIndex)
            , itemFormat(_itemFormat)
            , stringId(_stringId)
        {
        }

        int32_t expectedItemIndex;
        uint32_t itemFormat;
        rct_string_id stringId;
    };

    constexpr ItemExt ToggleOption(int32_t _expectedItemIndex, rct_string_id _stringId)
    {
        return ItemExt(_expectedItemIndex, STR_TOGGLE_OPTION, _stringId);
    }

    constexpr ItemExt Separator()
    {
        return ItemExt(-1, Dropdown::SeparatorString, STR_EMPTY);
    }

    template<int N> void SetItems(const Dropdown::ItemExt (&items)[N])
    {
        for (int i = 0; i < N; ++i)
        {
            const ItemExt& item = items[i];
            gDropdownItems[i].Format = item.itemFormat;
            gDropdownItems[i].Args = item.stringId;
        }
    }

    template<int N> constexpr bool ItemIDsMatchIndices(const Dropdown::ItemExt (&items)[N])
    {
        for (int i = 0; i < N; ++i)
        {
            const Dropdown::ItemExt& item = items[i];
            if (item.expectedItemIndex >= 0 && item.expectedItemIndex != i)
                return false;
        }

        return true;
    }

} // namespace Dropdown
