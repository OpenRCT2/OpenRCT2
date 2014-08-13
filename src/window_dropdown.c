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

#include <windows.h>
#include <memory.h>
#include "addresses.h"
#include "scenario.h"
#include "string_ids.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"

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

int _dropdown_num_items;
int _dropdown_num_columns;
int _dropdown_num_rows;
int _dropdown_item_width;
int _dropdown_item_height;
int _dropdown_highlighted_index;

uint16 gDropdownItemsFormat[64];
sint64 gDropdownItemsArgs[64];
// Replaces 0x009DED38
uint32 gDropdownItemsChecked;

static void window_dropdown_emptysub() { }
static void window_dropdown_paint();

static void* window_dropdown_events[] = {
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_emptysub,
	window_dropdown_paint,
	window_dropdown_emptysub
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

	RCT2_GLOBAL(0x009DE518, uint32) &= ~((1 << 2) | (1 << 1));
	if (flags & 0x80)
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 1);

	window_dropdown_close();
	_dropdown_num_columns = 1;
	_dropdown_item_width = width;
	_dropdown_item_height = 10;
	if (flags & 0x40)
		_dropdown_item_height = flags & 0x3F;
	
	// Set the widgets
	_dropdown_num_items = num_items;
	_dropdown_num_rows = num_items;
	window_dropdown_widgets[WIDX_BACKGROUND].bottom = _dropdown_item_height * num_items + 3;
	window_dropdown_widgets[WIDX_BACKGROUND].right = _dropdown_item_width + 3;

	// Create the window
	w = window_create(
		x, y + extray,
		window_dropdown_widgets[WIDX_BACKGROUND].right + 1,
		window_dropdown_widgets[WIDX_BACKGROUND].bottom + 1,
		(uint32*)window_dropdown_events,
		WC_DROPDOWN,
		0x02
	);
	w->widgets = window_dropdown_widgets;
	if (colour & 0x80)
		w->flags |= WF_TRANSPARENT;
	w->colours[0] = colour;

	// Input state
	_dropdown_highlighted_index = -1;
	RCT2_GLOBAL(0x009DED34, sint32) = 0;
	gDropdownItemsChecked = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, sint8) = INPUT_STATE_DROPDOWN_ACTIVE;

	// Copy the following properties until all use of it is decompiled
	RCT2_GLOBAL(0x009DEBA0, sint16) = _dropdown_num_items;
	RCT2_GLOBAL(0x009DED44, sint32) = _dropdown_num_columns;
	RCT2_GLOBAL(0x009DED48, sint32) = _dropdown_num_rows;
	RCT2_GLOBAL(0x009DED40, sint32) = _dropdown_item_width;
	RCT2_GLOBAL(0x009DED3C, sint32) = _dropdown_item_height;
	RCT2_GLOBAL(0x009DEBA2, sint16) = _dropdown_highlighted_index;
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

	RCT2_GLOBAL(0x009DE518, uint32) &= ~((1 << 2) | (1 << 1));
	if (flags & 0x80)
		RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 1);

	// Close existing dropdown
	window_dropdown_close();

	// Set and calculate num items, rows and columns
	_dropdown_item_width = itemWidth;
	_dropdown_item_height = itemHeight;
	_dropdown_num_items = numItems;
	_dropdown_num_columns = numColumns;
	_dropdown_num_rows = _dropdown_num_items / _dropdown_num_columns;
	if (_dropdown_num_items % _dropdown_num_columns != 0)
		_dropdown_num_rows++;

	// Calculate position and size
	width = _dropdown_item_width * _dropdown_num_columns + 3;
	height = _dropdown_item_height * _dropdown_num_rows + 3;
	if (x + width > RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16))
		x = max(0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - width);
	if (y + height > RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16))
		y = max(0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) - height);
	window_dropdown_widgets[WIDX_BACKGROUND].right = width;
	window_dropdown_widgets[WIDX_BACKGROUND].bottom = height;

	// Create the window
	w = window_create(
		x, y + extray,
		window_dropdown_widgets[WIDX_BACKGROUND].right + 1,
		window_dropdown_widgets[WIDX_BACKGROUND].bottom + 1,
		(uint32*)window_dropdown_events,
		WC_DROPDOWN,
		WF_STICK_TO_FRONT
	);
	w->widgets = window_dropdown_widgets;
	if (colour & 0x80)
		w->flags |= WF_TRANSPARENT;
	w->colours[0] = colour;

	// Input state
	_dropdown_highlighted_index = -1;
	RCT2_GLOBAL(0x009DED34, sint32) = 0;
	gDropdownItemsChecked = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, sint8) = INPUT_STATE_DROPDOWN_ACTIVE;

	// Copy the following properties until all use of it is decompiled
	RCT2_GLOBAL(0x009DEBA0, sint16) = _dropdown_num_items;
	RCT2_GLOBAL(0x009DED44, sint32) = _dropdown_num_columns;
	RCT2_GLOBAL(0x009DED48, sint32) = _dropdown_num_rows;
	RCT2_GLOBAL(0x009DED40, sint32) = _dropdown_item_width;
	RCT2_GLOBAL(0x009DED3C, sint32) = _dropdown_item_height;
	RCT2_GLOBAL(0x009DEBA2, sint16) = _dropdown_highlighted_index;
}

void window_dropdown_close()
{
	window_close_by_id(WC_DROPDOWN, 0);
}

static void window_dropdown_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	#ifdef _MSC_VER
	__asm mov w, esi
	#else
	__asm__ ( "mov %[w], esi " : [w] "+m" (w) );
	#endif

	#ifdef _MSC_VER
	__asm mov dpi, edi
	#else
	__asm__ ( "mov %[dpi], edi " : [dpi] "+m" (dpi) );
	#endif


	window_draw_widgets(w, dpi);

	_dropdown_highlighted_index = RCT2_GLOBAL(0x009DEBA2, sint16);
	{
		int i, cell_x, cell_y, l, t, r, b, item, image, colour;
		for (i = 0; i < _dropdown_num_items; i++) {
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
					gfx_fill_rect(dpi, l, t, r, b,
						*((char*)(0x00141FC47 + (w->colours[0] * 8))));
					gfx_fill_rect(dpi, l, t + 1, r, b + 1,
						*((char*)(0x00141FC4B + (w->colours[0] * 8))));
				}
			} else {
				// 
				if (i == _dropdown_highlighted_index) {
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
					if (item == (uint16)-2 && _dropdown_highlighted_index == i)
						image++;

					gfx_draw_sprite(
						dpi,
						image,
						w->x + 2 + (cell_x * _dropdown_item_width),
						w->y + 2 + (cell_y * _dropdown_item_height), 0
					);
				} else {
					// Text item
					if (i < 32)
						if (gDropdownItemsChecked & (1 << i))
							item++;

					// Calculate colour
					colour = w->colours[0] & 0x7F;
					if (i == _dropdown_highlighted_index)
						colour = 2;
					if (RCT2_GLOBAL(0x009DED34, uint32) & (1 << i))
						if (i < 32)
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
}
