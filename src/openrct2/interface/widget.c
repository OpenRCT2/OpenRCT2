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

#include "../drawing/drawing.h"
#include "../input.h"
#include "../sprites.h"
#include "widget.h"
#include "window.h"
#include "../platform/platform.h"
#include "../localisation/localisation.h"
#include "../util/util.h"

#include <math.h>

static void widget_frame_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_resize_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_button_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_tab_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_flat_button_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_text_button(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_text_unknown(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_text(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_text_inset(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_text_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_text_box_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_groupbox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_caption_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_checkbox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_closebox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_scroll_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);
static void widget_hscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, sint32 l, sint32 t, sint32 r, sint32 b, sint32 colour);
static void widget_vscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, sint32 l, sint32 t, sint32 r, sint32 b, sint32 colour);
static void widget_draw_image(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex);

/**
 *
 *  rct2: 0x006EAF26
 */
void widget_scroll_update_thumbs(rct_window *w, rct_widgetindex widget_index)
{
	rct_widget *widget = &w->widgets[widget_index];
	rct_scroll* scroll = &w->scrolls[window_get_scroll_data_index(w, widget_index)];

	if (scroll->flags & HSCROLLBAR_VISIBLE) {
		sint32 view_size = widget->right - widget->left - 21;
		if (scroll->flags & VSCROLLBAR_VISIBLE)
			view_size -= 11;
		sint32 x = scroll->h_left * view_size;
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

		if(scroll->h_thumb_right - scroll->h_thumb_left < 20) {
			double barPosition = (scroll->h_thumb_right * 1.0) / view_size;

			scroll->h_thumb_left = (uint16) lround(scroll->h_thumb_left - (20 * barPosition));
			scroll->h_thumb_right = (uint16) lround(scroll->h_thumb_right + (20 * (1 - barPosition)));
		}
	}

	if (scroll->flags & VSCROLLBAR_VISIBLE) {
		sint32 view_size = widget->bottom - widget->top - 21;
		if (scroll->flags & HSCROLLBAR_VISIBLE)
			view_size -= 11;
		sint32 y = scroll->v_top * view_size;
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

		if(scroll->v_thumb_bottom - scroll->v_thumb_top < 20) {
			double barPosition = (scroll->v_thumb_bottom * 1.0) / view_size;

			scroll->v_thumb_top = (uint16) lround(scroll->v_thumb_top - (20 * barPosition));
			scroll->v_thumb_bottom = (uint16) lround(scroll->v_thumb_bottom + (20 * (1 - barPosition)));
		}
	}

}

/**
 *
 *  rct2: 0x006EB2A8
 */
void widget_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
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
	case WWT_COLOURBTN:
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
static void widget_frame_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;

	//
	uint8 press = ((w->flags & WF_10) ? INSET_RECT_FLAG_FILL_MID_LIGHT : 0);

	// Get the colour
	uint8 colour = w->colours[widget->colour];

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
static void widget_resize_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;

	// Get the colour
	uint8 colour = w->colours[widget->colour];

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
static void widget_button_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;

	// Check if the button is pressed down
	uint8 press = widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex) ? INSET_RECT_FLAG_BORDER_INSET : 0;

	// Get the colour
	uint8 colour = w->colours[widget->colour];

	if (widget->image == -2) {
		// Draw border with no fill
		gfx_fill_rect_inset(dpi, l, t, r, b, colour, press | INSET_RECT_FLAG_FILL_NONE);
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
static void widget_tab_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	if (widget->image == -1)
		return;

	// Draw widgets that aren't explicitly disabled.
	if (!widget_is_disabled(w, widgetIndex)) {
		widget_draw_image(dpi, w, widgetIndex);
		return;
	}

	// Do not draw hidden tabs, unless given a sprite.
	if (widget->type == WWT_TAB && widget->image != (0x20000000 | SPR_G2_TAB_DISABLED))
		return;

	if (widget->type != WWT_TRNBTN) {
		widget_draw_image(dpi, w, widgetIndex);
		return;
	}

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;

	// Get the colour and disabled image
	uint8 colour = w->colours[widget->colour] & 0x7F;
	uint32 image = widget->image + 2;

	// Draw disabled image
	gfx_draw_sprite(dpi, image | (colour << 19), l, t, 0);
}

/**
 *
 *  rct2: 0x006EB861
 */
static void widget_flat_button_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	if (!widget_is_disabled(w, widgetIndex) && widget_is_highlighted(w, widgetIndex)) {
		widget_button_draw(dpi, w, widgetIndex);
		return;
	}

	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;

	// Get the colour
	uint8 colour = w->colours[widget->colour];

	// Check if the button is pressed down
	if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex)) {
		if (widget->image == -2) {
			// Draw border with no fill
			gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE);
			return;
		}

		// Draw the border with fill
		gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_FLAG_BORDER_INSET);
	}

	// Draw image
	widget_draw_image(dpi, w, widgetIndex);
}

/**
 *
 *  rct2: 0x006EBBEB
 */
static void widget_text_button(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;

	// Get the colour
	uint8 colour = w->colours[widget->colour];

	// Border
	uint8 press = widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex) ? INSET_RECT_FLAG_BORDER_INSET : 0;
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

	// Text
	widget_text_unknown(dpi, w, widgetIndex);
}

/**
 *
 *  rct2: 0x006EBC41
 */
static void widget_text_unknown(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Get the colour
	uint8 colour = w->colours[widget->colour];
	// do not use widget colour as this is already used as background for the text_button
	// colour = 2;

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;

	rct_string_id stringId = widget->text;
	if (stringId == STR_NONE)
		return;

	if (widget->type == WWT_11 && (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex)))
		// TODO: remove string addition
		stringId++;

	if (widget->type == WWT_13) {
		if (widget_is_disabled(w, widgetIndex))
			colour |= COLOUR_FLAG_INSET;
		gfx_draw_string_left_clipped(
			dpi,
			stringId,
			gCommonFormatArgs,
			colour,
			l + 1,
			t,
			widget->right - widget->left - 2
		);
	} else {
		colour &= ~(1 << 7);
		if (widget_is_disabled(w, widgetIndex))
			colour |= COLOUR_FLAG_INSET;
		gfx_draw_string_centred_clipped(
			dpi,
			stringId,
			gCommonFormatArgs,
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
static void widget_text(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Get the colour
	uint8 colour = w->colours[widget->colour];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;

	// TODO: -2 seems odd
	if (widget->text == (rct_string_id)-2 || widget->text == STR_NONE)
		return;

	if (widget_is_disabled(w, widgetIndex))
		colour |= COLOUR_FLAG_INSET;
	gfx_draw_string_left_clipped(dpi, widget->text, gCommonFormatArgs, colour, l + 1, t, r - l);
}

/**
 *
 *  rct2: 0x006EBD1F
 */
static void widget_text_inset(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;

	// Get the colour
	uint8 colour = w->colours[widget->colour];

	gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_F_60);
	widget_text(dpi, w, widgetIndex);
}

/**
 *
 *  rct2: 0x006EC1A6
 */
static void widget_text_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left + 5;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;

	// Get the colour
	uint8 colour = w->colours[widget->colour];

	sint32 press = 0;
	if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex))
		press |= INSET_RECT_FLAG_BORDER_INSET;

	gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

	// TODO

	gfx_fill_rect(dpi, l, t, r, b, colour);
}

/**
 *
 *  rct2: 0x006EB535
 */
static void widget_groupbox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left + 5;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;
	sint32 textRight = l;

	// Text
	if (widget->text != STR_NONE) {
		uint8 colour = w->colours[widget->colour] & 0x7F;
		if (widget_is_disabled(w, widgetIndex))
			colour |= 0x40;
		gfx_draw_string_left(dpi, widget->text, gCommonFormatArgs, colour, l, t);
		textRight = l + gfx_get_string_width(gCommonStringFormatBuffer) + 1;
	}

	// Border
	// Resolve the absolute ltrb
	l = w->x + widget->left;
	t = w->y + widget->top + 4;
	r = w->x + widget->right;
	b = w->y + widget->bottom;

	// Get the colour
	uint8 colour = w->colours[widget->colour] & 0x7F;

	// Border left of text
	gfx_fill_rect(dpi, l, t, l + 4, t, ColourMapA[colour].mid_dark);
	gfx_fill_rect(dpi, l + 1, t + 1, l + 4, t + 1, ColourMapA[colour].lighter);

	// Border right of text
	gfx_fill_rect(dpi, textRight, t, r - 1, t, ColourMapA[colour].mid_dark);
	gfx_fill_rect(dpi, textRight, t + 1, r - 2, t + 1, ColourMapA[colour].lighter);

	// Border right
	gfx_fill_rect(dpi, r - 1, t + 1, r - 1, b - 1, ColourMapA[colour].mid_dark);
	gfx_fill_rect(dpi, r, t, r, b, ColourMapA[colour].lighter);

	// Border bottom
	gfx_fill_rect(dpi, l, b - 1, r - 2, b - 1, ColourMapA[colour].mid_dark);
	gfx_fill_rect(dpi, l, b, r - 1, b, ColourMapA[colour].lighter);

	// Border left
	gfx_fill_rect(dpi, l, t + 1, l, b - 2, ColourMapA[colour].mid_dark);
	gfx_fill_rect(dpi, l + 1, t + 2, l + 1, b - 2, ColourMapA[colour].lighter);
}

/**
 *
 *  rct2: 0x006EB2F9
 */
static void widget_caption_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;

	// Get the colour
	uint8 colour = w->colours[widget->colour];

	uint8 press = INSET_RECT_F_60;
	if (w->flags & WF_10)
		press |= INSET_RECT_FLAG_FILL_MID_LIGHT;

	gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

	// Black caption bars look slightly green, this fixes that
	if (colour == 0)
		gfx_fill_rect(dpi, l + 1, t + 1, r - 1, b - 1, ColourMapA[colour].dark);
	else
		gfx_filter_rect(dpi, l + 1, t + 1, r - 1, b - 1, PALETTE_DARKEN_3);

	// Draw text
	if (widget->text == STR_NONE)
		return;

	l = widget->left + w->x + 2;
	t = widget->top + w->y + 1;
	sint32 width = widget->right - widget->left - 4;
	if ((widget + 1)->type == WWT_CLOSEBOX) {
		width -= 10;
		if ((widget + 2)->type == WWT_CLOSEBOX)
			width -= 10;
	}
	l += width / 2;
	gfx_draw_string_centred_clipped(dpi, widget->text, gCommonFormatArgs, COLOUR_WHITE | COLOUR_FLAG_OUTLINE, l, t, width);
}

/**
 *
 *  rct2: 0x006EBB85
 */
static void widget_closebox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;

	// Check if the button is pressed down
	uint8 press = 0;
	if (w->flags & WF_10)
		press |= INSET_RECT_FLAG_FILL_MID_LIGHT;
	if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex))
		press |= INSET_RECT_FLAG_BORDER_INSET;

	// Get the colour
	uint8 colour = w->colours[widget->colour];

	// Draw the button
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, press);

	if (widget->text == STR_NONE)
		return;

	l = w->x + (widget->left + widget->right) / 2 - 1;
	t = w->y + max(widget->top, (widget->top + widget->bottom) / 2 - 5);

	if (widget_is_disabled(w, widgetIndex))
		colour |= COLOUR_FLAG_INSET;

	gfx_draw_string_centred_clipped(dpi, widget->text, gCommonFormatArgs, colour, l, t, widget->right - widget->left - 2);
}

/**
 *
*  rct2: 0x006EBAD9
*/
static void widget_checkbox_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 b = w->y + widget->bottom;
	sint32 yMid = (b + t) / 2;

	// Get the colour
	uint8 colour = w->colours[widget->colour];

	if (widget->type != WWT_24) {
		// checkbox
		gfx_fill_rect_inset(dpi, l, yMid - 5, l + 9, yMid + 4, colour, INSET_RECT_F_60);

		// fill it when checkbox is pressed
		if (widget_is_pressed(w, widgetIndex)) {
			gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
			gfx_draw_string(dpi, (char*)CheckBoxMarkString, NOT_TRANSLUCENT(colour), l, yMid - 5);
		}
	}

	// draw the text
	if (widget->text == STR_NONE)
		return;

	if (widget_is_disabled(w, widgetIndex)) {
		colour |= COLOUR_FLAG_INSET;
	}

	gfx_draw_string_left_centred(dpi, widget->text, gCommonFormatArgs, colour, l + 14, yMid);
}

/**
 *
 *  rct2: 0x006EBD96
 */
static void widget_scroll_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	sint32 scrollIndex = window_get_scroll_data_index(w, widgetIndex);
	rct_widget *widget = &w->widgets[widgetIndex];
	rct_scroll* scroll = &w->scrolls[scrollIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;

	// Get the colour
	uint8 colour = w->colours[widget->colour];

	// Draw the border
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_F_60);

	// Inflate by -1
	l++;
	t++;
	r--;
	b--;

	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

	// Horizontal scrollbar
	if (scroll->flags & HSCROLLBAR_VISIBLE)
		widget_hscrollbar_draw(dpi, scroll, l, b - 10, ((scroll->flags & VSCROLLBAR_VISIBLE) ? r - 11 : r), b, colour);

	// Vertical scrollbar
	if (scroll->flags & VSCROLLBAR_VISIBLE)
		widget_vscrollbar_draw(dpi, scroll, r - 10, t, r, ((scroll->flags & HSCROLLBAR_VISIBLE) ? b - 11 : b), colour);

	// Contents
	if (scroll->flags & HSCROLLBAR_VISIBLE)
		b -= 11;
	if (scroll->flags & VSCROLLBAR_VISIBLE)
		r -= 11;

	b++;
	r++;

	// Create a new inner scroll dpi
	rct_drawpixelinfo scroll_dpi = *dpi;

	// Clip the scroll dpi against the outer dpi
	sint32 cl = max(dpi->x, l);
	sint32 ct = max(dpi->y, t);
	sint32 cr = min(dpi->x + dpi->width, r);
	sint32 cb = min(dpi->y + dpi->height, b);

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

static void widget_hscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, sint32 l, sint32 t, sint32 r, sint32 b, sint32 colour)
{
	colour &= 0x7F;
	// Trough
	gfx_fill_rect(dpi, l + 10, t, r - 10, b, ColourMapA[colour].lighter);
	gfx_fill_rect(dpi, l + 10, t, r - 10, b, 0x1000000 | ColourMapA[colour].mid_dark);
	gfx_fill_rect(dpi, l + 10, t + 2, r - 10, t + 2, ColourMapA[colour].mid_dark);
	gfx_fill_rect(dpi, l + 10, t + 3, r - 10, t + 3, ColourMapA[colour].lighter);
	gfx_fill_rect(dpi, l + 10, t + 7, r - 10, t + 7, ColourMapA[colour].mid_dark);
	gfx_fill_rect(dpi, l + 10, t + 8, r - 10, t + 8, ColourMapA[colour].lighter);

	// Left button
	gfx_fill_rect_inset(dpi, l, t, l + 9, b, colour, ((scroll->flags & HSCROLLBAR_LEFT_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
	gfx_draw_string(dpi, (char*)BlackLeftArrowString, COLOUR_BLACK, l + 1, t);

	// Thumb
	gfx_fill_rect_inset(dpi,
		max(l + 10, l + scroll->h_thumb_left - 1), t,
		min(r - 10, l + scroll->h_thumb_right - 1), b,
		colour, ((scroll->flags & HSCROLLBAR_THUMB_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));

	// Right button
	gfx_fill_rect_inset(dpi, r - 9, t, r, b, colour, ((scroll->flags & HSCROLLBAR_RIGHT_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
	gfx_draw_string(dpi, (char*)BlackRightArrowString, COLOUR_BLACK, r - 6, t);
}

static void widget_vscrollbar_draw(rct_drawpixelinfo *dpi, rct_scroll *scroll, sint32 l, sint32 t, sint32 r, sint32 b, sint32 colour)
{
	colour &= 0x7F;
	// Trough
	gfx_fill_rect(dpi, l, t + 10, r, b - 10, ColourMapA[colour].lighter);
	gfx_fill_rect(dpi, l, t + 10, r, b - 10, 0x1000000 | ColourMapA[colour].mid_dark);
	gfx_fill_rect(dpi, l + 2, t + 10, l + 2, b - 10, ColourMapA[colour].mid_dark);
	gfx_fill_rect(dpi, l + 3, t + 10, l + 3, b - 10, ColourMapA[colour].lighter);
	gfx_fill_rect(dpi, l + 7, t + 10, l + 7, b - 10, ColourMapA[colour].mid_dark);
	gfx_fill_rect(dpi, l + 8, t + 10, l + 8, b - 10, ColourMapA[colour].lighter);

	// Up button
	gfx_fill_rect_inset(dpi, l, t, r, t + 9, colour, ((scroll->flags & VSCROLLBAR_UP_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
	gfx_draw_string(dpi, (char*)BlackUpArrowString, COLOUR_BLACK, l + 1, t - 1);

	// Thumb
	gfx_fill_rect_inset(dpi,
		l, max(t + 10, t + scroll->v_thumb_top - 1),
		r, min(b - 10, t + scroll->v_thumb_bottom - 1),
		colour, ((scroll->flags & VSCROLLBAR_THUMB_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));

	// Down button
	gfx_fill_rect_inset(dpi, l, b - 9, r, b, colour, ((scroll->flags & VSCROLLBAR_DOWN_PRESSED) ? INSET_RECT_FLAG_BORDER_INSET : 0));
	gfx_draw_string(dpi, (char*)BlackDownArrowString, COLOUR_BLACK, l + 1, b - 9);
}

/**
 *
 *  rct2: 0x006EB951
 */
static void widget_draw_image(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Get the image
	sint32 image = widget->image;
	if (image == SPR_NONE)
		return;

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;

	// Get the colour
	uint8 colour = NOT_TRANSLUCENT(w->colours[widget->colour]);

	if (widget->type == WWT_4 || widget->type == WWT_COLOURBTN || widget->type == WWT_TRNBTN || widget->type == WWT_TAB)
		if (widget_is_pressed(w, widgetIndex) || widget_is_active_tool(w, widgetIndex))
			image++;

	if (widget_is_disabled(w, widgetIndex)) {
		// Draw greyed out (light border bottom right shadow)
		colour = w->colours[widget->colour];
		colour = ColourMapA[NOT_TRANSLUCENT(colour)].lighter;
		gfx_draw_sprite_solid(dpi, image, l + 1, t + 1, colour);

		// Draw greyed out (dark)
		colour = w->colours[widget->colour];
		colour = ColourMapA[NOT_TRANSLUCENT(colour)].mid_light;
		gfx_draw_sprite_solid(dpi, image, l, t, colour);
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

sint32 widget_is_enabled(rct_window *w, rct_widgetindex widgetIndex)
{
	return (w->enabled_widgets & (1LL << widgetIndex)) ? 1 : 0;
}

sint32 widget_is_disabled(rct_window *w, rct_widgetindex widgetIndex)
{
	return (w->disabled_widgets & (1LL << widgetIndex)) ? 1 : 0;
}

sint32 widget_is_pressed(rct_window *w, rct_widgetindex widgetIndex)
{
	if (w->pressed_widgets & (1LL << widgetIndex)) {
		return 1;
	}
	if (input_get_state() == INPUT_STATE_WIDGET_PRESSED || input_get_state() == INPUT_STATE_DROPDOWN_ACTIVE) {
		if (!(input_test_flag(INPUT_FLAG_WIDGET_PRESSED))) return 0;
		if (gPressedWidget.window_classification != w->classification) return 0;
		if (gPressedWidget.window_number != w->number) return 0;
		if (gPressedWidget.widget_index != widgetIndex) return 0;
		return 1;
	}
	return 0;
}

sint32 widget_is_highlighted(rct_window *w, rct_widgetindex widgetIndex)
{
	if (gHoverWidget.window_classification != w->classification) return 0;
	if (gHoverWidget.window_number != w->number) return 0;
	if (gHoverWidget.widget_index != widgetIndex) return 0;
	return 1;
}

sint32 widget_is_active_tool(rct_window *w, rct_widgetindex widgetIndex)
{
	if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
		return 0;
	if (gCurrentToolWidget.window_classification != w->classification)
		return 0;
	if (gCurrentToolWidget.window_number != w->number)
		return 0;
	if (gCurrentToolWidget.widget_index != widgetIndex)
		return 0;

	return 1;
}

/**
 *
 *  rct2: 0x006E9F92
 *	eax: x / output_x
 *	ebx: y / output_y
 *	ecx: output_scroll_area
 *  edx: scroll_id
 *	esi: w
 *	edi: widget
 */
void widget_scroll_get_part(rct_window *w, rct_widget *widget, sint32 x, sint32 y, sint32 *output_x, sint32 *output_y, sint32 *output_scroll_area, sint32 *scroll_id)
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
		// horizontal scrollbar
		sint32 rightOffset = 0;
		sint32 iteratorLeft = widget->left + w->x + 10;
		sint32 iteratorRight = widget->right + w->x - 10;
		if (!(w->scrolls[*scroll_id].flags & VSCROLLBAR_VISIBLE))
		{
			rightOffset = 11;
		}

		if (x <= iteratorLeft)
		{
			*output_scroll_area = SCROLL_PART_HSCROLLBAR_LEFT;
		}
		else if (x >= iteratorRight + rightOffset)
		{
			*output_scroll_area = SCROLL_PART_NONE;
		}
		else if (x >= iteratorRight + rightOffset - 10)
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
		// vertical scrollbar
		sint32 bottomOffset = 0;
		sint32 iteratorTop = widget->top + w->y + 10;
		sint32 iteratorBottom = widget->bottom + w->y;
		if (w->scrolls[*scroll_id].flags & HSCROLLBAR_VISIBLE)
		{
			bottomOffset = 11;
		}

		if (y <= iteratorTop)
		{
			*output_scroll_area = SCROLL_PART_VSCROLLBAR_TOP;
		}
		else if (y >= (iteratorBottom - bottomOffset))
		{
			*output_scroll_area = SCROLL_PART_NONE;
		}
		else if (y >= (iteratorBottom - bottomOffset - 10))
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
		if (*output_x <= 0 || *output_y <= 0)
		{
			*output_scroll_area = SCROLL_PART_NONE;
		}
		else
		{
			*output_x += w->scrolls[*scroll_id].h_left - 1;
			*output_y += w->scrolls[*scroll_id].v_top - 1;
		}
	}
}

void widget_set_enabled(rct_window *w, rct_widgetindex widgetIndex, bool enabled)
{
	if (enabled) {
		w->enabled_widgets |= (1ULL << widgetIndex);
		w->disabled_widgets &= ~(1ULL << widgetIndex);
	}
	else {
		w->enabled_widgets &= ~(1ULL << widgetIndex);
		w->disabled_widgets |= (1ULL << widgetIndex);
	}
}

void widget_set_checkbox_value(rct_window *w, rct_widgetindex widgetIndex, sint32 value)
{
	if (value)
		w->pressed_widgets |= (1ULL << widgetIndex);
	else
		w->pressed_widgets &= ~(1ULL << widgetIndex);
}

static void widget_text_box_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex)
{
	sint32 no_lines = 0;
	sint32 font_height = 0;
	char wrapped_string[TEXT_INPUT_SIZE];

	// Get the widget
	rct_widget *widget = &w->widgets[widgetIndex];

	// Resolve the absolute ltrb
	sint32 l = w->x + widget->left;
	sint32 t = w->y + widget->top;
	sint32 r = w->x + widget->right;
	sint32 b = w->y + widget->bottom;

	// Get the colour
	uint8 colour = w->colours[widget->colour];

	bool active = w->classification == gCurrentTextBox.window.classification &&
		w->number == gCurrentTextBox.window.number &&
		widgetIndex == gCurrentTextBox.widget_index;

	//gfx_fill_rect_inset(dpi, l, t, r, b, colour, 0x20 | (!active ? 0x40 : 0x00));
	gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_F_60);

	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	gCurrentFontFlags = 0;

	if (!active) {

		if (w->widgets[widgetIndex].text != 0) {
			safe_strcpy(wrapped_string, w->widgets[widgetIndex].string, 512);
			gfx_wrap_string(wrapped_string, r - l - 5, &no_lines, &font_height);
			gfx_draw_string(dpi, wrapped_string, w->colours[1], l + 2, t);
		}
		return;
	}


	safe_strcpy(wrapped_string, gTextBoxInput, TEXT_INPUT_SIZE);

	// String length needs to add 12 either side of box
	// +13 for cursor when max length.
	gfx_wrap_string(wrapped_string, r - l - 5 - 6, &no_lines, &font_height);


	gfx_draw_string(dpi, wrapped_string, w->colours[1], l + 2, t);


	size_t string_length = get_string_size(wrapped_string) - 1;

	// Make a copy of the string for measuring the width.
	char temp_string[TEXT_INPUT_SIZE] = { 0 };
	memcpy(temp_string, wrapped_string, min(string_length, gTextInput.selection_offset));
	sint32 cur_x = l + gfx_get_string_width(temp_string) + 3;

	sint32 width = 6;
	if ((uint32)gTextInput.selection_offset < strlen(gTextBoxInput)){
		// Make a new 1 character wide string for measuring the width
		// of the character that the cursor is under.
		temp_string[1] = '\0';
		temp_string[0] = gTextBoxInput[gTextInput.selection_offset];
		width = max(gfx_get_string_width(temp_string) - 2, 4);
	}

	if (gTextBoxFrameNo <= 15){
		colour = ColourMapA[w->colours[1]].mid_light;
		gfx_fill_rect(dpi, cur_x, t + 9, cur_x + width, t + 9, colour + 5);
	}
}
