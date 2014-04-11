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

#include <stdlib.h>
#include "addresses.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"

static void widget_frame_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_button_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_tab_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_flat_button_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_caption_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_closebox_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_scroll_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_hscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int l, int t, int r, int b, int colour);
static void widget_vscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int l, int t, int r, int b, int colour);

/**
 * 
 *  rct2: 0x006EAF26
 */
void widget_scroll_update_thumbs(rct_window *w, int widget_index)
{
	rct_widget* widget;
	rct_scroll* scroll;
	int x, y, view_size;

	widget = &w->widgets[widget_index];
	scroll = &w->scrolls[window_get_scroll_data_index(w, widget_index)];

	if (scroll->flags & HSCROLLBAR_VISIBLE) {
		view_size = widget->right - widget->left - 21;
		if (scroll->flags & VSCROLLBAR_VISIBLE)
			view_size -= 11;
		x = scroll->h_left * view_size;
		if (scroll->h_right != 0)
			x /= scroll->h_right;
		scroll->h_thumb_left = x + 11;

		x = widget->right - widget->left - 2;
		if (scroll->flags & VSCROLLBAR_VISIBLE)
			x -= 11;
		x += scroll->h_left;
		if (scroll->h_right != 0)
			x = (x * view_size) / scroll->h_right;
		x += 11;
		view_size += 10;
		scroll->h_thumb_right = min(x, view_size);
	}

	if (scroll->flags & VSCROLLBAR_VISIBLE) {
		view_size = widget->bottom - widget->top - 21;
		if (scroll->flags & HSCROLLBAR_VISIBLE)
			view_size -= 11;
		y = scroll->v_top * view_size;
		if (scroll->v_bottom != 0)
			y /= scroll->v_bottom;
		scroll->v_thumb_top = y + 11;

		y = widget->bottom - widget->top - 2;
		if (scroll->flags & HSCROLLBAR_VISIBLE)
			y -= 11;
		y += scroll->v_top;
		if (scroll->v_bottom != 0)
			y = (y * view_size) / scroll->v_bottom;
		y += 11;
		view_size += 10;
		scroll->v_thumb_bottom = min(y, view_size);
	}
}

void widget_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	switch (w->widgets[widgetIndex].type) {
	case WWT_FRAME:
		widget_frame_draw(dpi, w, widgetIndex);
		break;
	case WWT_RESIZE:
		break;
	case WWT_IMGBTN:
	case WWT_4:
		widget_button_draw(dpi, w, widgetIndex);
		break;
	case WWT_5:
	case WWT_6:
	case WWT_TRNBTN:
	case WWT_TAB:
		widget_tab_draw(dpi, w, widgetIndex);
		break;
	case WWT_FLATBTN:
		widget_flat_button_draw(dpi, w, widgetIndex);
		break;
	case WWT_DROPDOWN_BUTTON:
	case WWT_11:
	case WWT_13:
		break;
	case WWT_12:
		break;
	case WWT_14:
		break;
	case WWT_15:
	case WWT_DROPDOWN:
	case WWT_VIEWPORT:
		break;
	case WWT_18:
		break;
	case WWT_19:
		break;
	case WWT_CAPTION:
		widget_caption_draw(dpi, w, widgetIndex);
		break;
	case WWT_CLOSEBOX:
		widget_closebox_draw(dpi, w, widgetIndex);
		break;
	case WWT_SCROLL:
		widget_scroll_draw(dpi, w, widgetIndex);
		break;
	case WWT_23:
	case WWT_24:
		break;
	case WWT_25:
		break;
	}
}

/**
 * 
 *  rct2: 0x006EB6CE
 */
static void widget_frame_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b, press;
	uint8 colour;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// 
	press = (w->flags & 0x0400 ? 0x80 : 0);

	// Get the colour
	colour = w->colours[widget->colour];

	// Draw the frame
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

	// Check if the window can be resized
	if (!(w->flags & 0x100))
		return;
	if (w->min_width == w->max_width && w->min_height == w->max_height)
		return;

	// Draw the resize sprite at the bottom right corner
	l = w->x + widget->right - 18;
	t = w->y + widget->bottom - 18;
	gfx_draw_sprite(dpi, SPR_RESIZE | 0x20000000 | (colour << 19), l, t);
}

/**
 * 
 *  rct2: 0x006EB8E5
 */
static void widget_button_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b, press;
	uint32 image;
	uint8 colour;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Check if the button is pressed down
	press = widget_is_pressed(w, widgetIndex) ? 0x20 : 0;

	// Get the colour
	colour = w->colours[widget->colour];

	// ?
	if (widget->image == -2) {
		gfx_fill_rect_inset(dpi, l, t, r, b, colour, press | 0x10);
		return;
	}

	// Draw the button
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

	if (widget->image == -1)
		return;

	// Draw the image
	image = widget->image;
	if (!widget_is_disabled(w, widgetIndex)) {
		if (image & 0x80000000) {
			// ?
		}

		if (image & 0x40000000)
			image &= ~0x40000000;
		else
			image |= colour << 19;

		gfx_draw_sprite(dpi, image, l, t);
	} else {
		// ?
	}
}

/**
 *
 *  rct2: 0x006EB806
 */
static void widget_tab_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b;
	uint32 image;
	uint8 colour;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// 
	if (widget->image == -1)
		return;

	// Check if tab is disabled
	if (widget_is_disabled(w, widgetIndex))
		return;

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Get the colour
	colour = w->colours[widget->colour];

	// Check if the tab is pressed down
	image = widget->image;
	if (widget_is_pressed(w, widgetIndex))
		image++;

	if (image & 0x40000000)
		image &= ~0x40000000;
	else
		image |= colour << 19;

	gfx_draw_sprite(dpi, image, l, t);
}

/**
 * 
 *  rct2: 0x006EB861
 */
static void widget_flat_button_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b, state;
	uint32 image;
	uint8 colour;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Check if the button is pressed down
	state = 0;
	if (widget_is_pressed(w, widgetIndex))
		state = 1;

	// Check if the button is highlighted
	if (state == 0 && widget_is_highlighted(w, widgetIndex))
		state = 2;

	// Get the colour
	colour = w->colours[widget->colour];

	if (state == 1)
		gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0x20);
	else if (state == 2)
		gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0x10);
}

/**
 * 
 *  rct2: 0x006EB2F9
 */
static void widget_caption_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b, width, press;
	uint8 colour;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Get the colour
	colour = w->colours[widget->colour];

	// 
	if (w->var_4B8 != -1) {
		gfx_draw_sprite(dpi, *((char*)(0x013CA742 + w->var_4B8)) << 19, l + 1, t + 1);
		if (w->width > 638)
			gfx_draw_sprite(dpi, *((char*)(0x013CA742 + w->var_4B8)) << 19, l + 1 + 638, t + 1);
		if (w->var_4B9 != -1) {
			gfx_draw_sprite(dpi, *((char*)(0x013CA742 + w->var_4B9)) << 19, l + 1, t + 1);
			if (w->width > 638)
				gfx_draw_sprite(dpi, *((char*)(0x013CA742 + w->var_4B9)) << 19, l + 1 + 638, t + 1);
		}

		// 
		press = 0x70;
		if (w->flags & 0x0400)
			press |= 0x80;

		gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);
		gfx_fill_rect(dpi, r + 1, t, r + 1, b, *((char*)(0x0141FC47 + (colour * 8))));
	} else {
		// 
		press = 0x60;
		if (w->flags & 0x0400)
			press |= 0x80;

		gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);
		gfx_fill_rect(dpi, l + 1, t + 1, r - 1, b - 1, 0x2000000 | 47);
	}

	// Draw text
	l = widget->left + w->x + 2;
	t = widget->top + w->y + 1;
	width = widget->right - widget->left - 4;
	if ((widget + 1)->type == WWT_CLOSEBOX) {
		width -= 10;
		if ((widget + 2)->type == WWT_CLOSEBOX)
			width -= 10;
	}
	l += width / 2;
	gfx_draw_string_centred_clipped(dpi, widget->image, (void*)0x013CE952, 34, l, t, width);
}

/**
 * 
 *  rct2: 0x006EBB85
 */
static void widget_closebox_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b, press;
	uint8 colour;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Check if the button is pressed down
	press = 0;
	if (w->flags & 0x400)
		press |= 0x80;
	if (widget_is_pressed(w, widgetIndex))
		press |= 0x20;

	// Get the colour
	colour = w->colours[widget->colour];

	// Draw the button
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

	if (widget->image == -1)
		return;

	l = w->x + (widget->left + widget->right) / 2 - 1;
	t = w->y + max(widget->top, (widget->top + widget->bottom) / 2 - 5);

	if (widget_is_disabled(w, widgetIndex))
		colour |= 0x40;

	gfx_draw_string_centred_clipped(dpi, widget->image, (void*)0x013CE952, colour, l, t, widget->right - widget->left - 2);
}

/**
 * 
 *  rct2: 0x006EBD96
 */
static void widget_scroll_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	rct_scroll* scroll;
	int scrollIndex;
	int l, t, r, b, press, ebp;
	int cl, ct, cr, cb;
	uint8 colour;
	rct_drawpixelinfo scroll_dpi;

	// Get the widget
	scrollIndex = window_get_scroll_data_index(w, widgetIndex);
	widget = &w->widgets[widgetIndex];
	scroll = &w->scrolls[scrollIndex];

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Get the colour
	colour = w->colours[widget->colour];

	// Draw the border
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0x60);

	// Inflate by -1
	l++;
	t++;
	r--;
	b--;

	// Horizontal scrollbar
	if (scroll->flags & HSCROLLBAR_VISIBLE)
		widget_hscrollbar_draw(dpi, scroll, l, b - 10, (scroll->flags & VSCROLLBAR_VISIBLE ? r - 10 : r), b, colour);

	// Vertical scrollbar
	if (scroll->flags & VSCROLLBAR_VISIBLE)
		widget_vscrollbar_draw(dpi, scroll, r - 10, t, r, (scroll->flags & HSCROLLBAR_VISIBLE ? b - 10 : b), colour);

	// Contents
	if (scroll->flags & HSCROLLBAR_VISIBLE)
		b -= 11;
	if (scroll->flags & VSCROLLBAR_VISIBLE)
		r -= 11;

	// Create a new inner scroll dpi
	scroll_dpi = *dpi;

	// Clip the scroll dpi against the outer dpi
	cl = max(dpi->x, l);
	ct = max(dpi->y, t);
	cr = min(dpi->x + dpi->width, r);
	cb = min(dpi->y + dpi->height, b);

	// Set the respective dpi attributes
	scroll_dpi.x = cl - l + scroll->h_left;
	scroll_dpi.y = ct - t + scroll->v_top;
	scroll_dpi.width = cr - cl;
	scroll_dpi.height = cb - ct;
	scroll_dpi.bits += cl - dpi->x;
	scroll_dpi.bits += (ct - dpi->y) * (dpi->width + dpi->pitch);
	scroll_dpi.pitch = (dpi->width + dpi->pitch) - scroll_dpi.width;

	// Draw the scroll contents
	if (scroll_dpi.width > 0 && scroll_dpi.height > 0)
		RCT2_CALLPROC_X(w->event_handlers[WE_SCROLL_PAINT], 0, 0, 0, 0, w, &scroll_dpi, 0);
}

static void widget_hscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int l, int t, int r, int b, int colour)
{
	// Trough
	gfx_fill_rect(dpi, l + 10, t, r - 10, b, *((char*)(0x0141FC4B + (colour * 8))));
	gfx_fill_rect(dpi, l + 10, t, r - 10, b, 0x1000000 | *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 10, t + 2, r - 10, t + 2, *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 10, t + 3, r - 10, t + 3, *((char*)(0x0141FC4B + (colour * 8))));
	gfx_fill_rect(dpi, l + 10, t + 7, r - 10, t + 7, *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 10, t + 8, r - 10, t + 8, *((char*)(0x0141FC4B + (colour * 8))));
	
	// Left button
	gfx_fill_rect_inset(dpi, l, t, l + 9, b, colour, (scroll->flags & HSCROLLBAR_LEFT_PRESSED ? 0x20 : 0));
	gfx_draw_string(dpi, (char*)0x009DED6C, 0, l + 1, t);
	
	// Thumb
	gfx_fill_rect_inset(dpi,
		max(l + 10, l + scroll->h_thumb_left - 1), t,
		min(r - 10, r + scroll->h_thumb_right - 1), t,
		colour, (scroll->flags & HSCROLLBAR_THUMB_PRESSED ? 0x20 : 0));

	// Right button
	gfx_fill_rect_inset(dpi, r - 9, t, r, b, colour, (scroll->flags & HSCROLLBAR_RIGHT_PRESSED ? 0x20 : 0));
	gfx_draw_string(dpi, (char*)0x009DED6F, 0, r - 6, t);
}

static void widget_vscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int l, int t, int r, int b, int colour)
{
	// Trough
	gfx_fill_rect(dpi, l, t + 10, r, b - 10, *((char*)(0x0141FC4B + (colour * 8))));
	gfx_fill_rect(dpi, l, t + 10, r, b - 10, 0x1000000 | *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 2, t + 10, l + 2, b - 10, *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 3, t + 10, l + 3, b - 10, *((char*)(0x0141FC4B + (colour * 8))));
	gfx_fill_rect(dpi, l + 7, t + 10, l + 7, b - 10, *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 8, t + 10, l + 8, b - 10, *((char*)(0x0141FC4B + (colour * 8))));

	// Up button
	gfx_fill_rect_inset(dpi, l, t, r, t + 9, colour, (scroll->flags & VSCROLLBAR_UP_PRESSED ? 0x20 : 0));
	gfx_draw_string(dpi, (char*)0x009DED66, 0, l + 1, t - 1);

	// Thumb
	gfx_fill_rect_inset(dpi,
		l, max(t + 10, t + scroll->v_thumb_top - 1),
		r, min(b - 10, t + scroll->v_thumb_bottom - 1),
		colour, (scroll->flags & VSCROLLBAR_THUMB_PRESSED ? 0x20 : 0));

	// Down button
	gfx_fill_rect_inset(dpi, l, b - 9, r, b, colour, (scroll->flags & VSCROLLBAR_DOWN_PRESSED ? 0x20 : 0));
	gfx_draw_string(dpi, (char*)0x009DED69, 0, l + 1, b - 8);
}

int widget_is_disabled(rct_window *w, int widgetIndex)
{
	return w->disabled_widgets & (1LL << widgetIndex);
}

int widget_is_pressed(rct_window *w, int widgetIndex)
{
	int inputState = RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8);

	if (w->pressed_widgets & (1LL << widgetIndex))
		return 1;
	if (inputState == INPUT_STATE_WIDGET_PRESSED || inputState == INPUT_STATE_DROPDOWN_ACTIVE) {
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass) != w->classification)
			return 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWNUMBER, rct_windownumber) != w->number)
			return 0;
		if (!(RCT2_GLOBAL(0x009DE518, uint32) & 1))
			return 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, sint32) == widgetIndex)
			return 1;
	}
	return 0;
}

int widget_is_highlighted(rct_window *w, int widgetIndex)
{
	return 0;
}