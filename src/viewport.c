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

#include "addresses.h"
#include "config.h"
#include "gfx.h"
#include "string_ids.h"
#include "sprite.h"
#include "sprites.h"
#include "viewport.h"
#include "window.h"

#define RCT2_FIRST_VIEWPORT		(RCT2_ADDRESS(RCT2_ADDRESS_VIEWPORT_LIST, rct_viewport))
#define RCT2_LAST_VIEWPORT		(RCT2_GLOBAL(RCT2_ADDRESS_NEW_VIEWPORT_PTR, rct_viewport*) - 1)
#define RCT2_NEW_VIEWPORT		(RCT2_GLOBAL(RCT2_ADDRESS_NEW_VIEWPORT_PTR, rct_viewport*))

rct_viewport* g_viewport_list = RCT2_ADDRESS(RCT2_ADDRESS_VIEWPORT_LIST, rct_viewport);

typedef struct paint_struct paint_struct;

struct paint_struct{
	uint32 image_id;		// 0x00
	uint32 var_04;
	uint16 attached_x;		// 0x08
	uint16 attached_y;		// 0x0A
	uint8 var_0C;
	uint8 pad_0D;
	paint_struct* next_attached_ps;	//0x0E
	uint8 pad_12[2];
	uint16 x;				// 0x14
	uint16 y;				// 0x16
	uint8 pad_18[2];
	uint8 var_1A;
	uint8 pad_1B;
	paint_struct* attached_ps;	//0x1C
	paint_struct* var_20;
	paint_struct* var_24;
	uint8 sprite_type;		//0x28
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
		g1_element = &(RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[i]);
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
	RCT2_GLOBAL(0x009DE518, sint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, sint8) = INPUT_STATE_RESET;
	RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DOWN_WINDOWCLASS, rct_windowclass) = -1;
	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, sint32) = -1;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_NOT_SHOWN_TICKS, sint16) = -1;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, sint16) = 0;
	RCT2_GLOBAL(0x009DEA50, sint16) = -1;
	RCT2_CALLPROC_EBPSAFE(0x006EE3C3);
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
		y = y / 2 + start_x / 2 - z;
		break;
	case 1:
		x = -y - x;
		y = y / 2 - start_x / 2 - z;
		break;
	case 2:
		x = -y + x;
		y = -y / 2 - start_x / 2 - z;
		break;
	case 3:
		x = y + x;
		y = -y / 2 + start_x / 2 - z;
		break;
	}

	*out_x = x - viewport->view_width/2;
	*out_y = y - viewport->view_height/2;
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
		if (viewport >= RCT2_ADDRESS(RCT2_ADDRESS_NEW_VIEWPORT_PTR, rct_viewport)){
			error_string_quit(0xFF000001, -1);
		}
	}

	viewport->x = x;
	viewport->y = y;
	viewport->width = width;
	viewport->height = height;

	if (!(flags & (1 << 0))){
		zoom = 0;
	}

	viewport->view_width = width << zoom;
	viewport->view_height = height << zoom;
	viewport->zoom = zoom;
	viewport->flags = 0;

	if (RCT2_GLOBAL(0x9AAC7A, uint8) & 1){
		viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
	}
	w->viewport = viewport;

	if (flags & (1<<1)){
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
	rct_viewport **vp = RCT2_ADDRESS(RCT2_ADDRESS_NEW_VIEWPORT_PTR, rct_viewport*);

	if (*vp == NULL) *vp = g_viewport_list;
	for (viewport = g_viewport_list; viewport <= RCT2_NEW_VIEWPORT; viewport++)
		if (viewport->width != 0)
			*vp++ = viewport;

	*vp = NULL;
}

/**
 * 
 *  rct2: 0x006E7A3A
 */
void viewport_update_position(rct_window *window)
{
	RCT2_CALLPROC_X(0x006E7A3A, 0, 0, 0, 0, (int)window, 0, 0);
}

void viewport_paint(rct_viewport* viewport, rct_drawpixelinfo* dpi, int left, int top, int right, int bottom);

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
	if (left >= viewport->x + viewport->width )return;
	if (top  >= viewport->y + viewport->height )return;

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
		int x = attached_ps->attached_x + ps->x;
		int y = attached_ps->attached_y + ps->y;

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
		int x = ps->x;
		int y = ps->y;
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

		if (!(ps->var_1A & 1)){
			gfx_draw_sprite(dpi, image_id, x, y, ps->var_04);

			if (ps->var_20 != 0){
				ps = ps->var_20;
				continue;
			}
			paint_attached_ps(ps, ps->attached_ps, dpi);

			ps = previous_ps->var_24;
			previous_ps = ps;
			continue;
		}
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

int sub_0x686806(rct_sprite* sprite, int eax, int ecx, int edx){
	int ebp = (eax >> 8) & 0xFF;
	edx <<= 16;
	ebp += RCT2_GLOBAL(0x9DEA56, uint16);
	RCT2_GLOBAL(0xF1AD28, uint32) = 0;
	RCT2_GLOBAL(0xF1AD2C, uint32) = 0;
	edx = (edx >> 16) | (ebp << 16);
	ebp = RCT2_GLOBAL(0xEE7888, uint32);
	if ((uint32)ebp >= RCT2_GLOBAL(0xEE7880, uint32)) return 1;
	//686840 not finished

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

	switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION,uint32)){
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


	if (RCT2_GLOBAL(0x9DEA6F,uint8) & 1) return;
	
	dpi = RCT2_GLOBAL(0x140E9A8,rct_drawpixelinfo*);
	
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & 0x4000)return;
	
	if (dpi->zoom_level > 2) return;
	
	if (eax > 0x2000)return;
	if (ecx > 0x2000)return;
	
	//push eax, ecx
	eax = (eax&0x1FE0)<<3 | (ecx>>5);
	int sprite_idx = RCT2_ADDRESS(0xF1EF60, uint16)[eax];
	if (sprite_idx == SPRITE_INDEX_NULL) return;

	for (rct_sprite* spr = &g_sprite_list[sprite_idx]; sprite_idx != SPRITE_INDEX_NULL; sprite_idx = spr->unknown.var_02){
		spr = &g_sprite_list[sprite_idx];
		dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);

		if (dpi->y + dpi->height <= spr->unknown.var_18) continue;
		if (spr->unknown.var_1C <= dpi->y)continue;
		if (dpi->x + dpi->width <= spr->unknown.var_16)continue;
		if (spr->unknown.var_1A <= dpi->x)continue;

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
		case SPRITE_IDENTIFIER_FLOATING_TEXT:
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
			RCT2_CALLPROC_X(0x68B35F, ax, 0, cx, 0, 0, 0, 0);
			sub_0x69E8B0(ax, cx);
			cx += 0x20;
			ax -= 0x20;
			sub_0x69E8B0(ax, cx);
			ax += 0x20;
			RCT2_CALLPROC_X(0x68B35F, ax, 0, cx, 0, 0, 0, 0);
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
			RCT2_CALLPROC_X(0x68B35F, ax, 0, cx, 0, 0, 0, 0);
			sub_0x69E8B0(ax, cx);
			ax -= 0x20;
			cx -= 0x20;
			sub_0x69E8B0(ax, cx);
			cx += 0x20;
			RCT2_CALLPROC_X(0x68B35F, ax, 0, cx, 0, 0, 0, 0);
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
			RCT2_CALLPROC_X(0x68B35F, ax, 0, cx, 0, 0, 0, 0);
			sub_0x69E8B0(ax, cx);
			ax += 0x20;
			cx -= 0x20;
			sub_0x69E8B0(ax, cx);
			ax -= 0x20;
			RCT2_CALLPROC_X(0x68B35F, ax, 0, cx, 0, 0, 0, 0);
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
			RCT2_CALLPROC_X(0x68B35F, ax, 0, cx, 0, 0, 0, 0);
			sub_0x69E8B0(ax, cx);
			ax += 0x20;
			cx += 0x20;
			sub_0x69E8B0(ax, cx);
			cx -= 0x20;
			RCT2_CALLPROC_X(0x68B35F, ax, 0, cx, 0, 0, 0, 0);
			sub_0x69E8B0(ax, cx);
			ax -= 0x20;
			cx -= 0x20;
			sub_0x69E8B0(ax, cx);
			ax += 0x20;
		}
		break;
	}
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
		RCT2_CALLPROC_X(0x688217, start_x, ebx, ecx, (int)bits_pointer, esi, (int)dpi2, ebp); //Move memory
		sub_688485();
		//RCT2_CALLPROC_EBPSAFE(0x688485); //Big function call

		int weather_colour = RCT2_ADDRESS(0x98195C, uint32)[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_GLOOM, uint8)];
		if ((weather_colour != -1) && (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & 0x4000) && (RCT2_GLOBAL(0x9DEA6F, uint8) & 1)){
			dpi2 = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
			gfx_fill_rect(dpi2, dpi2->x, dpi2->y, dpi2->width + dpi2->x - 1, dpi2->height + dpi2->y - 1, weather_colour);
		}
		RCT2_CALLPROC_EBPSAFE(0x6860C3); //string related
	} 

	//RCT2_CALLPROC_X(0x00685CBF, left, top, 0, right, (int)viewport, (int)dpi, bottom);
}

/**
 * 
 *  rct2: 0x0068958D
 */
void screen_pos_to_map_pos(int *x, int *y)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	eax = *x;
	ebx = *y;
	RCT2_CALLFUNC_X(0x0068958D, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	*x = eax & 0xFFFF;
	*y = ebx & 0xFFFF;
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
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES)) {
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

	if (RCT2_GLOBAL(0x009E32B2, uint8) != 0) {
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
 *  rct2: 0x00664E8E
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

	if (RCT2_GLOBAL(0x009E32B3, uint8) != 0) {
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
