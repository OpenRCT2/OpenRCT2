/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2-ui/UiStringIds.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/interface/Window.h>

struct ImageId;

namespace OpenRCT2::Dropdown
{
    struct Item;

    constexpr StringId kSeparatorString = 0;
    constexpr StringId kFormatColourPicker = 0xFFFE;
    constexpr StringId kFormatLandPicker = 0xFFFF;
    constexpr int32_t kItemsMaxSize = 512;
} // namespace OpenRCT2::Dropdown

namespace OpenRCT2::Ui::Windows
{
    extern int32_t gDropdownNumItems;
    extern Dropdown::Item gDropdownItems[Dropdown::kItemsMaxSize];
    extern bool gDropdownIsColour;
    extern int32_t gDropdownLastColourHover;
    extern int32_t gDropdownHighlightedIndex;
    extern int32_t gDropdownDefaultIndex;

    void WindowDropdownShowText(
        const ScreenCoordsXY& screenPos, int32_t extray, ColourWithFlags colour, uint8_t flags, size_t num_items,
        size_t prefRowsPerColumn = 0);
    void WindowDropdownShowTextCustomWidth(
        const ScreenCoordsXY& screenPos, int32_t extray, ColourWithFlags colour, uint8_t custom_height, uint8_t flags,
        size_t num_items, int32_t width, size_t prefRowsPerColumn = 0);

    void WindowDropdownShowImage(
        int32_t x, int32_t y, int32_t extray, ColourWithFlags colour, uint8_t flags, int32_t numItems, int32_t itemWidth,
        int32_t itemHeight, int32_t numColumns);

    void WindowDropdownClose();

    int32_t DropdownIndexFromPoint(const ScreenCoordsXY& loc, WindowBase* w);

    void WindowDropdownShowColour(
        WindowBase* w, Widget* widget, ColourWithFlags dropdownColour, colour_t selectedColour,
        bool alwaysHideSpecialColours = false);
    void WindowDropdownShowColourAvailable(
        WindowBase* w, Widget* widget, uint8_t dropdownColour, uint8_t selectedColour, uint32_t availableColours);

    colour_t ColourDropDownIndexToColour(uint8_t ddidx);

    uint32_t DropdownGetAppropriateImageDropdownItemsPerRow(uint32_t numItems);
} // namespace OpenRCT2::Ui::Windows

namespace OpenRCT2::Dropdown
{
    bool IsChecked(int32_t index);
    bool IsDisabled(int32_t index);
    void SetChecked(int32_t index, bool value);
    void SetDisabled(int32_t index, bool value);
    void SetImage(int32_t index, ImageId image);

    enum Flag
    {
        CustomHeight = (1 << 6), // never set?
        StayOpen = (1 << 7)
    };

    enum class ItemFlag : uint8_t
    {
        IsDisabled = (1 << 0),
        IsChecked = (1 << 1),
    };

    struct Item
    {
        StringId Format;
        int64_t Args;
        uint8_t Flags;

        constexpr bool IsSeparator() const
        {
            return Format == kSeparatorString;
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
        constexpr ItemExt(int32_t _expectedItemIndex, uint32_t _itemFormat, StringId _stringId)
            : expectedItemIndex(_expectedItemIndex)
            , itemFormat(_itemFormat)
            , stringId(_stringId)
        {
        }

        int32_t expectedItemIndex;
        uint32_t itemFormat;
        StringId stringId;
    };

    constexpr ItemExt ToggleOption(int32_t _expectedItemIndex, StringId _stringId)
    {
        return ItemExt(_expectedItemIndex, STR_TOGGLE_OPTION, _stringId);
    }

    constexpr ItemExt Separator()
    {
        return ItemExt(-1, Dropdown::kSeparatorString, kStringIdEmpty);
    }

    template<int N>
    void SetItems(const Dropdown::ItemExt (&items)[N])
    {
        for (int i = 0; i < N; ++i)
        {
            const ItemExt& item = items[i];
            OpenRCT2::Ui::Windows::gDropdownItems[i].Format = item.itemFormat;
            OpenRCT2::Ui::Windows::gDropdownItems[i].Args = item.stringId;
        }
    }

    template<int N>
    constexpr bool ItemIDsMatchIndices(const Dropdown::ItemExt (&items)[N])
    {
        for (int i = 0; i < N; ++i)
        {
            const Dropdown::ItemExt& item = items[i];
            if (item.expectedItemIndex >= 0 && item.expectedItemIndex != i)
                return false;
        }

        return true;
    }
} // namespace OpenRCT2::Dropdown
