#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _WINDOW_DROPDOWN_H_
#define _WINDOW_DROPDOWN_H_

#include "../common.h"
#include "../interface/window.h"

#define DROPDOWN_SEPARATOR 0

enum
{
	DROPDOWN_FLAG_STAY_OPEN = (1 << 7)
};

extern int gAppropriateImageDropdownItemsPerRow[];

extern int gDropdownNumItems;
extern rct_string_id gDropdownItemsFormat[64];
extern sint64 gDropdownItemsArgs[64];
extern uint64 gDropdownItemsChecked;
extern uint64 gDropdownItemsDisabled;
extern bool gDropdownIsColour;
extern int gDropdownLastColourHover;
extern int gDropdownHighlightedIndex;
extern int gDropdownDefaultIndex;

bool dropdown_is_checked(int index);
bool dropdown_is_disabled(int index);
void dropdown_set_checked(int index, bool value);
void dropdown_set_disabled(int index, bool value);

void window_dropdown_show_text(int x, int y, int extray, uint8 colour, uint8 flags, int num_items);
void window_dropdown_show_text_custom_width(int x, int y, int extray, uint8 colour, uint8 flags, int num_items, int width);
void window_dropdown_show_image(int x, int y, int extray, uint8 colour, uint8 flags, int numItems, int itemWidth, int itemHeight, int numColumns);
void window_dropdown_close();
int dropdown_index_from_point(int x, int y, rct_window* w);
void window_dropdown_show_colour(rct_window *w, rct_widget *widget, uint8 dropdownColour, uint8 selectedColour);
void window_dropdown_show_colour_available(rct_window *w, rct_widget *widget, uint8 dropdownColour, uint8 selectedColour, uint32 availableColours);

#endif
