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

extern int32_t gAppropriateImageDropdownItemsPerRow[];

extern int32_t gDropdownNumItems;
extern rct_string_id gDropdownItemsFormat[DROPDOWN_ITEMS_MAX_SIZE];
extern int64_t gDropdownItemsArgs[DROPDOWN_ITEMS_MAX_SIZE];
extern bool gDropdownIsColour;
extern int32_t gDropdownLastColourHover;
extern int32_t gDropdownHighlightedIndex;
extern int32_t gDropdownDefaultIndex;

bool dropdown_is_checked(int32_t index);
bool dropdown_is_disabled(int32_t index);
void dropdown_set_checked(int32_t index, bool value);
void dropdown_set_disabled(int32_t index, bool value);

void window_dropdown_show_text(int32_t x, int32_t y, int32_t extray, uint8_t colour, uint8_t flags, size_t num_items);
void window_dropdown_show_text_custom_width(int32_t x, int32_t y, int32_t extray, uint8_t colour, uint8_t custom_height, uint8_t flags, size_t num_items, int32_t width);
void window_dropdown_show_image(int32_t x, int32_t y, int32_t extray, uint8_t colour, uint8_t flags, int32_t numItems, int32_t itemWidth, int32_t itemHeight, int32_t numColumns);
void window_dropdown_close();
int32_t dropdown_index_from_point(int32_t x, int32_t y, rct_window* w);
void window_dropdown_show_colour(rct_window *w, rct_widget *widget, uint8_t dropdownColour, uint8_t selectedColour);
void window_dropdown_show_colour_available(rct_window *w, rct_widget *widget, uint8_t dropdownColour, uint8_t selectedColour, uint32_t availableColours);
