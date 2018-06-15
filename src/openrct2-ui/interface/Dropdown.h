/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>
#include <openrct2-ui/interface/Window.h>

#define DROPDOWN_SEPARATOR            0
#define DROPDOWN_FORMAT_COLOUR_PICKER 0xFFFE
#define DROPDOWN_FORMAT_LAND_PICKER   0xFFFF
#define DROPDOWN_ITEMS_MAX_SIZE       128

enum
{
    DROPDOWN_FLAG_CUSTOM_HEIGHT = (1 << 6),
    DROPDOWN_FLAG_STAY_OPEN = (1 << 7)
};

extern sint32 gAppropriateImageDropdownItemsPerRow[];

extern sint32 gDropdownNumItems;
extern rct_string_id gDropdownItemsFormat[DROPDOWN_ITEMS_MAX_SIZE];
extern sint64 gDropdownItemsArgs[DROPDOWN_ITEMS_MAX_SIZE];
extern bool gDropdownIsColour;
extern sint32 gDropdownLastColourHover;
extern sint32 gDropdownHighlightedIndex;
extern sint32 gDropdownDefaultIndex;

bool dropdown_is_checked(sint32 index);
bool dropdown_is_disabled(sint32 index);
void dropdown_set_checked(sint32 index, bool value);
void dropdown_set_disabled(sint32 index, bool value);

void window_dropdown_show_text(sint32 x, sint32 y, sint32 extray, uint8 colour, uint8 flags, size_t num_items);
void window_dropdown_show_text_custom_width(sint32 x, sint32 y, sint32 extray, uint8 colour, uint8 custom_height, uint8 flags, size_t num_items, sint32 width);
void window_dropdown_show_image(sint32 x, sint32 y, sint32 extray, uint8 colour, uint8 flags, sint32 numItems, sint32 itemWidth, sint32 itemHeight, sint32 numColumns);
void window_dropdown_close();
sint32 dropdown_index_from_point(sint32 x, sint32 y, rct_window* w);
void window_dropdown_show_colour(rct_window *w, rct_widget *widget, uint8 dropdownColour, uint8 selectedColour);
void window_dropdown_show_colour_available(rct_window *w, rct_widget *widget, uint8 dropdownColour, uint8 selectedColour, uint32 availableColours);
