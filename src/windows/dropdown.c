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

#include "../addresses.h"
#include "../input.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../scenario.h"
#include "../sprites.h"
#include "dropdown.h"

int gAppropriateImageDropdownItemsPerRow[] = {
	1, 1, 1, 1, 2, 2, 3, 3, 4,
	3, 5, 4, 4, 5, 5, 5, 4, 5,
	6, 5, 5, 7, 4, 5, 6, 5, 6,
	6, 6, 6, 6, 8, 8, 0
};

enum {
	WIDX_BACKGROUND,
};

static rct_widget window_dropdown_widgets[] = {
	{ WWT_IMGBTN, 0, 0, 0, 0, 0, -1, STR_NONE },
	{ WIDGETS_END },
};

int _dropdown_num_columns;
int _dropdown_num_rows;
int _dropdown_item_width;
int _dropdown_item_height;

int gDropdownNumItems;
uint16 gDropdownItemsFormat[64];
sint64 gDropdownItemsArgs[64];
uint64 gDropdownItemsChecked;
uint64 gDropdownItemsDisabled;
bool gDropdownIsColour;
int gDropdownLastColourHover;
int gDropdownHighlightedIndex;

bool dropdown_is_checked(int index)
{
	return gDropdownItemsChecked & (1ULL << index);
}

void dropdown_set_checked(int index, bool value)
{
	if (value) {
		gDropdownItemsChecked |= 1ULL << index;
	} else {
		gDropdownItemsChecked &= ~(1ULL << index);
	}
}

void dropdown_set_disabled(int index, bool value)
{
	if (value) {
		gDropdownItemsDisabled |= 1ULL << index;
	} else {
		gDropdownItemsDisabled &= ~(1ULL << index);
	}
}

static void window_dropdown_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_dropdown_events = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_dropdown_paint,
	NULL
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
void window_dropdown_show_text(int x, int y, int extray, uint8 colour, uint8 flags, int num_items)
{
	int i, string_width, max_string_width;
	char buffer[256];

	// Calculate the longest string width
	max_string_width = 0;
	for (i = 0; i < num_items; i++) {
		format_string(buffer, gDropdownItemsFormat[i], (void*)(&gDropdownItemsArgs[i]));
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = 224;
		string_width = gfx_get_string_width(buffer);
		max_string_width = max(string_width, max_string_width);
	}

	window_dropdown_show_text_custom_width(x, y, extray, colour, flags, num_items, max_string_width + 3);
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
 */
void window_dropdown_show_text_custom_width(int x, int y, int extray, uint8 colour, uint8 flags, int num_items, int width)
{
	rct_window* w;

	// Copy the formats and arguments until all use of it is decompiled
	memcpy((void*)0x009DEBA4, gDropdownItemsFormat, 40 * 2);
	memcpy((void*)0x009DEBF4, gDropdownItemsArgs, 40 * 8);

	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) &= ~(INPUT_FLAG_DROPDOWN_STAY_OPEN | INPUT_FLAG_DROPDOWN_MOUSE_UP);
	if (flags & DROPDOWN_FLAG_STAY_OPEN)
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_DROPDOWN_STAY_OPEN;

	window_dropdown_close();
	_dropdown_num_columns = 1;
	_dropdown_item_width = width;
	_dropdown_item_height = 10;
	if (flags & 0x40)
		_dropdown_item_height = flags & 0x3F;

	// Set the widgets
	gDropdownNumItems = num_items;
	_dropdown_num_rows = num_items;

	width = _dropdown_item_width * _dropdown_num_columns + 3;
	int height = _dropdown_item_height * _dropdown_num_rows + 3;
	if (x + width > RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16))
		x = max(0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) - width);
	if (y + height > RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16))
		y = max(0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - height);

	window_dropdown_widgets[WIDX_BACKGROUND].bottom = _dropdown_item_height * num_items + 3;
	window_dropdown_widgets[WIDX_BACKGROUND].right = _dropdown_item_width + 3;

	// Create the window
	w = window_create(
		x, y + extray,
		window_dropdown_widgets[WIDX_BACKGROUND].right + 1,
		window_dropdown_widgets[WIDX_BACKGROUND].bottom + 1,
		&window_dropdown_events,
		WC_DROPDOWN,
		0x02
	);
	w->widgets = window_dropdown_widgets;
	if (colour & 0x80)
		w->flags |= WF_TRANSPARENT;
	w->colours[0] = colour;

	// Input state
	gDropdownHighlightedIndex = -1;
	gDropdownItemsDisabled = 0;
	gDropdownItemsChecked = 0;
	gDropdownIsColour = false;
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, sint8) = INPUT_STATE_DROPDOWN_ACTIVE;
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
void window_dropdown_show_image(int x, int y, int extray, uint8 colour, uint8 flags, int numItems, int itemWidth, int itemHeight, int numColumns)
{
	int width, height;
	rct_window* w;

	// Copy the formats and arguments until all use of it is decompiled
	memcpy((void*)0x009DEBA4, gDropdownItemsFormat, 40 * 2);
	memcpy((void*)0x009DEBF4, gDropdownItemsArgs, 40 * 8);

	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) &= ~(INPUT_FLAG_DROPDOWN_STAY_OPEN | INPUT_FLAG_DROPDOWN_MOUSE_UP);
	if (flags & DROPDOWN_FLAG_STAY_OPEN)
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_DROPDOWN_STAY_OPEN;

	// Close existing dropdown
	window_dropdown_close();

	// Set and calculate num items, rows and columns
	_dropdown_item_width = itemWidth;
	_dropdown_item_height = itemHeight;
	gDropdownNumItems = numItems;
	_dropdown_num_columns = numColumns;
	_dropdown_num_rows = gDropdownNumItems / _dropdown_num_columns;
	if (gDropdownNumItems % _dropdown_num_columns != 0)
		_dropdown_num_rows++;

	// Calculate position and size
	width = _dropdown_item_width * _dropdown_num_columns + 3;
	height = _dropdown_item_height * _dropdown_num_rows + 3;
	if (x + width > RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16))
		x = max(0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) - width);
	if (y + height > RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16))
		y = max(0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - height);
	window_dropdown_widgets[WIDX_BACKGROUND].right = width;
	window_dropdown_widgets[WIDX_BACKGROUND].bottom = height;

	// Create the window
	w = window_create(
		x, y + extray,
		window_dropdown_widgets[WIDX_BACKGROUND].right + 1,
		window_dropdown_widgets[WIDX_BACKGROUND].bottom + 1,
		&window_dropdown_events,
		WC_DROPDOWN,
		WF_STICK_TO_FRONT
	);
	w->widgets = window_dropdown_widgets;
	if (colour & 0x80)
		w->flags |= WF_TRANSPARENT;
	w->colours[0] = colour;

	// Input state
	gDropdownHighlightedIndex = -1;
	gDropdownItemsDisabled = 0;
	gDropdownItemsChecked = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, sint8) = INPUT_STATE_DROPDOWN_ACTIVE;

	// Copy the following properties until all use of it is decompiled
	gDropdownHighlightedIndex = gDropdownHighlightedIndex;
	gDropdownIsColour = false;
}

void window_dropdown_close()
{
	window_close_by_class(WC_DROPDOWN);
}

static void window_dropdown_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	int cell_x, cell_y, l, t, r, b, item, image, colour;

	window_draw_widgets(w, dpi);

	gDropdownHighlightedIndex = gDropdownHighlightedIndex;
	for (int i = 0; i < gDropdownNumItems; i++) {
		cell_x = i % _dropdown_num_columns;
		cell_y = i / _dropdown_num_columns;

		if (gDropdownItemsFormat[i] == DROPDOWN_SEPARATOR) {
			l = w->x + 2 + (cell_x * _dropdown_item_width);
			t = w->y + 2 + (cell_y * _dropdown_item_height);
			r = l + _dropdown_item_width - 1;
			t += (_dropdown_item_height / 2);
			b = t;

			if (w->colours[0] & 0x80) {
				gfx_fill_rect(dpi, l, t, r, b, (RCT2_ADDRESS(0x009DEDF4, uint8)[w->colours[0]] | 0x02000000) + 1);
				gfx_fill_rect(dpi, l, t + 1, r, b + 1, (RCT2_ADDRESS(0x009DEDF4, uint8)[w->colours[0]] | 0x02000000) + 2);
			} else {
				gfx_fill_rect(dpi, l, t, r, b, ColourMapA[w->colours[0]].mid_dark);
				gfx_fill_rect(dpi, l, t + 1, r, b + 1, ColourMapA[w->colours[0]].lightest);
			}
		} else {
			//
			if (i == gDropdownHighlightedIndex) {
				l = w->x + 2 + (cell_x * _dropdown_item_width);
				t = w->y + 2 + (cell_y * _dropdown_item_height);
				r = l + _dropdown_item_width - 1;
				b = t + _dropdown_item_height - 1;
				gfx_fill_rect(dpi, l, t, r, b, 0x2000000 | 0x2F);
			}

			item = gDropdownItemsFormat[i];
			if (item == (uint16)-1 || item == (uint16)-2) {
				// Image item
				image = *((uint32*)&gDropdownItemsArgs[i]);
				if (item == (uint16)-2 && gDropdownHighlightedIndex == i)
					image++;

				gfx_draw_sprite(
					dpi,
					image,
					w->x + 2 + (cell_x * _dropdown_item_width),
					w->y + 2 + (cell_y * _dropdown_item_height), 0
				);
			} else {
				// Text item
				if (i < 64) {
					if (dropdown_is_checked(i)) {
						item++;
					}
				}

				// Calculate colour
				colour = w->colours[0] & 0x7F;
				if (i == gDropdownHighlightedIndex)
					colour = 2;
				if (gDropdownItemsDisabled & (1ULL << i))
					if (i < 64)
						colour = (w->colours[0] & 0x7F) | 0x40;

				// Draw item string
				gfx_draw_string_left_clipped(
					dpi,
					item,
					(void*)(&gDropdownItemsArgs[i]), colour,
					w->x + 2 + (cell_x * _dropdown_item_width),
					w->y + 1 + (cell_y * _dropdown_item_height),
					w->width - 5
				);
			}
		}
	}
}

/* New function based on 6e914e
 * returns -1 if index is invalid
 */
int dropdown_index_from_point(int x, int y, rct_window *w)
{
	int top = y - w->y - 2;
	if (top < 0) return -1;

	int left = x - w->x;
	if (left >= w->width) return -1;
	left -= 2;
	if (left < 0) return -1;

	int column_no = left / _dropdown_item_width;
	if (column_no >= _dropdown_num_columns) return -1;

	int row_no = top / _dropdown_item_height;
	if (row_no >= _dropdown_num_rows) return -1;

	int dropdown_index = row_no * _dropdown_num_columns + column_no;
	if (dropdown_index >= gDropdownNumItems) return -1;

	return dropdown_index;
}

void window_dropdown_show_colour(rct_window *w, rct_widget *widget, uint8 dropdownColour, uint8 selectedColour)
{
	window_dropdown_show_colour_available(w, widget, dropdownColour, selectedColour, 0xFFFFFFFF);
}

/**
 *  rct2: 0x006ED43D
 * al: dropdown colour
 * ah: selected colour
 * esi: window
 * edi: widget
 * ebp: unknown
 */
void window_dropdown_show_colour_available(rct_window *w, rct_widget *widget, uint8 dropdownColour, uint8 selectedColour,
	uint32 availableColours)
{
	int i, numItems;

	// Count number of available colours
	numItems = 0;
	for (i = 0; i < 32; i++)
		if (availableColours & (1 << i))
			numItems++;

	// Set items
	for (i = 0; i < 32; i++) {
		if (availableColours & (1 << i)) {
			if (selectedColour == i)
				gDropdownHighlightedIndex = i;

			gDropdownItemsFormat[i] = 0xFFFE;
			gDropdownItemsArgs[i] = ((uint64)i << 32) | (0x20000000 | (i << 19) | 5059);
		}
	}

	// Show dropdown
	window_dropdown_show_image(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		dropdownColour,
		DROPDOWN_FLAG_STAY_OPEN,
		numItems,
		12,
		12,
		gAppropriateImageDropdownItemsPerRow[numItems]
	);

	gDropdownIsColour = true;
	gDropdownLastColourHover = -1;
}
