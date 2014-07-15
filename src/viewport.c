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
#include "viewport.h"
#include "window.h"

#define RCT2_FIRST_VIEWPORT		(RCT2_ADDRESS(RCT2_ADDRESS_VIEWPORT_LIST, rct_viewport))
#define RCT2_LAST_VIEWPORT		(RCT2_GLOBAL(RCT2_ADDRESS_NEW_VIEWPORT_PTR, rct_viewport*) - 1)
#define RCT2_NEW_VIEWPORT		(RCT2_GLOBAL(RCT2_ADDRESS_NEW_VIEWPORT_PTR, rct_viewport*))

rct_viewport* g_viewport_list = RCT2_ADDRESS(RCT2_ADDRESS_VIEWPORT_LIST, rct_viewport);

/**
 * 
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
	RCT2_GLOBAL(0x009DE550, sint32) = -1;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_NOT_SHOWN_TICKS, sint16) = -1;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, sint16) = 0;
	RCT2_GLOBAL(0x009DEA50, sint16) = -1;
	RCT2_CALLPROC_EBPSAFE(0x006EE3C3);
	format_string((char*)0x0141FA44, STR_CANCEL, NULL);
	format_string((char*)0x0141F944, STR_OK, NULL);
}

/**
 * 
 *  rct2: 0x006EB009
 *  x:      ax
 *  y:      eax (top 16)
 *  width:  bx
 *  height: ebx (top 16)
 *  ecx:    ecx
 *  edx:    edx
 *  w:      esi
 */
void viewport_create(rct_window *w, int x, int y, int width, int height, int ecx, int edx)
{
	x &= 0xFFFF;
	y &= 0xFFFF;
	RCT2_CALLPROC_X(0x006EB009, (y << 16) | x, (height << 16) | width, ecx, edx, (int)w, 0, 0);
}

/**
 * UNTESTED
 *  rct2: 0x006EE510
 */
void viewport_update_pointers()
{
	rct_viewport *viewport;
	rct_viewport **vp = &RCT2_NEW_VIEWPORT;

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

	//ax
	int x_end = left;

	if (left < viewport->x){
		x_end = viewport->x;
	}

	//dx
	int x_start = right;

	if (right > viewport->x + viewport->width){
		x_start = viewport->x + viewport->width;
	}

	//bx
	int y_end = top;

	if (top < viewport->y){
		y_end = viewport->y;
	}

	//bp
	int y_start = bottom;

	if (bottom > viewport->y + viewport->height){
		y_start = viewport->y + viewport->height;
	}

	x_end -= viewport->x;
	x_start -= viewport->x;
	y_end -= viewport->y;
	y_start -= viewport->y;

	x_end <<= viewport->zoom;
	x_start <<= viewport->zoom;
	y_end <<= viewport->zoom;
	y_start <<= viewport->zoom;

	x_end += viewport->view_x;
	x_start += viewport->view_x;
	y_end += viewport->view_y;
	y_start += viewport->view_y;

	//cx
	int height = y_start - y_end;
	if (height > 384){
		//Paint
		viewport_paint(viewport, dpi, x_end, y_end, x_start, y_end + 384);
		y_end += 384;
	}
	//Paint
	viewport_paint(viewport, dpi, x_end, y_end, x_start, y_start);
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
	RCT2_CALLPROC_X(0x00685CBF, left, top, 0, right, (int)viewport, (int)dpi, bottom);
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
