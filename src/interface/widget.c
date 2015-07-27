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
#include "../drawing/drawing.h"
#include "../input.h"
#include "../sprites.h"
#include "widget.h"
#include "window.h"
#include "../platform/platform.h"
#include "../localisation/localisation.h"

static void widget_frame_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_resize_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_button_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_tab_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_flat_button_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_text_button(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_text_unknown(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_text(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_text_inset(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_text_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_text_box_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_groupbox_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_caption_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_checkbox_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_closebox_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_scroll_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);
static void widget_hscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int l, int t, int r, int b, int colour);
static void widget_vscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int l, int t, int r, int b, int colour);
static void widget_draw_image(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex);

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

/**
 * 
 *  rct2: 0x006EB2A8
 */
void widget_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	switch (w->widgets[widgetIndex].type) {
	case WWT_FRAME:
		widget_frame_draw(dpi, w, widgetIndex);
		break;
	case WWT_RESIZE:
		widget_resize_draw(dpi, w, widgetIndex);
		break;
	case WWT_IMGBTN:
	case WWT_4:
		widget_button_draw(dpi, w, widgetIndex);
		break;
	case WWT_5:
	case WWT_COLORBTN:
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
		widget_text_button(dpi, w, widgetIndex);
		break;
	case WWT_12:
		widget_text_unknown(dpi, w, widgetIndex);
		break;
	case WWT_14:
		widget_text(dpi, w, widgetIndex);
		break;
	case WWT_SPINNER:
	case WWT_DROPDOWN:
	case WWT_VIEWPORT:
		widget_text_inset(dpi, w, widgetIndex);
		break;
	case WWT_18:
		break;
	case WWT_GROUPBOX:
		widget_groupbox_draw(dpi, w, widgetIndex);
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
	case WWT_CHECKBOX:
	case WWT_24:
		widget_checkbox_draw(dpi, w, widgetIndex);
		break;
	case WWT_25:
		break;
	case WWT_TEXT_BOX:
		widget_text_box_draw(dpi, w, widgetIndex);
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
	press = (w->flags & WF_10 ? 0x80 : 0);

	// Get the colour
	colour = w->colours[widget->colour];

	// Draw the frame
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

	// Check if the window can be resized
	if (!(w->flags & WF_RESIZABLE))
		return;
	if (w->min_width == w->max_width && w->min_height == w->max_height)
		return;

	// Draw the resize sprite at the bottom right corner
	l = w->x + widget->right - 18;
	t = w->y + widget->bottom - 18;
	gfx_draw_sprite(dpi, SPR_RESIZE | 0x20000000 | ((colour & 0x7F) << 19), l, t, 0);
}

/**
 * 
 *  rct2: 0x006EB765
 */
static void widget_resize_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b;
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

	// Draw the panel
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0);

	// Check if the window can be resized
	if (!(w->flags & WF_RESIZABLE))
		return;
	if (w->min_width == w->max_width && w->min_height == w->max_height)
		return;

	// Draw the resize sprite at the bottom right corner
	l = w->x + widget->right - 18;
	t = w->y + widget->bottom - 18;
	gfx_draw_sprite(dpi, SPR_RESIZE | 0x20000000 | ((colour & 0x7F) << 19), l, t, 0);
}

/**
 * 
 *  rct2: 0x006EB8E5
 */
static void widget_button_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
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
	press = widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex) ? 0x20 : 0;

	// Get the colour
	colour = w->colours[widget->colour];

	if (widget->image == -2) {
		// Draw border with no fill
		gfx_fill_rect_inset(dpi, l, t, r, b, colour, press | 0x10);
		return;
	}

	// Draw the border with fill
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

	widget_draw_image(dpi, w, widgetIndex);
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
	if (!widget_is_disabled(w, widgetIndex)) {
		widget_draw_image(dpi, w, widgetIndex);
		return;
	}

	if (widget->type == WWT_TAB)
		return;

	if (widget->type != WWT_TRNBTN) {
		widget_draw_image(dpi, w, widgetIndex);
		return;
	}

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Get the colour and image
	colour = w->colours[widget->colour] & 0x7F;
	image = widget->image + 2;

	// Draw coloured image
	gfx_draw_sprite(dpi, image | (colour << 19), l, t, 0);
}

/**
 * 
 *  rct2: 0x006EB861
 */
static void widget_flat_button_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b;
	uint8 colour;

	if (!widget_is_disabled(w, widgetIndex) && widget_is_highlighted(w, widgetIndex)) {
		widget_button_draw(dpi, w, widgetIndex);
		return;
	}

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Get the colour
	colour = w->colours[widget->colour];

	// Check if the button is pressed down
	if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex)) {
		if (widget->image == -2) {
			// Draw border with no fill
			gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0x20 | 0x10);
			return;
		}

		// Draw the border with fill
		gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0x20);
	}

	// Draw image
	widget_draw_image(dpi, w, widgetIndex);
}

/**
 * 
 *  rct2: 0x006EBBEB
 */
static void widget_text_button(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
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

	// Get the colour
	colour = w->colours[widget->colour];

	// Border
	press = widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex) ? 0x20 : 0;
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

	// Text
	widget_text_unknown(dpi, w, widgetIndex);
}

/**
 * 
 *  rct2: 0x006EBC41
 */
static void widget_text_unknown(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b, stringId;
	uint8 colour;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Get the colour
	colour = w->colours[widget->colour];
	// do not use widget color as this is already used as background for the text_button
	// colour = 2;

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	stringId = widget->image;
	if (stringId == -1)
		return;

	if (widget->type == WWT_11 && (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex)))
		stringId++;

	if (widget->type == WWT_13) {
		if (widget_is_disabled(w, widgetIndex))
			colour |= 0x40;
		gfx_draw_string_left_clipped(
			dpi,
			stringId,
			(void*)0x013CE952,
			colour,
			l + 1,
			t,
			widget->right - widget->left - 2
		);
	} else {
		colour &= ~(1 << 7);
		if (widget_is_disabled(w, widgetIndex))
			colour |= 0x40;
		gfx_draw_string_centred_clipped(
			dpi,
			stringId,
			(void*)0x013CE952,
			colour,
			(w->x + w->x + widget->left + widget->right + 1) / 2 - 1,
			t,
			widget->right - widget->left - 2
		);
	}
}

/**
 * 
 *  rct2: 0x006EBD52
 */
static void widget_text(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b;
	uint8 colour;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Get the colour
	colour = w->colours[widget->colour];

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	if (widget->image == (uint32)-2 || widget->image == (uint32)-1)
		return;

	if (widget_is_disabled(w, widgetIndex))
		colour |= 0x40;
	gfx_draw_string_left(dpi, widget->image, (void*)0x013CE952, colour, l + 1, t);
}

/**
 * 
 *  rct2: 0x006EBD1F
 */
static void widget_text_inset(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b;
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

	gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0x60);
	widget_text(dpi, w, widgetIndex);
}

/**
 * 
 *  rct2: 0x006EC1A6
 */
static void widget_text_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b, press;
	uint8 colour;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	l = w->x + widget->left + 5;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Get the colour
	colour = w->colours[widget->colour];

	press = 0;
	if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex))
		press |= 0x20;

	gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

	// TODO
	
	gfx_fill_rect(dpi, l, t, r, b, colour);
}

/**
 * 
 *  rct2: 0x006EB535
 */
static void widget_groupbox_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b, textRight;
	uint8 colour;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	l = w->x + widget->left + 5;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;
	textRight = l;

	// Text
	if (widget->image != (uint32)-1) {
		colour = w->colours[widget->colour] & 0x7F;
		if (widget_is_disabled(w, widgetIndex))
			colour |= 0x40;
		gfx_draw_string_left(dpi, widget->image, (void*)0x013CE952, colour, l, t);
		textRight = l + gfx_get_string_width((char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER) + 1;
	}

	// Border
	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top + 4;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Get the colour
	colour = w->colours[widget->colour] & 0x7F;

	// Border left of text
	gfx_fill_rect(dpi, l, t, l + 4, t, RCT2_ADDRESS(0x0141FC47, uint8)[colour * 8]);
	gfx_fill_rect(dpi, l + 1, t + 1, l + 4, t + 1, RCT2_ADDRESS(0x0141FC4B, uint8)[colour * 8]);

	// Border right of text
	gfx_fill_rect(dpi, textRight, t, r - 1, t, RCT2_ADDRESS(0x0141FC47, uint8)[colour * 8]);
	gfx_fill_rect(dpi, textRight, t + 1, r - 2, t + 1, RCT2_ADDRESS(0x0141FC4B, uint8)[colour * 8]);

	// Border right
	gfx_fill_rect(dpi, r - 1, t + 1, r - 1, b - 1, RCT2_ADDRESS(0x0141FC47, uint8)[colour * 8]);
	gfx_fill_rect(dpi, r, t, r, b, RCT2_ADDRESS(0x0141FC4B, uint8)[colour * 8]);

	// Border bottom
	gfx_fill_rect(dpi, l, b - 1, r - 2, b - 1, RCT2_ADDRESS(0x0141FC47, uint8)[colour * 8]);
	gfx_fill_rect(dpi, l, b, r - 1, b, RCT2_ADDRESS(0x0141FC4B, uint8)[colour * 8]);

	// Border left
	gfx_fill_rect(dpi, l, t + 1, l, b - 2, RCT2_ADDRESS(0x0141FC47, uint8)[colour * 8]);
	gfx_fill_rect(dpi, l + 1, t + 2, l + 1, b - 2, RCT2_ADDRESS(0x0141FC4B, uint8)[colour * 8]);
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
		gfx_draw_sprite(dpi, *((char*)(0x013CA742 + w->var_4B8)) << 19, l + 1, t + 1, 0);
		if (w->width > 638)
			gfx_draw_sprite(dpi, *((char*)(0x013CA742 + w->var_4B8)) << 19, l + 1 + 638, t + 1, 0);
		if (w->var_4B9 != -1) {
			gfx_draw_sprite(dpi, *((char*)(0x013CA742 + w->var_4B9)) << 19, l + 1, t + 1, 0);
			if (w->width > 638)
				gfx_draw_sprite(dpi, *((char*)(0x013CA742 + w->var_4B9)) << 19, l + 1 + 638, t + 1, 0);
		}

		// 
		press = 0x70;
		if (w->flags & WF_10)
			press |= 0x80;

		gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);
		gfx_fill_rect(dpi, r + 1, t, r + 1, b, *((char*)(0x0141FC47 + (colour * 8))));
	} else {
		// 
		press = 0x60;
		if (w->flags & WF_10)
			press |= 0x80;

		gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

		// Black caption bars look slightly green, this fixes that
		if (colour == 0)
			gfx_fill_rect(dpi, l + 1, t + 1, r - 1, b - 1, *((char*)(0x0141FC46 + (colour * 8))));
		else
			gfx_fill_rect(dpi, l + 1, t + 1, r - 1, b - 1, 0x2000000 | 47);
	}

	// Draw text
	if (widget->image == (uint32)-1)
		return;

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
	if (w->flags & WF_10)
		press |= 0x80;
	if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex))
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
*  rct2: 0x006EBAD9
*/
static void widget_checkbox_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, b, yMid;
	uint8 colour;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltb
	l = w->x + widget->left;
	t = w->y + widget->top;
	b = w->y + widget->bottom;
	yMid = (b + t) / 2;

	// Get the colour
	colour = w->colours[widget->colour];

	if (widget->type != WWT_24) {
		// checkbox
		gfx_fill_rect_inset(dpi, l, yMid - 5, l + 9, yMid + 4, colour, 0x60);

		// fill it when checkbox is pressed
		if (widget_is_pressed(w, widgetIndex)) {
			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
			gfx_draw_string(dpi, (char*)CheckBoxMarkString, colour & 0x7F, l, yMid - 5);
		}
	}

	// draw the text
	if (widget->image == -1)
		return;

	if (widget_is_disabled(w, widgetIndex)) {
		colour |= 0x40;
	}

	gfx_draw_string_left_centred(dpi, (rct_string_id)widget->image, (void*)0x013CE952, colour, l + 14, yMid);
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
	int l, t, r, b;
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

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16) = 0xE0;

	// Horizontal scrollbar
	if (scroll->flags & HSCROLLBAR_VISIBLE)
		widget_hscrollbar_draw(dpi, scroll, l, b - 10, (scroll->flags & VSCROLLBAR_VISIBLE ? r - 11 : r), b, colour);

	// Vertical scrollbar
	if (scroll->flags & VSCROLLBAR_VISIBLE)
		widget_vscrollbar_draw(dpi, scroll, r - 10, t, r, (scroll->flags & HSCROLLBAR_VISIBLE ? b - 11 : b), colour);

	// Contents
	if (scroll->flags & HSCROLLBAR_VISIBLE)
		b -= 11;
	if (scroll->flags & VSCROLLBAR_VISIBLE)
		r -= 11;

	b++;
	r++;

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
		window_event_scroll_paint_call(w, &scroll_dpi, scrollIndex);
}

static void widget_hscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int l, int t, int r, int b, int colour)
{
	colour &= 0x7F;
	// Trough
	gfx_fill_rect(dpi, l + 10, t, r - 10, b, *((char*)(0x0141FC4B + (colour * 8))));
	gfx_fill_rect(dpi, l + 10, t, r - 10, b, 0x1000000 | *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 10, t + 2, r - 10, t + 2, *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 10, t + 3, r - 10, t + 3, *((char*)(0x0141FC4B + (colour * 8))));
	gfx_fill_rect(dpi, l + 10, t + 7, r - 10, t + 7, *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 10, t + 8, r - 10, t + 8, *((char*)(0x0141FC4B + (colour * 8))));
	
	// Left button
	gfx_fill_rect_inset(dpi, l, t, l + 9, b, colour, (scroll->flags & HSCROLLBAR_LEFT_PRESSED ? 0x20 : 0));
	gfx_draw_string(dpi, (char*)BlackLeftArrowString, 0, l + 1, t);
	
	// Thumb
	gfx_fill_rect_inset(dpi,
		max(l + 10, l + scroll->h_thumb_left - 1), t,
		min(r - 10, l + scroll->h_thumb_right - 1), b,
		colour, (scroll->flags & HSCROLLBAR_THUMB_PRESSED ? 0x20 : 0));

	// Right button
	gfx_fill_rect_inset(dpi, r - 9, t, r, b, colour, (scroll->flags & HSCROLLBAR_RIGHT_PRESSED ? 0x20 : 0));
	gfx_draw_string(dpi, (char*)BlackRightArrowString, 0, r - 6, t);
}

static void widget_vscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, int l, int t, int r, int b, int colour)
{
	colour &= 0x7F;
	// Trough
	gfx_fill_rect(dpi, l, t + 10, r, b - 10, *((char*)(0x0141FC4B + (colour * 8))));
	gfx_fill_rect(dpi, l, t + 10, r, b - 10, 0x1000000 | *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 2, t + 10, l + 2, b - 10, *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 3, t + 10, l + 3, b - 10, *((char*)(0x0141FC4B + (colour * 8))));
	gfx_fill_rect(dpi, l + 7, t + 10, l + 7, b - 10, *((char*)(0x0141FC47 + (colour * 8))));
	gfx_fill_rect(dpi, l + 8, t + 10, l + 8, b - 10, *((char*)(0x0141FC4B + (colour * 8))));

	// Up button
	gfx_fill_rect_inset(dpi, l, t, r, t + 9, colour, (scroll->flags & VSCROLLBAR_UP_PRESSED ? 0x20 : 0));
	gfx_draw_string(dpi, (char*)BlackUpArrowString, 0, l + 1, t - 1);

	// Thumb
	gfx_fill_rect_inset(dpi,
		l, max(t + 10, t + scroll->v_thumb_top - 1),
		r, min(b - 10, t + scroll->v_thumb_bottom - 1),
		colour, (scroll->flags & VSCROLLBAR_THUMB_PRESSED ? 0x20 : 0));

	// Down button
	gfx_fill_rect_inset(dpi, l, b - 9, r, b, colour, (scroll->flags & VSCROLLBAR_DOWN_PRESSED ? 0x20 : 0));
	gfx_draw_string(dpi, (char*)BlackDownArrowString, 0, l + 1, b - 9);
}

/**
 * 
 *  rct2: 0x006EB951
 */
static void widget_draw_image(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	int l, t, r, b, colour, image;
	rct_widget *widget;

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Get the image
	image = widget->image;
	if (image == -1)
		return;

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Get the colour
	colour = w->colours[widget->colour] & 0x7F;

	if (widget->type == WWT_4 || widget->type == WWT_COLORBTN || widget->type == WWT_TRNBTN || widget->type == WWT_TAB)
		if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex))
			image++;

	if (widget_is_disabled(w, widgetIndex)) {
		// Draw greyed out (light border bottom right shadow)
		colour = w->colours[widget->colour];
		colour = RCT2_ADDRESS(0x00141FC4A, uint8)[(colour & 0x7F) * 8] & 0xFF;

		uint8 palette[256];
		memset(palette, colour, 256);
		palette[0] = 0;

		RCT2_GLOBAL(0x00EDF81C, uint32) = 0x20000000;
		image &= 0x7FFFF;
		gfx_draw_sprite_palette_set(dpi, image | 0x20000000, l + 1, t + 1, palette, NULL);

		// Draw greyed out (dark)
		colour = w->colours[widget->colour];
		colour = RCT2_ADDRESS(0x00141FC48, uint8)[(colour & 0x7F) * 8] & 0xFF;
		memset(palette, colour, 256);
		palette[0] = 0;

		RCT2_GLOBAL(0x00EDF81C, uint32) = 0x20000000;
		gfx_draw_sprite_palette_set(dpi, image | 0x20000000, l, t, palette, NULL);
	} else {
		if (image & 0x80000000) {
			// ?
		}

		if (image & 0x40000000)
			image &= ~0x40000000;
		else
			image |= colour << 19;

		gfx_draw_sprite(dpi, image, l, t, 0);
	}
}

int widget_is_enabled(rct_window *w, int widgetIndex)
{
	return (w->enabled_widgets & (1LL << widgetIndex)) ? 1 : 0;
}

int widget_is_disabled(rct_window *w, int widgetIndex)
{
	return (w->disabled_widgets & (1LL << widgetIndex)) ? 1 : 0;
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
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_WIDGET_PRESSED))
			return 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WIDGETINDEX, sint32) == widgetIndex)
			return 1;
	}
	return 0;
}

int widget_is_highlighted(rct_window *w, int widgetIndex)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWCLASS, rct_windowclass) != w->classification)
		return 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WINDOWNUMBER, rct_windownumber) != w->number)
		return 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_OVER_WIDGETINDEX, sint32) != widgetIndex)
		return 0;
	return 1;
}

int widget_is_active_tool(rct_window *w, int widgetIndex)
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE))
		return 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) != w->classification)
		return 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber) != w->number)
		return 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, sint32) != widgetIndex)
		return 0;

	return 1;
}

/**
 *  rct2: 0x006E9F92
 *	eax: x / output_x
 *	ebx: y / output_y
 *	ecx: output_scroll_area
 *  edx: scroll_id
 *	esi: w
 *	edi: widget
 */
void widget_scroll_get_part(rct_window *w, rct_widget* widget, int x, int y, int *output_x, int *output_y, int *output_scroll_area, int *scroll_id)
{
	rct_widget* iterator = w->widgets;
	*scroll_id = 0;
	while (++iterator != widget)
	{
		if (iterator->type == WWT_SCROLL)
		{
			(*scroll_id)++;
			break;
		}
	}

	if ((w->scrolls[*scroll_id].flags & HSCROLLBAR_VISIBLE) && y >= (w->y + widget->bottom - 11))
	{
		//horizon scrollbar
		int rightOffset = 0;
		int iteratorLeft = widget->left + w->x;
		int iteratorRight = widget->right + w->x;
		if (w->scrolls[*scroll_id].flags & VSCROLLBAR_VISIBLE)
		{
			rightOffset = 11;
		}
		if (x <= (iteratorLeft += 10))
		{
			*output_scroll_area = SCROLL_PART_HSCROLLBAR_LEFT;
		}
		else if (x >= (iteratorRight -= rightOffset))
		{
			*output_scroll_area = SCROLL_PART_NONE;
		}
		else if (x >= (iteratorRight -= 10))
		{
			*output_scroll_area = SCROLL_PART_HSCROLLBAR_RIGHT;
		}
		else if (x < (widget->left + w->x + w->scrolls[*scroll_id].h_thumb_left))
		{
			*output_scroll_area = SCROLL_PART_HSCROLLBAR_LEFT_TROUGH;
		}
		else if (x >(widget->left + w->x + w->scrolls[*scroll_id].h_thumb_right))
		{
			*output_scroll_area = SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH;
		}
		else
		{
			*output_scroll_area = SCROLL_PART_HSCROLLBAR_THUMB;
		}
	}
	else if ((w->scrolls[*scroll_id].flags & VSCROLLBAR_VISIBLE) && (x >= w->x + widget->right - 11))
	{
		//vertical scrollbar
		int bottomOffset = 0;
		int iteratorTop = widget->top + w->y;
		int iteratorBottom = widget->bottom + w->y;
		if (w->scrolls[*scroll_id].flags & HSCROLLBAR_VISIBLE)
		{
			bottomOffset = 11;
		}
		if (y <= (iteratorTop += 10))
		{
			*output_scroll_area = SCROLL_PART_VSCROLLBAR_TOP;
		}
		else if (y >= (iteratorBottom -= bottomOffset))
		{
			*output_scroll_area = SCROLL_PART_NONE;
		}
		else if (y >= (iteratorBottom -= 10))
		{
			*output_scroll_area = SCROLL_PART_VSCROLLBAR_BOTTOM;
		}
		else if (y < (widget->top + w->y + w->scrolls[*scroll_id].v_thumb_top))
		{
			*output_scroll_area = SCROLL_PART_VSCROLLBAR_TOP_TROUGH;
		}
		else if (y >(widget->top + w->y + w->scrolls[*scroll_id].v_thumb_bottom))
		{
			*output_scroll_area = SCROLL_PART_VSCROLLBAR_BOTTOM_TROUGH;
		}
		else
		{
			*output_scroll_area = SCROLL_PART_VSCROLLBAR_THUMB;
		}
	}
	else
	{
		//view
		*output_scroll_area = SCROLL_PART_VIEW;
		*output_x = x - widget->left;
		*output_y = y - widget->top;
		*output_x -= w->x;
		*output_y -= w->y;
		if (--*output_x < 0 || --*output_y < 0)
		{
			*output_scroll_area = SCROLL_PART_NONE;
		}
		else
		{
			*output_x += w->scrolls[*scroll_id].h_left;
			*output_y += w->scrolls[*scroll_id].v_top;
		}
	}
}

void widget_set_checkbox_value(rct_window *w, int widgetIndex, int value)
{
	if (value)
		w->pressed_widgets |= (1ULL << widgetIndex);
	else
		w->pressed_widgets &= ~(1ULL << widgetIndex);
}

static void widget_text_box_draw(rct_drawpixelinfo *dpi, rct_window *w, int widgetIndex)
{
	rct_widget* widget;
	int l, t, r, b;
	uint8 colour;
	int no_lines = 0;
	int font_height = 0;
	char wrapped_string[512];

	// Get the widget
	widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Get the colour
	colour = w->colours[widget->colour];


	bool active = w->classification == gCurrentTextBox.window.classification &&
		w->number == gCurrentTextBox.window.number &&
		widgetIndex == gCurrentTextBox.widget_index;

	//gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0x20 | (!active ? 0x40 : 0x00));
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0x60);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;

	if (!active) {

		if (w->widgets[widgetIndex].image != 0) {
			strcpy(wrapped_string, (char*)w->widgets[widgetIndex].image);
			gfx_wrap_string(wrapped_string, r - l - 5, &no_lines, &font_height);
			gfx_draw_string(dpi, wrapped_string, w->colours[1], l + 2, t);
		}
		return;
	}


	strcpy(wrapped_string, gTextBoxInput);

	// String length needs to add 12 either side of box
	// +13 for cursor when max length.
	gfx_wrap_string(wrapped_string, r - l - 5 - 6, &no_lines, &font_height);


	gfx_draw_string(dpi, wrapped_string, w->colours[1], l + 2, t);


	int string_length = get_string_size(wrapped_string) - 1;

	// Make a copy of the string for measuring the width.
	char temp_string[512] = { 0 };
	memcpy(temp_string, wrapped_string, min(string_length, gTextInputCursorPosition));
	int cur_x = l + gfx_get_string_width(temp_string) + 3;

	int width = 6;
	if ((uint32)gTextInputCursorPosition < strlen(gTextBoxInput)){
		// Make a new 1 character wide string for measuring the width
		// of the character that the cursor is under.
		temp_string[1] = '\0';
		temp_string[0] = gTextBoxInput[gTextInputCursorPosition];
		width = max(gfx_get_string_width(temp_string) - 2, 4);
	}

	if (gTextBoxFrameNo <= 15){
		uint8 colour = RCT2_ADDRESS(0x0141FC48, uint8)[w->colours[1] * 8];
		gfx_fill_rect(dpi, cur_x, t + 9, cur_x + width, t + 9, colour + 5);
	}
}
