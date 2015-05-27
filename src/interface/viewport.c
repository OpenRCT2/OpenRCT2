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
#include "../config.h"
#include "../drawing/drawing.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../world/map.h"
#include "../world/sprite.h"
#include "../world/banner.h"
#include "../world/scenery.h"
#include "viewport.h"
#include "window.h"

#define RCT2_FIRST_VIEWPORT		(RCT2_ADDRESS(RCT2_ADDRESS_VIEWPORT_LIST, rct_viewport))
#define RCT2_LAST_VIEWPORT		(RCT2_ADDRESS(RCT2_ADDRESS_ACTIVE_VIEWPORT_PTR_ARRAY, rct_viewport) - 1)
#define RCT2_NEW_VIEWPORT		(RCT2_GLOBAL(RCT2_ADDRESS_ACTIVE_VIEWPORT_PTR_ARRAY, rct_viewport*))

//#define DEBUG_SHOW_DIRTY_BOX

rct_viewport* g_viewport_list = RCT2_ADDRESS(RCT2_ADDRESS_VIEWPORT_LIST, rct_viewport);

typedef struct paint_struct paint_struct;

struct paint_struct{
	uint32 image_id;		// 0x00
	uint32 var_04;
	uint16 attached_x;		// 0x08
	uint16 attached_y;		// 0x0A
	union {
		struct {
			uint8 var_0C;
			uint8 pad_0D;
			paint_struct* next_attached_ps;	//0x0E
			uint16 pad_12;
		};
		struct {
			uint16 some_x; // 0x0C
			uint16 some_y; // 0x0E
			uint16 other_x; // 0x10
			uint16 other_y; // 0x12
		};
	};
	uint16 x;				// 0x14
	uint16 y;				// 0x16
	uint16 var_18;
	uint8 var_1A;
	uint8 var_1B;
	paint_struct* attached_ps;	//0x1C
	paint_struct* var_20;
	paint_struct* var_24;
	uint8 sprite_type;		//0x28
	uint8 var_29;
	uint16 pad_2A;
	uint16 map_x;			// 0x2C
	uint16 map_y;			// 0x2E
	rct_map_element *mapElement; // 0x30
};

/**
 *  This is not a viewport function. It is used to setup many variables for
 *  multiple things.
 *  rct2: 0x006E6EAC
 */
void viewport_init_all()
{
	int i, d;
	rct_g1_element *g1_element;

	// Palette from sprites?
	d = 0;
	for (i = 4915; i < 4947; i++) {
		g1_element = &g1Elements[i];
		*((int*)(0x0141FC44 + d)) = *((int*)(&g1_element->offset[0xF5]));
		*((int*)(0x0141FC48 + d)) = *((int*)(&g1_element->offset[0xF9]));
		*((int*)(0x0141FD44 + d)) = *((int*)(&g1_element->offset[0xFD]));
		d += 8;
	}

	// Setting up windows
	RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*) = g_window_list;
	RCT2_GLOBAL(0x01423604, sint32) = 0;

	// Setting up viewports
	for (i = 0; i < 9; i++)
		g_viewport_list[i].width = 0;
	RCT2_NEW_VIEWPORT = NULL;

	// ?
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, sint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, sint8) = INPUT_STATE_RESET;
	RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass) = -1;
	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, sint32) = -1;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_NOT_SHOWN_TICKS, sint16) = -1;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, sint16) = 0;
	RCT2_GLOBAL(0x009DEA50, sint16) = -1;
	textinput_cancel();
	format_string((char*)0x0141FA44, STR_CANCEL, NULL);
	format_string((char*)0x0141F944, STR_OK, NULL);
}
/**
 *  rct:0x006EB0C1
 *  x : ax
 *  y : bx
 *  z : cx
 *  out_x : ax
 *  out_y : bx
 *  Converts between 3d point of a sprite to 2d coordinates for centering on that sprite
 */
void center_2d_coordinates(int x, int y, int z, int* out_x, int* out_y, rct_viewport* viewport){
	int start_x = x;

	switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)){
	case 0:
		x = y - x;
		y = (y + start_x) / 2 - z;
		break;
	case 1:
		x = -y - x;
		y = (y - start_x) / 2 - z;
		break;
	case 2:
		x = -y + x;
		y = (-y - start_x) / 2 - z;
		break;
	case 3:
		x = y + x;
		y = (-y + start_x) / 2 - z;
		break;
	}

	*out_x = x - viewport->view_width / 2;
	*out_y = y - viewport->view_height / 2;
}

/**
*
*  rct2: 0x006EB009
*  x:      ax
*  y:      eax (top 16)
*  width:  bx
*  height: ebx (top 16)
*  zoom:    cl (8 bits)
*  center_x: edx lower 16 bits
*  center_y: edx upper 16 bits
*  center_z: ecx upper 16 bits
*  sprite: edx lower 16 bits
*  flags:  edx top most 2 bits 0b_X1 for zoom clear see below for 2nd bit.
*  w:      esi
*
*  Viewport will look at sprite or at coordinates as specified in flags 0b_1X for sprite 0b_0X for coordinates
*/
void viewport_create(rct_window *w, int x, int y, int width, int height, int zoom, int center_x, int center_y, int center_z, char flags, sint16 sprite)
{
	rct_viewport* viewport;
	for (viewport = g_viewport_list; viewport->width != 0; viewport++){
		if (viewport > RCT2_LAST_VIEWPORT){
			error_string_quit(0xFF000001, -1);
		}
	}

	viewport->x = x;
	viewport->y = y;
	viewport->width = width;
	viewport->height = height;

	if (!(flags & VIEWPORT_FOCUS_TYPE_COORDINATE)){
		zoom = 0;
	}

	viewport->view_width = width << zoom;
	viewport->view_height = height << zoom;
	viewport->zoom = zoom;
	viewport->flags = 0;

	if (gConfigGeneral.always_show_gridlines)
		viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
	w->viewport = viewport;

	if (flags & VIEWPORT_FOCUS_TYPE_SPRITE){
		w->viewport_target_sprite = sprite;
		rct_sprite* center_sprite = &g_sprite_list[sprite];
		center_x = center_sprite->unknown.x;
		center_y = center_sprite->unknown.y;
		center_z = center_sprite->unknown.z;
	}
	else{
		w->viewport_target_sprite = SPR_NONE;
	}

	int view_x, view_y;
	center_2d_coordinates(center_x, center_y, center_z, &view_x, &view_y, viewport);

	w->saved_view_x = view_x;
	w->saved_view_y = view_y;
	viewport->view_x = view_x;
	viewport->view_y = view_y;

	viewport_update_pointers();

	//x &= 0xFFFF;
	//y &= 0xFFFF;
	//RCT2_CALLPROC_X(0x006EB009, (y << 16) | x, (height << 16) | width, zoom, edx, (int)w, 0, 0);
}

/**
 *
 *  rct2: 0x006EE510
 */
void viewport_update_pointers()
{
	rct_viewport *viewport;
	rct_viewport **vp = RCT2_ADDRESS(RCT2_ADDRESS_ACTIVE_VIEWPORT_PTR_ARRAY, rct_viewport*);

	// The last possible viewport entry is 1 before what is the active viewport_ptr_array
	for (viewport = g_viewport_list; viewport <= RCT2_LAST_VIEWPORT; viewport++)
		if (viewport->width != 0)
			*vp++ = viewport;

	*vp = NULL;
}

/**
 * edx is assumed to be (and always is) the current rotation, so it is not needed as parameter.
 * rct2: 0x00689174
 */
void sub_689174(sint16* x, sint16* y, sint16 *z)
{
	//RCT2_CALLFUNC_X(0x00689174, (int*)&x, (int*)&y, (int*)&z, &curr_rotation, (int*)&window, (int*)&viewport, &ebp);

	sint16 start_x = *x;
	sint16 start_y = *y;
	sint16 height = 0;

	rct_xy16 pos;
	for (int i = 0; i < 6; i++) {
		pos = viewport_coord_to_map_coord(start_x, start_y, height);
		height = map_element_height((0xFFFF) & pos.x, (0xFFFF) & pos.y);

		// HACK: This is to prevent the x and y values being set to values outside
		// of the map. This can happen when the height is larger than the map size.
		sint16 max = RCT2_GLOBAL(RCT2_ADDRESS_MAP_MAXIMUM_X_Y, sint16);
		if (pos.x > max && pos.y > max) {
			int x_corr[] = { -1, 1, 1, -1 };
			int y_corr[] = { -1, -1, 1, 1 };
			uint32 rotation = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32);
			pos.x += x_corr[rotation] * height;
			pos.y += y_corr[rotation] * height;
		}
	}

	*x = pos.x;
	*y = pos.y;
	*z = height;
}

//void sub_6E7FF3(rct_window* w, rct_viewport* viewport, int x, int y){
//	int zoom = 1 << viewport->zoom;
//	if (w >= RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*)){
//		if (viewport != w->viewport){
//			if ((viewport->x + viewport->width > w->x) &&
//				(w->x + w->width > viewport->x) &&
//				(viewport->y + viewport->height > w->y) &&
//				(w->y + w->height > viewport->y)){
//				if (viewport->x < w->x){
//					rct_viewport viewport_bkup;
//					memcpy(&viewport_bkup, viewport, sizeof(rct_viewport));
//
//					viewport->width = w->x - viewport->x;
//					viewport->view_width = (w->x - viewport->x) * zoom;
//
//					sub_6E7FF3(w, viewport, x, y);
//					
//					viewport->x += viewport->width;
//					viewport->view_x += viewport->width*zoom;
//					viewport->view_width = (viewport_bkup.width - viewport->width) * zoom;
//					viewport->width = viewport_bkup.width - viewport->width;
//					
//					sub_6E7FF3(w, viewport, x, y);
//
//					memcpy(viewport, &viewport_bkup, sizeof(rct_viewport));
//					return;
//				}//x6E80C4
//			}//0x6E824a
//		} // 0x6e824a
//	}//x6e8255
//
//}

void sub_6E7F34(rct_window* w, rct_viewport* viewport, sint16 x_diff, sint16 y_diff){
	//RCT2_CALLPROC_X(0x6E7F34, 0, 0, 0, x_diff, (int)viewport, (int)w, y_diff);
	rct_window* orignal_w = w;
	int left = 0, right = 0, top = 0, bottom = 0;

	for (; w < RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*); w++){
		if (!(w->flags & WF_TRANSPARENT)) continue;
		if (w->viewport == viewport) continue;

		if (viewport->x + viewport->width <= w->x)continue;
		if (w->x + w->width <= viewport->x) continue;

		if (viewport->y + viewport->height <= w->y)continue;
		if (w->y + w->height <= viewport->y) continue;

		left = w->x;
		right = w->x + w->width;
		top = w->y;
		bottom = w->y + w->height;

		if (left < viewport->x)left = viewport->x;
		if (right > viewport->x + viewport->width) right = viewport->x + viewport->width;

		if (top < viewport->y)top = viewport->y;
		if (bottom > viewport->y + viewport->height) bottom = viewport->y + viewport->height;

		if (left >= right) continue;
		if (top >= bottom) continue;

		gfx_redraw_screen_rect(left, top, right, bottom);
	}

	w = orignal_w;
	RCT2_CALLPROC_X(0x6E7FF3, 0, 0, 0, x_diff, (int)viewport, (int)w, y_diff);
}

void sub_6E7DE1(sint16 x, sint16 y, rct_window* w, rct_viewport* viewport){
	//RCT2_CALLPROC_X(0x6E7DE1, x, y, 0, 0, w, viewport, 0);
	//return;
	uint8 zoom = (1 << viewport->zoom);

	// Note: do not do the subtraction and then divide!
	// Note: Due to arithmatic shift != /zoom a shift will have to be used
	// hopefully when 0x006E7FF3 is finished this can be converted to /zoom.
	sint16 x_diff = (viewport->view_x >> viewport->zoom) - (x >> viewport->zoom);
	sint16 y_diff = (viewport->view_y >> viewport->zoom) - (y >> viewport->zoom);

	viewport->view_x = x;
	viewport->view_y = y;

	// If no change in viewing area
	if ((!x_diff) && (!y_diff))return;

	if (w->flags & WF_7){
		int left = max(viewport->x, 0);
		int top = max(viewport->y, 0);
		int right = min(viewport->x + viewport->width, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16));
		int bottom = min(viewport->y + viewport->height, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16));

		if (left >= right) return;
		if (top >= bottom) return;

		gfx_redraw_screen_rect(left, top, right, bottom);
		return;
	}

	rct_viewport view_copy;
	memcpy(&view_copy, viewport, sizeof(rct_viewport));

	if (viewport->x < 0){
		viewport->width += viewport->x;
		viewport->view_width += viewport->x * zoom;
		viewport->view_x -= viewport->x * zoom;
		viewport->x = 0;
	}

	int eax = viewport->x + viewport->width - RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16);
	if (eax > 0){
		viewport->width -= eax;
		viewport->view_width -= eax * zoom;
	}

	if (viewport->width <= 0){
		memcpy(viewport, &view_copy, sizeof(rct_viewport));
		return;
	}

	if (viewport->y < 0){
		viewport->height += viewport->y;
		viewport->view_height += viewport->y * zoom;
		viewport->view_y -= viewport->y * zoom;
		viewport->y = 0;
	}

	eax = viewport->y + viewport->height - RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16);
	if (eax > 0){
		viewport->height -= eax;
		viewport->view_height -= eax * zoom;
	}

	if (viewport->height <= 0){
		memcpy(viewport, &view_copy, sizeof(rct_viewport));
		return;
	}

	sub_6E7F34(w, viewport, x_diff, y_diff);
	//RCT2_CALLPROC_X(0x6E7F34, 0, 0, 0, x_diff, (int)viewport, (int)w, y_diff);

	memcpy(viewport, &view_copy, sizeof(rct_viewport));
}

//rct2: 0x006E7A15
void viewport_set_underground_flag(int underground, rct_window* window, rct_viewport* viewport)
{
	if (window->classification != WC_MAIN_WINDOW)
	{
		if (!underground)
		{
			int bit = viewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE;
			viewport->flags &= ~VIEWPORT_FLAG_UNDERGROUND_INSIDE;
			if (!bit) return;
		}
		else
		{
			int bit = viewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE;
			viewport->flags |= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
			if (bit) return;
		}
		window_invalidate(window);
	}
}

/**
 *
 *  rct2: 0x006E7A3A
 */
void viewport_update_position(rct_window *window)
{
	//RCT2_CALLPROC_X(0x006E7A3A, 0, 0, 0, 0, (int)window, 0, 0);

	RCT2_CALLPROC_X(window->event_handlers[WE_RESIZE], 0, 0, 0, 0, (int)window, 0, 0);

	rct_viewport* viewport = window->viewport;
	if (!viewport)return;

	if (window->viewport_target_sprite != -1){
		rct_sprite* sprite = &g_sprite_list[window->viewport_target_sprite];

		int height = (map_element_height(0xFFFF & sprite->unknown.x, 0xFFFF & sprite->unknown.y) & 0xFFFF) - 16;
		int underground = sprite->unknown.z < height;

		viewport_set_underground_flag(underground, window, viewport);
		//RCT2_CALLPROC_X(0x6E7A15, sprite->unknown.x, sprite->unknown.y, sprite->unknown.z, underground, (int)window, (int)viewport, 0);

		int center_x, center_y;
		center_2d_coordinates(sprite->unknown.x, sprite->unknown.y, sprite->unknown.z, &center_x, &center_y, window->viewport);

		sub_6E7DE1(center_x, center_y, window, viewport);
		//RCT2_CALLPROC_X(0x6E7DE1, center_x, center_y, 0, 0, (int)window, (int)viewport, 0);
		return;
	}


	sint16 x = viewport->view_width / 2 + window->saved_view_x;
	sint16 y = viewport->view_height / 2 + window->saved_view_y;
	sint16 z;

	sub_689174(&x, &y, &z);

	viewport_set_underground_flag(0, window, viewport);
	//RCT2_CALLPROC_X(0x006E7A15, x, y, z, 0, (int)window, (int)viewport, 0);

	//Clamp to the map minimum value
	int at_map_edge = 0;
	if (x < MAP_MINIMUM_X_Y){
		x = MAP_MINIMUM_X_Y;
		at_map_edge = 1;
	}
	if (y < MAP_MINIMUM_X_Y){
		y = MAP_MINIMUM_X_Y;
		at_map_edge = 1;
	}

	//Clamp to the map maximum value (scenario specific)
	if (x > RCT2_GLOBAL(RCT2_ADDRESS_MAP_MAXIMUM_X_Y, sint16)){
		x = RCT2_GLOBAL(RCT2_ADDRESS_MAP_MAXIMUM_X_Y, sint16);
		at_map_edge = 1;
	}
	if (y > RCT2_GLOBAL(RCT2_ADDRESS_MAP_MAXIMUM_X_Y, sint16)){
		y = RCT2_GLOBAL(RCT2_ADDRESS_MAP_MAXIMUM_X_Y, sint16);
		at_map_edge = 1;
	}

	if (at_map_edge) {
		// The &0xFFFF is to prevent the sign extension messing the
		// function up.
		int z = map_element_height(x & 0xFFFF, y & 0xFFFF);
		int _2d_x, _2d_y;
		center_2d_coordinates(x, y, z, &_2d_x, &_2d_y, viewport);

		if (window->saved_view_x > 0){
			_2d_x = min(_2d_x, window->saved_view_x);
		}
		else{
			_2d_x = max(_2d_x, window->saved_view_x);
		}

		if (window->saved_view_y > 0){
			_2d_y = min(_2d_y, window->saved_view_y);
		}
		else{
			_2d_y = max(_2d_y, window->saved_view_y);
		}

		window->saved_view_x = _2d_x;
		window->saved_view_y = _2d_y;
	}

	x = window->saved_view_x;
	y = window->saved_view_y;
	if (window->flags & WF_SCROLLING_TO_LOCATION){
		// Moves the viewport if focusing in on an item
		uint8 flags = 0;
		x -= viewport->view_x;
		if (x < 0){
			x = -x;
			flags |= 1;
		}
		y -= viewport->view_y;
		if (y < 0){
			y = -y;
			flags |= 2;
		}
		x = (x + 7) / 8;
		y = (y + 7) / 8;

		//If we are at the final zoom position
		if (!x && !y){
			window->flags &= ~WF_SCROLLING_TO_LOCATION;
		}
		if (flags & 1){
			x = -x;
		}
		if (flags & 2){
			y = -y;
		}
		x += viewport->view_x;
		y += viewport->view_y;
	}

	sub_6E7DE1(x, y, window, viewport);
	//RCT2_CALLPROC_X(0x6E7DE1, x, y, 0, 0, (int)window, (int)viewport, 0);
}

/**
 *
 *  rct2: 0x00685C02
 *  ax: left
 *  bx: top
 *  dx: right
 *  esi: viewport
 *  edi: dpi
 *  ebp: bottom
 */
void viewport_render(rct_drawpixelinfo *dpi, rct_viewport *viewport, int left, int top, int right, int bottom)
{
	if (right <= viewport->x) return;
	if (bottom <= viewport->y) return;
	if (left >= viewport->x + viewport->width)return;
	if (top >= viewport->y + viewport->height)return;

	int l = left, t = top, r = right, b = bottom;

	left = max(left - viewport->x, 0);
	right = min(right - viewport->x, viewport->width);
	top = max(top - viewport->y, 0);
	bottom = min(bottom - viewport->y, viewport->height);

	left <<= viewport->zoom;
	right <<= viewport->zoom;
	top <<= viewport->zoom;
	bottom <<= viewport->zoom;

	left += viewport->view_x;
	right += viewport->view_x;
	top += viewport->view_y;
	bottom += viewport->view_y;

	int height = bottom - top;
	if (height > 384){
		//Paint
		viewport_paint(viewport, dpi, left, top, right, top + 384);
		top += 384;
	}
	//Paint
	viewport_paint(viewport, dpi, left, top, right, bottom);

#ifdef DEBUG_SHOW_DIRTY_BOX
	if (viewport != g_viewport_list){
		gfx_fill_rect_inset(dpi, l, t, r-1, b-1, 0x2, 0x30);
		return;
	}
#endif
}

/**
*
*  rct2: 0x0068615B
*  ebp: ebp
*/
void sub_0x68615B(int ebp){
	RCT2_GLOBAL(0xEE7888, uint32) = ebp;
	RCT2_GLOBAL(0xF1AD28, uint32) = 0;
	RCT2_GLOBAL(0xF1AD2C, uint32) = 0;
	uint8* edi = RCT2_ADDRESS(0xF1A50C, uint8);
	memset(edi, 0, 2048);
	RCT2_GLOBAL(0xF1AD0C, sint32) = -1;
	RCT2_GLOBAL(0xF1AD10, uint32) = 0;
	RCT2_GLOBAL(0xF1AD20, uint32) = 0;
	RCT2_GLOBAL(0xF1AD24, uint32) = 0;
}

/***
 *
 * rct2: 0x00688596
 * Part of 0x688485
 */
void paint_attached_ps(paint_struct* ps, paint_struct* attached_ps, rct_drawpixelinfo* dpi){
	for (; attached_ps; attached_ps = attached_ps->next_attached_ps){
		sint16 x = attached_ps->attached_x + ps->x;
		sint16 y = attached_ps->attached_y + ps->y;

		int image_id = attached_ps->image_id;
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & VIEWPORT_FLAG_SEETHROUGH_RIDES){
			if (ps->sprite_type == 3){
				if (image_id & 0x40000000){
					image_id &= 0x7FFFF;
					image_id |= 0x41880000;
				}
			}
		}

		if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & VIEWPORT_FLAG_SEETHROUGH_SCENERY){
			if (ps->sprite_type == 5){
				if (image_id & 0x40000000){
					image_id &= 0x7FFFF;
					image_id |= 0x41880000;
				}
			}
		}

		if (!(attached_ps->var_0C & 1)){
			gfx_draw_sprite(dpi, image_id, x, y, ps->var_04);
		}
		else{
			RCT2_CALLPROC_X(0x00681DE2, 0, image_id, x, y, 0, (int)dpi, attached_ps->var_04);
		}
	}
}

void sub_688485(){
	//RCT2_CALLPROC_EBPSAFE(0x688485);
	//return;
	rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
	paint_struct* ps = RCT2_GLOBAL(0xEE7884, paint_struct*);
	paint_struct* previous_ps = ps->var_24;

	for (ps = ps->var_24; ps;){
		sint16 x = ps->x;
		sint16 y = ps->y;
		if (ps->sprite_type == 2){
			if (dpi->zoom_level >= 1){
				x &= 0xFFFE;
				y &= 0xFFFE;
				if (dpi->zoom_level >= 2){
					x &= 0xFFFC;
					y &= 0xFFFC;
				}
			}
		}
		int image_id = ps->image_id;
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & VIEWPORT_FLAG_SEETHROUGH_RIDES){
			if (ps->sprite_type == 3){
				if (!(image_id & 0x40000000)){
					image_id &= 0x7FFFF;
					image_id |= 0x41880000;
				}
			}
		}
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & VIEWPORT_FLAG_UNDERGROUND_INSIDE){
			if (ps->sprite_type == 9){
				if (!(image_id & 0x40000000)){
					image_id &= 0x7FFFF;
					image_id |= 0x41880000;
				}
			}
		}
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & VIEWPORT_FLAG_SEETHROUGH_SCENERY){
			if (ps->sprite_type == 10 || ps->sprite_type == 12 || ps->sprite_type == 9 || ps->sprite_type == 5){
				if (!(image_id & 0x40000000)){
					image_id &= 0x7FFFF;
					image_id |= 0x41880000;
				}
			}
		}

		if (!(ps->var_1A & 1))
			gfx_draw_sprite(dpi, image_id, x, y, ps->var_04);
		else
			RCT2_CALLPROC_X(0x00681DE2, 0, image_id, x, y, 0, (int)dpi, ps->var_04);

		if (ps->var_20 != 0){
			ps = ps->var_20;
			continue;
		}

		paint_attached_ps(ps, ps->attached_ps, dpi);
		ps = previous_ps->var_24;
		previous_ps = ps;
	}

}

int sub_0x686806(rct_sprite* sprite, int eax, int image_id, int ecx, int edx){
	int ebp = (eax >> 8) & 0xFF;
	edx <<= 16;
	ebp += RCT2_GLOBAL(0x9DEA56, uint16);
	RCT2_GLOBAL(0xF1AD28, uint32) = 0;
	RCT2_GLOBAL(0xF1AD2C, uint32) = 0;
	edx = (edx >> 16) | (ebp << 16);

	//Not a paint struct but something similar
	paint_struct* ps = RCT2_GLOBAL(0xEE7888, paint_struct*);

	if ((uint32)ps >= RCT2_GLOBAL(0xEE7880, uint32)) return 1;

	ps->image_id = image_id;

	rct_g1_element *g1Element = &g1Elements[image_id & 0x7FFFF];

	eax = (eax & 0xFF) + RCT2_GLOBAL(0x9DE568, uint16);
	ecx = (ecx & 0xFF) + RCT2_GLOBAL(0x9DE56C, uint16);

	int x = ecx - eax;
	int y = (ecx + eax) / 2 - (edx & 0xFFFF);

	ps->x = x;
	ps->y = y;

	int left = x + g1Element->x_offset;
	int bottom = y + g1Element->y_offset;

	int right = left + g1Element->width;
	int top = bottom + g1Element->height;

	RCT2_GLOBAL(0xF1AD1C, uint16) = left;
	RCT2_GLOBAL(0xF1AD1E, uint16) = bottom;

	rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);

	if (right <= dpi->x)return 1;
	if (top <= dpi->y)return 1;
	if (left > dpi->x + dpi->width) return 1;
	if (bottom > dpi->y + dpi->height) return 1;

	RCT2_GLOBAL(0x9DE568, uint16);
	//686918 not finished

	return 0;
}


/**
*  Litter Paint Setup??
*  rct2: 0x006736FC
*/
void sub_0x6736FC(rct_litter* litter, int ebx, int edx){
	rct_drawpixelinfo* dpi;

	dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
	if (dpi->zoom_level != 0)return; //If zoomed at all no litter drawn

	int ebp = litter->var_01;
	//push litter
	ebx >>= 3;
	ebx &= RCT2_ADDRESS(0x97EF6C, uint32)[ebp * 2 + 1];
	ebx += RCT2_ADDRESS(0x97EF6C, uint32)[ebp * 2];
	int ecx = 0;
	int edi = 4;
	int esi = 4;
	int eax = 0xFF00;
	RCT2_GLOBAL(0x9DEA52, uint16) = 0xFFFC;
	RCT2_GLOBAL(0x9DEA54, uint16) = 0xFFFC;
	RCT2_GLOBAL(0x9DEA56, uint16) = edx + 2;

	switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)){
	case 0:
		//0x686806
		break;
	case 1:
		//0x6869b2
		break;
	case 2:
		//0x686b6f
		break;
	case 3:
		//0x686d31
		break;
	}
}


/**
*  Paint Quadrant
*  rct2: 0x0069E8B0
*/
void sub_0x69E8B0(uint32 eax, uint32 ecx){
	uint32 _eax = eax, _ecx = ecx;
	rct_drawpixelinfo* dpi;


	if (RCT2_GLOBAL(0x9DEA6F, uint8) & 1) return;

	dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & 0x4000)return;

	if (dpi->zoom_level > 2) return;

	if (eax > 0x2000)return;
	if (ecx > 0x2000)return;

	//push eax, ecx
	eax = (eax & 0x1FE0) << 3 | (ecx >> 5);
	int sprite_idx = RCT2_ADDRESS(0xF1EF60, uint16)[eax];
	if (sprite_idx == SPRITE_INDEX_NULL) return;
	
	for (rct_sprite* spr = &g_sprite_list[sprite_idx]; sprite_idx != SPRITE_INDEX_NULL; sprite_idx = spr->unknown.next_in_quadrant){
		spr = &g_sprite_list[sprite_idx];
		dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);

		if (dpi->y + dpi->height <= spr->unknown.sprite_top) continue;
		if (spr->unknown.sprite_bottom <= dpi->y)continue;
		if (dpi->x + dpi->width <= spr->unknown.sprite_left)continue;
		if (spr->unknown.sprite_right <= dpi->x)continue;

		int ebx = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32);
		RCT2_GLOBAL(0x9DE578, uint32) = (uint32)spr;
		int ebp = spr->unknown.sprite_identifier;
		ebx <<= 3;
		eax = spr->unknown.x;
		ebx += spr->unknown.sprite_direction;
		ecx = spr->unknown.y;
		ebx &= 0x1F;
		RCT2_GLOBAL(0x9DE568, uint16) = spr->unknown.x;
		RCT2_GLOBAL(0x9DE570, uint8) = 2;
		RCT2_GLOBAL(0x9DE56C, uint16) = spr->unknown.y;
		int edx = spr->unknown.z;
		switch (spr->unknown.sprite_identifier){
		case SPRITE_IDENTIFIER_VEHICLE:
			RCT2_CALLPROC_X(0x6D4244, eax, ebx, ecx, edx, (int)spr, (int)dpi, ebp);
			break;
		case SPRITE_IDENTIFIER_PEEP:
			RCT2_CALLPROC_X(0x68F0FB, eax, ebx, ecx, edx, (int)spr, (int)dpi, ebp);
			break;
		case SPRITE_IDENTIFIER_MISC:
			RCT2_CALLPROC_X(0x672AC9, eax, ebx, ecx, edx, (int)spr, (int)dpi, ebp);
			break;
		case SPRITE_IDENTIFIER_LITTER:
			RCT2_CALLPROC_X(0x6736FC, eax, ebx, ecx, edx, (int)spr, (int)dpi, ebp);
			break;
			//I am pretty sure there are no other sprite identifier types
		}
		//RCT2_CALLPROC_X(RCT2_ADDRESS(0x98BC40,uint32)[spr->unknown.sprite_identifier], eax, ebx, ecx, edx, (int)spr, (int)dpi, ebp);
	}

	//RCT2_CALLPROC_X(0x69E8B0, _eax, 0, _ecx, 0, 0, 0, 0);
	//return;
}

/*rct2: 0x006C42D9*/
int sub_6C42D9(rct_string_id string_id, int scroll, int ebp)
{
	rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
	if (dpi->zoom_level != 0) return 0x626;
	RCT2_GLOBAL(0x9D7A80, uint32_t)++;
	uint32_t edx = 0xFFFFFFFF;
	for (int i = 0; i < 0x20; i++)
	{
		uint8_t* unknown_pointer = RCT2_ADDRESS(0x9C3840, uint8_t) + 0xA12 * i;
		if (edx >= *((uint32_t*)(unknown_pointer + 0x0E)))
		{
			edx = *((uint32_t*)(unknown_pointer + 0x0E));
			RCT2_GLOBAL(0x9D7A84, uint32_t) = i;
			RCT2_GLOBAL(0x9D7A88, uint32_t) = (uint32_t)unknown_pointer;
		}
		if (*((rct_string_id*)unknown_pointer) == string_id &&
			*((uint32_t*)(unknown_pointer + 0x02)) == RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint32_t) &&
			*((uint32_t*)(unknown_pointer + 0x06)) == RCT2_GLOBAL(0x13CE956, uint32_t) &&
			*((uint16_t*)(unknown_pointer + 0x0A)) == scroll &&
			*((uint16_t*)(unknown_pointer + 0x0C)) == ebp)
		{
			*((uint32_t*)(unknown_pointer + 0x0E)) = RCT2_GLOBAL(0x9D7A80, uint32_t);
			return i + 0x606;
		}
	}
	uint8_t* unknown_pointer = RCT2_GLOBAL(0x9D7A88, uint8_t*);
	*((rct_string_id*)unknown_pointer) = string_id;
	*((uint32_t*)(unknown_pointer + 0x02)) = RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint32_t);
	*((uint32_t*)(unknown_pointer + 0x06)) = RCT2_GLOBAL(0x13CE956, uint32_t);
	*((uint16_t*)(unknown_pointer + 0x0A)) = scroll;
	*((uint16_t*)(unknown_pointer + 0x0C)) = ebp;
	*((uint32_t*)(unknown_pointer + 0x0E)) = RCT2_GLOBAL(0x9D7A80, uint32_t);
	unknown_pointer += 0x12;
	memset(unknown_pointer, 0, 0x280 * 4);
	format_string(RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char), string_id, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);
	int al = RCT2_GLOBAL(0x13CE959, uint8_t);
	int edi = al & 0x7F;
	int offs = 0;
	if (al >= 0x80) offs = 2;
	RCT2_GLOBAL(0x9D7A8C, uint8_t) = RCT2_ADDRESS(0x0141FC47, uint8_t)[offs + edi * 8];
	int16_t* unk = RCT2_ADDRESS(0x992FB8, uint16_t*)[ebp];
	uint8_t* format_result = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, uint8_t);
	while (true)
	{
		al = *format_result;
		format_result++;
		if (al == 0)
		{
			format_result = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
			continue;
		}
		if (al <= FORMAT_COLOUR_CODE_END && al >= FORMAT_COLOUR_CODE_START)
		{
			al -= FORMAT_COLOUR_CODE_START;
			RCT2_GLOBAL(0x9D7A8C, uint8_t) = RCT2_ADDRESS(RCT2_GLOBAL(0x9FF048, uint32_t), uint8_t)[al * 4];
			continue;
		}
		if (al < 0x20) continue;
		al -= 0x20;
		int edx = RCT2_ADDRESS(0x141EBA8, uint8_t)[al];
		uint8_t* unk2 = &(RCT2_ADDRESS(0xF4393C, uint8)[al * 8]);
		while (true)
		{
			if (scroll != 0)
			{
				scroll--;
				unk2++;
				edx--;
				if (edx == 0) break;
			}
			else
			{
				int16_t eax = *unk;
				if (eax == -1) return RCT2_GLOBAL(0x9D7A84, uint32_t) + 0x606;
				if (eax > -1)
				{
					uint8_t* dst = &unknown_pointer[eax];
					int ah = *unk2;
					int al = RCT2_GLOBAL(0x9D7A8C, uint8_t);
					while (true)
					{
						if (ah & 1) *dst = al;
						ah >>= 1;
						dst += 0x40;
						if (ah == 0) break;
					}
				}
				unk2++;
				unk++;
				edx--;
				if (edx == 0)  break;
			}
		}
	}
}

/* rct2: 0x006B9CC4 */
void viewport_banner_paint_setup(uint32_t direction, int edx, rct_map_element* map_element)
{
	rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
	RCT2_GLOBAL(0x9DE570, uint8_t) = 0xC;
	if (dpi->zoom_level > 1 || RCT2_GLOBAL(0x9DEA6F, uint8_t) & 1) return;
	edx -= 16;
	rct_scenery_entry* banner_scenery = g_bannerSceneryEntries[gBanners[map_element->properties.banner.index].type];
	direction += map_element->properties.banner.position;
	direction &= 3;
	RCT2_GLOBAL(0x9DEA56, uint16_t) = edx + 2;
	RCT2_GLOBAL(0x9DEA52, uint32_t) = RCT2_ADDRESS(0x98D884, uint32_t)[direction * 2];
	int ebx = (direction << 1) + banner_scenery->image;
	ebx += (gBanners[map_element->properties.banner.index].colour << 19) | 0x20000000;
	if (map_element->flags & 0x10)//if being placed (ghost appearance)
	{
		RCT2_GLOBAL(0x9DE570, uint8_t) = 0;
		ebx &= 0x7FFFF;
		ebx |= RCT2_ADDRESS(0x993CC4, uint32_t)[RCT2_GLOBAL(0x9AACBF, uint8_t)];
	}
	RCT2_CALLPROC_X(RCT2_ADDRESS(0x98197C, uint32_t)[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t)],
		0x1500, ebx, 0, edx, 1, 1, RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t));
	RCT2_GLOBAL(0x9DEA52, uint32_t) = RCT2_ADDRESS(0x98D888, uint32_t)[direction * 2];
	ebx++;
	RCT2_CALLPROC_X(RCT2_ADDRESS(0x98197C, uint32_t)[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t)],
		0x1500, ebx, 0, edx, 1, 1, RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t));
	direction ^= 2;
	direction--;
	if (direction >= 2 || (map_element->flags & 0x10)) return;
	int ebp = banner_scenery->banner.var_06;
	ebp += direction;
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint32_t) = 0;
	RCT2_GLOBAL(0x13CE956, uint32_t) = 0;
	rct_string_id string_id = 0xBA5;//no entry
	if (!(gBanners[map_element->properties.banner.index].flags & BANNER_FLAG_NO_ENTRY))
	{
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16_t) = gBanners[map_element->properties.banner.index].string_idx;
		string_id = 0x6C3;
	}
	format_string(RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char), string_id, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16_t) = 0x1C0;
	uint16_t string_width = gfx_get_string_width(RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char));
	uint16_t scroll = (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32_t) >> 1) % string_width;
	RCT2_CALLPROC_X(RCT2_ADDRESS(0x98199C, uint32_t)[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t)],
		0x1500, sub_6C42D9(string_id, scroll, ebp), 0, edx + 22, 1, 1, 0);
}

/*rct2: 0x0068B35F*/
void sub_68B35F(int ax, int cx)
{
	if (ax < RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16_t) &&
		cx < RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16_t) &&
		ax >= 32 && cx >= 32)
	{
		RCT2_GLOBAL(0x141E9B4, uint32_t) = 0xFFFF;
		RCT2_GLOBAL(0x141E9B8, uint32_t) = 0xFFFF;
		RCT2_GLOBAL(0x141E9BC, uint32_t) = 0xFFFF;
		RCT2_GLOBAL(0x141E9C0, uint32_t) = 0xFFFF;
		RCT2_GLOBAL(0x141E9C4, uint32_t) = 0xFFFF;
		RCT2_GLOBAL(0x141E9C8, uint32_t) = 0xFFFF;
		RCT2_GLOBAL(0x141E9CC, uint32_t) = 0xFFFF;
		RCT2_GLOBAL(0x141E9D0, uint32_t) = 0xFFFF;
		RCT2_GLOBAL(0x141E9D4, uint32_t) = 0xFFFF;
		RCT2_GLOBAL(0x141E9D8, uint32_t) = 0xFFFF;
		RCT2_GLOBAL(0x141E9DC, uint32_t) = 0xFFFF;
		//loc_68B3FB: Another function jumps to here. We need to split this!
		RCT2_GLOBAL(0x141F56A, uint16_t) = 0;
		RCT2_GLOBAL(0x9E3138, uint8_t) = 0xFF;
		RCT2_GLOBAL(0x9E30B6, uint8_t) = 0xFF;
		RCT2_GLOBAL(0x9E323C, uint8_t) = 0xFF;
		RCT2_GLOBAL(0x9DE56A, uint16_t) = ax;
		RCT2_GLOBAL(0x9DE56E, uint16_t) = cx;
		RCT2_GLOBAL(0x9DE574, uint16_t) = ax;
		RCT2_GLOBAL(0x9DE576, uint16_t) = cx;
		int dx = cx;
		int esi = dx;
		esi <<= 8;
		esi |= ax;
		esi >>= 3;
		int ax_tmp = ax;
		int cx_tmp = cx;
		rct_map_element* map_element = TILE_MAP_ELEMENT_POINTER(esi / 4);
		rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
		switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t))
		{
		case 0:
			dx = ax + cx;
			break;
		case 1:
			ax += 32;
			dx = cx - ax;
			break;
		case 2:
			ax += 32;
			cx += 32;
			dx = -(ax + cx);
			break;
		case 3:
			cx += 32;
			dx = ax - cx;
			break;
		}
		dx /= 2;
		// Display little yellow arrow when building footpaths?
		if ((RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16_t) & 4) &&
			RCT2_GLOBAL(0x9DE56A, uint16_t) == RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_X, uint16_t) &&
			RCT2_GLOBAL(0x9DE56E, uint16_t) == RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Y, uint16_t))
		{

			int ebx = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t);
			RCT2_GLOBAL(0x9DE568, uint16_t) = ax;
			RCT2_GLOBAL(0x9DE56C, uint16_t) = cx;
			int dl = RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8_t) & 3;
			ebx += dl;
			ebx &= 3;
			dl = RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8_t) & 0xFC;
			ebx += dl;
			ebx += 0x20900C27;
			int d = RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, uint16_t);
			RCT2_GLOBAL(0x9DE570, uint8_t) = 0;
			RCT2_GLOBAL(0x9DEA52, uint16_t) = 0;
			RCT2_GLOBAL(0x9DEA54, uint16_t) = 0;
			RCT2_GLOBAL(0x9DEA56, uint16_t) = d + 18;
			RCT2_CALLPROC_X(
				(int)RCT2_ADDRESS(0x0098197C, uint32_t*)[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t)],
				0xFF00, ebx, cx & 0xFF00, d, 32, 32, RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t));
		}
		int bx = dx + 52;
		if (bx > dpi->y)
		{
			rct_map_element* element = map_element;//push map_element
			bx = element->clearance_height;
			if (!map_element_is_last_for_tile(element))
			{
				while (true)
				{
					element++;
					bx = max(bx, element->clearance_height);
					if (map_element_is_last_for_tile(element)) break;
				}
			}
			if (map_element_get_type(element) == MAP_ELEMENT_TYPE_SURFACE &&
				(element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK) != 0)
			{
				bx = (element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK) << 1;
			}
			bx <<= 3;
			dx -= bx;
			dx -= 32;
			element = map_element;//pop map_element
			dx -= dpi->height;
			if (dx < dpi->y)
			{
				RCT2_GLOBAL(0x9DE568, uint16_t) = ax;
				RCT2_GLOBAL(0x9DE56C, uint16_t) = cx;
				RCT2_GLOBAL(0x9DE57C, uint16_t) = 0;
				while (true)
				{
					int direction = (map_element->type + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t)) & MAP_ELEMENT_DIRECTION_MASK;
					dx = map_element->base_height * 8;
					uint32_t dword_9DE574 = RCT2_GLOBAL(0x9DE574, uint32_t);
					RCT2_GLOBAL(0x9DE578, rct_map_element*) = map_element;
					//setup the painting of for example: the underground, signs, rides, scenery, etc.
					switch (map_element_get_type(map_element))
					{
					case MAP_ELEMENT_TYPE_SURFACE:
						RCT2_CALLPROC_X(0x66062C, 0, 0, direction, dx, (int)map_element, 0, 0);
						break;
					case MAP_ELEMENT_TYPE_PATH:
						RCT2_CALLPROC_X(0x6A3590, 0, 0, direction, dx, (int)map_element, 0, 0);
						break;
					case MAP_ELEMENT_TYPE_TRACK:
						RCT2_CALLPROC_X(0x6C4794, 0, 0, direction, dx, (int)map_element, 0, 0);
						break;
					case MAP_ELEMENT_TYPE_SCENERY:
						RCT2_CALLPROC_X(0x6DFF47, 0, 0, direction, dx, (int)map_element, 0, 0);
						break;
					case MAP_ELEMENT_TYPE_ENTRANCE:
						RCT2_CALLPROC_X(0x664FD4, 0, 0, direction, dx, (int)map_element, 0, 0);
						break;
					case MAP_ELEMENT_TYPE_FENCE:
						RCT2_CALLPROC_X(0x6E44B0, 0, 0, direction, dx, (int)map_element, 0, 0);
						break;
					case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
						RCT2_CALLPROC_X(0x6B7F0C, 0, 0, direction, dx, (int)map_element, 0, 0);
						break;
					case MAP_ELEMENT_TYPE_BANNER:
						//there are still some small localisation glitches,
						//because the old function still gets called sometimes
						//viewport_banner_paint_setup(direction, dx, map_element);
						//Until that is solved, use the original function instead
						RCT2_CALLPROC_X(0x6B9CC4, 0, 0, direction, dx, (int)map_element, 0, 0);
						break;
					default:
						break;
					}
					RCT2_GLOBAL(0x9DE574, uint32_t) = dword_9DE574;
					int stop = map_element_is_last_for_tile(map_element);
					map_element++;
					if (stop) break;
				}
			}
		}
	}
	else
	{
		rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
		int dx;
		switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t))
		{
		case 0:
			dx = ax + cx;
			break;
		case 1:
			ax += 32;
			dx = cx - ax;
			break;
		case 2:
			ax += 32;
			cx += 32;
			dx = -(ax + cx);
			break;
		case 3:
			cx += 32;
			dx = ax - cx;
			break;
		}
		dx /= 2;
		dx -= 16;
		int bx = dx + 32;
		if (bx <= dpi->y) return;
		dx -= 20;
		dx -= dpi->height;
		if (dx >= dpi->y) return;
		RCT2_GLOBAL(0x9DE568, uint16_t) = ax;
		RCT2_GLOBAL(0x9DE56C, uint16_t) = cx;
		RCT2_GLOBAL(0x9DE570, uint8_t) = 0;
		RCT2_CALLPROC_X((int)RCT2_ADDRESS(0x98196C, uint32_t*)[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t)],
			0xFF00, 3123, cx & 0xFF00, 16, 32, 32, RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t));
	}
}

/**
*
*  rct2: 0x0068B6C2
*/
void sub_0x68B6C2(){
	rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
	sint16 ax, bx, cx, dx;
	switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)){
	case 0:
		ax = dpi->y;
		bx = dpi->x;

		ax -= 16;
		bx &= 0xFFE0;
		ax &= 0xFFE0;
		bx >>= 1;
		cx = ax;
		ax -= bx;
		cx += bx;
		ax &= 0xFFE0;
		cx &= 0xFFE0;
		dx = dpi->height;
		dx += 2128;
		dx >>= 5;
		for (int i = dx; i > 0; --i){
			sub_68B35F(ax, cx);
			sub_0x69E8B0(ax, cx);
			cx += 0x20;
			ax -= 0x20;
			sub_0x69E8B0(ax, cx);
			ax += 0x20;
			sub_68B35F(ax, cx);
			sub_0x69E8B0(ax, cx);
			ax += 0x20;
			cx -= 0x20;
			sub_0x69E8B0(ax, cx);
			cx += 0x20;
		}
		break;
	case 1:
		ax = dpi->y;
		bx = dpi->x;
		ax -= 0x10;
		bx &= 0xFFE0;
		ax &= 0xFFE0;
		bx >>= 1;
		cx = ax;
		ax = -ax;
		ax -= bx;
		cx -= bx;
		cx -= 0x10;
		ax &= 0xFFE0;
		cx &= 0xFFE0;
		dx = dpi->height;
		dx += 0x860;
		dx >>= 5;
		for (int i = dx; i > 0; i--){
			sub_68B35F(ax, cx);
			sub_0x69E8B0(ax, cx);
			ax -= 0x20;
			cx -= 0x20;
			sub_0x69E8B0(ax, cx);
			cx += 0x20;
			sub_68B35F(ax, cx);
			sub_0x69E8B0(ax, cx);
			ax += 0x20;
			cx += 0x20;
			sub_0x69E8B0(ax, cx);
			ax -= 0x20;
		}
		break;
	case 2:
		ax = dpi->y;
		bx = dpi->x;
		ax -= 0x10;
		bx &= 0xFFE0;
		ax &= 0xFFE0;
		bx >>= 1;
		ax = -ax;
		cx = ax;
		ax += bx;
		cx -= bx;
		ax &= 0xFFE0;
		cx &= 0xFFE0;
		dx = dpi->height;
		dx += 0x860;
		dx >>= 5;
		for (int i = dx; i > 0; i--){
			sub_68B35F(ax, cx);
			sub_0x69E8B0(ax, cx);
			ax += 0x20;
			cx -= 0x20;
			sub_0x69E8B0(ax, cx);
			ax -= 0x20;
			sub_68B35F(ax, cx);
			sub_0x69E8B0(ax, cx);
			ax -= 0x20;
			cx += 0x20;
			sub_0x69E8B0(ax, cx);
			cx -= 0x20;
		}
		break;
	case 3:
		ax = dpi->y;
		bx = dpi->x;
		ax -= 0x10;
		bx &= 0xFFE0;
		ax &= 0xFFE0;
		bx >>= 1;
		cx = ax;
		ax += bx;
		cx = -cx;
		cx += bx;
		cx -= 0x10;
		ax &= 0xFFE0;
		cx &= 0xFFE0;
		dx = dpi->height;
		dx += 0x860;
		dx >>= 5;
		for (int i = dx; i > 0; i--){
			sub_68B35F(ax, cx);
			sub_0x69E8B0(ax, cx);
			ax += 0x20;
			cx += 0x20;
			sub_0x69E8B0(ax, cx);
			cx -= 0x20;
			sub_68B35F(ax, cx);
			sub_0x69E8B0(ax, cx);
			ax -= 0x20;
			cx -= 0x20;
			sub_0x69E8B0(ax, cx);
			ax += 0x20;
		}
		break;
	}
}

void sub_688217_helper(uint16 ax, uint8 flag)
{
	paint_struct *ps;
	paint_struct *ps_next = RCT2_GLOBAL(0x00EE7884, paint_struct*);

	do {
		ps = ps_next;
		ps_next = ps_next->var_24;
		if (ps_next == NULL) return;
	} while (ax > ps_next->var_18);

	RCT2_GLOBAL(0x00F1AD14, paint_struct*) = ps;

	do {
		ps = ps->var_24;
		if (ps == NULL) break;

		if (ps->var_18 > ax + 1) {
			ps->var_1B = 1 << 7;
		} else if (ps->var_18 == ax + 1) {
			ps->var_1B = (1 << 1) | (1 << 0);
		} else if (ps->var_18 == ax) {
			ps->var_1B = flag | (1 << 0);
		}
	} while (ps->var_18 <= ax + 1);

	ps = RCT2_GLOBAL(0x00F1AD14, paint_struct*);

	while (true) {
		while (true) {
			ps_next = ps->var_24;
			if (ps_next == NULL) return;
			if (ps_next->var_1B & (1 << 7)) return;
			if (ps_next->var_1B & (1 << 0)) break;
			ps = ps_next;
		}

		ps_next->var_1B &= ~(1 << 0);
		RCT2_GLOBAL(0x00F1AD18, paint_struct*) = ps;

		uint16 my_attached_x = ps_next->attached_x;
		uint16 my_attached_y = ps_next->attached_y;
		uint16 my_some_x = ps_next->some_x;
		uint16 my_some_y = ps_next->some_y;
		uint16 my_other_x = ps_next->other_x;
		uint16 my_other_y = ps_next->other_y;

		while (true) {
			ps = ps_next;
			ps_next = ps_next->var_24;
			if (ps_next == NULL) break;
			if (ps_next->var_1B & (1 << 7)) break;
			if (!(ps_next->var_1B & (1 << 1))) continue;

			int yes = 0;
			switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)) {
			case 0:
				if (my_some_y >= ps_next->some_x && my_other_y >= ps_next->attached_y && my_other_x >= ps_next->attached_x
					&& !(my_some_x < ps_next->some_y && my_attached_y < ps_next->other_y && my_attached_x < ps_next->other_x))
					yes = 1;
				break;
			case 1:
				if (my_some_y >= ps_next->some_x && my_other_y >= ps_next->attached_y && my_other_x < ps_next->attached_x
					&& !(my_some_x < ps_next->some_y && my_attached_y < ps_next->other_y && my_attached_x >= ps_next->other_x))
					yes = 1;
				break;
			case 2:
				if (my_some_y >= ps_next->some_x && my_other_y < ps_next->attached_y && my_other_x < ps_next->attached_x
					&& !(my_some_x < ps_next->some_y && my_attached_y >= ps_next->other_y && my_attached_x >= ps_next->other_x))
					yes = 1;
				break;
			case 3:
				if (my_some_y >= ps_next->some_x && my_other_y < ps_next->attached_y && my_other_x >= ps_next->attached_x
					&& !(my_some_x < ps_next->some_y && my_attached_y >= ps_next->other_y && my_attached_x < ps_next->other_x))
					yes = 1;
				break;
			}

			if (yes) {
				ps->var_24 = ps_next->var_24;
				paint_struct *ps_temp = RCT2_GLOBAL(0x00F1AD18, paint_struct*)->var_24;
				RCT2_GLOBAL(0x00F1AD18, paint_struct*)->var_24 = ps_next;
				ps_next->var_24 = ps_temp;
				ps_next = ps;
			}
		}

		ps = RCT2_GLOBAL(0x00F1AD18, paint_struct*);
	}
}

/**
*
*  rct2: 0x00688217
*/
void sub_688217()
{
	paint_struct *ps = RCT2_GLOBAL(0x00EE7888, paint_struct*);
	paint_struct *ps_next;
	RCT2_GLOBAL(0x00EE7888, uint32) += 0x34; // 0x34 is size of paint_struct?
	RCT2_GLOBAL(0x00EE7884, paint_struct*) = ps;
	ps->var_24 = NULL;
	uint32 edi = RCT2_GLOBAL(0x00F1AD0C, uint32);
	if (edi == -1)
		return;

	do {
		ps_next = RCT2_GLOBAL(0x00F1A50C + 4 * edi, paint_struct*);
		if (ps_next != NULL) {
			ps->var_24 = ps_next;
			do {
				ps = ps_next;
				ps_next = ps_next->var_24;
			} while (ps_next != NULL);
		}
	} while (++edi <= RCT2_GLOBAL(0x00F1AD10, uint32));

	uint32 eax = RCT2_GLOBAL(0x00F1AD0C, uint32);

	sub_688217_helper(eax & 0xFFFF, 1 << 1);

	eax = RCT2_GLOBAL(0x00F1AD0C, uint32);

	while (++eax < RCT2_GLOBAL(0x00F1AD10, uint32))
		sub_688217_helper(eax & 0xFFFF, 0);
}

/**
 *
 *  rct2:0x00685CBF
 *  eax: left
 *  ebx: top
 *  edx: right
 *  esi: viewport
 *  edi: dpi
 *  ebp: bottom
 */
void viewport_paint(rct_viewport* viewport, rct_drawpixelinfo* dpi, int left, int top, int right, int bottom){
	//RCT2_CALLPROC_X(0x00685CBF, left, top, 0, right, (int)viewport, (int)dpi, bottom);
	//return;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) = viewport->flags;
	RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_ZOOM, uint16) = viewport->zoom;

	uint16 width = right - left;
	uint16 height = bottom - top;
	uint16 bitmask = 0xFFFF & (0xFFFF << viewport->zoom);

	width &= bitmask;
	height &= bitmask;
	left &= bitmask;
	top &= bitmask;

	RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, uint16) = left;
	RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_Y, uint16) = top;
	RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_WIDTH, uint16) = width;
	RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_HEIGHT, uint16) = height;

	width >>= viewport->zoom;

	RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_PITCH, uint16) = (dpi->width + dpi->pitch) - width;

	int x = (sint16)(left - (sint16)(viewport->view_x & bitmask));
	x >>= viewport->zoom;
	x += viewport->x;

	int y = (sint16)(top - (sint16)(viewport->view_y & bitmask));
	y >>= viewport->zoom;
	y += viewport->y;

	uint8* bits_pointer = x - dpi->x + (y - dpi->y)*(dpi->width + dpi->pitch) + dpi->bits;
	RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_BITS_PTR, uint8*) = bits_pointer;

	rct_drawpixelinfo* dpi2 = RCT2_ADDRESS(RCT2_ADDRESS_VIEWPORT_DPI, rct_drawpixelinfo);
	dpi2->y = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_Y, uint16);
	dpi2->height = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_HEIGHT, uint16);
	dpi2->zoom_level = (uint8)RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_ZOOM, uint16);

	//Splits the screen into 32 pixel columns and renders them.
	for (x = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, uint16) & 0xFFFFFFE0;
		x < RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, uint16) + RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_WIDTH, uint16);
		x += 32){

		int start_x = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, uint16);
		int width_col = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_WIDTH, uint16);
		bits_pointer = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_BITS_PTR, uint8*);
		int pitch = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_PITCH, uint16);
		int zoom = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_ZOOM, uint16);
		if (x >= start_x){
			int left_pitch = x - start_x;
			width_col -= left_pitch;
			bits_pointer += left_pitch >> zoom;
			pitch += left_pitch >> zoom;
			start_x = x;
		}

		int paint_right = start_x + width_col;
		if (paint_right >= x + 32){
			int right_pitch = paint_right - x - 32;
			paint_right -= right_pitch;
			pitch += right_pitch >> zoom;
		}
		width_col = paint_right - start_x;
		dpi2->x = start_x;
		dpi2->width = width_col;
		dpi2->bits = bits_pointer;
		dpi2->pitch = pitch;

		if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & 0x3001){
			uint8 colour = 0x0A;
			if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & 0x4000){
				colour = 0;
			}
			gfx_clear(dpi2, colour);
		}
		RCT2_GLOBAL(0xEE7880, uint32) = 0xF1A4CC;
		RCT2_GLOBAL(0x140E9A8, uint32) = (int)dpi2;
		int ebp = 0, ebx = 0, esi = 0, ecx = 0;
		sub_0x68615B(0xEE788C); //Memory copy
		sub_0x68B6C2();
		//RCT2_CALLPROC_X(0x68B6C2, 0, 0, 0, 0, 0, 0, 0); //Big function call 4 rotation versions
		sub_688217();
		//RCT2_CALLPROC_X(0x688217, start_x, ebx, ecx, (int)bits_pointer, esi, (int)dpi2, ebp); //Move memory
		sub_688485();
		//RCT2_CALLPROC_EBPSAFE(0x688485); //Big function call

		int weather_colour = RCT2_ADDRESS(0x98195C, uint32)[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_GLOOM, uint8)];
		if ((weather_colour != -1) && (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & 0x4000)) && (!(RCT2_GLOBAL(0x9DEA6F, uint8) & 1))){
			gfx_fill_rect(dpi2, dpi2->x, dpi2->y, dpi2->width + dpi2->x - 1, dpi2->height + dpi2->y - 1, weather_colour);
		}
		RCT2_CALLPROC_EBPSAFE(0x6860C3); //string related
	}

	//RCT2_CALLPROC_X(0x00685CBF, left, top, 0, right, (int)viewport, (int)dpi, bottom);
}

/**
 *
 *  rct2: 0x00688972
 *  In:
 *		screen_x: eax
 *		screen_y: ebx
 *  Out:
 *		x: ax
 *		y: bx
 *		map_element: edx ?
 *		viewport: edi
 */
void sub_688972(int screenX, int screenY, sint16 *x, sint16 *y, rct_viewport **viewport) {
	sint16 my_x, my_y;
	int z, interactionType;
	rct_viewport *myViewport;
	get_map_coordinates_from_pos(screenX, screenY, VIEWPORT_INTERACTION_MASK_TERRAIN, &my_x, &my_y, &interactionType, NULL, &myViewport);
	if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE) {
		*x = 0x8000;
		return;
	}

	RCT2_GLOBAL(0x00F1AD34, sint16) = my_x;
	RCT2_GLOBAL(0x00F1AD36, sint16) = my_y;
	RCT2_GLOBAL(0x00F1AD38, sint16) = my_x + 31;
	RCT2_GLOBAL(0x00F1AD3A, sint16) = my_y + 31;

	rct_xy16 start_vp_pos = screen_coord_to_viewport_coord(myViewport, screenX, screenY);
	rct_xy16 map_pos = { my_x + 16, my_y + 16 };

	for (int i = 0; i < 5; i++) {
		z = map_element_height(map_pos.x, map_pos.y);
		map_pos = viewport_coord_to_map_coord(start_vp_pos.x, start_vp_pos.y, z);
		map_pos.x = clamp(RCT2_GLOBAL(0x00F1AD34, sint16), map_pos.x, RCT2_GLOBAL(0x00F1AD38, sint16));
		map_pos.y = clamp(RCT2_GLOBAL(0x00F1AD36, sint16), map_pos.y, RCT2_GLOBAL(0x00F1AD3A, sint16));
	}

	*x = map_pos.x;
	*y = map_pos.y;

	if (viewport != NULL) *viewport = myViewport;
}

/**
 *
 *  rct2: 0x0068958D
 */
void screen_pos_to_map_pos(sint16 *x, sint16 *y, int *direction)
{
	sub_688972(*x, *y, x, y, NULL);
	if (*x == 0x8000)
		return;

	int my_direction;
	int dist_from_center_x = abs(*x % 32);
	int dist_from_center_y = abs(*y % 32);
	if (dist_from_center_x > 8 && dist_from_center_x < 24 &&
		dist_from_center_y > 8 && dist_from_center_y < 24) {
		my_direction = 4;
	} else {
		sint16 mod_x = *x & 0x1F;
		sint16 mod_y = *y & 0x1F;
		if (mod_x <= 16) {
			if (mod_y < 16) {
				my_direction = 2;
			} else {
				my_direction = 3;
			}
		} else {
			if (mod_y < 16) {
				my_direction = 1;
			} else {
				my_direction = 0;
			}
		}
	}

	*x = *x & ~0x1F;
	*y = *y & ~0x1F;
	if (direction != NULL) *direction = my_direction;
}

rct_xy16 screen_coord_to_viewport_coord(rct_viewport *viewport, uint16 x, uint16 y)
{
	rct_xy16 ret;
	ret.x = ((x - viewport->x) << viewport->zoom) + viewport->view_x;
	ret.y = ((y - viewport->y) << viewport->zoom) + viewport->view_y;
	return ret;
}

rct_xy16 viewport_coord_to_map_coord(int x, int y, int z)
{
	rct_xy16 ret;
	switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)) {
	case 0:
		ret.x = -x / 2 + y + z;
		ret.y = x / 2 + y + z;
		break;
	case 1:
		ret.x = -x / 2 - y - z;
		ret.y = -x / 2 + y + z;
		break;
	case 2:
		ret.x = x / 2 - y - z;
		ret.y = -x / 2 - y - z;
		break;
	case 3:
		ret.x = x / 2 + y + z;
		ret.y = x / 2 - y - z;
		break;
	}
	return ret;
}

/**
 *
 *  rct2: 0x00664689
 */
void show_gridlines()
{
	rct_window *mainWindow;

	if (RCT2_GLOBAL(0x009E32B0, uint8) == 0) {
		if ((mainWindow = window_get_main()) != NULL) {
			if (!(mainWindow->viewport->flags & VIEWPORT_FLAG_GRIDLINES)) {
				mainWindow->viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
				window_invalidate(mainWindow);
			}
		}
	}
	RCT2_GLOBAL(0x009E32B0, uint8)++;
}

/**
 *
 *  rct2: 0x006646B4
 */
void hide_gridlines()
{
	rct_window *mainWindow;

	RCT2_GLOBAL(0x009E32B0, uint8)--;
	if (RCT2_GLOBAL(0x009E32B0, uint8) == 0) {
		if ((mainWindow = window_get_main()) != NULL) {
			if (!gConfigGeneral.always_show_gridlines) {
				mainWindow->viewport->flags &= ~VIEWPORT_FLAG_GRIDLINES;
				window_invalidate(mainWindow);
			}
		}
	}
}

/**
 *
 *  rct2: 0x00664E8E
 */
void show_land_rights()
{
	rct_window *mainWindow;

	if (RCT2_GLOBAL(0x009E32B2, uint8) == 0) {
		if ((mainWindow = window_get_main()) != NULL) {
			if (!(mainWindow->viewport->flags & VIEWPORT_FLAG_LAND_OWNERSHIP)) {
				mainWindow->viewport->flags |= VIEWPORT_FLAG_LAND_OWNERSHIP;
				window_invalidate(mainWindow);
			}
		}
	}
	RCT2_GLOBAL(0x009E32B2, uint8)++;
}

/**
 *
 *  rct2: 0x00664EB9
 */
void hide_land_rights()
{
	rct_window *mainWindow;

	RCT2_GLOBAL(0x009E32B2, uint8)--;
	if (RCT2_GLOBAL(0x009E32B2, uint8) == 0) {
		if ((mainWindow = window_get_main()) != NULL) {
			if (mainWindow->viewport->flags & VIEWPORT_FLAG_LAND_OWNERSHIP) {
				mainWindow->viewport->flags &= ~VIEWPORT_FLAG_LAND_OWNERSHIP;
				window_invalidate(mainWindow);
			}
		}
	}
}

/**
 *
 *  rct2: 0x00664EDD
 */
void show_construction_rights()
{
	rct_window *mainWindow;

	if (RCT2_GLOBAL(0x009E32B3, uint8) == 0) {
		if ((mainWindow = window_get_main()) != NULL) {
			if (!(mainWindow->viewport->flags & VIEWPORT_FLAG_CONSTRUCTION_RIGHTS)) {
				mainWindow->viewport->flags |= VIEWPORT_FLAG_CONSTRUCTION_RIGHTS;
				window_invalidate(mainWindow);
			}
		}
	}
	RCT2_GLOBAL(0x009E32B3, uint8)++;
}

/**
 *
 *  rct2: 0x00664F08
 */
void hide_construction_rights()
{
	rct_window *mainWindow;

	RCT2_GLOBAL(0x009E32B3, uint8)--;
	if (RCT2_GLOBAL(0x009E32B3, uint8) == 0) {
		if ((mainWindow = window_get_main()) != NULL) {
			if (mainWindow->viewport->flags & VIEWPORT_FLAG_CONSTRUCTION_RIGHTS) {
				mainWindow->viewport->flags &= ~VIEWPORT_FLAG_CONSTRUCTION_RIGHTS;
				window_invalidate(mainWindow);
			}
		}
	}
}

/**
 *
 * rct2: 0x006CB70A
 */
void viewport_set_visibility(uint8 mode)
{
	rct_window* window = window_get_main();

	if (window != NULL) {
		rct_viewport* edi = window->viewport;
		uint32 invalidate = 0;

		switch (mode) {
		case 0: { //Set all these flags to 0, and invalidate if any were active
			uint16 mask = VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_SEETHROUGH_RIDES |
				VIEWPORT_FLAG_SEETHROUGH_SCENERY | VIEWPORT_FLAG_INVISIBLE_SUPPORTS |
				VIEWPORT_FLAG_LAND_HEIGHTS | VIEWPORT_FLAG_TRACK_HEIGHTS |
				VIEWPORT_FLAG_PATH_HEIGHTS | VIEWPORT_FLAG_INVISIBLE_PEEPS |
				VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_VERTICAL;

			invalidate += edi->flags & mask;
			edi->flags &= ~mask;
			break;
		}
		case 1: //6CB79D
		case 4: //6CB7C4
			//Set underground on, invalidate if it was off
			invalidate += !(edi->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE);
			edi->flags |= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
			break;
		case 2: //6CB7EB
			//Set track heights on, invalidate if off
			invalidate += !(edi->flags & VIEWPORT_FLAG_TRACK_HEIGHTS);
			edi->flags |= VIEWPORT_FLAG_TRACK_HEIGHTS;
			break;
		case 3: //6CB7B1
		case 5: //6CB7D8
			//Set underground off, invalidate if it was on
			invalidate += edi->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE;
			edi->flags &= ~((uint16)VIEWPORT_FLAG_UNDERGROUND_INSIDE);
			break;
		}
		if (invalidate != 0)
			window_invalidate(window);
	}
}

/**
 * Stores some info about the element pointed at, if requested for this particular type through the interaction mask.
 * rct2: 0x00688697
 */
void store_interaction_info(paint_struct *ps)
{
	if (RCT2_GLOBAL(0x0141F569, uint8) == 0) return;

	if (ps->sprite_type == VIEWPORT_INTERACTION_ITEM_NONE
		|| ps->sprite_type == 11 // 11 as a type seems to not exist, maybe part of the typo mentioned later on.
		|| ps->sprite_type > VIEWPORT_INTERACTION_ITEM_BANNER) return;

	uint16 mask;
	if (ps->sprite_type == VIEWPORT_INTERACTION_ITEM_BANNER)
		// I think CS made a typo here. Let's replicate the original behaviour.
		mask = 1 << (ps->sprite_type - 3);
	else
		mask = 1 << (ps->sprite_type - 1);

	if (!(RCT2_GLOBAL(0x009AC154, uint16) & mask)) {
		RCT2_GLOBAL(0x009AC148, uint8) = ps->sprite_type;
		RCT2_GLOBAL(0x009AC149, uint8) = ps->var_29;
		RCT2_GLOBAL(0x009AC14C, uint32) = ps->map_x;
		RCT2_GLOBAL(0x009AC14E, uint32) = ps->map_y;
		RCT2_GLOBAL(0x009AC150, rct_map_element*) = ps->mapElement;
	}
}

/**
 * rct2: 0x0068862C
 */
void sub_68862C()
{
	rct_drawpixelinfo *dpi = RCT2_GLOBAL(0x0140E9A8, rct_drawpixelinfo*);
	paint_struct *ps = RCT2_GLOBAL(0x00EE7884, paint_struct*), *old_ps, *next_ps, *attached_ps;
	uint32 eax = 0xBBBBBBBB, ebx = 0xBBBBBBBB, ecx = 0xBBBBBBBB, edx = 0xBBBBBBBB, esi = 0xBBBBBBBB, edi = 0xBBBBBBBB, ebp = 0xBBBBBBBB;

	while ((ps = ps->var_24) != NULL) {
		old_ps = ps;

		next_ps = ps;
		while (next_ps != NULL) {
			ps = next_ps;
			ebx = ps->image_id;
			ecx = ps->x;
			edx = ps->y;
			edi = (uint32)dpi;
			ebp = (uint32)ps;
			//sub_679023(ps->image_id, ps->x, ps->y, dpi);
			RCT2_CALLFUNC_X(0x00679023, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
			store_interaction_info(ps);

			next_ps = ps->var_20;
		}

		attached_ps = ps->attached_ps;
		while (attached_ps != NULL) {
			esi = (uint32)attached_ps;
			ebp = (uint32)ps;
			ecx = (attached_ps->attached_x + ps->x) & 0xFFFF;
			edx = (attached_ps->attached_y + ps->y) & 0xFFFF;
			ebx = attached_ps->image_id;
			//sub_679023(ebx, ecx, edx, dpi);
			RCT2_CALLFUNC_X(0x00679023, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
			store_interaction_info(ps);

			attached_ps = attached_ps->next_attached_ps;
		}

		ps = old_ps;
	}
}

/**
 *
 *  rct2: 0x00685ADC
 * screenX: eax
 * screenY: ebx
 * flags: edx
 * x: ax
 * y: cx
 * z: bl
 * mapElement: edx
 * viewport: edi
 */
void get_map_coordinates_from_pos(int screenX, int screenY, int flags, sint16 *x, sint16 *y, int *interactionType, rct_map_element **mapElement, rct_viewport **viewport)
{
	RCT2_GLOBAL(0x9AC154, uint16_t) = flags & 0xFFFF;
	RCT2_GLOBAL(0x9AC148, uint8_t) = 0;
	rct_window* window = window_find_from_point(screenX, screenY);
	if (window != NULL && window->viewport != NULL)
	{
		rct_viewport* myviewport = window->viewport;
		RCT2_GLOBAL(0x9AC138 + 4, int16_t) = screenX;
		RCT2_GLOBAL(0x9AC138 + 6, int16_t) = screenY;
		screenX -= (int)myviewport->x;
		screenY -= (int)myviewport->y;
		if (screenX >= 0 && screenX < (int)myviewport->width && screenY >= 0 && screenY < (int)myviewport->height)
		{
			screenX <<= myviewport->zoom;
			screenY <<= myviewport->zoom;
			screenX += (int)myviewport->view_x;
			screenY += (int)myviewport->view_y;
			RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_ZOOM, uint16_t) = myviewport->zoom;
			screenX &= (0xFFFF << myviewport->zoom) & 0xFFFF;
			screenY &= (0xFFFF << myviewport->zoom) & 0xFFFF;
			RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, int16_t) = screenX;
			RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_Y, int16_t) = screenY;
			rct_drawpixelinfo* dpi = RCT2_ADDRESS(RCT2_ADDRESS_VIEWPORT_DPI, rct_drawpixelinfo);
			dpi->y = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_Y, int16_t);
			dpi->height = 1;
			dpi->zoom_level = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_ZOOM, uint16_t);
			dpi->x = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, int16_t);
			dpi->width = 1;
			RCT2_GLOBAL(0xEE7880, uint32_t) = 0xF1A4CC;
			RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*) = dpi;
			sub_0x68615B(0xEE788C);
			sub_0x68B6C2();
			sub_688217();
			sub_68862C();
		}
		if (viewport != NULL) *viewport = myviewport;
	}
	if (interactionType != NULL) *interactionType = RCT2_GLOBAL(0x9AC148, uint8_t);
	if (x != NULL) *x = RCT2_GLOBAL(0x9AC14C, int16_t);
	if (y != NULL) *y = RCT2_GLOBAL(0x9AC14E, int16_t);
	if (mapElement != NULL) *mapElement = RCT2_GLOBAL(0x9AC150, rct_map_element*);
}