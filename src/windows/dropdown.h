/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef _WINDOW_DROPDOWN_H_
#define _WINDOW_DROPDOWN_H_

#include "../common.h"

#define DROPDOWN_SEPARATOR 0

enum
{
	DROPDOWN_FLAG_STAY_OPEN = (1 << 7)
};

extern int gAppropriateImageDropdownItemsPerRow[];

extern int gDropdownNumItems;
extern uint16 gDropdownItemsFormat[64];
extern sint64 gDropdownItemsArgs[64];
extern uint32 gDropdownItemsChecked;

void window_dropdown_show_text(int x, int y, int extray, uint8 colour, uint8 flags, int num_items);
void window_dropdown_show_text_custom_width(int x, int y, int extray, uint8 colour, uint8 flags, int num_items, int width);
void window_dropdown_show_image(int x, int y, int extray, uint8 colour, uint8 flags, int numItems, int itemWidth, int itemHeight, int numColumns);
void window_dropdown_close();
int dropdown_index_from_point(int x, int y, rct_window* w);
void window_dropdown_show_colour(rct_window *w, rct_widget *widget, uint8 dropdownColour, uint8 selectedColour);
void window_dropdown_show_colour_available(rct_window *w, rct_widget *widget, uint8 dropdownColour, uint8 selectedColour, uint32 availableColours);

#endif