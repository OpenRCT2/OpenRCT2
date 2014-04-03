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
#include "widget.h"
#include "window.h"

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